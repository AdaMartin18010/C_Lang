#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
内容同步主控脚本
协调所有监控脚本，生成综合报告，支持GitHub Actions集成
"""

import os
import sys
import json
import logging
import argparse
import subprocess
from datetime import datetime
from pathlib import Path
from typing import Dict, List, Optional, Any, Tuple
from dataclasses import dataclass, field, asdict
from enum import Enum

import yaml


logging.basicConfig(
    level=logging.INFO,
    format='%(asctime)s - %(name)s - %(levelname)s - %(message)s',
    handlers=[
        logging.StreamHandler(),
        logging.FileHandler('content_sync.log', encoding='utf-8')
    ]
)
logger = logging.getLogger('content_sync')


class SyncStatus(Enum):
    """同步状态"""
    SUCCESS = "success"
    PARTIAL = "partial"
    FAILED = "failed"
    SKIPPED = "skipped"


@dataclass
class SyncResult:
    """同步结果"""
    script_name: str
    status: SyncStatus
    exit_code: int
    output_file: Optional[str] = None
    changes_detected: bool = False
    message: str = ""
    duration_seconds: float = 0.0


class ContentSynchronizer:
    """内容同步器"""
    
    # 监控脚本配置
    MONITOR_SCRIPTS = [
        {
            'name': 'standard_monitor',
            'script': 'standard_monitor.py',
            'description': 'C标准发布监控 (ISO/IEC 9899, WG14, MISRA)',
            'enabled': True,
            'critical': True  # 失败时影响整体状态
        },
        {
            'name': 'compiler_monitor',
            'script': 'compiler_monitor.py',
            'description': '编译器版本监控 (GCC, Clang, Zig)',
            'enabled': True,
            'critical': True
        },
        {
            'name': 'security_advisory',
            'script': 'security_advisory_aggregator.py',
            'description': '安全公告聚合 (CVE, CERT, CISA)',
            'enabled': True,
            'critical': False  # 非关键，即使失败也继续
        }
    ]
    
    def __init__(
        self,
        config_file: Optional[str] = None,
        scripts_dir: Optional[str] = None,
        output_dir: str = 'sync_reports',
        proxy: Optional[str] = None
    ):
        self.config = self._load_config(config_file)
        self.scripts_dir = Path(scripts_dir) if scripts_dir else Path(__file__).parent
        self.output_dir = Path(output_dir)
        self.output_dir.mkdir(exist_ok=True)
        self.proxy = proxy
        self.results: List[SyncResult] = []
        self.sync_start_time: Optional[datetime] = None
        self.sync_end_time: Optional[datetime] = None
    
    def _load_config(self, config_file: Optional[str]) -> Dict:
        """加载配置文件"""
        default_config = {
            'scripts_dir': 'scripts',
            'output_dir': 'sync_reports',
            'run_all': True,
            'parallel': False,
            'scripts': {},
            'github': {
                'enabled': False,
                'create_issue': True,
                'create_pr': False,
                'auto_merge': False
            },
            'notification': {
                'enabled': False,
                'webhook': None,
                'email': None
            }
        }
        
        if config_file and os.path.exists(config_file):
            try:
                with open(config_file, 'r', encoding='utf-8') as f:
                    if config_file.endswith(('.yml', '.yaml')):
                        user_config = yaml.safe_load(f)
                    else:
                        user_config = json.load(f)
                default_config.update(user_config)
                logger.info(f"Loaded config from {config_file}")
            except Exception as e:
                logger.error(f"Failed to load config: {e}")
        
        return default_config
    
    def _run_script(self, script_config: Dict, timeout: int = 300) -> SyncResult:
        """运行单个监控脚本"""
        script_name = script_config['name']
        script_file = script_config['script']
        script_path = self.scripts_dir / script_file
        
        if not script_path.exists():
            return SyncResult(
                script_name=script_name,
                status=SyncStatus.FAILED,
                exit_code=1,
                message=f"Script not found: {script_path}"
            )
        
        # 构建命令
        cmd = [sys.executable, str(script_path)]
        
        # 添加通用参数
        timestamp = datetime.now().strftime('%Y%m%d_%H%M%S')
        output_file = self.output_dir / f"{script_name}_report_{timestamp}.md"
        cmd.extend(['-o', str(output_file)])
        
        # 代理设置
        if self.proxy:
            cmd.extend(['--proxy', self.proxy])
        
        # 脚本特定配置
        script_opts = self.config.get('scripts', {}).get(script_name, {})
        if script_opts.get('verbose'):
            cmd.append('-v')
        if 'config' in script_opts:
            cmd.extend(['-c', script_opts['config']])
        
        logger.info(f"Running {script_name}: {' '.join(cmd)}")
        
        start_time = datetime.now()
        
        try:
            # 设置环境变量
            env = os.environ.copy()
            if self.proxy:
                env['HTTP_PROXY'] = self.proxy
                env['HTTPS_PROXY'] = self.proxy
            
            # 运行脚本
            result = subprocess.run(
                cmd,
                capture_output=True,
                text=True,
                timeout=timeout,
                env=env,
                cwd=str(self.scripts_dir)
            )
            
            duration = (datetime.now() - start_time).total_seconds()
            
            # 分析结果
            changes_detected = result.returncode != 0  # 脚本返回非零表示有变更
            
            if result.returncode in [0, 1, 2]:  # 0=无变更, 1=有变更, 2=关键漏洞
                status = SyncStatus.SUCCESS if result.returncode == 0 else SyncStatus.PARTIAL
                
                # 读取可能的JSON输出
                try:
                    if output_file.exists():
                        message = f"Report generated: {output_file}"
                    else:
                        message = result.stdout[-500:] if result.stdout else "Completed"
                except:
                    message = result.stdout[-500:] if result.stdout else "Completed"
                
                return SyncResult(
                    script_name=script_name,
                    status=status,
                    exit_code=result.returncode,
                    output_file=str(output_file) if output_file.exists() else None,
                    changes_detected=changes_detected,
                    message=message,
                    duration_seconds=duration
                )
            else:
                return SyncResult(
                    script_name=script_name,
                    status=SyncStatus.FAILED,
                    exit_code=result.returncode,
                    message=f"Script failed: {result.stderr[-500:]}"
                )
        
        except subprocess.TimeoutExpired:
            duration = (datetime.now() - start_time).total_seconds()
            return SyncResult(
                script_name=script_name,
                status=SyncStatus.FAILED,
                exit_code=-1,
                message=f"Script timed out after {timeout}s"
            )
        
        except Exception as e:
            duration = (datetime.now() - start_time).total_seconds()
            return SyncResult(
                script_name=script_name,
                status=SyncStatus.FAILED,
                exit_code=-1,
                message=f"Exception: {str(e)}"
            )
    
    def run_all(self, script_filter: Optional[List[str]] = None) -> List[SyncResult]:
        """运行所有监控脚本"""
        self.sync_start_time = datetime.now()
        self.results = []
        
        scripts = self.MONITOR_SCRIPTS
        
        # 过滤脚本
        if script_filter:
            scripts = [s for s in scripts if s['name'] in script_filter]
        
        # 检查是否启用
        scripts = [s for s in scripts if s.get('enabled', True)]
        
        logger.info(f"Starting content synchronization for {len(scripts)} scripts")
        
        for script_config in scripts:
            result = self._run_script(script_config)
            self.results.append(result)
            
            # 关键脚本失败时记录警告
            if result.status == SyncStatus.FAILED and script_config.get('critical'):
                logger.error(f"Critical script {script_config['name']} failed!")
        
        self.sync_end_time = datetime.now()
        
        return self.results
    
    def get_summary(self) -> Dict[str, Any]:
        """获取同步摘要"""
        if not self.sync_end_time:
            self.sync_end_time = datetime.now()
        
        total_duration = 0
        if self.sync_start_time:
            total_duration = (self.sync_end_time - self.sync_start_time).total_seconds()
        
        summary = {
            'sync_time': {
                'start': self.sync_start_time.isoformat() if self.sync_start_time else None,
                'end': self.sync_end_time.isoformat() if self.sync_end_time else None,
                'duration_seconds': total_duration
            },
            'total_scripts': len(self.results),
            'success': len([r for r in self.results if r.status == SyncStatus.SUCCESS]),
            'partial': len([r for r in self.results if r.status == SyncStatus.PARTIAL]),
            'failed': len([r for r in self.results if r.status == SyncStatus.FAILED]),
            'skipped': len([r for r in self.results if r.status == SyncStatus.SKIPPED]),
            'changes_detected': len([r for r in self.results if r.changes_detected]),
            'results': []
        }
        
        for result in self.results:
            summary['results'].append({
                'script': result.script_name,
                'status': result.status.value,
                'exit_code': result.exit_code,
                'changes': result.changes_detected,
                'duration': result.duration_seconds,
                'output': result.output_file,
                'message': result.message[:200] if result.message else ""
            })
        
        return summary
    
    def generate_markdown_report(self) -> str:
        """生成Markdown综合报告"""
        summary = self.get_summary()
        
        report = []
        report.append("# 内容同步综合报告\n")
        report.append(f"**生成时间**: {datetime.now().strftime('%Y-%m-%d %H:%M:%S')}\n")
        report.append(f"**同步耗时**: {summary['sync_time']['duration_seconds']:.1f} 秒\n\n")
        
        # 状态概览
        report.append("## 📊 同步状态概览\n\n")
        report.append(f"| 指标 | 数量 |\n")
        report.append(f"|------|------|\n")
        report.append(f"| 总脚本数 | {summary['total_scripts']} |\n")
        report.append(f"| ✅ 成功 | {summary['success']} |\n")
        report.append(f"| ⚠️ 部分成功 | {summary['partial']} |\n")
        report.append(f"| ❌ 失败 | {summary['failed']} |\n")
        report.append(f"| 📝 检测到变更 | {summary['changes_detected']} |\n\n")
        
        # 详细结果
        report.append("## 🔍 详细结果\n")
        
        for result in summary['results']:
            status_emoji = {
                'success': '✅',
                'partial': '⚠️',
                'failed': '❌',
                'skipped': '⏭️'
            }.get(result['status'], '❓')
            
            report.append(f"\n### {status_emoji} {result['script']}\n")
            report.append(f"- **状态**: {result['status']}\n")
            report.append(f"- **退出码**: {result['exit_code']}\n")
            report.append(f"- **变更检测**: {'是' if result['changes'] else '否'}\n")
            report.append(f"- **耗时**: {result['duration']:.1f} 秒\n")
            
            if result['output']:
                report.append(f"- **报告文件**: [{result['output']}]({result['output']})\n")
            
            if result['message']:
                report.append(f"- **消息**: {result['message']}\n")
        
        # 变更摘要
        changes = [r for r in self.results if r.changes_detected and r.output_file]
        if changes:
            report.append("\n## 📝 检测到的变更\n")
            report.append("\n以下内容需要审查:\n")
            
            for result in changes:
                report.append(f"\n### {result.script_name}\n")
                report.append(f"📄 [{result.output_file}]({result.output_file})\n")
        
        # 行动建议
        report.append("\n## 🚀 建议行动\n")
        
        if summary['failed'] > 0:
            report.append("\n### 需要修复的问题\n")
            for result in self.results:
                if result.status == SyncStatus.FAILED:
                    report.append(f"- [ ] 修复 {result.script_name}: {result.message[:100]}\n")
        
        if summary['changes_detected'] > 0:
            report.append("\n### 内容更新\n")
            report.append("- [ ] 审查各监控脚本的报告\n")
            report.append("- [ ] 更新知识库中的相关内容\n")
            report.append("- [ ] 根据需要创建GitHub Issue或PR\n")
        
        # GitHub Actions建议
        report.append("\n### CI/CD建议\n")
        report.append("```yaml\n")
        report.append("# 将此报告附加到GitHub Actions工作流\n")
        report.append("- name: Upload Reports\n")
        report.append("  uses: actions/upload-artifact@v4\n")
        report.append("  with:\n")
        report.append("    name: sync-reports\n")
        report.append("    path: sync_reports/\n")
        report.append("```\n")
        
        return '\n'.join(report)
    
    def generate_json_report(self) -> str:
        """生成JSON格式报告"""
        summary = self.get_summary()
        return json.dumps(summary, indent=2, ensure_ascii=False)
    
    def save_report(self, output_file: Optional[str] = None) -> str:
        """保存报告"""
        if output_file is None:
            timestamp = datetime.now().strftime('%Y%m%d_%H%M%S')
            output_file = self.output_dir / f"content_sync_report_{timestamp}.md"
        
        output_file = Path(output_file)
        
        # 保存Markdown报告
        content = self.generate_markdown_report()
        with open(output_file, 'w', encoding='utf-8') as f:
            f.write(content)
        
        # 同时保存JSON版本
        json_file = output_file.with_suffix('.json')
        with open(json_file, 'w', encoding='utf-8') as f:
            f.write(self.generate_json_report())
        
        logger.info(f"Reports saved: {output_file}, {json_file}")
        return str(output_file)
    
    def generate_github_actions_workflow(self) -> str:
        """生成GitHub Actions工作流配置建议"""
        workflow = '''name: Content Sync

on:
  schedule:
    # 每周一 00:00 UTC 运行
    - cron: '0 0 * * 1'
  workflow_dispatch:
    inputs:
      scripts:
        description: '要运行的脚本（逗号分隔，留空运行全部）'
        required: false
        default: ''

jobs:
  sync:
    runs-on: ubuntu-latest
    
    steps:
    - name: Checkout repository
      uses: actions/checkout@v4
      
    - name: Set up Python
      uses: actions/setup-python@v5
      with:
        python-version: '3.11'
        
    - name: Install dependencies
      run: |
        python -m pip install --upgrade pip
        pip install requests pyyaml
        
    - name: Run content sync
      env:
        NVD_API_KEY: ${{ secrets.NVD_API_KEY }}
        GITHUB_TOKEN: ${{ secrets.GITHUB_TOKEN }}
      run: |
        cd scripts
        SCRIPTS="${{ github.event.inputs.scripts }}"
        if [ -z "$SCRIPTS" ]; then
          python content_sync.py --output-dir ../sync_reports
        else
          python content_sync.py --scripts $SCRIPTS --output-dir ../sync_reports
        fi
        
    - name: Upload reports
      uses: actions/upload-artifact@v4
      with:
        name: sync-reports
        path: sync_reports/
        
    - name: Check for changes
      id: check
      run: |
        if [ -f sync_reports/content_sync_report_*.json ]; then
          CHANGES=$(cat sync_reports/content_sync_report_*.json | grep -o '"changes_detected": [0-9]*' | grep -o '[0-9]*')
          if [ "$CHANGES" -gt 0 ]; then
            echo "changes=true" >> $GITHUB_OUTPUT
          fi
        fi
        
    - name: Create Issue
      if: steps.check.outputs.changes == 'true'
      uses: actions/github-script@v7
      with:
        script: |
          const fs = require('fs');
          const path = require('path');
          
          // 读取最新的同步报告
          const reportsDir = 'sync_reports';
          const files = fs.readdirSync(reportsDir);
          const latestReport = files
            .filter(f => f.startsWith('content_sync_report_') && f.endsWith('.md'))
            .sort()
            .pop();
          
          if (latestReport) {
            const content = fs.readFileSync(path.join(reportsDir, latestReport), 'utf8');
            
            github.rest.issues.create({
              owner: context.repo.owner,
              repo: context.repo.repo,
              title: `📋 内容同步报告 - ${new Date().toISOString().split('T')[0]}`,
              body: content.substring(0, 65536),  // GitHub限制
              labels: ['automation', 'content-sync']
            });
          }
'''
        return workflow
    
    def generate_pr_template(self) -> str:
        """生成PR模板"""
        template = '''## 📋 内容同步更新

### 变更摘要
<!-- 自动生成的摘要将插入此处 -->

### 需要审查的内容
- [ ] 标准更新 (ISO/IEC 9899, WG14, MISRA)
- [ ] 编译器版本更新 (GCC, Clang, Zig)
- [ ] 安全公告 (CVE, CERT, CISA)

### 自动检查结果
- [ ] 所有监控脚本运行成功
- [ ] 生成的报告已审查
- [ ] 知识库内容已相应更新

### 相关报告
<!-- 报告链接将自动插入 -->

### 附加信息
由 `content_sync.py` 自动生成
'''
        return template


def create_sample_config(output_path: str = 'content_sync_config.yml'):
    """创建示例配置文件"""
    config = {
        'scripts_dir': 'scripts',
        'output_dir': 'sync_reports',
        'run_all': True,
        'parallel': False,
        'scripts': {
            'standard_monitor': {
                'enabled': True,
                'verbose': False
            },
            'compiler_monitor': {
                'enabled': True,
                'verbose': False
            },
            'security_advisory': {
                'enabled': True,
                'verbose': False,
                'config': 'security_advisory_config.yml'
            }
        },
        'github': {
            'enabled': False,
            'create_issue': True,
            'create_pr': False,
            'auto_merge': False
        },
        'notification': {
            'enabled': False,
            'webhook': None,
            'email': None
        }
    }
    
    with open(output_path, 'w', encoding='utf-8') as f:
        yaml.dump(config, f, default_flow_style=False, allow_unicode=True)
    
    print(f"Sample config created: {output_path}")


def main():
    parser = argparse.ArgumentParser(
        description='Content synchronization controller for C knowledge base',
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
Examples:
  %(prog)s                          # 运行所有监控脚本
  %(prog)s --scripts standard_monitor,compiler_monitor  # 运行指定脚本
  %(prog)s -o reports/              # 指定输出目录
  %(prog)s --proxy http://proxy:8080  # 使用代理
  %(prog)s --generate-workflow      # 生成GitHub Actions工作流
  %(prog)s --init-config            # 创建示例配置文件
        """
    )
    
    parser.add_argument('-c', '--config', help='配置文件路径')
    parser.add_argument('-o', '--output-dir', default='sync_reports', help='输出目录')
    parser.add_argument('--scripts-dir', help='脚本目录')
    parser.add_argument('--scripts', help='要运行的脚本，逗号分隔（默认全部）')
    parser.add_argument('--proxy', help='代理服务器地址')
    parser.add_argument('--generate-workflow', action='store_true', 
                       help='生成GitHub Actions工作流到stdout')
    parser.add_argument('--generate-pr-template', action='store_true',
                       help='生成PR模板到stdout')
    parser.add_argument('--init-config', action='store_true', 
                       help='创建示例配置文件')
    parser.add_argument('-v', '--verbose', action='store_true', help='显示详细日志')
    
    args = parser.parse_args()
    
    if args.init_config:
        create_sample_config()
        return 0
    
    if args.generate_workflow:
        synchronizer = ContentSynchronizer()
        print(synchronizer.generate_github_actions_workflow())
        return 0
    
    if args.generate_pr_template:
        synchronizer = ContentSynchronizer()
        print(synchronizer.generate_pr_template())
        return 0
    
    if args.verbose:
        logging.getLogger().setLevel(logging.DEBUG)
    
    try:
        synchronizer = ContentSynchronizer(
            config_file=args.config,
            scripts_dir=args.scripts_dir,
            output_dir=args.output_dir,
            proxy=args.proxy
        )
        
        # 解析要运行的脚本
        script_filter = None
        if args.scripts:
            script_filter = [s.strip() for s in args.scripts.split(',')]
        
        # 运行同步
        results = synchronizer.run_all(script_filter)
        
        # 生成报告
        report_file = synchronizer.save_report()
        
        # 输出摘要
        summary = synchronizer.get_summary()
        
        print(f"\n{'='*60}")
        print("内容同步完成")
        print(f"{'='*60}")
        print(f"总脚本数: {summary['total_scripts']}")
        print(f"✅ 成功: {summary['success']}")
        print(f"⚠️ 部分成功: {summary['partial']}")
        print(f"❌ 失败: {summary['failed']}")
        print(f"📝 检测到变更: {summary['changes_detected']}")
        print(f"耗时: {summary['sync_time']['duration_seconds']:.1f} 秒")
        print(f"\n报告文件: {report_file}")
        
        # 返回适当的退出码
        if summary['failed'] > 0:
            return 1
        if summary['changes_detected'] > 0:
            return 2  # 有变更但无失败
        return 0
        
    except KeyboardInterrupt:
        print("\n同步已取消")
        return 130
    except Exception as e:
        logger.error(f"Unexpected error: {e}", exc_info=True)
        print(f"\n错误: {e}")
        return 1


if __name__ == '__main__':
    sys.exit(main())

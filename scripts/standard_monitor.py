#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
C标准发布监控脚本
监控ISO/IEC 9899 C标准、WG14文档和MISRA规范的更新
"""

import os
import sys
import json
import hashlib
import logging
import argparse
from datetime import datetime, timedelta
from pathlib import Path
from typing import Dict, List, Optional, Set, Tuple
from dataclasses import dataclass, asdict
from urllib.parse import urljoin, urlparse

import requests
import yaml


# 配置日志
logging.basicConfig(
    level=logging.INFO,
    format='%(asctime)s - %(name)s - %(levelname)s - %(message)s',
    handlers=[
        logging.StreamHandler(),
        logging.FileHandler('standard_monitor.log', encoding='utf-8')
    ]
)
logger = logging.getLogger('standard_monitor')


@dataclass
class StandardSource:
    """标准来源配置"""
    name: str
    url: str
    type: str  # 'iso', 'wg14', 'misra', 'other'
    check_interval_days: int = 7
    enabled: bool = True
    headers: Dict = None
    
    def __post_init__(self):
        if self.headers is None:
            self.headers = {}


@dataclass
class ChangeRecord:
    """变更记录"""
    source: str
    title: str
    url: str
    detected_at: str
    change_type: str  # 'new', 'updated', 'removed'
    content_hash: str
    summary: str = ""
    severity: str = "info"  # 'critical', 'high', 'medium', 'low', 'info'


class ContentHasher:
    """内容哈希管理器"""
    
    def __init__(self, state_file: str = '.standard_state.json'):
        self.state_file = Path(state_file)
        self.hashes: Dict[str, str] = {}
        self._load_state()
    
    def _load_state(self):
        """加载之前的哈希状态"""
        if self.state_file.exists():
            try:
                with open(self.state_file, 'r', encoding='utf-8') as f:
                    self.hashes = json.load(f)
                logger.info(f"Loaded {len(self.hashes)} hash entries from state file")
            except Exception as e:
                logger.warning(f"Failed to load state file: {e}")
                self.hashes = {}
    
    def save_state(self):
        """保存哈希状态"""
        try:
            with open(self.state_file, 'w', encoding='utf-8') as f:
                json.dump(self.hashes, f, indent=2, ensure_ascii=False)
            logger.info(f"Saved {len(self.hashes)} hash entries to state file")
        except Exception as e:
            logger.error(f"Failed to save state file: {e}")
    
    def compute_hash(self, content: str) -> str:
        """计算内容哈希"""
        return hashlib.sha256(content.encode('utf-8')).hexdigest()[:16]
    
    def has_changed(self, key: str, content: str) -> Tuple[bool, str]:
        """检查内容是否变更"""
        current_hash = self.compute_hash(content)
        previous_hash = self.hashes.get(key)
        
        if previous_hash is None:
            return True, current_hash  # 新内容
        
        return previous_hash != current_hash, current_hash
    
    def update_hash(self, key: str, content_hash: str):
        """更新哈希值"""
        self.hashes[key] = content_hash


class StandardMonitor:
    """C标准监控器"""
    
    # 默认监控源配置
    DEFAULT_SOURCES = [
        StandardSource(
            name="ISO/IEC 9899 C17",
            url="https://www.iso.org/standard/74528.html",
            type="iso",
            check_interval_days=30
        ),
        StandardSource(
            name="ISO/IEC 9899 C23 (Upcoming)",
            url="https://www.iso.org/standard/83616.html",
            type="iso",
            check_interval_days=14
        ),
        StandardSource(
            name="WG14 Open Standards",
            url="https://www.open-std.org/jtc1/sc22/wg14/",
            type="wg14",
            check_interval_days=7
        ),
        StandardSource(
            name="WG14 Documents",
            url="https://www.open-std.org/jtc1/sc22/wg14/www/docs/",
            type="wg14",
            check_interval_days=7
        ),
        StandardSource(
            name="MISRA C Guidelines",
            url="https://misra.org.uk/misra-c/",
            type="misra",
            check_interval_days=30
        ),
        StandardSource(
            name="CERT C Coding Standard",
            url="https://wiki.sei.cmu.edu/confluence/display/c/SEI+CERT+C+Coding+Standard",
            type="other",
            check_interval_days=14
        ),
    ]
    
    def __init__(
        self,
        config_file: Optional[str] = None,
        proxy: Optional[str] = None,
        timeout: int = 30
    ):
        self.config = self._load_config(config_file)
        self.sources = self._init_sources()
        self.hasher = ContentHasher(
            self.config.get('state_file', '.standard_state.json')
        )
        self.timeout = timeout
        self.session = self._create_session(proxy)
        self.changes: List[ChangeRecord] = []
    
    def _load_config(self, config_file: Optional[str]) -> Dict:
        """加载配置文件"""
        default_config = {
            'state_file': '.standard_state.json',
            'output_format': 'markdown',
            'notification': {
                'enabled': False,
                'webhook_url': None,
                'email': None
            },
            'sources': []
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
                logger.error(f"Failed to load config: {e}, using defaults")
        
        return default_config
    
    def _init_sources(self) -> List[StandardSource]:
        """初始化监控源"""
        user_sources = self.config.get('sources', [])
        
        if user_sources:
            return [StandardSource(**s) for s in user_sources]
        
        return self.DEFAULT_SOURCES
    
    def _create_session(self, proxy: Optional[str]) -> requests.Session:
        """创建HTTP会话"""
        session = requests.Session()
        
        # 设置默认请求头
        session.headers.update({
            'User-Agent': 'Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 '
                         '(KHTML, like Gecko) Chrome/120.0.0.0 Safari/537.36',
            'Accept': 'text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8',
            'Accept-Language': 'en-US,en;q=0.5',
        })
        
        # 配置代理
        if proxy:
            session.proxies = {
                'http': proxy,
                'https': proxy
            }
            logger.info(f"Using proxy: {proxy}")
        elif 'HTTP_PROXY' in os.environ:
            session.proxies = {
                'http': os.environ['HTTP_PROXY'],
                'https': os.environ.get('HTTPS_PROXY', os.environ['HTTP_PROXY'])
            }
        
        return session
    
    def fetch_content(self, source: StandardSource) -> Optional[str]:
        """获取远程内容"""
        try:
            logger.info(f"Fetching {source.name} from {source.url}")
            response = self.session.get(
                source.url,
                headers=source.headers,
                timeout=self.timeout
            )
            response.raise_for_status()
            return response.text
        except requests.exceptions.RequestException as e:
            logger.error(f"Failed to fetch {source.name}: {e}")
            return None
    
    def extract_content_summary(self, html_content: str, source_type: str) -> str:
        """从HTML内容中提取摘要"""
        # 简化的内容提取，实际使用时可以集成BeautifulSoup等库
        summary = ""
        
        # 提取关键信息（基于简单文本分析）
        lines = html_content.split('\n')
        
        if source_type == 'iso':
            # 寻找ISO标准状态信息
            for line in lines:
                if any(kw in line.lower() for kw in ['status', 'stage', 'publication', 'edition']):
                    if len(line.strip()) < 200:
                        summary += line.strip() + " "
        
        elif source_type == 'wg14':
            # 寻找WG14文档列表
            doc_count = html_content.count('.pdf') + html_content.count('.doc')
            if doc_count > 0:
                summary = f"Found {doc_count} documents"
        
        elif source_type == 'misra':
            # MISRA相关更新
            for line in lines:
                if any(kw in line.lower() for kw in ['update', 'revision', 'version', 'release']):
                    if len(line.strip()) < 200:
                        summary += line.strip() + " "
        
        return summary.strip() or "Content retrieved successfully"
    
    def check_source(self, source: StandardSource) -> Optional[ChangeRecord]:
        """检查单个监控源"""
        if not source.enabled:
            logger.debug(f"Skipping disabled source: {source.name}")
            return None
        
        content = self.fetch_content(source)
        if content is None:
            return None
        
        # 检查内容是否变更
        has_changed, new_hash = self.hasher.has_changed(source.url, content)
        
        if has_changed:
            logger.info(f"Change detected in {source.name}")
            
            # 确定变更类型
            if source.url not in self.hasher.hashes:
                change_type = 'new'
            else:
                change_type = 'updated'
            
            # 更新哈希
            self.hasher.update_hash(source.url, new_hash)
            
            # 创建变更记录
            record = ChangeRecord(
                source=source.name,
                title=f"{source.name} - {change_type.capitalize()}",
                url=source.url,
                detected_at=datetime.now().isoformat(),
                change_type=change_type,
                content_hash=new_hash,
                summary=self.extract_content_summary(content, source.type),
                severity='medium' if source.type == 'iso' else 'low'
            )
            
            return record
        
        logger.debug(f"No changes in {source.name}")
        return None
    
    def run(self) -> List[ChangeRecord]:
        """运行监控检查"""
        logger.info("Starting standard monitoring...")
        self.changes = []
        
        for source in self.sources:
            try:
                change = self.check_source(source)
                if change:
                    self.changes.append(change)
            except Exception as e:
                logger.error(f"Error checking {source.name}: {e}", exc_info=True)
        
        # 保存状态
        self.hasher.save_state()
        
        logger.info(f"Monitoring complete. Found {len(self.changes)} changes")
        return self.changes
    
    def generate_markdown_report(self) -> str:
        """生成Markdown格式报告"""
        report = []
        report.append("# C标准监控报告\n")
        report.append(f"**生成时间**: {datetime.now().strftime('%Y-%m-%d %H:%M:%S')}\n")
        report.append(f"**监控源数量**: {len(self.sources)}\n")
        report.append(f"**检测到的变更**: {len(self.changes)}\n")
        
        if not self.changes:
            report.append("\n## 摘要\n")
            report.append("✅ 所有监控源无变更\n")
            return '\n'.join(report)
        
        # 按严重程度分组
        severity_order = ['critical', 'high', 'medium', 'low', 'info']
        changes_by_severity = {s: [] for s in severity_order}
        
        for change in self.changes:
            changes_by_severity.get(change.severity, changes_by_severity['info']).append(change)
        
        # 生成详细报告
        report.append("\n## 变更详情\n")
        
        for severity in severity_order:
            changes = changes_by_severity[severity]
            if not changes:
                continue
            
            emoji = {
                'critical': '🔴', 'high': '🟠', 'medium': '🟡',
                'low': '🟢', 'info': '🔵'
            }.get(severity, '⚪')
            
            report.append(f"\n### {emoji} {severity.upper()} ({len(changes)})\n")
            
            for change in changes:
                report.append(f"\n#### {change.source}\n")
                report.append(f"- **类型**: {change.change_type}\n")
                report.append(f"- **URL**: [{change.url}]({change.url})\n")
                report.append(f"- **检测时间**: {change.detected_at}\n")
                report.append(f"- **摘要**: {change.summary}\n")
        
        # 添加建议行动
        report.append("\n## 建议行动\n")
        
        iso_changes = [c for c in self.changes if c.change_type == 'iso']
        if iso_changes:
            report.append("\n### ISO标准更新\n")
            report.append("- [ ] 审查新标准文档\n")
            report.append("- [ ] 更新知识库中的标准引用\n")
            report.append("- [ ] 通知团队成员\n")
        
        wg14_changes = [c for c in self.changes if c.change_type == 'wg14']
        if wg14_changes:
            report.append("\n### WG14文档更新\n")
            report.append("- [ ] 查看新提案和缺陷报告\n")
            report.append("- [ ] 评估对编译器的影响\n")
        
        misra_changes = [c for c in self.changes if c.change_type == 'misra']
        if misra_changes:
            report.append("\n### MISRA规范更新\n")
            report.append("- [ ] 审查编码规范变更\n")
            report.append("- [ ] 更新代码检查工具配置\n")
        
        return '\n'.join(report)
    
    def generate_json_report(self) -> str:
        """生成JSON格式报告"""
        report = {
            'generated_at': datetime.now().isoformat(),
            'total_sources': len(self.sources),
            'changes_count': len(self.changes),
            'sources': [asdict(s) for s in self.sources],
            'changes': [asdict(c) for c in self.changes]
        }
        return json.dumps(report, indent=2, ensure_ascii=False)
    
    def save_report(self, output_file: Optional[str] = None) -> str:
        """保存报告到文件"""
        if output_file is None:
            timestamp = datetime.now().strftime('%Y%m%d_%H%M%S')
            output_file = f"standard_monitor_report_{timestamp}.md"
        
        output_format = self.config.get('output_format', 'markdown')
        
        if output_format == 'json':
            content = self.generate_json_report()
            if not output_file.endswith('.json'):
                output_file = output_file.replace('.md', '.json')
        else:
            content = self.generate_markdown_report()
        
        try:
            with open(output_file, 'w', encoding='utf-8') as f:
                f.write(content)
            logger.info(f"Report saved to {output_file}")
            return output_file
        except Exception as e:
            logger.error(f"Failed to save report: {e}")
            raise


def create_sample_config(output_path: str = 'standard_monitor_config.yml'):
    """创建示例配置文件"""
    config = {
        'state_file': '.standard_state.json',
        'output_format': 'markdown',
        'notification': {
            'enabled': False,
            'webhook_url': None,
            'email': None
        },
        'sources': [
            {
                'name': 'ISO/IEC 9899 C17',
                'url': 'https://www.iso.org/standard/74528.html',
                'type': 'iso',
                'check_interval_days': 30,
                'enabled': True,
                'headers': {}
            },
            {
                'name': 'WG14 Open Standards',
                'url': 'https://www.open-std.org/jtc1/sc22/wg14/',
                'type': 'wg14',
                'check_interval_days': 7,
                'enabled': True
            },
            {
                'name': 'MISRA C Guidelines',
                'url': 'https://misra.org.uk/misra-c/',
                'type': 'misra',
                'check_interval_days': 30,
                'enabled': True
            }
        ]
    }
    
    with open(output_path, 'w', encoding='utf-8') as f:
        yaml.dump(config, f, default_flow_style=False, allow_unicode=True)
    
    print(f"Sample config created: {output_path}")


def main():
    parser = argparse.ArgumentParser(
        description='Monitor C language standards for updates',
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
Examples:
  %(prog)s                          # 运行监控（使用默认配置）
  %(prog)s -c config.yml            # 使用自定义配置
  %(prog)s -o report.md             # 指定输出文件
  %(prog)s --proxy http://proxy:8080  # 使用代理
  %(prog)s --init-config            # 创建示例配置文件
        """
    )
    
    parser.add_argument('-c', '--config', help='配置文件路径')
    parser.add_argument('-o', '--output', help='输出文件路径')
    parser.add_argument('--proxy', help='代理服务器地址 (e.g., http://proxy:8080)')
    parser.add_argument('--format', choices=['markdown', 'json'], 
                       help='输出格式 (覆盖配置文件设置)')
    parser.add_argument('--timeout', type=int, default=30, help='请求超时时间（秒）')
    parser.add_argument('--init-config', action='store_true', 
                       help='创建示例配置文件')
    parser.add_argument('-v', '--verbose', action='store_true', 
                       help='显示详细日志')
    
    args = parser.parse_args()
    
    if args.init_config:
        create_sample_config()
        return 0
    
    if args.verbose:
        logging.getLogger().setLevel(logging.DEBUG)
    
    try:
        monitor = StandardMonitor(
            config_file=args.config,
            proxy=args.proxy,
            timeout=args.timeout
        )
        
        if args.format:
            monitor.config['output_format'] = args.format
        
        # 运行监控
        changes = monitor.run()
        
        # 生成报告
        report_file = monitor.save_report(args.output)
        
        # 输出摘要到控制台
        print(f"\n{'='*60}")
        print("标准监控完成")
        print(f"{'='*60}")
        print(f"监控源: {len(monitor.sources)}")
        print(f"变更数: {len(changes)}")
        print(f"报告文件: {report_file}")
        
        if changes:
            print("\n检测到的变更:")
            for change in changes:
                print(f"  - [{change.severity.upper()}] {change.source}")
            return 1  # 有变更时返回非零退出码，便于CI/CD检测
        
        return 0
        
    except KeyboardInterrupt:
        print("\n监控已取消")
        return 130
    except Exception as e:
        logger.error(f"Unexpected error: {e}", exc_info=True)
        print(f"\n错误: {e}")
        return 1


if __name__ == '__main__':
    sys.exit(main())

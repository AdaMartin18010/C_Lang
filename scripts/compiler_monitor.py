#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
编译器版本监控脚本
监控GCC、Clang、Zig等编译器的版本发布和新特性
"""

import os
import sys
import re
import json
import logging
import argparse
from datetime import datetime
from pathlib import Path
from typing import Dict, List, Optional, Set, Tuple, Any
from dataclasses import dataclass, field, asdict
from functools import lru_cache

import requests
import yaml


logging.basicConfig(
    level=logging.INFO,
    format='%(asctime)s - %(name)s - %(levelname)s - %(message)s',
    handlers=[
        logging.StreamHandler(),
        logging.FileHandler('compiler_monitor.log', encoding='utf-8')
    ]
)
logger = logging.getLogger('compiler_monitor')


@dataclass
class CompilerVersion:
    """编译器版本信息"""
    name: str
    version: str
    release_date: Optional[str] = None
    download_url: Optional[str] = None
    release_notes_url: Optional[str] = None
    new_features: List[str] = field(default_factory=list)
    bug_fixes: List[str] = field(default_factory=list)
    breaking_changes: List[str] = field(default_factory=list)
    
    def to_dict(self) -> Dict:
        return asdict(self)


@dataclass
class CompilerSupportMatrix:
    """编译器特性支持矩阵"""
    compiler: str
    version: str
    c89: bool = False
    c99: bool = False
    c11: bool = False
    c17: bool = False
    c23: bool = False
    attributes: bool = False
    static_assert: bool = False
    alignas: bool = False
    noreturn: bool = False
    atomic: bool = False
    threads: bool = False
    generic_selection: bool = False
    
    def to_dict(self) -> Dict:
        return asdict(self)


class CompilerMonitor:
    """编译器监控器"""
    
    # API端点配置
    API_ENDPOINTS = {
        'gcc': {
            'releases': 'https://ftp.gnu.org/gnu/gcc/',
            'changelog': 'https://gcc.gnu.org/git/?p=gcc.git;a=blob_plain;f=gcc/ChangeLog;hb=refs/heads/master',
            'announcements': 'https://gcc.gnu.org/ml/gcc-announce/'
        },
        'clang': {
            'releases': 'https://api.github.com/repos/llvm/llvm-project/releases',
            'tags': 'https://api.github.com/repos/llvm/llvm-project/tags'
        },
        'zig': {
            'releases': 'https://api.github.com/repos/ziglang/zig/releases',
            'tags': 'https://api.github.com/repos/ziglang/zig/tags'
        },
        'msvc': {
            'releases': 'https://docs.microsoft.com/en-us/cpp/overview/visual-cpp-language-standard-compliance'
        },
        'intel': {
            'releases': 'https://www.intel.com/content/www/us/en/developer/articles/tool/oneapi-standalone-components.html'
        }
    }
    
    # 本地支持矩阵文件路径
    SUPPORT_MATRIX_FILE = 'compiler_support_matrix.json'
    
    def __init__(
        self,
        config_file: Optional[str] = None,
        proxy: Optional[str] = None,
        timeout: int = 30,
        github_token: Optional[str] = None
    ):
        self.config = self._load_config(config_file)
        self.timeout = timeout
        self.github_token = github_token or os.environ.get('GITHUB_TOKEN')
        self.session = self._create_session(proxy)
        self.new_versions: Dict[str, List[CompilerVersion]] = {}
        self.support_matrix: Dict[str, List[CompilerSupportMatrix]] = {}
        self._load_support_matrix()
    
    def _load_config(self, config_file: Optional[str]) -> Dict:
        """加载配置文件"""
        default_config = {
            'compilers': ['gcc', 'clang', 'zig', 'msvc'],
            'check_prerelease': False,
            'output_format': 'markdown',
            'support_matrix_file': 'compiler_support_matrix.json',
            'min_version': {
                'gcc': '9.0.0',
                'clang': '9.0.0',
                'zig': '0.9.0'
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
    
    def _create_session(self, proxy: Optional[str]) -> requests.Session:
        """创建HTTP会话"""
        session = requests.Session()
        
        session.headers.update({
            'User-Agent': 'Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36',
            'Accept': 'application/vnd.github.v3+json'
        })
        
        # GitHub认证
        if self.github_token:
            session.headers['Authorization'] = f'token {self.github_token}'
        
        if proxy:
            session.proxies = {'http': proxy, 'https': proxy}
        elif 'HTTP_PROXY' in os.environ:
            session.proxies = {
                'http': os.environ['HTTP_PROXY'],
                'https': os.environ.get('HTTPS_PROXY', os.environ['HTTP_PROXY'])
            }
        
        return session
    
    def _load_support_matrix(self):
        """加载本地支持矩阵"""
        matrix_file = self.config.get('support_matrix_file', self.SUPPORT_MATRIX_FILE)
        
        if os.path.exists(matrix_file):
            try:
                with open(matrix_file, 'r', encoding='utf-8') as f:
                    data = json.load(f)
                    for compiler, versions in data.items():
                        self.support_matrix[compiler] = [
                            CompilerSupportMatrix(**v) for v in versions
                        ]
                logger.info(f"Loaded support matrix from {matrix_file}")
            except Exception as e:
                logger.error(f"Failed to load support matrix: {e}")
        else:
            # 初始化默认支持矩阵
            self._init_default_support_matrix()
    
    def _init_default_support_matrix(self):
        """初始化默认支持矩阵数据"""
        self.support_matrix = {
            'gcc': [
                CompilerSupportMatrix('gcc', '9.0', c89=True, c99=True, c11=True, c17=True),
                CompilerSupportMatrix('gcc', '10.0', c89=True, c99=True, c11=True, c17=True, attributes=True),
                CompilerSupportMatrix('gcc', '11.0', c89=True, c99=True, c11=True, c17=True, c23=True),
                CompilerSupportMatrix('gcc', '12.0', c89=True, c99=True, c11=True, c17=True, c23=True),
                CompilerSupportMatrix('gcc', '13.0', c89=True, c99=True, c11=True, c17=True, c23=True),
            ],
            'clang': [
                CompilerSupportMatrix('clang', '9.0', c89=True, c99=True, c11=True, c17=True),
                CompilerSupportMatrix('clang', '10.0', c89=True, c99=True, c11=True, c17=True),
                CompilerSupportMatrix('clang', '11.0', c89=True, c99=True, c11=True, c17=True, c23=True),
                CompilerSupportMatrix('clang', '12.0', c89=True, c99=True, c11=True, c17=True, c23=True),
                CompilerSupportMatrix('clang', '13.0', c89=True, c99=True, c11=True, c17=True, c23=True),
                CompilerSupportMatrix('clang', '14.0', c89=True, c99=True, c11=True, c17=True, c23=True),
            ],
            'zig': [
                CompilerSupportMatrix('zig', '0.9.0', c89=True, c99=True, c11=True, c17=True),
                CompilerSupportMatrix('zig', '0.10.0', c89=True, c99=True, c11=True, c17=True, c23=True),
                CompilerSupportMatrix('zig', '0.11.0', c89=True, c99=True, c11=True, c17=True, c23=True),
            ],
            'msvc': [
                CompilerSupportMatrix('msvc', '2019', c89=True, c99=True, c11=True, c17=True),
                CompilerSupportMatrix('msvc', '2022', c89=True, c99=True, c11=True, c17=True, c23=True),
            ]
        }
    
    def save_support_matrix(self):
        """保存支持矩阵"""
        matrix_file = self.config.get('support_matrix_file', self.SUPPORT_MATRIX_FILE)
        
        data = {}
        for compiler, versions in self.support_matrix.items():
            data[compiler] = [v.to_dict() for v in versions]
        
        try:
            with open(matrix_file, 'w', encoding='utf-8') as f:
                json.dump(data, f, indent=2, ensure_ascii=False)
            logger.info(f"Support matrix saved to {matrix_file}")
        except Exception as e:
            logger.error(f"Failed to save support matrix: {e}")
    
    def _parse_version(self, version_str: str) -> Tuple[int, ...]:
        """解析版本字符串为元组"""
        # 移除非版本字符
        version_str = re.sub(r'[^\d.]', '', version_str.split('-')[0])
        try:
            return tuple(int(x) for x in version_str.split('.'))
        except ValueError:
            return (0,)
    
    def _is_newer_version(self, new_ver: str, old_ver: str) -> bool:
        """比较版本号"""
        return self._parse_version(new_ver) > self._parse_version(old_ver)
    
    def fetch_gcc_releases(self) -> List[CompilerVersion]:
        """获取GCC发布版本"""
        versions = []
        
        try:
            logger.info("Fetching GCC releases...")
            response = self.session.get(
                self.API_ENDPOINTS['gcc']['releases'],
                timeout=self.timeout
            )
            response.raise_for_status()
            
            # 解析FTP目录列表
            content = response.text
            version_pattern = re.compile(r'href="(gcc-([\d.]+))\/"')
            matches = version_pattern.findall(content)
            
            min_version = self.config.get('min_version', {}).get('gcc', '9.0.0')
            
            for _, version_str in matches:
                if self._is_newer_version(version_str, min_version):
                    versions.append(CompilerVersion(
                        name='GCC',
                        version=version_str,
                        release_notes_url=f'https://gcc.gnu.org/gcc-{version_str}/changes.html',
                        download_url=f'https://ftp.gnu.org/gnu/gcc/gcc-{version_str}/'
                    ))
            
            logger.info(f"Found {len(versions)} GCC versions")
            
        except Exception as e:
            logger.error(f"Failed to fetch GCC releases: {e}")
        
        return versions
    
    def fetch_clang_releases(self) -> List[CompilerVersion]:
        """获取Clang发布版本"""
        versions = []
        
        try:
            logger.info("Fetching Clang releases...")
            response = self.session.get(
                self.API_ENDPOINTS['clang']['releases'],
                params={'per_page': 50},
                timeout=self.timeout
            )
            response.raise_for_status()
            
            releases = response.json()
            min_version = self.config.get('min_version', {}).get('clang', '9.0.0')
            
            for release in releases:
                tag_name = release.get('tag_name', '')
                # 只关注LLVM主版本
                if not tag_name.startswith('llvmorg-'):
                    continue
                
                version_str = tag_name.replace('llvmorg-', '').split('-')[0]
                
                if self._is_newer_version(version_str, min_version):
                    cv = CompilerVersion(
                        name='Clang/LLVM',
                        version=version_str,
                        release_date=release.get('published_at', '').split('T')[0],
                        download_url=release.get('html_url'),
                        release_notes_url=release.get('html_url')
                    )
                    
                    # 提取新特性
                    body = release.get('body', '')
                    cv.new_features = self._extract_features_from_notes(body, 'feature')
                    cv.bug_fixes = self._extract_features_from_notes(body, 'fix')
                    
                    versions.append(cv)
            
            logger.info(f"Found {len(versions)} Clang versions")
            
        except Exception as e:
            logger.error(f"Failed to fetch Clang releases: {e}")
        
        return versions
    
    def fetch_zig_releases(self) -> List[CompilerVersion]:
        """获取Zig发布版本"""
        versions = []
        
        try:
            logger.info("Fetching Zig releases...")
            response = self.session.get(
                self.API_ENDPOINTS['zig']['releases'],
                params={'per_page': 30},
                timeout=self.timeout
            )
            response.raise_for_status()
            
            releases = response.json()
            min_version = self.config.get('min_version', {}).get('zig', '0.9.0')
            
            for release in releases:
                tag_name = release.get('tag_name', '')
                version_str = tag_name.lstrip('v')
                
                if self._is_newer_version(version_str, min_version):
                    cv = CompilerVersion(
                        name='Zig',
                        version=version_str,
                        release_date=release.get('published_at', '').split('T')[0],
                        download_url=release.get('html_url'),
                        release_notes_url=release.get('html_url')
                    )
                    
                    body = release.get('body', '')
                    cv.new_features = self._extract_features_from_notes(body, 'feature')
                    
                    versions.append(cv)
            
            logger.info(f"Found {len(versions)} Zig versions")
            
        except Exception as e:
            logger.error(f"Failed to fetch Zig releases: {e}")
        
        return versions
    
    def _extract_features_from_notes(self, notes: str, feature_type: str) -> List[str]:
        """从发布说明中提取特性"""
        features = []
        
        if not notes:
            return features
        
        lines = notes.split('\n')
        
        if feature_type == 'feature':
            # 查找新特性（以+、*、-或数字开头的行）
            for line in lines:
                line = line.strip()
                if line.startswith(('+ ', '* ', '- ')) and len(line) > 3:
                    if any(kw in line.lower() for kw in ['new', 'add', 'support', 'implement']):
                        features.append(line.lstrip('+ *-'))
                elif re.match(r'^\d+\.\s', line):
                    features.append(line)
        
        elif feature_type == 'fix':
            # 查找修复
            for line in lines:
                line = line.strip()
                if any(kw in line.lower() for kw in ['fix', 'bug', 'issue', 'resolve']):
                    if len(line) < 200:  # 限制长度
                        features.append(line)
        
        return features[:20]  # 限制数量
    
    def check_new_versions(self) -> Dict[str, List[CompilerVersion]]:
        """检查所有编译器的新版本"""
        compilers = self.config.get('compilers', ['gcc', 'clang', 'zig'])
        
        self.new_versions = {}
        
        for compiler in compilers:
            try:
                if compiler == 'gcc':
                    self.new_versions['gcc'] = self.fetch_gcc_releases()
                elif compiler == 'clang':
                    self.new_versions['clang'] = self.fetch_clang_releases()
                elif compiler == 'zig':
                    self.new_versions['zig'] = self.fetch_zig_releases()
            except Exception as e:
                logger.error(f"Error checking {compiler}: {e}")
        
        return self.new_versions
    
    def compare_with_local_matrix(self) -> Dict[str, Any]:
        """与本地支持矩阵比较"""
        comparison = {
            'updates_needed': [],
            'new_standards_supported': [],
            'recommendations': []
        }
        
        for compiler, versions in self.new_versions.items():
            if not versions:
                continue
            
            # 获取最新版本
            latest = max(versions, key=lambda v: self._parse_version(v.version))
            
            # 检查是否已在支持矩阵中
            existing_versions = self.support_matrix.get(compiler, [])
            existing = [v for v in existing_versions 
                       if v.version == latest.version]
            
            if not existing:
                comparison['updates_needed'].append({
                    'compiler': compiler,
                    'new_version': latest.version,
                    'action': 'add_to_matrix'
                })
                
                # 生成建议
                recommendation = self._generate_recommendation(compiler, latest)
                if recommendation:
                    comparison['recommendations'].append(recommendation)
        
        return comparison
    
    def _generate_recommendation(self, compiler: str, version: CompilerVersion) -> Optional[Dict]:
        """生成更新建议"""
        recommendations = {
            'gcc': {
                'priority': 'high' if self._parse_version(version.version) >= (13, 0) else 'medium',
                'reasons': [
                    f"GCC {version.version} 提供改进的C23支持",
                    "更新的警告和优化能力",
                    "安全性和稳定性改进"
                ],
                'actions': [
                    f"更新CI/CD中的GCC版本至{version.version}",
                    "测试代码在新版本下的兼容性",
                    "检查新的编译器警告"
                ]
            },
            'clang': {
                'priority': 'high',
                'reasons': [
                    f"Clang {version.version} 提供优秀的诊断信息",
                    "改进的静态分析能力",
                    "更好的跨平台支持"
                ],
                'actions': [
                    f"更新CI/CD中的Clang版本至{version.version}",
                    "运行静态分析检查",
                    "检查代码格式化工具兼容性"
                ]
            },
            'zig': {
                'priority': 'medium',
                'reasons': [
                    f"Zig {version.version} 包含C编译器改进",
                    "可以作为C代码的编译和交叉编译工具"
                ],
                'actions': [
                    "评估作为交叉编译工具链",
                    "测试C代码在zig cc下的编译"
                ]
            }
        }
        
        return recommendations.get(compiler)
    
    def generate_markdown_report(self) -> str:
        """生成Markdown格式报告"""
        report = []
        report.append("# 编译器版本监控报告\n")
        report.append(f"**生成时间**: {datetime.now().strftime('%Y-%m-%d %H:%M:%S')}\n")
        
        # 汇总
        total_new = sum(len(v) for v in self.new_versions.values())
        report.append(f"**新版本数量**: {total_new}\n\n")
        
        # 各编译器详情
        for compiler, versions in self.new_versions.items():
            if not versions:
                continue
            
            report.append(f"## {compiler.upper()}\n")
            
            # 只显示最新的5个版本
            recent_versions = sorted(
                versions,
                key=lambda v: self._parse_version(v.version),
                reverse=True
            )[:5]
            
            for ver in recent_versions:
                report.append(f"\n### {ver.name} {ver.version}\n")
                
                if ver.release_date:
                    report.append(f"- **发布日期**: {ver.release_date}\n")
                if ver.download_url:
                    report.append(f"- **下载**: [{ver.download_url}]({ver.download_url})\n")
                if ver.release_notes_url:
                    report.append(f"- **发布说明**: [{ver.release_notes_url}]({ver.release_notes_url})\n")
                
                if ver.new_features:
                    report.append("\n**新特性**:\n")
                    for feat in ver.new_features[:10]:  # 限制显示数量
                        report.append(f"- {feat}\n")
                
                if ver.bug_fixes:
                    report.append("\n**重要修复**:\n")
                    for fix in ver.bug_fixes[:5]:
                        report.append(f"- {fix}\n")
            
            report.append("\n---\n")
        
        # 比较和建议
        comparison = self.compare_with_local_matrix()
        
        if comparison['updates_needed']:
            report.append("\n## 支持矩阵更新需求\n")
            for update in comparison['updates_needed']:
                report.append(f"\n- **{update['compiler']}**: 添加版本 {update['new_version']}\n")
        
        if comparison['recommendations']:
            report.append("\n## 更新建议\n")
            for rec in comparison['recommendations']:
                report.append(f"\n### 优先级: {rec['priority'].upper()}\n")
                report.append("\n**理由**:\n")
                for reason in rec['reasons']:
                    report.append(f"- {reason}\n")
                report.append("\n**建议行动**:\n")
                for action in rec['actions']:
                    report.append(f"- [ ] {action}\n")
        
        # 支持矩阵表格
        report.append("\n## 当前C标准支持矩阵\n")
        report.append("\n| 编译器 | 版本 | C89 | C99 | C11 | C17 | C23 |\n")
        report.append("|--------|------|-----|-----|-----|-----|-----|\n")
        
        for compiler, versions in self.support_matrix.items():
            for vm in versions[-3:]:  # 只显示最近3个版本
                row = f"| {vm.compiler} | {vm.version} |"
                row += f" {'✅' if vm.c89 else '❌'} |"
                row += f" {'✅' if vm.c99 else '❌'} |"
                row += f" {'✅' if vm.c11 else '❌'} |"
                row += f" {'✅' if vm.c17 else '❌'} |"
                row += f" {'✅' if vm.c23 else '❌'} |\n"
                report.append(row)
        
        return '\n'.join(report)
    
    def generate_json_report(self) -> str:
        """生成JSON格式报告"""
        report = {
            'generated_at': datetime.now().isoformat(),
            'new_versions': {
                compiler: [v.to_dict() for v in versions]
                for compiler, versions in self.new_versions.items()
            },
            'support_matrix': {
                compiler: [v.to_dict() for v in versions]
                for compiler, versions in self.support_matrix.items()
            },
            'comparison': self.compare_with_local_matrix()
        }
        return json.dumps(report, indent=2, ensure_ascii=False)
    
    def save_report(self, output_file: Optional[str] = None) -> str:
        """保存报告"""
        if output_file is None:
            timestamp = datetime.now().strftime('%Y%m%d_%H%M%S')
            output_file = f"compiler_monitor_report_{timestamp}.md"
        
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


def create_sample_config(output_path: str = 'compiler_monitor_config.yml'):
    """创建示例配置文件"""
    config = {
        'compilers': ['gcc', 'clang', 'zig'],
        'check_prerelease': False,
        'output_format': 'markdown',
        'support_matrix_file': 'compiler_support_matrix.json',
        'min_version': {
            'gcc': '9.0.0',
            'clang': '9.0.0',
            'zig': '0.9.0',
            'msvc': '2019'
        }
    }
    
    with open(output_path, 'w', encoding='utf-8') as f:
        yaml.dump(config, f, default_flow_style=False, allow_unicode=True)
    
    print(f"Sample config created: {output_path}")


def main():
    parser = argparse.ArgumentParser(
        description='Monitor compiler releases and C standard support',
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
Examples:
  %(prog)s                          # 运行监控
  %(prog)s -c config.yml            # 使用自定义配置
  %(prog)s -o report.md             # 指定输出文件
  %(prog)s --token YOUR_GITHUB_TOKEN # 使用GitHub API Token
  %(prog)s --init-config            # 创建示例配置文件
        """
    )
    
    parser.add_argument('-c', '--config', help='配置文件路径')
    parser.add_argument('-o', '--output', help='输出文件路径')
    parser.add_argument('--proxy', help='代理服务器地址')
    parser.add_argument('--token', help='GitHub API Token')
    parser.add_argument('--format', choices=['markdown', 'json'], help='输出格式')
    parser.add_argument('--timeout', type=int, default=30, help='请求超时时间（秒）')
    parser.add_argument('--init-config', action='store_true', help='创建示例配置文件')
    parser.add_argument('-v', '--verbose', action='store_true', help='显示详细日志')
    
    args = parser.parse_args()
    
    if args.init_config:
        create_sample_config()
        return 0
    
    if args.verbose:
        logging.getLogger().setLevel(logging.DEBUG)
    
    try:
        monitor = CompilerMonitor(
            config_file=args.config,
            proxy=args.proxy,
            timeout=args.timeout,
            github_token=args.token
        )
        
        if args.format:
            monitor.config['output_format'] = args.format
        
        # 检查新版本
        new_versions = monitor.check_new_versions()
        
        # 保存支持矩阵
        monitor.save_support_matrix()
        
        # 生成报告
        report_file = monitor.save_report(args.output)
        
        # 输出摘要
        print(f"\n{'='*60}")
        print("编译器监控完成")
        print(f"{'='*60}")
        total = sum(len(v) for v in new_versions.values())
        print(f"发现新版本总数: {total}")
        for compiler, versions in new_versions.items():
            if versions:
                latest = max(versions, key=lambda v: monitor._parse_version(v.version))
                print(f"  {compiler}: {len(versions)} 个版本 (最新: {latest.version})")
        print(f"报告文件: {report_file}")
        
        return 0 if total == 0 else 1
        
    except KeyboardInterrupt:
        print("\n监控已取消")
        return 130
    except Exception as e:
        logger.error(f"Unexpected error: {e}", exc_info=True)
        print(f"\n错误: {e}")
        return 1


if __name__ == '__main__':
    sys.exit(main())

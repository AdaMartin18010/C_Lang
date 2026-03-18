#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
安全公告聚合脚本
聚合CVE、CERT和CISA的安全公告，筛选与C语言相关的漏洞
"""

import os
import sys
import json
import logging
import argparse
import re
from datetime import datetime, timedelta
from pathlib import Path
from typing import Dict, List, Optional, Set, Tuple, Any
from dataclasses import dataclass, field, asdict
from collections import defaultdict

import requests
import yaml


logging.basicConfig(
    level=logging.INFO,
    format='%(asctime)s - %(name)s - %(levelname)s - %(message)s',
    handlers=[
        logging.StreamHandler(),
        logging.FileHandler('security_advisory.log', encoding='utf-8')
    ]
)
logger = logging.getLogger('security_advisory')


@dataclass
class SecurityAdvisory:
    """安全公告"""
    id: str
    title: str
    source: str  # 'nvd', 'cert', 'cisa'
    published_date: str
    modified_date: Optional[str] = None
    severity: str = "unknown"  # 'critical', 'high', 'medium', 'low', 'info', 'unknown'
    cvss_score: Optional[float] = None
    cvss_vector: Optional[str] = None
    description: str = ""
    affected_products: List[str] = field(default_factory=list)
    references: List[str] = field(default_factory=list)
    cwe_ids: List[str] = field(default_factory=list)
    cwe_names: List[str] = field(default_factory=list)
    mitigations: List[str] = field(default_factory=list)
    tags: List[str] = field(default_factory=list)
    is_c_related: bool = False
    
    def to_dict(self) -> Dict:
        return asdict(self)


class SecurityAdvisoryAggregator:
    """安全公告聚合器"""
    
    # API端点
    API_ENDPOINTS = {
        'nvd': 'https://services.nvd.nist.gov/rest/json/cves/2.0',
        'cert': 'https://api.kb.cert.org/v1/vulns',
        'cisa': 'https://api.cisa.gov/known-exploited-vulnerabilities/catalog'
    }
    
    # C语言相关的关键词
    C_RELATED_KEYWORDS = [
        'c language', 'c programming', 'c compiler', 'gcc', 'clang', 'llvm',
        'buffer overflow', 'memory corruption', 'use after free', 'double free',
        'null pointer', 'integer overflow', 'stack overflow', 'heap overflow',
        'format string', 'command injection', 'sql injection', 'c library',
        'libc', 'glibc', 'musl', 'newlib', 'openssl', 'zlib', 'libpng',
        'libjpeg', 'curl', 'libxml2', 'pcre', 'sqlite', 'redis', 'nginx',
        'apache', 'linux kernel', 'windows', 'macos', 'bsd',
        'memcpy', 'strcpy', 'strcat', 'sprintf', 'gets', 'malloc', 'free',
        'untrusted pointer', 'dereference', 'memory leak', 'race condition'
    ]
    
    # 关键C项目列表
    CRITICAL_C_PROJECTS = [
        'linux', 'openssl', 'openssh', 'curl', 'nginx', 'apache', 'redis',
        'sqlite', 'zlib', 'libpng', 'libjpeg-turbo', 'ffmpeg', 'libxml2',
        'gnutls', 'mbedtls', 'boringssl', 'libressl', 'musl', 'glibc',
        'busybox', 'coreutils', 'bash', 'zsh', 'vim', 'emacs', 'git',
        'postgresql', 'mysql', 'sqlite', 'bind', 'unbound', 'nsd',
        'openvpn', 'wireguard', 'strongswan', 'tor', 'i2p'
    ]
    
    def __init__(
        self,
        config_file: Optional[str] = None,
        proxy: Optional[str] = None,
        timeout: int = 60,
        api_key: Optional[str] = None
    ):
        self.config = self._load_config(config_file)
        self.timeout = timeout
        self.nvd_api_key = api_key or os.environ.get('NVD_API_KEY')
        self.session = self._create_session(proxy)
        self.advisories: List[SecurityAdvisory] = []
        self.c_related_advisories: List[SecurityAdvisory] = []
    
    def _load_config(self, config_file: Optional[str]) -> Dict:
        """加载配置文件"""
        default_config = {
            'nvd_api_key': None,
            'days_back': 7,
            'severity_filter': ['critical', 'high', 'medium'],
            'c_keywords': self.C_RELATED_KEYWORDS,
            'critical_projects': self.CRITICAL_C_PROJECTS,
            'output_format': 'markdown',
            'min_cvss_score': 0.0,
            'max_results': 100,
            'sources': ['nvd', 'cert', 'cisa'],
            'state_file': '.security_advisory_state.json'
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
            'User-Agent': 'Security-Advisory-Aggregator/1.0',
            'Accept': 'application/json'
        })
        
        if proxy:
            session.proxies = {'http': proxy, 'https': proxy}
        elif 'HTTP_PROXY' in os.environ:
            session.proxies = {
                'http': os.environ['HTTP_PROXY'],
                'https': os.environ.get('HTTPS_PROXY', os.environ['HTTP_PROXY'])
            }
        
        return session
    
    def _is_c_related(self, text: str) -> Tuple[bool, List[str]]:
        """判断是否与C语言相关"""
        text_lower = text.lower()
        matched_keywords = []
        
        for keyword in self.config.get('c_keywords', self.C_RELATED_KEYWORDS):
            if keyword.lower() in text_lower:
                matched_keywords.append(keyword)
        
        # 检查关键项目
        for project in self.config.get('critical_projects', self.CRITICAL_C_PROJECTS):
            if project.lower() in text_lower:
                matched_keywords.append(f"project:{project}")
        
        return len(matched_keywords) > 0, matched_keywords
    
    def _cvss_to_severity(self, score: float) -> str:
        """CVSS分数转严重程度"""
        if score >= 9.0:
            return 'critical'
        elif score >= 7.0:
            return 'high'
        elif score >= 4.0:
            return 'medium'
        elif score > 0:
            return 'low'
        return 'info'
    
    def fetch_nvd_cves(self) -> List[SecurityAdvisory]:
        """从NVD获取CVE数据"""
        advisories = []
        
        try:
            logger.info("Fetching CVE data from NVD...")
            
            # 计算日期范围
            days_back = self.config.get('days_back', 7)
            end_date = datetime.now()
            start_date = end_date - timedelta(days=days_back)
            
            params = {
                'pubStartDate': start_date.strftime('%Y-%m-%dT%H:%M:%S.000'),
                'pubEndDate': end_date.strftime('%Y-%m-%dT%H:%M:%S.000'),
                'resultsPerPage': min(self.config.get('max_results', 100), 2000)
            }
            
            headers = {}
            if self.nvd_api_key:
                headers['apiKey'] = self.nvd_api_key
            
            response = self.session.get(
                self.API_ENDPOINTS['nvd'],
                params=params,
                headers=headers,
                timeout=self.timeout
            )
            response.raise_for_status()
            
            data = response.json()
            vulnerabilities = data.get('vulnerabilities', [])
            
            logger.info(f"Fetched {len(vulnerabilities)} CVEs from NVD")
            
            for vuln in vulnerabilities:
                cve = vuln.get('cve', {})
                cve_id = cve.get('id', '')
                
                # 提取描述
                descriptions = cve.get('descriptions', [])
                description = ''
                for desc in descriptions:
                    if desc.get('lang') == 'en':
                        description = desc.get('value', '')
                        break
                
                if not description:
                    description = descriptions[0].get('value', '') if descriptions else ''
                
                # 检查是否与C相关
                full_text = f"{cve_id} {description}"
                is_c_related, tags = self._is_c_related(full_text)
                
                # 提取CVSS评分
                cvss_score = None
                cvss_vector = None
                severity = 'unknown'
                
                metrics = cve.get('metrics', {})
                
                # 优先使用CVSS v3.1
                if 'cvssMetricV31' in metrics:
                    cvss_data = metrics['cvssMetricV31'][0].get('cvssData', {})
                    cvss_score = cvss_data.get('baseScore')
                    cvss_vector = cvss_data.get('vectorString')
                    severity = metrics['cvssMetricV31'][0].get('baseSeverity', '').lower()
                elif 'cvssMetricV30' in metrics:
                    cvss_data = metrics['cvssMetricV30'][0].get('cvssData', {})
                    cvss_score = cvss_data.get('baseScore')
                    cvss_vector = cvss_data.get('vectorString')
                    severity = metrics['cvssMetricV30'][0].get('baseSeverity', '').lower()
                elif 'cvssMetricV2' in metrics:
                    cvss_score = metrics['cvssMetricV2'][0].get('cvssData', {}).get('baseScore')
                    cvss_vector = metrics['cvssMetricV2'][0].get('cvssData', {}).get('vectorString')
                    severity = self._cvss_to_severity(cvss_score) if cvss_score else 'unknown'
                
                # 检查严重程度过滤
                severity_filter = self.config.get('severity_filter', [])
                min_cvss = self.config.get('min_cvss_score', 0.0)
                
                if severity_filter and severity not in severity_filter:
                    continue
                
                if cvss_score is not None and cvss_score < min_cvss:
                    continue
                
                # 提取CWE
                cwes = []
                cwe_names = []
                weaknesses = cve.get('weaknesses', [])
                for weakness in weaknesses:
                    for desc in weakness.get('description', []):
                        if desc.get('lang') == 'en':
                            cwe_id = desc.get('value', '')
                            if cwe_id.startswith('CWE-'):
                                cwes.append(cwe_id)
                
                # 提取参考链接
                references = []
                for ref in cve.get('references', []):
                    url = ref.get('url', '')
                    if url:
                        references.append(url)
                
                # 提取影响的产品
                affected_products = []
                configurations = vuln.get('configurations', [])
                for config in configurations:
                    for node in config.get('nodes', []):
                        for cpe in node.get('cpeMatch', []):
                            criteria = cpe.get('criteria', '')
                            if criteria:
                                affected_products.append(criteria)
                
                advisory = SecurityAdvisory(
                    id=cve_id,
                    title=f"CVE: {description[:100]}..." if len(description) > 100 else f"CVE: {description}",
                    source='nvd',
                    published_date=cve.get('published', ''),
                    modified_date=cve.get('lastModified', ''),
                    severity=severity,
                    cvss_score=cvss_score,
                    cvss_vector=cvss_vector,
                    description=description,
                    affected_products=affected_products[:10],  # 限制数量
                    references=references[:5],
                    cwe_ids=cwes,
                    cwe_names=cwe_names,
                    tags=tags,
                    is_c_related=is_c_related
                )
                
                advisories.append(advisory)
            
        except requests.exceptions.RequestException as e:
            logger.error(f"Failed to fetch NVD data: {e}")
        except Exception as e:
            logger.error(f"Error processing NVD data: {e}", exc_info=True)
        
        return advisories
    
    def fetch_cert_advisories(self) -> List[SecurityAdvisory]:
        """获取CERT公告"""
        advisories = []
        
        try:
            logger.info("Fetching CERT advisories...")
            
            # CERT API可能需要特定的访问权限
            # 这里使用简化的实现
            
            # 获取CERT RSS/Atom feed的替代方法
            cert_feeds = [
                'https://www.kb.cert.org/vuls/atom/',
                'https://seclists.org/rss/bugtraq.rss'
            ]
            
            for feed_url in cert_feeds:
                try:
                    response = self.session.get(feed_url, timeout=self.timeout)
                    if response.status_code == 200:
                        # 简化的RSS解析
                        content = response.text.lower()
                        
                        # 查找C相关的内容
                        is_related, tags = self._is_c_related(content)
                        
                        if is_related:
                            # 提取条目（简化处理）
                            advisory = SecurityAdvisory(
                                id=f"CERT-{datetime.now().strftime('%Y%m%d')}",
                                title="CERT Advisory (C-related)",
                                source='cert',
                                published_date=datetime.now().isoformat(),
                                description=f"Found C-related security content in {feed_url}",
                                references=[feed_url],
                                tags=tags,
                                is_c_related=True
                            )
                            advisories.append(advisory)
                
                except Exception as e:
                    logger.warning(f"Failed to fetch {feed_url}: {e}")
        
        except Exception as e:
            logger.error(f"Error fetching CERT advisories: {e}")
        
        return advisories
    
    def fetch_cisa_advisories(self) -> List[SecurityAdvisory]:
        """获取CISA已知被利用漏洞目录"""
        advisories = []
        
        try:
            logger.info("Fetching CISA KEV catalog...")
            
            response = self.session.get(
                self.API_ENDPOINTS['cisa'],
                timeout=self.timeout
            )
            response.raise_for_status()
            
            data = response.json()
            vulnerabilities = data.get('vulnerabilities', [])
            
            logger.info(f"Fetched {len(vulnerabilities)} vulnerabilities from CISA")
            
            for vuln in vulnerabilities:
                cve_id = vuln.get('cveID', '')
                description = vuln.get('shortDescription', '')
                
                # 检查是否与C相关
                full_text = f"{cve_id} {description} {vuln.get('product', '')}"
                is_c_related, tags = self._is_c_related(full_text)
                
                if not is_c_related:
                    continue
                
                # 映射严重程度
                known_ransomware = vuln.get('knownRansomwareCampaignUse', '')
                severity = 'critical' if known_ransomware == 'Known' else 'high'
                
                advisory = SecurityAdvisory(
                    id=cve_id,
                    title=f"CISA KEV: {description[:100]}..." if len(description) > 100 else f"CISA KEV: {description}",
                    source='cisa',
                    published_date=vuln.get('dateAdded', ''),
                    severity=severity,
                    description=description,
                    affected_products=[vuln.get('product', '')],
                    references=[vuln.get('references', '')],
                    mitigations=[vuln.get('requiredAction', '')],
                    tags=tags + (['ransomware'] if known_ransomware == 'Known' else []),
                    is_c_related=True
                )
                
                advisories.append(advisory)
        
        except requests.exceptions.RequestException as e:
            logger.error(f"Failed to fetch CISA data: {e}")
        except Exception as e:
            logger.error(f"Error processing CISA data: {e}", exc_info=True)
        
        return advisories
    
    def aggregate(self) -> Tuple[List[SecurityAdvisory], List[SecurityAdvisory]]:
        """聚合所有安全公告"""
        logger.info("Starting security advisory aggregation...")
        
        all_advisories = []
        sources = self.config.get('sources', ['nvd', 'cert', 'cisa'])
        
        if 'nvd' in sources:
            try:
                nvd_advisories = self.fetch_nvd_cves()
                all_advisories.extend(nvd_advisories)
                logger.info(f"NVD: {len(nvd_advisories)} advisories")
            except Exception as e:
                logger.error(f"Error fetching from NVD: {e}")
        
        if 'cert' in sources:
            try:
                cert_advisories = self.fetch_cert_advisories()
                all_advisories.extend(cert_advisories)
                logger.info(f"CERT: {len(cert_advisories)} advisories")
            except Exception as e:
                logger.error(f"Error fetching from CERT: {e}")
        
        if 'cisa' in sources:
            try:
                cisa_advisories = self.fetch_cisa_advisories()
                all_advisories.extend(cisa_advisories)
                logger.info(f"CISA: {len(cisa_advisories)} advisories")
            except Exception as e:
                logger.error(f"Error fetching from CISA: {e}")
        
        self.advisories = all_advisories
        self.c_related_advisories = [a for a in all_advisories if a.is_c_related]
        
        # 去重
        seen_ids = set()
        unique_advisories = []
        for adv in all_advisories:
            if adv.id not in seen_ids:
                seen_ids.add(adv.id)
                unique_advisories.append(adv)
        
        self.advisories = unique_advisories
        self.c_related_advisories = [a for a in unique_advisories if a.is_c_related]
        
        logger.info(f"Aggregation complete: {len(self.advisories)} total, {len(self.c_related_advisories)} C-related")
        
        return self.advisories, self.c_related_advisories
    
    def generate_markdown_report(self) -> str:
        """生成Markdown格式报告"""
        report = []
        report.append("# C语言安全公告报告\n")
        report.append(f"**生成时间**: {datetime.now().strftime('%Y-%m-%d %H:%M:%S')}\n")
        report.append(f"**报告周期**: 过去 {self.config.get('days_back', 7)} 天\n")
        report.append(f"**总公告数**: {len(self.advisories)}\n")
        report.append(f"**C相关公告数**: {len(self.c_related_advisories)}\n\n")
        
        # C相关公告摘要
        if self.c_related_advisories:
            report.append("## 🚨 C相关安全公告\n")
            
            # 按严重程度分组
            severity_order = ['critical', 'high', 'medium', 'low', 'info', 'unknown']
            by_severity = defaultdict(list)
            
            for adv in self.c_related_advisories:
                by_severity[adv.severity].append(adv)
            
            for severity in severity_order:
                advisories = by_severity.get(severity, [])
                if not advisories:
                    continue
                
                emoji = {
                    'critical': '🔴', 'high': '🟠', 'medium': '🟡',
                    'low': '🟢', 'info': '🔵', 'unknown': '⚪'
                }.get(severity, '⚪')
                
                report.append(f"\n### {emoji} {severity.upper()} ({len(advisories)})\n")
                
                for adv in sorted(advisories, key=lambda x: x.cvss_score or 0, reverse=True):
                    report.append(f"\n#### {adv.id}\n")
                    report.append(f"**标题**: {adv.title}\n")
                    report.append(f"**来源**: {adv.source.upper()}\n")
                    report.append(f"**发布日期**: {adv.published_date[:10] if adv.published_date else 'Unknown'}\n")
                    
                    if adv.cvss_score:
                        report.append(f"**CVSS评分**: {adv.cvss_score}\n")
                    
                    if adv.cwe_ids:
                        report.append(f"**CWE**: {', '.join(adv.cwe_ids[:3])}\n")
                    
                    if adv.affected_products:
                        report.append(f"**影响产品**: {', '.join(adv.affected_products[:3])}\n")
                    
                    if adv.tags:
                        report.append(f"**相关标签**: {', '.join(adv.tags[:5])}\n")
                    
                    report.append(f"\n**描述**:\n{adv.description}\n")
                    
                    if adv.mitigations:
                        report.append(f"\n**缓解措施**:\n")
                        for mit in adv.mitigations:
                            report.append(f"- {mit}\n")
                    
                    if adv.references:
                        report.append(f"\n**参考链接**:\n")
                        for ref in adv.references[:3]:
                            report.append(f"- [{ref}]({ref})\n")
                    
                    report.append("\n---\n")
        
        # 统计信息
        report.append("\n## 📊 统计摘要\n")
        
        # 按来源统计
        by_source = defaultdict(int)
        for adv in self.advisories:
            by_source[adv.source] += 1
        
        report.append("\n### 按来源分布\n")
        for source, count in sorted(by_source.items()):
            report.append(f"- **{source.upper()}**: {count}\n")
        
        # 按严重程度统计
        by_severity = defaultdict(int)
        for adv in self.advisories:
            by_severity[adv.severity] += 1
        
        report.append("\n### 按严重程度分布\n")
        for severity in severity_order:
            count = by_severity.get(severity, 0)
            if count > 0:
                report.append(f"- **{severity}**: {count}\n")
        
        # 常见CWE
        cwe_counts = defaultdict(int)
        for adv in self.c_related_advisories:
            for cwe in adv.cwe_ids:
                cwe_counts[cwe] += 1
        
        if cwe_counts:
            report.append("\n### 常见CWE类型（C相关）\n")
            for cwe, count in sorted(cwe_counts.items(), key=lambda x: x[1], reverse=True)[:10]:
                report.append(f"- **{cwe}**: {count} 次\n")
        
        # 行动建议
        report.append("\n## 🛡️ 安全建议\n")
        
        critical_high = [a for a in self.c_related_advisories 
                        if a.severity in ['critical', 'high']]
        
        if critical_high:
            report.append(f"\n### 高优先级处理 ({len(critical_high)} 个)\n")
            report.append("\n1. 立即审查以下CVE影响的系统:\n")
            for adv in critical_high[:10]:
                report.append(f"   - {adv.id}: {adv.title[:80]}\n")
            report.append("\n2. 检查使用的C库和依赖项是否需要更新\n")
            report.append("\n3. 运行静态分析工具扫描代码中的类似漏洞\n")
        
        report.append("\n### 常规安全实践\n")
        report.append("\n- [ ] 定期更新编译器和依赖库\n")
        report.append("- [ ] 启用编译器安全选项 (-fstack-protector, -D_FORTIFY_SOURCE, etc.)\n")
        report.append("- [ ] 使用静态分析工具 (Clang Static Analyzer, Coverity, etc.)\n")
        report.append("- [ ] 审查代码中的不安全函数使用 (strcpy, gets, sprintf, etc.)\n")
        report.append("- [ ] 实施模糊测试 (fuzzing) 流程\n")
        
        return '\n'.join(report)
    
    def generate_json_report(self) -> str:
        """生成JSON格式报告"""
        report = {
            'generated_at': datetime.now().isoformat(),
            'config': {
                'days_back': self.config.get('days_back', 7),
                'severity_filter': self.config.get('severity_filter', [])
            },
            'summary': {
                'total_advisories': len(self.advisories),
                'c_related_advisories': len(self.c_related_advisories),
                'by_source': {},
                'by_severity': {}
            },
            'advisories': [a.to_dict() for a in self.advisories],
            'c_related_advisories': [a.to_dict() for a in self.c_related_advisories]
        }
        
        # 统计
        for adv in self.advisories:
            report['summary']['by_source'][adv.source] = \
                report['summary']['by_source'].get(adv.source, 0) + 1
            report['summary']['by_severity'][adv.severity] = \
                report['summary']['by_severity'].get(adv.severity, 0) + 1
        
        return json.dumps(report, indent=2, ensure_ascii=False)
    
    def save_report(self, output_file: Optional[str] = None) -> str:
        """保存报告"""
        if output_file is None:
            timestamp = datetime.now().strftime('%Y%m%d_%H%M%S')
            output_file = f"security_advisory_report_{timestamp}.md"
        
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


def create_sample_config(output_path: str = 'security_advisory_config.yml'):
    """创建示例配置文件"""
    config = {
        'nvd_api_key': None,
        'days_back': 7,
        'severity_filter': ['critical', 'high', 'medium'],
        'c_keywords': SecurityAdvisoryAggregator.C_RELATED_KEYWORDS,
        'critical_projects': SecurityAdvisoryAggregator.CRITICAL_C_PROJECTS,
        'output_format': 'markdown',
        'min_cvss_score': 0.0,
        'max_results': 100,
        'sources': ['nvd', 'cert', 'cisa'],
        'state_file': '.security_advisory_state.json'
    }
    
    with open(output_path, 'w', encoding='utf-8') as f:
        yaml.dump(config, f, default_flow_style=False, allow_unicode=True)
    
    print(f"Sample config created: {output_path}")


def main():
    parser = argparse.ArgumentParser(
        description='Aggregate security advisories for C language',
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
Examples:
  %(prog)s                          # 运行聚合（默认配置）
  %(prog)s -c config.yml            # 使用自定义配置
  %(prog)s -o report.md             # 指定输出文件
  %(prog)s --days 14                # 查看过去14天的公告
  %(prog)s --key YOUR_NVD_API_KEY   # 使用NVD API Key
  %(prog)s --init-config            # 创建示例配置文件

Environment Variables:
  NVD_API_KEY    - NVD API访问密钥
  HTTP_PROXY     - HTTP代理设置
        """
    )
    
    parser.add_argument('-c', '--config', help='配置文件路径')
    parser.add_argument('-o', '--output', help='输出文件路径')
    parser.add_argument('--proxy', help='代理服务器地址')
    parser.add_argument('--key', help='NVD API Key')
    parser.add_argument('--days', type=int, help='查询天数范围')
    parser.add_argument('--format', choices=['markdown', 'json'], help='输出格式')
    parser.add_argument('--timeout', type=int, default=60, help='请求超时时间（秒）')
    parser.add_argument('--init-config', action='store_true', help='创建示例配置文件')
    parser.add_argument('-v', '--verbose', action='store_true', help='显示详细日志')
    
    args = parser.parse_args()
    
    if args.init_config:
        create_sample_config()
        return 0
    
    if args.verbose:
        logging.getLogger().setLevel(logging.DEBUG)
    
    try:
        aggregator = SecurityAdvisoryAggregator(
            config_file=args.config,
            proxy=args.proxy,
            timeout=args.timeout,
            api_key=args.key
        )
        
        if args.days:
            aggregator.config['days_back'] = args.days
        
        if args.format:
            aggregator.config['output_format'] = args.format
        
        # 聚合数据
        all_advisories, c_related = aggregator.aggregate()
        
        # 生成报告
        report_file = aggregator.save_report(args.output)
        
        # 输出摘要
        print(f"\n{'='*60}")
        print("安全公告聚合完成")
        print(f"{'='*60}")
        print(f"总公告数: {len(all_advisories)}")
        print(f"C相关公告: {len(c_related)}")
        
        if c_related:
            by_severity = defaultdict(int)
            for adv in c_related:
                by_severity[adv.severity] += 1
            print("\nC相关公告严重程度分布:")
            for sev, count in sorted(by_severity.items(), 
                                     key=lambda x: {'critical':0,'high':1,'medium':2,'low':3,'unknown':4}.get(x[0],5)):
                print(f"  {sev}: {count}")
        
        print(f"\n报告文件: {report_file}")
        
        # 有关键漏洞时返回非零退出码
        critical_count = len([a for a in c_related if a.severity == 'critical'])
        return 2 if critical_count > 0 else (1 if c_related else 0)
        
    except KeyboardInterrupt:
        print("\n操作已取消")
        return 130
    except Exception as e:
        logger.error(f"Unexpected error: {e}", exc_info=True)
        print(f"\n错误: {e}")
        return 1


if __name__ == '__main__':
    sys.exit(main())

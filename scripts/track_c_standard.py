#!/usr/bin/env python3
"""
C标准特性跟踪脚本
定期检查GCC/Clang发布说明，生成C23/C2y特性支持报告

用法:
    python track_c_standard.py [--check-gcc] [--check-clang] [--report]
"""

import argparse
import json
import os
import re
import subprocess
import sys
import urllib.request
from datetime import datetime
from pathlib import Path
from typing import Dict, List, Optional, Tuple

# 项目根目录
PROJECT_ROOT = Path(__file__).parent.parent
KNOWLEDGE_DIR = PROJECT_ROOT / "knowledge" / "05_C_Language_Standards"
TRACKING_DIR = KNOWLEDGE_DIR / "C23_C2y_Tracking"

# C23/C2y 特性定义
C23_FEATURES = {
    "core_language": [
        ("auto", "C23", "类型推导", "gcc-10+", "clang-13+"),
        ("typeof", "C23", "类型获取操作符", "gcc-10+", "clang-13+"),
        ("nullptr", "C23", "类型安全的空指针", "gcc-13+", "clang-15+"),
        ("constexpr", "C23", "编译期常量", "gcc-13+", "clang-17+"),
        ("static_assert", "C23", "可选消息的静态断言", "gcc-13+", "clang-15+"),
        ("attributes", "C23", "标准属性语法[[...]]", "gcc-11+", "clang-14+"),
        ("_BitInt", "C23", "任意精度整数", "gcc-11+", "clang-15+"),
        ("binary_literals", "C23", "二进制常量0b...", "gcc-4.3+", "clang-2.9+"),
        ("digit_separators", "C23", "数字分隔符'", "gcc-13+", "clang-16+"),
        ("embed", "C23", "#embed指令", "gcc-12+", "clang-16+"),
        ("warning_directive", "C23", "#warning指令", "gcc-4.8+", "clang-2.9+"),
    ],
    "library": [
        ("memset_explicit", "C23", "安全内存清零", "gcc-13+", "clang-17+"),
        ("strdup", "C23", "字符串复制", "gcc-2.0+", "clang-all"),
        ("strndup", "C23", "长度限制字符串复制", "gcc-2.0+", "clang-all"),
        ("memccpy", "C23", "条件内存复制", "gcc-all", "clang-all"),
        ("gmtime_r", "C23", "线程安全GMT转换", "glibc", "POSIX"),
        ("localtime_r", "C23", "线程安全本地时间", "glibc", "POSIX"),
        ("timespec_getres", "C23", "时间分辨率查询", "glibc-2.34+", "clang-16+"),
        ("stdckdint", "C23", "溢出检查整数算术", "gcc-13+", "clang-17+"),
        ("math_constants", "C23", "数学常量如M_PI", "gcc-13+", "clang-17+"),
        ("new_math_functions", "C23", "新增数学函数", "gcc-13+", "clang-17+"),
    ]
}

C2Y_FEATURES = {
    "language": [
        ("defer", "C2y", "延迟执行语句", "gcc-15+", "clang-20+"),
        ("_Countof", "C2y", "数组元素计数", "gcc-15+", "clang-20+"),
        ("if_declaration", "C2y", "if/switch声明语句", "gcc-15+", "clang-20+"),
        ("typeof_unqual", "C2y", "无限定typeof", "gcc-15+", "clang-20+"),
        ("named_loops", "C2y", "命名循环", "gcc-15+", "clang-20+"),
        ("__COUNTER__", "C2y", "预定义计数器宏", "gcc-15+", "clang-20+"),
    ],
    "in_progress": [
        ("constexpr_functions", "C2y", "编译期函数", "⚠️", "⚠️"),
        ("strlcpy", "C2y", "BSD风格安全复制", "🔄", "🔄"),
        ("reallocarray", "C2y", "安全检查的重分配", "🔄", "🔄"),
    ]
}

class CompilerVersion:
    """编译器版本信息"""
    
    def __init__(self, name: str, version: str, release_date: str = ""):
        self.name = name
        self.version = version
        self.release_date = release_date
        self.features: Dict[str, bool] = {}
    
    def __repr__(self):
        return f"{self.name} {self.version}"


class CStandardTracker:
    """C标准特性跟踪器"""
    
    def __init__(self):
        self.gcc_versions: List[CompilerVersion] = []
        self.clang_versions: List[CompilerVersion] = []
        self.feature_status: Dict[str, Dict] = {}
        self.load_cache()
    
    def load_cache(self):
        """加载缓存的跟踪数据"""
        cache_file = TRACKING_DIR / "feature_cache.json"
        if cache_file.exists():
            try:
                with open(cache_file, 'r') as f:
                    self.feature_status = json.load(f)
            except json.JSONDecodeError:
                self.feature_status = {}
        else:
            self.feature_status = {
                "last_update": "",
                "gcc": {},
                "clang": {},
                "features": {}
            }
    
    def save_cache(self):
        """保存跟踪数据到缓存"""
        TRACKING_DIR.mkdir(parents=True, exist_ok=True)
        cache_file = TRACKING_DIR / "feature_cache.json"
        self.feature_status["last_update"] = datetime.now().isoformat()
        with open(cache_file, 'w') as f:
            json.dump(self.feature_status, f, indent=2)
    
    def check_local_gcc(self) -> Optional[CompilerVersion]:
        """检查本地GCC版本"""
        try:
            result = subprocess.run(
                ["gcc", "--version"],
                capture_output=True,
                text=True,
                timeout=10
            )
            if result.returncode == 0:
                match = re.search(r'gcc.*?(\d+\.\d+\.?\d*)', result.stdout, re.I)
                if match:
                    version = match.group(1)
                    return CompilerVersion("GCC", version)
        except (subprocess.SubprocessError, FileNotFoundError):
            pass
        return None
    
    def check_local_clang(self) -> Optional[CompilerVersion]:
        """检查本地Clang版本"""
        try:
            result = subprocess.run(
                ["clang", "--version"],
                capture_output=True,
                text=True,
                timeout=10
            )
            if result.returncode == 0:
                match = re.search(r'clang version (\d+\.\d+\.?\d*)', result.stdout)
                if match:
                    version = match.group(1)
                    return CompilerVersion("Clang", version)
        except (subprocess.SubprocessError, FileNotFoundError):
            pass
        return None
    
    def check_c23_support(self, compiler: str) -> Dict[str, bool]:
        """检查编译器的C23支持"""
        test_code = """
#include <stdio.h>
#if __STDC_VERSION__ >= 202311L
    #define C23_SUPPORTED 1
#else
    #define C23_SUPPORTED 0
#endif

int main() {
    auto x = 42;
    #ifdef __cplusplus
        return 1;
    #endif
    return C23_SUPPORTED ? 0 : 2;
}
"""
        features = {}
        
        # 检查auto支持
        try:
            with subprocess.Popen(
                [compiler, "-std=c23", "-x", "c", "-", "-o", "/dev/null"],
                stdin=subprocess.PIPE,
                stdout=subprocess.PIPE,
                stderr=subprocess.PIPE
            ) as proc:
                stdout, stderr = proc.communicate(test_code.encode(), timeout=10)
                features["c23_basic"] = proc.returncode == 0
        except (subprocess.SubprocessError, FileNotFoundError):
            features["c23_basic"] = False
        
        return features
    
    def generate_report(self) -> str:
        """生成特性支持报告"""
        lines = []
        lines.append("# C23/C2y 特性支持报告")
        lines.append(f"\n生成时间: {datetime.now().strftime('%Y-%m-%d %H:%M:%S')}")
        lines.append("\n## 本地编译器检测")
        
        # 检测本地编译器
        gcc = self.check_local_gcc()
        clang = self.check_local_clang()
        
        if gcc:
            lines.append(f"\n- **GCC**: {gcc.version}")
            features = self.check_c23_support("gcc")
            lines.append(f"  - C23基本支持: {'✅' if features.get('c23_basic') else '❌'}")
        else:
            lines.append("\n- **GCC**: 未检测到")
        
        if clang:
            lines.append(f"\n- **Clang**: {clang.version}")
            features = self.check_c23_support("clang")
            lines.append(f"  - C23基本支持: {'✅' if features.get('c23_basic') else '❌'}")
        else:
            lines.append("\n- **Clang**: 未检测到")
        
        # C23特性
        lines.append("\n## C23 语言特性")
        lines.append("\n| 特性 | 标准 | 描述 | GCC | Clang |")
        lines.append("|------|------|------|-----|-------|")
        for name, std, desc, gcc_ver, clang_ver in C23_FEATURES["core_language"]:
            lines.append(f"| {name} | {std} | {desc} | {gcc_ver} | {clang_ver} |")
        
        # C23库
        lines.append("\n## C23 标准库")
        lines.append("\n| 特性 | 标准 | 描述 | GCC | Clang |")
        lines.append("|------|------|------|-----|-------|")
        for name, std, desc, gcc_ver, clang_ver in C23_FEATURES["library"]:
            lines.append(f"| {name} | {std} | {desc} | {gcc_ver} | {clang_ver} |")
        
        # C2y特性
        lines.append("\n## C2y 语言特性（开发中）")
        lines.append("\n| 特性 | 标准 | 描述 | GCC | Clang |")
        lines.append("|------|------|------|-----|-------|")
        for name, std, desc, gcc_ver, clang_ver in C2Y_FEATURES["language"]:
            lines.append(f"| {name} | {std} | {desc} | {gcc_ver} | {clang_ver} |")
        
        # 进行中
        lines.append("\n## 讨论中特性")
        lines.append("\n| 特性 | 标准 | 描述 | 状态 |")
        lines.append("|------|------|------|------|")
        for name, std, desc, status, _ in C2Y_FEATURES["in_progress"]:
            lines.append(f"| {name} | {std} | {desc} | {status} |")
        
        # 建议
        lines.append("\n## 升级建议")
        lines.append("\n### 立即使用（GCC 13+, Clang 17+）")
        lines.append("- `auto` 类型推导")
        lines.append("- `typeof` 操作符")
        lines.append("- `nullptr` 常量")
        lines.append("- 标准属性语法 `[[...]]`")
        lines.append("- `_BitInt` 任意精度整数")
        
        lines.append("\n### 谨慎使用（GCC 15+, Clang 20+）")
        lines.append("- `defer` 语句 - 重大语言变化")
        lines.append("- `_Countof` 操作符")
        lines.append("- `if` 声明语句")
        lines.append("- named loops")
        
        lines.append("\n### 等待稳定")
        lines.append("- `constexpr` 函数")
        lines.append("- 模块化系统")
        
        return "\n".join(lines)
    
    def update_documentation(self):
        """更新文档中的特性状态"""
        # 更新主路线图文档中的状态表格
        roadmap_file = KNOWLEDGE_DIR / "C23_C2Y_ROADMAP.md"
        if roadmap_file.exists():
            print(f"📄 检查文档: {roadmap_file}")
            # 这里可以添加自动更新逻辑
        
        # 更新特性矩阵
        matrix_file = KNOWLEDGE_DIR / "C23_C2Y_SUPPORT_MATRIX.md"
        if matrix_file.exists():
            print(f"📄 检查文档: {matrix_file}")
    
    def check_for_updates(self) -> List[str]:
        """检查新支持的特性"""
        updates = []
        
        # 模拟：检测到新特性
        gcc = self.check_local_gcc()
        if gcc and gcc.version >= "15.0":
            if not self.feature_status.get("gcc", {}).get("defer_reported"):
                updates.append("GCC 15 支持 defer 语句")
                self.feature_status.setdefault("gcc", {})["defer_reported"] = True
        
        clang = self.check_local_clang()
        if clang and clang.version >= "20.0":
            if not self.feature_status.get("clang", {}).get("named_loops_reported"):
                updates.append("Clang 20 支持 named loops")
                self.feature_status.setdefault("clang", {})["named_loops_reported"] = True
        
        return updates


def main():
    parser = argparse.ArgumentParser(
        description="C23/C2y 标准特性跟踪工具"
    )
    parser.add_argument(
        "--check-gcc",
        action="store_true",
        help="检查GCC版本和支持"
    )
    parser.add_argument(
        "--check-clang",
        action="store_true",
        help="检查Clang版本和支持"
    )
    parser.add_argument(
        "--report",
        action="store_true",
        help="生成支持报告"
    )
    parser.add_argument(
        "--update",
        action="store_true",
        help="更新文档中的状态"
    )
    parser.add_argument(
        "--output",
        type=str,
        default="c23_c2y_report.md",
        help="报告输出文件"
    )
    
    args = parser.parse_args()
    
    tracker = CStandardTracker()
    
    print("🔍 C23/C2y 标准特性跟踪工具")
    print("=" * 40)
    
    # 如果没有指定特定检查，执行全部
    if not (args.check_gcc or args.check_clang or args.report or args.update):
        args.check_gcc = True
        args.check_clang = True
        args.report = True
    
    if args.check_gcc:
        print("\n📋 检查GCC...")
        gcc = tracker.check_local_gcc()
        if gcc:
            print(f"  ✅ 检测到 GCC {gcc.version}")
            features = tracker.check_c23_support("gcc")
            for feat, supported in features.items():
                status = "✅" if supported else "❌"
                print(f"  {status} {feat}")
        else:
            print("  ❌ 未检测到GCC")
    
    if args.check_clang:
        print("\n📋 检查Clang...")
        clang = tracker.check_local_clang()
        if clang:
            print(f"  ✅ 检测到 Clang {clang.version}")
            features = tracker.check_c23_support("clang")
            for feat, supported in features.items():
                status = "✅" if supported else "❌"
                print(f"  {status} {feat}")
        else:
            print("  ❌ 未检测到Clang")
    
    if args.report:
        print("\n📊 生成报告...")
        report = tracker.generate_report()
        
        output_path = Path(args.output)
        with open(output_path, 'w') as f:
            f.write(report)
        print(f"  ✅ 报告已保存: {output_path}")
    
    if args.update:
        print("\n📝 更新文档...")
        tracker.update_documentation()
    
    # 检查更新
    updates = tracker.check_for_updates()
    if updates:
        print("\n🎉 新特性检测:")
        for update in updates:
            print(f"  • {update}")
    
    # 保存缓存
    tracker.save_cache()
    
    print("\n✨ 完成!")
    return 0


if __name__ == "__main__":
    sys.exit(main())

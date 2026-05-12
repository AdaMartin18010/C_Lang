#!/usr/bin/env python3
"""
C_Lang 示例代码自动化测试脚本
遍历 examples/ 所有子目录，执行 make && make test，生成测试报告

Usage:
    python scripts/test_all_examples.py
    python scripts/test_all_examples.py --report test_report.json
    python scripts/test_all_examples.py --verbose
"""

import os
import sys
import json
import subprocess
import argparse
from pathlib import Path
from datetime import datetime
from dataclasses import dataclass, field, asdict
from typing import List, Optional

PROJECT_ROOT = Path(__file__).parent.parent.resolve()
EXAMPLES_DIR = PROJECT_ROOT / "examples"


@dataclass
class TestResult:
    name: str
    path: str
    build_success: bool = False
    build_output: str = ""
    build_time_ms: int = 0
    test_success: bool = False
    test_output: str = ""
    test_time_ms: int = 0
    has_tests: bool = False
    skipped: bool = False
    skip_reason: str = ""
    compiler: str = ""
    std: str = ""


def run_command(cmd: List[str], cwd: Path, timeout: int = 60) -> tuple[int, str, float]:
    """Run a command and return (returncode, output, elapsed_ms)."""
    import time
    start = time.time()
    try:
        result = subprocess.run(
            cmd,
            cwd=cwd,
            capture_output=True,
            text=True,
            timeout=timeout,
            env={**os.environ, "LC_ALL": "C"},
        )
        elapsed = int((time.time() - start) * 1000)
        output = result.stdout
        if result.stderr:
            output += "\n[STDERR]\n" + result.stderr
        return result.returncode, output, elapsed
    except subprocess.TimeoutExpired:
        elapsed = int((time.time() - start) * 1000)
        return -1, f"TIMEOUT after {timeout}s", elapsed
    except Exception as e:
        elapsed = int((time.time() - start) * 1000)
        return -2, f"ERROR: {e}", elapsed


def discover_examples() -> List[Path]:
    """Discover all example directories containing a Makefile or .c files."""
    examples = []
    if not EXAMPLES_DIR.exists():
        return examples
    for subdir in sorted(EXAMPLES_DIR.iterdir()):
        if subdir.is_dir():
            has_makefile = (subdir / "Makefile").exists() or (subdir / "makefile").exists()
            has_c_files = len(list(subdir.glob("*.c"))) > 0
            if has_makefile or has_c_files:
                examples.append(subdir)
    return examples


def test_example(example_dir: Path, verbose: bool = False) -> TestResult:
    """Test a single example directory."""
    name = example_dir.name
    result = TestResult(name=name, path=str(example_dir.relative_to(PROJECT_ROOT)))

    # Check for Makefile
    makefile = example_dir / "Makefile"
    if not makefile.exists():
        makefile = example_dir / "makefile"

    c_files = list(example_dir.glob("*.c"))

    if not makefile.exists():
        if not c_files:
            result.skipped = True
            result.skip_reason = "No Makefile and no .c files found"
            if verbose:
                print(f"  ⚠️  {name}: No Makefile, skipping")
            return result
        if verbose:
            print(f"  🔨 Building {name} (no Makefile, compiling {len(c_files)} .c files)...")
    else:
        if verbose:
            print(f"  🔨 Building {name}...")

    cc = os.environ.get("CC", "gcc")
    std = os.environ.get("STD", "c11")
    cflags = os.environ.get("CFLAGS", f"-Wall -Wextra -Werror -std={std} -O2")

    # Check if make is available
    make_rc, _, _ = run_command(["make", "--version"], cwd=example_dir, timeout=5)
    has_make = make_rc == 0

    if makefile.exists() and has_make:
        # Try 'make clean' first (ignore failure)
        run_command(["make", "clean"], cwd=example_dir, timeout=10)
        # Build
        build_cmd = ["make", f"CC={cc}", f"CFLAGS={cflags}"]
        rc, output, elapsed = run_command(build_cmd, cwd=example_dir, timeout=60)
    elif c_files:
        # Fallback: compile directly with compiler
        exe_name = f"{name}_test"
        if sys.platform == "win32":
            exe_name += ".exe"
        build_cmd = [cc] + cflags.split() + [str(f) for f in c_files] + ["-o", exe_name]
        rc, output, elapsed = run_command(build_cmd, cwd=example_dir, timeout=60)
        if rc == 0:
            result.build_output = f"Direct compilation successful: {' '.join(build_cmd)}\n{output}"
    else:
        rc = -1
        output = "No Makefile and no compiler available"
        elapsed = 0

    result.build_success = rc == 0
    result.build_output = output
    result.build_time_ms = elapsed
    result.compiler = cc
    result.std = std

    if not result.build_success:
        if verbose:
            print(f"  ❌ {name}: BUILD FAILED ({elapsed}ms)")
        return result

    if verbose:
        print(f"  ✅ {name}: Build OK ({elapsed}ms)")

    # Run tests: try 'make test' first
    rc, output, elapsed = run_command(["make", "test"], cwd=example_dir, timeout=30)
    if rc == 0 or rc == 2:  # Some makefiles return 2 if test target doesn't exist
        # Check if it actually ran tests vs "no rule to make target 'test'"
        if "No rule to make target" in output or "no rule to make target" in output:
            result.has_tests = False
        else:
            result.has_tests = True
            result.test_success = rc == 0
            result.test_output = output
            result.test_time_ms = elapsed
            if verbose:
                status = "✅" if result.test_success else "❌"
                print(f"  {status} {name}: Tests {'OK' if result.test_success else 'FAILED'} ({elapsed}ms)")
            return result

    # Fallback: run any executable we find
    executables = []
    for f in example_dir.iterdir():
        if f.is_file() and os.access(f, os.X_OK) and not f.name.endswith((".sh", ".py")):
            executables.append(f)

    if executables:
        result.has_tests = True
        all_ok = True
        outputs = []
        for exe in executables:
            rc, output, elapsed = run_command([str(exe)], cwd=example_dir, timeout=10)
            outputs.append(f"=== {exe.name} ===\n{output}")
            if rc != 0:
                all_ok = False
        result.test_success = all_ok
        result.test_output = "\n".join(outputs)
        result.test_time_ms = elapsed
        if verbose:
            status = "✅" if result.test_success else "❌"
            print(f"  {status} {name}: Executables {'OK' if result.test_success else 'FAILED'}")
    else:
        result.has_tests = False
        if verbose:
            print(f"  ⚠️  {name}: No tests found")

    return result


def main():
    parser = argparse.ArgumentParser(description="Test all C examples")
    parser.add_argument("--report", type=str, help="Path to write JSON report")
    parser.add_argument("--verbose", "-v", action="store_true", help="Verbose output")
    parser.add_argument("--example", type=str, help="Test only a specific example")
    args = parser.parse_args()

    print("=" * 60)
    print("C_Lang 示例代码测试报告")
    print(f"时间: {datetime.now().isoformat()}")
    print(f"编译器: {os.environ.get('CC', 'gcc')}")
    print(f"标准: {os.environ.get('STD', 'c11')}")
    print("=" * 60)

    examples = discover_examples()
    if args.example:
        examples = [e for e in examples if e.name == args.example]
        if not examples:
            print(f"Error: Example '{args.example}' not found")
            sys.exit(1)

    print(f"\n发现 {len(examples)} 个示例目录\n")

    results: List[TestResult] = []
    build_pass = 0
    build_fail = 0
    test_pass = 0
    test_fail = 0
    no_tests = 0
    skipped = 0

    for i, ex_dir in enumerate(examples, 1):
        print(f"[{i}/{len(examples)}] {ex_dir.name}")
        result = test_example(ex_dir, verbose=args.verbose)
        results.append(result)

        if result.skipped:
            skipped += 1
        elif result.build_success:
            build_pass += 1
            if result.has_tests:
                if result.test_success:
                    test_pass += 1
                else:
                    test_fail += 1
            else:
                no_tests += 1
        else:
            build_fail += 1

    # Summary
    print("\n" + "=" * 60)
    print("汇总")
    print("=" * 60)
    print(f"  总计示例:      {len(results)}")
    print(f"  构建通过:      {build_pass}")
    print(f"  构建失败:      {build_fail}")
    print(f"  无测试:        {no_tests}")
    print(f"  测试通过:      {test_pass}")
    print(f"  测试失败:      {test_fail}")
    print(f"  跳过:          {skipped}")
    print(f"  构建成功率:    {build_pass/(build_pass+build_fail)*100:.1f}%" if (build_pass+build_fail) > 0 else "  N/A")

    if build_fail > 0:
        print("\n  构建失败列表:")
        for r in results:
            if not r.skipped and not r.build_success:
                print(f"    ❌ {r.name}")
                if args.verbose:
                    for line in r.build_output.splitlines()[:5]:
                        print(f"       {line}")

    if test_fail > 0:
        print("\n  测试失败列表:")
        for r in results:
            if r.build_success and r.has_tests and not r.test_success:
                print(f"    ❌ {r.name}")

    # Write report
    report = {
        "generated_at": datetime.now().isoformat(),
        "compiler": os.environ.get("CC", "gcc"),
        "std": os.environ.get("STD", "c11"),
        "summary": {
            "total": len(results),
            "build_pass": build_pass,
            "build_fail": build_fail,
            "no_tests": no_tests,
            "test_pass": test_pass,
            "test_fail": test_fail,
            "skipped": skipped,
        },
        "results": [asdict(r) for r in results],
    }

    if args.report:
        report_path = Path(args.report)
        report_path.write_text(json.dumps(report, indent=2, ensure_ascii=False), encoding="utf-8")
        print(f"\n  报告已保存: {report_path}")

    # Exit code
    sys.exit(1 if (build_fail > 0 or test_fail > 0) else 0)


if __name__ == "__main__":
    main()

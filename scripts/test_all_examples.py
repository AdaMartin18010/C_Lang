#!/usr/bin/env python3
"""
Examples 自动化测试脚本
遍历 examples/ 所有子目录，执行 make clean all，然后运行测试程序。

用法:
    python scripts/test_all_examples.py
    python scripts/test_all_examples.py --verbose
    python scripts/test_all_examples.py --only bit_manipulation data_structures
"""
import os
import subprocess
import sys
import argparse
from pathlib import Path
from concurrent.futures import ThreadPoolExecutor, as_completed

# POSIX-only examples that won't compile on Windows without WSL/MSYS2
POSIX_ONLY = {
    "network", "system", "http_parser", "rt_scheduler"
}

# Expected test executables (pattern -> description)
TEST_PATTERNS = ["test_*", "cve_cases", "performance"]


def is_windows():
    return sys.platform == "win32"


def find_make():
    """Find the correct make command for the platform"""
    if is_windows():
        # Try mingw32-make first, then make
        for cmd in ["mingw32-make", "make"]:
            try:
                subprocess.run([cmd, "--version"], capture_output=True, check=True)
                return cmd
            except (subprocess.CalledProcessError, FileNotFoundError):
                continue
    return "make"


def run_in_dir(directory: Path, cmd: list[str], timeout: int = 60) -> tuple[bool, str]:
    """Run command in directory, return (success, output)"""
    try:
        result = subprocess.run(
            cmd,
            cwd=directory,
            capture_output=True,
            text=True,
            timeout=timeout,
        )
        output = result.stdout + result.stderr
        return result.returncode == 0, output
    except subprocess.TimeoutExpired:
        return False, "TIMEOUT"
    except Exception as e:
        return False, str(e)


def test_directory(directory: Path, verbose: bool) -> dict:
    """Test a single examples subdirectory"""
    name = directory.name
    result = {
        "name": name,
        "build": False,
        "build_output": "",
        "tests": [],
        "skipped": False,
    }

    # Skip POSIX-only on native Windows
    if is_windows() and name in POSIX_ONLY:
        result["skipped"] = True
        result["build_output"] = "Skipped (POSIX only on Windows)"
        return result

    makefile = directory / "Makefile"
    if not makefile.exists():
        result["build_output"] = "No Makefile"
        return result

    # Build
    make_cmd = find_make()
    ok, output = run_in_dir(directory, [make_cmd, "clean", "all"])
    result["build"] = ok
    result["build_output"] = output
    if not ok:
        return result

    # Find and run test executables
    for pattern in TEST_PATTERNS:
        if pattern.endswith("*"):
            prefix = pattern[:-1]
            for exe in directory.iterdir():
                if exe.name.startswith(prefix) and exe.is_file():
                    # Check if executable (posix) or .exe (windows)
                    if is_windows():
                        exe_path = exe.with_suffix(".exe")
                        if not exe_path.exists():
                            continue
                        exe = exe_path
                    elif not os.access(exe, os.X_OK):
                        continue
                    ok2, out2 = run_in_dir(directory, [str(exe)])
                    result["tests"].append({
                        "name": exe.name,
                        "passed": ok2,
                        "output": out2,
                    })
        else:
            exe = directory / pattern
            if is_windows():
                exe = exe.with_suffix(".exe")
            if exe.exists() and (is_windows() or os.access(exe, os.X_OK)):
                ok2, out2 = run_in_dir(directory, [str(exe)])
                result["tests"].append({
                    "name": exe.name,
                    "passed": ok2,
                    "output": out2,
                })

    return result


def print_result(result: dict, verbose: bool):
    name = result["name"]
    if result["skipped"]:
        print(f"  ⏭️  {name}: SKIPPED ({result['build_output']})")
        return

    if not result["build"]:
        print(f"  ❌ {name}: BUILD FAILED")
        if verbose:
            print(f"      {result['build_output'][:500]}")
        return

    test_summary = ""
    if result["tests"]:
        passed = sum(1 for t in result["tests"] if t["passed"])
        total = len(result["tests"])
        test_summary = f" | tests: {passed}/{total} passed"
    print(f"  ✅ {name}: BUILD OK{test_summary}")

    if verbose:
        for t in result["tests"]:
            status = "✅" if t["passed"] else "❌"
            print(f"      {status} {t['name']}")
            if not t["passed"]:
                print(f"         {t['output'][:300]}")


def main():
    parser = argparse.ArgumentParser(description="Test all examples")
    parser.add_argument("--verbose", "-v", action="store_true", help="Verbose output")
    parser.add_argument("--only", nargs="+", help="Only test specified directories")
    parser.add_argument("--jobs", "-j", type=int, default=4, help="Parallel jobs")
    args = parser.parse_args()

    examples_dir = Path("examples")
    if not examples_dir.exists():
        print("Error: examples/ directory not found")
        sys.exit(1)

    dirs = [d for d in examples_dir.iterdir() if d.is_dir()]
    if args.only:
        dirs = [d for d in dirs if d.name in args.only]

    print(f"Testing {len(dirs)} example directories...")
    print(f"Platform: {sys.platform}")
    print()

    results = []
    with ThreadPoolExecutor(max_workers=args.jobs) as executor:
        futures = {executor.submit(test_directory, d, args.verbose): d for d in dirs}
        for future in as_completed(futures):
            result = future.result()
            results.append(result)
            print_result(result, args.verbose)

    print()
    # Summary
    total = len(results)
    built = sum(1 for r in results if r["build"])
    skipped = sum(1 for r in results if r["skipped"])
    failed = total - built - skipped
    tests_passed = sum(
        sum(1 for t in r["tests"] if t["passed"])
        for r in results
    )
    tests_total = sum(len(r["tests"]) for r in results)

    print("=" * 50)
    print(f"Summary: {total} directories")
    print(f"  ✅ Built: {built}")
    print(f"  ❌ Failed: {failed}")
    print(f"  ⏭️  Skipped: {skipped}")
    print(f"  🧪 Tests: {tests_passed}/{tests_total} passed")
    print("=" * 50)

    if failed > 0:
        sys.exit(1)


if __name__ == "__main__":
    main()

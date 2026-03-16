#!/usr/bin/env bash
# =============================================================================
# C_Lang 知识库编译器矩阵测试脚本
# =============================================================================
# 功能：
#   1. 多编译器支持 (GCC 13/14/15, Clang 17/18/19)
#   2. 多标准验证 (C11/C17/C23)
#   3. 详细的测试报告生成
#   4. 支持多种编译选项和警告级别
#   5. 性能基准测试
#
# 用法：
#   ./compiler_matrix.sh [选项]
#
# 选项：
#   -c, --compiler COMPILER    指定编译器 (gcc, gcc-13, clang, clang-17, 等)
#   -s, --standard STANDARD    指定C标准 (c11, c17, c23)
#   -d, --source-dir DIR       源代码目录
#   -o, --output DIR           输出目录
#   -r, --report FILE          生成JSON格式报告
#   -j, --jobs N               并行编译任务数 (默认: 4)
#   -t, --timeout SECONDS      编译超时时间 (默认: 30)
#   --strict                   启用严格模式 (更多警告作为错误)
#   --performance              启用性能测试
#   --verbose                  详细输出
#   -h, --help                 显示帮助信息
#
# 示例：
#   # 使用 GCC 14 测试 C17 标准
#   ./compiler_matrix.sh --compiler gcc-14 --standard c17
#
#   # 完整测试所有文件并生成报告
#   ./compiler_matrix.sh --compiler gcc-14 --standard c23 --report results.json --verbose
#
#   # 严格模式测试
#   ./compiler_matrix.sh --compiler clang-18 --standard c17 --strict
#
# 作者：C_Lang Knowledge Base Team
# 版本：2.0.0
# 最后更新：2026-03-17
# =============================================================================

# -----------------------------------------------------------------------------
# 严格模式设置
# -----------------------------------------------------------------------------
set -o pipefail  # 管道命令中只要有一个失败就返回失败

# -----------------------------------------------------------------------------
# 全局变量
# -----------------------------------------------------------------------------

# 脚本版本
readonly SCRIPT_VERSION="2.0.0"
readonly SCRIPT_NAME="compiler_matrix.sh"

# 默认配置
declare -A CONFIG=(
    [compiler]="gcc"
    [standard]="c17"
    [source_dir]="extracted_code/c"
    [output_dir]="test_output"
    [report_file]=""
    [jobs]=4
    [timeout]=30
    [strict]=false
    [performance]=false
    [verbose]=false
)

# 统计信息
declare -A STATS=(
    [total]=0
    [success]=0
    [failed]=0
    [skipped]=0
    [timeout]=0
    [total_time_ms]=0
)

# 编译器特性支持表
declare -A COMPILER_FEATURES=(
    ["gcc-13_c11"]=true
    ["gcc-13_c17"]=true
    ["gcc-13_c23"]=false
    ["gcc-14_c11"]=true
    ["gcc-14_c17"]=true
    ["gcc-14_c23"]=true
    ["gcc-15_c11"]=true
    ["gcc-15_c17"]=true
    ["gcc-15_c23"]=true
    ["clang-17_c11"]=true
    ["clang-17_c17"]=true
    ["clang-17_c23"]=false
    ["clang-18_c11"]=true
    ["clang-18_c17"]=true
    ["clang-18_c23"]=true
    ["clang-19_c11"]=true
    ["clang-19_c17"]=true
    ["clang-19_c23"]=true
)

# 颜色定义
readonly COLOR_RESET='\033[0m'
readonly COLOR_RED='\033[0;31m'
readonly COLOR_GREEN='\033[0;32m'
readonly COLOR_YELLOW='\033[1;33m'
readonly COLOR_BLUE='\033[0;34m'
readonly COLOR_CYAN='\033[0;36m'
readonly COLOR_BOLD='\033[1m'

# -----------------------------------------------------------------------------
# 日志函数
# -----------------------------------------------------------------------------

log_info() {
    printf "${COLOR_BLUE}[INFO]${COLOR_RESET} %s\n" "$1"
}

log_success() {
    printf "${COLOR_GREEN}[OK]${COLOR_RESET} %s\n" "$1"
}

log_warning() {
    printf "${COLOR_YELLOW}[WARN]${COLOR_RESET} %s\n" "$1" >&2
}

log_error() {
    printf "${COLOR_RED}[ERROR]${COLOR_RESET} %s\n" "$1" >&2
}

log_debug() {
    if [[ "${CONFIG[verbose]}" == true ]]; then
        printf "${COLOR_CYAN}[DEBUG]${COLOR_RESET} %s\n" "$1"
    fi
}

# -----------------------------------------------------------------------------
# 帮助信息
# -----------------------------------------------------------------------------

show_help() {
    cat << EOF
C_Lang 编译器矩阵测试脚本 v${SCRIPT_VERSION}

用法: ${SCRIPT_NAME} [选项]

选项:
    -c, --compiler COMPILER    指定编译器 (默认: gcc)
                               支持: gcc, gcc-13, gcc-14, gcc-15,
                                     clang, clang-17, clang-18, clang-19
    
    -s, --standard STANDARD    指定C标准 (默认: c17)
                               支持: c11, c17, c23
    
    -d, --source-dir DIR       源代码目录 (默认: extracted_code/c)
    
    -o, --output DIR           输出目录 (默认: test_output)
    
    -r, --report FILE          生成JSON格式报告文件
    
    -j, --jobs N               并行编译任务数 (默认: 4)
    
    -t, --timeout SECONDS      单个文件编译超时时间 (默认: 30)
    
    --strict                   启用严格模式
                               将更多警告视为错误
    
    --performance              启用性能测试
                               测量编译时间和生成的代码性能
    
    --verbose                  详细输出模式
    
    -h, --help                 显示此帮助信息
    
    -v, --version              显示版本信息

示例:
    # 基础用法
    ./${SCRIPT_NAME} --compiler gcc-14 --standard c17

    # 完整测试
    ./${SCRIPT_NAME} --compiler clang-18 --standard c23 --strict --report result.json --verbose

    # 多任务并行测试
    ./${SCRIPT_NAME} --compiler gcc-14 --jobs 8 --performance

退出码:
    0 - 所有测试通过
    1 - 部分测试失败
    2 - 配置错误
    3 - 编译器不可用
EOF
}

show_version() {
    echo "${SCRIPT_NAME} version ${SCRIPT_VERSION}"
    echo "Copyright (c) 2026 C_Lang Knowledge Base Team"
}

# -----------------------------------------------------------------------------
# 参数解析
# -----------------------------------------------------------------------------

parse_arguments() {
    while [[ $# -gt 0 ]]; do
        case $1 in
            -c|--compiler)
                CONFIG[compiler]="$2"
                shift 2
                ;;
            -s|--standard)
                CONFIG[standard]="$2"
                shift 2
                ;;
            -d|--source-dir)
                CONFIG[source_dir]="$2"
                shift 2
                ;;
            -o|--output)
                CONFIG[output_dir]="$2"
                shift 2
                ;;
            -r|--report)
                CONFIG[report_file]="$2"
                shift 2
                ;;
            -j|--jobs)
                CONFIG[jobs]="$2"
                shift 2
                ;;
            -t|--timeout)
                CONFIG[timeout]="$2"
                shift 2
                ;;
            --strict)
                CONFIG[strict]=true
                shift
                ;;
            --performance)
                CONFIG[performance]=true
                shift
                ;;
            --verbose)
                CONFIG[verbose]=true
                shift
                ;;
            -h|--help)
                show_help
                exit 0
                ;;
            -v|--version)
                show_version
                exit 0
                ;;
            *)
                log_error "未知选项: $1"
                show_help
                exit 2
                ;;
        esac
    done
}

# -----------------------------------------------------------------------------
# 验证配置
# -----------------------------------------------------------------------------

validate_config() {
    log_debug "验证配置..."
    
    # 验证编译器
    local compiler="${CONFIG[compiler]}"
    if ! command -v "$compiler" &> /dev/null; then
        log_error "编译器未找到: $compiler"
        log_info "请确保编译器已安装并在 PATH 中"
        exit 3
    fi
    
    # 验证标准
    local standard="${CONFIG[standard]}"
    if [[ ! "$standard" =~ ^c(11|17|23)$ ]]; then
        log_error "不支持的C标准: $standard"
        log_info "支持的标准: c11, c17, c23"
        exit 2
    fi
    
    # 验证源目录
    local source_dir="${CONFIG[source_dir]}"
    if [[ ! -d "$source_dir" ]]; then
        log_error "源目录不存在: $source_dir"
        exit 2
    fi
    
    # 检查编译器是否支持指定的标准
    local feature_key="${compiler}_${standard}"
    if [[ "${COMPILER_FEATURES[$feature_key]}" == false ]]; then
        log_warning "编译器 $compiler 可能不完全支持 $standard 标准"
        log_info "将继续测试，但可能会遇到兼容性问题"
    fi
    
    log_debug "配置验证通过"
}

# -----------------------------------------------------------------------------
# 获取编译器版本信息
# -----------------------------------------------------------------------------

get_compiler_version() {
    local compiler="${CONFIG[compiler]}"
    local version
    
    case "$compiler" in
        gcc*|gcc)
            version=$($compiler --version 2>/dev/null | head -1)
            ;;
        clang*|clang)
            version=$($compiler --version 2>/dev/null | head -1)
            ;;
        *)
            version="Unknown"
            ;;
    esac
    
    echo "$version"
}

# -----------------------------------------------------------------------------
# 构建编译命令
# -----------------------------------------------------------------------------

build_compile_command() {
    local source_file="$1"
    local output_file="$2"
    local compiler="${CONFIG[compiler]}"
    local standard="${CONFIG[standard]}"
    local strict="${CONFIG[strict]}"
    
    local cmd=($compiler)
    
    # 基本选项
    cmd+=(-std="$standard")
    cmd+=(-fsyntax-only)  # 仅语法检查，不生成可执行文件
    
    # 警告选项
    cmd+=(-Wall -Wextra -Wpedantic)
    
    # 严格模式额外选项
    if [[ "$strict" == true ]]; then
        cmd+=(-Werror)  # 所有警告视为错误
        cmd+=(-Werror=implicit-function-declaration)
        cmd+=(-Werror=return-type)
        cmd+=(-Werror=uninitialized)
        cmd+=(-Werror=unused-variable)
        cmd+=(-Werror=missing-prototypes)
        cmd+=(-Werror=strict-prototypes)
    else
        # 非严格模式下，关键警告仍视为错误
        cmd+=(-Werror=implicit-function-declaration)
        cmd+=(-Werror=return-type)
    fi
    
    # 特定标准的选项
    case "$standard" in
        c11)
            # C11 特定检查
            cmd+=(-Wc11-c2x-compat)
            ;;
        c17)
            # C17 特定检查
            ;;
        c23)
            # C23 特定选项
            # 注意：不同编译器对 C23 的支持程度不同
            ;;
    esac
    
    # 包含路径
    cmd+=(-I. -I"${CONFIG[source_dir]}")
    
    # 源文件
    cmd+=("$source_file")
    
    echo "${cmd[*]}"
}

# -----------------------------------------------------------------------------
# 性能测试函数
# -----------------------------------------------------------------------------

run_performance_test() {
    local source_file="$1"
    local compiler="${CONFIG[compiler]}"
    local standard="${CONFIG[standard]}"
    local temp_exe
    temp_exe=$(mktemp)
    
    # 编译为可执行文件（不使用 -fsyntax-only）
    local compile_cmd="$compiler -std=$standard -O2 -o $temp_exe $source_file"
    
    local start_time end_time compile_time
    start_time=$(date +%s%N)
    
    if eval "$compile_cmd" &>/dev/null; then
        end_time=$(date +%s%N)
        compile_time=$(( (end_time - start_time) / 1000000 ))  # 转换为毫秒
        
        # 获取可执行文件大小
        local exe_size
        exe_size=$(stat -f%z "$temp_exe" 2>/dev/null || stat -c%s "$temp_exe" 2>/dev/null || echo "0")
        
        echo "{\"compile_time_ms\": $compile_time, \"exe_size_bytes\": $exe_size}"
    else
        echo "{\"compile_time_ms\": -1, \"exe_size_bytes\": 0}"
    fi
    
    # 清理
    rm -f "$temp_exe"
}

# -----------------------------------------------------------------------------
# 测试单个文件
# -----------------------------------------------------------------------------

test_single_file() {
    local source_file="$1"
    local index="$2"
    local total="$3"
    
    local filename
    filename=$(basename "$source_file")
    
    log_debug "[$index/$total] 测试: $filename"
    
    # 检查文件大小（跳过过大的文件）
    local file_size
    file_size=$(stat -f%z "$source_file" 2>/dev/null || stat -c%s "$source_file" 2>/dev/null || echo "0")
    if [[ $file_size -gt 1048576 ]]; then  # 1MB
        log_warning "跳过超大文件: $filename (${file_size} bytes)"
        ((STATS[skipped]++))
        return 0
    fi
    
    # 构建编译命令
    local compile_cmd
    compile_cmd=$(build_compile_command "$source_file")
    
    # 执行编译（带超时）
    local start_time end_time elapsed
    start_time=$(date +%s%N)
    
    local output
    output=$(timeout "${CONFIG[timeout]}" bash -c "$compile_cmd 2>&1")
    local exit_code=$?
    
    end_time=$(date +%s%N)
    elapsed=$(( (end_time - start_time) / 1000000 ))
    
    ((STATS[total_time_ms] += elapsed))
    
    local result
    case $exit_code in
        0)
            result="SUCCESS"
            ((STATS[success]++))
            printf "  ${COLOR_GREEN}✓${COLOR_RESET} %-50s %6d ms\n" "$filename" "$elapsed"
            ;;
        124)
            result="TIMEOUT"
            ((STATS[timeout]++))
            printf "  ${COLOR_YELLOW}⏱${COLOR_RESET} %-50s ${COLOR_YELLOW}TIMEOUT${COLOR_RESET}\n" "$filename"
            ;;
        *)
            result="FAILED"
            ((STATS[failed]++))
            printf "  ${COLOR_RED}✗${COLOR_RESET} %-50s ${COLOR_RED}FAILED${COLOR_RESET}\n" "$filename"
            
            if [[ "${CONFIG[verbose]}" == true && -n "$output" ]]; then
                echo "    错误输出:"
                echo "$output" | head -5 | sed 's/^/      /'
            fi
            ;;
    esac
    
    # 性能测试
    local perf_data="{}"
    if [[ "${CONFIG[performance]}" == true && "$result" == "SUCCESS" ]]; then
        perf_data=$(run_performance_test "$source_file")
    fi
    
    # 返回结果
    echo "{\"file\": \"$filename\", \"result\": \"$result\", \"time_ms\": $elapsed, \"performance\": $perf_data}"
}

# -----------------------------------------------------------------------------
# 导出编译结果
# -----------------------------------------------------------------------------

export_compile_result() {
    local source_file="$1"
    local output_dir="${CONFIG[output_dir]}"
    local compiler="${CONFIG[compiler]}"
    local standard="${CONFIG[standard]}"
    
    # 创建输出子目录
    local result_dir="$output_dir/${compiler}_${standard}"
    mkdir -p "$result_dir"
    
    # 复制源文件
    cp "$source_file" "$result_dir/"
    
    echo "$result_dir/$(basename "$source_file")"
}

# -----------------------------------------------------------------------------
# 生成 JSON 报告
# -----------------------------------------------------------------------------

generate_report() {
    local report_file="${CONFIG[report_file]}"
    
    if [[ -z "$report_file" ]]; then
        return 0
    fi
    
    log_info "生成测试报告: $report_file"
    
    local total=$((STATS[total]))
    local success_rate=0
    if [[ $total -gt 0 ]]; then
        success_rate=$(echo "scale=2; ${STATS[success]} * 100 / $total" | bc -l 2>/dev/null || echo "0")
    fi
    
    local avg_time=0
    if [[ $total -gt 0 ]]; then
        avg_time=$((STATS[total_time_ms] / total))
    fi
    
    cat > "$report_file" << EOF
{
    "report_version": "${SCRIPT_VERSION}",
    "timestamp": "$(date -u +"%Y-%m-%dT%H:%M:%SZ")",
    "configuration": {
        "compiler": "${CONFIG[compiler]}",
        "compiler_version": "$(get_compiler_version)",
        "standard": "${CONFIG[standard]}",
        "source_dir": "${CONFIG[source_dir]}",
        "strict_mode": ${CONFIG[strict]},
        "performance_test": ${CONFIG[performance]},
        "parallel_jobs": ${CONFIG[jobs]},
        "timeout_seconds": ${CONFIG[timeout]}
    },
    "statistics": {
        "total_files": ${STATS[total]},
        "success": ${STATS[success]},
        "failed": ${STATS[failed]},
        "skipped": ${STATS[skipped]},
        "timeout": ${STATS[timeout]},
        "success_rate_percent": ${success_rate},
        "total_time_ms": ${STATS[total_time_ms]},
        "avg_time_per_file_ms": ${avg_time}
    },
    "summary": {
        "status": "$([[ ${STATS[failed]} -eq 0 ]] && echo "PASSED" || echo "FAILED")",
        "message": "${STATS[success]}/${STATS[total]} tests passed (${success_rate}% success rate)"
    }
}
EOF
    
    log_success "报告已生成: $report_file"
}

# -----------------------------------------------------------------------------
# 打印摘要
# -----------------------------------------------------------------------------

print_summary() {
    local total=$((STATS[total]))
    local success_rate=0
    
    if [[ $total -gt 0 ]]; then
        success_rate=$((STATS[success] * 100 / total))
    fi
    
    echo ""
    echo "================================================================================"
    echo "                           编译器矩阵测试摘要"
    echo "================================================================================"
    printf "  编译器:        ${COLOR_BOLD}%s${COLOR_RESET}\n" "$(get_compiler_version)"
    printf "  C 标准:        ${COLOR_BOLD}%s${COLOR_RESET}\n" "${CONFIG[standard]}"
    printf "  严格模式:      ${COLOR_BOLD}%s${COLOR_RESET}\n" "$([[ "${CONFIG[strict]}" == true ]] && echo "启用" || echo "禁用")"
    printf "  性能测试:      ${COLOR_BOLD}%s${COLOR_RESET}\n" "$([[ "${CONFIG[performance]}" == true ]] && echo "启用" || echo "禁用")"
    echo "--------------------------------------------------------------------------------"
    printf "  总文件数:      ${COLOR_BOLD}%4d${COLOR_RESET}\n" "$total"
    printf "  成功:          ${COLOR_GREEN}%4d${COLOR_RESET}\n" "${STATS[success]}"
    printf "  失败:          ${COLOR_RED}%4d${COLOR_RESET}\n" "${STATS[failed]}"
    printf "  跳过:          ${COLOR_YELLOW}%4d${COLOR_RESET}\n" "${STATS[skipped]}"
    printf "  超时:          ${COLOR_YELLOW}%4d${COLOR_RESET}\n" "${STATS[timeout]}"
    echo "--------------------------------------------------------------------------------"
    printf "  成功率:        ${COLOR_BOLD}%4d%%${COLOR_RESET}\n" "$success_rate"
    printf "  总耗时:        ${COLOR_BOLD}%4d ms${COLOR_RESET}\n" "${STATS[total_time_ms]}"
    printf "  平均耗时:      ${COLOR_BOLD}%4d ms${COLOR_RESET}\n" "$([[ $total -gt 0 ]] && echo $((STATS[total_time_ms] / total)) || echo "0")"
    echo "================================================================================"
    
    # 质量判断
    if [[ ${STATS[failed]} -eq 0 ]]; then
        echo ""
        log_success "🎉 所有测试通过！"
    elif [[ $success_rate -ge 90 ]]; then
        echo ""
        log_success "✓ 测试通过率良好 (${success_rate}%)"
    elif [[ $success_rate -ge 80 ]]; then
        echo ""
        log_warning "⚠ 测试通过率一般 (${success_rate}%)，建议检查失败项"
    else
        echo ""
        log_error "✗ 测试通过率较低 (${success_rate}%)，需要关注"
    fi
}

# -----------------------------------------------------------------------------
# 主函数
# -----------------------------------------------------------------------------

main() {
    # 解析参数
    parse_arguments "$@"
    
    # 验证配置
    validate_config
    
    # 打印标题
    echo "================================================================================"
    echo "              C_Lang 知识库编译器矩阵测试脚本 v${SCRIPT_VERSION}"
    echo "================================================================================"
    echo ""
    
    # 显示配置
    log_info "编译器: $(get_compiler_version)"
    log_info "C 标准: ${CONFIG[standard]}"
    log_info "源目录: ${CONFIG[source_dir]}"
    log_info "并行任务: ${CONFIG[jobs]}"
    echo ""
    
    # 创建输出目录
    mkdir -p "${CONFIG[output_dir]}"
    
    # 查找源文件
    local source_files=()
    while IFS= read -r -d '' file; do
        source_files+=("$file")
    done < <(find "${CONFIG[source_dir]}" -name "*.c" -type f -print0 2>/dev/null)
    
    local total_files=${#source_files[@]}
    
    if [[ $total_files -eq 0 ]]; then
        log_warning "未找到 C 源文件"
        exit 0
    fi
    
    log_info "找到 $total_files 个 C 源文件"
    echo ""
    
    # 初始化统计
    STATS[total]=$total_files
    
    # 执行测试
    echo "开始测试..."
    echo "--------------------------------------------------------------------------------"
    
    # 并行测试
    local pids=()
    local results=()
    local max_jobs="${CONFIG[jobs]}"
    
    for i in "${!source_files[@]}"; do
        local file="${source_files[$i]}"
        local idx=$((i + 1))
        
        # 限制并行数
        while [[ ${#pids[@]} -ge $max_jobs ]]; do
            local new_pids=()
            for pid in "${pids[@]}"; do
                if kill -0 "$pid" 2>/dev/null; then
                    new_pids+=("$pid")
                fi
            done
            pids=("${new_pids[@]}")
            [[ ${#pids[@]} -ge $max_jobs ]] && sleep 0.1
        done
        
        # 后台执行测试
        test_single_file "$file" "$idx" "$total_files" &
        pids+=($!)
    done
    
    # 等待所有后台任务完成
    wait
    
    echo "--------------------------------------------------------------------------------"
    
    # 生成报告
    generate_report
    
    # 打印摘要
    print_summary
    
    # 返回退出码
    if [[ ${STATS[failed]} -eq 0 ]]; then
        exit 0
    else
        exit 1
    fi
}

# -----------------------------------------------------------------------------
# 脚本入口
# -----------------------------------------------------------------------------

# 确保在正确的目录运行
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "$SCRIPT_DIR/.." && pwd)"

# 切换到项目根目录（如果需要）
if [[ -d "$PROJECT_ROOT/knowledge" ]]; then
    cd "$PROJECT_ROOT"
fi

# 运行主函数
main "$@"

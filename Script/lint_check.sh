#!/bin/bash

# RabbitRemoteControl Lint 检查脚本
# 支持 C++ (Clazy) 和 QML (qmllint) 检查

set -e  # 遇到错误立即退出

source $(dirname $(readlink -f $0))/common.sh

# 项目根目录（脚本所在目录的上级）
SCRIPT_DIR=$(dirname $(safe_readlink $0))
PROJECT_ROOT=$(safe_readlink $(dirname $(dirname $(safe_readlink $0))))

# 默认值
CHECK_CPP=true
CHECK_QML=false
FIX_MODE=false
VERBOSE=true
CLAZY_CHECKS="level2"  # 检查级别: level0, level1, level2
QML_IMPORT_PATH=""

# 解析命令行参数
usage() {
    echo "用法: $0 [选项]"
    echo "选项:"
    echo "  --no-cpp          跳过 C++ 检查"
    echo "  --no-qml          跳过 QML 检查"
    echo "  --fix             尝试自动修复（仅支持部分问题）"
    echo "  --clazy-level     设置 Clazy 检查级别 (level0/level1/level2, 默认: level2)"
    echo "  --qml-import-path QML 导入路径（多个路径用冒号分隔）"
    echo "  --verbose         显示详细输出"
    echo "  --help            显示此帮助信息"
    exit 0
}

while [[ $# -gt 0 ]]; do
    case $1 in
        --no-cpp) CHECK_CPP=false ;;
        --no-qml) CHECK_QML=false ;;
        --fix) FIX_MODE=true ;;
        --clazy-level) CLAZY_CHECKS="$2"; shift ;;
        --qml-import-path) QML_IMPORT_PATH="$2"; shift ;;
        --verbose) VERBOSE=true ;;
        --help) usage ;;
        *) echo "未知选项: $1"; usage ;;
    esac
    shift
done

# 检查必要工具
check_requirements() {
    local missing_tools=()
    
    if [[ "$CHECK_CPP" == true ]]; then
        if ! command -v clazy &> /dev/null; then
            missing_tools+=("clazy")
        fi
        if ! command -v cmake &> /dev/null && ! command -v qmake &> /dev/null; then
            missing_tools+=("cmake or qmake")
        fi
    fi
    
    if [[ "$CHECK_QML" == true ]]; then
        if ! command -v qmllint &> /dev/null; then
            missing_tools+=("qmllint")
        fi
    fi
    
    if [[ ${#missing_tools[@]} -gt 0 ]]; then
        log_error "Missing necessary tools: ${missing_tools[*]}"
        echo ""
        echo "Installation method:"
        echo "  Ubuntu/Debian: sudo apt install clazy qml6-tools cmake"
        echo "  macOS: brew install qt6"
        echo "  Or use the Qt installer to install Qt 6 + Qt Creator"
        exit 1
    fi
    
    log_success "All necessary tools have been installed"
}

# 查找 C++ 源文件
find_cpp_sources() {
    local src_dirs=(
        "$PROJECT_ROOT/Src"
        "$PROJECT_ROOT/Plugins"
        "$PROJECT_ROOT/App"
    )
    
    local sources=()
    for dir in "${src_dirs[@]}"; do
        if [[ -d "$dir" ]]; then
            while IFS= read -r file; do
                sources+=("$file")
            done < <(find "$dir" -type f \( -name "*.cpp" -o -name "*.cc" -o -name "*.cxx" -o -name "*.h" -o -name "*.hpp" \) 2>/dev/null || true)
        fi
    done
    
    printf '%s\n' "${sources[@]}"
}

# 查找 QML 文件
find_qml_sources() {
    local src_dirs=(
        "$PROJECT_ROOT/src"
        "$PROJECT_ROOT/qml"
        "$PROJECT_ROOT/resources"
    )
    
    local qml_files=()
    for dir in "${src_dirs[@]}"; do
        if [[ -d "$dir" ]]; then
            while IFS= read -r file; do
                qml_files+=("$file")
            done < <(find "$dir" -type f -name "*.qml" 2>/dev/null || true)
        fi
    done
    
    printf '%s\n' "${qml_files[@]}"
}

# C++ Lint 检查 (使用 Clazy)
run_cpp_lint() {
    echo_status "C++ Lint check (Clazy)"
    
    local cpp_sources=($(find_cpp_sources))
    if [[ ${#cpp_sources[@]} -eq 0 ]]; then
        log_warn "Don't find C++ source files, skip check"
        return 0
    fi
    
    log_info "Find ${#cpp_sources[@]} C++ source files"
    [[ "$VERBOSE" == true ]] && log_info "File list: ${cpp_sources[*]}"
    
    # 检查是否找到构建系统文件
    local build_dir="$PROJECT_ROOT/build-lint"
    mkdir -p "$build_dir"
    
    local errors_found=0
    local warnings_found=0
    
    # 尝试查找 CMakeLists.txt 或 .pro 文件
    if [[ -f "$PROJECT_ROOT/CMakeLists.txt" ]]; then
        echo_status "Use CMake build ......"
        export CLAZY_CHECKS="$CLAZY_CHECKS"
        
        cd "$build_dir"
        
        if [[ "$VERBOSE" == true ]]; then
            cmake "$PROJECT_ROOT" -DCMAKE_CXX_COMPILER=clazy -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
        else
            cmake "$PROJECT_ROOT" -DCMAKE_CXX_COMPILER=clazy -DCMAKE_EXPORT_COMPILE_COMMANDS=ON > /dev/null 2>&1
        fi
        
        # 运行 Clazy 检查（编译但不链接）
        if [[ "$VERBOSE" == true ]]; then
            make -k 2>&1 | tee clazy_output.log
        else
            make -k 2>&1 > clazy_output.log
        fi
        
        # 分析输出
        local clazy_errors=$(grep -c "error:" clazy_output.log 2>/dev/null || echo "0")
        local clazy_warnings=$(grep -c "warning:" clazy_output.log 2>/dev/null || echo "0")
        
        errors_found=$clazy_errors
        warnings_found=$clazy_warnings
        
        if [[ $errors_found -gt 0 ]] || [[ $warnings_found -gt 0 ]]; then
            echo ""
            log_warn "Clazy check to find issues:"
            grep -E "(error:|warning:)" clazy_output.log | head -20 || true
            if [[ $(grep -c -E "(error:|warning:)" clazy_output.log) -gt 20 ]]; then
                echo "... (There are more questions, see $build_dir/clazy_output.log)"
            fi
        fi
        
        cd "$PROJECT_ROOT"
        # 清理构建目录（可选）
        # rm -rf "$build_dir"
        
    elif [[ -f "$PROJECT_ROOT/RabbitRemoteControl.pro" ]]; then
        echo_status "Use qmake build ......"
        export CLAZY_CHECKS="$CLAZY_CHECKS"
        
        cd "$build_dir"
        
        if [[ "$VERBOSE" == true ]]; then
            qmake "$PROJECT_ROOT/RabbitRemoteControl.pro" QMAKE_CXX=clazy
        else
            qmake "$PROJECT_ROOT/RabbitRemoteControl.pro" QMAKE_CXX=clazy > /dev/null 2>&1
        fi
        
        if [[ "$VERBOSE" == true ]]; then
            make -k 2>&1 | tee clazy_output.log
        else
            make -k 2>&1 > clazy_output.log
        fi
        
        local clazy_errors=$(grep -c "error:" clazy_output.log 2>/dev/null || echo "0")
        local clazy_warnings=$(grep -c "warning:" clazy_output.log 2>/dev/null || echo "0")
        
        errors_found=$clazy_errors
        warnings_found=$clazy_warnings
        
        cd "$PROJECT_ROOT"
    else
        # 没有构建系统，进行简单的语法检查
        log_warn "Don't find CMakeLists.txt or .pro file, Perform a simple grammar check"
        
        for file in "${cpp_sources[@]}"; do
            if [[ "$file" =~ \.(cpp|cc|cxx)$ ]]; then
                if [[ "$VERBOSE" == true ]]; then
                    g++ -fsyntax-only -std=c++17 "$file" 2>&1 || true
                else
                    g++ -fsyntax-only -std=c++17 "$file" 2>&1 > /dev/null || true
                fi
            fi
        done
    fi
    
    echo_success "C++ Check completed"
    if [[ $errors_found -gt 0 ]]; then
        echo_error "Find $errors_found errors"
        return 1
    elif [[ $warnings_found -gt 0 ]]; then
        log_warn "Find $warnings_found warnings"
        return 0
    else
        echo_success "No issues found"
        return 0
    fi
}

# QML Lint 检查
run_qml_lint() {
    echo_status "QML Lint check (qmllint)"
    
    local qml_files=($(find_qml_sources))
    if [[ ${#qml_files[@]} -eq 0 ]]; then
        log_warn "QML file not found, skipping check"
        return 0
    fi
    
    log_info "Found ${#qml_files[@]}  QML files"
    [[ "$VERBOSE" == true ]] && log_info "File list: ${qml_files[*]}"
    
    local failed=0
    local lint_output=""
    
    # 构建 qmllint 命令
    local qmllint_cmd="qmllint"
    if [[ -n "$QML_IMPORT_PATH" ]]; then
        qmllint_cmd="$qmllint_cmd -I $QML_IMPORT_PATH"
    fi
    
    if [[ "$FIX_MODE" == true ]]; then
        qmllint_cmd="$qmllint_cmd --fix"
        log_info "Automatic repair mode is enabled"
    fi
    
    for qml_file in "${qml_files[@]}"; do
        local filename=$(basename "$qml_file")
        log_info "Check: $filename"
        
        if [[ "$VERBOSE" == true ]]; then
            $qmllint_cmd "$qml_file" 2>&1 || true
        else
            local output=$($qmllint_cmd "$qml_file" 2>&1)
            if [[ -n "$output" ]]; then
                echo "$output" | grep -v "Info:" | head -5 || true
                if echo "$output" | grep -q "error:"; then
                    failed=1
                fi
            fi
        fi
    done
    
    if [[ $failed -eq 0 ]]; then
        echo_success "All QML files passed the check"
        return 0
    else
        echo_error "Some QML files have problems"
        return 1
    fi
}

# 运行额外的 Qt 特定检查
run_qt_specific_checks() {
    echo_status "Qt specific check"
    
    local issues=0
    
    # 检查常见问题
    
    # 1. 检查重复的 tr() 调用
    echo_status "Check localization issues ......"
    local tr_issues=$(grep -r "tr(\"\")\"\")\|tr('')\"\")" "$PROJECT_ROOT" 2>/dev/null | wc -l || echo "0")
    if [[ $tr_issues -gt 0 ]]; then
        log_warn "Found $tr_issues potentially problematic tr() calls"
        issues=$((issues + 1))
    fi
    
    # 2. 检查未使用的 lambda 捕获
    echo_status "Check lambda caption ......"
    local lambda_issues=$(grep -r "\[&\]\|\[=\]" "$PROJECT_ROOT" --include="*.cpp" 2>/dev/null | wc -l || echo "0")
    if [[ $lambda_issues -gt 0 ]]; then
        log_warn "Found use of default capture mode [&] or [=] in $lambda_issues (explicit capture is recommended)"
    fi
    
    # 3. 检查内存泄漏风险 (使用 new 但不使用智能指针)
    echo_status "Check memory management ......"
    local raw_new=$(grep -r "new " "$PROJECT_ROOT" --include="*.cpp" 2>/dev/null | grep -v "QScopedPointer\|QSharedPointer\|std::unique_ptr\|std::shared_ptr" | wc -l || echo "0")
    if [[ $raw_new -gt 0 ]]; then
        log_warn "Found use of raw new at $raw_new (smart pointers are recommended)"
    fi
    
    # 4. 检查连接类型
    echo_status "Check signal-slot connection ......"
    local connect_issues=$(grep -r "connect(.*,.*,.*)" "$PROJECT_ROOT" --include="*.cpp" 2>/dev/null | grep -v "Qt::AutoConnection" | wc -l || echo "0")
    
    if [[ $issues -eq 0 ]]; then
        echo_success "Qt specific check passed"
    fi
}

# 生成报告
generate_report() {
    echo_status "Inspection Report"
    echo "Project: RabbitRemoteControl"
    echo "Check time: $(date)"
    echo "C++ check: $([ "$CHECK_CPP" == true ] && echo "Enable" || echo "Skip")"
    echo "QML check: $([ "$CHECK_QML" == true ] && echo "Enable" || echo "Skip")"
    echo "Repair Mode: $([ "$FIX_MODE" == true ] && echo "Enable" || echo "Disable")"
    echo "Clazy level: $CLAZY_CHECKS"
    echo ""
}

# 主函数
main() {
    echo_status "RabbitRemoteControl Lint check tool"
    echo "Project path: $PROJECT_ROOT"
    
    # 切换到项目根目录
    cd "$PROJECT_ROOT"
    
    # 检查依赖
    check_requirements
    
    # 生成报告头部
    generate_report
    
    local exit_code=0
    
    # 运行 C++ 检查
    if [[ "$CHECK_CPP" == true ]]; then
        if ! run_cpp_lint; then
            exit_code=1
        fi
    fi
    
    # 运行 QML 检查
    if [[ "$CHECK_QML" == true ]]; then
        if ! run_qml_lint; then
            exit_code=1
        fi
    fi
    
    # 运行额外检查
    run_qt_specific_checks
    
    # 最终结果
    echo_status "Summary"
    if [[ $exit_code -eq 0 ]]; then
        echo_success "All checks passed!"
    else
        echo_error "A problem was detected. Please fix it and try again. "
    fi
    
    exit $exit_code
}

# 执行主函数
main "$@"

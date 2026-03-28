# Hack and RISC-V Assembler Makefile
# ==================================
#
# 使用方法:
#   make              - 编译所有汇编器
#   make hack         - 只编译Hack汇编器
#   make riscv        - 只编译RISC-V汇编器
#   make test         - 运行所有测试
#   make test-hack    - 只运行Hack测试
#   make test-riscv   - 只运行RISC-V测试
#   make clean        - 清理生成的文件
#
# 配套文档: knowledge/22_Assembler_Implementation/*.md

CC = gcc
CFLAGS = -Wall -Wextra -O2 -std=c99
DEBUG_FLAGS = -g -DDEBUG

# 目标文件
HACK_ASM = hack_assembler
RISCV_ASM = riscv_assembler

# 默认目标
.PHONY: all hack riscv test test-hack test-riscv clean help

all: hack riscv
	@echo "========================================"
	@echo "Build complete!"
	@echo "  Hack assembler:    ./$(HACK_ASM)"
	@echo "  RISC-V assembler:  ./$(RISCV_ASM)"
	@echo "========================================"

hack: $(HACK_ASM)

riscv: $(RISCV_ASM)

$(HACK_ASM): hack_assembler.c
	$(CC) $(CFLAGS) -o $@ $<
	@echo "Built: $@"

$(RISCV_ASM): riscv_assembler.c
	$(CC) $(CFLAGS) -o $@ $<
	@echo "Built: $@"

# 调试版本
debug: CFLAGS += $(DEBUG_FLAGS)
debug: all

# 测试
test: test-hack test-riscv
	@echo "========================================"
	@echo "All tests completed!"
	@echo "========================================"

test-hack: hack
	@echo ""
	@echo "========================================"
	@echo "Running Hack Assembler Tests"
	@echo "========================================"
	@mkdir -p test_output
	@echo ""
	@echo "Test 1: Add.asm (1+2+...+100)"
	./$(HACK_ASM) tests/Add.asm -o test_output/Add.hack -v
	@echo ""
	@echo "Test 2: Max.asm (求最大值)"
	./$(HACK_ASM) tests/Max.asm -o test_output/Max.hack -v
	@echo ""
	@echo "Test 3: Rect.asm (绘制矩形)"
	./$(HACK_ASM) tests/Rect.asm -o test_output/Rect.hack -v
	@echo ""
	@echo "Test 4: Pong.asm (乒乓球游戏)"
	./$(HACK_ASM) tests/Pong.asm -o test_output/Pong.hack -v
	@echo ""
	@echo "Test 5: 符号表测试"
	./$(HACK_ASM) tests/SymbolTest.asm -s -o test_output/SymbolTest.hack
	@echo ""
	@echo "Hack tests completed. Check test_output/ directory."

test-riscv: riscv
	@echo ""
	@echo "========================================"
	@echo "Running RISC-V Assembler Tests"
	@echo "========================================"
	@mkdir -p test_output
	@echo ""
	@echo "Test 1: Basic arithmetic (test1.s)"
	./$(RISCV_ASM) tests/test1.s -o test_output/test1.txt -t
	@echo ""
	@echo "Test 2: Load/Store (test2.s)"
	./$(RISCV_ASM) tests/test2.s -o test_output/test2.txt -t
	@echo ""
	@echo "Test 3: Branch and jump (test3.s)"
	./$(RISCV_ASM) tests/test3.s -o test_output/test3.txt -t -s
	@echo ""
	@echo "Test 4: Pseudo instructions (test4.s)"
	./$(RISCV_ASM) tests/test4.s -o test_output/test4.txt -t
	@echo ""
	@echo "Test 5: Function call example (test5.s)"
	./$(RISCV_ASM) tests/test5.s -o test_output/test5.txt -t -s
	@echo ""
	@echo "RISC-V tests completed. Check test_output/ directory."

# 清理
clean:
	rm -f $(HACK_ASM) $(RISCV_ASM)
	rm -rf test_output
	rm -f tests/*.hack tests/*.bin tests/*.txt
	@echo "Cleaned up build files."

# 帮助
help:
	@echo "Hack and RISC-V Assembler Build System"
	@echo ""
	@echo "Targets:"
	@echo "  make           - Build all assemblers"
	@echo "  make hack      - Build only Hack assembler"
	@echo "  make riscv     - Build only RISC-V assembler"
	@echo "  make debug     - Build with debug symbols"
	@echo "  make test      - Run all tests"
	@echo "  make test-hack - Run Hack assembler tests"
	@echo "  make test-riscv- Run RISC-V assembler tests"
	@echo "  make clean     - Remove built files"
	@echo "  make help      - Show this help"
	@echo ""
	@echo "Usage Examples:"
	@echo "  ./hack_assembler program.asm"
	@echo "  ./hack_assembler program.asm -o output.hack -v"
	@echo "  ./riscv_assembler program.s -o output.bin"
	@echo "  ./riscv_assembler program.s -t -o output.txt"

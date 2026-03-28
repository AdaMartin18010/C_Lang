#!/usr/bin/env python3
"""
图灵机模拟器
============
配套文档: knowledge/21_Computation_Theory/01_Turing_Machines.md

本模拟器用于直观理解图灵机的工作原理。
图灵机是计算理论的基础模型，理解它有助于建立从理论到物理实现的认知桥梁。

使用方法:
    python tm_simulator.py

作者: AI Code Review
创建: 2026-03-29
"""

from typing import Dict, Tuple, Optional


class TuringMachine:
    """
    图灵机模拟器
    
    实现了一个通用的图灵机模型，支持：
    - 任意数量的状态和符号
    - 可配置的转移函数
    - 详细的执行跟踪
    
    图灵机定义: M = (Q, Σ, Γ, δ, q₀, q_accept, q_reject)
    """
    
    def __init__(
        self,
        transitions: Dict[Tuple[str, str], Tuple[str, str, str]],
        start_state: str,
        accept_state: str,
        reject_state: str,
        blank_symbol: str = '□'
    ):
        """
        初始化图灵机
        
        Args:
            transitions: 转移函数 δ
                        键: (当前状态, 读到的符号)
                        值: (新状态, 写入符号, 移动方向)
            start_state: 起始状态 q₀
            accept_state: 接受状态
            reject_state: 拒绝状态
            blank_symbol: 空白符号（默认为 '□'）
        """
        self.transitions = transitions
        self.start_state = start_state
        self.accept_state = accept_state
        self.reject_state = reject_state
        self.blank = blank_symbol
        
    def run(
        self,
        input_string: str,
        max_steps: int = 1000,
        verbose: bool = False
    ) -> Tuple[str, int, str]:
        """
        运行图灵机
        
        Args:
            input_string: 输入字符串（磁带初始内容）
            max_steps: 最大执行步数（防止无限循环）
            verbose: 是否打印详细执行过程
            
        Returns:
            (结果, 执行步数, 最终磁带内容)
            结果: 'accept' | 'reject' | 'timeout'
        """
        # 初始化磁带
        if input_string:
            tape = list(input_string)
        else:
            tape = [self.blank]
        
        head = 0  # 磁头位置
        state = self.start_state  # 当前状态
        steps = 0
        
        if verbose:
            print("=" * 60)
            print("图灵机执行开始")
            print("=" * 60)
            print(f"初始配置:\n{self._config_str(tape, head, state)}")
            print()
        
        while steps < max_steps:
            # 扩展磁带（如果磁头超出边界）
            if head < 0:
                tape.insert(0, self.blank)
                head = 0
            elif head >= len(tape):
                tape.append(self.blank)
            
            # 读取当前符号
            symbol = tape[head]
            key = (state, symbol)
            
            # 查找转移
            if key not in self.transitions:
                # 无转移，隐式拒绝
                if verbose:
                    print(f"步骤 {steps}: 状态={state}, 符号={symbol}")
                    print(f"  无定义转移，隐式拒绝")
                return ('reject', steps, ''.join(tape).strip(self.blank))
            
            new_state, write_symbol, direction = self.transitions[key]
            
            # 执行转移
            tape[head] = write_symbol
            state = new_state
            
            if direction == 'R':
                head += 1
            elif direction == 'L':
                head -= 1
            else:
                raise ValueError(f"无效移动方向: {direction}")
            
            steps += 1
            
            if verbose:
                print(f"步骤 {steps}:")
                print(f"  转移: ({key[0]}, {key[1]}) → ({new_state}, {write_symbol}, {direction})")
                print(f"{self._config_str(tape, head, state)}")
                print()
            
            # 检查是否停机
            if state == self.accept_state:
                if verbose:
                    print(f"到达接受状态 '{self.accept_state}'，计算完成 ✓")
                    print(f"总步数: {steps}")
                return ('accept', steps, ''.join(tape).strip(self.blank))
            elif state == self.reject_state:
                if verbose:
                    print(f"到达拒绝状态 '{self.reject_state}'，计算结束 ✗")
                    print(f"总步数: {steps}")
                return ('reject', steps, ''.join(tape).strip(self.blank))
        
        # 超时
        if verbose:
            print(f"达到最大步数限制 ({max_steps})，计算超时")
        return ('timeout', steps, ''.join(tape).strip(self.blank))
    
    def _config_str(self, tape: list, head: int, state: str) -> str:
        """
        生成配置字符串的可视化表示
        
        配置格式: u q v
        其中 u 是磁头左侧，q 是状态，v 是磁头位置及右侧
        """
        tape_str = ''.join(tape)
        # 去掉尾部的空白符号以便显示
        tape_str = tape_str.rstrip(self.blank)
        if not tape_str:
            tape_str = self.blank
        
        # 磁头指针
        pointer = ' ' * head + '↑'
        
        # 状态信息
        state_info = f"状态: {state}"
        
        return f"磁带: {tape_str}\n      {pointer}\n{state_info}"


# ==================== 预定义图灵机 ====================

def create_zero_checker() -> TuringMachine:
    """
    创建判断全0的图灵机
    
    如果输入字符串全是'0'，则接受；否则拒绝。
    
    转移函数:
        δ(q₀, 0) = (q₀, 0, R)      // 是0，继续右移
        δ(q₀, 1) = (q_reject, 1, R) // 是1，拒绝
        δ(q₀, □) = (q_accept, □, R) // 空白，接受（全是0）
    """
    transitions = {
        ('q0', '0'): ('q0', '0', 'R'),
        ('q0', '1'): ('q_reject', '1', 'R'),
        ('q0', '□'): ('q_accept', '□', 'R'),
    }
    return TuringMachine(
        transitions,
        start_state='q0',
        accept_state='q_accept',
        reject_state='q_reject'
    )


def create_palindrome_checker() -> TuringMachine:
    """
    创建判断回文的图灵机
    
    判断二进制字符串是否为回文（正读反读相同）。
    使用标记法：将已匹配的符号替换为X。
    
    算法:
    1. 记住第一个符号（0或1），标记为X
    2. 移动到最右端，检查是否匹配
    3. 如果匹配，标记为X，返回最左端
    4. 重复直到所有符号都被标记
    """
    transitions = {
        # === 开始：读取第一个符号并记住 ===
        ('q0', '0'): ('q_find_0', 'X', 'R'),  # 是0，去找最右的0
        ('q0', '1'): ('q_find_1', 'X', 'R'),  # 是1，去找最右的1
        ('q0', 'X'): ('q_accept', 'X', 'R'),  # 全是X，接受
        ('q0', '□'): ('q_accept', '□', 'R'),  # 空输入，接受
        
        # === 找最后一个0 ===
        ('q_find_0', '0'): ('q_find_0', '0', 'R'),
        ('q_find_0', '1'): ('q_find_0', '1', 'R'),
        ('q_find_0', 'X'): ('q_find_0', 'X', 'R'),
        ('q_find_0', '□'): ('q_check_0', '□', 'L'),
        
        # 检查最右是否是0
        ('q_check_0', '0'): ('q_return', 'X', 'L'),  # 是0，标记，返回
        ('q_check_0', '1'): ('q_reject', '1', 'L'),  # 是1，不匹配
        ('q_check_0', 'X'): ('q_reject', 'X', 'L'),  # 全是X了，但之前没接受，说明奇数长度错误
        
        # === 找最后一个1 ===
        ('q_find_1', '0'): ('q_find_1', '0', 'R'),
        ('q_find_1', '1'): ('q_find_1', '1', 'R'),
        ('q_find_1', 'X'): ('q_find_1', 'X', 'R'),
        ('q_find_1', '□'): ('q_check_1', '□', 'L'),
        
        # 检查最右是否是1
        ('q_check_1', '0'): ('q_reject', '0', 'L'),
        ('q_check_1', '1'): ('q_return', 'X', 'L'),  # 是1，标记，返回
        ('q_check_1', 'X'): ('q_reject', 'X', 'L'),
        
        # === 返回到最左边的非X符号 ===
        ('q_return', '0'): ('q_return', '0', 'L'),
        ('q_return', '1'): ('q_return', '1', 'L'),
        ('q_return', 'X'): ('q_return', 'X', 'L'),
        ('q_return', '□'): ('q0', '□', 'R'),  # 回到开始，继续
    }
    return TuringMachine(
        transitions,
        start_state='q0',
        accept_state='q_accept',
        reject_state='q_reject'
    )


def create_binary_incrementer() -> TuringMachine:
    """
    创建二进制加1的图灵机
    
    将二进制数加1。例如：101 (5) → 110 (6)
    
    算法：从右向左，遇到0变1停止，遇到1变0继续（进位）
    """
    transitions = {
        # 移动到最右端
        ('q0', '0'): ('q0', '0', 'R'),
        ('q0', '1'): ('q0', '1', 'R'),
        ('q0', '□'): ('q_add', '□', 'L'),
        
        # 加1操作
        ('q_add', '0'): ('q_done', '1', 'L'),  # 遇到0，变1，完成
        ('q_add', '1'): ('q_add', '0', 'L'),   # 遇到1，变0，继续进位
        ('q_add', '□'): ('q_done', '1', 'L'),  # 溢出，在最前面加1
        
        # 完成，返回到最左端（可选，为了美观）
        ('q_done', '0'): ('q_done', '0', 'L'),
        ('q_done', '1'): ('q_done', '1', 'L'),
        ('q_done', '□'): ('q_accept', '□', 'R'),
    }
    return TuringMachine(
        transitions,
        start_state='q0',
        accept_state='q_accept',
        reject_state='q_reject'
    )


# ==================== 测试与演示 ====================

def test_zero_checker():
    """测试全0检查器"""
    print("\n" + "=" * 60)
    print("测试1: 全0检查器")
    print("=" * 60)
    print("功能：判断输入是否全是'0'")
    print()
    
    tm = create_zero_checker()
    
    test_cases = [
        ("000", True, "三个0"),
        ("010", False, "包含1"),
        ("", True, "空输入"),
        ("111", False, "全是1"),
        ("0000000000", True, "十个0"),
    ]
    
    for input_str, expected, description in test_cases:
        result, steps, tape = tm.run(input_str, verbose=False)
        passed = (result == 'accept') == expected
        status = "✓" if passed else "✗"
        expected_str = "接受" if expected else "拒绝"
        actual_str = "接受" if result == 'accept' else "拒绝"
        
        print(f"  {status} 输入 '{input_str}' ({description}):")
        print(f"      预期: {expected_str}, 实际: {actual_str} ({steps}步)")
    
    # 详细演示
    print("\n  详细演示输入 '000':")
    tm.run("000", verbose=True)


def test_palindrome_checker():
    """测试回文检查器"""
    print("\n" + "=" * 60)
    print("测试2: 回文检查器")
    print("=" * 60)
    print("功能：判断二进制字符串是否为回文")
    print()
    
    tm = create_palindrome_checker()
    
    test_cases = [
        ("0110", True, "偶数长度回文"),
        ("010", True, "奇数长度回文"),
        ("1", True, "单个字符"),
        ("", True, "空字符串"),
        ("0101", False, "非回文"),
        ("01100", False, "非回文"),
    ]
    
    for input_str, expected, description in test_cases:
        result, steps, tape = tm.run(input_str, verbose=False)
        passed = (result == 'accept') == expected
        status = "✓" if passed else "✗"
        expected_str = "接受" if expected else "拒绝"
        actual_str = "接受" if result == 'accept' else "拒绝"
        
        print(f"  {status} 输入 '{input_str}' ({description}):")
        print(f"      预期: {expected_str}, 实际: {actual_str} ({steps}步)")
    
    # 详细演示
    print("\n  详细演示输入 '0110':")
    tm.run("0110", verbose=True)


def test_binary_incrementer():
    """测试二进制加1"""
    print("\n" + "=" * 60)
    print("测试3: 二进制加1")
    print("=" * 60)
    print("功能：将二进制数加1")
    print()
    
    tm = create_binary_incrementer()
    
    test_cases = [
        ("101", "110", "5 + 1 = 6"),
        ("111", "1000", "7 + 1 = 8"),
        ("0", "1", "0 + 1 = 1"),
        ("1", "10", "1 + 1 = 2"),
        ("1000", "1001", "8 + 1 = 9"),
    ]
    
    for input_str, expected_output, description in test_cases:
        result, steps, tape = tm.run(input_str, verbose=False)
        passed = tape == expected_output
        status = "✓" if passed else "✗"
        
        print(f"  {status} 输入 '{input_str}' ({description}):")
        print(f"      输出: '{tape}', 预期: '{expected_output}' ({steps}步)")
    
    # 详细演示
    print("\n  详细演示输入 '111' (7+1=8):")
    tm.run("111", verbose=True)


def demonstrate_universal_computation():
    """
    演示通用计算的概念
    
    说明：图灵机可以模拟其他图灵机（通用图灵机）
    这是现代计算机的理论基础。
    """
    print("\n" + "=" * 60)
    print("概念演示：图灵机的通用性")
    print("=" * 60)
    print()
    print("图灵机的一个重要性质是'通用性'：")
    print("  存在一种特殊的图灵机，称为'通用图灵机'，")
    print("  它可以模拟任何其他图灵机的执行。")
    print()
    print("这正是现代计算机的基础：")
    print("  - CPU = 通用图灵机（硬件）")
    print("  - 程序 = 被模拟的图灵机描述（软件）")
    print("  - 输入数据 = 被模拟图灵机的输入")
    print()
    print("本模拟器中的 TuringMachine 类就是一个通用图灵机：")
    print("  - 通过传递不同的 transitions 参数，")
    print("  - 它可以表现为 zero_checker、palindrome_checker 等")
    print("  - 这正是'存储程序计算机'的概念！")
    print()


# ==================== 主程序 ====================

if __name__ == "__main__":
    print("\n" + "=" * 70)
    print(" " * 15 + "图灵机模拟器")
    print("=" * 70)
    print()
    print("配套文档: knowledge/21_Computation_Theory/01_Turing_Machines.md")
    print("用途: 理解计算理论的基础模型")
    print()
    
    # 运行测试
    test_zero_checker()
    test_palindrome_checker()
    test_binary_incrementer()
    
    # 概念演示
    demonstrate_universal_computation()
    
    # 总结
    print("\n" + "=" * 70)
    print("总结")
    print("=" * 70)
    print()
    print("通过以上示例，您应该理解了：")
    print("  1. 图灵机的基本结构（状态、磁带、转移函数）")
    print("  2. 如何设计和实现简单的图灵机")
    print("  3. 图灵机的通用性（一个机器可以模拟其他机器）")
    print()
    print("下一步：")
    print("  - 尝试修改上述图灵机实现新功能")
    print("  - 思考：图灵机如何映射到物理计算机？")
    print("  - 阅读文档中的'与现有知识库的连接'部分")
    print()

#!/usr/bin/env python3
"""
λ演算解释器（简化版）
====================
配套文档: knowledge/21_Computation_Theory/02_Lambda_Calculus.md

本解释器演示λ演算的核心概念：
- 变量、抽象、应用
- β规约
- Church编码（自然数）

使用方法:
    python lambda_eval.py

作者: AI Code Review
创建: 2026-03-29
"""

from dataclasses import dataclass
from typing import Union, Optional


# ==================== AST定义 ====================

@dataclass(frozen=True)
class Var:
    """变量"""
    name: str
    
    def __repr__(self):
        return self.name

@dataclass(frozen=True)
class Abs:
    """λ抽象: λparam.body"""
    param: str
    body: 'Expr'
    
    def __repr__(self):
        return f"(λ{self.param}.{self.body})"

@dataclass(frozen=True)
class App:
    """应用: func arg"""
    func: 'Expr'
    arg: 'Expr'
    
    def __repr__(self):
        return f"({self.func} {self.arg})"

Expr = Union[Var, Abs, App]


# ==================== 操作 ====================

def substitute(expr: Expr, var: str, replacement: Expr) -> Expr:
    """
    替换: expr[var := replacement]
    将expr中所有自由的var替换为replacement
    """
    if isinstance(expr, Var):
        return replacement if expr.name == var else expr
    
    elif isinstance(expr, Abs):
        if expr.param == var:
            # 变量被绑定，不替换
            return expr
        else:
            # 替换body中的自由变量
            return Abs(expr.param, substitute(expr.body, var, replacement))
    
    elif isinstance(expr, App):
        return App(
            substitute(expr.func, var, replacement),
            substitute(expr.arg, var, replacement)
        )


def beta_reduce_once(expr: Expr) -> Optional[Expr]:
    """
    执行一次β规约
    如果可以规约，返回新的表达式
    否则返回None
    """
    if isinstance(expr, App):
        if isinstance(expr.func, Abs):
            # (λx.M) N → M[x := N]
            return substitute(expr.func.body, expr.func.param, expr.arg)
        else:
            # 尝试规约函数部分
            new_func = beta_reduce_once(expr.func)
            if new_func is not None:
                return App(new_func, expr.arg)
            
            # 尝试规约参数部分
            new_arg = beta_reduce_once(expr.arg)
            if new_arg is not None:
                return App(expr.func, new_arg)
    
    return None


def eval_expr(expr: Expr, max_steps: int = 100, verbose: bool = False) -> Expr:
    """
    求值：多步β规约直到范式
    
    返回:
        无法继续规约的表达式（范式或达到步数限制）
    """
    if verbose:
        print(f"  0: {expr}")
    
    for i in range(max_steps):
        new_expr = beta_reduce_once(expr)
        if new_expr is None:
            if verbose:
                print(f"  → 范式（{i}步）")
            return expr
        expr = new_expr
        if verbose:
            print(f"  {i+1}: {expr}")
    
    if verbose:
        print(f"  → 达到步数限制")
    return expr


# ==================== Church编码 ====================

def church_num(n: int) -> Expr:
    """
    将Python整数n转换为Church数
    
    Church数n = λf.λx.f^n x
    即：应用函数f共n次
    """
    # 从x开始
    body: Expr = Var("x")
    # 嵌套n次f
    for _ in range(n):
        body = App(Var("f"), body)
    # 包装成 λf.λx.body
    return Abs("f", Abs("x", body))


def church_succ() -> Expr:
    """后继函数: SUCC = λn.λf.λx.f (n f x)"""
    return Abs("n", Abs("f", Abs("x",
        App(
            Var("f"),
            App(App(Var("n"), Var("f")), Var("x"))
        )
    )))


def church_add() -> Expr:
    """加法: ADD = λm.λn.λf.λx.m f (n f x)"""
    return Abs("m", Abs("n", Abs("f", Abs("x",
        App(
            App(Var("m"), Var("f")),
            App(App(Var("n"), Var("f")), Var("x"))
        )
    ))))


def church_mul() -> Expr:
    """乘法: MUL = λm.λn.λf.m (n f)"""
    return Abs("m", Abs("n", Abs("f",
        App(Var("m"), App(Var("n"), Var("f")))
    )))


def church_true() -> Expr:
    """TRUE = λx.λy.x"""
    return Abs("x", Abs("y", Var("x")))


def church_false() -> Expr:
    """FALSE = λx.λy.y"""
    return Abs("x", Abs("y", Var("y")))


def church_if() -> Expr:
    """IF = λb.λt.λf.b t f"""
    return Abs("b", Abs("t", Abs("f",
        App(App(Var("b"), Var("t")), Var("f"))
    )))


# ==================== 测试 ====================

def test_identity():
    """测试恒等函数"""
    print("\n" + "=" * 60)
    print("测试1: 恒等函数 (λx.x)")
    print("=" * 60)
    
    id_func = Abs("x", Var("x"))
    
    # (λx.x) y → y
    test1 = App(id_func, Var("y"))
    print(f"\n表达式: {test1}")
    result = eval_expr(test1, verbose=True)
    print(f"预期: y")
    print(f"结果: {result}")
    print(f"✓ 通过" if str(result) == "y" else "✗ 失败")
    
    # (λx.x) ((λx.x) z) → z
    test2 = App(id_func, App(id_func, Var("z")))
    print(f"\n表达式: {test2}")
    result = eval_expr(test2, verbose=True)
    print(f"预期: z")
    print(f"结果: {result}")
    print(f"✓ 通过" if str(result) == "z" else "✗ 失败")


def test_church_numbers():
    """测试Church数"""
    print("\n" + "=" * 60)
    print("测试2: Church数")
    print("=" * 60)
    
    for n in range(4):
        cn = church_num(n)
        print(f"\n{n} = {cn}")


def test_successor():
    """测试后继函数"""
    print("\n" + "=" * 60)
    print("测试3: 后继函数 SUCC")
    print("=" * 60)
    
    succ = church_succ()
    print(f"SUCC = {succ}")
    
    # SUCC 1 → 2
    one = church_num(1)
    test = App(succ, one)
    print(f"\nSUCC 1:")
    print(f"表达式: {test}")
    result = eval_expr(test, verbose=True)
    expected = church_num(2)
    print(f"预期:   2 = λf.λx.f (f x)")
    print(f"结果:   {result}")


def test_addition():
    """测试加法"""
    print("\n" + "=" * 60)
    print("测试4: 加法 1 + 2")
    print("=" * 60)
    
    add = church_add()
    one = church_num(1)
    two = church_num(2)
    
    # ADD 1 2
    test = App(App(add, one), two)
    print(f"表达式: {test}")
    
    # 为了验证结果，我们应用到一个具体的函数
    # (ADD 1 2) (λx.x+1) 0 应该等于 3
    
    print("\n规约过程:")
    result = eval_expr(test, verbose=True)
    print(f"\n结果形式: {result}")
    print("(这是一个Church数，将函数应用3次)")


def test_boolean():
    """测试布尔值"""
    print("\n" + "=" * 60)
    print("测试5: Church布尔值")
    print("=" * 60)
    
    true = church_true()
    false = church_false()
    if_func = church_if()
    
    print(f"TRUE  = {true}")
    print(f"FALSE = {false}")
    print(f"IF    = {if_func}")
    
    # IF TRUE a b → a
    test1 = App(App(App(if_func, true), Var("a")), Var("b"))
    print(f"\nIF TRUE a b:")
    print(f"表达式: {test1}")
    result = eval_expr(test1, verbose=True)
    print(f"预期: a")
    print(f"结果: {result}")
    
    # IF FALSE a b → b
    test2 = App(App(App(if_func, false), Var("a")), Var("b"))
    print(f"\nIF FALSE a b:")
    print(f"表达式: {test2}")
    result = eval_expr(test2, verbose=True)
    print(f"预期: b")
    print(f"结果: {result}")


def demonstrate_omega():
    """演示Ω组合子（无限循环）"""
    print("\n" + "=" * 60)
    print("演示: Ω组合子（无限循环）")
    print("=" * 60)
    
    # ω = λx.x x
    omega = Abs("x", App(Var("x"), Var("x")))
    
    # Ω = ω ω = (λx.x x) (λx.x x)
    Omega = App(omega, omega)
    
    print(f"ω = {omega}")
    print(f"Ω = ω ω = {Omega}")
    print("\n尝试规约（限制10步）:")
    result = eval_expr(Omega, max_steps=10, verbose=True)
    print("\nΩ永不终止！这正是图灵机的无限循环")


# ==================== 主程序 ====================

if __name__ == "__main__":
    print("\n" + "=" * 70)
    print(" " * 20 + "λ演算解释器")
    print("=" * 70)
    print()
    print("配套文档: knowledge/21_Computation_Theory/02_Lambda_Calculus.md")
    print()
    
    test_identity()
    test_church_numbers()
    test_successor()
    test_addition()
    test_boolean()
    demonstrate_omega()
    
    print("\n" + "=" * 70)
    print("总结")
    print("=" * 70)
    print()
    print("通过以上测试，您应该理解了：")
    print("  1. λ演算的语法（变量、抽象、应用）")
    print("  2. β规约的原理（函数应用即替换）")
    print("  3. Church编码（用函数表示数据）")
    print("  4. 无限循环的构造（Ω组合子）")
    print()
    print("与图灵机的联系：")
    print("  - 图灵机：状态 + 磁带操作（命令式）")
    print("  - λ演算：函数 + 应用（函数式）")
    print("  - 两者等价：描述同一类计算！")
    print()

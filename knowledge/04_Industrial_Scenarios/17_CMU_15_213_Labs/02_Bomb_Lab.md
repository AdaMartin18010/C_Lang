# Lab 2: Bomb Lab - 二进制炸弹实验

## 1. 实验目标

### 1.1 总体目标

Bomb Lab通过逆向工程一个"二进制炸弹"程序来学习汇编语言和程序分析。炸弹包含多个phase，每个phase需要输入特定字符串才能解除，错误输入会"引爆炸弹"。

| 目标 | 描述 |
|------|------|
| **汇编阅读** | 阅读和理解x86-64汇编代码 |
| **逆向工程** | 分析程序逻辑而不看源代码 |
| **调试技巧** | 掌握GDB调试工具的使用 |
| **程序分析** | 理解编译器生成的代码 |

### 1.2 实验设置

```
炸弹结构:
┌─────────────────┐
│   phase_1       │ 字符串比较
├─────────────────┤
│   phase_2       │ 循环/数组
├─────────────────┤
│   phase_3       │ 条件分支/开关
├─────────────────┤
│   phase_4       │ 递归
├─────────────────┤
│   phase_5       │ 指针/结构
├─────────────────┤
│   phase_6       │ 链表/排序
├─────────────────┤
│   secret_phase  │ 隐藏关卡
└─────────────────┘
```

---

## 2. 背景知识

### 2.1 x86-64汇编基础

```asm
; 基本指令
mov     dst, src        ; 数据传送
add/sub dst, src        ; 加减
imul    dst, src        ; 乘法
idiv    divisor         ; 除法 (商在rax, 余数在rdx)

; 比较和跳转
cmp     a, b            ; 比较a和b
je/jz   label           ; 等于则跳转
jne/jnz label           ; 不等于则跳转
jg/jl   label           ; 大于/小于跳转
jge/jle label           ; 大于等于/小于等于跳转

; 函数调用
call    function        ; 调用函数
ret                     ; 返回
push    val             ; 压栈
pop     val             ; 出栈
```

### 2.2 调用约定（System V AMD64 ABI）

```asm
; 参数传递寄存器
%rdi    ; 第1个参数
%rsi    ; 第2个参数
%rdx    ; 第3个参数
%rcx    ; 第4个参数
%r8     ; 第5个参数
%r9     ; 第6个参数

; 返回值
%rax    ; 函数返回值

; 调用者保存寄存器
%rax, %rcx, %rdx, %rsi, %rdi, %r8-%r11

; 被调用者保存寄存器
%rbx, %rbp, %r12-%r15
```

### 2.3 GDB调试命令

```bash
# 启动和运行
gdb ./bomb                    # 启动gdb
run [args] / r                # 运行程序
quit / q                      # 退出

# 断点
break *0x4005f6 / b           # 在地址设置断点
break phase_1                 # 在函数设置断点
break 15                      # 在行号设置断点
info breakpoints / i b        # 查看断点
delete 1                      # 删除断点1

# 执行
stepi / si                    # 单步执行（进入函数）
nexti / ni                    # 单步执行（跳过函数）
continue / c                  # 继续执行
finish                        # 运行到函数返回

# 查看
print /x $rax / p/x           # 以十六进制打印寄存器
x/s $rsi                      # 打印字符串
x/4wx $rsp                    # 打印栈上4个字
x/10i $pc                     # 打印10条指令
disas phase_1 / disa          # 反汇编函数

# 信息
info registers / i r          # 查看所有寄存器
info frame / i f              # 查看栈帧
backtrace / bt                # 查看调用栈
```

---

## 3. 各Phase完整解析

### 3.1 Phase 1: 字符串比较

#### 反汇编代码

```asm
0000000000400ee0 <phase_1>:
  400ee0:   48 83 ec 08             sub    $0x8,%rsp
  400ee4:   be 00 24 40 00          mov    $0x402400,%esi    ; 目标字符串地址
  400ee9:   e8 4a 04 00 00          callq  401338 <strings_not_equal>
  400eee:   85 c0                   test   %eax,%eax
  400ef0:   74 05                   je     400ef7 <phase_1+0x17>
  400ef2:   e8 43 05 00 00          callq  40143a <explode_bomb>
  400ef7:   48 83 c4 08             add    $0x8,%rsp
  400efb:   c3                      retq
```

#### 分析过程

```bash
# 在phase_1设置断点
(gdb) break phase_1
(gdb) run

# 查看比较的字符串
(gdb) x/s 0x402400
"Border relations with Canada have never been better."
```

#### 解答

```
Border relations with Canada have never been better.
```

#### 关键知识点

1. `strings_not_equal`函数比较两个字符串
2. `%esi`存储了目标字符串的地址
3. 输入字符串通过`%rdi`传递

---

### 3.2 Phase 2: 循环与数组

#### 反汇编代码

```asm
0000000000400efc <phase_2>:
  400efc:   55                      push   %rbp
  400efd:   53                      push   %rbx
  400efe:   48 83 ec 28             sub    $0x28,%rsp
  400f02:   48 89 e6                mov    %rsp,%rsi           ; rsi = 栈指针（存储数组）
  400f05:   e8 52 05 00 00          callq  40145c <read_six_numbers>

  400f0a:   83 3c 24 01             cmpl   $0x1,(%rsp)         ; 比较第一个数是否为1
  400f0e:   74 20                   je     400f30 <phase_2+0x34>
  400f10:   e8 25 05 00 00          callq  40143a <explode_bomb>

  400f15:   eb 19                   jmp    400f30 <phase_2+0x34>

  400f17:   8b 43 fc                mov    -0x4(%rbx),%eax     ; eax = 前一个数
  400f1a:   01 c0                   add    %eax,%eax           ; eax *= 2
  400f1c:   39 03                   cmp    %eax,(%rbx)         ; 比较当前数是否等于前一个*2
  400f1e:   74 05                   je     400f25 <phase_2+0x29>
  400f20:   e8 15 05 00 00          callq  40143a <explode_bomb>

  400f25:   48 83 c3 04             add    $0x4,%rbx           ; rbx指向下一个数
  400f29:   48 39 eb                cmp    %rbp,%rbx           ; 检查是否结束
  400f2c:   75 e9                   jne    400f17 <phase_2+0x1b>
  400f2e:   eb 0c                   jmp    400f3c <phase_2+0x40>

  400f30:   48 8d 5c 24 04          lea    0x4(%rsp),%rbx      ; rbx = 第2个数的地址
  400f35:   48 8d 6c 24 18          lea    0x18(%rsp),%rbp     ; rbp = 结束地址（第7个）
  400f3a:   eb db                   jmp    400f17 <phase_2+0x1b>

  400f3c:   48 83 c4 28             add    $0x28,%rsp
  400f40:   5b                      pop    %rbx
  400f41:   5d                      pop    %rbp
  402f42:   c3                      retq
```

#### 分析过程

```bash
# 分析逻辑
# 1. 读取6个数字
# 2. 第一个数必须是1
# 3. 后续每个数是前一个的2倍

# 预期序列: 1, 2, 4, 8, 16, 32
```

#### 解答

```
1 2 4 8 16 32
```

#### 关键知识点

1. `read_six_numbers`读取6个整数到数组
2. 数组存储在栈上
3. 循环结构：rbx遍历数组，rbp是结束标记
4. 每个元素是前一个的两倍（等比数列）

---

### 3.3 Phase 3: 条件分支与开关语句

#### 反汇编代码

```asm
0000000000400f43 <phase_3>:
  400f43:   48 83 ec 18             sub    $0x18,%rsp
  400f47:   48 8d 4c 24 0c          lea    0xc(%rsp),%rcx      ; arg3: &num2
  400f4c:   48 8d 54 24 08          lea    0x8(%rsp),%rdx      ; arg2: &num1
  400f51:   be cf 25 40 00          mov    $0x4025cf,%esi      ; format string: "%d %d"
  400f56:   b8 00 00 00 00          mov    $0x0,%eax
  400f5b:   e8 90 fc ff ff          callq  400bf0 <__isoc99_sscanf@plt>
  400f60:   83 f8 01                cmp    $0x1,%eax
  400f63:   7f 05                   jg     400f6a <phase_3+0x27>
  400f65:   e8 d0 04 00 00          callq  40143a <explode_bomb>

  400f6a:   83 7c 24 08 07          cmpl   $0x7,0x8(%rsp)      ; num1 <= 7?
  400f6f:   77 3c                   ja     400fad <phase_3+0x6a>  ; 无符号大于则爆炸

  400f71:   8b 44 24 08             mov    0x8(%rsp),%eax      ; eax = num1
  400f75:   ff 24 c5 70 24 40 00    jmpq   *0x402470(,%rax,8)  ; 跳转表

  400f7c:   b8 cf 00 00 00          mov    $0xcf,%eax          ; case 0: 207
  400f81:   eb 3b                   jmp    400fbe <phase_3+0x7b>

  400f83:   b8 c3 02 00 00          mov    $0x2c3,%eax         ; case 2: 707
  400f88:   eb 34                   jmp    400fbe <phase_3+0x7b>

  400f8a:   b8 00 01 00 00          mov    $0x100,%eax         ; case 3: 256
  400f8f:   eb 2d                   jmp    400fbe <phase_3+0x7b>

  400f91:   b8 85 01 00 00          mov    $0x185,%eax         ; case 4: 389
  400f96:   eb 26                   jmp    400fbe <phase_3+0x7b>

  400f98:   b8 ce 00 00 00          mov    $0xce,%eax          ; case 5: 206
  400f9d:   eb 1f                   jmp    400fbe <phase_3+0x7b>

  400f9f:   b8 aa 02 00 00          mov    $0x2aa,%eax         ; case 6: 682
  400fa4:   eb 18                   jmp    400fbe <phase_3+0x7b>

  400fa6:   b8 47 01 00 00          mov    $0x147,%eax         ; case 7: 327
  400fab:   eb 11                   jmp    400fbe <phase_3+0x7b>

  400fad:   e8 88 04 00 00          callq  40143a <explode_bomb>
  400fb2:   b8 00 00 00 00          mov    $0x0,%eax
  400fb7:   eb 05                   jmp    400fbe <phase_3+0x7b>

  400fb9:   b8 37 01 00 00          mov    $0x137,%eax         ; case 1: 311
  400fbe:   3b 44 24 0c             cmp    0xc(%rsp),%eax      ; 比较num2
  400fc2:   74 05                   je     400fc9 <phase_3+0x86>
  400fc4:   e8 71 04 00 00          callq  40143a <explode_bomb>
  400fc9:   48 83 c4 18             add    $0x18,%rsp
  400fcd:   c3                      retq
```

#### 跳转表分析

```bash
# 查看跳转表
(gdb) x/8gx 0x402470
0x402470:   0x0000000000400f7c   # case 0
0x402478:   0x0000000000400fb9   # case 1
0x402480:   0x0000000000400f83   # case 2
0x402488:   0x0000000000400f8a   # case 3
0x402490:   0x0000000000400f91   # case 4
0x402498:   0x0000000000400f98   # case 5
0x4024a0:   0x0000000000400f9f   # case 6
0x4024a8:   0x0000000000400fa6   # case 7
```

#### case值表

| case | num1 | num2 (期望) |
|------|------|-------------|
| 0    | 0    | 207 (0xCF)  |
| 1    | 1    | 311 (0x137) |
| 2    | 2    | 707 (0x2C3) |
| 3    | 3    | 256 (0x100) |
| 4    | 4    | 389 (0x185) |
| 5    | 5    | 206 (0xCE)  |
| 6    | 6    | 682 (0x2AA) |
| 7    | 7    | 327 (0x147) |

#### 解答

任意一对都可以，例如：

```
0 207
```

或

```
7 327
```

---

### 3.4 Phase 4: 递归函数

#### 反汇编代码

```asm
0000000000400fce <phase_4>:
  400fce:   48 83 ec 18             sub    $0x18,%rsp
  400fd2:   48 8d 4c 24 0c          lea    0xc(%rsp),%rcx
  400fd7:   48 8d 54 24 08          lea    0x8(%rsp),%rdx
  400fdc:   be cf 25 40 00          mov    $0x4025cf,%esi      ; "%d %d"
  400fe1:   b8 00 00 00 00          mov    $0x0,%eax
  400fe6:   e8 05 fc ff ff          callq  400bf0 <__isoc99_sscanf@plt>
  400feb:   83 f8 02                cmp    $0x2,%eax
  400fee:   75 07                   jne    400ff7 <phase_4+0x29>
  400ff0:   83 7c 24 08 0e          cmpl   $0xe,0x8(%rsp)      ; num1 <= 14
  400ff5:   76 05                   jbe    400ffc <phase_4+0x2e>
  400ff7:   e8 3e 04 00 00          callq  40143a <explode_bomb>

  400ffc:   ba 0e 00 00 00          mov    $0xe,%edx           ; arg3: 14
  401001:   be 00 00 00 00          mov    $0x0,%esi           ; arg2: 0
  401006:   8b 7c 24 08             mov    0x8(%rsp),%edi      ; arg1: num1
  40100a:   e8 81 ff ff ff          callq  400f90 <func4>
  40100f:   85 c0                   test   %eax,%eax           ; 返回值必须为0
  401011:   75 07                   jne    40101a <phase_4+0x4c>
  401013:   83 7c 24 0c 00          cmpl   $0x0,0xc(%rsp)      ; num2 == 0
  401018:   74 05                   je     40101f <phase_4+0x51>
  40101a:   e8 1b 04 00 00          callq  40143a <explode_bomb>
  40101f:   48 83 c4 18             add    $0x18,%rsp
  401023:   c3                      retq

; func4 - 递归函数
0000000000400f90 <func4>:
  400f90:   53                      push   %rbx
  400f91:   89 fb                   mov    %edi,%ebx           ; ebx = x
  400f93:   89 f0                   mov    %esi,%eax           ; eax = y
  400f95:   89 f1                   mov    %esi,%ecx
  400f97:   29 f9                   sub    %edi,%ecx           ; ecx = y - x
  400f99:   89 c7                   mov    %eax,%edi           ; edi = y
  400f9b:   c1 e9 1f                shr    $0x1f,%ecx          ; ecx = (y-x) >> 31
  400f9e:   01 c8                   add    %ecx,%eax           ; eax = y + ((y-x)>>31)
  400fa0:   d1 f8                   sar    %eax                ; eax = (y + ((y-x)>>31)) / 2
  400fa2:   8d 0c 00                lea    (%rax,%rax,1),%ecx  ; ecx = 2 * mid
  400fa5:   01 f9                   add    %edi,%ecx           ; ecx = x + 2*mid
  400fa7:   c1 e9 02                shr    $0x2,%ecx           ; ecx = (x + 2*mid) / 4
  400faa:   01 c1                   add    %eax,%ecx           ;
  400fac:   89 ca                   mov    %ecx,%edx           ; edx = mid
  400fae:   39 da                   cmp    %ebx,%edx           ; compare mid with target
  400fb0:   7e 17                   jle    400fc9 <func4+0x39>
  ; mid > target, 递归调用 func4(x, mid-1)
  400fb2:   8d 51 ff                lea    -0x1(%rcx),%edx
  400fb5:   89 54 24 08             mov    %edx,0x8(%rsp)
  400fb9:   89 d9                   mov    %ebx,%ecx
  400fbb:   e8 d0 ff ff ff          callq  400f90 <func4>
  400fc0:   01 c0                   add    %eax,%eax
  400fc2:   eb 15                   jmp    400fd9 <func4+0x49>

  400fc9:   b8 00 00 00 00          mov    $0x0,%eax           ; eax = 0
  400fce:   39 da                   cmp    %ebx,%edx
  400fd0:   7d 07                   jge    400fd9 <func4+0x49> ; mid == target, return 0
  ; mid < target, 递归调用 func4(mid+1, y)
  400fd2:   8d 51 01                lea    0x1(%rcx),%edx
  400fd5:   89 d9                   mov    %ebx,%ecx
  400fd7:   e8 b4 ff ff ff          callq  400f90 <func4>
  400fdc:   8d 44 00 01             lea    0x1(%rax,%rax,1),%eax

  400fe0:   5b                      pop    %rbx
  400fe1:   c3                      retq
```

#### func4分析

func4是一个二分查找的变体：

```c
int func4(int target, int low, int high) {
    int mid = (low + high) / 2;
    if (mid > target)
        return 2 * func4(target, low, mid - 1);
    if (mid < target)
        return 2 * func4(target, mid + 1, high) + 1;
    return 0;  // found
}
```

#### 解答

需要找到使func4返回0的num1，且num2=0：

通过分析，num1 = 7 时满足条件：

```
7 0
```

---

### 3.5 Phase 5: 指针与数组

#### 反汇编代码

```asm
0000000000401062 <phase_5>:
  401062:   53                      push   %rbx
  401063:   48 83 ec 20             sub    $0x20,%rsp
  401067:   48 89 fb                mov    %rdi,%rbx           ; rbx = input string
  40106a:   64 48 8b 04 25 28 00    mov    %fs:0x28,%rax       ; stack canary
  401071:   00 00
  401073:   48 89 44 24 18          mov    %rax,0x18(%rsp)
  401078:   31 c0                   xor    %eax,%eax
  40107a:   e8 9c 02 00 00          callq  40131b <string_length>
  40107f:   83 f8 06                cmp    $0x6,%eax           ; length must be 6
  401082:   74 4e                   je     4010d2 <phase_5+0x70>
  401084:   e8 b1 03 00 00          callq  40143a <explode_bomb>

  401089:   eb 47                   jmp    4010d2 <phase_5+0x70>

  ; loop processing each character
  40108b:   0f b6 0c 03             movzbl (%rbx,%rax,1),%ecx  ; ecx = input[i]
  40108f:   88 0c 24                mov    %cl,(%rsp)
  401092:   48 8b 14 24             mov    (%rsp),%rdx
  401096:   83 e2 0f                and    $0xf,%edx           ; edx = input[i] & 0xf
  401099:   0f b6 92 78 24 40 00    movzbl 0x402478(%rdx),%edx ; lookup in table
  4010a0:   88 54 04 10             mov    %dl,0x10(%rsp,%rax,1) ; store result
  4010a4:   48 83 c0 01             add    $0x1,%rax
  4010a8:   48 83 f8 06             cmp    $0x6,%rax
  4010ac:   75 dd                   jne    40108b <phase_5+0x29>

  4010ae:   c6 44 24 16 00          movb   $0x0,0x16(%rsp)     ; null terminator
  4010b3:   be 5e 24 40 00          mov    $0x40245e,%esi      ; target string "flyers"
  4010b8:   48 8d 7c 24 10          lea    0x10(%rsp),%rdi     ; our constructed string
  4010bd:   e8 76 02 00 00          callq  401338 <strings_not_equal>
  4010c2:   85 c0                   test   %eax,%eax
  4010c4:   74 13                   je     4010d9 <phase_5+0x77>
  4010c6:   e8 6f 03 00 00          callq  40143a <explode_bomb>

  4010cb:   0f 1f 44 00 00          nopl   0x0(%rax,%rax,1)
  4010d0:   eb 07                   jmp    4010d9 <phase_5+0x77>
  4010d2:   b8 00 00 00 00          mov    $0x0,%eax
  4010d7:   eb b2                   jmp    40108b <phase_5+0x29>
  4010d9:   48 8b 44 24 18          mov    0x18(%rsp),%rax
  4010de:   64 48 33 04 25 28 00    xor    %fs:0x28,%rax       ; check canary
  4010e5:   00 00
  4010e7:   74 05                   je     4010ee <phase_5+0x8c>
  4010e9:   e8 42 fa ff ff          callq  400b30 <__stack_chk_fail@plt>
  4010ee:   48 83 c4 20             add    $0x20,%rsp
  4010f2:   5b                      pop    %rbx
  4010f3:   c3                      retq
```

#### 查找表分析

```bash
(gdb) x/16c 0x402478
0x402478:   'm' 'a' 'd' 'u' 'i' 'e' 'r' 's'
0x402480:   'n' 'f' 'o' 't' 'v' 'b' 'y' 'l'

; Index:  0   1   2   3   4   5   6   7   8   9   10  11  12  13  14  15
; Char:   m   a   d   u   i   e   r   s   n   f   o   t   v   b   y   l
```

#### 目标字符串

```bash
(gdb) x/s 0x40245e
"flyers"
```

#### 解答推导

需要从查找表中找出得到"flyers"的索引：

| 目标字符 | 索引 | 可能的ASCII |
|---------|------|------------|
| 'f'     | 9    | 'I' (0x49), 'Y' (0x59), 'i' (0x69), 'y' (0x79) |
| 'l'     | 15   | 'O' (0x4F), '_' (0x5F), 'o' (0x6F) |
| 'y'     | 14   | 'N' (0x4E), '^' (0x5E), 'n' (0x6E) |
| 'e'     | 5    | 'E' (0x45), 'U' (0x55), 'e' (0x65), 'u' (0x75) |
| 'r'     | 6    | 'F' (0x46), 'V' (0x56), 'f' (0x66), 'v' (0x76) |
| 's'     | 7    | 'G' (0x47), 'W' (0x57), 'g' (0x67), 'w' (0x77) |

一个有效的输入是：`9?>EF7`（使用不同的ASCII低4位组合）

更常见的解答（使用字母）：

```
ionefg
```

或

```
YONEFG
```

---

### 3.6 Phase 6: 链表与排序

#### 反汇编代码（简化）

```asm
00000000004010f4 <phase_6>:
  4010f4:   41 56                   push   %r14
  4010f6:   41 55                   push   %r13
  4010f8:   41 54                   push   %r12
  4010fa:   55                      push   %rbp
  4010fb:   53                      push   %rbx
  4010fc:   48 83 ec 50             sub    $0x50,%rsp
  401100:   49 89 e5                mov    %rsp,%r13           ; array pointer
  401103:   48 89 e6                mov    %rsp,%rsi           ; arg for read_six_numbers
  401106:   e8 51 03 00 00          callq  40145c <read_six_numbers>

  ; 检查输入是否在1-6范围内且互不相同
  40110b:   49 89 e6                mov    %rsp,%r14           ; outer loop ptr
  40110e:   41 bc 00 00 00 00       mov    $0x0,%r12d          ; loop counter
  401114:   4c 89 ed                mov    %r13,%rbp           ; inner loop ptr
  401117:   41 8b 45 00             mov    0x0(%r13),%eax      ; load number
  40111b:   83 e8 01                sub    $0x1,%eax
  40111e:   83 f8 05                cmp    $0x5,%eax           ; must be <= 6
  401121:   76 05                   jbe    401128 <phase_6+0x34>
  401123:   e8 12 03 00 00          callq  40143a <explode_bomb>
  401128:   41 83 c4 01             add    $0x1,%r12d
  40112c:   41 83 fc 06             cmp    $0x6,%r12d
  401130:   74 21                   je     401153 <phase_6+0x5f>
  401132:   44 89 e3                mov    %r12d,%ebx
  401135:   48 63 c3                movslq %ebx,%rax
  401138:   8b 04 84                mov    (%rsp,%rax,4),%eax
  40113b:   39 45 00                cmp    %eax,0x0(%rbp)      ; check uniqueness
  40113e:   75 05                   jne    401145 <phase_6+0x51>
  401140:   e8 f5 02 00 00          callq  40143a <explode_bomb>
  401145:   83 c3 01                add    $0x1,%ebx
  401148:   83 fb 05                cmp    $0x5,%ebx
  40114b:   7e e8                   jle    401135 <phase_6+0x41>
  40114d:   49 83 c5 04             add    $0x4,%r13
  401151:   eb c1                   jmp    401114 <phase_6+0x20>

  ; 转换为7-x
  401153:   48 8d 74 24 18          lea    0x18(%rsp),%rsi
  401158:   4c 89 f0                mov    %r14,%rax
  40115b:   b9 07 00 00 00          mov    $0x7,%ecx
  401160:   89 ca                   mov    %ecx,%edx
  401162:   2b 10                   sub    (%rax),%edx         ; 7 - num[i]
  401164:   89 10                   mov    %edx,(%rax)
  401166:   48 83 c0 04             add    $0x4,%rax
  40116a:   48 39 f0                cmp    %rsi,%rax
  40116d:   75 f1                   jne    401160 <phase_6+0x6c>

  ; 根据转换后的值重新排列链表节点
  ; ... (复杂链表操作)

  ; 检查链表是否按降序排列
  4011e5:   48 8b 5b 08             mov    0x8(%rbx),%rbx      ; next node
  4011e9:   48 8b 45 00             mov    0x0(%rbp),%rax
  4011ed:   8b 00                   mov    (%rax),%eax
  4011ef:   39 03                   cmp    %eax,(%rbx)         ; compare values
  4011f1:   7d 05                   jge    4011f8 <phase_6+0x104>
  4011f3:   e8 42 02 00 00          callq  40143a <explode_bomb>
  4011f8:   48 8b 5d 08             mov    0x8(%rbp),%rbp
  4011fc:   48 89 dd                mov    %rbx,%rbp
  4011ff:   48 8b 5b 08             mov    0x8(%rbx),%rbx
  401203:   48 85 db                test   %rbx,%rbx
  401206:   75 dd                   jne    4011e5 <phase_6+0xf1>
```

#### 链表节点数据

```bash
# 查看链表节点
(gdb) x/3gx 0x6032d0
0x6032d0 <node1>:   0x000000010000014c   0x00000000006032e0
0x6032e0 <node2>:   0x00000002000000a8   0x00000000006032f0

# 节点值（低32位）
node1: 0x14c = 332
node2: 0xa8 = 168
node3: 0x39c = 924
node4: 0x2b3 = 691
node5: 0x1dd = 477
node6: 0x1bb = 443
```

#### 解答推导

1. 输入1-6的排列
2. 转换为7-x
3. 按转换后的值索引链表节点
4. 节点值必须降序排列

节点值降序：924(3), 691(4), 477(5), 443(6), 332(1), 168(2)

输入序列（7-x）：4 3 2 1 6 5

```
4 3 2 1 6 5
```

---

## 4. Secret Phase

### 4.1 触发条件

在phase_defused函数中检查：

```bash
(gdb) disas phase_defused
# 检查输入中是否有额外字符串
```

### 4.2 解答

在特定phase的输入后添加特定字符串触发secret_phase。

---

## 5. GDB调试技巧总结

### 5.1 常用调试流程

```bash
# 1. 启动并设置断点
gdb ./bomb
(gdb) break phase_1

# 2. 运行到断点
(gdb) run

# 3. 单步执行查看寄存器
(gdb) stepi
(gdb) info registers

# 4. 查看内存
(gdb) x/s $rsi          # 查看字符串
(gdb) x/4wx $rsp        # 查看栈

# 5. 继续执行
(gdb) continue
```

### 5.2 自动化脚本

```bash
# bomb.gdb
set pagination off
break phase_1
break phase_2
break phase_3
break phase_4
break phase_5
break phase_6
run
```

---

**创建日期**: 2026-03-19


---

## 深入理解

### 核心原理

深入探讨技术原理和实现细节。

### 实践应用

- 应用场景1
- 应用场景2
- 应用场景3

### 最佳实践

1. 理解基础概念
2. 掌握核心机制
3. 应用到实际项目

---

> **最后更新**: 2026-03-21  
> **维护者**: AI Code Review

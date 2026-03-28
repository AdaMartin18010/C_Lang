/*
 * C4 - C in four functions
 * ========================
 * 配套文档: knowledge/23_VM_Implementation/02_C_Subset_Interpreter.md
 *
 * 这是一个极简的C语言解释器，仅使用4个主要函数实现：
 * - next(): 词法分析
 * - expr(): 表达式解析
 * - stmt(): 语句解析  
 * - main(): 主循环
 *
 * 支持C的子集：
 * - 4种类型: int, char, int*, char*
 * - 16个关键字: if, else, while, return, printf, etc.
 * - 基本表达式和语句
 *
 * 原项目: https://github.com/rswier/c4
 * 本版本添加了详细中文注释，用于教学目的
 *
 * 编译: gcc -Wall -Wextra -std=c99 -o c4 c4.c
 * 运行: ./c4 test_program.c
 */

#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <string.h>
#include <ctype.h>

/* 内存池 */
int *mem;           /* 数据段 */
int *e;             /* 当前 emit 位置 */
int *le;            /* 当前行的 emit 位置 */
int *text;          /* 代码段 */
int *data;          /* 数据段基址 */
int *sp;            /* 栈指针 */
int *bp;            /* 基址指针 */
int *pc;            /* 程序计数器 */

/* 词法分析状态 */
int tk;             /* 当前 token */
int ival;           /* 整数值 */
int *id;            /* 当前标识符 */
int ty;             /* 当前类型 */
int loc;            /* 局部变量偏移 */
int line;           /* 当前行号 */
int src;            /* 打印源代码标志 */
int debug;          /* 调试标志 */

char *p;            /* 源代码指针 */

/* tokens 和 types */
enum {
    Num = 128, Fun, Sys, Glo, Loc, Id,
    Char, Else, Enum, If, Int, Return, Sizeof, While,
    Assign, Cond, Lor, Lan, Or, Xor, And, Eq, Ne, Lt, Gt, Le, Ge, Shl, Shr, Add, Sub, Mul, Div, Mod, Inc, Dec, Brak
};

/* 操作码 */
enum { LEA ,IMM ,JMP ,CALL,JZ ,JNZ,ENT ,ADJ ,LEV ,LI ,LC ,SI ,SC ,PSH,
       OR ,XOR,AND ,EQ ,NE ,LT ,GT ,LE ,GE ,SHL,SHR,ADD,SUB,MUL,DIV,MOD,
       OPEN,READ,CLOS,PRTF,MALC,FREE,MSET,MCMP,EXIT };

/* 标识符结构 */
/* id[0]: tk, id[1]: hash, id[2]: name, id[3]: type/class, id[4]: value/addr */
int *idmain;        /* main函数指针 */

/* 符号表（简化版） */
int *sym;           /* 符号表基址 */
int *ids;           /* 符号表当前位置 */
int *ide;           /* 符号表结束 */

/* 获取下一个token（词法分析器） */
void next()
{
    char *pp;
    
    while ((tk = *p)) {
        ++p;
        
        /* 换行 */
        if (tk == '\n') {
            if (src) {
                /* 打印当前行的指令 */
                printf("%d: %.*s", line, p - (char*)le, le);
                le = (int*)p;
                while (e > le) {
                    printf("%8.4s", &"LEA ,IMM ,JMP ,CALL,JZ ,JNZ,ENT ,ADJ ,LEV ,LI ,LC ,SI ,SC ,PSH,"
                           "OR ,XOR,AND ,EQ ,NE ,LT ,GT ,LE ,GE ,SHL,SHR,ADD,SUB,MUL,DIV,MOD,"
                           "OPEN,READ,CLOS,PRTF,MALC,FREE,MSET,MCMP,EXIT,"[*++le * 5]);
                    if (*le <= ADJ) printf(" %d\n", *++le); else printf("\n");
                }
            }
            ++line;
        }
        /* 关键字或标识符 */
        else if ((tk >= 'a' && tk <= 'z') || (tk >= 'A' && tk <= 'Z') || tk == '_') {
            pp = p - 1;
            while ((*p >= 'a' && *p <= 'z') || (*p >= 'A' && *p <= 'Z') || 
                   (*p >= '0' && *p <= '9') || *p == '_')
                ++p;
            
            /* 简单的hash */
            int h = (tk * 147) + *(p-1);
            
            /* 在符号表中查找 */
            id = sym;
            while (id[0]) {
                if (id[1] == h && !memcmp((char*)id[2], pp, p - pp)) {
                    tk = id[0];
                    return;
                }
                id = id + 5;
            }
            
            /* 新标识符 */
            id[0] = Id;
            id[1] = h;
            id[2] = (int)pp;
            id[3] = 0;
            id[4] = 0;
            tk = Id;
            return;
        }
        /* 数字 */
        else if (tk >= '0' && tk <= '9') {
            ival = tk - '0';
            while (*p >= '0' && *p <= '9') {
                ival = ival * 10 + (*p++ - '0');
            }
            tk = Num;
            return;
        }
        /* 字符串 */
        else if (tk == '"' || tk == '\'') {
            pp = (char*)data;
            while (*p != tk) {
                if ((ival = *p++) == '\\') {
                    if ((ival = *p++) == 'n') ival = '\n';
                }
                *data++ = ival;
            }
            ++p;
            *data++ = 0;
            ival = (int)pp;
            tk = Num;
            return;
        }
        /* 注释 */
        else if (tk == '/') {
            if (*p == '/') {
                while (*p != 0 && *p != '\n') ++p;
            }
            else {
                tk = Div;
                return;
            }
        }
        /* 操作符 */
        else if (tk == '=') { if (*p == '=') { ++p; tk = Eq; } else tk = Assign; return; }
        else if (tk == '+') { if (*p == '+') { ++p; tk = Inc; } else tk = Add; return; }
        else if (tk == '-') { if (*p == '-') { ++p; tk = Dec; } else tk = Sub; return; }
        else if (tk == '!') { if (*p == '=') { ++p; tk = Ne; } else tk = '!'; return; }
        else if (tk == '<') { if (*p == '=') { ++p; tk = Le; } else if (*p == '<') { ++p; tk = Shl; } else tk = Lt; return; }
        else if (tk == '>') { if (*p == '=') { ++p; tk = Ge; } else if (*p == '>') { ++p; tk = Shr; } else tk = Gt; return; }
        else if (tk == '|') { if (*p == '|') { ++p; tk = Lor; } else tk = Or; return; }
        else if (tk == '&') { if (*p == '&') { ++p; tk = Lan; } else tk = And; return; }
        else if (tk == '^') { tk = Xor; return; }
        else if (tk == '%') { tk = Mod; return; }
        else if (tk == '*') { tk = Mul; return; }
        else if (tk == '[') { tk = Brak; return; }
        else if (tk == '?') { tk = Cond; return; }
        else if (tk == '~' || tk == ';' || tk == '{' || tk == '}' || 
                 tk == '(' || tk == ')' || tk == ']' || tk == ',' || tk == ':') return;
    }
}

/* 表达式解析 */
int expr(int lev)
{
    int *a, *b;
    int o;
    
    /* 一元表达式 */
    if (!tk) { printf("%d: unexpected eof in expression\n", line); exit(-1); }
    else if (tk == Num) { *++e = IMM; *++e = ival; next(); ty = Int; }
    else if (tk == '"') {
        *++e = IMM; *++e = ival; next();
        while (tk == '"') next();
        data = (char*)(((int)data + sizeof(int)) & (-sizeof(int)));
        ty = Char + 1;
    }
    else if (tk == Sizeof) {
        next(); if (tk == '(') next(); else { printf("%d: open paren expected\n", line); exit(-1); }
        ty = Int; while (tk == Int || tk == Char) { next(); if (tk == Mul) { next(); ty = ty + 1; } }
        if (tk == ')') next(); else { printf("%d: close paren expected\n", line); exit(-1); }
        *++e = IMM; *++e = (ty == Char) ? 1 : sizeof(int);
        ty = Int;
    }
    else if (tk == Id) {
        a = id; next();
        if (tk == '(') {
            /* 函数调用 */
            next();
            o = 0;
            while (tk != ')') { expr(Assign); *++e = PSH; ++o; if (tk == ',') next(); }
            next();
            if (a[3] == Sys) *++e = a[4];
            else if (a[3] == Fun) { *++e = CALL; *++e = a[4]; }
            else { printf("%d: bad function call\n", line); exit(-1); }
            if (o) { *++e = ADJ; *++e = o; }
            ty = a[3] == Fun ? Int : Char + 1;
        }
        else if (a[3] == Num) { *++e = IMM; *++e = a[4]; ty = Int; }
        else {
            /* 变量访问 */
            if (a[3] == Loc) { *++e = LEA; *++e = loc - a[4]; }
            else if (a[3] == Glo) { *++e = IMM; *++e = a[4]; }
            else { printf("%d: undefined variable\n", line); exit(-1); }
            *++e = ((ty = a[3] == Loc ? Int : Char + 1) == Char) ? LC : LI;
        }
    }
    else if (tk == '(') {
        next();
        if (tk == Int || tk == Char) {
            /* 类型转换 */
            o = (tk == Char) ? Char : Int; next();
            while (tk == Mul) { next(); o = o + 1; }
            if (tk == ')') next(); else { printf("%d: bad cast\n", line); exit(-1); }
            expr(Inc);
            ty = o;
        }
        else {
            expr(Assign);
            if (tk == ')') next(); else { printf("%d: close paren expected\n", line); exit(-1); }
        }
    }
    else if (tk == Mul) {
        /* 解引用 */
        next(); expr(Inc);
        if (ty > Int) ty = ty - 1; else { printf("%d: bad dereference\n", line); exit(-1); }
        *++e = (ty == Char) ? LC : LI;
    }
    else if (tk == And) {
        /* 取地址 */
        next(); expr(Inc);
        if (*e == LC || *e == LI) --e; else { printf("%d: bad address-of\n", line); exit(-1); }
        ty = ty + 1;
    }
    else if (tk == '!') { next(); expr(Inc); *++e = PSH; *++e = IMM; *++e = 0; *++e = EQ; ty = Int; }
    else if (tk == '~') { next(); expr(Inc); *++e = PSH; *++e = IMM; *++e = -1; *++e = XOR; ty = Int; }
    else if (tk == Add) { next(); expr(Inc); ty = Int; }
    else if (tk == Sub) { next(); *++e = IMM; *++e = 0; expr(Inc); *++e = SUB; ty = Int; }
    else if (tk == Inc || tk == Dec) {
        /* 前置++ -- */
        o = tk; next(); expr(Inc);
        *++e = PSH; *++e = IMM; *++e = (ty == Char) ? 1 : sizeof(int);
        *++e = (o == Inc) ? ADD : SUB;
        *++e = (ty == Char) ? SC : SI;
    }
    else { printf("%d: bad expression\n", line); exit(-1); }
    
    /* 二元表达式 */
    while (tk >= lev) {
        o = ty;
        if (tk == Assign) {
            /* 赋值 */
            next();
            if (*e == LC || *e == LI) *e = PSH; else { printf("%d: bad lvalue in assignment\n", line); exit(-1); }
            expr(Assign); *++e = ((ty = o) == Char) ? SC : SI;
        }
        else if (tk == Cond) {
            /* 三元运算符 */
            next();
            *++e = JZ; b = ++e;
            expr(Assign);
            if (tk == ':') next(); else { printf("%d: conditional missing colon\n", line); exit(-1); }
            *b = (int)(e + 3); *++e = JMP; b = ++e;
            expr(Cond);
            *b = (int)(e + 1);
        }
        else if (tk == Lor) { next(); *++e = JNZ; b = ++e; expr(Lan); *b = (int)(e + 1); ty = Int; }
        else if (tk == Lan) { next(); *++e = JZ; b = ++e; expr(Or);  *b = (int)(e + 1); ty = Int; }
        else if (tk == Or)  { next(); *++e = PSH; expr(Xor); *++e = OR;  ty = Int; }
        else if (tk == Xor) { next(); *++e = PSH; expr(And); *++e = XOR; ty = Int; }
        else if (tk == And) { next(); *++e = PSH; expr(Eq);  *++e = AND; ty = Int; }
        else if (tk == Eq)  { next(); *++e = PSH; expr(Lt);  *++e = EQ;  ty = Int; }
        else if (tk == Ne)  { next(); *++e = PSH; expr(Lt);  *++e = NE;  ty = Int; }
        else if (tk == Lt)  { next(); *++e = PSH; expr(Shl); *++e = LT;  ty = Int; }
        else if (tk == Gt)  { next(); *++e = PSH; expr(Shl); *++e = GT;  ty = Int; }
        else if (tk == Le)  { next(); *++e = PSH; expr(Shl); *++e = LE;  ty = Int; }
        else if (tk == Ge)  { next(); *++e = PSH; expr(Shl); *++e = GE;  ty = Int; }
        else if (tk == Shl) { next(); *++e = PSH; expr(Add); *++e = SHL; ty = Int; }
        else if (tk == Shr) { next(); *++e = PSH; expr(Add); *++e = SHR; ty = Int; }
        else if (tk == Add) {
            next(); *++e = PSH; expr(Mul);
            if ((ty = o) > Int) { *++e = PSH; *++e = IMM; *++e = sizeof(int); *++e = MUL;  }
            *++e = ADD;
        }
        else if (tk == Sub) {
            next(); *++e = PSH; expr(Mul);
            if (o > Int && ty > Int) { *++e = SUB; *++e = PSH; *++e = IMM; *++e = sizeof(int); *++e = DIV; ty = Int; }
            else if ((ty = o) > Int) { *++e = PSH; *++e = IMM; *++e = sizeof(int); *++e = MUL; }
            *++e = SUB;
        }
        else if (tk == Mul) { next(); *++e = PSH; expr(Inc); *++e = MUL; ty = Int; }
        else if (tk == Div) { next(); *++e = PSH; expr(Inc); *++e = DIV; ty = Int; }
        else if (tk == Mod) { next(); *++e = PSH; expr(Inc); *++e = MOD; ty = Int; }
        else if (tk == Inc || tk == Dec) {
            /* 后置++ -- */
            *++e = PSH; *++e = IMM; *++e = (ty == Char) ? 1 : sizeof(int);
            *++e = (tk == Inc) ? ADD : SUB;
            *++e = (ty == Char) ? SC : SI;
            *++e = PSH; *++e = IMM; *++e = (ty == Char) ? 1 : sizeof(int);
            *++e = (tk == Inc) ? SUB : ADD;
            next();
        }
        else if (tk == Brak) {
            /* 数组索引 */
            next(); *++e = PSH; expr(Assign);
            if (tk == ']') next(); else { printf("%d: close bracket expected\n", line); exit(-1); }
            if (o > Int) { *++e = PSH; *++e = IMM; *++e = sizeof(int); *++e = MUL;  }
            *++e = ADD;
            *++e = ((ty = o) == Char) ? LC : LI;
        }
        else { printf("%d: compiler error tk=%d\n", line, tk); exit(-1); }
    }
    return 0;
}

/* 语句解析 */
void stmt()
{
    int *a, *b;
    
    if (tk == If) {
        /* if语句 */
        next();
        if (tk == '(') next(); else { printf("%d: open paren expected\n", line); exit(-1); }
        expr(Assign);
        if (tk == ')') next(); else { printf("%d: close paren expected\n", line); exit(-1); }
        *++e = JZ; b = ++e;
        stmt();
        if (tk == Else) {
            *b = (int)(e + 3);
            *++e = JMP; b = ++e;
            next();
            stmt();
        }
        *b = (int)(e + 1);
    }
    else if (tk == While) {
        /* while语句 */
        next();
        a = e + 1;
        if (tk == '(') next(); else { printf("%d: open paren expected\n", line); exit(-1); }
        expr(Assign);
        if (tk == ')') next(); else { printf("%d: close paren expected\n", line); exit(-1); }
        *++e = JZ; b = ++e;
        stmt();
        *++e = JMP; *++e = (int)a;
        *b = (int)(e + 1);
    }
    else if (tk == Return) {
        /* return语句 */
        next();
        if (tk != ';') expr(Assign);
        *++e = LEV;
        if (tk == ';') next(); else { printf("%d: semicolon expected\n", line); exit(-1); }
    }
    else if (tk == '{') {
        /* 复合语句 */
        next();
        while (tk != '}') stmt();
        next();
    }
    else if (tk == ';') {
        /* 空语句 */
        next();
    }
    else {
        /* 表达式语句 */
        expr(Assign);
        if (tk == ';') next(); else { printf("%d: semicolon expected\n", line); exit(-1); }
    }
}

/* 初始化符号表 */
void init_symbols()
{
    /* 初始化符号表 */
    sym = (int*)malloc(1024 * sizeof(int));
    memset(sym, 0, 1024 * sizeof(int));
    ids = sym;
    
    /* 添加关键字到符号表 */
    p = "char else enum if int return sizeof while "
        "open read close printf malloc free memset memcmp exit void main";
    
    /* char */
    id = ids; id[0] = Char; id[1] = (int)"char"; id[2] = 0; id[3] = 0; id[4] = 0; ids += 5;
    /* else */
    id = ids; id[0] = Else; id[1] = (int)"else"; id[2] = 0; id[3] = 0; id[4] = 0; ids += 5;
    /* enum */
    id = ids; id[0] = Enum; id[1] = (int)"enum"; id[2] = 0; id[3] = 0; id[4] = 0; ids += 5;
    /* if */
    id = ids; id[0] = If; id[1] = (int)"if"; id[2] = 0; id[3] = 0; id[4] = 0; ids += 5;
    /* int */
    id = ids; id[0] = Int; id[1] = (int)"int"; id[2] = 0; id[3] = 0; id[4] = 0; ids += 5;
    /* return */
    id = ids; id[0] = Return; id[1] = (int)"return"; id[2] = 0; id[3] = 0; id[4] = 0; ids += 5;
    /* sizeof */
    id = ids; id[0] = Sizeof; id[1] = (int)"sizeof"; id[2] = 0; id[3] = 0; id[4] = 0; ids += 5;
    /* while */
    id = ids; id[0] = While; id[1] = (int)"while"; id[2] = 0; id[3] = 0; id[4] = 0; ids += 5;
    
    /* 系统函数 */
    id = ids; id[0] = Sys; id[1] = (int)"open"; id[2] = 0; id[3] = Char + 1; id[4] = OPEN; ids += 5;
    id = ids; id[0] = Sys; id[1] = (int)"read"; id[2] = 0; id[3] = Int; id[4] = READ; ids += 5;
    id = ids; id[0] = Sys; id[1] = (int)"close"; id[2] = 0; id[3] = Int; id[4] = CLOS; ids += 5;
    id = ids; id[0] = Sys; id[1] = (int)"printf"; id[2] = 0; id[3] = Int; id[4] = PRTF; ids += 5;
    id = ids; id[0] = Sys; id[1] = (int)"malloc"; id[2] = 0; id[3] = Int; id[4] = MALC; ids += 5;
    id = ids; id[0] = Sys; id[1] = (int)"free"; id[2] = 0; id[3] = Int; id[4] = FREE; ids += 5;
    id = ids; id[0] = Sys; id[1] = (int)"memset"; id[2] = 0; id[3] = Int; id[4] = MSET; ids += 5;
    id = ids; id[0] = Sys; id[1] = (int)"memcmp"; id[2] = 0; id[3] = Int; id[4] = MCMP; ids += 5;
    id = ids; id[0] = Sys; id[1] = (int)"exit"; id[2] = 0; id[3] = Int; id[4] = EXIT; ids += 5;
    
    idmain = ids;
}

/* 主函数 */
int main(int argc, char **argv)
{
    int fd, bt;
    int *pc_base;
    
    /* 初始化内存 */
    mem = (int*)malloc(256 * 1024);  /* 256KB内存 */
    memset(mem, 0, 256 * 1024);
    
    text = mem;
    data = mem + (64 * 1024 / sizeof(int));  /* 代码段64KB */
    e = text;
    le = text;
    
    /* 初始化符号表 */
    init_symbols();
    
    /* 处理参数 */
    argc--; argv++;
    while (argc > 0 && **argv == '-') {
        if ((*argv)[1] == 's') src = 1;
        if ((*argv)[1] == 'd') debug = 1;
        argc--; argv++;
    }
    
    if (argc < 1) {
        printf("usage: c4 [-s] [-d] file.c\n");
        printf("  -s: print source code\n");
        printf("  -d: debug mode\n");
        return -1;
    }
    
    /* 读取源文件 */
    fd = open(argv[0], 0);
    if (fd < 0) { printf("could not open %s\n", argv[0]); return -1; }
    
    /* 简单读取文件到内存 */
    p = (char*)malloc(64 * 1024);
    bt = read(fd, p, 64 * 1024 - 1);
    p[bt] = 0;
    close(fd);
    
    /* 解析程序 */
    line = 1;
    next();
    
    while (tk) {
        /* 全局声明 */
        ty = Int;
        if (tk == Int) next();
        else if (tk == Char) { next(); ty = Char; }
        else if (tk == Enum) {
            /* enum */
            next();
            if (tk != '{') next();
            if (tk == '{') {
                next();
                int i = 0;
                while (tk != '}') {
                    if (tk != Id) { printf("%d: bad enum identifier %d\n", line, tk); return -1; }
                    next();
                    if (tk == Assign) {
                        next();
                        if (tk != Num) { printf("%d: bad enum initializer\n", line); return -1; }
                        i = ival;
                        next();
                    }
                    /* 添加到符号表 */
                    id[0] = Num; id[1] = id[1]; id[2] = id[2]; id[3] = Num; id[4] = i++;
                    id += 5;
                    if (tk == ',') next();
                }
                next();
            }
        }
        
        while (tk != ';' && tk != '}') {
            ty = ty + (tk == Int || tk == Char ? 0 : 1);
            while (tk == Mul) { next(); ty = ty + 1; }
            if (tk != Id) { printf("%d: bad global declaration\n", line); return -1; }
            if (id[3]) { printf("%d: duplicate global definition\n", line); return -1; }
            next();
            if (tk == '(') {
                /* 函数定义 */
                id[3] = Fun;
                id[4] = (int)(e + 1);
                next(); o = 0;
                while (tk != ')') {
                    if (tk == Int || tk == Char) { next(); }
                    while (tk == Mul) { next(); }
                    if (tk != Id) { printf("%d: bad parameter\n", line); return -1; }
                    /* 添加参数 */
                    id[0] = Loc; id[3] = ty; id[4] = o++;
                    id += 5;
                    if (tk == ',') next();
                }
                next();
                if (tk != '{') { printf("%d: bad function body\n", line); return -1; }
                loc = o + 1;
                next();
                while (tk == Int || tk == Char) {
                    /* 局部变量 */
                    o = (tk == Char) ? Char : Int;
                    next();
                    while (tk == Mul) { next(); o = o + 1; }
                    if (tk != Id) { printf("%d: bad local declaration\n", line); return -1; }
                    id[0] = Loc; id[3] = o; id[4] = loc++;
                    id += 5;
                    if (tk == ',') next();
                    if (tk == ';') next();
                }
                *++e = ENT; *++e = loc - o - 1;
                while (tk != '}') stmt();
                *++e = LEV;
                next();
            }
            else {
                /* 全局变量 */
                id[3] = Glo;
                id[4] = (int)data;
                data = data + sizeof(int);
            }
            if (tk == ',') next();
        }
        next();
    }
    
    /* 设置main函数入口 */
    if (!(pc = (int*)idmain[4])) { printf("main() not defined\n"); return -1; }
    
    /* 初始化栈 */
    sp = (int*)((int)mem + 256 * 1024);
    *--sp = EXIT;  /* 调用main后退出 */
    *--sp = PSH; *--sp = argc; *--sp = (int)argv;
    *--sp = (int)pc;
    
    /* 执行 */
    if (src) return 0;  /* 仅编译 */
    
    printf("\n=== Execution ===\n\n");
    
    /* 虚拟机执行循环 */
    int cycle = 0;
    while (1) {
        cycle++;
        if (debug) {
            printf("%d> %.4s", cycle,
                &"LEA ,IMM ,JMP ,CALL,JZ ,JNZ,ENT ,ADJ ,LEV ,LI ,LC ,SI ,SC ,PSH,"
                 "OR ,XOR,AND ,EQ ,NE ,LT ,GT ,LE ,GE ,SHL,SHR,ADD,SUB,MUL,DIV,MOD,"
                 "OPEN,READ,CLOS,PRTF,MALC,FREE,MSET,MCMP,EXIT,"[*pc * 5]);
            if (*pc <= ADJ) printf(" %d\n", pc[1]); else printf("\n");
        }
        
        switch (*pc++) {
            case LEA: a = bp + *pc++; *--sp = a; break;          /* 加载局部变量地址 */
            case IMM: *--sp = *pc++; break;                       /* 加载立即数 */
            case JMP: pc = (int*)*pc; break;                      /* 跳转 */
            case JZ:  pc = *sp++ ? pc + 1 : (int*)*pc; break;     /* 为零跳转 */
            case JNZ: pc = *sp++ ? (int*)*pc : pc + 1; break;     /* 非零跳转 */
            case CALL: *--sp = (int)(pc + 1); pc = (int*)*pc; break; /* 函数调用 */
            case RET: pc = (int*)*sp++; break;                    /* 返回 */
            case ENT: *--sp = (int)bp; bp = sp; sp = sp - *pc++; break; /* 进入函数 */
            case ADJ: sp = sp + *pc++; break;                     /* 调整栈 */
            case LEV: sp = bp; bp = (int*)*sp++; pc = (int*)*sp++; break; /* 离开函数 */
            case LI:  a = *sp++; *--sp = *(int*)a; break;         /* 加载整数 */
            case LC:  a = *sp++; *--sp = *(char*)a; break;        /* 加载字符 */
            case SI:  a = *sp++; *(int*)a = *sp++; break;         /* 存储整数 */
            case SC:  a = *sp++; *(char*)a = *sp++; break;        /* 存储字符 */
            case PSH: *--sp = a; break;                           /* 压栈 */
            case OR:  b = *sp++; a = *sp++; *--sp = a | b; break;
            case XOR: b = *sp++; a = *sp++; *--sp = a ^ b; break;
            case AND: b = *sp++; a = *sp++; *--sp = a & b; break;
            case EQ:  b = *sp++; a = *sp++; *--sp = a == b; break;
            case NE:  b = *sp++; a = *sp++; *--sp = a != b; break;
            case LT:  b = *sp++; a = *sp++; *--sp = a < b; break;
            case GT:  b = *sp++; a = *sp++; *--sp = a > b; break;
            case LE:  b = *sp++; a = *sp++; *--sp = a <= b; break;
            case GE:  b = *sp++; a = *sp++; *--sp = a >= b; break;
            case SHL: b = *sp++; a = *sp++; *--sp = a << b; break;
            case SHR: b = *sp++; a = *sp++; *--sp = a >> b; break;
            case ADD: b = *sp++; a = *sp++; *--sp = a + b; break;
            case SUB: b = *sp++; a = *sp++; *--sp = a - b; break;
            case MUL: b = *sp++; a = *sp++; *--sp = a * b; break;
            case DIV: b = *sp++; a = *sp++; *--sp = a / b; break;
            case MOD: b = *sp++; a = *sp++; *--sp = a % b; break;
            case OPEN: a = open((char*)sp[1], *sp); sp += 2; *--sp = a; break;
            case READ: a = read(sp[2], (char*)sp[1], *sp); sp += 3; *--sp = a; break;
            case CLOS: a = close(*sp); sp++; *--sp = a; break;
            case PRTF: 
                a = sp + pc[1] - 1;
                *--sp = printf((char*)a[0], a[1], a[2], a[3], a[4], a[5]);
                sp += pc[1];
                pc++;
                break;
            case MALC: a = malloc(*sp); *sp = (int)a; break;
            case FREE: free((void*)*sp); sp++; break;
            case MSET: a = sp[2]; sp += 3; *--sp = memset((void*)a[2], a[1], a[0]) ? 0 : 1; break;
            case MCMP: a = sp[2]; sp += 3; *--sp = memcmp((void*)a[2], (void*)a[1], a[0]); break;
            case EXIT: printf("\n[Exit code: %d]\n", *sp); return *sp;
            default: printf("unknown instruction: %d\n", pc[-1]); return -1;
        }
    }
    
    return 0;
}

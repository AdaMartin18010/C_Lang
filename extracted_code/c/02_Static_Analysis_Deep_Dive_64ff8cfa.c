/*
 * Auto-generated from: 07_Modern_Toolchain\05_Code_Quality_Toolchain\02_Static_Analysis_Deep_Dive.md
 * Line: 247
 * Language: c
 * Block ID: 64ff8cfa
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 源代码
if (x > 0) {
    y = x * 2;
}

// 抽象语法树(AST)
IfStatement
├── Condition: BinaryOp(>)
│   ├── Left: Variable(x)
│   └── Right: Literal(0)
└── ThenBody: CompoundStatement
    └── Assignment
        ├── Left: Variable(y)
        └── Right: BinaryOp(*)
            ├── Left: Variable(x)
            └── Right: Literal(2)

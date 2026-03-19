/*
 * Auto-generated from: 14_Video_Tutorials\01_Video_Scripts\02_Pointer_Deep_Dive.md
 * Line: 741
 * Language: c
 * Block ID: 8522736f
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

struct Node {
    int data;
    struct Node *next;  // 自引用指针
};

链表结构：
┌──────┬─────┐    ┌──────┬─────┐    ┌──────┬──────┐
│ data │ next│───►│ data │ next│───►│ data │ NULL │
│  10  │     │    │  20  │     │    │  30  │      │
└──────┴─────┘    └──────┴─────┘    └──────┴──────┘
   节点1             节点2             节点3

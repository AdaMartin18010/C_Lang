/*
 * Auto-generated from: 07_Modern_Toolchain\08_Design_Patterns\README.md
 * Line: 61
 * Language: c
 * Block ID: e00a597d
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// singleton.h
#ifndef SINGLETON_H
#define SINGLETON_H

typedef struct {
    int data;
    char name[32];
} Singleton;

// 获取唯一实例
Singleton* singleton_get_instance(void);

// 实例操作
void singleton_set_data(Singleton* self, int value);
int singleton_get_data(const Singleton* self);

#endif

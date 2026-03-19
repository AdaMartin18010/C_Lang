/*
 * Auto-generated from: 01_Core_Knowledge_System\05_Engineering_Layer\README.md
 * Line: 187
 * Language: c
 * Block ID: 1540cf86
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// include/myproject/version.h
#ifndef MYPROJECT_VERSION_H
#define MYPROJECT_VERSION_H

#define MYPROJECT_VERSION_MAJOR 1
#define MYPROJECT_VERSION_MINOR 2
#define MYPROJECT_VERSION_PATCH 3
#define MYPROJECT_VERSION_TWEAK 0

#define MYPROJECT_VERSION_STRING "1.2.3"

#define MYPROJECT_VERSION(major, minor, patch) \
    ((major) * 1000000 + (minor) * 1000 + (patch))

#define MYPROJECT_VERSION_CURRENT \
    MYPROJECT_VERSION(MYPROJECT_VERSION_MAJOR, \
                      MYPROJECT_VERSION_MINOR, \
                      MYPROJECT_VERSION_PATCH)

// 版本检查宏
#define MYPROJECT_VERSION_AT_LEAST(major, minor, patch) \
    (MYPROJECT_VERSION_CURRENT >= MYPROJECT_VERSION(major, minor, patch))

#endif

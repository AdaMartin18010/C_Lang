/*
 * Auto-generated from: 01_Core_Knowledge_System\07_Modern_C\README.md
 * Line: 716
 * Language: c
 * Block ID: 55010e15
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// include/project/config.h
#ifndef PROJECT_CONFIG_H
#define PROJECT_CONFIG_H

#if __STDC_VERSION__ < 201112L
    #error "C11 or later is required"
#endif

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

#if __STDC_VERSION__ >= 202311L
    #define NODISCARD [[nodiscard]]
    #define MAYBE_UNUSED [[maybe_unused]]
#else
    #define NODISCARD __attribute__((warn_unused_result))
    #define MAYBE_UNUSED __attribute__((unused))
#endif

#endif /* PROJECT_CONFIG_H */

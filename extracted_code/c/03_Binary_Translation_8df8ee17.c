/*
 * Auto-generated from: 05_Deep_Structure_MetaPhysics\03_Binary_Translation.md
 * Line: 348
 * Language: c
 * Block ID: 8df8ee17
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// DynamoRIO客户端示例：内存访问跟踪
#include "dr_api.h"

static void event_exit(void);
static dr_emit_flags_t event_bb_analysis(
    void* drcontext,
    void* tag,
    instrlist_t* bb,
    bool for_trace,
    bool translating
);

DR_EXPORT void dr_client_main(client_id_t id, int argc, const char* argv[]) {
    dr_register_exit_event(event_exit);
    drmgr_register_bb_instrumentation_event(event_bb_analysis, NULL, NULL);
}

static dr_emit_flags_t event_bb_analysis(
    void* drcontext,
    void* tag,
    instrlist_t* bb,
    bool for_trace,
    bool translating
) {
    instr_t* instr;
    for (instr = instrlist_first(bb); instr != NULL; instr = instr_get_next(instr)) {
        if (instr_reads_memory(instr) || instr_writes_memory(instr)) {
            // 插桩内存访问指令
            dr_insert_clean_call(drcontext, bb, instr,
                                 (void*)memory_access_hook,
                                 false, 2,
                                 OPND_CREATE_INTPTR(instr_get_app_pc(instr)),
                                 instr_get_dst(instr, 0));
        }
    }
    return DR_EMIT_DEFAULT;
}

static void memory_access_hook(app_pc pc, void* addr) {
    dr_fprintf(STDERR, "内存访问 @%p: 地址=%p\n", pc, addr);
}

static void event_exit(void) {
    dr_fprintf(STDERR, "客户端退出\n");
}

/*
 * Auto-generated from: 04_Industrial_Scenarios\02_Automotive_ECU\04_OBD_Diagnostics.md
 * Line: 470
 * Language: c
 * Block ID: a4daba7f
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// DTC数据库查询示例
typedef struct {
    uint16_t code;
    const char* description;
    const char* possible_causes;
    const char* recommended_action;
} dtc_entry_t;

static const dtc_entry_t dtc_database[] = {
    {0x0171, "System Too Lean (Bank 1)",
     "Vacuum leak, faulty MAF, low fuel pressure",
     "Check intake hoses, clean MAF, check fuel pump"},

    {0x0300, "Random/Multiple Cylinder Misfire",
     "Worn spark plugs, bad coils, poor fuel quality",
     "Replace spark plugs, test ignition coils"},

    {0x0420, "Catalyst System Efficiency Below Threshold",
     "Aging catalytic converter, O2 sensor failure",
     "Replace catalytic converter or O2 sensors"},

    // ... 更多DTC
};

const dtc_entry_t* lookup_dtc(uint16_t code) {
    for (size_t i = 0; i < sizeof(dtc_database)/sizeof(dtc_database[0]); i++) {
        if (dtc_database[i].code == code) {
            return &dtc_database[i];
        }
    }
    return NULL;
}

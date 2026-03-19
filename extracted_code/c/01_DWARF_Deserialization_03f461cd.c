/*
 * Auto-generated from: 05_Deep_Structure_MetaPhysics\10_Debug_Info_Encoding\01_DWARF_Deserialization.md
 * Line: 290
 * Language: c
 * Block ID: 03f461cd
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 解析缩略码声明
typedef struct {
    uint32_t code;
    uint16_t tag;
    bool has_children;

    struct {
        uint16_t name;
        uint16_t form;
    } *attrs;
    int num_attrs;
} Dwarf_Abbrev;

// 加载缩略码表
GHashTable *load_abbrev_table(const uint8_t *data, size_t size) {
    GHashTable *table = g_hash_table_new(g_direct_hash, g_direct_equal);
    size_t offset = 0;

    while (offset < size) {
        size_t bytes;
        uint32_t code = decode_uleb128(data + offset, &bytes);
        offset += bytes;

        if (code == 0) continue;  // 缩略码表结束

        Dwarf_Abbrev *abbrev = malloc(sizeof(Dwarf_Abbrev));
        abbrev->code = code;
        abbrev->tag = decode_uleb128(data + offset, &bytes);
        offset += bytes;
        abbrev->has_children = data[offset++];

        // 读取属性规范
        GArray *attrs = g_array_new(FALSE, FALSE,
                                    sizeof(struct { uint16_t name; uint16_t form; }));
        while (true) {
            uint16_t name = decode_uleb128(data + offset, &bytes);
            offset += bytes;
            uint16_t form = decode_uleb128(data + offset, &bytes);
            offset += bytes;

            if (name == 0 && form == 0) break;

            struct attr_spec spec = { name, form };
            g_array_append_val(attrs, spec);
        }

        abbrev->num_attrs = attrs->len;
        abbrev->attrs = g_array_free(attrs, FALSE);

        g_hash_table_insert(table, GUINT_TO_POINTER(code), abbrev);
    }

    return table;
}

// 解析单个DIE
Dwarf_Die *parse_die(const uint8_t *data, size_t *offset,
                     GHashTable *abbrev_table,
                     const uint8_t *str_section) {
    size_t start_offset = *offset;

    size_t bytes;
    uint32_t abbrev_code = decode_uleb128(data + *offset, &bytes);
    *offset += bytes;

    if (abbrev_code == 0) return NULL;  // NULL DIE（兄弟标记）

    Dwarf_Abbrev *abbrev = g_hash_table_lookup(abbrev_table,
                                               GUINT_TO_POINTER(abbrev_code));
    if (!abbrev) {
        fprintf(stderr, "Unknown abbrev code: %u\n", abbrev_code);
        return NULL;
    }

    Dwarf_Die *die = calloc(1, sizeof(Dwarf_Die));
    die->offset = start_offset;
    die->abbrev_code = abbrev_code;
    die->tag = abbrev->tag;
    die->has_children = abbrev->has_children;

    // 解析属性
    die->num_attributes = abbrev->num_attrs;
    die->attributes = calloc(abbrev->num_attrs, sizeof(Dwarf_AttributeValue));
    die->attr_names = calloc(abbrev->num_attrs, sizeof(uint16_t));

    for (int i = 0; i < abbrev->num_attrs; i++) {
        uint16_t name = abbrev->attrs[i].name;
        uint16_t form = abbrev->attrs[i].form;

        die->attr_names[i] = name;
        Dwarf_AttributeValue *val = &die->attributes[i];
        val->form = form;

        // 根据form解析值
        switch (form) {
            case DW_FORM_addr:
                val->addr = read_uint64(data + *offset);
                *offset += 8;
                break;

            case DW_FORM_data1:
                val->uval = data[(*offset)++];
                break;

            case DW_FORM_data2:
                val->uval = read_uint16(data + *offset);
                *offset += 2;
                break;

            case DW_FORM_data4:
                val->uval = read_uint32(data + *offset);
                *offset += 4;
                break;

            case DW_FORM_data8:
                val->uval = read_uint64(data + *offset);
                *offset += 8;
                break;

            case DW_FORM_udata:
                val->uval = decode_uleb128(data + *offset, &bytes);
                *offset += bytes;
                break;

            case DW_FORM_sdata:
                val->sval = decode_sleb128(data + *offset, &bytes);
                *offset += bytes;
                break;

            case DW_FORM_string:
                val->string = (const char *)(data + *offset);
                *offset += strlen(val->string) + 1;
                break;

            case DW_FORM_strp:
                val->string = (const char *)(str_section +
                                              read_uint32(data + *offset));
                *offset += 4;
                break;

            case DW_FORM_ref4:
                val->ref = start_offset + read_uint32(data + *offset);
                *offset += 4;
                break;

            case DW_FORM_exprloc: {
                size_t len = decode_uleb128(data + *offset, &bytes);
                *offset += bytes;
                val->block = malloc(len);
                memcpy(val->block, data + *offset, len);
                val->block_len = len;
                *offset += len;
                break;
            }

            case DW_FORM_flag_present:
                val->flag = true;
                break;

            case DW_FORM_flag:
                val->flag = data[(*offset)++] != 0;
                break;

            default:
                fprintf(stderr, "Unhandled form: 0x%x\n", form);
                break;
        }
    }

    return die;
}

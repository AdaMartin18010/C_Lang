/*
 * Auto-generated from: 03_System_Technology_Domains\07_Hardware_Security\08_Hardware_Root_of_Trust.md
 * Line: 273
 * Language: c
 * Block ID: 834710d2
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * 信任链建立和管理
 */

/* 信任链节点 */
typedef struct trust_chain_node {
    const char *name;
    const rot_descriptor_t *rot;
    uint8_t measurement[64];        /* SHA-512哈希 */
    uint32_t measurement_len;
    struct trust_chain_node *parent;
    struct trust_chain_node *child;
} trust_chain_node_t;

/* 信任链 */
typedef struct {
    trust_chain_node_t *root;
    trust_chain_node_t *current;
    uint32_t depth;
    uint32_t max_depth;
} trust_chain_t;

/* 初始化信任链 */
int trust_chain_init(trust_chain_t *chain, const rot_descriptor_t *root_rot) {
    chain->root = calloc(1, sizeof(trust_chain_node_t));
    if (!chain->root) return -ENOMEM;

    chain->root->name = "Root of Trust";
    chain->root->rot = root_rot;
    chain->root->parent = NULL;
    chain->current = chain->root;
    chain->depth = 1;
    chain->max_depth = 10;

    return 0;
}

/* 扩展信任链 */
int trust_chain_extend(trust_chain_t *chain,
                       const char *name,
                       const rot_descriptor_t *rot,
                       const uint8_t *measurement,
                       uint32_t measurement_len) {
    if (chain->depth >= chain->max_depth) {
        return -ECHAIN_TOO_DEEP;
    }

    trust_chain_node_t *new_node = calloc(1, sizeof(trust_chain_node_t));
    if (!new_node) return -ENOMEM;

    new_node->name = name;
    new_node->rot = rot;
    memcpy(new_node->measurement, measurement,
           measurement_len > 64 ? 64 : measurement_len);
    new_node->measurement_len = measurement_len;
    new_node->parent = chain->current;

    chain->current->child = new_node;
    chain->current = new_node;
    chain->depth++;

    /* 记录到TPM */
    if (tpm_available()) {
        tpm_pcr_extend(chain->depth - 1, measurement, measurement_len);
    }

    return 0;
}

/* 验证信任链 */
int trust_chain_verify(const trust_chain_t *chain,
                       bool (*verify_callback)(const trust_chain_node_t *)) {
    trust_chain_node_t *node = chain->root;

    while (node) {
        /* 验证每个节点 */
        if (!verify_callback(node)) {
            return -ECHAIN_BROKEN;
        }

        /* 验证测量值 */
        if (node->parent && node->rot->is_measurable) {
            uint8_t computed_hash[64];
            hash_component(node->name, computed_hash);

            if (memcmp(computed_hash, node->measurement,
                       node->measurement_len) != 0) {
                return -EINTEGRITY_FAIL;
            }
        }

        node = node->child;
    }

    return 0;
}

/* 信任链可视化 */
void trust_chain_print(const trust_chain_t *chain) {
    printf("Trust Chain:\n");

    trust_chain_node_t *node = chain->root;
    int level = 0;

    while (node) {
        printf("%*s[%d] %s (%s)\n",
               level * 2, "",
               level,
               node->name,
               node->rot->is_mutable ? "mutable" : "immutable");

        printf("%*s    Type: %s\n", level * 2, "",
               node->rot->type == ROT_TYPE_HARDWARE ? "Hardware" :
               node->rot->type == ROT_TYPE_FIRMWARE ? "Firmware" : "Software");

        printf("%*s    Hash: ", level * 2, "");
        for (uint32_t i = 0; i < node->measurement_len && i < 8; i++) {
            printf("%02x", node->measurement[i]);
        }
        printf("...\n");

        node = node->child;
        level++;
    }
}

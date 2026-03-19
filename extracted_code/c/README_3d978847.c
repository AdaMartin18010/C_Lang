/*
 * Auto-generated from: 03_System_Technology_Domains\15_Network_Programming\README.md
 * Line: 1529
 * Language: c
 * Block ID: 3d978847
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* NUMA 亲和配置 */
void setup_numa_affinity(uint16_t port_id, unsigned lcore_id) {
    int socket_id = rte_eth_dev_socket_id(port_id);

    /* 将线程绑定到 NUMA 节点 */
    cpu_set_t cpuset;
    CPU_ZERO(&cpuset);
    CPU_SET(lcore_id, &cpuset);
    pthread_setaffinity_np(pthread_self(), sizeof(cpuset), &cpuset);

    printf("Port %u on socket %d, lcore %u\n", port_id, socket_id, lcore_id);
}

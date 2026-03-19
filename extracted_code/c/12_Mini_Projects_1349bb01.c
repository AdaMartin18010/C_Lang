/*
 * Auto-generated from: 12_Practice_Exercises\12_Mini_Projects.md
 * Line: 203
 * Language: c
 * Block ID: 1349bb01
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 解析域名
DNSResult* dns_resolve(const char *hostname, DNSRecordType type);

// 示例
DNSResult *result = dns_resolve("www.example.com", DNS_A);
for (int i = 0; i < result->count; i++) {
    printf("%s -> %s\n", result->name, result->records[i].ip);
}

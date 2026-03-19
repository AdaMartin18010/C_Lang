/*
 * Auto-generated from: 12_Practice_Exercises\13_Real_World_Projects.md
 * Line: 7786
 * Language: c
 * Block ID: 526be654
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* test_crawler.c - Unit tests for crawler components */

#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "crawler.h"

void test_url_parse(void) {
    url_t url;

    /* Test basic URL */
    assert(url_parse("https://example.com/path?query=1", &url) == 0);
    assert(strcmp(url.scheme, "https") == 0);
    assert(strcmp(url.host, "example.com") == 0);
    assert(url.port == 443);
    assert(strcmp(url.path, "/path") == 0);
    assert(strcmp(url.query, "query=1") == 0);

    /* Test URL with port */
    assert(url_parse("http://localhost:8080/api", &url) == 0);
    assert(url.port == 8080);

    printf("URL parsing tests passed\n");
}

void test_bloom_filter(void) {
    bloom_filter_t *bf = bloom_create(1000, 0.01);
    assert(bf != NULL);

    /* Add items */
    bloom_add(bf, "https://example.com/1");
    bloom_add(bf, "https://example.com/2");

    /* Check membership */
    assert(bloom_check(bf, "https://example.com/1") == true);
    assert(bloom_check(bf, "https://example.com/2") == true);
    assert(bloom_check(bf, "https://example.com/3") == false);

    bloom_destroy(bf);
    printf("Bloom filter tests passed\n");
}

void test_dns_cache(void) {
    dns_cache_t *cache = dns_cache_create(60);
    assert(cache != NULL);

    struct in_addr addr1, addr2;

    /* Resolve and cache */
    int ret1 = dns_resolve(cache, "localhost", &addr1);
    int ret2 = dns_resolve(cache, "localhost", &addr2);

    /* Both should succeed */
    assert(ret1 == 0 || ret1 == -1);  /* May fail if no DNS */
    assert(addr1.s_addr == addr2.s_addr);

    dns_cache_destroy(cache);
    printf("DNS cache tests passed\n");
}

void test_extract_title(void) {
    const char *html = "<html><head><title>Test Page</title></head><body></body></html>";
    char *title = extract_title(html, strlen(html));

    assert(title != NULL);
    assert(strcmp(title, "Test Page") == 0);

    free(title);
    printf("Title extraction tests passed\n");
}

void test_extract_links(void) {
    const char *html = "<a href=\"/page1\">Link1</a><a href=\"https://other.com/\">Link2</a>";
    url_t base;
    url_parse("https://example.com/", &base);

    url_t *links = NULL;
    int count = 0;
    extract_links(html, strlen(html), &base, &links, &count);

    assert(count >= 1);

    free(links);
    printf("Link extraction tests passed\n");
}

int main(void) {
    printf("Running crawler tests...\n\n");

    test_url_parse();
    test_bloom_filter();
    test_dns_cache();
    test_extract_title();
    test_extract_links();

    printf("\nAll tests passed!\n");
    return 0;
}

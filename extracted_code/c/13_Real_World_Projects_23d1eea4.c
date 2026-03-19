/*
 * Auto-generated from: 12_Practice_Exercises\13_Real_World_Projects.md
 * Line: 6554
 * Language: c
 * Block ID: 23d1eea4
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

#ifndef CRAWLER_H
#define CRAWLER_H

#include <stdint.h>
#include <stdbool.h>
#include <time.h>

#define CRAWLER_VERSION "1.0.0"
#define MAX_URL_LEN 2048
#define MAX_HOST_LEN 256
#define MAX_PATH_LEN 1024
#define MAX_HEADERS 32
#define MAX_CONCURRENT 1000
#define MAX_DEPTH 10
#define USER_AGENT "Mozilla/5.0 (compatible; CCrawler/1.0)"
#define CONNECT_TIMEOUT 10
#define READ_TIMEOUT 30
#define DEFAULT_DELAY_MS 1000

typedef enum {
    FETCH_PENDING = 0,
    FETCH_IN_PROGRESS = 1,
    FETCH_SUCCESS = 2,
    FETCH_FAILED = 3,
    FETCH_ROBOTS_DENIED = 4,
    FETCH_TIMEOUT = 5
} fetch_status_t;

typedef enum {
    PRIORITY_HIGH = 0,
    PRIORITY_NORMAL = 1,
    PRIORITY_LOW = 2
} url_priority_t;

/* URL structure */
typedef struct url {
    char scheme[16];
    char host[MAX_HOST_LEN];
    uint16_t port;
    char path[MAX_PATH_LEN];
    char query[512];
    char full_url[MAX_URL_LEN];
    url_priority_t priority;
    int depth;
    time_t scheduled_time;
} url_t;

/* HTTP response */
typedef struct http_response {
    int status_code;
    char content_type[128];
    char *body;
    size_t body_len;
    char headers[MAX_HEADERS][2][512];
    int header_count;
    double fetch_time_ms;
    time_t fetched_at;
} http_response_t;

/* Parsed content */
typedef struct page_data {
    url_t url;
    http_response_t response;
    char *title;
    char *description;
    url_t *links;
    int link_count;
    char *extracted_data;
} page_data_t;

/* Rate limiter (token bucket) */
typedef struct rate_limiter {
    char domain[MAX_HOST_LEN];
    double tokens;
    double rate;  /* tokens per second */
    double capacity;
    time_t last_update;
    pthread_mutex_t lock;
} rate_limiter_t;

/* DNS cache entry */
typedef struct dns_entry {
    char host[MAX_HOST_LEN];
    struct in_addr addr;
    time_t expires;
    struct dns_entry *next;
} dns_entry_t;

typedef struct dns_cache {
    dns_entry_t *entries[256];  /* Simple hash bucket */
    pthread_rwlock_t lock;
    int ttl_seconds;
} dns_cache_t;

/* Connection pool */
typedef struct conn_pool_entry {
    char host[MAX_HOST_LEN];
    uint16_t port;
    int fd;
    time_t created;
    time_t last_used;
    bool in_use;
    struct conn_pool_entry *next;
} conn_pool_entry_t;

typedef struct connection_pool {
    conn_pool_entry_t *connections;
    int max_connections;
    int idle_timeout;
    pthread_mutex_t lock;
} connection_pool_t;

/* Bloom filter for URL deduplication */
typedef struct bloom_filter {
    uint8_t *bits;
    size_t size;
    int num_hashes;
    uint64_t count;
    uint64_t capacity;
} bloom_filter_t;

/* URL frontier (priority queue) */
typedef struct url_queue {
    url_t *urls;
    size_t capacity;
    size_t count;
    pthread_mutex_t lock;
    pthread_cond_t not_empty;
} url_queue_t;

/* Crawler configuration */
typedef struct crawler_config {
    int max_workers;
    int max_depth;
    int max_pages;
    int polite_delay_ms;
    bool respect_robots;
    bool follow_redirects;
    int max_redirects;
    const char *user_agent;
    const char *start_url;
    const char **allowed_domains;
    int allowed_domain_count;
    const char **denied_extensions;
    int denied_extension_count;
    void (*on_page_fetched)(const page_data_t *page);
    void (*on_error)(const url_t *url, const char *error);
} crawler_config_t;

/* Main crawler structure */
typedef struct crawler {
    crawler_config_t config;

    /* Components */
    url_queue_t *frontier;
    bloom_filter_t *seen_urls;
    dns_cache_t *dns_cache;
    connection_pool_t *conn_pool;
    rate_limiter_t **rate_limiters;
    int rate_limiter_count;

    /* Workers */
    pthread_t *workers;
    int active_workers;

    /* Statistics */
    uint64_t pages_fetched;
    uint64_t pages_failed;
    uint64_t bytes_downloaded;
    time_t start_time;

    /* State */
    volatile bool running;
    pthread_mutex_t stats_lock;
} crawler_t;

/* API */
crawler_t* crawler_create(const crawler_config_t *config);
void crawler_destroy(crawler_t *crawler);
int crawler_add_url(crawler_t *crawler, const char *url_str, int depth);
int crawler_start(crawler_t *crawler);
void crawler_stop(crawler_t *crawler);
int crawler_wait(crawler_t *crawler);

/* URL operations */
int url_parse(const char *str, url_t *url);
int url_normalize(url_t *url);
int url_to_string(const url_t *url, char *out, size_t len);
int url_get_domain_key(const url_t *url, char *out, size_t len);
bool url_is_valid(const url_t *url);
bool url_matches_domain(const url_t *url, const char *domain);

/* HTTP client */
int http_fetch(const url_t *url, http_response_t *response,
               connection_pool_t *pool, dns_cache_t *dns,
               int timeout_ms);
void http_response_free(http_response_t *response);

/* DNS cache */
dns_cache_t* dns_cache_create(int ttl);
void dns_cache_destroy(dns_cache_t *cache);
int dns_resolve(dns_cache_t *cache, const char *host, struct in_addr *addr);
void dns_cache_invalidate(dns_cache_t *cache, const char *host);

/* Connection pool */
connection_pool_t* conn_pool_create(int max_conn, int idle_timeout);
void conn_pool_destroy(connection_pool_t *pool);
int conn_pool_get(connection_pool_t *pool, const char *host,
                  uint16_t port, int *fd);
void conn_pool_release(connection_pool_t *pool, int fd, bool reuse);
void conn_pool_cleanup(connection_pool_t *pool);

/* Rate limiting */
rate_limiter_t* rate_limiter_get(crawler_t *crawler, const char *domain);
bool rate_limiter_check(rate_limiter_t *rl);
void rate_limiter_wait(rate_limiter_t *rl);

/* Bloom filter */
bloom_filter_t* bloom_create(size_t expected_items, double false_positive_rate);
void bloom_destroy(bloom_filter_t *bf);
void bloom_add(bloom_filter_t *bf, const char *str);
bool bloom_check(bloom_filter_t *bf, const char *str);
void bloom_clear(bloom_filter_t *bf);

/* Robots.txt handling */
bool robots_can_fetch(const char *robots_txt, const char *path);
char* robots_fetch(const char *domain, connection_pool_t *pool);

/* Content extraction */
char* extract_title(const char *html, size_t len);
char* extract_links(const char *html, size_t len, const url_t *base_url,
                    url_t **out_links, int *count);
char* extract_by_xpath(const char *html, size_t len, const char *xpath);

/* Statistics */
typedef struct crawler_stats {
    uint64_t pages_fetched;
    uint64_t pages_failed;
    uint64_t bytes_downloaded;
    uint64_t urls_queued;
    double avg_fetch_time_ms;
    time_t elapsed_seconds;
} crawler_stats_t;

void crawler_get_stats(crawler_t *crawler, crawler_stats_t *stats);
void crawler_print_stats(crawler_t *crawler);

#endif /* CRAWLER_H */

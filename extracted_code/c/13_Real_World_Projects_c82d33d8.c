/*
 * Auto-generated from: 12_Practice_Exercises\13_Real_World_Projects.md
 * Line: 6807
 * Language: c
 * Block ID: c82d33d8
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * High Performance Web Crawler Framework
 */

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <pthread.h>
#include <signal.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <time.h>
#include <math.h>

#include "crawler.h"

/* URL parsing */
int url_parse(const char *str, url_t *url) {
    memset(url, 0, sizeof(url_t));
    strncpy(url->full_url, str, MAX_URL_LEN - 1);

    const char *p = str;

    /* Parse scheme */
    const char *scheme_end = strstr(p, "://");
    if (!scheme_end) return -1;

    size_t scheme_len = scheme_end - p;
    if (scheme_len >= sizeof(url->scheme)) return -1;
    memcpy(url->scheme, p, scheme_len);
    url->scheme[scheme_len] = '\0';

    p = scheme_end + 3;

    /* Parse host and port */
    const char *path_start = strchr(p, '/');
    const char *query_start = strchr(p, '?');

    size_t host_len;
    if (path_start) {
        host_len = path_start - p;
    } else if (query_start) {
        host_len = query_start - p;
    } else {
        host_len = strlen(p);
    }

    /* Check for port */
    const char *port_start = memchr(p, ':', host_len);
    if (port_start) {
        size_t hlen = port_start - p;
        if (hlen >= sizeof(url->host)) return -1;
        memcpy(url->host, p, hlen);
        url->host[hlen] = '\0';

        url->port = (uint16_t)atoi(port_start + 1);
    } else {
        if (host_len >= sizeof(url->host)) return -1;
        memcpy(url->host, p, host_len);
        url->host[host_len] = '\0';

        url->port = (strcmp(url->scheme, "https") == 0) ? 443 : 80;
    }

    /* Parse path and query */
    if (path_start) {
        size_t path_len;
        if (query_start) {
            path_len = query_start - path_start;
        } else {
            path_len = strlen(path_start);
        }

        if (path_len >= sizeof(url->path)) path_len = sizeof(url->path) - 1;
        memcpy(url->path, path_start, path_len);
        url->path[path_len] = '\0';

        if (query_start) {
            strncpy(url->query, query_start + 1, sizeof(url->query) - 1);
        }
    } else {
        strcpy(url->path, "/");
    }

    url->priority = PRIORITY_NORMAL;

    return 0;
}

int url_normalize(url_t *url) {
    /* Convert scheme and host to lowercase */
    for (char *p = url->scheme; *p; p++) *p = (char)tolower((unsigned char)*p);
    for (char *p = url->host; *p; p++) *p = (char)tolower((unsigned char)*p);

    /* Remove default ports */
    if ((strcmp(url->scheme, "http") == 0 && url->port == 80) ||
        (strcmp(url->scheme, "https") == 0 && url->port == 443)) {
        url->port = 0;
    }

    /* Remove fragment */
    char *hash = strchr(url->path, '#');
    if (hash) *hash = '\0';

    return 0;
}

int url_to_string(const url_t *url, char *out, size_t len) {
    if (url->port && url->port != 80 && url->port != 443) {
        return snprintf(out, len, "%s://%s:%d%s%s%s",
                       url->scheme, url->host, url->port,
                       url->path,
                       url->query[0] ? "?" : "",
                       url->query);
    } else {
        return snprintf(out, len, "%s://%s%s%s%s",
                       url->scheme, url->host,
                       url->path,
                       url->query[0] ? "?" : "",
                       url->query);
    }
}

/* Bloom filter implementation */
static uint64_t hash_fnv1a(const char *str) {
    uint64_t hash = 14695981039346656037ULL;
    while (*str) {
        hash ^= (uint64_t)(unsigned char)*str++;
        hash *= 1099511628211ULL;
    }
    return hash;
}

static uint64_t hash_murmur(const char *str) {
    uint64_t h = 5252014111076555551ULL;
    while (*str) {
        h ^= (uint64_t)(unsigned char)*str++;
        h *= 0x5bd1e9955bd1e995;
        h ^= h >> 47;
    }
    return h;
}

bloom_filter_t* bloom_create(size_t expected_items, double false_positive_rate) {
    bloom_filter_t *bf = calloc(1, sizeof(bloom_filter_t));
    if (!bf) return NULL;

    /* Calculate optimal size and hash count */
    double m = -(expected_items * log(false_positive_rate)) / (log(2) * log(2));
    bf->size = (size_t)m;
    bf->num_hashes = (int)round((m / expected_items) * log(2));
    bf->capacity = expected_items;

    bf->bits = calloc((bf->size + 7) / 8, 1);
    if (!bf->bits) {
        free(bf);
        return NULL;
    }

    return bf;
}

void bloom_destroy(bloom_filter_t *bf) {
    if (!bf) return;
    free(bf->bits);
    free(bf);
}

void bloom_add(bloom_filter_t *bf, const char *str) {
    for (int i = 0; i < bf->num_hashes; i++) {
        uint64_t hash = hash_fnv1a(str) + i * hash_murmur(str);
        size_t idx = hash % bf->size;
        bf->bits[idx / 8] |= (1 << (idx % 8));
    }
    bf->count++;
}

bool bloom_check(bloom_filter_t *bf, const char *str) {
    for (int i = 0; i < bf->num_hashes; i++) {
        uint64_t hash = hash_fnv1a(str) + i * hash_murmur(str);
        size_t idx = hash % bf->size;
        if (!(bf->bits[idx / 8] & (1 << (idx % 8)))) {
            return false;
        }
    }
    return true;
}

/* DNS cache */
dns_cache_t* dns_cache_create(int ttl) {
    dns_cache_t *cache = calloc(1, sizeof(dns_cache_t));
    cache->ttl_seconds = ttl;
    pthread_rwlock_init(&cache->lock, NULL);
    return cache;
}

void dns_cache_destroy(dns_cache_t *cache) {
    if (!cache) return;

    for (int i = 0; i < 256; i++) {
        dns_entry_t *e = cache->entries[i];
        while (e) {
            dns_entry_t *next = e->next;
            free(e);
            e = next;
        }
    }

    pthread_rwlock_destroy(&cache->lock);
    free(cache);
}

static unsigned char hash_host(const char *host) {
    unsigned char h = 0;
    while (*host) h = (h * 31) + (unsigned char)*host++;
    return h;
}

int dns_resolve(dns_cache_t *cache, const char *host, struct in_addr *addr) {
    /* Check cache first */
    unsigned char h = hash_host(host);

    pthread_rwlock_rdlock(&cache->lock);

    dns_entry_t *e = cache->entries[h];
    time_t now = time(NULL);

    while (e) {
        if (strcmp(e->host, host) == 0) {
            if (e->expires > now) {
                *addr = e->addr;
                pthread_rwlock_unlock(&cache->lock);
                return 0;
            }
            break;
        }
        e = e->next;
    }

    pthread_rwlock_unlock(&cache->lock);

    /* Perform DNS lookup */
    struct hostent *he = gethostbyname(host);
    if (!he || !he->h_addr_list[0]) {
        return -1;
    }

    memcpy(addr, he->h_addr_list[0], sizeof(struct in_addr));

    /* Add to cache */
    pthread_rwlock_wrlock(&cache->lock);

    dns_entry_t *new_entry = malloc(sizeof(dns_entry_t));
    strncpy(new_entry->host, host, MAX_HOST_LEN - 1);
    new_entry->addr = *addr;
    new_entry->expires = now + cache->ttl_seconds;
    new_entry->next = cache->entries[h];
    cache->entries[h] = new_entry;

    pthread_rwlock_unlock(&cache->lock);

    return 0;
}

/* Connection pool */
connection_pool_t* conn_pool_create(int max_conn, int idle_timeout) {
    connection_pool_t *pool = calloc(1, sizeof(connection_pool_t));
    pool->max_connections = max_conn;
    pool->idle_timeout = idle_timeout;
    pthread_mutex_init(&pool->lock, NULL);
    return pool;
}

void conn_pool_destroy(connection_pool_t *pool) {
    if (!pool) return;

    conn_pool_entry_t *e = pool->connections;
    while (e) {
        conn_pool_entry_t *next = e->next;
        if (e->fd >= 0) close(e->fd);
        free(e);
        e = next;
    }

    pthread_mutex_destroy(&pool->lock);
    free(pool);
}

int conn_pool_get(connection_pool_t *pool, const char *host,
                  uint16_t port, int *fd) {
    pthread_mutex_lock(&pool->lock);

    conn_pool_entry_t *e = pool->connections;
    conn_pool_entry_t *prev = NULL;
    time_t now = time(NULL);

    while (e) {
        if (!e->in_use && strcmp(e->host, host) == 0 && e->port == port) {
            if (now - e->last_used < pool->idle_timeout) {
                e->in_use = true;
                *fd = e->fd;
                pthread_mutex_unlock(&pool->lock);
                return 0;
            } else {
                /* Connection expired, remove it */
                close(e->fd);
                if (prev) {
                    prev->next = e->next;
                } else {
                    pool->connections = e->next;
                }
                free(e);
                break;
            }
        }
        prev = e;
        e = e->next;
    }

    pthread_mutex_unlock(&pool->lock);

    /* Create new connection */
    *fd = socket(AF_INET, SOCK_STREAM, 0);
    if (*fd < 0) return -1;

    int nodelay = 1;
    setsockopt(*fd, IPPROTO_TCP, TCP_NODELAY, &nodelay, sizeof(nodelay));

    return 1;  /* New connection created */
}

void conn_pool_release(connection_pool_t *pool, int fd, bool reuse) {
    if (!reuse || fd < 0) {
        if (fd >= 0) close(fd);
        return;
    }

    pthread_mutex_lock(&pool->lock);

    conn_pool_entry_t *e = pool->connections;
    while (e) {
        if (e->fd == fd) {
            e->in_use = false;
            e->last_used = time(NULL);
            pthread_mutex_unlock(&pool->lock);
            return;
        }
        e = e->next;
    }

    pthread_mutex_unlock(&pool->lock);
    close(fd);
}

/* HTTP fetch */
static int connect_to_host(const char *host, uint16_t port,
                           dns_cache_t *dns, int timeout_sec) {
    struct in_addr addr;

    if (dns_resolve(dns, host, &addr) != 0) {
        return -1;
    }

    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0) return -1;

    /* Set non-blocking for timeout support */
    int flags = fcntl(fd, F_GETFL, 0);
    fcntl(fd, F_SETFL, flags | O_NONBLOCK);

    struct sockaddr_in sin;
    memset(&sin, 0, sizeof(sin));
    sin.sin_family = AF_INET;
    sin.sin_port = htons(port);
    sin.sin_addr = addr;

    int ret = connect(fd, (struct sockaddr *)&sin, sizeof(sin));
    if (ret < 0 && errno != EINPROGRESS) {
        close(fd);
        return -1;
    }

    if (ret < 0) {
        /* Wait for connection with timeout */
        fd_set fdset;
        FD_ZERO(&fdset);
        FD_SET(fd, &fdset);

        struct timeval tv;
        tv.tv_sec = timeout_sec;
        tv.tv_usec = 0;

        ret = select(fd + 1, NULL, &fdset, NULL, &tv);
        if (ret <= 0) {
            close(fd);
            return -1;
        }

        int so_error;
        socklen_t len = sizeof(so_error);
        getsockopt(fd, SOL_SOCKET, SO_ERROR, &so_error, &len);
        if (so_error != 0) {
            close(fd);
            return -1;
        }
    }

    /* Restore blocking mode */
    fcntl(fd, F_SETFL, flags);

    return fd;
}

int http_fetch(const url_t *url, http_response_t *response,
               connection_pool_t *pool, dns_cache_t *dns,
               int timeout_ms) {
    memset(response, 0, sizeof(http_response_t));

    int fd;
    int pool_result = conn_pool_get(pool, url->host, url->port, &fd);

    if (pool_result == 1) {
        /* New connection needed */
        fd = connect_to_host(url->host, url->port, dns, timeout_ms / 1000);
        if (fd < 0) return -1;
    } else if (pool_result < 0) {
        return -1;
    }

    /* Build HTTP request */
    char request[4096];
    int req_len = snprintf(request, sizeof(request),
                          "GET %s%s%s HTTP/1.1\r\n"
                          "Host: %s\r\n"
                          "User-Agent: " USER_AGENT "\r\n"
                          "Accept: text/html,application/xhtml+xml\r\n"
                          "Accept-Language: en\r\n"
                          "Connection: keep-alive\r\n"
                          "\r\n",
                          url->path,
                          url->query[0] ? "?" : "",
                          url->query,
                          url->host);

    /* Send request */
    struct timespec start_time;
    clock_gettime(CLOCK_MONOTONIC, &start_time);

    if (send(fd, request, req_len, 0) != req_len) {
        conn_pool_release(pool, fd, false);
        return -1;
    }

    /* Read response */
    char buffer[65536];
    int total = 0;
    int n;

    while ((n = recv(fd, buffer + total, sizeof(buffer) - total - 1, 0)) > 0) {
        total += n;
        if (total >= sizeof(buffer) - 1) break;
    }

    if (total == 0) {
        conn_pool_release(pool, fd, false);
        return -1;
    }

    buffer[total] = '\0';

    struct timespec end_time;
    clock_gettime(CLOCK_MONOTONIC, &end_time);
    response->fetch_time_ms = (end_time.tv_sec - start_time.tv_sec) * 1000.0 +
                              (end_time.tv_nsec - start_time.tv_nsec) / 1000000.0;
    response->fetched_at = time(NULL);

    /* Parse status line */
    char *p = buffer;
    char *line_end = strstr(p, "\r\n");
    if (!line_end) {
        conn_pool_release(pool, fd, false);
        return -1;
    }

    *line_end = '\0';
    sscanf(p, "HTTP/1.1 %d", &response->status_code);
    p = line_end + 2;

    /* Parse headers */
    while ((line_end = strstr(p, "\r\n")) != NULL) {
        if (p == line_end) {
            p += 2;
            break;  /* End of headers */
        }

        *line_end = '\0';

        if (response->header_count < MAX_HEADERS) {
            char *colon = strchr(p, ':');
            if (colon) {
                *colon = '\0';
                strncpy(response->headers[response->header_count][0], p, 511);
                strncpy(response->headers[response->header_count][1],
                       colon + 2, 511);
                response->header_count++;

                /* Extract content type */
                if (strcasecmp(p, "Content-Type") == 0) {
                    strncpy(response->content_type, colon + 2, 127);
                }
            }
        }

        p = line_end + 2;
    }

    /* Extract body */
    response->body_len = total - (p - buffer);
    response->body = malloc(response->body_len + 1);
    memcpy(response->body, p, response->body_len);
    response->body[response->body_len] = '\0';

    /* Determine if connection can be reused */
    bool keep_alive = (response->status_code < 400);
    for (int i = 0; i < response->header_count; i++) {
        if (strcasecmp(response->headers[i][0], "Connection") == 0 &&
            strcasecmp(response->headers[i][1], "close") == 0) {
            keep_alive = false;
            break;
        }
    }

    conn_pool_release(pool, fd, keep_alive);

    return 0;
}

void http_response_free(http_response_t *response) {
    if (response->body) {
        free(response->body);
        response->body = NULL;
    }
}

/* Crawler worker thread */
static void* crawler_worker(void *arg) {
    crawler_t *crawler = arg;
    url_t url;

    while (crawler->running) {
        /* Get URL from frontier */
        pthread_mutex_lock(&crawler->frontier->lock);

        while (crawler->frontier->count == 0 && crawler->running) {
            struct timespec ts;
            clock_gettime(CLOCK_REALTIME, &ts);
            ts.tv_sec += 1;
            pthread_cond_timedwait(&crawler->frontier->not_empty,
                                   &crawler->frontier->lock, &ts);
        }

        if (!crawler->running) {
            pthread_mutex_unlock(&crawler->frontier->lock);
            break;
        }

        if (crawler->frontier->count == 0) {
            pthread_mutex_unlock(&crawler->frontier->lock);
            continue;
        }

        /* Get highest priority URL */
        int best_idx = 0;
        for (size_t i = 1; i < crawler->frontier->count; i++) {
            if (crawler->frontier->urls[i].priority <
                crawler->frontier->urls[best_idx].priority) {
                best_idx = (int)i;
            }
        }

        url = crawler->frontier->urls[best_idx];
        crawler->frontier->urls[best_idx] =
            crawler->frontier->urls[crawler->frontier->count - 1];
        crawler->frontier->count--;

        pthread_mutex_unlock(&crawler->frontier->lock);

        /* Check depth limit */
        if (url.depth > crawler->config.max_depth) continue;

        /* Check if already seen */
        char url_str[MAX_URL_LEN];
        url_to_string(&url, url_str, sizeof(url_str));

        if (bloom_check(crawler->seen_urls, url_str)) continue;
        bloom_add(crawler->seen_urls, url_str);

        /* Apply rate limiting */
        char domain_key[MAX_HOST_LEN + 8];
        url_get_domain_key(&url, domain_key, sizeof(domain_key));

        rate_limiter_t *rl = rate_limiter_get(crawler, domain_key);
        if (rl && !rate_limiter_check(rl)) {
            /* Re-queue with delay */
            url.scheduled_time = time(NULL) + 1;
            pthread_mutex_lock(&crawler->frontier->lock);
            if (crawler->frontier->count < crawler->frontier->capacity) {
                crawler->frontier->urls[crawler->frontier->count++] = url;
            }
            pthread_mutex_unlock(&crawler->frontier->lock);
            continue;
        }

        /* Fetch page */
        http_response_t response;
        if (http_fetch(&url, &response, crawler->conn_pool,
                       crawler->dns_cache, READ_TIMEOUT * 1000) == 0) {

            pthread_mutex_lock(&crawler->stats_lock);
            crawler->pages_fetched++;
            crawler->bytes_downloaded += response.body_len;
            pthread_mutex_unlock(&crawler->stats_lock);

            /* Extract links and data */
            page_data_t page = {0};
            page.url = url;
            page.response = response;
            page.title = extract_title(response.body, response.body_len);

            /* Callback */
            if (crawler->config.on_page_fetched) {
                crawler->config.on_page_fetched(&page);
            }

            /* Add new URLs to frontier */
            if (url.depth < crawler->config.max_depth) {
                url_t *links = NULL;
                int link_count = 0;
                extract_links(response.body, response.body_len, &url,
                             &links, &link_count);

                pthread_mutex_lock(&crawler->frontier->lock);
                for (int i = 0; i < link_count && i < 100; i++) {
                    if (crawler->frontier->count < crawler->frontier->capacity) {
                        links[i].depth = url.depth + 1;
                        crawler->frontier->urls[crawler->frontier->count++] = links[i];
                    }
                }
                pthread_cond_signal(&crawler->frontier->not_empty);
                pthread_mutex_unlock(&crawler->frontier->lock);

                free(links);
            }

            free(page.title);
        } else {
            pthread_mutex_lock(&crawler->stats_lock);
            crawler->pages_failed++;
            pthread_mutex_unlock(&crawler->stats_lock);

            if (crawler->config.on_error) {
                crawler->config.on_error(&url, "Fetch failed");
            }
        }

        http_response_free(&response);

        /* Politeness delay */
        usleep(crawler->config.polite_delay_ms * 1000);
    }

    return NULL;
}

/* Content extraction helpers */
char* extract_title(const char *html, size_t len) {
    const char *title_start = strcasestr(html, "<title>");
    if (!title_start) title_start = strcasestr(html, "<title ");
    if (!title_start) return NULL;

    title_start = strchr(title_start, '>') + 1;
    if (!title_start) return NULL;

    const char *title_end = strcasestr(title_start, "</title>");
    if (!title_end || title_end > html + len) return NULL;

    size_t title_len = title_end - title_start;
    char *title = malloc(title_len + 1);
    memcpy(title, title_start, title_len);
    title[title_len] = '\0';

    return title;
}

char* extract_links(const char *html, size_t len, const url_t *base_url,
                    url_t **out_links, int *count) {
    *count = 0;
    *out_links = malloc(100 * sizeof(url_t));

    const char *p = html;
    const char *end = html + len;

    while (p < end && *count < 100) {
        p = strcasestr(p, "href=");
        if (!p || p >= end) break;
        p += 5;

        char quote = *p;
        if (quote != '"' && quote != '\'') continue;
        p++;

        const char *href_end = strchr(p, quote);
        if (!href_end || href_end >= end) break;

        size_t href_len = href_end - p;
        if (href_len >= MAX_URL_LEN) href_len = MAX_URL_LEN - 1;

        char href[MAX_URL_LEN];
        memcpy(href, p, href_len);
        href[href_len] = '\0';
        p = href_end + 1;

        /* Resolve relative URLs */
        url_t new_url;
        if (href[0] == '/') {
            /* Absolute path */
            snprintf(new_url.full_url, MAX_URL_LEN, "%s://%s%s",
                    base_url->scheme, base_url->host, href);
        } else if (strncasecmp(href, "http://", 7) == 0 ||
                   strncasecmp(href, "https://", 8) == 0) {
            /* Absolute URL */
            strncpy(new_url.full_url, href, MAX_URL_LEN - 1);
        } else {
            /* Relative path */
            snprintf(new_url.full_url, MAX_URL_LEN, "%s://%s%s%s",
                    base_url->scheme, base_url->host,
                    base_url->path[0] ? base_url->path : "/",
                    href);
        }

        if (url_parse(new_url.full_url, &new_url) == 0) {
            url_normalize(&new_url);
            (*out_links)[(*count)++] = new_url;
        }
    }

    return NULL;
}

/* Rate limiter */
rate_limiter_t* rate_limiter_get(crawler_t *crawler, const char *domain) {
    pthread_mutex_lock(&crawler->stats_lock);

    for (int i = 0; i < crawler->rate_limiter_count; i++) {
        if (strcmp(crawler->rate_limiters[i]->domain, domain) == 0) {
            pthread_mutex_unlock(&crawler->stats_lock);
            return crawler->rate_limiters[i];
        }
    }

    /* Create new rate limiter */
    rate_limiter_t *rl = calloc(1, sizeof(rate_limiter_t));
    strncpy(rl->domain, domain, MAX_HOST_LEN - 1);
    rl->rate = 1.0;  /* 1 request per second default */
    rl->capacity = 1.0;
    rl->tokens = 1.0;
    rl->last_update = time(NULL);
    pthread_mutex_init(&rl->lock, NULL);

    crawler->rate_limiters[crawler->rate_limiter_count++] = rl;
    pthread_mutex_unlock(&crawler->stats_lock);

    return rl;
}

bool rate_limiter_check(rate_limiter_t *rl) {
    pthread_mutex_lock(&rl->lock);

    time_t now = time(NULL);
    double elapsed = difftime(now, rl->last_update);
    rl->tokens = fmin(rl->capacity, rl->tokens + elapsed * rl->rate);
    rl->last_update = now;

    if (rl->tokens >= 1.0) {
        rl->tokens -= 1.0;
        pthread_mutex_unlock(&rl->lock);
        return true;
    }

    pthread_mutex_unlock(&rl->lock);
    return false;
}

/* Crawler lifecycle */
crawler_t* crawler_create(const crawler_config_t *config) {
    crawler_t *crawler = calloc(1, sizeof(crawler_t));
    if (!crawler) return NULL;

    memcpy(&crawler->config, config, sizeof(crawler_config_t));

    /* Initialize components */
    crawler->frontier = calloc(1, sizeof(url_queue_t));
    crawler->frontier->capacity = 100000;
    crawler->frontier->urls = calloc(crawler->frontier->capacity, sizeof(url_t));
    pthread_mutex_init(&crawler->frontier->lock, NULL);
    pthread_cond_init(&crawler->frontier->not_empty, NULL);

    crawler->seen_urls = bloom_create(10000000, 0.01);
    crawler->dns_cache = dns_cache_create(300);
    crawler->conn_pool = conn_pool_create(1000, 60);
    crawler->rate_limiters = calloc(1000, sizeof(rate_limiter_t*));

    pthread_mutex_init(&crawler->stats_lock, NULL);
    crawler->start_time = time(NULL);

    /* Add start URL */
    if (config->start_url) {
        url_t start;
        if (url_parse(config->start_url, &start) == 0) {
            url_normalize(&start);
            start.depth = 0;
            start.priority = PRIORITY_HIGH;
            crawler->frontier->urls[crawler->frontier->count++] = start;
        }
    }

    return crawler;
}

void crawler_destroy(crawler_t *crawler) {
    if (!crawler) return;

    crawler_stop(crawler);

    free(crawler->frontier->urls);
    pthread_mutex_destroy(&crawler->frontier->lock);
    pthread_cond_destroy(&crawler->frontier->not_empty);
    free(crawler->frontier);

    bloom_destroy(crawler->seen_urls);
    dns_cache_destroy(crawler->dns_cache);
    conn_pool_destroy(crawler->conn_pool);

    for (int i = 0; i < crawler->rate_limiter_count; i++) {
        pthread_mutex_destroy(&crawler->rate_limiters[i]->lock);
        free(crawler->rate_limiters[i]);
    }
    free(crawler->rate_limiters);

    pthread_mutex_destroy(&crawler->stats_lock);
    free(crawler);
}

int crawler_add_url(crawler_t *crawler, const char *url_str, int depth) {
    url_t url;
    if (url_parse(url_str, &url) != 0) return -1;

    url_normalize(&url);
    url.depth = depth;
    url.priority = PRIORITY_NORMAL;

    pthread_mutex_lock(&crawler->frontier->lock);

    if (crawler->frontier->count < crawler->frontier->capacity) {
        crawler->frontier->urls[crawler->frontier->count++] = url;
        pthread_cond_signal(&crawler->frontier->not_empty);
    }

    pthread_mutex_unlock(&crawler->frontier->lock);

    return 0;
}

int crawler_start(crawler_t *crawler) {
    crawler->running = true;

    crawler->workers = calloc(crawler->config.max_workers, sizeof(pthread_t));

    for (int i = 0; i < crawler->config.max_workers; i++) {
        pthread_create(&crawler->workers[i], NULL, crawler_worker, crawler);
    }

    printf("Crawler started with %d workers\n", crawler->config.max_workers);

    return 0;
}

void crawler_stop(crawler_t *crawler) {
    crawler->running = false;
}

int crawler_wait(crawler_t *crawler) {
    for (int i = 0; i < crawler->config.max_workers; i++) {
        pthread_join(crawler->workers[i], NULL);
    }

    printf("\nCrawl complete.\n");
    crawler_print_stats(crawler);

    return 0;
}

void crawler_get_stats(crawler_t *crawler, crawler_stats_t *stats) {
    pthread_mutex_lock(&crawler->stats_lock);
    stats->pages_fetched = crawler->pages_fetched;
    stats->pages_failed = crawler->pages_failed;
    stats->bytes_downloaded = crawler->bytes_downloaded;
    stats->elapsed_seconds = time(NULL) - crawler->start_time;
    pthread_mutex_unlock(&crawler->stats_lock);

    stats->urls_queued = crawler->frontier->count;
}

void crawler_print_stats(crawler_t *crawler) {
    crawler_stats_t stats;
    crawler_get_stats(crawler, &stats);

    printf("=== Crawler Statistics ===\n");
    printf("Pages fetched: %lu\n", stats.pages_fetched);
    printf("Pages failed: %lu\n", stats.pages_failed);
    printf("Success rate: %.1f%%\n",
           100.0 * stats.pages_fetched /
           (stats.pages_fetched + stats.pages_failed + 1));
    printf("Bytes downloaded: %.2f MB\n", stats.bytes_downloaded / (1024.0 * 1024.0));
    printf("URLs in queue: %lu\n", stats.urls_queued);
    printf("Elapsed time: %lu seconds\n", stats.elapsed_seconds);
    printf("Pages/sec: %.1f\n", (double)stats.pages_fetched / (stats.elapsed_seconds + 1));
}

/*
 * Auto-generated from: 12_Practice_Exercises\13_Real_World_Projects.md
 * Line: 3485
 * Language: c
 * Block ID: 6517ca8e
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

#ifndef PAGER_H
#define PAGER_H

#include <stdint.h>
#include <stdbool.h>
#include <pthread.h>
#include "db.h"

/* Page flags */
#define PAGE_DIRTY      0x01
#define PAGE_PINNED     0x02
#define PAGE_IN_JOURNAL 0x04

/* Page structure */
typedef struct page {
    uint32_t number;
    uint8_t data[PAGE_SIZE];
    int flags;
    int ref_count;
    struct page *lru_next;
    struct page *lru_prev;
    struct page *hash_next;
} page_t;

/* Page cache */
typedef struct pager {
    int fd;
    uint32_t page_count;
    uint32_t orig_size;  /* Size at transaction start */

    /* Cache */
    page_t **pages;  /* Hash table */
    page_t *lru_head;
    page_t *lru_tail;
    int cache_size;
    int used_pages;

    /* Journal */
    int journal_fd;
    uint32_t *journal_pages;
    int journal_count;

    /* Locking */
    pthread_mutex_t lock;
    bool in_transaction;
    bool read_only;
} pager_t;

/* Pager API */
pager_t* pager_open(const char *path, int cache_size);
void pager_close(pager_t *pager);
page_t* pager_get_page(pager_t *pager, uint32_t page_num);
page_t* pager_allocate_page(pager_t *pager);
void pager_release_page(pager_t *pager, page_t *page);
void pager_mark_dirty(pager_t *pager, page_t *page);
int pager_sync(pager_t *pager);

/* Transaction support */
int pager_begin_transaction(pager_t *pager);
int pager_commit(pager_t *pager);
int pager_rollback(pager_t *pager);
int pager_write_journal_header(pager_t *pager);
int pager_sync_journal(pager_t *pager);

/* File format */
#define DB_MAGIC 0x4442494D  /* "DBIM" */
#define DB_HEADER_SIZE 100

typedef struct db_header {
    uint32_t magic;
    uint32_t version;
    uint32_t page_size;
    uint32_t page_count;
    uint32_t free_page_list;
    uint32_t schema_page;
    uint64_t transaction_id;
    uint8_t reserved[DB_HEADER_SIZE - 32];
} db_header_t;

int pager_read_header(pager_t *pager, db_header_t *header);
int pager_write_header(pager_t *pager, const db_header_t *header);

#endif /* PAGER_H */

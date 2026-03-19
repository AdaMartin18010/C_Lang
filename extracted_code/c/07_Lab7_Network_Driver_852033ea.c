/*
 * Auto-generated from: 04_Industrial_Scenarios\16_MIT_6S081_Labs\07_Lab7_Network_Driver.md
 * Line: 443
 * Language: c
 * Block ID: 852033ea
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// kernel/mbuf.c

struct mbuf *
mbufalloc(uint headroom)
{
    struct mbuf *m;

    if (headroom > MBUF_SIZE)
        return 0;

    m = kalloc();
    if (!m)
        return 0;

    m->next = 0;
    m->head = m->buf + headroom;
    m->len = 0;

    return m;
}

void
mbuffree(struct mbuf *m)
{
    kfree((void *)m);
}

char *
mbufpush(struct mbuf *m, uint len)
{
    m->head -= len;
    m->len += len;
    return m->head;
}

char *
mbufput(struct mbuf *m, uint len)
{
    char *tail = m->head + m->len;
    m->len += len;
    return tail;
}

char *
mbufpull(struct mbuf *m, uint len)
{
    char *head = m->head;
    m->head += len;
    m->len -= len;
    return head;
}

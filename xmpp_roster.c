#include <strophe.h>
#include <stdio.h>
#include <string.h>
#include "xmpp_roster.h"

// Get the user roster.
int roster_result_handler(
    xmpp_conn_t *const conn,
    xmpp_stanza_t *const stanza,
    void *const userdata)
{
    xmpp_stanza_t *query, *next;

    query = xmpp_stanza_get_child_by_name(stanza, "query");
    if (query)
        next = xmpp_stanza_get_children(query);
    while (next)
    {
        const char *roster_jid = xmpp_stanza_get_attribute(next, "jid");
        if (roster_jid)
            fprintf(stderr, "DEBUG: ROSTER has contact '%s'.\n", roster_jid);
        next = xmpp_stanza_get_next(next);
    }

    return 1;
}

// Send IQ stanza to get the users roster.
void get_roster(xmpp_conn_t *const conn, xmpp_ctx_t *const ctx)
{
    xmpp_stanza_t *iq, *query;
    iq = xmpp_iq_new(ctx, "get", "get_roster");
    query = xmpp_stanza_new(ctx);
    xmpp_stanza_set_name(query, "query");
    xmpp_stanza_set_ns(query, XMPP_NS_ROSTER);
    xmpp_stanza_add_child(iq, query);
    xmpp_stanza_release(query);
    xmpp_send(conn, iq);
    xmpp_stanza_release(iq);
}

int presence_subscription_handler(
    xmpp_conn_t *const conn,
    xmpp_stanza_t *const stanza,
    void *const userdata)
{
    xmpp_ctx_t *ctx = xmpp_conn_get_context(conn);
    const char *jid = xmpp_conn_get_bound_jid(conn);
    const char *name = xmpp_stanza_get_name(stanza);
    const char *type = xmpp_stanza_get_type(stanza);

    // someone wants to be my friend :)
    if (type && strcmp(type, "subscribe") == 0)
    {
        const char *from = xmpp_stanza_get_from(stanza);
        fprintf(stderr, "DEBUG: '%s' wants to be my friend!\n", from);
        xmpp_stanza_t *subscribed = xmpp_presence_new(ctx);
        xmpp_stanza_set_from(subscribed, jid);
        xmpp_stanza_set_to(subscribed, from);
        xmpp_stanza_set_type(subscribed, "subscribed");
        xmpp_send(conn, subscribed);
        xmpp_stanza_release(subscribed);
    }

    return 1;
}
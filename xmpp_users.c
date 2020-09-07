#include <strophe.h>
#include <stdio.h>
#include "xmpp_utils.h"
#include "xmpp_users.h"

// Use the search service to get all users.
void get_all_users(xmpp_conn_t *const conn, xmpp_ctx_t *const ctx)
{
    const char *jid = xmpp_conn_get_bound_jid(conn);
    xmpp_stanza_t *iq_search = xmpp_iq_new(ctx, "set", "search_result");
    xmpp_stanza_t *query_search = xmpp_stanza_new(ctx);
    xmpp_stanza_t *query_email = xmpp_stanza_new(ctx);
    xmpp_stanza_t *email_text = xmpp_stanza_new(ctx);
    xmpp_stanza_set_to(iq_search, "search.redes2020.xyz");
    xmpp_stanza_set_from(iq_search, jid);
    xmpp_stanza_set_name(query_search, "query");
    xmpp_stanza_set_ns(query_search, XMPP_NS_JABBER_SEARCH);
    xmpp_stanza_set_name(query_email, "email");
    xmpp_stanza_set_text(email_text, "*");
    xmpp_stanza_add_child(query_email, email_text);
    xmpp_stanza_add_child(query_search, query_email);
    xmpp_stanza_add_child(iq_search, query_search);
    xmpp_send(conn, iq_search);
    xmpp_stanza_release(email_text);
    xmpp_stanza_release(query_email);
    xmpp_stanza_release(query_search);
    xmpp_stanza_release(iq_search);
}

int search_result_handler(
    xmpp_conn_t *const conn,
    xmpp_stanza_t *const stanza,
    void *const userdata)
{
    xmpp_stanza_t *query = NULL;
    xmpp_stanza_t *next = NULL;
    xmpp_stanza_t *item = NULL;

    query = xmpp_stanza_get_child_by_name(stanza, "query");
    if (query)
        next = xmpp_stanza_get_children(query);
    while (next)
    {
        const char *jid = xmpp_stanza_get_attribute(next, "jid");
        if (jid)
            fprintf(stderr, "DEBUG: found user '%s'\n", jid);
        next = xmpp_stanza_get_next(next);
    }

    return 1;
}
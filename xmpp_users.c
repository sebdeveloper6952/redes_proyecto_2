#include <strophe.h>
#include <stdio.h>
#include <string.h>
#include "xmpp_utils.h"
#include "xmpp_users.h"

// Use the search service to get all users.
void get_all_users(xmpp_conn_t *const conn)
{
    xmpp_ctx_t *ctx = xmpp_conn_get_context(conn);
    xmpp_stanza_t *iq, *query, *field, *text;
    const char *jid = xmpp_conn_get_bound_jid(conn);
    const char *fields[4] = {"email", "first", "last", "nick"};

    iq = xmpp_iq_new(ctx, "set", XMPP_ID_SEARCH_USERS);
    query = xmpp_stanza_new(ctx);
    xmpp_stanza_set_from(iq, xmpp_conn_get_bound_jid(conn));
    xmpp_stanza_set_to(iq, "search.redes2020.xyz");
    xmpp_stanza_set_name(query, "query");
    xmpp_stanza_set_ns(query, XMPP_NS_JABBER_SEARCH);

    for (int i = 0; i < 4; i++)
    {
        field = xmpp_stanza_new(ctx);
        text = xmpp_stanza_new(ctx);
        xmpp_stanza_set_text(text, "*");
        xmpp_stanza_add_child(field, text);
        xmpp_stanza_release(text);
        xmpp_stanza_set_name(field, fields[i]);
        xmpp_stanza_add_child(query, field);
        xmpp_stanza_release(field);
    }

    xmpp_stanza_add_child(iq, query);
    xmpp_stanza_release(query);
    xmpp_send(conn, iq);
    xmpp_stanza_release(iq);
}

int search_result_handler(
    xmpp_conn_t *const conn,
    xmpp_stanza_t *const stanza,
    void *const userdata)
{
    xmpp_stanza_t *query = NULL;
    xmpp_stanza_t *next = NULL;
    xmpp_stanza_t *item = NULL;
    my_data *data = (my_data *)userdata;
    char users_buf[64 * 37];

    query = xmpp_stanza_get_child_by_name(stanza, "query");
    if (query)
        next = xmpp_stanza_get_children(query);
    while (next)
    {
        const char *jid = xmpp_stanza_get_attribute(next, "jid");
        if (jid)
        {
            strcat(users_buf, " * ");
            strcat(users_buf, jid);
            strcat(users_buf, "\n");
        }
        next = xmpp_stanza_get_next(next);
    }

    // callback
    data->cb(users_buf);

    return 0;
}

int search_discovery_handler(
    xmpp_conn_t *const conn,
    xmpp_stanza_t *const stanza,
    void *const userdata)
{
    return 0;
}

void search_discovery(xmpp_conn_t *const conn)
{
    xmpp_ctx_t *ctx = xmpp_conn_get_context(conn);
    xmpp_stanza_t *iq, *query;
    iq = xmpp_iq_new(ctx, "get", "search_discovery");
    query = xmpp_stanza_new(ctx);
    xmpp_stanza_set_from(iq, xmpp_conn_get_bound_jid(conn));
    xmpp_stanza_set_to(iq, "search.redes2020.xyz");
    xmpp_stanza_set_name(query, "query");
    xmpp_stanza_set_ns(query, XMPP_NS_JABBER_SEARCH);
    xmpp_stanza_add_child(iq, query);
    xmpp_stanza_release(query);
    xmpp_send(conn, iq);
    xmpp_stanza_release(iq);
}
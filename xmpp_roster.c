#include <strophe.h>
#include <stdio.h>
#include <string.h>
#include "xmpp_roster.h"
#include "xmpp_utils.h"

// Get the user roster.
int roster_result_handler(
    xmpp_conn_t *const conn,
    xmpp_stanza_t *const stanza,
    void *const userdata)
{
    xmpp_stanza_t *query, *next;
    my_data *data = (my_data *)userdata;
    char roster_buf[64 * 37];

    memset(roster_buf, 0, sizeof(roster_buf));
    query = xmpp_stanza_get_child_by_name(stanza, "query");
    if (query)
        next = xmpp_stanza_get_children(query);
    while (next)
    {
        const char *roster_jid = xmpp_stanza_get_attribute(next, "jid");
        // if roster contact is valid
        if (roster_jid)
        {
            strcat(roster_buf, " * ");
            strcat(roster_buf, roster_jid);
            strcat(roster_buf, "\n");
        }

        next = xmpp_stanza_get_next(next);
    }

    // callback
    data->cb(roster_buf);

    return 0;
}

// Send IQ stanza to get the users roster.
void get_roster(xmpp_conn_t *const conn)
{
    // send iq
    xmpp_ctx_t *ctx = xmpp_conn_get_context(conn);
    xmpp_stanza_t *iq, *query;
    iq = xmpp_iq_new(ctx, "get", XMPP_ID_GET_ROSTER);
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

void send_subscription_request(xmpp_conn_t *const conn, const char *jid)
{
    xmpp_ctx_t *ctx = xmpp_conn_get_context(conn);
    xmpp_stanza_t *p = xmpp_presence_new(ctx);
    xmpp_stanza_set_from(p, xmpp_conn_get_bound_jid(conn));
    xmpp_stanza_set_to(p, jid);
    xmpp_stanza_set_type(p, "subscribe");
    xmpp_send(conn, p);
    xmpp_stanza_release(p);
}
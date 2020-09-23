#include <strophe.h>
#include <stdio.h>
#include <string.h>
#include "xmpp_utils.h"
#include "xmpp_users.h"

// Use the search service to get all users.
void get_all_users(xmpp_conn_t *const conn)
{
    xmpp_send_raw_string(conn,
                         "<iq type='set'"
                         "from='%s'"
                         "to='search.redes2020.xyz'"
                         "id='search_result'"
                         "xml:lang='en'>"
                         "<query xmlns='jabber:iq:search'>"
                         "<x xmlns='jabber:x:data' type='submit'>"
                         "<field type='hidden' var='FORM_TYPE'>"
                         "<value>jabber:iq:search</value>"
                         " </field>"
                         "<field var='Username'>"
                         "<value>1</value>"
                         "</field>"
                         "<field var='search'>"
                         "<value>*</value>"
                         "</field>"
                         "</x>"
                         "</query>"
                         "</iq>",
                         xmpp_conn_get_bound_jid(conn));
}

int search_result_handler(
    xmpp_conn_t *const conn,
    xmpp_stanza_t *const stanza,
    void *const userdata)
{
    xmpp_ctx_t *ctx;
    xmpp_stanza_t *query = NULL;
    xmpp_stanza_t *next = NULL;
    xmpp_stanza_t *item = NULL;
    my_data *data = (my_data *)userdata;
    char users_buf[64 * 32];
    unsigned char i = 0;

    ctx = xmpp_conn_get_context(conn);
    query = xmpp_stanza_get_child_by_name(stanza, "query");
    if (query)
        query = xmpp_stanza_get_child_by_name(query, "x");
    if (query)
        next = xmpp_stanza_get_children(query);

    while (next)
    {
        if (strcmp(xmpp_stanza_get_name(next), "item") == 0)
        {
            xmpp_stanza_t *item_child = xmpp_stanza_get_children(next);
            xmpp_stanza_t *value;
            while (item_child)
            {
                if (strcmp(xmpp_stanza_get_attribute(item_child, "var"), "jid") == 0)
                {
                    value = xmpp_stanza_get_child_by_name(item_child, "value");
                    char *value_s = xmpp_stanza_get_text(value);
                    strcat(users_buf, " * ");
                    strcat(users_buf, value_s);
                    strcat(users_buf, "\n");
                    xmpp_free(ctx, value_s);
                }

                item_child = xmpp_stanza_get_next(item_child);
            }
        }

        if (++i == 32)
            break;
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
    iq = xmpp_iq_new(ctx, XMPP_TYPE_GET, "search_discovery");
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
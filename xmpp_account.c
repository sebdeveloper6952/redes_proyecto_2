#include <strophe.h>
#include <stdio.h>
#include "xmpp_account.h"
#include "xmpp_utils.h"

int delete_account_handler(
    xmpp_conn_t *const conn,
    xmpp_stanza_t *const stanza,
    void *const userdata)
{
    xmpp_ctx_t *ctx;
    my_data *data;

    ctx = xmpp_conn_get_context(conn);
    data = (my_data *)userdata;

    // fprintf(stderr, "DEBUG: account was deleted from server.\n");
    if (data && data->cb)
        data->cb("ACCOUNT DELETED.");

    xmpp_disconnect(conn);
    xmpp_stop(ctx);

    return 0;
}

void delete_account(xmpp_conn_t *const conn, void(*on_result))
{
    xmpp_ctx_t *ctx;
    xmpp_stanza_t *iq, *query, *remove;
    my_data *data;

    ctx = xmpp_conn_get_context(conn);
    data = new_data();
    data->cb = on_result;
    xmpp_id_handler_add(conn, delete_account_handler, "delete_account", data);

    iq = xmpp_iq_new(ctx, "set", "delete_account");
    query = xmpp_stanza_new(ctx);
    remove = xmpp_stanza_new(ctx);
    xmpp_stanza_set_name(remove, "remove");
    xmpp_stanza_add_child(query, remove);
    xmpp_stanza_release(remove);
    xmpp_stanza_set_name(query, "query");
    xmpp_stanza_set_ns(query, XMPP_NS_REGISTER);
    xmpp_stanza_add_child(iq, query);
    xmpp_stanza_release(query);
    xmpp_send(conn, iq);
    xmpp_stanza_release(iq);
}
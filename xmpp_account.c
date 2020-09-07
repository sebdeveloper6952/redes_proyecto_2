#include <strophe.h>
#include <stdio.h>
#include "xmpp_account.h"

int delete_account_handler(
    xmpp_conn_t *const conn,
    xmpp_stanza_t *const stanza,
    void *const userdata)
{
    xmpp_ctx_t *ctx = (xmpp_ctx_t *)userdata;

    fprintf(stderr, "DEBUG: account was deleted from server.\n");
    xmpp_disconnect(conn);
    xmpp_stop(ctx);

    return 0;
}

void delete_account(xmpp_conn_t *const conn, xmpp_ctx_t *const ctx)
{
    xmpp_stanza_t *iq, *query, *remove;
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
    xmpp_id_handler_add(conn, delete_account_handler, "delete_account", ctx);
    xmpp_send(conn, iq);
    xmpp_stanza_release(iq);
}
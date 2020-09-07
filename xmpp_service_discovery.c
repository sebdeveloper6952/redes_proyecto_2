#include <strophe.h>
#include <stdio.h>
#include "xmpp_service_discovery.h"

int service_discovery_handler(
    xmpp_conn_t *const conn,
    xmpp_stanza_t *const stanza,
    void *const userdata)
{
    xmpp_ctx_t *ctx = (xmpp_ctx_t *)userdata;
    xmpp_stanza_t *query = NULL;
    xmpp_stanza_t *next = NULL;
    const char *jid = NULL;

    fprintf(stderr, "DEBUG: service discovery response.\n");
    jid = xmpp_conn_get_bound_jid(conn);
    query = xmpp_stanza_get_child_by_name(stanza, "query");
    if (query)
        next = xmpp_stanza_get_children(query);
    while (next)
    {
        const char *service_jid = xmpp_stanza_get_attribute(next, "jid");
        const char *att = xmpp_stanza_get_attribute(next, "name");
        fprintf(stderr, "DEBUG: querying service '%s' with jid '%s'.\n", att, jid);
        xmpp_stanza_t *iq = xmpp_iq_new(ctx, "get", NULL);
        xmpp_stanza_t *query0 = xmpp_stanza_new(ctx);
        xmpp_stanza_set_from(iq, jid);
        xmpp_stanza_set_to(iq, service_jid);
        xmpp_stanza_set_name(query0, "query");
        xmpp_stanza_set_ns(query0, XMPP_NS_DISCO_INFO);
        xmpp_stanza_add_child(iq, query0);
        xmpp_send(conn, iq);
        xmpp_stanza_release(query0);
        xmpp_stanza_release(iq);
        next = xmpp_stanza_get_next(next);
    }

    return 0;
}
#include <strophe.h>
#include <stdio.h>
#include <string.h>
#include "xmpp_gm.h"
#include "xmpp_utils.h"

int gm_msg_handler(xmpp_conn_t *const conn, xmpp_stanza_t *const st, void *const data)
{
    xmpp_stanza_t *body;
    char msg[1024];
    const char *from;

    body = xmpp_stanza_get_child_by_name(st, "body");
    from = xmpp_stanza_get_attribute(st, "from");

    strcat(msg, "\n\n******* GROUP MSG *******\n");
    strcat(msg, "* FROM: ");
    strcat(msg, from);
    strcat(msg, "\n");
    strcat(msg, "* BODY: ");
    if (body)
        strcat(msg, xmpp_stanza_get_text(body));
    strcat(msg, "\n***************************\n");

    fprintf(stderr, "%s\n", msg);

    return 1;
}

void join_gm_room(xmpp_conn_t *const conn, const char *room_name, const char *nickname)
{
    xmpp_ctx_t *ctx;
    xmpp_stanza_t *p;
    char room_jid[256] = {};

    strcat(room_jid, room_name);
    strcat(room_jid, "@conference.redes2020.xyz/");
    strcat(room_jid, nickname);
    ctx = xmpp_conn_get_context(conn);
    p = xmpp_presence_new(ctx);
    xmpp_stanza_set_from(p, xmpp_conn_get_bound_jid(conn));
    xmpp_stanza_set_to(p, room_jid);
    xmpp_send(conn, p);
    xmpp_stanza_release(p);
}

void send_gm_msg(xmpp_conn_t *const conn, const char *group_jid, const char *body)
{
    xmpp_ctx_t *ctx = xmpp_conn_get_context(conn);
    xmpp_stanza_t *msg;

    msg = xmpp_message_new(ctx, XMPP_TYPE_GROUPCHAT, group_jid, NULL);
    xmpp_stanza_set_from(msg, xmpp_conn_get_bound_jid(conn));
    xmpp_message_set_body(msg, body);

    xmpp_send(conn, msg);
    xmpp_stanza_release(msg);
}
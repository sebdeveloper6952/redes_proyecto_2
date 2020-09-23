#include <strophe.h>
#include <stdio.h>
#include <string.h>
#include "xmpp_gm.h"
#include "xmpp_utils.h"

int gm_msg_handler(xmpp_conn_t *const conn, xmpp_stanza_t *const st, void *const userdata)
{
    my_data *data = (my_data *)userdata;
    xmpp_stanza_t *st_body;
    char msg[1024];
    const char *from;
    char *c_body = NULL;

    st_body = xmpp_stanza_get_child_by_name(st, "body");
    from = xmpp_stanza_get_attribute(st, "from");

    if (st_body)
        c_body = xmpp_stanza_get_text(st_body);
    if (data->msg_cb != NULL)
        data->msg_cb(from ? from : "", c_body ? c_body : "");

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

void leave_gm_room(xmpp_conn_t *const conn, const char *room)
{
    xmpp_ctx_t *ctx;
    xmpp_stanza_t *st_p;
    char room_jid[256] = {};

    strcat(room_jid, room);
    strcat(room_jid, "@conference.redes2020.xyz");
    ctx = xmpp_conn_get_context(conn);
    st_p = xmpp_presence_new(ctx);
    xmpp_stanza_set_type(st_p, "unavailable");
    xmpp_stanza_set_from(st_p, xmpp_conn_get_bound_jid(conn));
    xmpp_stanza_set_to(st_p, room_jid);
    xmpp_send(conn, st_p);
    xmpp_stanza_release(st_p);
}

void send_gm_msg(xmpp_conn_t *const conn, const char *jid, const char *body)
{
    xmpp_ctx_t *ctx = xmpp_conn_get_context(conn);
    xmpp_stanza_t *msg;
    char group_jid[256] = {};

    strcat(group_jid, jid);
    strcat(group_jid, "@conference.redes2020.xyz");

    msg = xmpp_message_new(ctx, XMPP_TYPE_GROUPCHAT, group_jid, NULL);
    xmpp_stanza_set_from(msg, xmpp_conn_get_bound_jid(conn));
    xmpp_message_set_body(msg, body);

    xmpp_send(conn, msg);
    xmpp_stanza_release(msg);
}
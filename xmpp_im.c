#include <strophe.h>
#include <stdio.h>
#include <string.h>
#include "xmpp_im.h"
#include "xmpp_utils.h"

int im_handler(xmpp_conn_t *const conn, xmpp_stanza_t *const st, void *const userdata)
{
    xmpp_ctx_t *ctx;
    xmpp_stanza_t *body;
    my_data *data = (my_data *)userdata;
    char msg[1024] = {};
    const char *from;
    char *msg_body;

    ctx = xmpp_conn_get_context(conn);
    body = xmpp_stanza_get_child_by_name(st, "body");
    from = xmpp_stanza_get_attribute(st, "from");

    if (body && data->msg_cb)
    {
        msg_body = xmpp_stanza_get_text(body);
        data->msg_cb(from, msg_body ? msg_body : "");
        xmpp_free(ctx, msg_body);
    }

    return 1;
}

void send_im_msg(xmpp_conn_t *const conn, const char *jid_to, const char *body)
{
    xmpp_ctx_t *ctx = xmpp_conn_get_context(conn);
    xmpp_stanza_t *msg;

    msg = xmpp_message_new(ctx, XMPP_TYPE_CHAT, jid_to, NULL);
    xmpp_message_set_body(msg, body);

    xmpp_send(conn, msg);
    xmpp_stanza_release(msg);
}
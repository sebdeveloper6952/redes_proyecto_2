#include <strophe.h>
#include <stdio.h>
#include <string.h>
#include "xmpp_im.h"
#include "xmpp_utils.h"

int im_handler(xmpp_conn_t *const conn, xmpp_stanza_t *const st, void *const data)
{
    xmpp_stanza_t *body;
    char msg[1024];
    const char *from;

    body = xmpp_stanza_get_child_by_name(st, "body");
    from = xmpp_stanza_get_attribute(st, "from");

    strcat(msg, "\n\n******* PRIVATE MSG *******\n");
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

void send_im_msg(xmpp_conn_t *const conn, const char *jid_to, const char *body)
{
    xmpp_ctx_t *ctx = xmpp_conn_get_context(conn);
    xmpp_stanza_t *msg;

    msg = xmpp_message_new(ctx, XMPP_TYPE_CHAT, jid_to, NULL);
    xmpp_message_set_body(msg, body);

    xmpp_send(conn, msg);
    xmpp_stanza_release(msg);
}
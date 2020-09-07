#include <strophe.h>
#include <stdio.h>
#include "xmpp_presence.h"

// Sends a <presence/> stanza to indicate we are available.
void send_logged_in_presence(xmpp_conn_t *const conn, xmpp_ctx_t *const ctx)
{
    xmpp_stanza_t *presence, *status, *text;
    presence = xmpp_presence_new(ctx);
    status = xmpp_stanza_new(ctx);
    text = xmpp_stanza_new(ctx);
    xmpp_stanza_set_name(status, "status");
    xmpp_stanza_set_text(text, "Ola jeje");
    xmpp_stanza_add_child(status, text);
    xmpp_stanza_release(text);
    xmpp_stanza_add_child(presence, status);
    xmpp_stanza_release(status);
    xmpp_send(conn, presence);
    xmpp_stanza_release(presence);
}
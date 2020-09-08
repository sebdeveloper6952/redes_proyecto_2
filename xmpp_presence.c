#include <strophe.h>
#include <stdio.h>
#include <string.h>
#include "xmpp_presence.h"

int presence_handler(xmpp_conn_t *const conn, xmpp_stanza_t *const stanza, void *const data)
{
    xmpp_ctx_t *ctx;
    xmpp_stanza_t *st;
    const char *from;
    char *show = NULL, *status = NULL;

    ctx = xmpp_conn_get_context(conn);
    from = xmpp_stanza_get_attribute(stanza, "from");
    st = xmpp_stanza_get_child_by_name(stanza, "show");
    if (st)
        show = xmpp_stanza_get_text(st);

    st = xmpp_stanza_get_child_by_name(stanza, "status");
    if (st)
        status = xmpp_stanza_get_text(st);

    fprintf(stderr, "DEBUG: GOT <presence/> from %s, show(%s) - status(%s).\n",
            from, show ? show : "-", status ? status : "-");

    if (show)
        xmpp_free(ctx, show);
    if (status)
        xmpp_free(ctx, status);

    return 1;
}

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

void change_presence(xmpp_conn_t *const conn, xmpp_ctx_t *const ctx, status_t st, const char *status)
{
    xmpp_stanza_t *p, *sh, *s, *t;
    p = xmpp_presence_new(ctx);
    sh = xmpp_stanza_new(ctx);
    xmpp_stanza_set_name(sh, "show");
    t = xmpp_stanza_new(ctx);

    if (st == chat)
        xmpp_stanza_set_text(t, "chat");
    if (st == away)
        xmpp_stanza_set_text(t, "away");
    if (st == xa)
        xmpp_stanza_set_text(t, "xa");
    if (st == dnd)
        xmpp_stanza_set_text(t, "dnd");
    xmpp_stanza_add_child(sh, t);
    xmpp_stanza_release(t);
    xmpp_stanza_add_child(p, sh);
    xmpp_stanza_release(sh);

    if (strlen(status) > 0)
    {
        s = xmpp_stanza_new(ctx);
        t = xmpp_stanza_new(ctx);
        xmpp_stanza_set_name(s, "status");
        xmpp_stanza_set_text(t, status);
        xmpp_stanza_add_child(s, t);
        xmpp_stanza_release(t);
        xmpp_stanza_add_child(p, s);
        xmpp_stanza_release(s);
    }

    xmpp_send(conn, p);
    xmpp_stanza_release(p);
}
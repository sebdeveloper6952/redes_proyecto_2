#include <strophe.h>

#ifndef XMPP_PRESENCE
#define XMPP_PRESENCE
enum av_status
{
    available,
    chat,
    away,
    xa,
    dnd
};
typedef enum av_status status_t;
int presence_handler(xmpp_conn_t *const conn, xmpp_stanza_t *const st, void *const data);
int my_presence_handler(xmpp_conn_t *const conn, xmpp_stanza_t *const st, void *const data);
void send_logged_in_presence(xmpp_conn_t *const conn, xmpp_ctx_t *const ctx);
void change_presence(xmpp_conn_t *const conn, status_t st, const char *status);
#endif
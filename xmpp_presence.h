#include <strophe.h>

int presence_handler(xmpp_conn_t *const conn, xmpp_stanza_t *const st, void *const data);
void send_logged_in_presence(xmpp_conn_t *const conn, xmpp_ctx_t *const ctx);
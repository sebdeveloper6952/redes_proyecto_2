#include <strophe.h>

int delete_account_handler(xmpp_conn_t *const conn, xmpp_stanza_t *const st, void *const data);
void delete_account(xmpp_conn_t *const conn, xmpp_ctx_t *const ctx);
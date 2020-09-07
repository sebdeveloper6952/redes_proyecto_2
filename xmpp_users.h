#include <strophe.h>

void get_all_users(xmpp_conn_t *const conn, xmpp_ctx_t *const ctx);
int search_result_handler(
    xmpp_conn_t *const conn,
    xmpp_stanza_t *const stanza,
    void *const userdata);
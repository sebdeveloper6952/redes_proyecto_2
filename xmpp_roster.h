#include <strophe.h>

#define GET_ROSTER_ID "get_roster"

int roster_result_handler(xmpp_conn_t *const conn, xmpp_stanza_t *const st, void *const data);
int presence_subscription_handler(xmpp_conn_t *const conn, xmpp_stanza_t *const st, void *const data);
void get_roster(xmpp_conn_t *const conn, xmpp_ctx_t *const ctx);
void send_subscription_request(xmpp_conn_t *const conn, xmpp_ctx_t *const ctx, const char *jid_to);
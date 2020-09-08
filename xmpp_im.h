#include <strophe.h>

int im_handler(xmpp_conn_t *const conn, xmpp_stanza_t *const st, void *const data);
void send_im_message(xmpp_conn_t *const conn, const char *jid_to, const char *msg);
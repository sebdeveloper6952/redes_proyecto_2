#include <strophe.h>

int vcard_handler(xmpp_conn_t *const conn, xmpp_stanza_t *const stanza, void *const data);
void get_vcard(xmpp_conn_t *const conn, const char *jid);
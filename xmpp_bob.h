#include <strophe.h>

int xmpp_bob_img_recv_handler(xmpp_conn_t *const conn, xmpp_stanza_t *const st, void *const data);
void xmpp_bob_send_image(xmpp_conn_t *const conn, const char *jid, const char *path, void *const data);
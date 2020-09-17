#include <strophe.h>

void iq_get_from_proxy(xmpp_conn_t *const conn, const char *service);
int file_transfer_init_handler(xmpp_conn_t *const conn, xmpp_stanza_t *const st, void *const data);
void file_transfer_init_response(
    xmpp_conn_t *const conn,
    const char *iq_id,
    const char *stream_id,
    const char *jid_to);
void start_file_transfer(xmpp_conn_t *const conn, const char *jid, const char *host, long port, void *const userdata);
int file_transfer_streamhost_handler(xmpp_conn_t *const conn, xmpp_stanza_t *const st, void *const data);
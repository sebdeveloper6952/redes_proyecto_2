#include <strophe.h>

int gm_msg_handler(xmpp_conn_t *const conn, xmpp_stanza_t *const st, void *const data);
void join_gm_room(xmpp_conn_t *const conn, const char *room_name, const char *nickname);
void send_gm_msg(xmpp_conn_t *const conn, const char *group_jid, const char *body);
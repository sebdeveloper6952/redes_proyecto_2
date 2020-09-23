#include <strophe.h>
#include "xmpp_presence.h"

void xmpp_client_login(const char *host, const char *jid, const char *pass, void(*on_login));
void xmpp_client_logout();
void xmpp_client_get_users(void(*on_result));
void xmpp_client_get_roster(void(*on_result));
void xmpp_client_add_to_roster(const char *jid);
void xmpp_client_set_presence(status_t av, const char *status, void(*on_result));
void xmpp_client_send_msg(char is_p, const char *jid, const char *msg);
void xmpp_client_add_priv_msg_handler(void(*on_result));
void xmpp_client_join_group_chat(const char *group_jid, const char *nick);
void xmpp_client_add_gm_msg_handler(void(*on_result));
void xmpp_client_add_presence_handler(void(*on_result));
void xmpp_client_get_vcard(const char *jid);
void xmpp_client_add_vcard_handler(void(*on_result));
void xmpp_client_delete_account(void(*on_result));
void xmpp_client_add_subscription_handler(void(*on_result));
void xmpp_client_add_img_recv_handler(void(*on_result));
void xmpp_client_send_img(const char *jid_to, const char *path, void(*on_result));
void xmpp_client_offer_file(const char *path, const char *jid_to, void(*on_result));
void xmpp_client_add_file_offer_handler(void(*on_result));
void xmpp_client_add_file_offer_recv_handler(void(*on_result));
void xmpp_client_offer_streamhost(const char *jid_to, void(*on_result));
void xmpp_client_add_streamhost_offer_handler(void(*on_result));
void xmpp_client_add_ibb_offer_recv_handler(void(*on_result));

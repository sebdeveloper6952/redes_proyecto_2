#include <strophe.h>

void xmpp_login(const char *jid, const char *pass, void(*on_login));
void xmpp_logout();

// public API
void xmpp_client_get_users(void(*on_result));
void xmpp_client_get_roster(void(*on_result));
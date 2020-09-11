#include <strophe.h>

#ifndef XMPP_UTILS
#define XMPP_UTILS
#define XMPP_NS_JABBER_SEARCH "jabber:iq:search"
#define XMPP_ST_MESSAGE "message"
#define XMPP_ST_PRESENCE "presence"
#define XMPP_TYPE_CHAT "chat"
#define XMPP_TYPE_GROUPCHAT "groupchat"
#define XMPP_TYPE_SUBSCRIBE "subscribe"
#define XMPP_ID_SEARCH_USERS "search_result"
#define XMPP_ID_GET_ROSTER "get_roster"

typedef struct
{
    xmpp_ctx_t *ctx;
    const char *jid;
} xmpp_reg_t;

typedef struct
{
    xmpp_ctx_t *ctx;
    void (*cb)(const char *data);
} my_data;

void print_menu();
void get_jid_and_pass(char *jid, char *pass);
#endif
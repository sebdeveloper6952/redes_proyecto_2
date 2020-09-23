#include <strophe.h>

#ifndef XMPP_UTILS
#define XMPP_UTILS
#define XMPP_NS_REGISTER "jabber:iq:register"
#define XMPP_NS_JABBER_SEARCH "jabber:iq:search"
#define XMPP_NS_X_DATA "jabber:x:data"
#define XMPP_NS_FEAT_NEG "http://jabber.org/protocol/feature-neg"
#define XMPP_NS_FILE_TRANSFER "http://jabber.org/protocol/si/profile/file-transfer"
#define XMPP_NS_PROT_SI "http://jabber.org/protocol/si"
#define XMPP_NS_PROT_BYTESTREAMS "http://jabber.org/protocol/bytestreams"
#define XMPP_NS_PROT_IBB "http://jabber.org/protocol/ibb"
#define XMPP_ST_IQ "iq"
#define XMPP_ST_MESSAGE "message"
#define XMPP_ST_PRESENCE "presence"
#define XMPP_TYPE_CHAT "chat"
#define XMPP_TYPE_GROUPCHAT "groupchat"
#define XMPP_TYPE_NORMAL "normal"
#define XMPP_TYPE_SUBSCRIBE "subscribe"
#define XMPP_TYPE_GET "get"
#define XMPP_TYPE_SET "set"
#define XMPP_TYPE_RESULT "result"
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
    void (*msg_cb)(const char *jid, const char *body);
} my_data;

my_data *new_data();
void print_menu();
void get_jid_and_pass(char *jid, char *pass);
#endif
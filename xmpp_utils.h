#include <strophe.h>

#ifndef XMPP_UTILS
#define XMPP_UTILS
#define XMPP_NS_JABBER_SEARCH "jabber:iq:search"

typedef struct
{
    xmpp_ctx_t *ctx;
    const char *jid;
} xmpp_reg_t;

xmpp_reg_t *xmpp_reg_new(void);
void print_menu();
void get_jid_and_pass(char *jid, char *pass);
#endif
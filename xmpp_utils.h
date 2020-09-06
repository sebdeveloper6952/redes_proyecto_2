#include <strophe.h>

#ifndef XMPP_UTILS
#define XMPP_UTILS
typedef struct
{
    xmpp_ctx_t *ctx;
    const char *jid;
} xmpp_reg_t;

void print_menu();
void get_jid_and_pass(char *jid, char *pass);
#endif
#include <strophe.h>
#include "xmpp_utils.h"

xmpp_reg_t *reg_new(void);
void xmpp_register();
void send_register_form(xmpp_reg_t *reg, xmpp_conn_t *conn, xmpp_stanza_t *stanza);
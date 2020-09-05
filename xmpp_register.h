#include <strophe.h>

typedef struct
{
    xmpp_ctx_t *ctx;
    const char *jid;
} xmpp_reg_t;

xmpp_reg_t *reg_new(void);
void xmpp_register(const char *jid, const char *pass, const char *name);
void send_register_form(xmpp_reg_t *reg, xmpp_conn_t *conn, xmpp_stanza_t *stanza);

// Callbacks
void xmpp_conn_cb(xmpp_conn_t *const conn,
                  const xmpp_conn_event_t status,
                  const int error,
                  xmpp_stream_error_t *const stream_error,
                  void *const userdata);

int xmpp_features_cb(xmpp_conn_t *const conn,
                     xmpp_stanza_t *const stanza,
                     void *const userdata);

int xmpp_register_cb(xmpp_conn_t *const conn,
                     xmpp_stanza_t *const stanza,
                     void *const userdata);

int xmpp_register_form_cb(xmpp_conn_t *const conn,
                          xmpp_stanza_t *const stanza,
                          void *const userdata);
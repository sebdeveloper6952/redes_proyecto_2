#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strophe.h>
#include "xmpp_register.h"
#include "xmpp_utils.h"

void send_register_form(
    xmpp_reg_t *reg,
    xmpp_conn_t *conn,
    xmpp_stanza_t *stanza)
{
    xmpp_ctx_t *ctx = reg->ctx;
    xmpp_stanza_t *query;
    xmpp_stanza_t *elem;
    xmpp_stanza_t *text;
    xmpp_stanza_t *iq;
    const char *name;
    size_t len;
    char buf[256];
    char *value_read;
    char *fields[4] = {"username", "password", "email", "name"};

    query = xmpp_stanza_get_child_by_name(stanza, "query");
    if (!query)
    {
        xmpp_disconnect(conn);
        return;
    }

    query = xmpp_stanza_new(ctx);
    xmpp_stanza_set_name(query, "query");
    xmpp_stanza_set_ns(query, XMPP_NS_REGISTER);

    for (int i = 0; i < 4; i++)
    {
        printf("Enter your desired %s: ", fields[i]);
        value_read = fgets(buf, sizeof(buf), stdin);
        len = strlen(value_read);
        if (len > 0 && value_read[len - 1] == '\n')
        {
            value_read[len - 1] = '\0';
            --len;
        }
        if (len > 0)
        {
            elem = xmpp_stanza_new(ctx);
            text = xmpp_stanza_new(ctx);
            xmpp_stanza_set_text(text, value_read);
            xmpp_stanza_set_name(elem, fields[i]);
            xmpp_stanza_add_child(elem, text);
            xmpp_stanza_add_child(query, elem);
            xmpp_stanza_release(text);
            xmpp_stanza_release(elem);
        }
    }

    if (xmpp_stanza_get_children(query) == NULL)
    {
        fprintf(stderr, "DEBUG: register form empty, not sending.\n");
        xmpp_disconnect(conn);
    }
    else
    {
        iq = xmpp_iq_new(ctx, "set", "register_form_response");
        xmpp_stanza_add_child(iq, query);
        xmpp_send(conn, iq);
        xmpp_stanza_release(query);
        xmpp_stanza_release(iq);
    }
}

// Callback for form submit.
int xmpp_register_form_cb(xmpp_conn_t *const conn,
                          xmpp_stanza_t *const stanza,
                          void *const userdata)
{
    const char *type;

    type = xmpp_stanza_get_type(stanza);
    if (!type || strcmp(type, "error") == 0)
    {
        fprintf(stderr, "DEBUG: error during registration.\n");
        xmpp_disconnect(conn);
        return 0;
    }

    if (strcmp(type, "result") != 0)
    {
        fprintf(stderr, "DEBUG: expected type 'result' but got: %s\n", type);
        xmpp_disconnect(conn);
        return 0;
    }

    fprintf(stderr, "DEBUG: registration was succesful!\n");
    xmpp_disconnect(conn);

    return 0;
}

int xmpp_register_cb(xmpp_conn_t *const conn,
                     xmpp_stanza_t *const stanza,
                     void *const userdata)
{
    xmpp_reg_t *reg_info = (xmpp_reg_t *)userdata;
    xmpp_stanza_t *registered = NULL;
    xmpp_stanza_t *query;
    const char *type;

    type = xmpp_stanza_get_type(stanza);
    // check for error
    if (!type || strcmp(type, "error") == 0)
    {
        fprintf(stderr, "DEBUG: error during registration.\n");
        xmpp_disconnect(conn);
        return 0;
    }

    // if we get type different to result, show error
    if (strcmp(type, "result") != 0)
    {
        fprintf(stderr, "DEBUG: expected result but got (%s).\n", type);
        xmpp_disconnect(conn);
        return 0;
    }

    // check if account is already registered
    query = xmpp_stanza_get_child_by_name(stanza, "query");
    if (query)
        registered = xmpp_stanza_get_child_by_name(query, "registered");
    if (registered != NULL)
    {
        fprintf(stderr, "DEBUG: account is already registered!\n");
        xmpp_disconnect(conn);
        return 0;
    }

    // add handler for form response, and send form
    xmpp_id_handler_add(
        conn,
        xmpp_register_form_cb,
        "register_form_response",
        reg_info);
    send_register_form(reg_info, conn, stanza);

    return 0;
}

int xmpp_features_cb(xmpp_conn_t *const conn,
                     xmpp_stanza_t *const stanza,
                     void *const userdata)
{
    xmpp_reg_t *reg_info = (xmpp_reg_t *)userdata;
    xmpp_ctx_t *ctx = reg_info->ctx;
    xmpp_stanza_t *child;
    xmpp_stanza_t *iq;
    char *domain;

    // check if server supports registration
    child = xmpp_stanza_get_child_by_name(stanza, "register");
    if (child && strcmp(xmpp_stanza_get_ns(child), XMPP_NS_REGISTER) == 0)
    {
        fprintf(stderr, "DEBUG: server does not support registration.\n");
        xmpp_disconnect(conn);
        return 0;
    }

    // server supports registration
    fprintf(stderr, "DEBUG server supports registration.\n");

    // send the request for the register form, and register callback
    domain = xmpp_jid_domain(ctx, reg_info->jid);
    iq = xmpp_iq_new(ctx, "get", "register_form_request");
    xmpp_stanza_set_to(iq, domain);
    child = xmpp_stanza_new(ctx);
    xmpp_stanza_set_name(child, "query");
    xmpp_stanza_set_ns(child, XMPP_NS_REGISTER);
    xmpp_stanza_add_child(iq, child);
    xmpp_handler_add(conn, xmpp_register_cb, XMPP_NS_REGISTER, "iq", NULL, reg_info);
    xmpp_send(conn, iq);
    xmpp_free(ctx, domain);
    xmpp_stanza_release(child);
    xmpp_stanza_release(iq);

    return 0;
}

void xmpp_conn_cb(xmpp_conn_t *const conn,
                  const xmpp_conn_event_t status,
                  const int error,
                  xmpp_stream_error_t *const stream_error,
                  void *const userdata)
{
    xmpp_reg_t *reg_info = (xmpp_reg_t *)userdata;

    if (status == XMPP_CONN_RAW_CONNECT)
    {
        fprintf(stderr, "DEBUG: raw connection established.\n");
        xmpp_conn_open_stream_default(conn);
    }
    else if (status == XMPP_CONN_CONNECT)
    {
        fprintf(stderr, "DEBUG: stream opened.\n");

        // TODO: set error callback

        // Features callback
        xmpp_handler_add(conn,
                         xmpp_features_cb,
                         XMPP_NS_STREAMS,
                         "features",
                         NULL, reg_info);
    }
    else
    {
        fprintf(stderr, "DEBUG: disconnected from server.\n");
        xmpp_stop(reg_info->ctx);
    }

    return;
}

void xmpp_register()
{
    xmpp_ctx_t *ctx;
    xmpp_conn_t *conn;
    xmpp_log_t *log;
    xmpp_reg_t *register_info;

    // initialize library
    xmpp_initialize();
    log = xmpp_get_default_logger(XMPP_LEVEL_DEBUG);
    // ctx = xmpp_ctx_new(NULL, log);
    ctx = xmpp_ctx_new(NULL, NULL);
    conn = xmpp_conn_new(ctx);

    // // register details
    xmpp_conn_set_jid(conn, "redes2020.xyz");
    register_info = reg_new();
    register_info->ctx = ctx;
    register_info->jid = "redes2020.xyz";

    // // connect to server
    xmpp_connect_raw(conn, NULL, 0, xmpp_conn_cb, register_info);
    xmpp_run(ctx);

    // free resources
    xmpp_conn_release(conn);
    xmpp_ctx_free(ctx);
    xmpp_shutdown();
}
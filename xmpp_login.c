#include <strophe.h>
#include <stdio.h>
#include <string.h>
#include "xmpp_utils.h"

// Get the user roster.
int roster_result_handler(
    xmpp_conn_t *const conn,
    xmpp_stanza_t *const stanza,
    void *const userdata)
{
    xmpp_stanza_t *query, *next;

    query = xmpp_stanza_get_child_by_name(stanza, "query");
    if (query)
        next = xmpp_stanza_get_children(query);
    while (next)
    {
        const char *roster_jid = xmpp_stanza_get_attribute(next, "jid");
        if (roster_jid)
            fprintf(stderr, "DEBUG: ROSTER has contact '%s'.\n", roster_jid);
        next = xmpp_stanza_get_next(next);
    }

    return 1;
}

// Send IQ stanza to get the users roster.
void get_roster(xmpp_conn_t *const conn, xmpp_ctx_t *const ctx)
{
    xmpp_stanza_t *iq, *query;
    iq = xmpp_iq_new(ctx, "get", "get_roster");
    query = xmpp_stanza_new(ctx);
    xmpp_stanza_set_name(query, "query");
    xmpp_stanza_set_ns(query, XMPP_NS_ROSTER);
    xmpp_stanza_add_child(iq, query);
    xmpp_stanza_release(query);
    xmpp_send(conn, iq);
    xmpp_stanza_release(iq);
}

int xmpp_subscription_handler(
    xmpp_conn_t *const conn,
    xmpp_stanza_t *const stanza,
    void *const userdata)
{
    xmpp_ctx_t *ctx = xmpp_conn_get_context(conn);
    const char *jid = xmpp_conn_get_bound_jid(conn);
    const char *name = xmpp_stanza_get_name(stanza);
    const char *type = xmpp_stanza_get_type(stanza);

    // someone wants to be my friend :)
    if (type && strcmp(type, "subscribe") == 0)
    {
        const char *from = xmpp_stanza_get_from(stanza);
        fprintf(stderr, "DEBUG: '%s' wants to be my friend!\n", from);
        xmpp_stanza_t *subscribed = xmpp_presence_new(ctx);
        xmpp_stanza_set_from(subscribed, jid);
        xmpp_stanza_set_to(subscribed, from);
        xmpp_stanza_set_type(subscribed, "subscribed");
        xmpp_send(conn, subscribed);
        xmpp_stanza_release(subscribed);
    }

    return 1;
}

// Use the search service to get all users.
void get_all_users(xmpp_conn_t *const conn, xmpp_ctx_t *const ctx)
{
    const char *jid = xmpp_conn_get_bound_jid(conn);
    xmpp_stanza_t *iq_search = xmpp_iq_new(ctx, "set", "search_result");
    xmpp_stanza_t *query_search = xmpp_stanza_new(ctx);
    xmpp_stanza_t *query_email = xmpp_stanza_new(ctx);
    xmpp_stanza_t *email_text = xmpp_stanza_new(ctx);
    xmpp_stanza_set_to(iq_search, "search.redes2020.xyz");
    xmpp_stanza_set_from(iq_search, jid);
    xmpp_stanza_set_name(query_search, "query");
    xmpp_stanza_set_ns(query_search, XMPP_NS_JABBER_SEARCH);
    xmpp_stanza_set_name(query_email, "email");
    xmpp_stanza_set_text(email_text, "*");
    xmpp_stanza_add_child(query_email, email_text);
    xmpp_stanza_add_child(query_search, query_email);
    xmpp_stanza_add_child(iq_search, query_search);
    xmpp_send(conn, iq_search);
    xmpp_stanza_release(email_text);
    xmpp_stanza_release(query_email);
    xmpp_stanza_release(query_search);
    xmpp_stanza_release(iq_search);
}

int search_result_handler(
    xmpp_conn_t *const conn,
    xmpp_stanza_t *const stanza,
    void *const userdata)
{
    xmpp_stanza_t *query = NULL;
    xmpp_stanza_t *next = NULL;
    xmpp_stanza_t *item = NULL;

    query = xmpp_stanza_get_child_by_name(stanza, "query");
    if (query)
        next = xmpp_stanza_get_children(query);
    while (next)
    {
        const char *jid = xmpp_stanza_get_attribute(next, "jid");
        if (jid)
            fprintf(stderr, "DEBUG: found user '%s'\n", jid);
        next = xmpp_stanza_get_next(next);
    }

    return 1;
}

int service_discovery_handler(
    xmpp_conn_t *const conn,
    xmpp_stanza_t *const stanza,
    void *const userdata)
{
    xmpp_ctx_t *ctx = (xmpp_ctx_t *)userdata;
    xmpp_stanza_t *query = NULL;
    xmpp_stanza_t *next = NULL;
    const char *jid = NULL;

    fprintf(stderr, "DEBUG: service discovery response.\n");
    jid = xmpp_conn_get_bound_jid(conn);
    query = xmpp_stanza_get_child_by_name(stanza, "query");
    if (query)
        next = xmpp_stanza_get_children(query);
    while (next)
    {
        const char *service_jid = xmpp_stanza_get_attribute(next, "jid");
        const char *att = xmpp_stanza_get_attribute(next, "name");
        fprintf(stderr, "DEBUG: querying service '%s' with jid '%s'.\n", att, jid);
        xmpp_stanza_t *iq = xmpp_iq_new(ctx, "get", NULL);
        xmpp_stanza_t *query0 = xmpp_stanza_new(ctx);
        xmpp_stanza_set_from(iq, jid);
        xmpp_stanza_set_to(iq, service_jid);
        xmpp_stanza_set_name(query0, "query");
        xmpp_stanza_set_ns(query0, XMPP_NS_DISCO_INFO);
        xmpp_stanza_add_child(iq, query0);
        xmpp_send(conn, iq);
        xmpp_stanza_release(query0);
        xmpp_stanza_release(iq);
        next = xmpp_stanza_get_next(next);
    }

    return 0;
}

// Sends a <presence/> stanza to indicate we are available.
void send_logged_in_presence(xmpp_conn_t *const conn, xmpp_ctx_t *const ctx)
{
    xmpp_stanza_t *login = xmpp_presence_new(ctx);
    xmpp_send(conn, login);
    xmpp_stanza_release(login);
}

void xmpp_login_conn_cb(xmpp_conn_t *const conn,
                        const xmpp_conn_event_t status,
                        const int error,
                        xmpp_stream_error_t *const stream_error,
                        void *const userdata)
{
    xmpp_ctx_t *ctx = (xmpp_ctx_t *)userdata;
    const char *jid = xmpp_conn_get_bound_jid(conn);

    if (status == XMPP_CONN_CONNECT)
    {
        fprintf(stderr, "DEBUG: login succesful.\n");
        int secured = xmpp_conn_is_secured(conn);
        fprintf(stderr, "DEBUG: connection is %s.\n",
                secured ? "secured" : "NOT secured");

        // add handler for presence stanza
        xmpp_handler_add(conn, xmpp_subscription_handler, NULL, "presence", NULL, userdata);

        // add service discovery handler
        xmpp_id_handler_add(
            conn,
            service_discovery_handler,
            "service_discovery",
            userdata);

        // add handler for user search result
        xmpp_id_handler_add(conn, search_result_handler, "search_result", NULL);

        // add handler for user roster result
        xmpp_id_handler_add(conn, roster_result_handler, "get_roster", userdata);

        // send the presence stanza to show available status
        send_logged_in_presence(conn, ctx);

        // testing
        // get_all_users(conn, ctx);
        get_roster(conn, ctx);
    }
    else if (status == XMPP_CONN_DISCONNECT)
    {
        fprintf(stderr, "DEBUG: connection disconnect event.\n");
    }
    else
    {
        fprintf(stderr, "DEBUG: not able to login.\n");
        xmpp_stop(ctx);
    }
}

void xmpp_login(const char *jid, const char *pass)
{
    xmpp_ctx_t *ctx;
    xmpp_conn_t *conn;
    xmpp_log_t *log;
    long flags = XMPP_CONN_FLAG_TRUST_TLS;

    // initialize library
    xmpp_initialize();
    log = xmpp_get_default_logger(XMPP_LEVEL_DEBUG);
    ctx = xmpp_ctx_new(NULL, log);
    conn = xmpp_conn_new(ctx);

    // set flag to trus TLS cert
    xmpp_conn_set_flags(conn, flags);

    // set jid and password for authentication
    // xmpp_conn_set_jid(conn, jid);
    // xmpp_conn_set_pass(conn, pass);
    xmpp_conn_set_jid(conn, "sebdev@redes2020.xyz");
    xmpp_conn_set_pass(conn, "sebasxmpp0985");

    // connect to server
    xmpp_connect_client(conn, NULL, 0, xmpp_login_conn_cb, ctx);

    // run the event loop
    xmpp_run(ctx);

    // free resources
    xmpp_conn_release(conn);
    xmpp_ctx_free(ctx);
    xmpp_shutdown();
}
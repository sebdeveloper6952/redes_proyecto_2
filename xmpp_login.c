#include <strophe.h>
#include <stdio.h>
#include <string.h>
#include "xmpp_utils.h"
#include "xmpp_roster.h"
#include "xmpp_users.h"
#include "xmpp_presence.h"
#include "xmpp_account.h"
#include "xmpp_vcard.h"
#include "xmpp_im.h"
#include "xmpp_gm.h"

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

        // TODO: add all handlers here
        // add handler for friend requests
        xmpp_handler_add(conn, presence_subscription_handler, NULL, XMPP_ST_PRESENCE, XMPP_TYPE_SUBSCRIBE, NULL);
        // presence handler
        xmpp_handler_add(conn, presence_handler, NULL, XMPP_ST_PRESENCE, NULL, NULL);
        // add handler for user search result
        xmpp_id_handler_add(conn, search_result_handler, XMPP_ID_SEARCH_USERS, NULL);
        // add handler for user roster result
        xmpp_id_handler_add(conn, roster_result_handler, XMPP_ID_GET_ROSTER, NULL);
        // vcard handler
        xmpp_id_handler_add(conn, vcard_handler, "vcard_get", NULL);
        // private message handler
        xmpp_handler_add(conn, im_handler, NULL, XMPP_ST_MESSAGE, XMPP_TYPE_CHAT, NULL);
        // group chat message handler
        xmpp_handler_add(conn, gm_msg_handler, NULL, XMPP_ST_MESSAGE, XMPP_TYPE_GROUPCHAT, NULL);

        // send the presence stanza to show available status
        send_logged_in_presence(conn, ctx);

        // testing
        // get_all_users(conn, ctx);
        // get_roster(conn, ctx);

        // test v-card get
        // get_vcard(conn, ctx, "sebdev_gajim@redes2020.xyz");

        // test add friend
        // send_subscription_request(conn, ctx, "sebdev_gajim@redes2020.xyz");

        // test change presence
        // change_presence(conn, ctx, chat, "ola");

        // test send private message
        // send_im_msg(conn, "jua17315@redes2020.xyz", "ola?");

        // join group
        // join_gm_room(conn, "f1", "pierre_gasly");
        // send_gm_msg(conn, "f1@conference.redes2020.xyz", "mugello!!!!1");
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
    // ctx = xmpp_ctx_new(NULL, NULL);
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
#include <strophe.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "xmpp_utils.h"
#include "xmpp_service_discovery.h"
#include "xmpp_roster.h"
#include "xmpp_users.h"
#include "xmpp_presence.h"
#include "xmpp_account.h"
#include "xmpp_vcard.h"
#include "xmpp_im.h"
#include "xmpp_gm.h"
#include "xmpp_file_transfer.h"

// global connection
static xmpp_conn_t *conn;

void xmpp_login_conn_cb(xmpp_conn_t *const conn,
                        const xmpp_conn_event_t status,
                        const int error,
                        xmpp_stream_error_t *const stream_error,
                        void *const userdata)
{
    // xmpp_ctx_t *ctx = (xmpp_ctx_t *)userdata;
    my_data *data = (my_data *)userdata;
    const char *jid = xmpp_conn_get_bound_jid(conn);

    if (status == XMPP_CONN_CONNECT)
    {
        // fprintf(stderr, "DEBUG: login succesful.\n");
        // int secured = xmpp_conn_is_secured(conn);
        // fprintf(stderr, "DEBUG: connection is %s.\n",
        //         secured ? "secured" : "NOT secured");

        // add handler for friend requests
        // xmpp_handler_add(conn, presence_subscription_handler, NULL, XMPP_ST_PRESENCE, XMPP_TYPE_SUBSCRIBE, NULL);

        // private message handler
        // xmpp_handler_add(conn, im_handler, NULL, XMPP_ST_MESSAGE, XMPP_TYPE_CHAT, NULL);

        // group chat message handler
        // xmpp_handler_add(conn, gm_msg_handler, NULL, XMPP_ST_MESSAGE, XMPP_TYPE_GROUPCHAT, NULL);

        // file transfer init handler
        // xmpp_handler_add(conn, file_transfer_init_handler, NULL, "iq", "set", NULL);

        // send the presence stanza to show available status
        send_logged_in_presence(conn, data->ctx);

        // login callback passed externally
        if (data->cb != NULL)
            data->cb(NULL);

        // testing
        // get_all_users(conn);
        // get_roster(conn, ctx);

        // test change presence
        // change_presence(conn, ctx, away, "en clases :(");

        // test send private message
        // send_im_msg(conn, "javi@redes2020.xyz", "ola amigo, buenas noches!");

        // join or create group
        // join_gm_room(conn, "f1", "checo_perez");
        // send_gm_msg(conn, "f1@conference.redes2020.xyz", "printf");

        // server items
        // get_items_from_server(conn, "redes2020.xyz");

        // proxy service info
        // iq_get_from_proxy(conn, "proxy.redes2020.xyz");
        // get_info_from_service(conn, "proxy");
        // get_items_from_service(conn, "proxy");
    }
    else if (status == XMPP_CONN_DISCONNECT)
    {
        // fprintf(stderr, "DEBUG: connection disconnect event.\n");
    }
    else
    {
        // fprintf(stderr, "DEBUG: not able to login.\n");
        xmpp_stop(data->ctx);
    }
}

void xmpp_login(const char *jid, const char *pass, void(*on_login))
{
    xmpp_ctx_t *ctx;
    xmpp_log_t *log;
    long flags = XMPP_CONN_FLAG_TRUST_TLS;
    char full_jid[256];

    // initialize library
    xmpp_initialize();

    // log = xmpp_get_default_logger(XMPP_LEVEL_DEBUG);
    // ctx = xmpp_ctx_new(NULL, log);

    ctx = xmpp_ctx_new(NULL, NULL);
    conn = xmpp_conn_new(ctx);

    // set flag to trust TLS cert
    xmpp_conn_set_keepalive(conn, 60, 30);
    xmpp_conn_set_flags(conn, flags);

    // set jid and password for authentication
    // xmpp_conn_set_jid(conn, jid);
    // xmpp_conn_set_pass(conn, pass);

    strcat(full_jid, jid);
    strcat(full_jid, "@redes2020.xyz");
    xmpp_conn_set_jid(conn, full_jid);
    xmpp_conn_set_pass(conn, pass);

    // connect to server
    my_data *data = new_data();
    data->ctx = ctx;
    data->cb = on_login;
    data->msg_cb = NULL;

    xmpp_connect_client(conn, NULL, 0, xmpp_login_conn_cb, data);

    // run the event loop
    xmpp_run(ctx);

    // free resources
    xmpp_conn_release(conn);
    xmpp_ctx_free(ctx);
    xmpp_shutdown();
}

void xmpp_client_get_roster(void(*on_result))
{
    // attach handler for result
    my_data *data = new_data();
    data->cb = on_result;
    // add handler for user roster result
    xmpp_id_handler_add(conn, roster_result_handler, XMPP_ID_GET_ROSTER, data);
    // roster request
    get_roster(conn);
}

void xmpp_client_get_users(void(*on_result))
{
    // attach handler for result
    my_data *data = new_data();
    data->cb = on_result;
    // add handler for user search result
    xmpp_id_handler_add(conn, search_result_handler, XMPP_ID_SEARCH_USERS, data);
    // send user request
    get_all_users(conn);
}

void xmpp_client_set_presence(status_t av, const char *status, void(*on_result))
{
    // attach handler for result
    my_data *data = new_data();
    data->cb = on_result;
    // my presence change handler
    xmpp_id_handler_add(conn, my_presence_handler, "my_presence_change", data);
    // send presence to server
    change_presence(conn, av, status);
}

void xmpp_client_add_priv_msg_handler(void(*on_result))
{
    // attach handler for result
    my_data *data = new_data();
    data->msg_cb = on_result;
    // private message handler
    xmpp_handler_add(conn, im_handler, NULL, XMPP_ST_MESSAGE, XMPP_TYPE_CHAT, data);
}

void xmpp_client_add_gm_msg_handler(void(*on_result))
{
    // attach handler for result
    my_data *data = new_data();
    data->msg_cb = on_result;
    // group chat message handler
    xmpp_handler_add(conn, gm_msg_handler, NULL, XMPP_ST_MESSAGE, XMPP_TYPE_GROUPCHAT, data);
}

void xmpp_client_add_vcard_handler(void(*on_result))
{
    // attach handler for result
    my_data *data = new_data();
    data->cb = on_result;
    // vcard handler
    xmpp_id_handler_add(conn, vcard_handler, "vcard_get", data);
}

void xmpp_client_join_group_chat(const char *group_jid, const char *nick)
{
    join_gm_room(conn, group_jid, nick);
}

void xmpp_client_send_msg(const char *is_priv, const char *jid, const char *msg)
{
    if (is_priv)
        send_im_msg(conn, jid, msg);
    else
        send_gm_msg(conn, jid, msg);
}

void xmpp_client_add_presence_handler(void(*on_result))
{
    // attach handler for result
    my_data *data = new_data();
    data->msg_cb = on_result;
    // presence handler
    xmpp_handler_add(conn, presence_handler, NULL, XMPP_ST_PRESENCE, NULL, data);
}

void xmpp_client_add_to_roster(const char *jid)
{
    // test add friend
    send_subscription_request(conn, jid);
}

void xmpp_client_get_vcard(const char *jid)
{
    get_vcard(conn, jid);
}

void xmpp_client_delete_account(void(*on_result))
{
    delete_account(conn, on_result);
}
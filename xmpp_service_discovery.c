#include <strophe.h>
#include <stdio.h>
#include "xmpp_service_discovery.h"

void get_items_from_server(xmpp_conn_t *const conn, const char *server)
{
    xmpp_send_raw_string(
        conn,
        "<iq from='%s' to='%s' type='get' id='server_items'>"
        "<query xmlns='http://jabber.org/protocol/disco#items'/>"
        "</iq>",
        xmpp_conn_get_bound_jid(conn),
        server);
}

void get_info_from_service(xmpp_conn_t *const conn, const char *service)
{
    xmpp_send_raw_string(
        conn,
        "<iq from='%s' to='%s.redes2020.xyz' type='get' id='%s_service_info'>"
        "<query xmlns='http://jabber.org/protocol/disco#info'/>"
        "</iq>",
        xmpp_conn_get_bound_jid(conn),
        service,
        service);
}

void get_items_from_service(xmpp_conn_t *const conn, const char *service)
{
    xmpp_send_raw_string(
        conn,
        "<iq from='%s' to='%s.redes2020.xyz' type='get' id='%s_service_items'>"
        "<query xmlns='http://jabber.org/protocol/disco#items'/>"
        "</iq>",
        xmpp_conn_get_bound_jid(conn),
        service,
        service);
}
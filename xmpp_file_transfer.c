#include <strophe.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include "xmpp_file_transfer.h"
#include "xmpp_utils.h"

char stream_id[64] = {};
const char *filename = NULL;
long filesize = 0;

void iq_get_from_proxy(xmpp_conn_t *const conn, const char *service)
{
    xmpp_send_raw_string(
        conn,
        "<iq from='%s' to='%s' type='get' id='%s_service_network'>"
        "<query xmlns='http://jabber.org/protocol/bytestreams'/>"
        "</iq>",
        xmpp_conn_get_bound_jid(conn),
        service,
        service);
}

int file_transfer_init_handler(xmpp_conn_t *const conn, xmpp_stanza_t *const st, void *const userdata)
{
    xmpp_stanza_t *si = NULL, *file = NULL;
    my_data *data;
    const char *iq_id = NULL, *jid_to = NULL;
    const char *file_size = NULL, *tstream_id = NULL;
    long size;

    data = (my_data *)userdata;
    iq_id = xmpp_stanza_get_attribute(st, "id");
    jid_to = xmpp_stanza_get_attribute(st, "from");
    si = xmpp_stanza_get_child_by_name(st, "si");
    if (!si)
    {
        return 1;
    }

    tstream_id = xmpp_stanza_get_attribute(si, "id");
    if (!tstream_id)
    {
        // fprintf(stderr, "DEBUG: file transfer, stream id not found.\n");
        return 1;
    }
    strcpy(stream_id, tstream_id);

    file = xmpp_stanza_get_child_by_name(si, "file");
    if (!file)
    {
        // fprintf(stderr, "DEBUG: file transfer, <file> not found.\n");
        return 1;
    }

    filename = xmpp_stanza_get_attribute(file, "name");
    file_size = xmpp_stanza_get_attribute(file, "size");
    if (filename && file_size)
    {
        filesize = atol(file_size);
        if (data && data->msg_cb)
        {
            char buf[256] = {};
            strcat(buf, filename);
            strcat(buf, " - ");
            strcat(buf, file_size);
            data->msg_cb(jid_to, buf);
        }

        file_transfer_init_response(conn, iq_id, stream_id, jid_to);
    }

    return 1;
}

void file_transfer_init_response(
    xmpp_conn_t *const conn,
    const char *iq_id,
    const char *stream_id,
    const char *jid_to)
{
    xmpp_send_raw_string(
        conn,
        "<iq from='%s' to='%s' type='result' id='%s'>"
        "<si xmlns='http://jabber.org/protocol/si' id='%s'>"
        "<feature xmlns='http://jabber.org/protocol/feature-neg'>"
        "<x xmlns='jabber:x:data' type='submit'>"
        "<field var='stream-method'>"
        "<value>http://jabber.org/protocol/bytestreams</value>"
        "</field>"
        "</x>"
        "</feature>"
        "</si>"
        "</iq>",
        xmpp_conn_get_bound_jid(conn), jid_to, iq_id, stream_id);
}

int file_transfer_streamhost_handler(xmpp_conn_t *const conn, xmpp_stanza_t *const st, void *const userdata)
{
    my_data *data = (my_data *)userdata;
    xmpp_stanza_t *query = NULL, *streamhost = NULL;
    char buf[256] = {};
    const char *jid, *host, *port;

    query = xmpp_stanza_get_child_by_name(st, "query");
    if (query)
        streamhost = xmpp_stanza_get_children(query);

    if (streamhost)
    {
        jid = xmpp_stanza_get_attribute(streamhost, "jid");
        host = xmpp_stanza_get_attribute(streamhost, "host");
        port = xmpp_stanza_get_attribute(streamhost, "port");
        if (data)
        {
            strcat(buf, jid);
            strcat(buf, "|");
            strcat(buf, host);
            strcat(buf, ":");
            strcat(buf, port);
            data->msg_cb(xmpp_stanza_get_from(st), buf);
            start_file_transfer(conn, jid, host, atoi(port), userdata);
        }
    }

    return 1;
}

void start_file_transfer(
    xmpp_conn_t *const conn,
    const char *jid,
    const char *host,
    long port,
    void *const userdata)
{
    my_data *data;
    struct sockaddr_in server;
    int sockfd;
    char send_buf[256] = {};
    char recv_buf[256] = {};
    int numbytes = 0;
    char msg_buf[256] = {};
    char temp[4];
    char sha1_buf[256] = {};
    char *sha1;
    char *file_buf;
    int received = 0;

    if (userdata == NULL)
        return;

    data = (my_data *)userdata;
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1)
    {
        data->msg_cb(jid, "Error opening socket to streamhost.");
        return;
    }

    server.sin_addr.s_addr = inet_addr(host);
    server.sin_family = AF_INET;
    server.sin_port = htons(port);

    if (connect(sockfd, (struct sockaddr *)&server, sizeof(server)) < 0)
    {
        data->msg_cb(jid, "Error connecting to streamhost.");
        return;
    }
    data->msg_cb(jid, "Successfully connected to streamhost.");

    // SOCKS5 Authentication
    send_buf[0] = 5;
    send_buf[1] = 1;
    send_buf[2] = 0x00;
    if (send(sockfd, send_buf, sizeof(send_buf), 0) < 0)
    {
        data->msg_cb(jid, "Error sending command.");
    }
    data->msg_cb(jid, "SOCKS5: auth sent.");

    // auth response
    if ((numbytes = recv(sockfd, recv_buf, sizeof(recv_buf), 0)) == -1)
    {
        data->msg_cb(jid, "Error recv.");
    }
    sprintf(temp, "%x", recv_buf[1]);
    strcat(msg_buf, "SOCKS5: recv ");
    strcat(msg_buf, temp);
    data->msg_cb(jid, msg_buf);

    // SOCKS5 connect command
    memset(send_buf, 0, sizeof(send_buf));
    memset(sha1_buf, 0, sizeof(sha1_buf));
    strcat(sha1_buf, stream_id);
    strcat(sha1_buf, jid);
    strcat(sha1_buf, xmpp_conn_get_bound_jid(conn));
    sha1 = xmpp_sha1(xmpp_conn_get_context(conn), sha1_buf, sizeof(sha1_buf));
    data->msg_cb("stream id: ", stream_id);
    data->msg_cb("sha1: ", sha1);

    send_buf[0] = 5;
    send_buf[1] = 1;
    send_buf[2] = 0;
    send_buf[3] = 3;
    for (int i = 0; i < strlen(sha1); i++)
        send_buf[4 + i] = sha1[i];
    send_buf[strlen(sha1) + 5] = 0x00;
    if (send(sockfd, send_buf, sizeof(send_buf), 0) < 0)
        data->msg_cb(jid, "Error sending command.");

    data->msg_cb(jid, "SOCKS5: connect command sent.");
    xmpp_free(xmpp_conn_get_context(conn), sha1);

    // connect command response
    memset(recv_buf, 0, sizeof(recv_buf));
    memset(msg_buf, 0, sizeof(msg_buf));
    memset(temp, 0, sizeof(temp));
    if ((numbytes = recv(sockfd, recv_buf, sizeof(recv_buf), 0)) == -1)
        data->msg_cb(jid, "Error recv.");

    sprintf(temp, "%x", recv_buf[1]);
    strcat(msg_buf, "SOCKS5: recv ");
    strcat(msg_buf, temp);
    data->msg_cb(jid, msg_buf);

    // ack of stream used
    xmpp_send_raw_string(
        conn,
        "<iq from='%s' to='%s' type='result'>"
        "<query xmlns='http://jabber.org/protocol/bytestreams' sid='%s'>"
        "<streamhost-used jid='%s'/>"
        "</query>"
        "</iq>",
        xmpp_conn_get_bound_jid(conn),
        host,
        stream_id,
        host);

    // connect command response
    file_buf = malloc(filesize);
    char mbuf[256] = {"Allocated: "};
    char tmbuf[2];
    sprintf(tmbuf, "%ld", filesize);
    strcat(mbuf, tmbuf);
    strcat(mbuf, " bytes.");
    data->msg_cb(jid, mbuf);
    while (received < filesize)
    {
        numbytes = recv(sockfd, file_buf, 256, 0);
        if (numbytes == -1)
        {
            data->msg_cb(jid, "Error reading file data.");
            return;
        }
        received += numbytes;
    }
    data->msg_cb(jid, "Read file data.");
    free(file_buf);
}

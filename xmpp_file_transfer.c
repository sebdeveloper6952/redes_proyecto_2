#include <strophe.h>
#include <stdio.h>
#include <stdlib.h>
#include "xmpp_file_transfer.h"

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

int file_transfer_init_handler(xmpp_conn_t *const conn, xmpp_stanza_t *const st, void *const data)
{
    xmpp_stanza_t *si = NULL, *file = NULL;
    const char *iq_id = NULL, *stream_id = NULL, *jid_to = NULL;
    const char *filename = NULL, *file_size = NULL;
    long size;

    iq_id = xmpp_stanza_get_attribute(st, "id");
    jid_to = xmpp_stanza_get_attribute(st, "from");
    si = xmpp_stanza_get_child_by_name(st, "si");
    if (!si)
    {
        fprintf(stderr, "DEBUG: file transfer, <si> not found.\n");
        return 1;
    }

    stream_id = xmpp_stanza_get_attribute(si, "id");
    if (!stream_id)
    {
        fprintf(stderr, "DEBUG: file transfer, stream id not found.\n");
        return 1;
    }

    file = xmpp_stanza_get_child_by_name(si, "file");
    if (!file)
    {
        fprintf(stderr, "DEBUG: file transfer, <file> not found.\n");
        return 1;
    }

    filename = xmpp_stanza_get_attribute(file, "name");
    file_size = xmpp_stanza_get_attribute(file, "size");
    if (filename && file_size)
    {
        size = strtol(file_size, NULL, 10);
        fprintf(
            stderr,
            "DEBUG: file to transfer is named %s with size %ld bytes.\n",
            filename,
            size);

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

void start_file_transfer(void)
{
}

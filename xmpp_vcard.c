#include <strophe.h>
#include <stdio.h>
#include "xmpp_vcard.h"
#include <string.h>

int vcard_handler(xmpp_conn_t *const conn, xmpp_stanza_t *const stanza, void *const data)
{
    xmpp_stanza_t *vcard, *temp;
    char contact[1024];
    char *text;

    vcard = xmpp_stanza_get_child_by_name(stanza, "vCard");
    if (!vcard)
    {
        fprintf(stderr, "DEBUG: no vCard was found!\n");
        return 1;
    }

    strcat(contact, "\n\n******** vCard ********\n");
    temp = xmpp_stanza_get_child_by_name(vcard, "FN");
    if (temp)
    {
        strcat(contact, "* Full Name: ");
        strcat(contact, xmpp_stanza_get_text(temp));
        strcat(contact, "\n");
    }
    temp = xmpp_stanza_get_child_by_name(vcard, "NICKNAME");
    if (temp)
    {
        strcat(contact, "* Nickname: ");
        strcat(contact, xmpp_stanza_get_text(temp));
        strcat(contact, "\n");
    }
    temp = xmpp_stanza_get_child_by_name(vcard, "EMAIL");
    if (temp)
    {
        temp = xmpp_stanza_get_child_by_name(temp, "USERID");
        strcat(contact, "* Email: ");
        strcat(contact, xmpp_stanza_get_text(temp));
        strcat(contact, "\n");
    }
    temp = xmpp_stanza_get_child_by_name(vcard, "TEL");
    if (temp)
    {
        temp = xmpp_stanza_get_child_by_name(temp, "NUMBER");
        strcat(contact, "* Phone #: ");
        strcat(contact, xmpp_stanza_get_text(temp));
        strcat(contact, "\n");
    }
    strcat(contact, "***********************\n");
    fprintf(stderr, "%s", contact);

    return 1;
}

void get_vcard(xmpp_conn_t *const conn, xmpp_ctx_t *const ctx, const char *jid)
{
    xmpp_send_raw_string(
        conn,
        "<iq from='%s' to='%s' type='get' id='%s'>"
        "<vCard xmlns='vcard-temp'/></iq>",
        xmpp_conn_get_bound_jid(conn), jid, "vcard_get");
}
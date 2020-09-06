#include <strophe.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "xmpp_utils.h"
#include "xmpp_register.h"
#include "xmpp_login.h"

void conn_handler(xmpp_conn_t *const conn,
                  const xmpp_conn_event_t status,
                  const int error,
                  xmpp_stream_error_t *const stream_error,
                  void *const userdata);

int main(int argc, char **argv)
{
    char jid[64], pass[32];
    char option;
    long flags = 0;

    print_menu();
    printf("Enter an option: ");
    scanf("%s", &option);
    getchar();

    if (option == '1')
    {
        // printf("enter full jid: ");
        // fgets(jid, 64, stdin);
        // jid[strlen(jid) - 1] = '\0';
        // printf("enter password: ");
        // fgets(pass, 32, stdin);
        // pass[strlen(pass) - 1] = '\0';
        // xmpp_login(jid, pass);
        xmpp_login("a", "b");
    }
    else if (option == '2')
    {
        xmpp_register();
    }
    else if (option == '3')
    {
        printf("remove account from server\n");
    }
    else
    {
        printf("wrong option\n");
    }

    return 0;
}
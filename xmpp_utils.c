#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "xmpp_utils.h"

xmpp_reg_t *reg_new(void)
{
    xmpp_reg_t *reg;
    reg = malloc(sizeof(*reg));
    if (reg != NULL)
    {
        memset(reg, 0, sizeof(*reg));
    }

    return reg;
}

void print_menu()
{
    printf("****************************\n");
    printf("1 - Login\n");
    printf("2 - Create Account\n");
    printf("3 - Delete Account\n");
    printf("****************************\n");
}

void get_jid_and_pass(char *jid, char *pass)
{
    printf("get jid\n");
}
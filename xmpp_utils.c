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

my_data *new_data()
{
    my_data *data;
    data = malloc(sizeof(*data));
    if (data != NULL)
        memset(data, 0, sizeof(*data));

    return data;
}
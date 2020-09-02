#include <strophe.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv)
{
    char *jid, *pass;

    // validate user input
    if (argc < 3)
    {
        printf("usage: ./client <user@host> <password>\n");
        exit(1);
    }

    // save user input
    jid = argv[1];
    pass = argv[2];

    printf("Connecting *%s* with password *%s*\n", jid, pass);

    return 0;
}
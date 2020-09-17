#include <strophe.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "xmpp_utils.h"

int xmpp_bob_img_recv_handler(
    xmpp_conn_t *const conn,
    xmpp_stanza_t *const stanza,
    void *const userdata)
{
    my_data *data;
    xmpp_ctx_t *ctx;
    FILE *file_d;
    char *encoded;
    unsigned char *decoded;
    const char *path;
    char cb_msg[256] = {};
    size_t dec_len;

    ctx = xmpp_conn_get_context(conn);

    data = (my_data *)userdata;
    if (data)
    {
        encoded = xmpp_message_get_body(stanza);
        if (encoded == NULL)
            return 1;

        xmpp_base64_decode_bin(ctx, encoded, strlen(encoded), &decoded, &dec_len);
        path = xmpp_stanza_get_id(stanza);
        if ((file_d = fopen(path, "w")) != NULL)
        {
            if (fwrite(decoded, 1, dec_len, file_d) < dec_len)
            {
                data->cb("error writing to file.");
                fclose(file_d);
                xmpp_free(ctx, encoded);
                return 1;
            }
        }

        // callback
        strcat(cb_msg, "File was saved at the current directory as: ");
        strcat(cb_msg, path);
        data->cb(cb_msg);

        // release resources
        fclose(file_d);
        xmpp_free(ctx, encoded);
        xmpp_free(ctx, decoded);
    }

    return 1;
}

void xmpp_bob_send_image(
    xmpp_conn_t *const conn,
    const char *jid,
    const char *path,
    void *const userdata)
{
    my_data *data;
    xmpp_ctx_t *ctx;
    xmpp_stanza_t *st_msg;
    FILE *file_d;
    unsigned char *buf;
    char *encoded;
    long fsize = 0;

    data = (my_data *)userdata;
    ctx = xmpp_conn_get_context(conn);

    file_d = fopen(path, "r");
    if (file_d == NULL)
        return;

    // get file size
    fseek(file_d, 0, SEEK_END);
    fsize = ftell(file_d);
    fseek(file_d, 0, SEEK_SET);

    // allocate exact memory
    buf = malloc(fsize + 1);
    if (buf == NULL)
    {
        fclose(file_d);
        data->cb("malloc fail");
        return;
    }

    // read
    if (fread(buf, 1, fsize, file_d) < (fsize - 1))
    {
        fclose(file_d);
        free(buf);
        data->cb("fread fail");
        return;
    }
    buf[fsize] = '\0';

    // encode img
    encoded = xmpp_base64_encode(ctx, buf, fsize);

    // create and send stanza
    st_msg = xmpp_message_new(ctx, "normal", jid, path);
    xmpp_message_set_body(st_msg, encoded);
    xmpp_send(conn, st_msg);
    xmpp_stanza_release(st_msg);

    // callback
    data->cb("File sent!");

    // release mem
    xmpp_free(ctx, encoded);
    free(buf);
    // close file
    fclose(file_d);
}
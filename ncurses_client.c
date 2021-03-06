#include <ncurses.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <strophe.h>
#include "xmpp_login.h"
#include "xmpp_register.h"
#include "xmpp_roster.h"

WINDOW *create_newwin(int height, int width, int starty, int startx);
void destroy_win(WINDOW *local_win);
void clear_win(WINDOW *w);
void update_win(WINDOW *w, const char *title, const char *content);
void clear_prompt();
void show_saved_presences(void);
void print_title(WINDOW *window, int row, int col);

// worker thread
struct thread_data
{
    char host[64];
    char username[64];
    char password[64];
};
void *thread_work(void *data);

// callbacks
void on_login();
void on_users_result(const char *roster);
void on_roster_result(const char *roster);
void on_my_presence_result(const char *new_presence);
void on_presence(const char *jid, const char *st);
void on_msg(const char *jid_from, const char *body);
void on_vcard_result(const char *result);
void on_register_result(const char *result);
void on_delete_account(const char *result);
void on_subscription_request(const char *result);
void on_file_recv_init(const char *from, const char *msg);
void on_file_streamhost_recv(const char *from, const char *msg);
void on_image_sent_result(const char *result);
void on_img_recv(const char *result);
void on_file_offer_result(const char *result);
void on_file_offer_recv(const char *result);
void on_streamhost_offer_sent(const char *result);
void on_streamhost_offer_received(const char *result);
void on_ibb_offer_recv(const char *result);

WINDOW *w_title, *w_help, *w_prompt, *w_active, *w_content;
pthread_t worker_thread;
char curr_chat_jid[256] = {};
char in_p_chat = 0, in_g_chat = 0, account_was_deleted = 0;
unsigned int curr_msg_count = 0;

// presence handling
unsigned char p_count = 0;
char *pres_arr[100] = {};

// message handling
unsigned char msg_count = 0;

// file transfer
unsigned char fmsg_count = 2;

int main(int argc, char *argv[])
{
    int ch;

    // init
    initscr();
    cbreak();
    start_color();
    init_pair(1, COLOR_GREEN, COLOR_BLACK);
    attron(COLOR_PAIR(1));
    keypad(stdscr, TRUE);

    // menu
    print_title(stdscr, LINES / 2 - 10, COLS / 2 - 30);
    mvprintw(LINES / 2 + 2, COLS / 2 - 10, "1 - LOGIN");
    mvprintw(LINES / 2 + 3, COLS / 2 - 10, "2 - CREATE ACCOUNT");
    mvprintw(LINES - 1, 0, "ENTER OPTION: ");
    ch = getch();
    clear();
    refresh();

    while (ch != '1' && ch != '2' && ch != '3')
    {
        print_title(stdscr, LINES / 2 - 10, COLS / 2 - 30);
        mvprintw(LINES / 2 + 2, COLS / 2 - 10, "1 - LOGIN");
        mvprintw(LINES / 2 + 3, COLS / 2 - 10, "2 - CREATE ACCOUNT");
        mvprintw(LINES - 1, 0, "ENTER OPTION: ");
        ch = getch();
        clear();
        refresh();
    }

    if (ch == '1')
    {
        char host[64];
        char username[64];
        char password[64];
        char ch;

        print_title(stdscr, LINES / 2 - 10, COLS / 2 - 30);
        mvprintw(LINES / 2, COLS / 2 - 5, "HOST: ");
        mvprintw(LINES / 2 + 1, COLS / 2 - 5, "USERNAME: ");
        mvprintw(LINES / 2 + 2, COLS / 2 - 5, "PASSWORD: ");
        move(LINES / 2, COLS / 2 + 1);
        getstr(host);
        move(LINES / 2 + 1, COLS / 2 + 5);
        getstr(username);
        move(LINES / 2 + 2, COLS / 2 + 5);
        noecho();
        getstr(password);
        clear();

        // reactivate character echoing
        echo();

        w_title = create_newwin(3, COLS - 10, 0, 5);
        wbkgd(w_title, COLOR_PAIR(1));
        mvwprintw(w_title, 1, COLS / 2 - 2, "LOGGING IN, PLEASE WAIT...");
        wrefresh(w_title);

        w_help = create_newwin(LINES - 3, 30, 3, 5);
        wbkgd(w_help, COLOR_PAIR(1));
        mvwprintw(w_help, 1, 11, "COMMANDS");
        mvwprintw(w_help, 3, 2, "/help <command>");
        mvwprintw(w_help, 4, 2, "/users");
        mvwprintw(w_help, 5, 2, "/roster [add <jid>]");
        mvwprintw(w_help, 6, 2, "/active");
        mvwprintw(w_help, 7, 2, "/presence <show> [status]");
        mvwprintw(w_help, 8, 2, "/priv <jid>");
        mvwprintw(w_help, 9, 2, "/group <room_jid> <nick>");
        mvwprintw(w_help, 10, 2, "/vcard <jid>");
        mvwprintw(w_help, 11, 2, "/file <path> <jid>");
        mvwprintw(w_help, 12, 2, "/menu");
        mvwprintw(w_help, 13, 2, "/delete");
        mvwprintw(w_help, 14, 2, "/quit");
        wrefresh(w_help);

        w_content = create_newwin(LINES - 5, COLS - 40, 3, 35);
        wbkgd(w_content, COLOR_PAIR(1));
        mvwprintw(w_content, 10, 1, "\tWelcome!");
        mvwprintw(w_content, 11, 1, "\tBrowse the commands and enter /help <command>"
                                    " to find more info about the commands.");
        print_title(w_content, 2, 2);

        w_prompt = create_newwin(3, COLS - 40, LINES - 3, 35);
        wbkgd(w_prompt, COLOR_PAIR(1));
        mvwprintw(w_prompt, 1, 1, "COMMAND: ");
        wrefresh(w_prompt);

        // start the xmpp client
        struct thread_data td;
        strcpy(td.host, host);
        strcpy(td.username, username);
        strcpy(td.password, password);
        pthread_create(&worker_thread, NULL, thread_work, &td);

        // ask for command, clear and redraw the prompt
        char cmd[256];
        char *tokens[10] = {};
        char *o_cmd, *token;
        int i = 0;

        wgetstr(w_prompt, cmd);
        clear_prompt();

        // command read and evaluate loop
        while (1)
        {
            // if not in chat
            if (in_p_chat == 0 && in_g_chat == 0)
            {
                // if account was deleted
                if (account_was_deleted)
                    break;

                // split command
                o_cmd = strdup(cmd);
                while ((token = strsep(&o_cmd, " ")) != NULL)
                    tokens[i++] = token;

                if (tokens[0] == "\n")
                    continue;

                if (strcmp(tokens[0], "/quit") == 0)
                    break;

                // go back to main menu
                if (strcmp(tokens[0], "/menu") == 0)
                {
                    in_p_chat = 0;
                    in_g_chat = 0;
                    wclear(w_content);
                    print_title(w_content, 2, 2);
                    mvwprintw(w_content, 10, 1, "\tWelcome!");
                    mvwprintw(w_content, 11, 1, "\tBrowse the commands and enter /help <command>"
                                                " to find more info about the command.");
                    wborder(w_content, '|', '|', '-', '-', '*', '*', '*', '*');
                    wrefresh(w_content);
                }
                // command usage help
                else if (strcmp(tokens[0], "/help") == 0)
                {
                    if (tokens[1] == NULL)
                        continue;

                    if (strcmp(tokens[1], "users") == 0)
                    {
                        update_win(
                            w_content,
                            "USERS COMMAND HELP",
                            "- usage: /users\n"
                            " - description: shows all users that have an account on the server.");
                    }
                    else if (strcmp(tokens[1], "roster") == 0)
                    {
                        update_win(
                            w_content,
                            "ROSTER COMMAND HELP",
                            "- usage: /roster [add <jid>]\n"
                            " - description:\n\t/roster: shows your roster."
                            "\n\t/roster add <jid>: adds the user with bare jid to your roster.");
                    }
                    else if (strcmp(tokens[1], "active") == 0)
                    {
                        update_win(
                            w_content,
                            "ACTIVE COMMAND HELP",
                            "- usage: /active\n"
                            " - description: shows all the <presence> stanzas received.");
                    }
                    else if (strcmp(tokens[1], "presence") == 0)
                    {
                        update_win(
                            w_content,
                            "PRESENCE COMMAND HELP",
                            "- usage: /presence <show> [status]\n"
                            " - description: change your current presence show and status.\n\t"
                            "<show> must be one of the following:\n\t- available\n\t- away"
                            "\n\t- xa\n\t- dnd\n\t[status] is optional and it is a string.");
                    }
                    else if (strcmp(tokens[1], "priv") == 0)
                    {
                        update_win(
                            w_content,
                            "PRIV COMMAND HELP",
                            "- usage: /priv <jid>\n"
                            " - description: enter into a private chat with user identified by bare jid <jid>.");
                    }
                    else if (strcmp(tokens[1], "group") == 0)
                    {
                        update_win(
                            w_content,
                            "GROUP COMMAND HELP",
                            "- usage: /group <room_jid> <nick>\n"
                            " - description: enter group chat with group identified by <room_jid> (bare jid)."
                            "\n\t Your nickname is <nick> and it is not optional.");
                    }
                    else if (strcmp(tokens[1], "vcard") == 0)
                    {
                        update_win(
                            w_content,
                            "VCARD COMMAND HELP",
                            "- usage: /vcard <jid>\n"
                            " - description: get the vCard of user identified by bare jid <jid>.");
                    }
                    else if (strcmp(tokens[1], "file") == 0)
                    {
                        update_win(
                            w_content,
                            "FILE COMMAND HELP",
                            "- usage: /file <filename> <jid>\n"
                            " - description: sends the file identified by <filename> to the user identified by <jid>. \n\tThe Jabber ID MUST be FULLY specified. \n\tIt is recommended to first run the /active command to get a user's full Jabber ID. \n\tNOTE: for simplicity, only files that are in the current directory can be sent.");
                    }
                    else if (strcmp(tokens[1], "menu") == 0)
                    {
                        update_win(
                            w_content,
                            "MENU COMMAND HELP",
                            "- usage: /menu\n"
                            " - description: when on a private or group chat, it exits the chat.\n\t"
                            " Otherwise, it clears the screen.");
                    }
                    else if (strcmp(tokens[1], "delete") == 0)
                    {
                        update_win(
                            w_content,
                            "DELETE COMMAND HELP",
                            "- usage: /delete\n"
                            " - description: deletes your account from the server.");
                    }
                    else if (strcmp(tokens[1], "quit") == 0)
                    {
                        update_win(
                            w_content,
                            "QUIT COMMAND HELP",
                            "- usage: /quit\n"
                            " - description: exit the chat :(");
                    }
                }
                else if (strcmp(tokens[0], "/users") == 0)
                {
                    xmpp_client_get_users(on_users_result);
                }
                else if (strcmp(tokens[0], "/roster") == 0)
                {
                    if (tokens[1] == NULL)
                    {
                        xmpp_client_get_roster(on_roster_result);
                    }
                    else if (strcmp(tokens[1], "add") == 0 && tokens[2] != NULL)
                    {
                        xmpp_client_add_to_roster(tokens[2]);
                        update_win(w_content, "\tFRIEND REQUEST SENT!", "");
                    }
                }
                else if (strcmp(tokens[0], "/active") == 0)
                {
                    show_saved_presences();
                }
                else if (strcmp(tokens[0], "/presence") == 0)
                {
                    if (tokens[1] != NULL)
                    {
                        status_t st;
                        char status[256] = {" "};

                        if (strcmp(tokens[1], "away") == 0)
                            st = away;
                        else if (strcmp(tokens[1], "xa") == 0)
                            st = xa;
                        else if (strcmp(tokens[1], "dnd") == 0)
                            st = dnd;
                        else if (strcmp(tokens[1], "available") == 0)
                            st = available;

                        for (int i = 2; i < 10; i++)
                        {
                            if (tokens[i] != NULL)
                            {
                                strcat(status, tokens[i]);
                                strcat(status, " ");
                            }
                        }

                        xmpp_client_set_presence(
                            st,
                            status,
                            on_my_presence_result);
                    }
                }
                else if (strcmp(tokens[0], "/priv") == 0)
                {
                    if (tokens[1] != NULL)
                    {
                        in_p_chat = 1;
                        msg_count = 0;
                        strcpy(curr_chat_jid, tokens[1]);
                        char title[256] = {};
                        strcat(title, "PRIVATE CHAT WITH [");
                        strcat(title, tokens[1]);
                        strcat(title, "]");
                        update_win(w_content, title, "");
                    }
                }
                else if (strcmp(tokens[0], "/group") == 0)
                {
                    if (tokens[1] != NULL && tokens[2] != NULL)
                    {
                        in_g_chat = 1;
                        msg_count = 0;
                        strcpy(curr_chat_jid, tokens[1]);
                        char title[256] = {};
                        strcat(title, "GROUP CHAT [");
                        strcat(title, tokens[1]);
                        strcat(title, "]");
                        update_win(w_content, title, "");
                        xmpp_client_join_group_chat(tokens[1], tokens[2]);
                    }
                }
                else if (strcmp(tokens[0], "/vcard") == 0 && tokens[1] != NULL)
                {
                    update_win(w_content, "GETTING VCARD", "Please wait...");
                    xmpp_client_get_vcard(tokens[1]);
                }
                else if (strcmp(tokens[0], "/delete") == 0)
                {
                    update_win(w_content, "DELETING ACCOUNT", "Please wait...");
                    xmpp_client_delete_account(on_delete_account);
                }
                else if (strcmp(tokens[0], "/file") == 0)
                {
                    if (tokens[1] != NULL && tokens[2] != NULL)
                    {
                        fmsg_count = 2;
                        wclear(w_content);
                        mvwprintw(w_content, 1, 2, "[FILE TRANSFER]");
                        wrefresh(w_content);
                        xmpp_client_offer_file(tokens[1], tokens[2], on_file_offer_result);
                    }
                }
            }
            // if in private or group chat
            else
            {
                if (strcmp(cmd, "/menu") == 0)
                {
                    in_p_chat = 0;
                    in_g_chat = 0;
                    msg_count = 0;
                    memset(curr_chat_jid, 0, sizeof(curr_chat_jid));
                    update_win(w_content, "MENU", "");
                }

                if (in_p_chat)
                {
                    // private chat message
                    xmpp_client_send_msg(1, curr_chat_jid, cmd);
                    mvwprintw(w_content, 3 + msg_count++, 2, "[YOU] %s", cmd);
                    wborder(w_content, '|', '|', '-', '-', '*', '*', '*', '*');
                    wrefresh(w_content);
                }
                else if (in_g_chat)
                {
                    // group chat message
                    xmpp_client_send_msg(0, curr_chat_jid, cmd);
                }
            }

            // command buffers reset
            for (int i = 0; i < 10; i++)
                tokens[i] = NULL;
            i = 0;

            if (!account_was_deleted)
            {
                wgetstr(w_prompt, cmd);
                wclear(w_prompt);
                wborder(w_prompt, '|', '|', '-', '-', '*', '*', '*', '*');
                mvwprintw(w_prompt, 1, 1, "COMMAND: ");
                wrefresh(w_prompt);
            }
        }
    }
    else if (ch == '2')
    {
        char username[64];
        char email[64];
        char fullname[64];
        char password[64];
        char ch;

        mvprintw(LINES / 2, COLS / 2 - 5, "USERNAME: ");
        mvprintw(LINES / 2 + 1, COLS / 2 - 5, "EMAIL: ");
        mvprintw(LINES / 2 + 2, COLS / 2 - 5, "FULLNAME: ");
        mvprintw(LINES / 2 + 3, COLS / 2 - 5, "PASSWORD: ");
        move(LINES / 2, COLS / 2 + 5);
        getstr(username);
        move(LINES / 2 + 1, COLS / 2 + 2);
        getstr(email);
        move(LINES / 2 + 2, COLS / 2 + 5);
        getstr(fullname);
        move(LINES / 2 + 3, COLS / 2 + 5);
        noecho();
        getstr(password);
        mvprintw(LINES / 2 + 5, COLS / 2 - 28, "PRESS ENTER TO CREATE ACCOUNT OR ANY OTHER KEY TO CANCEL");

        keypad(stdscr, TRUE);
        ch = getch();
        clear();
        if (ch == KEY_ENTER || ch == 10)
        {
            mvprintw(LINES / 2, COLS / 2 - 10, "CREATING ACCOUNT...");
            refresh();
            xmpp_client_register_account(username, email, fullname, password, on_register_result);
        }
        else
        {
            mvprintw(LINES / 2, COLS / 2 - 10, "ACCOUNT CREATION CANCELED.");
        }

        getch();
    }

    // release resources
    pthread_cancel(worker_thread);
    endwin();

    return 0;
}

WINDOW *create_newwin(int height, int width, int starty, int startx)
{
    WINDOW *local_win;

    local_win = newwin(height, width, starty, startx);
    wborder(local_win, '|', '|', '-', '-', '*', '*', '*', '*');
    wrefresh(local_win);

    return local_win;
}

void *thread_work(void *data)
{
    struct thread_data *td = (struct thread_data *)data;
    xmpp_client_login(td->host, td->username, td->password, on_login);
}

void update_win(WINDOW *w, const char *title, const char *content)
{
    wclear(w);
    mvwprintw(w, 1, 1, title);
    mvwprintw(w, 3, 1, "%s", content);
    wborder(w, '|', '|', '-', '-', '*', '*', '*', '*');
    wrefresh(w);
}

void clear_win(WINDOW *w)
{
    wclear(w);
    wborder(w, '|', '|', '-', '-', '*', '*', '*', '*');
    wrefresh(w);
}

void clear_prompt()
{
    wclear(w_prompt);
    wborder(w_prompt, '|', '|', '-', '-', '*', '*', '*', '*');
    mvwprintw(w_prompt, 1, 1, "COMMAND: ");
    wrefresh(w_prompt);
}

void on_login()
{
    wclear(w_title);
    wborder(w_title, '|', '|', '-', '-', '*', '*', '*', '*');
    mvwprintw(w_title, 1, COLS / 2 - 2, "LOGGED IN! []");
    wrefresh(w_title);
    // xmpp client handlers
    xmpp_client_add_presence_handler(on_presence);
    xmpp_client_add_subscription_handler(on_subscription_request);
    xmpp_client_add_priv_msg_handler(on_msg);
    xmpp_client_add_gm_msg_handler(on_msg);
    xmpp_client_add_vcard_handler(on_vcard_result);
    xmpp_client_add_file_offer_handler(on_file_offer_result);
    xmpp_client_add_file_offer_recv_handler(on_file_offer_recv);
    xmpp_client_add_streamhost_offer_handler(on_streamhost_offer_received);
    xmpp_client_add_ibb_offer_recv_handler(on_ibb_offer_recv);
}

void on_users_result(const char *roster)
{
    wclear(w_content);
    const char *roster_title = "ALL USERS";
    mvwprintw(w_content, 1, (COLS / 2) - 30, roster_title);
    mvwprintw(w_content, 3, 2, "%s", roster);
    wborder(w_content, '|', '|', '-', '-', '*', '*', '*', '*');
    wrefresh(w_content);
}

void on_roster_result(const char *roster)
{
    wclear(w_content);
    const char *roster_title = "YOUR ROSTER";
    mvwprintw(w_content, 1, (COLS / 2) - 30, roster_title);
    mvwprintw(w_content, 3, 1, "%s", roster);
    wborder(w_content, '|', '|', '-', '-', '*', '*', '*', '*');
    wrefresh(w_content);
}

void on_my_presence_result(const char *new_presence)
{
    wclear(w_title);
    mvwprintw(w_title, 1, COLS / 2 - 2, "LOGGED IN [%s]", new_presence);
    wborder(w_title, '|', '|', '-', '-', '*', '*', '*', '*');
    wrefresh(w_title);
}

void on_msg(const char *jid_from, const char *body)
{
    if (in_p_chat)
    {
        mvwprintw(w_content, 3 + msg_count++, 2, "[%s] %s", jid_from, body);
        wborder(w_content, '|', '|', '-', '-', '*', '*', '*', '*');
        wrefresh(w_content);
    }
    else if (in_g_chat)
    {
        mvwprintw(w_content, 3 + msg_count++, 2, "[%s] %s", jid_from, body);
        wborder(w_content, '|', '|', '-', '-', '*', '*', '*', '*');
        wrefresh(w_content);
    }
    else
    {
        wclear(w_content);
        mvwprintw(w_content, 1, 2, "[NOTIFICATION]: NEW MESSAGE from [%s] %s", jid_from, body);
        wborder(w_content, '|', '|', '-', '-', '*', '*', '*', '*');
        wrefresh(w_content);
    }
}

void on_presence(const char *jid, const char *status)
{
    char *new_p;
    new_p = malloc(256);
    memset(new_p, 0, 256);
    new_p[0] = '[';
    strcat(new_p, jid);
    strcat(new_p, "]");
    strcat(new_p, "(");
    strcat(new_p, status);
    strcat(new_p, ")");
    pres_arr[p_count++] = new_p;

    wclear(w_content);
    mvwprintw(w_content, 1, 2, "[NOTIFICATION]: <presence> from [%s] %s", jid, status);
    wborder(w_content, '|', '|', '-', '-', '*', '*', '*', '*');
    wrefresh(w_content);
}

void show_saved_presences(void)
{
    wclear(w_content);

    for (int i = 0; i < p_count; i++)
        mvwprintw(w_content, 3 + i, 3, pres_arr[i]);

    wborder(w_content, '|', '|', '-', '-', '*', '*', '*', '*');
    wrefresh(w_content);
}

void on_vcard_result(const char *vcard)
{
    update_win(w_content, "VCARD RESULT", vcard);
}

void on_register_result(const char *result)
{
    update_win(w_content, "REGISTER RESULT", result);
    clear();
    mvprintw(LINES / 2, COLS / 2, "REGISTER RESULT");
    mvprintw(LINES / 2, COLS / 2 - 20, result);
    refresh();
}

void on_delete_account(const char *result)
{
    account_was_deleted = 1;
    update_win(w_content, "DELETE ACCOUNT RESULT", result);
}

void on_subscription_request(const char *subscription)
{
    wclear(w_content);
    mvwprintw(w_content, 1, 2, "[NOTIFICATION]: %s", subscription);
    wborder(w_content, '|', '|', '-', '-', '*', '*', '*', '*');
    wrefresh(w_content);
}

void on_file_recv_init(const char *from, const char *msg)
{
    fmsg_count = 1;
    wclear(w_content);
    mvwprintw(
        w_content,
        fmsg_count++,
        2,
        "[FILE TRANSFER]: [%s] wants to send you a file: (%s)",
        from,
        msg);
    wborder(w_content, '|', '|', '-', '-', '*', '*', '*', '*');
    wrefresh(w_content);
}

void on_file_streamhost_recv(const char *from, const char *msg)
{
    fmsg_count += 2;
    mvwprintw(
        w_content,
        fmsg_count,
        2,
        "[FILE TRANSFER]: [%s] offers the following streamhosts:\n\t%s",
        from,
        msg);
    wrefresh(w_content);
}

void on_image_sent_result(const char *result)
{
    update_win(w_content, "IMAGE SENT", result);
}

void on_img_recv(const char *result)
{
    update_win(w_content, "IMAGE RECEIVED", result);
}

void on_file_offer_result(const char *result)
{
    fmsg_count++;
    mvwprintw(w_content, fmsg_count, 2, result);
    wrefresh(w_content);
}

void on_file_offer_recv(const char *result)
{
    fmsg_count++;
    mvwprintw(w_content, fmsg_count, 2, result);
    wrefresh(w_content);
}

void on_streamhost_offer_sent(const char *result)
{
    update_win(w_content, "STREAMHOST OFFER SENT", result);
}

void on_streamhost_offer_received(const char *result)
{
    update_win(w_content, "STREAMHOST OFFER RECEIVED", result);
}

void on_ibb_offer_recv(const char *result)
{
    fmsg_count++;
    mvwprintw(w_content, fmsg_count, 2, result);
    wrefresh(w_content);
}

void print_title(WINDOW *win, int row, int col)
{
    char *title0 = "    __  __           __                ________          __\n";
    char *title1 = "   / / / /___ ______/ /_____  _____   / ____/ /_  ____ _/ /_\n";
    char *title2 = "  / /_/ / __ `/ ___/ //_/ _ \\/ ___/  / /   / __ \\/ __ `/ __/\n";
    char *title3 = " / __  / /_/ / /__/ ,< /  __/ /     / /___/ / / / /_/ / /_  \n";
    char *title4 = "/_/ /_/\\__,_/\\___/_/|_|\\___/_/      \\____/_/ /_/\\__,_/\\__/\n";
    mvwprintw(win, row, col, title0);
    mvwprintw(win, row + 1, col, title1);
    mvwprintw(win, row + 2, col, title2);
    mvwprintw(win, row + 3, col, title3);
    mvwprintw(win, row + 4, col, title4);
    wrefresh(win);
}
#include <ncurses.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <strophe.h>
#include "xmpp_login.h"
#include "xmpp_roster.h"

WINDOW *create_newwin(int height, int width, int starty, int startx);
void destroy_win(WINDOW *local_win);
void clear_win(WINDOW *w);
void update_win(WINDOW *w, const char *title, const char *content);
void clear_prompt();
void on_login();
void on_users_result(const char *roster);
void on_roster_result(const char *roster);
void on_my_presence_result(const char *new_presence);
void on_presence(const char *jid, const char *st);
void on_msg(const char *jid_from, const char *body);
void show_saved_presences(void);
void *thread_work(void *data);

WINDOW *w_title, *w_help, *w_prompt, *w_active, *w_content;
pthread_t worker_thread;
char curr_chat_jid[256] = {};
char in_p_chat = 0, in_g_chat = 0;
unsigned int curr_msg_count = 0;

// presence handling
unsigned char p_count = 0;
char *pres_arr[100] = {};

// group message handling
unsigned char g_msg_count = 0;

int main(int argc, char *argv[])
{
    int startx, starty, width, height;
    int ch;

    // init
    initscr();
    cbreak();
    start_color();
    init_pair(1, COLOR_GREEN, COLOR_BLACK);
    attron(COLOR_PAIR(1));
    keypad(stdscr, TRUE);

    // menu
    mvprintw(LINES / 2, COLS / 2 - 5, "HackerChat");
    mvprintw(LINES / 2 + 2, COLS / 2 - 10, "1 - Login");
    mvprintw(LINES / 2 + 3, COLS / 2 - 10, "2 - Crear cuenta");
    mvprintw(LINES / 2 + 4, COLS / 2 - 10, "3 - Eliminar cuenta");
    mvprintw(LINES - 1, 0, "Ingresa tu opcion: ");
    ch = getch();
    clear();
    refresh();

    while (ch != '1' && ch != '2' && ch != '3')
    {
        mvprintw(LINES / 2, COLS / 2 - 5, "HackerChat");
        mvprintw(LINES / 2 + 2, COLS / 2 - 10, "1 - Login");
        mvprintw(LINES / 2 + 3, COLS / 2 - 10, "2 - Crear cuenta");
        mvprintw(LINES / 2 + 4, COLS / 2 - 10, "3 - Eliminar cuenta");
        mvprintw(LINES - 1, 0, "Ingresa tu opcion: ");
        ch = getch();
        clear();
        refresh();
    }

    if (ch == '1')
    {
        height = 3;
        width = COLS - 10;
        starty = 0;
        startx = 5;
        w_title = create_newwin(height, width, starty, startx);
        wbkgd(w_title, COLOR_PAIR(1));
        mvwprintw(w_title, 1, COLS / 2 - 2, "Logging in, please wait...");
        wrefresh(w_title);

        w_help = create_newwin(LINES - 3, 30, 3, 5);
        wbkgd(w_help, COLOR_PAIR(1));
        mvwprintw(w_help, 1, 11, "COMMANDS");
        mvwprintw(w_help, 3, 2, "/help <command>");
        mvwprintw(w_help, 4, 2, "/users");
        mvwprintw(w_help, 5, 2, "/roster [add <jid>]");
        mvwprintw(w_help, 6, 2, "/active");
        mvwprintw(w_help, 7, 2, "/presence <show> <status>");
        mvwprintw(w_help, 8, 2, "/priv <jid>");
        mvwprintw(w_help, 9, 2, "/group <room_jid> <nickname>");
        mvwprintw(w_help, 10, 2, "/vcard <jid>");
        mvwprintw(w_help, 11, 2, "/file <path> <jid>");
        mvwprintw(w_help, 12, 2, "/menu");
        mvwprintw(w_help, 13, 2, "/quit");
        wrefresh(w_help);

        w_content = create_newwin(LINES - 5, COLS - 40, 3, 35);
        wbkgd(w_content, COLOR_PAIR(1));
        mvwprintw(w_content, 1, 1, "MENU");
        wrefresh(w_content);

        w_prompt = create_newwin(3, COLS - 40, LINES - 3, 35);
        wbkgd(w_prompt, COLOR_PAIR(1));
        mvwprintw(w_prompt, 1, 1, "COMMAND: ");
        wrefresh(w_prompt);

        // start the xmpp client
        pthread_create(&worker_thread, NULL, thread_work, NULL);

        // ask for command, clear and redraw the prompt
        char cmd[256];
        char *tokens[10];
        char *o_cmd, *token;
        int i = 0;

        wgetstr(w_prompt, cmd);
        clear_prompt();

        while (1)
        {
            // if not in chat
            if (in_p_chat == 0 && in_g_chat == 0)
            {
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
                    update_win(w_content, "MENU", "");
                }

                // command usage help
                if (strcmp(tokens[0], "/help") == 0)
                {
                    if (strcmp(tokens[1], "users") == 0)
                    {
                        update_win(
                            w_content,
                            "HELP",
                            "- usage: /users\n"
                            " - description: shows all users that have an account on the server");
                    }
                }

                if (strcmp(tokens[0], "/users") == 0)
                {
                    xmpp_client_get_users(on_users_result);
                }

                if (strcmp(tokens[0], "/roster") == 0)
                {
                    xmpp_client_get_roster(on_roster_result);
                }

                if (strcmp(tokens[0], "/active") == 0)
                {
                    show_saved_presences();
                }

                if (strcmp(tokens[0], "/presence") == 0)
                {
                    if (tokens[1] != NULL)
                    {
                        status_t st;
                        char status[256] = {};

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

                if (strcmp(tokens[0], "/priv") == 0)
                {
                    if (tokens[1] != NULL)
                    {
                        in_p_chat = 1;
                        strcpy(curr_chat_jid, tokens[1]);
                        char title[256] = {};
                        strcat(title, "PRIVATE CHAT WITH [");
                        strcat(title, tokens[1]);
                        strcat(title, "]");
                        update_win(w_content, title, "");
                    }
                }

                if (strcmp(tokens[0], "/group") == 0)
                {
                    if (tokens[1] != NULL && tokens[2] != NULL)
                    {
                        in_g_chat = 1;
                        strcpy(curr_chat_jid, tokens[1]);
                        char title[256] = {};
                        strcat(title, "GROUP CHAT [");
                        strcat(title, tokens[1]);
                        strcat(title, "]");
                        update_win(w_content, title, "");
                        xmpp_client_join_group_chat(tokens[1], tokens[2]);
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
                    update_win(w_content, "MENU", "");
                }

                if (in_p_chat)
                {
                    // private chat message
                    xmpp_client_send_msg(1, curr_chat_jid, cmd);
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

            wgetstr(w_prompt, cmd);
            wclear(w_prompt);
            wborder(w_prompt, '|', '|', '-', '-', '*', '*', '*', '*');
            mvwprintw(w_prompt, 1, 1, "COMMAND: ");
            wrefresh(w_prompt);
        }
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
    xmpp_login("a", "a", on_login);
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
    mvwprintw(w_title, 1, COLS / 2 - 2, "sebdev@redes2020.xyz []");
    wrefresh(w_title);
    // xmpp client handlers
    xmpp_client_add_presence_handler(on_presence);
    xmpp_client_add_priv_msg_handler(on_msg);
    xmpp_client_add_gm_msg_handler(on_msg);
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
    mvwprintw(w_title, 1, COLS / 2 - 2, "sebdev@redes2020.xyz [%s]", new_presence);
    wborder(w_title, '|', '|', '-', '-', '*', '*', '*', '*');
    wrefresh(w_title);
}

void on_msg(const char *jid_from, const char *body)
{
    if (in_p_chat)
    {
        update_win(w_content, "PRIVATE MSG", body);
    }
    else if (in_g_chat)
    {
        mvwprintw(w_content, 3 + g_msg_count++, 2, "[%s] %s", jid_from, body);
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
}

void show_saved_presences(void)
{
    wclear(w_content);

    for (int i = 0; i < p_count; i++)
        mvwprintw(w_content, 3 + i, 3, pres_arr[i]);

    wborder(w_content, '|', '|', '-', '-', '*', '*', '*', '*');
    wrefresh(w_content);
}
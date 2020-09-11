#include <ncurses.h>
#include <pthread.h>
#include <string.h>
#include <strophe.h>
#include "xmpp_login.h"
#include "xmpp_roster.h"

WINDOW *create_newwin(int height, int width, int starty, int startx);
void destroy_win(WINDOW *local_win);

WINDOW *title, *help, *prompt, *content;
pthread_t worker_thread;

void on_login()
{
    wclear(title);
    wborder(title, '|', '|', '-', '-', '*', '*', '*', '*');
    mvwprintw(title, 1, COLS / 2 - 2, "HackerChat");
    wrefresh(title);
}

void on_users_result(const char *roster)
{
    wclear(content);
    const char *roster_title = "Users on the Server";
    mvwprintw(content, 1, (COLS / 2) - 30, roster_title);
    mvwprintw(content, 3, 1, "%s", roster);
    wborder(content, '|', '|', '-', '-', '*', '*', '*', '*');
    wrefresh(content);
}

void on_roster_result(const char *roster)
{
    wclear(content);
    const char *roster_title = "Your Roster";
    mvwprintw(content, 1, (COLS / 2) - 30, roster_title);
    mvwprintw(content, 3, 1, "%s", roster);
    wborder(content, '|', '|', '-', '-', '*', '*', '*', '*');
    wrefresh(content);
}

void *thread_work(void *data);

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
        title = create_newwin(height, width, starty, startx);
        wbkgd(title, COLOR_PAIR(1));
        mvwprintw(title, 1, COLS / 2 - 2, "Loggin in, please wait...");
        wrefresh(title);

        pthread_create(&worker_thread, NULL, thread_work, NULL);

        help = create_newwin(LINES - 3, 30, 3, 5);
        wbkgd(help, COLOR_PAIR(1));
        mvwprintw(help, 1, 11, "COMMANDS");
        mvwprintw(help, 3, 2, "/help <command>");
        mvwprintw(help, 4, 2, "/users");
        mvwprintw(help, 5, 2, "/roster");
        mvwprintw(help, 6, 2, "/priv <jid>");
        mvwprintw(help, 7, 2, "/quit");
        wrefresh(help);

        content = create_newwin(LINES - 6, COLS - 41, 3, 36);
        wbkgd(content, COLOR_PAIR(1));
        mvwprintw(content, 1, 1, "Content ");
        wrefresh(content);

        prompt = create_newwin(3, COLS - 41, LINES - 3, 36);
        wbkgd(prompt, COLOR_PAIR(1));
        mvwprintw(prompt, 1, 1, "COMMAND: ");
        wrefresh(prompt);

        // ask for command, clear and redraw the prompt
        char cmd[256];
        wgetstr(prompt, cmd);
        wclear(prompt);
        wborder(prompt, '|', '|', '-', '-', '*', '*', '*', '*');
        mvwprintw(prompt, 1, 1, "COMMAND: ");
        wrefresh(prompt);

        int i = 1;
        while (1)
        {
            if (strcmp(cmd, "/quit") == 0)
                break;

            if (strcmp(cmd, "/help"))
            {
            }

            if (strcmp(cmd, "/users") == 0)
            {
                xmpp_client_get_users(on_users_result);
            }

            if (strcmp(cmd, "/roster") == 0)
            {
                xmpp_client_get_roster(on_roster_result);
            }

            wgetstr(prompt, cmd);
            wclear(prompt);
            wborder(prompt, '|', '|', '-', '-', '*', '*', '*', '*');
            mvwprintw(prompt, 1, 1, "COMMAND: ");
            wrefresh(prompt);
        }
    }

    // release resources
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
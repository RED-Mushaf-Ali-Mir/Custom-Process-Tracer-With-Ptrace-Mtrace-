
#include <sys/types.h>   
#include <ncurses.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/wait.h>

#include "Filter.h"      
#include "TUI.h"

/* ── colour pairs ─────────────────────────────────────────── */
#define CP_HEADER  1   /* white on blue   - title bar        */
#define CP_NORMAL  2   /* white on black  - regular text     */
#define CP_HILITE  3   /* black on cyan   - highlighted row  */
#define CP_BORDER  4   /* cyan  on black  - window borders   */
#define CP_ERROR   5   /* white on red    - error message    */
#define CP_STATUS  6   /* yellow on black - footer / hints   */
#define CP_CHECK   7   /* green on black  - checked [X]      */

/* ── main menu labels ─────────────────────────────────────── */
static const char *MENU_ITEMS[] = {
    "Trace by PID",
    "Trace by Process Name",
    "Filter Syscall Categories",
    "Exit",
};
static const int N_ITEMS = sizeof(MENU_ITEMS) / sizeof(MENU_ITEMS[0]);

static void tui_init(void)
{
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    curs_set(0);

    if (has_colors()) {
        start_color();
        init_pair(CP_HEADER, COLOR_WHITE,  COLOR_BLUE);
        init_pair(CP_NORMAL, COLOR_WHITE,  COLOR_BLACK);
        init_pair(CP_HILITE, COLOR_BLACK,  COLOR_CYAN);
        init_pair(CP_BORDER, COLOR_CYAN,   COLOR_BLACK);
        init_pair(CP_ERROR,  COLOR_WHITE,  COLOR_RED);
        init_pair(CP_STATUS, COLOR_YELLOW, COLOR_BLACK);
        init_pair(CP_CHECK,  COLOR_GREEN,  COLOR_BLACK);
    }
}

static void draw_chrome(const char *footer_hint)
{
    int rows, cols;
    getmaxyx(stdscr, rows, cols);

    // Header 
    attron(COLOR_PAIR(CP_HEADER) | A_BOLD);
    mvhline(0, 0, ' ', cols);
    const char *title = "  MTracer  v1.0  ";
    mvprintw(0, (cols - (int)strlen(title)) / 2, "%s", title);
    attroff(COLOR_PAIR(CP_HEADER) | A_BOLD);

   //footer
    attron(COLOR_PAIR(CP_STATUS));
    mvhline(rows - 1, 0, ' ', cols);
    mvprintw(rows - 1, 2, "%s", footer_hint);
    attroff(COLOR_PAIR(CP_STATUS));

    refresh();
}

/* main menu window */

static void draw_menu(WINDOW *win, int highlight)
{
    int height, width;
    getmaxyx(win, height, width);
    (void)height;

    werase(win);

    wattron(win, COLOR_PAIR(CP_BORDER));
    box(win, 0, 0);
    const char *wtitle = " Select Action ";
    wattron(win, A_BOLD);
    mvwprintw(win, 0, (width - (int)strlen(wtitle)) / 2, "%s", wtitle);
    wattroff(win, A_BOLD);
    wattroff(win, COLOR_PAIR(CP_BORDER));

    for (int i = 0; i < N_ITEMS; ++i) {
        int row = 2 + i * 2;   
        int col = 4;

        if (i == highlight) {
            wattron(win, COLOR_PAIR(CP_HILITE) | A_BOLD);
            mvwhline(win, row, 1, ' ', width - 2);
            mvwprintw(win, row, col, ">  %s", MENU_ITEMS[i]);  
            wattroff(win, COLOR_PAIR(CP_HILITE) | A_BOLD);
        } else {
            wattron(win, COLOR_PAIR(CP_NORMAL));
            mvwprintw(win, row, col, "   %s", MENU_ITEMS[i]);
            wattroff(win, COLOR_PAIR(CP_NORMAL));
        }
    }
    wrefresh(win);
}


/*
 Returns 1 = accepted, 0 = cancelled (Esc or q).
 digits_only: if 1, rejects non-digit keypresses.
 */
static int popup_input(const char *prompt,
                       char *buf, int bufsize,
                       int digits_only)
{
    int rows, cols;
    getmaxyx(stdscr, rows, cols);

    int pw = 52, ph = 7;
    int py = (rows - ph) / 2;
    int px = (cols - pw) / 2;

    WINDOW *pop = newwin(ph, pw, py, px);
    keypad(pop, TRUE);

    wattron(pop, COLOR_PAIR(CP_BORDER));
    box(pop, 0, 0);
    const char *wtitle = " Input ";
    mvwprintw(pop, 0, (pw - (int)strlen(wtitle)) / 2, "%s", wtitle);
    wattroff(pop, COLOR_PAIR(CP_BORDER));

    wattron(pop, A_BOLD);
    mvwprintw(pop, 2, 3, "%s", prompt);
    wattroff(pop, A_BOLD);

    int field_w = pw - 6;
    wattron(pop, A_UNDERLINE | COLOR_PAIR(CP_NORMAL));
    mvwhline(pop, 4, 3, ' ', field_w);
    wattroff(pop, A_UNDERLINE | COLOR_PAIR(CP_NORMAL));

    mvwprintw(pop, ph - 2, 3, "[Enter] Accept  [Esc/q] Cancel");
    wrefresh(pop);

    memset(buf, 0, bufsize);
    int pos = 0;
    curs_set(1);

    while (1) {
        /* redraw field */
        wattron(pop, A_UNDERLINE | COLOR_PAIR(CP_NORMAL));
        mvwprintw(pop, 4, 3, "%-*s", field_w, buf);
        wattroff(pop, A_UNDERLINE | COLOR_PAIR(CP_NORMAL));
        wmove(pop, 4, 3 + pos);
        wrefresh(pop);

        int ch = wgetch(pop);

        if (ch == 27 || ch == 'q' || ch == 'Q') {
            /* cancel */
            curs_set(0);
            delwin(pop);
            touchwin(stdscr);
            refresh();
            return 0;
        }
        if (ch == '\n' || ch == KEY_ENTER) {
            if (pos == 0) continue;   /* don't accept empty */
            break;
        }
        if ((ch == KEY_BACKSPACE || ch == 127 || ch == '\b') && pos > 0) {
            buf[--pos] = '\0';
            mvwprintw(pop, ph - 2, 3, "[Enter] Accept  [Esc/q] Cancel");
            continue;
        }
        if (pos >= bufsize - 1) continue;

        if (digits_only && !isdigit(ch)) {
            wattron(pop, COLOR_PAIR(CP_ERROR));
            mvwprintw(pop, ph - 2, 3, "  Digits only!  Press q to cancel  ");
            wattroff(pop, COLOR_PAIR(CP_ERROR));
            wrefresh(pop);
            continue;
        }
        if (!isprint(ch)) continue;

        buf[pos++] = (char)ch;
        buf[pos]   = '\0';
        mvwprintw(pop, ph - 2, 3, "[Enter] Accept  [Esc/q] Cancel");
    }

    curs_set(0);
    delwin(pop);
    touchwin(stdscr);
    refresh();
    return 1;
}


static unsigned int run_filter_menu(unsigned int current_filter)
{
    int rows, cols;
    getmaxyx(stdscr, rows, cols);

    int ph = CAT_COUNT + 7;   /* rows for all categories + chrome */
    int pw = 52;
    int py = (rows - ph) / 2;
    int px = (cols - pw) / 2;

    WINDOW *win = newwin(ph, pw, py, px);
    keypad(win, TRUE);

    unsigned int sel = current_filter;  /* working copy */
    int highlight    = 0;

    while (1) {
        werase(win);

  
        wattron(win, COLOR_PAIR(CP_BORDER));
        box(win, 0, 0);
        const char *wtitle = " Filter Syscall Categories ";
        wattron(win, A_BOLD);
        mvwprintw(win, 0, (pw - (int)strlen(wtitle)) / 2, "%s", wtitle);
        wattroff(win, A_BOLD);
        wattroff(win, COLOR_PAIR(CP_BORDER));

        // Subtitle lines */
        wattron(win, COLOR_PAIR(CP_STATUS));
        mvwprintw(win, 1, 3, "Space=toggle  Enter=confirm  q=cancel");
        wattroff(win, COLOR_PAIR(CP_STATUS));

        wattron(win, COLOR_PAIR(CP_NORMAL));
        mvwprintw(win, 2, 3, "(no selection = trace ALL syscalls)");
        wattroff(win, COLOR_PAIR(CP_NORMAL));

        // Separator */
        wattron(win, COLOR_PAIR(CP_BORDER));
        mvwhline(win, 3, 1, ACS_HLINE, pw - 2);
        wattroff(win, COLOR_PAIR(CP_BORDER));

        // Category rows */
        for (int i = 0; i < CAT_COUNT; i++) {
            int row     = 4 + i;
            int checked = (sel & (1u << i)) ? 1 : 0;

            if (i == highlight) {
                wattron(win, COLOR_PAIR(CP_HILITE) | A_BOLD);
                mvwhline(win, row, 1, ' ', pw - 2);
            } else {
                wattron(win, COLOR_PAIR(CP_NORMAL));
            }

            // [X] or [ ] 
            if (checked) {
                if (i != highlight) {
                    wattroff(win, COLOR_PAIR(CP_NORMAL));
                    wattron(win, COLOR_PAIR(CP_CHECK) | A_BOLD);
                }
                mvwprintw(win, row, 3, "[X]");
                if (i != highlight) {
                    wattroff(win, COLOR_PAIR(CP_CHECK) | A_BOLD);
                    wattron(win, COLOR_PAIR(CP_NORMAL));
                }
            } else {
                mvwprintw(win, row, 3, "[ ]");
            }

            mvwprintw(win, row, 7, "%s", categories[i].name);

            if (i == highlight)
                wattroff(win, COLOR_PAIR(CP_HILITE) | A_BOLD);
            else
                wattroff(win, COLOR_PAIR(CP_NORMAL));
        }

        /* Separator + status line */
        wattron(win, COLOR_PAIR(CP_BORDER));
        mvwhline(win, 4 + CAT_COUNT, 1, ACS_HLINE, pw - 2);
        wattroff(win, COLOR_PAIR(CP_BORDER));

        wattron(win, COLOR_PAIR(CP_STATUS));
        if (sel == 0)
            mvwprintw(win, ph - 2, 3, "Active filter: ALL (no filter set)   ");
        else {
            char fbuf[128] = "";
            for (int i = 0; i < CAT_COUNT; i++)
                if (sel & (1u << i)) {
                    strncat(fbuf, categories[i].name,
                            sizeof(fbuf) - strlen(fbuf) - 2);
                    strncat(fbuf, " ", sizeof(fbuf) - strlen(fbuf) - 1);
                }
            mvwprintw(win, ph - 2, 3, "%-*s", pw - 6, fbuf);
        }
        wattroff(win, COLOR_PAIR(CP_STATUS));

        wrefresh(win);

        int ch = wgetch(win);
        switch (ch) {
        case KEY_UP:
            highlight = (highlight == 0) ? CAT_COUNT - 1 : highlight - 1;
            break;
        case KEY_DOWN:
            highlight = (highlight == CAT_COUNT - 1) ? 0 : highlight + 1;
            break;
        case ' ':                   /* toggle */
            sel ^= (1u << highlight);
            break;
        case '\n':
        case KEY_ENTER:             /* confirm */
            delwin(win);
            touchwin(stdscr);
            refresh();
            return sel;
        case 'q':
        case 'Q':
        case 27:                    /* Escape - cancel */
            delwin(win);
            touchwin(stdscr);
            refresh();
            return current_filter;  /* unchanged */
        default:
            break;
        }
    }
}

/* ── status bar (row above footer) ───────────────────────── */

static void show_status(const char *msg, int is_error)
{
    int rows, cols;
    getmaxyx(stdscr, rows, cols);
    int pair = is_error ? CP_ERROR : CP_STATUS;
    attron(COLOR_PAIR(pair) | A_BOLD);
    mvhline(rows - 2, 0, ' ', cols);
    mvprintw(rows - 2, 2, "%s", msg);
    attroff(COLOR_PAIR(pair) | A_BOLD);
    refresh();
}



pid_t pidByname(char *p_name)
{
    int fd[2];
    if (pipe(fd) == -1) return 0;

    pid_t pid = fork();
    if (pid == 0) {
        close(fd[0]);
        dup2(fd[1], STDOUT_FILENO);
        close(fd[1]);
        execlp("pgrep", "pgrep", "-n", p_name, NULL);
        _exit(1);
    }

    close(fd[1]);
    char p_pid[16] = {0};
    int n = read(fd[0], p_pid, sizeof(p_pid) - 1);
    close(fd[0]);
    wait(NULL);

    if (n <= 0) return 0;
    p_pid[n] = '\0';
    return (pid_t) atoi(p_pid);
}


pid_t RenderMenu(unsigned int *out_filter)
{
    tui_init();

    int rows, cols;
    getmaxyx(stdscr, rows, cols);

    int mw = 46;
    int mh = 2 + N_ITEMS * 2 + 2;
    int my = (rows - mh) / 2;
    int mx = (cols - mw) / 2;

    WINDOW *menu_win = newwin(mh, mw, my, mx);
    keypad(menu_win, TRUE);

    int highlight       = 0;
    unsigned int filter = 0;   /* 0 = trace all */

    while (1) {
        draw_chrome("Up/Down  Navigate    Enter  Select    q  Quit");
        draw_menu(menu_win, highlight);

        /* Show active filter beneath the menu box */
        {
            int r2, c2;
            getmaxyx(stdscr, r2, c2);
            (void)r2;
            attron(COLOR_PAIR(CP_STATUS));
            mvhline(my + mh + 1, 0, ' ', c2);
            if (filter == 0) {
                mvprintw(my + mh + 1, mx, "Filter: ALL syscalls");
            } else {
                char fbuf[160] = "Filter: ";
                for (int i = 0; i < CAT_COUNT; i++) {
                    if (filter & (1u << i)) {
                        strncat(fbuf, categories[i].name,
                                sizeof(fbuf) - strlen(fbuf) - 2);
                        strncat(fbuf, " ", sizeof(fbuf) - strlen(fbuf) - 1);
                    }
                }
                mvprintw(my + mh + 1, mx, "%s", fbuf);
            }
            attroff(COLOR_PAIR(CP_STATUS));
            refresh();
        }

        int ch = wgetch(menu_win);

        switch (ch) {
        case KEY_UP:
            highlight = (highlight == 0) ? N_ITEMS - 1 : highlight - 1;
            break;

        case KEY_DOWN:
            highlight = (highlight == N_ITEMS - 1) ? 0 : highlight + 1;
            break;

        case '\n':
        case KEY_ENTER: {
            char buf[64] = {0};

            if (highlight == 0) {
                /* ---- Trace by PID ---- */
                int ok = popup_input("Enter PID to trace:", buf, sizeof(buf), 1);
                if (!ok) break;   /* q or Esc -> back to menu */
                pid_t t = (pid_t) atoi(buf);
                if (t <= 0) { show_status("Invalid PID entered.", 1); break; }
                delwin(menu_win);
                endwin();
                if (out_filter) *out_filter = filter;
                return t;

            } else if (highlight == 1) {
                /* ---- Trace by Name ---- */
                int ok = popup_input("Enter process name:", buf, sizeof(buf), 0);
                if (!ok) break;
                show_status("Looking up process...", 0);
                pid_t t = pidByname(buf);
                if (t == 0) {
                    char errmsg[128];
                    snprintf(errmsg, sizeof(errmsg),
                             "No process found: \"%s\"  (press any key)", buf);
                    show_status(errmsg, 1);
                    wgetch(menu_win);
                    break;
                }
                delwin(menu_win);
                endwin();
                if (out_filter) *out_filter = filter;
                return t;

            } else if (highlight == 2) {
                /* ---- Filter menu ---- */
                filter = run_filter_menu(filter);

            } else {
                /* ---- Exit ---- */
                delwin(menu_win);
                endwin();
                exit(0);
            }
            break;
        }

        case 'q':
        case 'Q':
            delwin(menu_win);
            endwin();
            exit(0);

        default:
            break;
        }
    }
}
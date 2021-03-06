#include<seq_view.h>

namespace SeqView {

    /*
     * Erase the previous character and move the cursor
     * back a space
     *
     */
    void backspace() {
        int y, x;
        getyx(stdscr, y, x);
        wmove(stdscr, y, x - 1);
        addch(' ');
        wmove(stdscr, y, x - 1);
    }

    void clear_line(int line) {
        int x = getmaxx(stdscr);
        for(int i = 0; i <= x; i++)
            mvwaddch(stdscr, line, i, ' ');
    }

    void initDisplay() {
        // Use in place of initscr so that we can read files from stdin.
        // Very much obliged to https://gist.github.com/dreiss/226539
        // (David Reiss).
        char* tt = getenv("TERM");
        const char* term_type;
        if (tt == NULL || *tt == '\0') {
            term_type = "unknown";
        } else {
            term_type = ( const char* ) tt;
        }
        FILE* term_in = fopen("/dev/tty", "r");
        if (term_in == NULL) {
            perror("fopen(/dev/tty)");
            exit(EXIT_FAILURE);
        }
        SCREEN* main_screen = newterm((NCURSES_CONST char*) term_type, stdout, term_in);
        set_term(main_screen);
        // End replacement for initscr();

        clear();
        cbreak();
        noecho();
        keypad(stdscr, true);
        curs_set(0);

        start_color();
        init_pair(1,  COLOR_RED,     COLOR_BLACK);
        init_pair(2,  COLOR_GREEN,   COLOR_BLACK);
        init_pair(3,  COLOR_YELLOW,  COLOR_BLACK);
        init_pair(4,  COLOR_BLUE,    COLOR_BLACK);
        init_pair(5,  COLOR_MAGENTA, COLOR_BLACK);
        init_pair(6,  COLOR_CYAN,    COLOR_BLACK);
        init_pair(7,  COLOR_WHITE,   COLOR_BLACK);
        init_pair(8,  COLOR_BLACK,   COLOR_BLACK);
    }
}

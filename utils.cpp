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

    void initDisplay() {
        initscr();
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

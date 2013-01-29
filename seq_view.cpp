/*
 * seq_view: curses-based program for viewing biological sequence data. 
 *
 * Written in 2013 by Brendan Epstein.
 *
 * To the extent possible under law, the author(s) have dedicated all 
 * copyright and related and neighboring rights to this software to the 
 * public domain worldwide. This software is distributed without any warranty.
 *
 * You should have received a copy of the CC0 Public Domain Dedication along 
 * with this software. If not, 
 * see <http://creativecommons.org/publicdomain/zero/1.0/>. 
 *
 */

/*
 * Use:
 *  seq_view [files...]
 *
 * To compile:
 *  Make sure seq_view.h is in the same directory
 *  g++ -o seq_view *.cpp -I"." -lncurses
 *
 */

#include <seq_view.h>

using namespace SeqView;

int main(int argc, char * argv[]) {

    initDisplay();
    WindowSet wins;
    for(int i = 1; i < argc; i++) {
        string fname(argv[i]);
        wins.add_window(fname);
    }
    bool stay_open = true;
    while(stay_open) {
        wins.update();
        Command command = getCommand();
        stay_open = wins.handle_command(command);
    }
    endwin();
    return 0;
}

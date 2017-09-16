/*
 * Use:
 *  seq_view [files...]
 *
 * To compile:
 *  Make sure seq_view.h is in the same directory
 *  g++ -o seq_view *.cpp -I"." -lncurses --std=c++11
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

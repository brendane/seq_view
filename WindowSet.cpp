#include<seq_view.h>

namespace SeqView {

    void WindowSet::adjust_to_fill_evenly() {
        if(windows.size() == 0)
            return;
        // Space the windows equally in vertical distance
        // and fill full horizontal space
        update_size();
        std::vector<int> newheights;
        int used_height = 0;
        int h = (int) ((height - 1) / (double)windows.size());
        int win = 0;
        int adj = 0;
        while(win < windows.size()) {
            if(height - used_height < h)
                h = height - used_height - 1;
            if(win == windows.size() - 1)
                if(height - used_height > h + 1)
                    h = height - used_height - 1;
            windows[win] -> resize(0, used_height + adj, width, h);
            used_height += h;
            win++;
        }
    }

    WindowSet::WindowSet() {
        getmaxyx(stdscr, height, width);
        focal_window = 0;
        std::vector<SeqWindow*> windows();
    }

    void WindowSet::run_command(Command command) {
        // if command in SeqSet commands, pass it along
        // else run it here
    }

    void WindowSet::update_size() {
        getmaxyx(stdscr, height, width);
    }

    void WindowSet::print_message(string message) {
        wattron(stdscr, A_NORMAL | A_BOLD);
        mvwprintw(stdscr, height - 1, 0, string(width, ' ').c_str());
        mvwprintw(stdscr, height - 1, 0, message.c_str());
        wattroff(stdscr, A_BOLD);
    }

    void WindowSet::update() {
        int xtemp = width;
        int ytemp = height;
        update_size();
        if(width != xtemp || height != ytemp)
            adjust_to_fill_evenly();
        for(int i = 0; i < windows.size(); i++) {
            windows[i] -> display();
        }
        refresh();
    }

    void WindowSet::add_window(string filename) {
        try {
            SeqWindow * s = new SeqWindow(0, 0, width, height,
                    filename);
            windows.push_back(s);
            update_size();
            adjust_to_fill_evenly();
            change_focus(windows.size() - 1);
            update();
            return;
        } catch(...) {
            print_message("Cannot open file " + filename);
        }
    }

    void WindowSet::change_focus(int newfocus) {
        if(newfocus > -1 && newfocus < windows.size()) {
            windows[focal_window] -> set_focus(false);
            focal_window = newfocus;
            windows[focal_window] -> set_focus(true);
            update();
        }
    }

    // Return true if the program should keep going, 
    // otherwise false; If there any windows left, 
    // keep going.
    bool WindowSet::close_focus() {
        if(windows.size() == 0) {
            return false;
        } else {
            SeqWindow * s = windows[focal_window];
            windows.erase(windows.begin() + focal_window);
            delete s;
            if(focal_window >= windows.size())
                focal_window = 0;
            change_focus(focal_window);
            update_size();
            adjust_to_fill_evenly();
            update();
            return (windows.size() > 0);
        }
    }

    bool WindowSet::handle_command(Command command) {
        if(seqSetCommands.count(command.first) > 0 &&
                windows.size() > 0) {
            windows[focal_window] -> handle_command(command);
            return true;
        } else {
            if(command.first == QUIT)
                return close_focus();
            if(command.first == CHANGEFOCUS) {
                if(command.second == 0) {
                    command.second = focal_window + 2; // Add 2 b/c command is 1 based, but focal_window is 0 based
                    if(command.second > windows.size())
                        command.second = 1;
                }
                change_focus(command.second - 1);
                return true;
            }
            if(command.first == CHANGEFOCUSREV) {
                command.second = focal_window; // Subtract nothing b/c command is 1 based, but focal_window is 0 based
                if(command.second == 0)
                    command.second = windows.size();
                change_focus(command.second - 1);
                return true;
            }
            if(command.first == RESIZE) {
                update();
            }
            if(command.first == SPECIAL) {
                handle_special_command();
                return true;
            }
        }
        return true;
    }

    void WindowSet::handle_special_command() {
        /* 
         * TODO:
         *  2) Implement a simple command history
         *  3) Allow commands to stretch over more than one line:
         *     - keep track of line length relative to window width
         *     - keep track of which line
         *     - when command is run, remember to redisplay the bottom
         *       window if the command took up multiple lines.
         *  4) DELETE KEY
         *  6) Tab file completion
         */
        for(int i = 0; i <= width; i++)
            mvwaddch(stdscr, height - 1, i, ' ');
        mvwprintw(stdscr, height - 1, 0, ";");
        string buffer = "";
        int cursor = 0; // where to insert the new character
        int line = height - 1; // line to print commands - to deal with
                               // long commands
        wchar_t ch;
        curs_set(1);
        while(true) {
            ch = getch();

            // Eventually keep a stack of previous commands
            // and cycle through them with UP and DOWN.

            // Backspace
            if(ch == KEY_BACKSPACE || ch == KEY_DL || ch == 7) {
                if(buffer.size()) {
                    backspace();
                    cursor -= 1;
                }
                if(buffer.size())
                    buffer.erase(buffer.size() - 1);
                continue;
            }

            if(ch == KEY_ENTER || ch == '\n' || ch == '\r')
                break;
            if(ch < 27 || ch > 126)
                continue;
            if(ch == 27) {
                for(int i = 0; i <= width; i++)
                    mvwaddch(stdscr, height - 1, i, ' ');
                return; // ESC key
            }
            waddch(stdscr, ch); // Also modify to deal with cursor
            buffer += ch; // This can be modified to deal with arrow keys
            cursor += 1;
        }
        curs_set(0);

        // break buffer into tokens separated by spaces
        // and process
        if(buffer.size() > 0) {
            std::vector<string> tokens;
            string tok = "";
            for(int i = 0; i < buffer.size(); i++) {
                if(buffer[i] == ' ') {
                    tokens.push_back(string(tok));

                    // Get rid of these printing commands
                    printw(tok.c_str());
                    printw(" ");
                    tok = "";
                } else {
                    tok += buffer[i];
                }
            }
            if(tok.size())
                tokens.push_back(string(tok));

            // open file
            // Eventually this should allow file completion
            // on tab.
            if(!tokens[0].compare("open") && tokens.size() == 2) {
                clear_line(height - 1);
                add_window(tokens[1]);
            } else if(!tokens[0].compare("mode") && tokens.size() == 2) {
                if(!tokens[1].compare("ten"))
                    handle_command(Command(DISPLAYMODE, 3));
                if(!tokens[1].compare("codon"))
                    handle_command(Command(DISPLAYMODE, 2));
                if(!tokens[1].compare("normal"))
                    handle_command(Command(DISPLAYMODE, 1));
                clear_line(height - 1);
            } else if(!tokens[0].compare("bold") && tokens.size() == 2) {
                handle_command(Command(TOGGLEBOLD, 0));
            } else if(!tokens[0].compare("compare-mode") &&
                    tokens.size() == 2 && CompModeMap.count(string(tokens[1]))) {
                    handle_command(Command(COMPARE, CompModeMap[string(tokens[1])]));
                    clear_line(height-1);
            } else {
                clear_line(height - 1);
            }
        } else {
            clear_line(height - 1);
        }
    }
}

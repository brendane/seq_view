/*
 * File containing basic data structures for the program.
 *
 * Brendan Epstein 
 * 06 May 2012
 */

#include <string>
#include <vector>
#include <stdint.h>
#include <math.h>
#include <iostream>
#include <fstream>
#include <ncurses.h>
#include <stdlib.h>
#include <exception>
#include <set>

using std::cout;
using std::getline;
using std::endl;
using std::string;
using std::ifstream;
using std::exception;
using std::set;

namespace SeqView {
    
    void backspace() {
        int y, x;
        getyx(stdscr, y, x);
        wmove(stdscr, y, x - 1);
        addch(' ');
        wmove(stdscr, y, x - 1);
    }

    enum SeqType {dna, rna, aa, unknown};
    // instead of this enum, it might be better to have classes of
    // SeqRecord that correspond to types

    //SeqType guessType(std::string seq) {
        //SeqType typ = unknown;
        //for(i = 0; i < seq.length(); i++) {
            // make a set of DNA letters, RNA letters, and AA letters
            // and test for membership in each set
            // If any letters are outside these sets, the type is unknown
        //}
    //}

    class SeqRecord {
        /*
         * Class to hold a sequence for a single accession.
         */
        private:
            SeqType typ;
            std::string seq;
            std::string name;

        public:

            std::string getName() {
                return name;
            }

            void setName(std::string nm) {
                name = nm;
            }

            void setSeq(std::string sq) {
                seq = sq;
                typ = unknown;
            }

            void append(std::string sq, SeqType tp=unknown) {
                seq += sq;
                if(tp != unknown && tp == getType()) {
                } else {
                    typ = unknown;
                }
            }

            void append(SeqRecord sq) {
                seq = sq.getSeq() + seq;
                if (sq.getType() != getType() ){
                    typ = unknown;
                }
            }

            std::string getSeq() {
                return seq;
            }

            std::string getSeq(int64_t beg, int64_t end) {
                int64_t len = end - beg + 1;
                std::string ret;
                if(end > length()) {
                    if(beg > length()) {
                        ret = std::string(len, ' ');
                    } else {
                        ret = seq.substr(beg, (length() - beg)) +
                              std::string(end - length() + 1, ' ');
                    }
                } else {
                    ret = seq.substr(beg, len);
                }
                return ret;
            }

            std::vector<std::string> getCodon(int64_t beg, int64_t end,
                    int frame) {
            }

            int64_t length() {
                return seq.length();
            }

            SeqType getType() {
                return typ;
            }

            void setType(SeqType tp) {
                typ = tp;
            }

            //void guessType() {
             //   typ = guessType(seq);
            //}
    };

    class SeqSet {
        /*
         * Class to hold a set of sequences - an alignment, though
         * not necessarily the same length.
         */
        private:
            std::vector<SeqRecord> seqs;
            int64_t maxlen;
            int64_t nseqs;
            
        public:

            SeqSet() {
                maxlen = 0;
                nseqs = 0;
                std::vector<SeqRecord> seqs;
            }

            string filename;

            int64_t numseqs() {
                return nseqs;
            }

            int64_t length() {
                return maxlen;
            }

            std::vector<std::string> nameSlice(int64_t first,
                                               int64_t last) {
                std::vector<std::string> ret;
                ret.reserve(last - first + 1);
                for(int64_t i = first; i < last; i++) {
                    if(i >= nseqs) {
                        ret.push_back(" ");
                    } else {
                        ret.push_back(seqs[i].getName());
                    }
                }
                return ret;
            }

            std::vector<std::string> slice(int64_t beg, int64_t end,
                                           int64_t first, int64_t last) {
                std::vector<std::string> ret;
                ret.reserve(last - first + 1);

                for(int64_t i = first; i < last; i++) {
                    if(i > (nseqs - 1)) {
                        ret.push_back(std::string(end - beg + 1, ' '));
                    } else {
                        ret.push_back(seqs[i].getSeq(beg, end));
                    }
                }
                return ret;
            }

            char * column(int64_t pos) {
                char ret[nseqs];
                for(int64_t i = 0; i < nseqs; i++) {
                    ret[i] = (seqs[i].getSeq(i, i)).c_str()[0];
                }
            }

            void append(SeqRecord rec) {
                seqs.push_back(rec);
                if(rec.length() > maxlen) {
                    maxlen = rec.length();
                }
                nseqs += 1;
            }
    };

    //std::vector<bool> compare(std::vector<std::string>) {
        // will need a mode argument as well
    //}

    
    enum Com {QUIT, SCROLLUP, SCROLLDOWN, SCROLLRIGHT, SCROLLLEFT,
              COMPARETOGGLE, SCROLLMODE, COMPAREMODE, GOTO,
              GOTOEND, GOTOBEGIN, SCROLLTOP, SCROLLBOTTOM,
              SHOWHELP, CHANGEFOCUS, NAMEWIDTH, 
              SPECIAL};
    typedef std::pair<Com, int> Command;

    Com ssc[] = {SCROLLUP, SCROLLDOWN, SCROLLRIGHT, SCROLLLEFT,
                 SCROLLMODE, COMPAREMODE, GOTO, GOTOEND,
                 COMPARETOGGLE, GOTOBEGIN, SCROLLTOP,
                 SCROLLBOTTOM, NAMEWIDTH};
    set<int> seqSetCommands(ssc, ssc + 13);

    // forward declare
    void parseFasta(string filename, SeqSet &data);

    class SeqWindow {
        /*
         * Class to represent a SeqSet display
         *
         * Very much a stub
         *
         */
        private:
            WINDOW * window; // the ncurses window this maps to
            SeqSet seqs;
            int scrollmode;
            int width;
            int height;
            int names_width; // width allocated for names
            int64_t first_seq; // first sequence record, 0-based
            int64_t first_pos; // leftmost position in window; 0-based
            int num_pos_displayed;
            int num_seqs_displayed;
            int64_t last_pos;
            int64_t last_seq;
            bool isfocal;

        void _scroll(int64_t newleft, int64_t newtop) {
            first_seq = newtop;
            first_pos = newleft;
            display();
        }

        // Given the current window size and the width
        // of the names, figure out how many sequences and positions
        // should be displayed. Also recalculate last_pos and last_seq.
        void _recalculate_num_displayed() {
            // Need to reserve three lines - two for positions and one
            // for the filename
            // Need to reserve names_width + 1 columns for names
            num_pos_displayed = width - names_width - 1;
            num_seqs_displayed = height - 3;
            last_pos = first_pos + num_pos_displayed - 1;
            last_seq = first_seq + num_seqs_displayed - 1;
        }

        void _display_names() {
            if(isfocal)
                wattron(window, A_BOLD);
            wattron(window, COLOR_PAIR(7));
            std::vector<string> names = seqs.nameSlice(first_seq, last_seq);
            for(int i = 0; i < num_seqs_displayed - 1; i++) {
                mvwprintw(window, i+2, 0, string(names_width + 1, ' ').c_str());
                if(names_width > names[i].length())
                    mvwprintw(window, i+2, 0, names[i].c_str());
                else
                    mvwprintw(window, i + 2, 0,
                              names[i].substr(0, names_width).c_str());
            }
            if(isfocal)
                wattroff(window, A_BOLD);
            wattroff(window, COLOR_PAIR(7));
        }

        void _display_positions() {
            if(isfocal)
                wattron(window, A_BOLD);
            wattron(window, COLOR_PAIR(7));
            for(int i = 0; i < width; i++) {
                    mvwprintw(window, 0, i, " ");
                    mvwprintw(window, 1, i, " ");
            }
            int col = names_width + 1;
            for(int i = first_pos + 1; i < last_pos + 2; i++) {
                if(!(i % 10)) {
                    mvwprintw(window, 1, col, "|");
                    mvwprintw(window, 0, col, "%i", i);
                } else if(!(i % 5)) {
                    mvwprintw(window, 1, col, ":");
                } else {
                    mvwprintw(window, 1, col, ".");
                }
                col++;
            }
            // Sometimes the numbers spill over to the next line - this
            // gets rid of that.
            mvwprintw(window, 1, 0, string(names_width, ' ').c_str());
            if(isfocal)
                wattroff(window, A_BOLD);
            wattroff(window, COLOR_PAIR(7));
        }

        void _display_seqs() {
            // This function is more tricky - will require some added
            // arguments for formatting eventually.
            // Also may require some adjustment to color amino acid
            // sequences properly.
            std::vector<string> sequences = seqs.slice(first_pos, last_pos,
                                                       first_seq, last_seq);
            int row = 2;
            char ch;
            int col;
            for(int i = 0; i < num_seqs_displayed - 1; i++) {
                for(int j = 0; j < sequences[i].length(); j++) {
                    ch = sequences[i][j];
                    if(ch == 'A' || ch == 'a')
                        wattron(window, COLOR_PAIR(1));
                    else if(ch == 'T' || ch == 't')
                        wattron(window, COLOR_PAIR(2));
                    else if(ch == 'G' || ch == 'g')
                        wattron(window, COLOR_PAIR(3));
                    else if(ch == 'C' || ch == 'c')
                        wattron(window, COLOR_PAIR(4));
                    else
                        col = 7;
                    mvwaddch(window, row, names_width + 1 + j, ch);
                    wattroff(window, COLOR_PAIR(col));
                }
                row++;
            }
        }

        void _display_filename() {
            // display at bottom
            wattron(window, COLOR_PAIR(7) | A_UNDERLINE);
            if(isfocal)
                wattron(window, A_REVERSE | A_BOLD);
            mvwprintw(window, height - 2, 0, seqs.filename.c_str());
            for(int i = seqs.filename.length() + 1; i <= width; i++)
                mvwprintw(window, height - 2, i - 1, " ");
            if(isfocal)
                wattroff(window, A_REVERSE | A_BOLD);
            wattroff(window, COLOR_PAIR(7) | A_UNDERLINE);
        }

        public:

            SeqWindow() {
            }

            SeqWindow(int upperleftX, int upperleftY, 
                      int _width, int _height, SeqSet &sq) {
                window = newwin(_height, _width, upperleftY, upperleftX);
                width = _width;
                height = _height;
                seqs = sq;
                scrollmode = 1;
                names_width = 15;
                first_pos = 0;
                first_seq = 0;
                update_size();
                _recalculate_num_displayed();
                isfocal = true;
                display();
            }

            SeqWindow(int upperleftX, int upperleftY, 
                      int _width, int _height, string filename) {
                SeqSet sq;
                try {
                    parseFasta(filename, sq);
                } catch(exception &e) {
                    throw("");
                }
                window = newwin(_height, _width, upperleftY, upperleftX);
                width = _width + 1;
                height = _height + 1;
                seqs = sq;
                scrollmode = 1;
                names_width = 15;
                first_pos = 0;
                first_seq = 0;
                update_size();
                _recalculate_num_displayed();
                display();
            }

            ~SeqWindow() {
                wattron(window, COLOR_PAIR(8));
                for(int i = 0; i < height; i++) {
                    mvwprintw(window, i, 0, string(width, ' ').c_str());
                }
                wattroff(window, COLOR_PAIR(8));
                wrefresh(window);
                delwin(window);
            }

            void set_focus(bool focal) {
                isfocal = focal;
            }

            // scroll mode is in powers of 10
            void set_scroll_mode(int mode) {
                if(mode < 0 || mode > 9)
                    return;
                scrollmode = pow(10, mode);
            }

            void update_size() {
                getmaxyx(window, height, width);
                height += 1;
                width += 1;
            }

            void change_name_width(int newwidth) {
                if(newwidth < width - 5  && newwidth > 1) {
                    names_width = newwidth;
                    update_size();
                    _recalculate_num_displayed();
                }
            }

            // Deal with commands that change SeqSet params
            void handle_command(Command command) {
                Com com_name = command.first;
                int param = command.second;
                int newpos;
                if(com_name == SCROLLUP) {
                    if(first_seq > 0) {
                        newpos = first_seq - param;
                        if(newpos < 0)
                            newpos = 0;
                        _scroll(first_pos, newpos);
                    }
                } else if(com_name == SCROLLDOWN) {
                    if(first_seq < seqs.numseqs()) {
                        newpos = first_seq + param;
                        if(newpos >= seqs.numseqs())
                            newpos = seqs.numseqs() - 1;
                        _scroll(first_pos, newpos);
                    }
                } else if(com_name == SCROLLLEFT) {
                    if(first_pos > 0) {
                        newpos = first_pos - scrollmode * param;
                        if(newpos < 0)
                            newpos = 0;
                        _scroll(newpos, first_seq);
                    }
                } else if(com_name == SCROLLRIGHT) {
                    if(first_pos < seqs.length()) {
                        newpos = first_pos + scrollmode * param;
                        if(newpos >= seqs.length())
                            newpos = seqs.length() - 1;
                        _scroll(newpos, first_seq);
                    }
                } else if(com_name == SCROLLTOP) {
                    _scroll(first_pos, 0);
                } else if(com_name == SCROLLBOTTOM) {
                    _scroll(first_pos, seqs.numseqs() - 1);
                } else if(com_name == GOTOBEGIN) {
                    _scroll(0, first_seq);
                } else if(com_name == GOTOEND) {
                    _scroll(seqs.length() - 1, first_seq);
                } else if(com_name == GOTO) {
                    newpos = param - 1 - (width - names_width) / 2;
                    if(newpos < 0)
                        newpos = 0;
                    if(newpos > seqs.length() - 1)
                        newpos = seqs.length() - 1;
                    _scroll(newpos, first_seq);
                } else if(com_name == SCROLLMODE) {
                    set_scroll_mode(param);
                } else if(com_name == NAMEWIDTH) {
                    change_name_width(param);
                }
            }

            void resize(int upperleftX, int upperleftY,
                        int newwidth, int newheight) {
                wattron(window, COLOR_PAIR(8));
                for(int i = 0; i < height; i++) {
                    mvwprintw(window, i, 0, string(width, ' ').c_str());
                }
                wrefresh(window);
                delwin(window);
                window = newwin(newheight, newwidth,
                                upperleftY, upperleftX);
                height = newheight;
                width = newwidth;
                update_size();
                _recalculate_num_displayed();
                display();
            }

            // Refresh, get the slice of the sequence, add the formatting,
            // and use wprintw to put everything on the screen.
            void display() {
                update_size();
                _recalculate_num_displayed();
                _display_names();
                _display_positions();
                _display_seqs();
                _display_filename();
                wrefresh(window);
            }
    };

    class WindowSet {
        /*
         * Hold all the windows in the session
         *
         */

        private:
            int width, height; // unlike in SeqWindow, these are really xmax and ymax
            std::vector<SeqWindow*> windows;
            int focal_window;

            void adjust_to_fill_evenly() {
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
                //update();
            }

        public:

            WindowSet() {
                getmaxyx(stdscr, height, width);
                focal_window = 0;
                std::vector<SeqWindow*> windows();
            }

            void run_command(Command command) {
                // if command in SeqSet commands, pass it along
                // else run it here
            }

            void update_size() {
                getmaxyx(stdscr, height, width);
            }

            void print_message(string message) {
                mvwprintw(stdscr, height - 1, 0, string(width, ' ').c_str());
                mvwprintw(stdscr, height - 1, 0, message.c_str());
            }

            void update() {
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

            void add_window(string filename) {
                try {
                    SeqWindow * s = new SeqWindow(0, 0, width, height,
                                                  filename);
                    windows.push_back(s);
                    update_size();
                    adjust_to_fill_evenly();
                    change_focus(windows.size() - 1);
                    update();
                    return;
                } catch(exception &e) {
                }
                print_message("Cannot open file");
            }

            void change_focus(int newfocus) {
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
            bool close_focus() {
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

            bool handle_command(Command command) {
                if(seqSetCommands.count(command.first) > 0 &&
                   windows.size() > 0) {
                    windows[focal_window] -> handle_command(command);
                    return true;
                } else {
                    if(command.first == QUIT)
                        return close_focus();
                    if(command.first == CHANGEFOCUS) {
                        change_focus(command.second - 1);
                        return true;
                    }
                    if(command.first == SPECIAL) {
                        handle_special_command();
                        return true;
                    }
                }
            }

            void handle_special_command() {
                for(int i = 0; i <= width; i++)
                    mvwaddch(stdscr, height - 1, i, ' ');
                mvwprintw(stdscr, height - 1, 0, ":");
                string buffer = "";
                char ch;
                while(true) {
                    ch = getch();

                    // Eventually keep a stack of previous commands
                    // and cycle through them with UP and DOWN.

                    // Backspace
                    if(ch == KEY_BACKSPACE || ch == KEY_DL || ch == 7) {
                        backspace();
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
                    waddch(stdscr, ch);
                    buffer += ch;
                }
                // break buffer into tokens separated by spaces
                // and process
                std::vector<string> tokens;
                string tok = "";
                for(int i = 0; i < buffer.size(); i++) {
                    if(buffer[i] == ' ') {
                        tokens.push_back(string(tok));
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
                    add_window(tokens[1]);
                }

                // Clear space
                for(int i = 0; i <= width; i++)
                    mvwaddch(stdscr, height - 1, i, ' ');
            }
    };

    
    void parseFasta(string filename, SeqSet &data) {

        try {
            ifstream input(filename.c_str(), ifstream::in);

            data.filename = filename;

            char ch;
            string temp = "";
            string nm;

            // Enclose all of this in a while loop that goes to EOF:
            input.get(ch);
            if(ch != '>') {
                throw("Not in FASTA format");
            }


            bool inseq = false;
            bool linebreak = false;
            while(!input.eof()) {
                SeqRecord rec;
                nm = "";
                while (true && !inseq) {
                    input.get(ch);
                    if (ch == '\n' || ch == '\r')
                        inseq = true;
                    nm += ch;
                }
                rec.setName(nm);

                temp = "";
                while(inseq){
                    input.get(ch);
                    if(input.eof())
                        break;

                    // ">" after a linebreak means a new name
                    if(ch == '>' && linebreak) {
                        inseq = false;
                        linebreak = false;
                        continue;
                    }

                    // Ignore, but note linebreaks
                    linebreak = false;
                    if(ch == '\n' || ch == '\r') {
                        linebreak = true;
                        continue;
                    }

                    // Ignore whitespace
                    if(ch == ' ' || ch == '\t') {
                        continue;
                    }

                    temp += ch;
                }
                rec.append(temp);
                data.append(rec);
            }
        } catch (exception &e) {
            throw(e);
        }
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


    Command getCommand() {
        // Get the command followed by the parameters

        int param = 1;
        string param_buffer = "";
        string command_buffer = "";

        bool on_param = true;
        bool on_command = false;

        // need a map/tree that matches commands to characters
        while(TRUE) {
            char ch = getch();

            // ESC key clears the buffers
            if(ch == 27) {
                param = 1;
                param_buffer = "";
                command_buffer = "";
                on_param = true;
                on_command = false;
                continue;
            }

            // if a non-number was pushed, convert the 
            // param_buffer to int and look for the command
            if(ch > 57 || ch < 48) {
                on_command = true;
                on_param = false;
                if(param_buffer.length())
                    param = atoi(param_buffer.c_str());
            }

            // Start by looking for numbers
            if(on_param) {
                param_buffer += ch;
            }

            // if we are looking for a command
            if(on_command) {
                command_buffer += ch;
                if(!command_buffer.compare("q"))
                    return Command(QUIT, param);
                if(!command_buffer.compare("s"))
                    return Command(SCROLLMODE, param);
                if(!command_buffer.compare("j"))
                    return Command(SCROLLDOWN, param);
                if(!command_buffer.compare("J"))
                    return Command(SCROLLBOTTOM, param);
                if(!command_buffer.compare("k"))
                    return Command(SCROLLUP, param);
                if(!command_buffer.compare("K"))
                    return Command(SCROLLTOP, param);
                if(!command_buffer.compare("h"))
                    return Command(SCROLLLEFT, param);
                if(!command_buffer.compare("l"))
                    return Command(SCROLLRIGHT, param);
                if(!command_buffer.compare("H"))
                    return Command(SHOWHELP, param);
                if(!command_buffer.compare("g"))
                    return Command(GOTOBEGIN, param);
                if(!command_buffer.compare("w"))
                    return Command(CHANGEFOCUS, param);
                if(!command_buffer.compare("G")) {
                    if(param_buffer.length()) {
                        return Command(GOTO, param);
                    } else {
                        return Command(GOTOEND, param);
                    }
                }
                if(!command_buffer.compare("n"))
                    return Command(NAMEWIDTH, param);
                if(!command_buffer.compare(":"))
                    return Command(SPECIAL, param);
                
                // if no matches, clear the buffers
                param_buffer = "";
                command_buffer = "";
                on_command = false;
                on_param = true;
                param = 1;
            }

        }
    }
}

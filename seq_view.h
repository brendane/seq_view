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

using std::cout;
using std::endl;
using std::string;
using std::ifstream;

namespace SeqView {

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
              SHOWHELP};
    typedef std::pair<Com, int> Command;

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

        // Needs a destructor and constructor
        // Destructor should call delwin, deallocate display

        void _scroll(int64_t newleft, int64_t newtop) {
            first_seq = newtop;
            first_pos = newleft;
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
            int col = names_width + 1;
            for(int i = first_pos + 1; i < last_pos + 2; i++) {
                if(i % 10) {
                    mvwprintw(window, 1, col, ".");
                } else {
                    mvwprintw(window, 1, col, "|");
                    mvwprintw(window, 0, col, "%i", i);
                }
                col++;
            }
            if(isfocal)
                wattroff(window, A_BOLD);
            wattroff(window, COLOR_PAIR(7));
            wnoutrefresh(window);
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
            wattron(window, COLOR_PAIR(7));
            if(isfocal)
                wattron(window, A_REVERSE | A_BOLD);
            mvwprintw(window, height - 1, 0, seqs.filename.c_str());
            for(int i = seqs.filename.length() + 1; i <= width; i++)
                mvwprintw(window, height - 1, i - 1, " ");
            if(isfocal)
                wattroff(window, A_REVERSE | A_BOLD);
            wattroff(window, COLOR_PAIR(7));
        }

        public:

            SeqWindow(int upperleftX, int upperleftY, 
                      int _width, int _height, SeqSet &sq) {
                refresh(); // this should probably be moved to the
                           // WindowSet
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
                refresh(); // this should probably be moved to the
                           // WindowSet
                SeqSet sq;
                parseFasta(filename, sq);
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
                display();
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
            }

            void change_name_width(int newwidth) {
                update_size();
                if(newwidth < width - 5) {
                    names_width = newwidth;
                }
            }

            // Scroll the window or move to a particular location
            void scroll_window(Command scroll_command) {
                Com direction = scroll_command.first;
                int param = scroll_command.second;
                int newpos;
                if(direction == SCROLLUP) {
                    if(first_seq > 0) {
                        newpos = first_seq - (scrollmode * param);
                        if(newpos < 0)
                            newpos = 0;
                        _scroll(first_pos, newpos);
                    }
                } else if(direction == SCROLLDOWN) {
                    if(first_seq < seqs.numseqs()) {
                        newpos = first_seq + scrollmode * param;
                        if(newpos >= seqs.numseqs())
                            newpos = seqs.numseqs() - 1;
                        _scroll(first_pos, newpos);
                    }
                } else if(direction == SCROLLLEFT) {
                    if(first_pos > 0) {
                        newpos = first_pos - scrollmode * param;
                        if(newpos < 0)
                            newpos = 0;
                        _scroll(newpos, first_seq);
                    }
                } else if(direction == SCROLLRIGHT) {
                    if(first_pos < seqs.length()) {
                        newpos = first_pos + scrollmode * param;
                        if(newpos >= seqs.length())
                            newpos = seqs.length() - 1;
                        _scroll(newpos, first_seq);
                    }
                } else if(direction == SCROLLTOP) {
                    _scroll(first_pos, 0);
                } else if(direction == SCROLLBOTTOM) {
                    _scroll(first_pos, seqs.numseqs() - 1);
                } else if(direction == GOTOBEGIN) {
                    _scroll(0, first_seq);
                } else if(direction == GOTOEND) {
                    _scroll(seqs.length() - 1, first_seq);
                } else if(direction == GOTO) {
                    newpos = param;
                    if(newpos < 0)
                        newpos = 0;
                    if(newpos > seqs.length() - 1)
                        newpos = seqs.length() - 1;
                    _scroll(newpos, first_seq);
                }
            }

            void resize(int newwidth, int newheight) {
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
                wnoutrefresh(window);
                wrefresh(window);
                // call doupdate() in master loop
            }
    };

    /*
    class WindowSet {
        /*
         * Hold all the windows in the session
         *

        private:
            std::vector< *SeqWindow > windows;
            int focal_window;

        public:
            void add_window() {
            }
            void change_focus(int newfocus) {
            }
            void resize_focus(int newwidth, int newheight) {
            }
            void close_focus() {
                // remove a SeqWindow, and call its destructor
            }
    }
    */

    
    void parseFasta(string filename, SeqSet &data) {
        try {

            ifstream input(filename.c_str(), ifstream::in);

            data.filename = filename;

            char ch;
            string temp;
            string nm;

            // Enclose all of this in a while loop that goes to EOF:
            input.get(ch);
            if(ch != '>') {
                throw("Not in FASTA format");
            }


            while(!input.eof()) {
                SeqRecord rec;
                nm = "";
                while (true) {
                    input.get(ch);
                    if (ch == '\n')
                        break;
                    nm += ch;
                }

                rec.setName(nm);

                while(!input.eof()) {
                    input.get(ch);

                    if(ch == '>') {
                        break;
                    }
                    getline(input, temp);
                    rec.append(ch + temp);
                }
                data.append(rec);
            }
        } catch (ifstream::failure e) {
            cout << "Failure reading file" << endl;
            // should probably delete all the SeqRecords here??
            // or maybe that will be handled in a different piece of code
        }
    }

    void initDisplay() {
        initscr();
        keypad(stdscr, true);
        cbreak();
        noecho();
        curs_set(0);

        start_color();
        init_pair(1,  COLOR_RED,     COLOR_BLACK);
        init_pair(2,  COLOR_GREEN,   COLOR_BLACK);
        init_pair(3,  COLOR_YELLOW,  COLOR_BLACK);
        init_pair(4,  COLOR_BLUE,    COLOR_BLACK);
        init_pair(5,  COLOR_MAGENTA, COLOR_BLACK);
        init_pair(6,  COLOR_CYAN,    COLOR_BLACK);
        init_pair(7,  COLOR_WHITE,   COLOR_BLACK);
    }


    Command getCommand() {
        // Get the command followed by the parameters

        int param = 0;
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
                if(!command_buffer.compare("G")) {
                    if(param_buffer.length()) {
                        return Command(GOTO, param);
                    } else {
                        return Command(GOTOEND, param);
                    }
                }
            }

            //printw(param_buffer.c_str());
            //printw(":");
            //printw(command_buffer.c_str());
            //printw("  ");
        }
    }
}

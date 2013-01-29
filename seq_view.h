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
#ifndef SEQVIEW_H
#define SEQVIEW_H

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
    
    void initDisplay();
    void backspace();

    enum Com {QUIT, SCROLLUP, SCROLLDOWN, SCROLLRIGHT, SCROLLLEFT,
        COMPARETOGGLE, SCROLLMODE, COMPAREMODE, GOTO,
        GOTOEND, GOTOBEGIN, SCROLLTOP, SCROLLBOTTOM,
        SHOWHELP, CHANGEFOCUS, NAMEWIDTH, 
        SPECIAL, DISPLAYMODE, SETFRAME, COMPARE};
    typedef std::pair<Com, int> Command;
    extern Com ssc[];
    extern set<int> seqSetCommands;
    Command getCommand();

    enum DisplayMode {NORMAL, CODON};


    enum SeqType {dna, rna, aa, unknown};


    class SeqRecord {

        private:
            SeqType typ;
            std::string seq;
            std::string name;

        public:

            std::string getName();

            void setName(std::string nm);

            void setSeq(std::string sq);

            void append(std::string sq, SeqType tp=unknown);

            void append(SeqRecord sq);

            std::string getSeq();

            std::string getSeq(int64_t beg, int64_t end, 
                               DisplayMode mode, int frame=1);

            std::vector<std::string> getCodon(int64_t beg, int64_t end,
                    int frame);

            int64_t length();

            SeqType getType();

            void setType(SeqType tp);
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
            int frame;

            std::vector<std::string> _slice(int64_t beg, int64_t end, 
                    int64_t first, int64_t last,
                    DisplayMode mode);

        public:

            SeqSet();

            string filename;

            void set_frame(int f);

            int get_frame();

            int64_t numseqs();

            int64_t length();

            std::vector<std::string> nameSlice(int64_t first,
                    int64_t last);

            std::pair< std::vector<std::string>, std::vector<bool> >
                slice(int64_t beg, int64_t end, int64_t first, int64_t last,
                      DisplayMode mode, bool compare);


            std::vector<bool> do_compare(std::vector<string> s);


            char * column(int64_t pos);

            void append(SeqRecord rec);
    };

    //std::vector<bool> compare(std::vector<std::string>) {
    // will need a mode argument as well
    //}

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
            DisplayMode display_mode;
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
            bool modified;
            bool compare;

            void _scroll(int64_t newleft, int64_t newtop);

            void _recalculate_num_displayed();

            void _display_names();

            void _display_positions();

            void _display_seqs();

            void _display_filename();

        public:

            SeqWindow();

            SeqWindow(int upperleftX, int upperleftY, 
                    int _width, int _height, SeqSet &sq);

            SeqWindow(int upperleftX, int upperleftY, 
                    int _width, int _height, string filename);

            ~SeqWindow();

            void set_focus(bool focal);

            // scroll mode is in powers of 10
            void set_scroll_mode(int mode);

            void update_size();

            void change_name_width(int newwidth);

            // Deal with commands that change SeqSet params
            void handle_command(Command command);

            void resize(int upperleftX, int upperleftY,
                        int newwidth, int newheight);

            // Refresh, get the slice of the sequence, add the formatting,
            // and use wprintw to put everything on the screen.
            void display();
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
            void adjust_to_fill_evenly();

        public:

            WindowSet();

            void run_command(Command command);

            void update_size();

            void print_message(string message);

            void update();

            void add_window(string filename);

            void change_focus(int newfocus);

            // Return true if the program should keep going, 
            // otherwise false; If there any windows left, 
            // keep going.
            bool close_focus();

            bool handle_command(Command command);

            void handle_special_command();
    };

    void parseFasta(string filename, SeqSet &data);
}
#endif

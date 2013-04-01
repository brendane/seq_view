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
 * Header file - contains declarations for all functions and
 * classes.
 *
 * Also global variables:
 *
 * ssc : Commands that are handled by the SeqSet class
 * seqSetCommands : A set of the above
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
    
    /*
     * Utilities
     */

    void initDisplay();
    void backspace();
    void clear_line(int line);


    /*
     * Command-related declarations
     */

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


    /*
     * Typedef for comparison functions; these are implemented in
     * Comparison.cpp
     */

    std::vector<bool> simple_compare(std::vector<string> s);
    std::vector<bool> nuc_compare(std::vector<string> s);
    std::vector<bool> aa_compare(std::vector<string> s);

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

            // beg and end are 0-based.
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

            // The string vector is the set of sequences to display
            // The boolean vector is whether there is a match
            std::pair< std::vector<std::string>, std::vector<bool> >
                slice(int64_t beg, int64_t end, int64_t first, int64_t last,
                      DisplayMode mode, bool compare);

            char * column(int64_t pos);

            void append(SeqRecord rec);
    };



    /*
     * PARSER DECLARATIONS
     *
     * ParserFunction defines the type for functions that take a
     * filename and a reference to a SeqSet object and fill the
     * SeqSet with data from the file.
     *
     */

    typedef void (*ParserFunction)(string filename, SeqSet &data);
    void parseFasta(string filename, SeqSet &data);


    /*
     * SeqWindow contains a single SeqSet object (one alignment) and
     * information about how it should be displayed:
     *   mode, position, etc.
     *
     * Also contains a pointer to the curses window the sequences
     * should be displayed in.
     *
     * Destructor prints the window over with blank spaces and
     * calls delwin.
     *
     */
     
    class SeqWindow {
        private:
            WINDOW * window;        // the ncurses window this maps to
            SeqSet seqs;
            int scrollmode;
            DisplayMode display_mode;
            int width;              // window height
            int height;             // window width
            int names_width;        // width allocated for names
            int64_t first_seq;      // first sequence record, 0-based
            int64_t first_pos;      // leftmost position in window; 0-based
            int num_pos_displayed;  // Number of positions (width of window - width allocated to names)
            int num_seqs_displayed; // Number of records = height of window - lines allocated to other information
            int64_t last_pos;       // rightmost position
            int64_t last_seq;       // bottom sequence
            bool isfocal;           // whether this is the focal window
            bool modified;          // flag to determine whether to redraw the window
            bool compare;           // whether comparison is on

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
                    int _width, int _height, string filename,
                    ParserFunction parser=&parseFasta);

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

}
#endif

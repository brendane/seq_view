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

#include <math.h>
#include <ctype.h>
#include <stdlib.h>

#include <exception>
#include <fstream>
#include <iostream>
#include <map>
#include <set>
#include <stdint.h>
#include <string>
#include <vector>

#include <ncurses.h>

using std::cout;
using std::endl;
using std::exception;
using std::getline;
using std::ifstream;
using std::istream;
using std::map;
using std::set;
using std::string;

namespace SeqView {

    /*
     * Settings
     */
    typedef std::map<char,int> ColMap;
    typedef ColMap::const_iterator ColMapIt;
    const ColMap dna_colors = {
        {'A', 1}, {'a', 1}, {'T', 2}, {'t', 2},
        {'U', 2}, {'u', 2},
        {'G', 3}, {'g', 3}, {'C', 6}, {'c', 6},
        {'N', 7}, {'n', 7}, {'-', 7}, {'*', 7}}; // Has to be const if defined here

    const ColMap aa_colors = {
        {'R', 1}, {'K', 1}, {'D', 1}, {'E', 1},
        {'r', 1}, {'k', 1}, {'d', 1}, {'e', 1},
        {'Q', 2}, {'N', 2}, {'H', 2}, {'S', 2}, {'T', 2}, {'Y', 2}, {'C', 2},
        {'q', 2}, {'n', 2}, {'h', 2}, {'s', 2}, {'t', 2}, {'y', 2}, {'c', 2},
        {'W', 3}, {'Y', 3}, {'M', 3},
        {'w', 3}, {'y', 3}, {'m', 3},
        {'A', 6}, {'I', 6}, {'L', 6}, {'M', 6}, {'F', 6}, {'V', 6}, {'P', 6}, {'G', 6},
        {'a', 6}, {'i', 6}, {'l', 6}, {'m', 6}, {'f', 6}, {'v', 6}, {'p', 6}, {'g', 6},
        {'X', 7}, {'x', 7}, {'*', 4}};

    typedef std::map<std::string, std::string> CodonTable;
    static CodonTable std_codon_table = {
        {"TTT", "F"}, {"TTC", "F"}, {"TTA", "L"}, {"TTG", "L"},
        {"TCT", "S"}, {"TCC", "S"}, {"TCA", "S"}, {"TCG", "S"},
        {"TAT", "Y"}, {"TAC", "Y"}, {"TAA", "*"}, {"TAG", "*"},
        {"TGT", "C"}, {"TGC", "C"}, {"TGA", "*"}, {"TGG", "W"},
        {"CTT", "L"}, {"CTC", "L"}, {"CTA", "L"}, {"CTG", "L"},
        {"CCT", "P"}, {"CCC", "P"}, {"CCA", "P"}, {"CCG", "P"},
        {"CAT", "H"}, {"CAC", "H"}, {"CAA", "Q"}, {"CAG", "Q"},
        {"CGT", "R"}, {"CGC", "R"}, {"CGA", "R"}, {"CGG", "R"},
        {"ATT", "I"}, {"ATC", "I"}, {"ATA", "I"}, {"ATG", "M"},
        {"ACT", "T"}, {"ACC", "T"}, {"ACA", "T"}, {"ACG", "T"},
        {"AAT", "N"}, {"AAC", "N"}, {"AAA", "K"}, {"AAG", "K"},
        {"AGT", "S"}, {"AGC", "S"}, {"AGA", "R"}, {"AGG", "R"},
        {"GTT", "V"}, {"GTC", "V"}, {"GTA", "V"}, {"GTG", "V"},
        {"GCT", "A"}, {"GCC", "A"}, {"GCA", "A"}, {"GCG", "A"},
        {"GAT", "D"}, {"GAC", "D"}, {"GAA", "E"}, {"GAG", "E"},
        {"GGT", "G"}, {"GGC", "G"}, {"GGA", "G"}, {"GGG", "G"},
        {"UUU", "F"}, {"UUC", "F"}, {"UUA", "L"}, {"UUG", "L"},
        {"UCU", "S"}, {"UCC", "S"}, {"UCA", "S"}, {"UCG", "S"},
        {"UAU", "Y"}, {"UAC", "Y"}, {"UAA", "*"}, {"UAG", "*"},
        {"UGU", "C"}, {"UGC", "C"}, {"UGA", "*"}, {"UGG", "W"},
        {"CUU", "L"}, {"CUC", "L"}, {"CUA", "L"}, {"CUG", "L"},
        {"CCU", "P"}, {"CAU", "H"}, {"CGU", "R"}, {"AUU", "I"},
        {"AUC", "I"}, {"AUA", "I"}, {"AUG", "M"}, {"ACU", "U"},
        {"ACC", "U"}, {"ACA", "U"}, {"ACG", "U"}, {"AAU", "N"},
        {"AGU", "S"}, {"GUU", "V"}, {"GUC", "V"}, {"GUA", "V"},
        {"GUG", "V"}, {"GCU", "A"}, {"GAU", "D"}, {"GGU", "G"}
    };

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
        SHOWHELP, CHANGEFOCUS, CHANGEFOCUSREV, NAMEWIDTH, 
        SPECIAL, DISPLAYMODE, SETFRAME, COMPARE, TOGGLEBOLD,
        RESIZE, ALLQUIT};
    typedef std::pair<Com, int> Command;
    extern Com ssc[];
    extern set<int> seqSetCommands;
    Command getCommand();

    enum DisplayMode {NORMAL, CODON, TEN, TRANSLATE};

    enum SeqType {dna, rna, aa, unknown};


    /*
     * Typedef for comparison functions; these are implemented in
     * Comparison.cpp
     */

    std::vector<bool> simple_compare(std::vector<string> s);
    std::vector<bool> nuc_compare(std::vector<string> s);
    std::vector<bool> aa_compare(std::vector<string> s);
    enum ComparisonMode {NOCOMPARE, NUCAMB, NUCAMBNOGAP,
        NUC, NUCNOGAP, TRANSLATED, TRANSLATEDNOGAP,
        CDN, CDNNOGAP, PLAIN, AA, AANOGAP};
    static std::map<std::string, ComparisonMode> CompModeMap = 
        {
            {"nocompare", NOCOMPARE}, {"off", NOCOMPARE},
            {"nucamb", NUCAMB}, {"nucambnogap", NUCAMBNOGAP},
            {"nuc", NUC}, {"nucnogap", NUCNOGAP},
            {"translated", TRANSLATED}, {"translatednogap", TRANSLATEDNOGAP},
            {"codon", CDN}, {"codonnogap", CDNNOGAP},
            {"plain", PLAIN}, {"aa", AA}, {"aanogap", AANOGAP}
        };

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

            void reserve(unsigned new_cap);

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
                      DisplayMode mode, ComparisonMode compare);

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

    class SeqStream {
        private:
            istream * is;
            char buffer[200];
            unsigned ipos;
            unsigned opos;

        public:
            SeqStream(istream * is_);
            bool good();
            bool eof();
            void get(char &ch);
            bool start_over();
    };

    typedef void (*ParserFunction)(string filename, SeqSet &data);
    void parseSeqs(string filename, SeqSet &data);
    void parseFasta(SeqStream input_stream, string filename, SeqSet &data);
    void parseFastq(SeqStream input_stream, string filename, SeqSet &data);
    string guessFormat(SeqStream s, string &format);
    string guessFormat(SeqStream s);
    std::istream * openSeqFile(string filename, ifstream &input);

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
            ComparisonMode compare; // whether comparison is on and what type of comparison
            ComparisonMode pcomp;   // previous comparison mode
            bool bolded;            // whether the sequences are displayed in bold
            int transl_adj;         // position adjustment for translation mode

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
                    ParserFunction parser=&parseSeqs);

            ~SeqWindow();

            void set_focus(bool focal);

            int64_t numseqs();

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
            unsigned focal_window;
            void adjust_to_fill_evenly();
            void adjust_to_fill_proportionally();

        public:

            WindowSet();

            void update_size();

            void print_message(string message);

            void update();

            void add_window(string filename);

            void change_focus(int newfocus);

            // Return true if the program should keep going, 
            // otherwise false; If there any windows left, 
            // keep going.
            bool close_focus();

            void close_all();

            bool handle_command(Command command);

            void handle_special_command();
    };

}
#endif

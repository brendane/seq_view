/*
 * Classes to compare sequences
 *
 * All have a constructor which takes the beginning and end of the displayed
 * sequence (0-based) and the frame (1, 2, or 3). They have a get_slice()
 * function which returns a pair with the first and last positions (0-based)
 * to use to slice the sequence. They also have a compare() function which
 * takes the slice (a vector<string>) and does the actual comparison.
 *
 */

#include<seq_view.h>

namespace SeqView {
    
//    typedef std::vector<bool> (*CompareFunction)(std::vector<string>);


    // Just check if all the accessions have the same letter,
    // ignoring blank spaces (past the end of sequences).
    std::vector<bool> simple_compare(std::vector<string> s) {
        std::vector <bool> ret;
        ret.reserve(s[0].size());
        for(int pos = 0; pos < s[0].size(); pos++) {
            char matches = 15; // bit array to hold matches to A,T,C,G
            bool match = true;
            char ch = toupper(s[0][pos]);
            for(int i = 1; i < s.size(); i++) {
                if(ch != toupper(s[i][pos])) {
                    if(ch == ' ') {
                        // Blank should not count as a mismatch
                        ch = toupper(s[i][pos]);
                    } else {
                        match = false;
                        break;
                    }
                }
                if(ch != ' ') {
                    match = false;
                    break;
                }
            }
            // All accessions are blank
            if(match && ch == ' ')
                match = false;
            ret.push_back(match);
        }
        return ret;
    }

    // Nucleotide comparison taking into account IUPAC ambiguity codes
    std::vector<bool> nuc_compare(std::vector<string> s) {
        std::vector <bool> ret;
        ret.reserve(s[0].size());
        for(int pos = 0; pos < s[0].size(); pos++) {
            char matches = 15; // bit array to hold matches to A,T,C,G
            bool match = true;
            bool all_spaces = true;
            for(int i = 0; i < s.size(); i++) {
                char ch = toupper(s[i][pos]);
                if(ch != ' ')
                    all_spaces = false;
                if(ch == 'A') {
                    matches &= ~14;
                } else if(ch == 'T') {
                    matches &= ~13;
                } else if(ch == 'C') {
                    matches &= ~11;
                } else if(ch == 'G') {
                    matches &= ~7;
                } else if(ch == 'N' || ch == 'X' || ch == '-' || ch == ' ') {
                } else if(ch == 'M') {
                    matches &= ~5;
                } else if(ch == 'R') {
                    matches &= ~9;
                } else if(ch == 'W') {
                    matches &= ~3;
                } else if(ch == 'S') {
                    matches &= ~12;
                } else if(ch == 'Y') {
                    matches &= ~6;
                } else if(ch == 'K') {
                    matches &= ~10;
                } else if(ch == 'V') {
                    matches &= ~2;
                } else if(ch == 'H') {
                    matches &= ~8;
                } else if(ch == 'D') {
                    matches &= ~4;
                } else if(ch == 'B') {
                    matches &= ~1;
                } else {
                    matches = 0;
                }
                if(matches == 0) {
                    match = false;
                    break;
                }
            }
            if(match && !all_spaces)
                ret.push_back(true);
            else
                ret.push_back(false);
        }
        return ret;
    }

    // now codon comparison: check if codon sequence is the same
    // How to handle ambiguities? Probably best to just treat them
    // as mismatches

    // translated comparison - see if codons match the same amino
    // acid


   // Amino acid comparison taking into account ambiguities
   // Treats X and "-" as matching anything; otherwise works the
   // same as the simple comparison function (does not check that
   // the character is a valid AA code letter).
    std::vector<bool> aa_compare(std::vector<string> s) {
        std::vector <bool> ret;
        ret.reserve(s[0].size());
        for(int pos = 0; pos < s[0].size(); pos++) {
            bool match = true;
            bool all_spaces = true;
            char ch = toupper(s[0][pos]);
            char chh;
            for(int i = 1; i < s.size(); i++) {
                char chh = toupper(s[i][pos]);
                if(chh != ' ')
                    all_spaces = false;
                if(chh == ' ' || ch == 'X' || ch == '-') {
                } else if(ch != chh) {
                    if(ch == ' ') {
                        ch = chh;
                    } else {
                        match = false;
                    }
                }
                if(!match) {
                    break;
                }
            }
            if(match && !all_spaces)
                ret.push_back(true);
            else
                ret.push_back(false);
        }
        return ret;
    }

    class ComparisonBase {

        private:
            int64_t original_beg;
            int64_t original_end;
            int frame;
            int64_t request_beg;
            int64_t request_end;

        public:

            ComparisonBase(int64_t beg, int64_t end, int _frame) {
                original_beg = beg;
                original_end = end;
                frame = _frame;
            }

        public:
            // Get the start and end of the slice needed for the comparison
            virtual const std::pair<int64_t, int64_t> get_slice() = 0;

            // Return the vector of match / not match
            virtual const std::vector<bool> compare(std::vector<string> s) = 0;
    };

    class SimpleCompare: public ComparisonBase {

        private:
            int64_t original_beg;
            int64_t original_end;
            int frame;
            int64_t request_beg;
            int64_t request_end;

        public:

            SimpleCompare(int64_t beg, int64_t end, int _frame)
                : ComparisonBase(beg, end, _frame) {};

        virtual const std::pair<int64_t, int64_t> get_slice() {
            return std::make_pair(original_beg, original_end);
        }

        virtual const std::vector<bool> compare(std::vector<string> s) {
            std::vector <bool> ret;
            ret.reserve(s[0].size());
            for(int pos = 0; pos < s[0].size(); pos++) {
                char matches = 15; // bit array to hold matches to A,T,C,G
                bool match = true;
                char ch = toupper(s[0][pos]);
                for(int i = 1; i < s.size(); i++) {
                    if(ch != toupper(s[i][pos])) {
                        if(ch == ' ') {
                            // Blank should not count as a mismatch
                            ch = toupper(s[i][pos]);
                        } else {
                            match = false;
                            break;
                        }
                    }
                    if(ch != ' ') {
                        match = false;
                        break;
                    }
                }
                // All accessions are blank
                if(match && ch == ' ')
                    match = false;
                ret.push_back(match);
            }
            return ret;
        }
    };


    class NucCompare: public ComparisonBase {

        private:
            int64_t original_beg;
            int64_t original_end;
            int frame;
            int64_t request_beg;
            int64_t request_end;

        public:

            NucCompare(int64_t beg, int64_t end, int _frame)
                : ComparisonBase(beg, end, _frame) {};

        virtual const std::pair<int64_t, int64_t> get_slice() {
            return std::make_pair(original_beg, original_end);
        }

        virtual const std::vector<bool> compare(std::vector<string> s) {
            std::vector <bool> ret;
            ret.reserve(s[0].size());
            for(int pos = 0; pos < s[0].size(); pos++) {
                char matches = 15; // bit array to hold matches to A,T,C,G
                bool match = true;
                bool all_spaces = true;
                for(int i = 0; i < s.size(); i++) {
                    char ch = toupper(s[i][pos]);
                    if(ch != ' ')
                        all_spaces = false;
                    switch (ch) {
                        case 'A':
                            matches &= ~14;
                            break;
                        case 'T':
                            matches &= ~13;
                            break;
                        case 'C':
                            matches &= ~11;
                            break;
                        case 'G':
                            matches &= ~7;
                            break;
                        case 'N':
                            break;
                        case 'X':
                            break;
                        case '-':
                            break;
                        case ' ':
                            break;
                        case 'M':
                            matches &= ~5;
                            break;
                        case 'R':
                            matches &= ~9;
                            break;
                        case 'W':
                            matches &= ~3;
                            break;
                        case 'S':
                            matches &= ~12;
                            break;
                        case 'Y':
                            matches &= ~6;
                            break;
                        case 'K':
                            matches &= ~10;
                            break;
                        case 'V':
                            matches &= ~2;
                            break;
                        case 'H':
                            matches &= ~8;
                            break;
                        case 'D':
                            matches &= ~4;
                            break;
                        case 'B':
                            matches &= ~1;
                            break;
                        default:
                            matches = 0;
                    }
                    if(matches == 0) {
                        match = false;
                        break;
                    }
                }
                if(match && !all_spaces)
                    ret.push_back(true);
                else
                    ret.push_back(false);
            }
            return ret;
        }
    };


    class CodonNucCompare: public ComparisonBase {
        // Compare codons for exactly matching nucleotide sequence
        // Ambiguities are counted as mismatches.

        // Initialize these with a constructor, then let slice
        // produce a slice using private member variables.
        //
        // TODO: Adjust base class and other comparison classes to have
        // these private variables and a constructor.
        private:
            int64_t original_beg;
            int64_t original_end;
            int frame;
            int64_t request_beg;
            int64_t request_end;

        public:

            CodonNucCompare(int64_t beg, int64_t end, int _frame)
                : ComparisonBase(beg, end, _frame) {};

            virtual const std::pair<int64_t, int64_t> get_slice() {
                // Note that beg and end are 0-based
                // beg_offset is the position in the codon (0-based) of
                // the beginning of the sequence
                int64_t beg = original_beg;
                int64_t end = original_end;
                int beg_offset = ((beg - frame - 1) % 3) - 1;
                int end_offset = ((end - frame - 1) % 3);
                beg -= beg_offset;
                if(beg < 0) beg = 0;
                end += end_offset;

                request_beg = beg; request_end = end;

            return std::make_pair(beg, end);

        }


        // TODO: make the comparison function!
        // Remember that because begin and end might not be the same as the
        // begin and end displayed on the screen, the return vector needs to
        // be adjusted to the right length
        //
        // Also, make sure the algorithm can deal with the spaces in codon
        // mode (spaces between codons). Something like - if all_spaces and
        // just finished a whole codon, ignore...

    };


    //class CompareTranslateCodon: ComparisonBase {
    // // compare nucleotide codon sequences to see if they translate to
    // // the same amino acid.
    //};


    // TODO: If a translated mode is added, make sure that the comparison mode
    // is adjusted to make sense with amino acids (or just set to SimpleComparison
    // whenever certain modes are changed).
}

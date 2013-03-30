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
 * Functions to compare sequences
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
}

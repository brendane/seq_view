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

#include<seq_view.h>

namespace SeqView {

    std::vector<std::string> SeqSet::_slice(int64_t beg, int64_t end, 
            int64_t first, int64_t last,
            DisplayMode mode) {
        std::vector<std::string> ret;
        ret.reserve(last - first + 1);

        for(int64_t i = first; i < last; i++) {
            if(i > (nseqs - 1)) {
                ret.push_back(std::string(end - beg + 1, ' '));
            } else {
                ret.push_back(seqs[i].getSeq(beg, end, mode, frame));
            }
        }
        std::vector<bool> comps;
        return ret;
    }

    SeqSet::SeqSet() {
        maxlen = 0;
        nseqs = 0;
        frame = 1;
        std::vector<SeqRecord> seqs;
    }

    void SeqSet::set_frame(int f) {
        if(f < 1 || f > 3) {
        } else {
            frame = f;
        }
    }

    int SeqSet::get_frame() {
        return frame;
    }

    int64_t SeqSet::numseqs() {
        return nseqs;
    }

    int64_t SeqSet::length() {
        return maxlen;
    }

    std::vector<std::string> SeqSet::nameSlice(int64_t first,
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

    std::pair< std::vector<std::string>, std::vector<bool> >
        SeqSet::slice(int64_t beg, int64_t end, int64_t first, int64_t last,
                DisplayMode mode, bool compare) {
            std::vector<std::string> ret = _slice(beg, end, first, last, mode);
            ret.reserve(last - first + 1);
            std::vector<bool> comps;
            if(compare)
                comps = do_compare(_slice(beg, end, 0, numseqs(), mode));
            return std::make_pair(ret, comps);
        }


    std::vector<bool> SeqSet::do_compare(std::vector<string> s) {
        // For now, just a simple comparison that considers ambiguities
        // in nucleotide sequence; anything that is not an IUPAC DNA
        // letter is considered completely ambiguous

        std::vector <bool> ret;
        ret.reserve(s[0].size());
        for(int pos = 0; pos < s[0].size(); pos++) {
            // A, T, C, G
            bool matches[4] = {true, true, true, true};
            bool match = true;
            bool all_spaces = true;
            for(int i = 0; i < s.size(); i++) {
                char ch = s[i][pos];
                if(ch != ' ')
                    all_spaces = false;
                if(ch == 'A' || ch == 'a') {
                    matches[1] = false;
                    matches[2] = false;
                    matches[3] = false;
                } else if(ch == 'T' || ch == 't') {
                    matches[0] = false;
                    matches[2] = false;
                    matches[3] = false;
                } else if(ch == 'C' || ch == 'c') {
                    matches[0] = false;
                    matches[1] = false;
                    matches[3] = false;
                } else if(ch == 'G' || ch == 'g') {
                    matches[0] = false;
                    matches[1] = false;
                    matches[2] = false;
                }
                // IUPAC ambiguity codes go here
                if(!matches[0] && !matches[1] && !matches[2] && !matches[3]) {
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


    char * SeqSet::column(int64_t pos) {
        char ret[nseqs];
        for(int64_t i = 0; i < nseqs; i++) {
            ret[i] = (seqs[i].getSeq(i, i, NORMAL)).c_str()[0];
        }
    }

    void SeqSet::append(SeqRecord rec) {
        seqs.push_back(rec);
        if(rec.length() > maxlen) {
            maxlen = rec.length();
        }
        nseqs += 1;
    }
}

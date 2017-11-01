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
                comps = nuc_compare(_slice(beg, end, 0, numseqs(), mode));
            return std::make_pair(ret, comps);
        }

    void SeqSet::append(SeqRecord rec) {
        seqs.push_back(rec);
        if(rec.length() > maxlen) {
            maxlen = rec.length();
        }
        nseqs += 1;
    }
}

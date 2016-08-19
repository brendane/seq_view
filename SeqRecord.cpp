#include<seq_view.h>

namespace SeqView {

    std::string SeqRecord::getName() {
        return name;
    }

    void SeqRecord::setName(std::string nm) {
        name = nm;
    }

    void SeqRecord::setSeq(std::string sq) {
        seq = sq;
        typ = unknown;
    }

    void SeqRecord::append(std::string sq, SeqType tp) {
        seq += sq;
        if(tp != unknown && tp == getType()) {
        } else {
            typ = unknown;
        }
    }

    void SeqRecord::append(SeqRecord sq) {
        seq = sq.getSeq() + seq;
        if (sq.getType() != getType() ){
            typ = unknown;
        }
    }

    std::string SeqRecord::getSeq() {
        return seq;
    }

    std::string SeqRecord::getSeq(int64_t beg, int64_t end, 
            DisplayMode mode, int frame) {
        int64_t len = end - beg + 1;
        std::string ret;
        if(end > length()) {
            if(beg > length()) {
                ret = std::string(len, ' ');
            } else {
                if(mode == NORMAL) {
                    ret = seq.substr(beg, (length() - beg)) +
                        std::string(end - length() + 1, ' ');
                } else if(mode == CODON) {
                    int pos = beg;
                    for(int i = 0; i < length() - beg; i++) {
                        if((pos - frame - 1) % 3 == 0) {
                            ret += seq.substr(pos, 1);
                            ret += " ";
                        } else {
                            ret += seq.substr(pos, 1);
                        }
                        pos++;
                    }
                    ret += string(end - length() - 1, ' ');
                }
            }
        } else {
            if(mode == NORMAL) {
                ret = seq.substr(beg, len);
            } else if(mode == CODON) {
                int pos = beg;
                for(int i = 0; i < len; i++) {
                    if((pos - frame - 1) % 3 == 0) {
                        ret += seq.substr(pos, 1);
                        ret += " ";
                    } else {
                        ret += seq.substr(pos, 1);
                    }
                    pos++;
                }
            }

        }
        return ret;
    }

    std::vector<std::string> SeqRecord::getCodon(int64_t beg, int64_t end,
            int frame) {
    }

    int64_t SeqRecord::length() {
        return seq.length();
    }

    SeqType SeqRecord::getType() {
        return typ;
    }

    void SeqRecord::setType(SeqType tp) {
        typ = tp;
    }
}

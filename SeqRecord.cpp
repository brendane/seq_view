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

    void SeqRecord::reserve(unsigned new_cap) {
        seq.reserve(new_cap);
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
                // TODO: figure out whether calls to length() are appropriate
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
                } else if(mode == TEN) {
                    int pos = beg;
                    for(int i = 0; i < length() - beg; i++) {
                        if((pos + 1) % 10 == 0) {
                            ret += seq.substr(pos, 1);
                            ret += " ";
                        } else {
                            ret += seq.substr(pos, 1);
                        }
                        pos++;
                    }
                    ret += string(end - length() - 1, ' ');
                } else if (mode == TRANSLATE) {
                    int pos = beg - ((beg - frame + 1) % 3);
                    if(pos < 0) pos = 0;
                    std::string codon = "";
                    for(int i = 0; i < length() - beg; i++) {
                        if((pos - frame - 1) % 3 == 0) {
                            codon += seq.substr(pos, 1);
                            if(std_codon_table.count(codon)) {
                                ret += std_codon_table[codon];
                            } else {
                                ret += "X";
                            }
                            codon = "";
                        } else {
                            codon += seq.substr(pos, 1);
                        }
                        pos++;
                    }
                    if(pos < length() && codon.size() < 3) {
                        codon += seq.substr(pos, 3-codon.size());
                        if(std_codon_table.count(codon)) {
                            ret += std_codon_table[codon];
                        } else {
                            ret += "X";
                        }
                    }
                    ret += string((end - length() - 1)/3, ' ');

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
            } else if(mode == TEN) {
                int pos = beg;
                for(int i = 0; i < len; i++) {
                    if((pos + 1) % 10 == 0) {
                        ret += seq.substr(pos, 1);
                        ret += " ";
                    } else {
                        ret += seq.substr(pos, 1);
                    }
                    pos++;
                }
            } else if (mode == TRANSLATE) {
                int pos = beg - ((beg - frame + 1) % 3);
                if(pos < 0) pos = 0;
                std::string codon = "";
                for(int i = 0; i < length() - beg; i++) {
                    if((pos - frame - 1) % 3 == 0) {
                        codon += seq.substr(pos, 1);
                        if(std_codon_table.count(codon)) {
                            ret += std_codon_table[codon];
                        } else {
                            ret += "X";
                        }
                        codon = "";
                    } else {
                        codon += seq.substr(pos, 1);
                    }
                    pos++;
                }
                if(pos < length() && codon.size() < 3) {
                    codon += seq.substr(pos, 3-codon.size());
                    if(std_codon_table.count(codon)) {
                        ret += std_codon_table[codon];
                    } else {
                        ret += "X";
                    }
                }
            }

        }
        return ret;
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

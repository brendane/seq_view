#include<seq_view.h>

namespace SeqView {

    SeqStream::SeqStream(istream * is_) {
        is = is_;
        ipos = 0;
        opos = 0;
        while(ipos < 199 && is->good() && !is->eof()) {
            char ch;
            is->get(ch);
            buffer[ipos] = ch;
            ipos += 1;
        }
    }

    bool SeqStream::good() {
        return is->good() || ipos != opos;
    }

    bool SeqStream::eof() {
        return opos > ipos && is->eof();
    }

    void SeqStream::get(char &ch) {
        if(opos < ipos) {
            ch = buffer[opos];
            opos += 1;
        } else {
            is->get(ch);
            opos += 1;
        }
    }

    bool SeqStream::start_over() {
        bool okay = false;
        if(opos <= ipos) {
            opos = 0;
            okay = true;
        }
        return okay;
    }
}

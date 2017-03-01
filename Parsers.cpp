#include<seq_view.h>

namespace SeqView {

    void parseFasta(string filename, SeqSet &data) {

        //try {
            //istream * inputstream;
            ifstream input;
            std::istream * input_stream;
            if(filename == "-") {
                input_stream = &std::cin;
            } else {
                input.open(filename.c_str(), ifstream::in);
                input_stream = &input;
            }

            data.filename = filename;

            char ch;
            string temp = "";
            string nm;

            // Enclose all of this in a while loop that goes to EOF:
            input_stream->get(ch);
            if(ch != '>') {
                throw("Not in FASTA format");
            }


            bool inseq = false;
            bool linebreak = false;
            while(!input_stream->eof()) {
                SeqRecord rec;
                nm = "";
                while (true && !inseq) {
                    input_stream->get(ch);
                    if (ch == '\n' || ch == '\r')
                        inseq = true;
                    nm += ch;
                }
                rec.setName(nm);

                temp = "";
                while(inseq){
                    input_stream->get(ch);
                    if(input_stream->eof())
                        break;

                    // ">" after a linebreak means a new name
                    if(ch == '>' && linebreak) {
                        inseq = false;
                        linebreak = false;
                        continue;
                    }

                    // Ignore, but note linebreaks
                    linebreak = false;
                    if(ch == '\n' || ch == '\r') {
                        linebreak = true;
                        continue;
                    }

                    // Ignore whitespace
                    if(ch == ' ' || ch == '\t') {
                        continue;
                    }

                    temp += ch;
                }
                rec.append(temp);
                data.append(rec);
            }
        //} catch (...) {
        //    throw("Problem parsing file");
        //}

        input.close();
    }
}

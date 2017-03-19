#include<seq_view.h>

namespace SeqView {

    std::istream * openSeqFile(string filename, ifstream &input) {
        std::istream * input_stream;
        if(filename == "-") {
            input_stream = &std::cin;
        } else {
            input.open(filename.c_str(), ifstream::in);
            input_stream = &input;
        }
        if(!input_stream->good())
            throw("Problem reading file");
        return input_stream;
    }

    void parseFasta(string filename, SeqSet &data) {

        ifstream input;
        std::istream * input_stream;
        input_stream = openSeqFile(filename, input);

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
                if(!input_stream->good())
                    throw("Problem reading file");
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

        input.close();
    }

    // This function should be placed within another function that
    // creates, opens, and closes the istream*, and also runs the
    // parsing function.
    // NEED TO #include <ctype.h>
    /*
    string guessFormat(string &filename, std::istream * is, string &format) {

        // If the format is already determined, skip guessing
        if(format.size() > 0) {
            return format;
        }

        // Make sure the stream is okay before reading
        if(!is->good()) {
            throw("File stream is in a bad state");
        }

        // Look at the first 1000 characters of the file to determine
        // format
        char buffer[1000];
        is->getline(buffer, 1000);
        char c;
        unsigned k = 0;
        while(k < 1000) {
            c = buffer[k];
            k++;
            if(c == '\n' || c == '\r' || c == ' ' || c == '\t')
                continue;
            if(c == '>') {
                format = "fasta";
                break;
            }
            if(c == '@') {
                format = "fastq";
                break;
            }
            if(toupper(c) == 'L' && k < 1000-4 &&
                    toupper(buffer[k+1]) == 'O' &&
                    toupper(buffer[k+2]) == 'C' &&
                    toupper(buffer[k+3]) == 'U' &&
                    toupper(buffer[k+4] == 'S') {
                    format = "genbank";
                    break;
            }
            if(toupper(c) == 'N' && k < 1000-4 &&
                    toupper(buffer[k+1]) == 'E' &&
                    toupper(buffer[k+2]) == 'X' &&
                    toupper(buffer[k+3]) == 'U' &&
                    toupper(buffer[k+4] == 'S') {
                    format = "nexus";
                    break;
            }
            // Other common formats:
            //  - phylip: could be recognized by having two numbers at
            //    the beginning, but sequential vs. interleaved is quite
            //    tricky.
            //  - fastg: will look like fasta, might be best to just have
            //    the user specify if I decide to support it
            //  - bam & sam: beyond the scope of this program; use
            //    samtools tview instead
            //  - plink & VCF: probably could be recognized (VCF is not
            //    too hard, at least), but also probably beyond the
            //    scope of this program
            //  - Clustal: does anyone use this? Just convert your
            //    sequences to fasta format!

        } // End format guessing loop

        / *
        // Put all the characters back on the stream for the parsing
        // function
        for(; k > 0; k--) {
            is->putback(buffer[k]);
            if(!is->good()) {
                throw("Problem resetting file stream");
            }
        } // End put back
        * /
        // Put back just the last character (all leading whitespace
        // trimmed off this way):
        is->putback(buffer[k]);

        // Throw an exception if the format was undetermined
        if(format.size() == 0)
            throw("Could not determine format from first 1000 characters");

        return format;
    }

    string guessFormat(string &filename, std::istream * is) {
        return guessFormat(filename, is, "");
    }
    */
}

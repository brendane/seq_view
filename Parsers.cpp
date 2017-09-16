#include<seq_view.h>

namespace SeqView {

    void parseSeqs(string filename, SeqSet &data) {
        ifstream input;
        std::istream * input_stream = openSeqFile(filename, input);
        if(!input_stream->good()) {
            throw("File stream is in a bad state");
        }
        //string format = guessFormat(input_stream);
        //if(format == "fasta") {
            //parseFasta(filename, input_stream); -> Modified to not open or close the file
        //}
        input.close();
    }

    std::istream * openSeqFile(string filename, ifstream &input) {
        std::istream * input_stream;
        if(filename == "-") {
            input_stream = &std::cin;
        } else {
            input.open(filename.c_str(), ifstream::in);
            input_stream = &input;
        }
        if(!input_stream->good()) {
            input.close();
            throw("Problem reading file");
        }
        return input_stream;
    }

    void parseFasta(string filename, SeqSet &data) {

        ifstream input;
        SeqStream input_stream(openSeqFile(filename, input));

        // This causes errors right now:
        //std::cerr << guessFormat(input_stream) << std::endl;

        data.filename = filename;

        char ch;
        string temp = "";
        string nm;
        unsigned size_guess = 10000; // Seems like it might speed things up

        // Enclose all of this in a while loop that goes to EOF:
        input_stream.get(ch);
        if(ch != '>') {
            input.close();
            throw("Not in FASTA format");
        }


        bool inseq = false;
        bool linebreak = false;
        while(!input_stream.eof()) {
            SeqRecord rec;
            rec.reserve(size_guess);
            nm = "";
            while (true && !inseq) {
                if(!input_stream.good()) {
                    input.close();
                    throw("Problem reading file");
                }
                input_stream.get(ch);
                if (ch == '\n' || ch == '\r')
                    inseq = true;
                nm += ch;
            }
            rec.setName(nm);

            temp = "";
            while(inseq){
                input_stream.get(ch);
                if(input_stream.eof())
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
            size_guess = rec.getSeq().size();
        }
        input.close();
    }

    // This function should be placed within another function that
    // creates, opens, and closes the istream*, and also runs the
    // parsing function.

    string guessFormat(std::istream * is) {
        string format = "";
        return guessFormat(is, format);
    }

    string guessFormat(std::istream * is, string &format) {

        // If the format is already determined, skip guessing
        if(format.size() > 0) {
            return format;
        }

        // Make sure the stream is okay before reading
        if(!is->good()) {
            std::cerr << "bad state!!!" << std::endl;
            throw("File stream is in a bad state");
        }
        std::streampos sp = is->tellg();

        // Look at the first 199 characters of the file to determine
        // format
        char buffer[200];
        // I want to just read 200 chars without checking for a delim;
        // seems unlikely that XXX will be in a sequence file
        is->getline(buffer, 199, '\n');
        int nread = std::char_traits<char>::length(buffer);
        char c;
        unsigned k = 0;
        while(k < 199 && k < nread) {
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
            if(toupper(c) == 'L' && k < 199-4 &&
                    toupper(buffer[k+1]) == 'O' &&
                    toupper(buffer[k+2]) == 'C' &&
                    toupper(buffer[k+3]) == 'U' &&
                    toupper(buffer[k+4]) == 'S') {
                    format = "genbank";
                    break;
            }
            if(toupper(c) == 'N' && k < 199-4 &&
                    toupper(buffer[k+1]) == 'E' &&
                    toupper(buffer[k+2]) == 'X' &&
                    toupper(buffer[k+3]) == 'U' &&
                    toupper(buffer[k+4]) == 'S') {
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

        // Put all the characters back on the stream for the parsing
        // function
        // This doesn't work because you often can't put more than
        // one character back.
        // Need a class that wrap ifstream with get, good, and eof methods
        // but will take characters from a buffer until it runs out and
        // then from the ifstream. It would read 200 characters into the
        // buffer when initialized.
        /*
        for(int i = nread-1; i > -1; i--) {
            is->putback(buffer[i]);
            if(is->fail()) {
                std::cerr << "problem resetting!!!" << std::endl;
                throw("Problem resetting file stream");
            }
        } // End put back
        */
        // This works for regular files, but not for stdin
        is->seekg(sp);
        if(!is->good()) {
            std::cerr << "problem resetting!!!" << std::endl;
            throw("Problem resetting file stream");
        }

        // Throw an exception if the format was undetermined
        if(format.size() == 0) {
            std::cerr << "format unknown!!!" << std::endl;
            throw("Could not determine format from first 199 characters");
        }

        return format;
    }

}

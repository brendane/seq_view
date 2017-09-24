#include<seq_view.h>

#include<unistd.h>

namespace SeqView {

    void parseSeqs(string filename, SeqSet &data) {
        ifstream input;
        SeqStream input_stream(openSeqFile(filename, input));
        if(!input_stream.good()) {
            throw("File stream is in a bad state");
        }
        string format = guessFormat(input_stream);
        if(format == "fasta") {
            try {
                parseFasta(input_stream, filename, data);
            } catch(...) {
                input.close();
                throw;
            }
        } else if(format == "fastq") {
            try {
                parseFastq(input_stream, filename, data);
            } catch(...) {
                input.close();
                throw;
            }
        } else {
            input.close();
            throw("Unrecognized format");
        }
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


    void parseFasta(SeqStream input_stream, string filename, SeqSet &data) {

        data.filename = filename;

        char ch;
        string temp = "";
        string nm;
        unsigned size_guess = 10000; // Seems like it might speed things up

        // Enclose all of this in a while loop that goes to EOF:
        input_stream.get(ch);
        if(ch != '>') {
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
    }




    void parseFastq(SeqStream input_stream, string filename, SeqSet &data) {

        data.filename = filename;
        char ch;
        string temp = "";
        string nm = "";
        unsigned size_guess = 150; // Seems like it might speed things up
        unsigned line_num = 0;
        bool linebreak = false;
        bool name = false;
        while(!input_stream.eof()) {
            // Check if stream is okay and read a character
            if(!input_stream.good()) {
                throw("Problem reading file");
            }
            input_stream.get(ch);

            // Check for linebreaks. Treat multiple linebreak characters
            // as one linebreak. Also, count the number of lines, and when
            // four lines have been reach, construct a SeqRecord and reset.
            if(ch == '\n' || ch == '\r') {
                if(!linebreak) {
                    line_num += 1;
                    if(line_num == 4) {
                        line_num = 0;
                        SeqRecord rec;
                        rec.setName(nm);
                        rec.append(temp);
                        data.append(rec);
                        size_guess = rec.getSeq().size();
                        nm = "";
                        temp = "";
                        temp.reserve(size_guess);
                        name = false;
                    }
                }
                linebreak = true;
                continue;
            }

            // If this far, not a linebreak
            linebreak = false;

            // For each line of the fastq file
            if(line_num == 0) {
                // Name
                if(!name and ch != '@') {
                    throw("Not in fastq format");
                }
                if(name) {
                    nm += ch;
                }
                name = true;
            } else if(line_num == 1) {
                // Sequence
                temp += ch;
            } else if(line_num == 2) {
                // Plus line - Ignore
                continue;
            } else if(line_num == 3) {
                // Quality scores - ignore
                continue;
            }
        }
    }

    // This function should be placed within another function that
    // creates, opens, and closes the istream*, and also runs the
    // parsing function.

    string guessFormat(SeqStream s) {
        string format = "";
        return guessFormat(s, format);
    }

    string guessFormat(SeqStream s, string &format) {

        // If the format is already determined, skip guessing
        if(format.size() > 0) {
            return format;
        }

        // Make sure the stream is okay before reading
        if(!s.good()) {
            std::cerr << "bad state!!!" << std::endl;
            throw("File stream is in a bad state");
        }

        // Look at the first 150 characters of the file to determine
        // format
        char buffer[150];
        unsigned k = 0;
        unsigned nread = 0;
        char c;
        while(k < 150 && s.good() && !s.eof()) {
            s.get(c);
            buffer[k] = c;
            k++;
            nread++;
        }

        k = 0;
        while(k < 150 && k < nread) {
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
            if(toupper(c) == 'L' && k < nread-4 &&
                    toupper(buffer[k+1]) == 'O' &&
                    toupper(buffer[k+2]) == 'C' &&
                    toupper(buffer[k+3]) == 'U' &&
                    toupper(buffer[k+4]) == 'S') {
                format = "genbank";
                break;
            }
            if(toupper(c) == 'N' && k < nread-4 &&
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

        if(!s.start_over()) {
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

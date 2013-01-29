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

    void parseFasta(string filename, SeqSet &data) {

        //try {
            ifstream input(filename.c_str(), ifstream::in);

            data.filename = filename;

            char ch;
            string temp = "";
            string nm;

            // Enclose all of this in a while loop that goes to EOF:
            input.get(ch);
            if(ch != '>') {
                throw("Not in FASTA format");
            }


            bool inseq = false;
            bool linebreak = false;
            while(!input.eof()) {
                SeqRecord rec;
                nm = "";
                while (true && !inseq) {
                    input.get(ch);
                    if (ch == '\n' || ch == '\r')
                        inseq = true;
                    nm += ch;
                }
                rec.setName(nm);

                temp = "";
                while(inseq){
                    input.get(ch);
                    if(input.eof())
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
    }
}

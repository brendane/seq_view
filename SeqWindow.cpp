#include<seq_view.h>

namespace SeqView {

    void SeqWindow::_scroll(int64_t newleft, int64_t newtop) {
        first_seq = newtop;
        first_pos = newleft;
        modified = true;
        display();
    }

    // Given the current window size and the width
    // of the names, figure out how many sequences and positions
    // should be displayed. Also recalculate last_pos and last_seq.
    void SeqWindow::_recalculate_num_displayed() {
        // Need to reserve three lines - two for positions and one
        // for the filename
        // Need to reserve names_width + 1 columns for names
        num_pos_displayed = width - names_width - 1;
        num_seqs_displayed = height - 3;
        last_pos = first_pos + num_pos_displayed - 1;
        last_seq = first_seq + num_seqs_displayed - 1;
        modified = true;
    }

    void SeqWindow::_display_names() {
        if(isfocal)
            wattron(window, A_BOLD);
        wattron(window, COLOR_PAIR(7));
        std::vector<string> names = seqs.nameSlice(first_seq, last_seq);
        for(int i = 0; i < num_seqs_displayed - 1; i++) {
            mvwprintw(window, i+2, 0, string(names_width + 1, ' ').c_str());
            if(names_width > names[i].length())
                mvwprintw(window, i+2, 0, names[i].c_str());
            else
                mvwprintw(window, i + 2, 0,
                        names[i].substr(0, names_width).c_str());
        }
        if(isfocal)
            wattroff(window, A_BOLD);
        wattroff(window, COLOR_PAIR(7));
    }

    void SeqWindow::_display_positions() {
        if(isfocal)
            wattron(window, A_BOLD);
        wattron(window, COLOR_PAIR(7));
        for(int i = 0; i < width; i++) {
            mvwprintw(window, 0, i, " ");
            mvwprintw(window, 1, i, " ");
        }
        int col = names_width + 1;
        int frame = seqs.get_frame();
        for(int i = first_pos + 1; i < last_pos + 2; i++) {
            if(!(i % 10)) {
                mvwprintw(window, 1, col, "|");
                mvwprintw(window, 0, col, "%i", i);
            } else if(!(i % 5)) {
                mvwprintw(window, 1, col, ":");
            } else {
                mvwprintw(window, 1, col, ".");
            }
            if(display_mode == CODON) {
                if((i - frame + 1) % 3 == 0) {
                    col++;
                    mvwprintw(window, 1, col, " ");
                }
            }
            if(display_mode == TEN) {
                if(i % 10 == 0) {
                    col++;
                    mvwprintw(window, 1, col, " ");
                }
            }
            col++;
            if(col >= width)
                break;
        }
        // Sometimes the numbers spill over to the next line - this
        // gets rid of that.
        mvwprintw(window, 1, 0, string(names_width, ' ').c_str());
        if(isfocal)
            wattroff(window, A_BOLD);
        wattroff(window, COLOR_PAIR(7));
    }

    // TODO: Add code for different ways of comparing - just need to
    // pass the comparison mode to SeqSet::slice.
    void SeqWindow::_display_seqs() {
        // This function is more tricky - will require some added
        // arguments for formatting eventually.
        // Also may require some adjustment to color amino acid
        // sequences properly.
        std::pair< std::vector<string>, std::vector<bool> > 
            s= seqs.slice(first_pos, last_pos, first_seq, last_seq,
                    display_mode, compare);
        std::vector<string> sequences = s.first;
        std::vector<bool> comps = s.second;
        int row = 2;
        char ch;
        int col;
        if(bolded)
            wattron(window, A_BOLD);
        for(int i = 0; i < num_seqs_displayed - 1; i++) {
            for(int j = 0; j < sequences[i].length(); j++) {
                ch = sequences[i][j];
                if(compare && comps[j] && (i + first_seq) < seqs.numseqs() && ch != ' ')
                    wattron(window, A_REVERSE);
                ColMapIt cit = dna_colors.find(ch);
                if(cit != dna_colors.end()) {
                    col = cit->second;
                } else {
                    col = dna_colors.find('*')->second; // Don't know why dna_colors['*'] doesn't work
                }
                wattron(window, COLOR_PAIR(col));
                mvwaddch(window, row, names_width + 1 + j, ch);
                wattroff(window, COLOR_PAIR(col));
                if(compare && comps[j])
                    wattroff(window, A_REVERSE);
            }
            row++;
        }
        if(bolded)
            wattroff(window, A_BOLD);
    }

    void SeqWindow::_display_filename() {
        // display at bottom
        wattron(window, COLOR_PAIR(7) | A_UNDERLINE);
        if(isfocal)
            wattron(window, A_REVERSE | A_BOLD);
        mvwprintw(window, height - 2, 0, seqs.filename.c_str());
        for(int i = seqs.filename.length() + 1; i <= width; i++)
            mvwprintw(window, height - 2, i - 1, " ");
        if(isfocal)
            wattroff(window, A_REVERSE | A_BOLD);
        wattroff(window, COLOR_PAIR(7) | A_UNDERLINE);
    }


    // TODO: Comparison mode, not just TRUE or FALSE
    SeqWindow::SeqWindow() {
        modified = true;
        compare = false;
        bolded = false;
    }

    // TODO: Comparison mode, not just true or false
    SeqWindow::SeqWindow(int upperleftX, int upperleftY, 
            int _width, int _height, SeqSet &sq) {
        window = newwin(_height, _width, upperleftY, upperleftX);
        width = _width;
        height = _height;
        seqs = sq;
        scrollmode = 1;
        display_mode = NORMAL;
        names_width = 15;
        first_pos = 0;
        first_seq = 0;
        update_size();
        _recalculate_num_displayed();
        isfocal = true;
        bolded = false;
        modified = true;
        compare = false;
        display();
    }

    // TODO: Comparison mode, not just true or false
    SeqWindow::SeqWindow(int upperleftX, int upperleftY, 
            int _width, int _height, string filename,
            ParserFunction parser) {
        SeqSet sq;
        parser(filename, sq);
        window = newwin(_height, _width, upperleftY, upperleftX);
        width = _width + 1;
        height = _height + 1;
        seqs = sq;
        scrollmode = 1;
        display_mode = NORMAL;
        names_width = 15;
        first_pos = 0;
        first_seq = 0;
        update_size();
        _recalculate_num_displayed();
        bolded = false;
        modified = true;
        compare = false;
        display();
    }

    SeqWindow::~SeqWindow() {
        wattron(window, COLOR_PAIR(8));
        for(int i = 0; i < height; i++) {
            mvwprintw(window, i, 0, string(width, ' ').c_str());
        }
        wattroff(window, COLOR_PAIR(8));
        wrefresh(window);
        delwin(window);
    }

    void SeqWindow::set_focus(bool focal) {
        isfocal = focal;
        modified = true;
    }

    // scroll mode is in powers of 10
    void SeqWindow::set_scroll_mode(int mode) {
        if(mode < 0 || mode > 9)
            return;
        scrollmode = pow(10, mode);
    }

    void SeqWindow::update_size() {
        getmaxyx(window, height, width);
        height += 1;
        width += 1;
    }

    void SeqWindow::change_name_width(int newwidth) {
        if(newwidth < width - 5  && newwidth > 1) {
            names_width = newwidth;
            update_size();
            _recalculate_num_displayed();
        }
    }

    // Deal with commands that change SeqSet params
    //
    // TODO: handle comparison modes by checking the "param"
    void SeqWindow::handle_command(Command command) {
        Com com_name = command.first;
        int param = command.second;
        int newpos;
        if(com_name == SCROLLUP) {
            if(first_seq > 0) {
                newpos = first_seq - param;
                if(newpos < 0)
                    newpos = 0;
                _scroll(first_pos, newpos);
            }
        } else if(com_name == SCROLLDOWN) {
            if(first_seq < seqs.numseqs()) {
                newpos = first_seq + param;
                if(newpos >= seqs.numseqs())
                    newpos = seqs.numseqs() - 1;
                _scroll(first_pos, newpos);
            }
        } else if(com_name == SCROLLLEFT) {
            if(first_pos > 0) {
                newpos = first_pos - scrollmode * param;
                if(newpos < 0)
                    newpos = 0;
                _scroll(newpos, first_seq);
            }
        } else if(com_name == SCROLLRIGHT) {
            if(first_pos < seqs.length()) {
                newpos = first_pos + scrollmode * param;
                if(newpos >= seqs.length())
                    newpos = seqs.length() - 1;
                _scroll(newpos, first_seq);
            }
        } else if(com_name == SCROLLTOP) {
            _scroll(first_pos, 0);
        } else if(com_name == SCROLLBOTTOM) {
            _scroll(first_pos, seqs.numseqs() - 1);
        } else if(com_name == GOTOBEGIN) {
            _scroll(0, first_seq);
        } else if(com_name == GOTOEND) {
            _scroll(seqs.length() - 1, first_seq);
        } else if(com_name == GOTO) {
            // Do I need to add something here for display_mode == TEN ?
            newpos = param - 1 - (width - names_width) / 2;
            if(display_mode == CODON)
                newpos += (width - names_width) / 6;
            if(newpos < 0)
                newpos = 0;
            if(newpos > seqs.length() - 1)
                newpos = seqs.length() - 1;
            _scroll(newpos, first_seq);
        } else if(com_name == SCROLLMODE) {
            set_scroll_mode(param);
        } else if(com_name == NAMEWIDTH) {
            if(param > 0) {
                change_name_width(param);
            } else if(param == -1) {
                change_name_width(names_width-1);
            } else if(param == 0) {
                change_name_width(names_width+1);
            }
        } else if(com_name == DISPLAYMODE) {
            if(param == 1) {
                display_mode = NORMAL;
            } else if(param == 2) {
                display_mode = CODON;
            } else if(param == 3) {
                display_mode = TEN;
            }
            modified = true;
        } else if(com_name == SETFRAME) {
            seqs.set_frame(param);
            modified = true;
        } else if(com_name == COMPARE) {
            compare = !compare;
            modified = true;
        } else if(com_name == TOGGLEBOLD) {
            bolded = !bolded;
            modified = true;
        } else if(com_name == RESIZE) {
            modified = true;
        }
    }

    void SeqWindow::resize(int upperleftX, int upperleftY,
            int newwidth, int newheight) {
        wattron(window, COLOR_PAIR(8));
        for(int i = 0; i < height; i++) {
            mvwprintw(window, i, 0, string(width, ' ').c_str());
        }
        wrefresh(window);
        delwin(window);
        window = newwin(newheight, newwidth,
                upperleftY, upperleftX);
        height = newheight;
        width = newwidth;
        update_size();
        _recalculate_num_displayed();
        modified = true;
        display();
    }

    // Refresh, get the slice of the sequence, add the formatting,
    // and use wprintw to put everything on the screen.
    void SeqWindow::display() {
        int w = width;
        int h = height;
        update_size();
        if(w != width || h != height)
            modified = true;
        if(modified) {
            _recalculate_num_displayed();
            _display_names();
            _display_positions();
            _display_seqs();
            _display_filename();
            wrefresh(window);
        } else {
        }
    }
}

#include<seq_view.h>

namespace SeqView {

    set<int> seqSetCommands(ssc, ssc + 16);

    Com ssc[] = {SCROLLUP, SCROLLDOWN, SCROLLRIGHT, SCROLLLEFT,
        SCROLLMODE, COMPAREMODE, GOTO, GOTOEND,
        COMPARETOGGLE, GOTOBEGIN, SCROLLTOP,
        SCROLLBOTTOM, NAMEWIDTH, DISPLAYMODE, SETFRAME,
        COMPARE};

    Command getCommand() {
        // Get the command followed by the parameters

        int param = 1;
        string param_buffer = "";
        string command_buffer = "";

        bool on_param = true;
        bool on_command = false;

        // need a map/tree that matches commands to characters
        while(TRUE) {
            char ch = getch();

            // ESC key clears the buffers
            if(ch == 27) {
                param = 1;
                param_buffer = "";
                command_buffer = "";
                on_param = true;
                on_command = false;
                continue;
            }

            // if a non-number was pushed, convert the 
            // param_buffer to int and look for the command
            if(ch > 57 || ch < 48) {
                on_command = true;
                on_param = false;
                if(param_buffer.length())
                    param = atoi(param_buffer.c_str());
            }

            // Start by looking for numbers
            if(on_param) {
                param_buffer += ch;
            }

            // if we are looking for a command
            if(on_command) {
                command_buffer += ch;
                if(!command_buffer.compare("q"))
                    return Command(QUIT, param);
                if(!command_buffer.compare("s"))
                    return Command(SCROLLMODE, param);
                if(!command_buffer.compare("j"))
                    return Command(SCROLLDOWN, param);
                if(!command_buffer.compare("J"))
                    return Command(SCROLLBOTTOM, param);
                if(!command_buffer.compare("k"))
                    return Command(SCROLLUP, param);
                if(!command_buffer.compare("K"))
                    return Command(SCROLLTOP, param);
                if(!command_buffer.compare("h"))
                    return Command(SCROLLLEFT, param);
                if(!command_buffer.compare("l"))
                    return Command(SCROLLRIGHT, param);
                if(!command_buffer.compare("H"))
                    return Command(SHOWHELP, param);
                if(!command_buffer.compare("g"))
                    return Command(GOTOBEGIN, param);
                if(!command_buffer.compare("w")) {
                    if(param_buffer.length() == 0) {
                        param = 0;
                    }
                    return Command(CHANGEFOCUS, param);
                }
                if(!command_buffer.compare("G")) {
                    if(param_buffer.length()) {
                        return Command(GOTO, param);
                    } else {
                        return Command(GOTOEND, param);
                    }
                }
                if(!command_buffer.compare("d"))
                    return Command(DISPLAYMODE, param);
                if(!command_buffer.compare("f"))
                    return Command(SETFRAME, param);
                if(!command_buffer.compare("n"))
                    return Command(NAMEWIDTH, param);
                if(!command_buffer.compare(";"))
                    return Command(SPECIAL, param);
                if(!command_buffer.compare("c"))
                    return Command(COMPARE, param);

                // if no matches, clear the buffers
                param_buffer = "";
                command_buffer = "";
                on_command = false;
                on_param = true;
                param = 1;
            }

        }
    }

}
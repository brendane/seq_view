#seq_view

Program to display molecular sequence files using ncurses. Meant to be
an intermediate step between the Unix command line tools and graphical
sequence editing programs.

Rewritten in C++.

##Installation

==================================================

Requires the ncurses library to be installed. This means that seq_view
will probably only work on UNIX-based systems.

To compile:

cd seq_view

g++ -o seq_view *.cpp -I"." -lncurses

Note that some systems may have ncurses installed as "curses." In that
case compile with:

g++ -o seq_view *.cpp -I"." -lcurses


##Use

==================================================

seq_view FASTA_FILE [FILE, FILE, ...]

At present, seq_view can only read FASTA files (more file formats may be
added in the future).

Once the file is open, a set of keyboard command, similar to vim's
commands can be used to move through the data and control the
appearance. To cancel any command type ESC.

####KEY                 ACTION

#####General

q                   Close current window. When last window is closed,
                    the program quits.

w                   Cycle through windows

[number]w           Move to window *number* (1 based)


#####Scrolling

All scrolling commands can be proceeded by a number that is multiplied
by the current base scroll amount to get the number of positions to
move.

[number]s           Set base scroll amount to 10^[number]

[number]h           Move [number] units to the left

[number]l           Move [number] units to the right

[number]j           Move [number] units down

[number]k           Move [number] units up

K                   Move to the top

J                   Move to the bottom

G                   Move to the end

g                   Move to the beginning

[number]G           Move to position [number]


#####Appearance

c                   Turn on and off comparison highlighting. When
                    comparison highlighting is on, sites that are all
                    the same are highlighted. See section on comparison
                    below.

[number]d           Display mode. Right now has 1 for normal and 2 for
                    codon. See display mode section below.

[number]n           Adjust the number of characters of the names
                    displayed to be [number].

[number]f           Adjust the frame for codon display mode. Should be
                    1, 2, or 3.


#####Special commands

These commands are entered by typing ";". There are just a few of them
right now:

"open":             Type ";open FILENAME" to open FILENAME.

"mode":             Type ";mode MODE" to switch display mode
                    ("normal" or "codon" right now).


### Comparison modes

TODO...


### Display modes

* *normal*    Displays sequences with one column per base
* *codon*     Puts a space between every codon (set the frame
            with 'f' key command).

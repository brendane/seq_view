# seq_view

Program to display molecular sequence files using ncurses and a vim-like
user interface. Meant to be an intermediate step between the Unix
command line tools and graphical sequence editing programs. I find it
useful when I'm logged into a server remotely and I want to look at some
fasta files. It's a bit like samtools tview, but for sequence files
(currently just FASTA and FASTQ format) instead of read alignments.

### Screenshot:
A single fasta file in codon mode:
![](screenshot.png)

Two files displayed at once, showing comparison mode:
![](two-files.png)

## Installation

==================================================

Requires the ncurses library to be installed. This means that seq_view
will probably only work on UNIX-based systems.

To compile:

cd seq_view

g++ -o seq_view *.cpp -I"." -lncurses

Note that some systems may have ncurses installed as "curses." In that
case compile with:

g++ -o seq_view *.cpp -I"." -lcurses


## Use

==================================================

seq_view FILE [FILE, FILE, ...]

At present, seq_view can only read FASTA and FASTQ files (more file
formats may be added in the future). Use "-" to read from stdin.

Once the file is open, a set of keyboard commands, similar to vim's
commands can be used to move through the data and control the
appearance. To cancel any command type ESC. To quit use Ctrl-C or type
'q'.

#### KEY                 ACTION

##### General

**q**                   Close current window. When last window is closed,
                        the program quits.

**aq**                  Close all windows and quit.

**w**                   Cycle through windows
**W**                   Cycle through windows backwards

**[number]w**           Move to window *number* (1 based)


##### Scrolling

All scrolling commands can be proceeded by a number that is multiplied
by the current base scroll amount to get the number of positions to
move.

**[number]s**           Set base scroll amount to 10^[number]

**[number]h**           Move [number] units to the left (can also use arrow keys)

**[number]l**           Move [number] units to the right (can also use arrow keys)

**[number]j**           Move [number] units down (can also use arrow keys)

**[number]k**           Move [number] units up (can also use arrow keys)

**K**                   Move to the top

**J**                   Move to the bottom

**G**                   Move to the end
**[end]**               

**g**                   Move to the beginning
**[home]**

**[number]G**           Move to position [number]


##### Appearance

**c**                   Turn on and off comparison highlighting. When
                        comparison highlighting is on, sites that are all
                        the same are highlighted. See section on comparison
                        below.

**[number]d**           Display mode. Right now has 1 for normal, 2 for
                        codon, and three for ten. See display mode section
                        below.

**[number]n**           Adjust the number of characters of the names
                        displayed to be [number].

**[shift][left/right]** Increase or decrease the number of characters of
                        the names displayed. 

**[number]f**           Adjust the frame for codon display mode. Should be
                        1, 2, or 3.


##### Special commands

These commands are entered by typing ";", similar to how vim commands
are entered by typing ":". There are just a few of them right now:

"open":             Type ";open FILENAME" to open FILENAME.

"mode":             Type ";mode MODE" to switch display mode
                    ("normal", "codon", or "ten" right now).

"compare-mode":     Type ";compare-mode MODE" to switch between
                    different ways of determining whether sites vary.
                    See Comparison modes section below.

"bold":             Toggle bolded sequence display. (Not working yet.)


### Comparison modes

"plain":            Tests only whether all sequences have the same state,
                    ignoring case.

"nucamb":           Tests whether all sequences *could* be the same,
                    taking into account IUPAC ambiguity codes and
                    counting gaps ("-") as matching anything. For
                    nucleotide sequences.


### Display modes

* *normal*    Displays sequences with one column per base
* *codon*     Puts a space between every codon (set the frame
              with 'f' key command).
* *ten*       Puts a space every 10 bases

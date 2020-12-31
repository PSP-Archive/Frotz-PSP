Frotz-PSP

A PSP port of the Frotz Z-Machine interpreter by Stefan Jokisch Et. El. (see
doc/AUTHORS for a complete list).

Port by Norman B. Lancaster (QBRADQ)

INSTALLATION

1. Copy the PSP directory onto the root of your memory stick. This will create
   a PSP/GAME/frotz-psp directory containing the executable.
2. Place story files somewhere on your PSP memory stick. For recent community
   works this will be the .z3 or .z5 file provided by the author. For
   commercial games this will typically be a .DAT file (such as ZORK1.DAT).

STARTING A GAME

1. Start the application as normal for your firmware.
2. When the application starts it will open a file selection dialog allowing
   you to choose the story file you want to play.
    a. If you prefer to auto-load a story file, place said story file on the
       root of your memory stick and name it "story.file". This will be
       automatically loaded and the file select dialog will be skipped.

CONTROLS

Mode Select
The analog stick selects the input mode.

[down]           Key Box input mode
[right]          Quick Words input mode
[up]             Screen Buffer input mode

Common Controls
[square]         Insert a space into the input line
[circle]         Delete a character from the input line
[R+Left / Right] Move the input caret position
[R+Up / Down]    Cycle through the input history list
[L OR start]     End the input line (like pressing Enter)
[select]         Built-in text reader (PSPEText V1.1-RC2)

Key Box Mode
[d-pad]          Select a letter
[cross]          Add the selected letter to the input line
[triangle]       Add the entire input line to your Quick Words list

Quick Words Mode
[d-pad]          Select a Quick Word
[cross]          Add the selected Quick Word to the input line
[triangle]       Remote the selected Quick Word from your list

Screen Buffer Mode
[d-pad]          Select a word on the screen
[cross]          Add the selected word to the input line
[triangle]       Add the selected word to your Quick Words list

HOW TO PLAY
1. Read http://www.microheaven.com/IFGuide/
    a. If you don’t have Internet access at the moment, read the
       doc/HOW_TO_PLAY document for pointers. This is not the same document
       as above, but does provide a decent overview.
2. Saving the game is done by saying "SAVE".
3. Loading a saved game is done by saying "RESTORE".

WHERE TO FIND INTERACTIVE FICTION GAMES
1. The Zork trilogy has been released as freeware, and can be downloaded
   legaly from the rights holder here:
   http://www.infocom-if.org/downloads/downloads.html . Just copy the
   DATA/ZORK*.DAT files to your PSP.
2. The Interactive Fiction Archive ( http://www.ifarchive.org ) hosts just
   about every community-written IF there is.
    a. All English-language games for Z-Machine:
       http://www.ifarchive.org/indexes/if-archiveXgamesXzcode.html
    b. Colossal Cave Adventure, 350-Point Version:
       http://www.ifarchive.org/if-archive/games/zcode/Advent.z5
    c. Curses: http://www.ifarchive.org/if-archive/games/zcode/curses.z5
    d. Note: Only z1 through z5 files are supported by Frotz.

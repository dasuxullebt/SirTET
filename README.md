SirTET
======

SirTET aims to become a "reverse Tetris". It has two interfaces: A
terminal interface, using
[LibLanterna](https://code.google.com/p/lanterna/downloads/detail?name=lanterna-2.1.7.jar),
and a Braille-Interface, using [BrlAPI](http://mielke.cc/brltty/). It
is at a very early stage of development - there is only one Level, and
it likely contains bugs.

To run it under Ubuntu 14.04 (and probably other Debian-derivatives), the Packages default-jre and
libbrlapi-java are required to run it. Furthermore, [Lanterna
(2.1.7)](https://code.google.com/p/lanterna/downloads/detail?name=lanterna-2.1.7.jar)
is required.

Assuming BrlAPI-Java is installed in /usr/share/java/brlapi.jar, and
lanterna-2.1.7.jar is in the current directory of SirTET.java, you can
compile it with

    javac -cp /usr/share/java/brlapi.jar:lanterna-2.1.7.jar:. SirTET.java

You can run it with

    java -Djava.awt.headless=true -cp /usr/share/java/brlapi.jar:lanterna-2.1.7.jar:. SirTET

If it complains that it cannot find the dynamic library, search for
libbrlapi_java.so, and set -Djava.library.path= to its directory.

The Playfield is 10 lines times 20 columns, but different from
original Tetris, the direction of the stones goes from right to left
(to get the corresponding Tetris-Playfield, you would have to rotate
the field 90° counterclockwise). This is done to make it easier to
access on a Braille line. A Braille line can show 4 lines of dots at
once. Thus, the Playfield contains 3 Braille-lines, the last line only
uses the upper two dot-lines.

A stone blinks slowly, if it fits, and it will be at the position
where it fits. A stone blinks quickly in the center of the playfield,
if it does not fit.

With the up/down-Arrow-Keys, you can move the current stone up an
down. With space, you can rotate it. With Tab, you can switch to
another stone. Notice that the 8th "stone" to choose is actually a
full column: As the game is a reverse Tetris, you can introduce such
columns under certain conditions, since complete rows in Tetris
disappear. The column can be moved with the
left/right-Arrow-Keys. With Enter, you remove a stone or place a
column, if that is possible. With Backspace, you can undo your last
move.

You can end the program using the Escape-Key.

===============
Laser projector
===============

This program is intended to run a laser projector (like those used in music show) to expose photographic processes like cyanotype, dichromate gum, etc.
This laser projector features an arduino board, a power supply, two galvos and their drivers (X and Y), a laser and its driver.
It also features a C++ program (that was initialy writtent in python, but was too slow) that opens files, transform it into formatted data strings and serial-stream it to the arduino board

It's at the very beggining of its developpement, so for now the architecture can move, and some functions are not implemented yet.
It's mostly based on Qt.

Here is how the C++ side should be:
* The program gives an GUI that permit to open images, set up parameters and comput them
* It features a serial link that can interface the arduino
* It can read images of different formats
* It keeps in memory the configuration of the connexion
* It should (not implemented yet) be able to keep in memory settings for a given format (e.g. 28x24cm, speed and distance given)
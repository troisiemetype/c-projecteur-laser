===============
Laser projector
===============

This program is intended to run a laser projector (like those used in music show) to expose photographic processes like cyanotype, dichromate gum, etc.
This kind of laser projector can be easily find on Internet, ready to use or made from scratch, from a pair of galvos mirrors, their drivers and power supply, and a laser module.

CAUTION: laser can blind you definitively in a glimpse. Be careful when using them, wear safety glasses, don't pass inthe beam area. That said, let's talk of what it is about.

This laser projector is mainly based on a program running on a computer, that process an image so as the laser can project it.
From beginning the project was based on an Arduino board (first Uno, then Due), wich role was to receive positions and laser intensities from the computer, and send it to the laser galvos. It appears while developping that Arduino, both Uno and Due, were far too slow to drive the gavlos efficiently. As the gavlos are driven from a tension going from -5 to 5V, they are nok drive by the sound board of the computer the program is running on. With at least 44100Hz and 16 or 24 bits precision, this is far better.

The program is mainly composed of these parts:
* A GUI that enables to open and view images, set up the system.
* An Image object that stores the image that's been openned, compute negative and thumbnail, etc.
* A compute module that execute trigonometric computation to convert the image and its pixel values to position coordinates for the laser. This generates scanning lines for the whole image, with a defined angle, and can handle other options.
* An audio module, that populate a buffer using data from the compute module, and can play it when needed. It can also save the datastream generated to a classic audio file, so the image can be projected again without the program.

For now it can do basic things:
* Open an image
* Set its projection size, exposure ratio, loop into several expositions, print one line out of two, three, etc.
* Project it with wanted settings.
* Save the resulting data stream to a wave file.
* Project a rectangle of given size, to place the support receiving image at the right place in front of the laser.
* Project a sight, to enable galvos setting up.
* Remember some settings (for know, just remember window position on the screen).

More to come (ideas, maybe they will be implemented, maybe not):
* Project the image randomly, pixel by pixel and/or by splitting it into several squares.
* Create masks on the image surface, with variables shapes, each mask having its own settings.
* Save project (image, settings). For now the image is saved as a wave file that can be used stand-alone, but it cannot be re-openend and modified.
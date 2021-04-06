# bootleg wendypaint

![title img](https://github.com/mccap079/wpainter/blob/master/bin/data/paintings/title.png?raw=true)

---

Wendypaint.com clone as a windows and macos native app. Not including a few features. Credit for the underlying concept goes to Kim Asendorf, creator of the now defunct wendypaint.com, who has nothing to do with this project otherwise. This has been built from scratch (using openframeworks) to match the original functionality.

I was an avid wendypaint.com participant, and particularly appreciated its embrace of the pixel in its visual style instead of attempting to antialias and skeuomorph the harsh grid of the lcd screen out of sight. So I enjoyed being able to effortlessly create my own paintings in this sandbox that captured this sensibility in a compelling way. Like every creators platform its time has come to die, bringing all of its users' uploaded content along with it, so I figured I'd just rebuild it so I (and maybe anyone else) can continue making cutesy little pixellated paintings forevermore.

Todo
:
 - Change canvas size
 - Better painting loader
 - Autofill functions for brush canvas
 - Bleed fx

## Installation

### macOS

Download the macOS release from Releases page, unzip and launch the bootlegWendypaint/wpainter.app.

NOTE:
 - You'll probably need to right-click > Open then click thru the warning dialog because I'm an unidentified developer.
 - Due to ["app translocation"](https://lapcatsoftware.com/articles/app-translocation.html) you may notice that no brushes have been loaded. If this is the case, quit the app, drag wpainter.app out of the bootlegWendypaint folder, drag it back in, then relaunch the app. Once you do this the issue should not happen again.

### Windows

Release TBD

Download the Windows release from the Releases page, unzip and launch the bootlegWendypaint/wpainter.exe.

### Loading brush packs

To load a new set of brushes, copy the pngs from one of the collections in the brushPacks folder and paste them into the data/brushes/ folder. The png filenames have to be numbered 0-29 and 25x25 pixels or they either won't load or crash the app altogether.


# bootleg wendypaint

![title img](https://github.com/mccap079/wpainter/blob/master/title.png?raw=true)

---

Wendypaint.com clone as a windows and macos native app. Not including a few features. Credit for the underlying idea goes to Kim Asendorf, creator of the now defunct wendypaint.com, who has nothing to do with this project otherwise. This has been built from scratch (using openframeworks) to match the original functionality, plus a few convenience features.

Todo:
 - Autofill functions for brush canvas
 - Bleed fx

## Installation

### macOS

Download the macOS release from Releases page, unzip and launch the bootlegWendypaint/wpainter.app.

NOTE:
 - You may need to right-click > Open then click thru the warning dialog because I'm an unidentified developer.
 - Due to ["app translocation"](https://lapcatsoftware.com/articles/app-translocation.html) you may notice that no brushes have been loaded. If this is the case, quit the app, drag wpainter.app out of the bootlegWendypaint folder, drag it back in, then relaunch the app.

### Windows

Release TBD

Download the Windows release from the Releases page, unzip and launch the bootlegWendypaint/wpainter.exe.

## Usage

### Loading brush packs

To load a new set of brushes, copy the pngs from one of the collections in the brushPacks folder and paste them into the data/brushes/ folder. The png filenames have to be numbered 0-29 and be 24x24 pixels or they either won't load or crash the app altogether.


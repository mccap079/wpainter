# bootleg wendypaint

![title img](https://github.com/mccap079/wpainter/blob/master/title.png?raw=true)

---

Wendypaint.com clone as a windows and macos native app. Not including a few features. Credit for the underlying idea goes to Kim Asendorf, creator of the now defunct wendypaint.com, who has nothing to do with this project otherwise. This has been built from scratch (using openframeworks) to match the original functionality, plus a few convenience features.

Todo:
 - Autofill functions for brush canvas
 - Scroll window for brush collection
 - Bitmap font loader

## Installation

### macOS

Download the `v1.0` macOS release from the Releases page, unzip and launch bootlegWendypaint/wpainter.app.

NOTE:
 - You may need to right-click > Open then click thru the warning dialog because I'm an unidentified developer.
 - Due to ["app translocation"](https://lapcatsoftware.com/articles/app-translocation.html) you may notice that no brushes have been loaded. If this is the case, quit the app, drag wpainter.app out of the bootlegWendypaint folder, drag it back in, then relaunch the app.

### Windows

Download the `v1.1` Windows release from the Releases page, unzip and launch bin/wPainter.exe.

## Usage

### User interface

 - Brush canvas: smaller canvas, used to create brushes that are stored in the brush collection and which can be used as the paint cursor on the paint canvas
 - Paint canvas: larger canvas, this is your actual painting
 - Brush collection: the grid of squares where created brushes (including brushes loaded from the `data/brushes/` folder) are stored
 - Info bar: the text ticker along the top of the window that gives you info about what you're doing when you do things, like a tooltip
 - Brush settings menu: the menu entitled "BRUSH SETTINGS" that serves options related to the brush canvas
 - Canvas settings menu: the menu entitled "CANVAS SETTINGS" that serves options related to the paint canvas

### How to create a brush

1. Select an empty slot in the brush collection, or select an existing brush to overwrite.
2. Use the color options on the brush settings menu to select a color.
3. Click and drag on the brush canvas to create a brush.
4. Use the eraser toggle on the brush settings menu to erase pixels on the brush canvas. Empty pixels are transparent.
5. Save the brush to add it to the brush collection.

### How to create a painting

1. Use the Set size setting on the canvas settings menu to set the dimensions of the paint canvas.
2. Use the fill canvas color setting on the canvas settings menu to set a canvas background color. By default the background it transparent.
3. Select a brush from the brush collection.
4. Select the brush's anchor point to determine how you'd like the brush image positioned under your cursor as you paint.
5. Click and drag around on the paint canvas to paint.
6. Export the painting using the canvas settings menu to save it to file.
7. Import a painting to load a new painting from file. The import/export features can be used like an "undo" function, to revert back to previous states of a painting if a mistake is made, so long as you save (export) often.
8. Use the clear canvas button on the canvas settings menu to start over.

### Loading brush packs

To load a new set of brushes, copy the pngs from one of the collections in the brushPacks folder and paste them into the `data/brushes/` folder. The png filenames have to be numbered 0-29 and be 24x24 pixels or they either won't load or crash the app altogether.


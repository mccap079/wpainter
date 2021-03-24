#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup() {
	ofDisableAntiAliasing();
	ofLogToConsole();
	ofSetFrameRate(60);

	mainCanvasPos = { windowMargin + (mainCanvasSize.x / 2),
						status.getHeight() + (windowMargin * 2) + (mainCanvasSize.y / 2) };

	mainCanvasRect.set(mainCanvasPos.x - (mainCanvasSize.x / 2),
		mainCanvasPos.y - (mainCanvasSize.y / 2),
		mainCanvasSize.x,
		mainCanvasSize.y);

	brushCanvasPos = { windowMargin + (brushCanvasDisplaySize.y / 2),
						 mainCanvasRect.getBottom() + windowMargin + (brushCanvasDisplaySize.y / 2) };

	brushCanvasRect.set(brushCanvasPos.x - (brushCanvasDisplaySize.x / 2),
		brushCanvasPos.y - (brushCanvasDisplaySize.y / 2),
		brushCanvasDisplaySize.x,
		brushCanvasDisplaySize.y);

	mainCanvasBgFbo.allocate(mainCanvasSize.x, mainCanvasSize.y, GL_RGB);
	mainCanvasBgFbo.begin();
	ofClear(255, 255, 255, 0);
	mainCanvasBgFbo.end();

	brushCanvasBgFbo.allocate(brushCanvasDisplaySize.x, brushCanvasDisplaySize.y, GL_RGB);
	brushCanvasBgFbo.begin();
	ofClear(255, 255, 255, 0);
	brushCanvasBgFbo.end();

	makeCanvasBg();

	prevBrushCanvasGridPoint = { -100,-100 };

	brushCanvasFbo.allocate(brushCanvasDisplaySize.x, brushCanvasDisplaySize.y, GL_RGBA);
	brushCanvasFbo.begin();
	ofClear(255, 255, 255, 0);
	brushCanvasFbo.end();

	mainCanvasFbo.allocate(mainCanvasSize.x, mainCanvasSize.y, GL_RGBA);
	mainCanvasFbo.begin();
	ofClear(255, 255, 255, 0);
	mainCanvasFbo.end();

	brush.allocate(brushCanvasComputeSize.x, brushCanvasComputeSize.y, OF_IMAGE_COLOR_ALPHA);

	brushAnchor.setup(brushCanvasPos, brushCanvasDisplaySize);

	/// ----- Status bars

    float fpsTxtLen = ( 88 + ( windowMargin * 2 ));
	status.setup({ mainCanvasRect.getLeft() + fpsTxtLen,
		windowMargin }, mainCanvasRect.getWidth() - fpsTxtLen);

	/// ----- Saved brushes

	savedBrushesPos =
	{ (brushCanvasRect.getRight() + windowMargin) + (brushCanvasComputeSize.x / 2),
		brushCanvasRect.getTop() + (brushCanvasComputeSize.y / 2) };
	savedBrushRects.resize(numSavedBrushes);
	int rowLength = 5;
	int y = 0, x = 0;
	for (int i = 0; i < savedBrushRects.size(); i++) {
		savedBrushRects[i].setSize(
			brushCanvasComputeSize.x,
			brushCanvasComputeSize.y);

		if (i > 0) {
			if (i % rowLength == 0) {
				x -= rowLength - 1;
				y++;
			}
			else x++;
		}
		savedBrushRects[i].setPosition(
			savedBrushesPos.x + ((savedBrushRects[i].getWidth() + windowMargin) * x),
			savedBrushesPos.y + ((savedBrushRects[i].getHeight() + windowMargin) * y));
	}

	savedBrushFbos.resize(numSavedBrushes);
	for (int i = 0; i < savedBrushFbos.size(); i++) {
		savedBrushFbos[i].allocate(brushCanvasComputeSize.x, brushCanvasComputeSize.y, GL_RGBA);
		savedBrushFbos[i].begin();
		ofClear(255, 255, 255, 0);
		savedBrushFbos[i].end();
	}

	loadBrushesFromFile();
	int i = 0;
	loadBrush(i);

	/// ------ GUI

	/// Brush panel

	colorPanelPos = { savedBrushesPos.x + ((savedBrushRects[0].getWidth() + windowMargin) * rowLength - windowMargin),
		brushCanvasRect.getTop() };

	brushPanel.setup();
	brushPanel.setPosition(colorPanelPos);

	brushPanel.add(brushPanelTitle.setup(brushPanelTitleStr));
	brushPanel.add(saveBrushBtn.setup(saveBrushButtonTxt));
	brushPanel.add(colorPreview.setup(colorLabel));
	brushPanel.add(red.setup("<-> R", 0, 0, 255));
	brushPanel.add(green.setup("<-> G", 0, 0, 255));
	brushPanel.add(blue.setup("<-> B", 0, 0, 255));
	brushPanel.add(erase.setup("< Eraser", false));
	brushPanel.add(setAnchorLabel.setup(setAnchorLabelTxt));
	brushPanel.add(setAnchorBtn_topLeft.setup(setAnchorBtnTxt_topLeft, true));
	brushPanel.add(setAnchorBtn_center.setup(setAnchorBtnTxt_center, false));
	brushPanel.add(startOverLabel_brushCanvas.setup(startOverLabelTxt));
	brushPanel.add(clearBrushBtn.setup(clearBrushBtnTxt));

	ofColor bgCol = ofColor::lightGray;
	ofColor warningBgCol = ofColor::pink;

	saveBrushBtn.setBackgroundColor(bgCol);
	saveBrushBtn.setTextColor(ofColor::black);
	saveBrushBtn.setFillColor(ofColor::black);
	saveBrushBtn.setBorderColor(ofColor::black);

	colorPreview.setBackgroundColor(bgCol);
	colorPreview.setTextColor(ofColor::black);
	colorPreview.setFillColor(ofColor::black);
	colorPreview.setBorderColor(ofColor::black);

	red.setBackgroundColor(bgCol);
	red.setTextColor(ofColor::black);
	red.setFillColor(ofColor::red);
	green.setBackgroundColor(bgCol);
	green.setTextColor(ofColor::black);
	green.setFillColor(ofColor::green);
	blue.setBackgroundColor(bgCol);
	blue.setTextColor(ofColor::black);
	blue.setFillColor(ofColor::blue);

	erase.setBackgroundColor(bgCol);
	erase.setTextColor(ofColor::black);
	erase.setFillColor(ofColor::black);
	erase.setBorderColor(ofColor::black);

	setAnchorLabel.setBackgroundColor(ofColor::black);
	setAnchorLabel.setTextColor(ofColor::white);
	setAnchorLabel.setFillColor(ofColor::black);
	setAnchorLabel.setBorderColor(ofColor::black);

	setAnchorBtn_topLeft.setBackgroundColor(bgCol);
	setAnchorBtn_topLeft.setTextColor(ofColor::black);
	setAnchorBtn_topLeft.setFillColor(ofColor::black);
	setAnchorBtn_topLeft.setBorderColor(ofColor::black);

	setAnchorBtn_center.setBackgroundColor(bgCol);
	setAnchorBtn_center.setTextColor(ofColor::black);
	setAnchorBtn_center.setFillColor(ofColor::black);
	setAnchorBtn_center.setBorderColor(ofColor::black);

	startOverLabel_brushCanvas.setBackgroundColor(ofColor::black);
	startOverLabel_brushCanvas.setTextColor(ofColor::white);
	startOverLabel_brushCanvas.setFillColor(ofColor::black);
	startOverLabel_brushCanvas.setBorderColor(ofColor::black);

	clearBrushBtn.setBackgroundColor(warningBgCol);
	clearBrushBtn.setTextColor(ofColor::black);
	clearBrushBtn.setFillColor(ofColor::black);
	clearBrushBtn.setBorderColor(ofColor::black);

	saveBrushBtn.addListener(this, &ofApp::saveBrushBtnAction);
	clearBrushBtn.addListener(this, &ofApp::clearBrushCanvas);
	setAnchorBtn_topLeft.addListener(this, &ofApp::setBrushAnchor_topLeft);
	setAnchorBtn_center.addListener(this, &ofApp::setBrushAnchor_center);

	/// Main canvas panel
	canvasPanelPos = { brushPanel.getPosition().x + brushPanel.getWidth() + windowMargin,
		brushCanvasRect.getTop() };

	canvasPanel.setup();
	canvasPanel.setPosition(canvasPanelPos);
	canvasPanel.add(canvasPanelTitle.setup(canvasPanelTitleStr));
	canvasPanel.add(savePaintingBtn.setup(savePaintingBtnTxt));
	canvasPanel.add(loadPaintingLabel.setup(loadPaintingLabelTxt));
	canvasPanel.add(loadPaintingField.setup("Type filename.png here", ""));
	canvasPanel.add(loadPaintingBtn.setup(loadPaintingBtnTxt));
	canvasPanel.add(bgColorLabel.setup(canvasBgLabelTxt));
	canvasPanel.add(fillRed.setup("<-> R", 0, 0, 255));
	canvasPanel.add(fillGreen.setup("<-> G", 0, 0, 255));
	canvasPanel.add(fillBlue.setup("<-> B", 0, 0, 255));
	canvasPanel.add(setCanvasBgBtn.setup(setCanvasBgTxt));
	canvasPanel.add(startOverLabel_mainCanvas.setup(startOverLabelTxt));
	canvasPanel.add(clearCanvasBtn.setup(clearCanvasBtnTxt));

	savePaintingBtn.setBackgroundColor(bgCol);
	savePaintingBtn.setTextColor(ofColor::black);
	savePaintingBtn.setFillColor(ofColor::black);
	savePaintingBtn.setBorderColor(ofColor::black);

	loadPaintingLabel.setBackgroundColor(ofColor::black);
	loadPaintingLabel.setTextColor(ofColor::white);
	loadPaintingLabel.setFillColor(ofColor::black);
	loadPaintingLabel.setBorderColor(ofColor::black);

	loadPaintingField.setBackgroundColor(bgCol);
	loadPaintingField.setTextColor(ofColor(0, 140));
	loadPaintingField.setFillColor(ofColor::black);
	loadPaintingField.setBorderColor(ofColor::black);

	loadPaintingBtn.setBackgroundColor(bgCol);
	loadPaintingBtn.setTextColor(ofColor::black);
	loadPaintingBtn.setFillColor(ofColor::black);
	loadPaintingBtn.setBorderColor(ofColor::black);

	setCanvasBgBtn.setBackgroundColor(bgCol);
	setCanvasBgBtn.setTextColor(ofColor::black);
	setCanvasBgBtn.setFillColor(ofColor::black);
	setCanvasBgBtn.setBorderColor(ofColor::black);

	bgColorLabel.setBackgroundColor(bgCol);
	bgColorLabel.setTextColor(ofColor::black);
	bgColorLabel.setFillColor(ofColor::black);
	bgColorLabel.setBorderColor(ofColor::black);

	fillRed.setBackgroundColor(bgCol);
	fillRed.setTextColor(ofColor::black);
	fillRed.setFillColor(ofColor::red);
	fillGreen.setBackgroundColor(bgCol);
	fillGreen.setTextColor(ofColor::black);
	fillGreen.setFillColor(ofColor::green);
	fillBlue.setBackgroundColor(bgCol);
	fillBlue.setTextColor(ofColor::black);
	fillBlue.setFillColor(ofColor::blue);

	startOverLabel_mainCanvas.setBackgroundColor(ofColor::black);
	startOverLabel_mainCanvas.setTextColor(ofColor::white);
	startOverLabel_mainCanvas.setFillColor(ofColor::black);
	startOverLabel_mainCanvas.setBorderColor(ofColor::black);

	clearCanvasBtn.setBackgroundColor(warningBgCol);
	clearCanvasBtn.setTextColor(ofColor::black);
	clearCanvasBtn.setFillColor(ofColor::black);
	clearCanvasBtn.setBorderColor(ofColor::black);

	clearCanvasBtn.addListener(this, &ofApp::clearMainCanvas);
	setCanvasBgBtn.addListener(this, &ofApp::fillMainCanvas);
	savePaintingBtn.addListener(this, &ofApp::savePainting);
	loadPaintingField.addListener(this, &ofApp::getTextFieldContent);
	loadPaintingBtn.addListener(this, &ofApp::loadPainting);

	loadPaintingFilename = "null";

	status.say("Welcome artist ^ - ^");
} /// end setup

//--------------------------------------------------------------
void ofApp::exit() {
	saveBrushBtn.removeListener(this, &ofApp::saveBrushBtnAction);
	setAnchorBtn_topLeft.removeListener(this, &ofApp::setBrushAnchor_topLeft);
	setAnchorBtn_center.removeListener(this, &ofApp::setBrushAnchor_center);
	clearBrushBtn.removeListener(this, &ofApp::clearBrushCanvas);
	clearCanvasBtn.removeListener(this, &ofApp::clearMainCanvas);
	setCanvasBgBtn.removeListener(this, &ofApp::fillMainCanvas);
	savePaintingBtn.removeListener(this, &ofApp::savePainting);
	loadPaintingField.removeListener(this, &ofApp::getTextFieldContent);
	loadPaintingBtn.removeListener(this, &ofApp::loadPainting);
}

//--------------------------------------------------------------
void ofApp::update() {
	drawCol = ofColor(red, green, blue, erase ? 0 : 255);
	colorPreview.setBackgroundColor(drawCol);

	ofColor c = ofColor(fillRed, fillGreen, fillBlue);
	bgColorLabel.setBackgroundColor(c);

	int speed = 30;
	int t = ofGetElapsedTimef() * speed;
	selectionHighlight.setHsb(t % 255, 255, 255);

	brushAnchor.update();

	status.update();
}

//--------------------------------------------------------------
void ofApp::draw() {
	ofBackground(ofColor::lightGray - 20);
    
    ofSetColor(ofColor::black);
    ofDrawBitmapString("FPS: " + ofToString(ofGetFrameRate()),
                       mainCanvasRect.getLeft(),
                       status.getBottom() - 5);
    
    cout << ofPoint(mainCanvasPos.x, windowMargin + 10) << endl;

	/// ----- Draw main canvas

	ofSetRectMode(OF_RECTMODE_CENTER);

	/// Canvas border
	ofSetColor(canvasBorderCol);
	ofDrawRectangle(mainCanvasPos.x + 1,
		mainCanvasPos.y + 1,
		mainCanvasSize.x + 3,
		mainCanvasSize.y + 3);

	/// Canvas
	ofSetColor(canvasBgCol);
	mainCanvasBgFbo.draw(mainCanvasPos.x, mainCanvasPos.y);

	/// ----- Draw brush canvas

	/// Canvas border
	ofSetColor(canvasBorderCol);
	ofDrawRectangle(brushCanvasPos.x + 1,
		brushCanvasPos.y + 1,
		brushCanvasDisplaySize.x + 3,
		brushCanvasDisplaySize.y + 3);

	/// Canvas
	ofSetColor(canvasBgCol);
	brushCanvasBgFbo.draw(brushCanvasPos.x,
		brushCanvasPos.y,
		brushCanvasDisplaySize.x,
		brushCanvasDisplaySize.y);

	/// ----- Canvas status bars

	status.draw();

	/// ----- Saved brushes

	for (int i = 0; i < savedBrushRects.size(); i++) {
		/// Border
		if (i == selectedBrush) {
			ofSetColor(selectionHighlight);
			ofDrawRectangle(savedBrushRects[i].getPosition().x,
				savedBrushRects[i].getPosition().y,
				savedBrushRects[i].getWidth() + 6,
				savedBrushRects[i].getHeight() + 6);
		}
		else {
			int borderSz = 2;
			if (hoveredBrush == i) borderSz = 6;
			ofSetColor(canvasBorderCol);
			ofDrawRectangle(savedBrushRects[i].getPosition().x,
				savedBrushRects[i].getPosition().y,
				savedBrushRects[i].getWidth() + borderSz,
				savedBrushRects[i].getHeight() + borderSz);
		}
		ofSetColor(ofColor::white);
		ofDrawRectangle(savedBrushRects[i]);
		savedBrushFbos[i].draw(savedBrushRects[i]);

		/// Selected brush highlight
	}

	/// ----- Brush paint

	if (bPaintingInBrushCanvas) updateBrushCanvas();
	ofSetColor(ofColor::white);
	brushCanvasFbo.draw(brushCanvasRect.getLeft() + (brushCanvasDisplaySize.x / 2),
		brushCanvasRect.getTop() + (brushCanvasDisplaySize.x / 2));

	/// ----- Main canvas paint

	if (bPaintingInMainCanvas) updateMainCanvas();
	mainCanvasFbo.draw(mainCanvasRect.getCenter().x,
		mainCanvasRect.getCenter().y);

	/// ----- Draw UI

	brushPanel.draw();
	canvasPanel.draw();
	brushAnchor.draw();

	/// ----- Debug stuff

	/*ofSetColor(ofColor::red);
	ofDrawBitmapString("FPS: " + ofToString(ofGetFrameRate()),
		windowMargin + 5,
		windowMargin + status.getHeight() - 5);*/
}

//--------------------------------------------------------------
void ofApp::updateBrushCanvas() {
	int x = ofGetMouseX();
	int y = ofGetMouseY();

	glm::vec2 brushCanvasMousePos = { x - brushCanvasRect.getLeft(),
		y - brushCanvasRect.getTop() };

	glm::vec2 p = posInGrid(brushCanvasMagnify,
		brushCanvasMousePos.x,
		brushCanvasMousePos.y);

	ofSetColor(drawCol);

	if (p == prevBrushCanvasGridPoint) return;

	/// Erase current pixel before drawing new pixel in its place
	/// Method: reallocate the entire canvas minus p.x,p.y
	if (erase) {
		ofPixels pix, culledPix;
		brushCanvasFbo.getTexture().readToPixels(pix);
		culledPix.allocate(pix.getWidth(), pix.getHeight(), pix.getImageType());
		for (int x = 0; x < brushCanvasFbo.getWidth(); x += brushCanvasMagnify) {
			for (int y = 0; y < brushCanvasFbo.getHeight(); y += brushCanvasMagnify) {
				if (glm::vec2(x, y) != p) {
					for (int xx = 0; xx < brushCanvasMagnify; xx++) {
						for (int yy = 0; yy < brushCanvasMagnify; yy++) {
							culledPix.setColor(x + xx, y + yy, pix.getColor(x, y));
						}
					}
				}
				else {
					for (int xx = 0; xx < brushCanvasMagnify; xx++) {
						for (int yy = 0; yy < brushCanvasMagnify; yy++) {
							culledPix.setColor(x + xx, y + yy, ofColor(255, 255, 255, 0));
						}
					}
				}
			}
		}

		brushCanvasFbo.begin(); ofClear(255, 255, 255, 0); brushCanvasFbo.end();
		brushCanvasFbo.getTexture().loadData(culledPix);
	}

	brushCanvasFbo.begin();
	ofDrawRectangle(p.x + (brushCanvasMagnify / 2),
		p.y + (brushCanvasMagnify / 2),
		brushCanvasMagnify,
		brushCanvasMagnify);
	brushCanvasFbo.end();

	prevBrushCanvasGridPoint = p;
}

//--------------------------------------------------------------
void ofApp::clearBrushCanvas() {
	brushCanvasFbo.begin();
	ofClear(255, 255, 255, 0);
	brushCanvasFbo.end();

    status.say("Brush canvas cleared!", StatusBar::UrgencyLevel::URGENCY_LEVEL_WARNING);
}

//--------------------------------------------------------------
void ofApp::setBrushAnchor_topLeft(bool& b) {
	if (b) {
		bAnchorCenter = false;
		setAnchorBtn_center = false;
		brushAnchor.animate(bAnchorCenter);
	}
	else {
		if (setAnchorBtn_center == false) setAnchorBtn_topLeft = true;
	}
}

//--------------------------------------------------------------
void ofApp::setBrushAnchor_center(bool& b) {
	if (b) {
		bAnchorCenter = true;
		setAnchorBtn_topLeft = false;
		brushAnchor.animate(bAnchorCenter);
	}
	else {
		if (setAnchorBtn_topLeft == false) setAnchorBtn_center = true;
	}
}

//--------------------------------------------------------------
void ofApp::updateMainCanvas() {
	int x = ofGetMouseX() - mainCanvasRect.getLeft();
	int y = ofGetMouseY() - mainCanvasRect.getTop();

	glm::vec2 anchor = { (x + (brushCanvasComputeSize.x / 2)),
			y + (brushCanvasComputeSize.y / 2) };
	if (bAnchorCenter) {
		anchor = { x,y };
	}

	mainCanvasFbo.begin();
	brush.draw(anchor.x, anchor.y, 26, 26);
	mainCanvasFbo.end();
}

//--------------------------------------------------------------
void ofApp::clearMainCanvas() {
	mainCanvasFbo.begin();
	ofClear(255, 255, 255, 0);
	mainCanvasFbo.end();

	status.say("Canvas cleared!", StatusBar::UrgencyLevel::URGENCY_LEVEL_WARNING);
}

//--------------------------------------------------------------
void ofApp::fillMainCanvas() {
	mainCanvasFbo.begin();
	ofClear(fillRed, fillGreen, fillBlue, 255);
	mainCanvasFbo.end();

	if (ofColor(fillRed, fillGreen, fillBlue) == ofColor::lime) {
		if (ofRandom(1.0) > 0.8) status.say("U about 2 steal christmas");
	}
	else if (ofColor(fillRed, fillGreen, fillBlue) == ofColor::red) {
		if (ofRandom(1.0) > 0.9) status.say("H A I L   S A T A N");
	}
	else if (ofColor(fillRed, fillGreen, fillBlue) == ofColor::blue) {
		if (ofRandom(1.0) > 0.8) status.say("Cold, yet soothing...");
	}
	else if (ofColor(fillRed, fillGreen, fillBlue) == ofColor::black) {
		if (ofRandom(1.0) > 0.8) status.say("Gaze unto the void");
	}
	else if (ofColor(fillRed, fillGreen, fillBlue) == ofColor::white) {
		if (ofRandom(1.0) > 0.8) status.say("A fluorescent white box glows before you.");
	}
	else if (ofColor(fillRed, fillGreen, fillBlue) == ofColor::magenta) {
		if (ofRandom(1.0) > 0.8) status.say("YAAAAAAAAAAS");
	}
	else if (ofColor(fillRed, fillGreen, fillBlue) == ofColor::cyan) {
		if (ofRandom(1.0) > 0.8) status.say("Not a single cloud.");
	}
	else if (ofColor(fillRed, fillGreen, fillBlue) == ofColor::yellow) {
		if (ofRandom(1.0) > 0.8) status.say("Urine trouble!");
	}
}

//--------------------------------------------------------------
void ofApp::savePainting() {
	ofPixels p;
	mainCanvasFbo.getTexture().readToPixels(p);
	string filename = ofGetTimestampString("%F_%H-%M-%S");
    ofDirectory paintingsFolder(ofToDataPath("paintings/"));
    if(!paintingsFolder.exists()){
        paintingsFolder.create(true);
    }
	ofImage img;
	img.setFromPixels(p);
	img.saveImage(ofToDataPath("paintings/" + filename + ".png"));
	status.say("Beautiful!!!!!!!!!! Painting saved @ \"" + paintingsFolder.getAbsolutePath() + "\\" + filename + ".png" + "\" :D",
		StatusBar::UrgencyLevel::URGENCY_LEVEL_PARTY);
}

//--------------------------------------------------------------
void ofApp::getTextFieldContent(string& filename) {
	loadPaintingFilename = filename;
	cout << "Text field input: " << loadPaintingFilename << endl;
}

//--------------------------------------------------------------
void ofApp::loadPainting() {
	string filename = loadPaintingFilename;
	std::cout << "Loading [" << filename << "]" << endl;
	ofFile f;

	if (!f.doesFileExist(ofToDataPath("paintings/" + filename))) {
		cout << "File " << ofToDataPath("paintings/" + filename) << " doesnt exist" << endl;
		status.say("File named \"" + filename + "\" doesn't exist in \"paintings/\" folder!",
			StatusBar::UrgencyLevel::URGENCY_LEVEL_WARNING);
		return;
	}

	ofImage img;

	if (!img.load(ofToDataPath("paintings/" + filename))) {
		status.say("Error loading file \"" + filename + "\" :( Is it corrupt?",
			StatusBar::UrgencyLevel::URGENCY_LEVEL_WARNING);
		return;
	};
	ofPixels p;
	img.getTexture().readToPixels(p);

	mainCanvasFbo.begin();
	ofClear(255, 255, 255, 0);
	mainCanvasFbo.end();
	mainCanvasFbo.getTexture().loadData(p);

	loadPaintingFilename = "null";
	status.say("Painting loaded!");
}

//--------------------------------------------------------------
void ofApp::saveBrushBtnAction(){
    updateBrush();
    status.say("Brush saved!");
}

//--------------------------------------------------------------
void ofApp::updateBrush() {
	ofPixels pix, culledPix;

	brushCanvasFbo.getTexture().setTextureMinMagFilter(GL_NEAREST, GL_NEAREST);
	brushCanvasFbo.getTexture().readToPixels(pix);
	culledPix.allocate(brush.getWidth(), brush.getHeight(), pix.getImageType());

	for (int y = 0; y < brushCanvasFbo.getTexture().getWidth(); y += brushCanvasMagnify) {
		for (int x = 0; x < brushCanvasFbo.getTexture().getHeight(); x += brushCanvasMagnify) {
			culledPix.setColor(x / brushCanvasMagnify, y / brushCanvasMagnify, pix.getColor(x, y));
		}
	}

	brush.getTextureReference().setTextureMinMagFilter(GL_NEAREST, GL_NEAREST);
	brush.getTextureReference().loadData(culledPix);

	saveBrush(culledPix);
}

//--------------------------------------------------------------
void ofApp::saveBrush(ofPixels& p) {
	savedBrushFbos[selectedBrush].getTextureReference().loadData(p);
	saveBrushToFile(p);
}

//--------------------------------------------------------------
void ofApp::saveBrushToFile(ofPixels& p) {
	ofImage img;
	img.setFromPixels(p);
	img.saveImage(ofToDataPath("brushes/" + ofToString(selectedBrush) + ".png"));
}

//--------------------------------------------------------------
void ofApp::loadBrush(int& brushId) {
	/// Read every pixel of savedBrushFbos[brushId] into pix
	std::cout << "Loading brush " << brushId << endl;
	ofPixels pix, biggifiedPix;
	brushCanvasFbo.getTexture().setTextureMinMagFilter(GL_NEAREST, GL_NEAREST);
	savedBrushFbos[brushId].getTexture().readToPixels(pix);

	/// Biggify pixels from pix to biggifiedPix
	biggifiedPix.allocate(brushCanvasFbo.getWidth(), brushCanvasFbo.getHeight(), pix.getImageType());

	/// For every pixel in brushCanvasFbo, check each pixel from pix 8 times
	int xx = -1;
	int yy = -1;
	for (int y = 0; y < brushCanvasFbo.getTexture().getWidth(); y++) {
		if (y % brushCanvasMagnify == 0)yy++;
		for (int x = 0; x < brushCanvasFbo.getTexture().getHeight(); x++) {
			if (x % brushCanvasMagnify == 0) {
				xx++;
				if (xx >= savedBrushRects[0].getWidth()) xx = 0;
			}
			biggifiedPix.setColor(x, y, pix.getColor(xx, yy));
		}
	}

	brushCanvasFbo.getTextureReference().setTextureMinMagFilter(GL_NEAREST, GL_NEAREST);
	brushCanvasFbo.getTextureReference().loadData(biggifiedPix);

//    std::cout << "Brush loaded!" << endl;
    status.say("Brush loaded!");
	updateBrush();
}

//--------------------------------------------------------------
void ofApp::loadBrushesFromFile() {
	int i = 0;
	for (auto fbo : savedBrushFbos) {
		ofImage img;
		ofFile f;

		if (!f.doesFileExist(ofToDataPath("brushes/" + ofToString(i) + ".png"))) return;

		img.load(ofToDataPath("brushes/" + ofToString(i) + ".png"));
		ofPixels p;
		img.getTexture().readToPixels(p);

		fbo.begin();
		ofClear(255, 255, 255, 0);
		fbo.end();
		fbo.getTexture().loadData(p);

		i++;
	}
}

//--------------------------------------------------------------
glm::vec2 ofApp::posInGrid(int gridSize, int x, int y) ///via https://stackoverflow.com/a/36795011/1757149
{
	glm::vec2 result = { x, y }; /// 199, 199
	result.x = (int)(result.x / gridSize) * gridSize; /// (199 / 8 ) * 8
	result.y = (int)(result.y / gridSize) * gridSize;
	return result;
}

//--------------------------------------------------------------
void ofApp::makeCanvasBg()
{
	/// ----- Main canvas

	int blockSz = 13;
	mainCanvasBgFbo.begin();
	bool columnStartsGray = false;
	for (int x = 0; x < mainCanvasSize.x; x += blockSz) {
		if (x % (blockSz * 2) == 0 || x == 0) columnStartsGray = true;
		else columnStartsGray = false;
		for (int y = 0; y < mainCanvasSize.y; y += blockSz) {
			if (y % (blockSz * 2) == 0 || y == 0) {
				if (columnStartsGray) ofSetColor(ofColor::lightGray);
				else ofSetColor(ofColor::white);
			}
			else {
				if (columnStartsGray) ofSetColor(ofColor::white);
				else ofSetColor(ofColor::lightGray);
			}
			//ofSetColor(ofColor::green);
			ofDrawRectangle(x, y, blockSz, blockSz);
		}
	}
	mainCanvasBgFbo.end();

	/// ----- Brush canvas

	brushCanvasBgFbo.begin();
	columnStartsGray = false;
	for (int x = 0; x < brushCanvasDisplaySize.x; x += brushCanvasMagnify) {
		if (x % (brushCanvasMagnify * 2) == 0 || x == 0) columnStartsGray = true;
		else columnStartsGray = false;
		for (int y = 0; y < brushCanvasDisplaySize.y; y += brushCanvasMagnify) {
			if (y % (brushCanvasMagnify * 2) == 0 || y == 0) {
				if (columnStartsGray) ofSetColor(ofColor::lightGray);
				else ofSetColor(ofColor::white);
			}
			else {
				if (columnStartsGray) ofSetColor(ofColor::white);
				else ofSetColor(ofColor::lightGray);
			}
			ofDrawRectangle(x, y, brushCanvasMagnify, brushCanvasMagnify);
		}
	}
	brushCanvasBgFbo.end();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key) {
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key) {
}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y) {
	int savedBrushRectX = x + (savedBrushRects[0].getWidth() / 2);
	int savedBrushRectY = y + (savedBrushRects[0].getHeight() / 2);

	for (int i = 0; i < savedBrushRects.size(); i++) {
		if (savedBrushRects[i].inside(savedBrushRectX, savedBrushRectY)) {
			hoveredBrush = i;
			return;
		}
		else hoveredBrush = -1;
	}
}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button) {
	bPaintingInBrushCanvas = brushCanvasRect.inside(x, y) ? 1 : 0;
	bPaintingInMainCanvas = mainCanvasRect.inside(x, y) ? 1 : 0;
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button) {
	bPaintingInBrushCanvas = brushCanvasRect.inside(x, y) ? 1 : 0;
	bPaintingInMainCanvas = mainCanvasRect.inside(x, y) ? 1 : 0;
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button) {
	bPaintingInBrushCanvas = false;
	bPaintingInMainCanvas = false;

	int savedBrushRectX = x + (savedBrushRects[0].getWidth() / 2);
	int savedBrushRectY = y + (savedBrushRects[0].getHeight() / 2);

	for (int i = 0; i < savedBrushRects.size(); i++) {
		if (savedBrushRects[i].inside(savedBrushRectX, savedBrushRectY)) {
			selectedBrush = i;
			loadBrush(i);
			return;
		}
	}
}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y) {
}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y) {
}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h) {
}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg) {
}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo) {
}

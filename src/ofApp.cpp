#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup() {
	ofDisableAntiAliasing();
	ofLogToConsole();
	ofSetFrameRate(60);

	glGetIntegerv(GL_MAX_TEXTURE_SIZE, &maxTexSz);
	maxTexSz -= 8;
	cout << maxTexSz << endl;

	/// ----- Placement of all window elements

	/// Main canvas

	mainCanvasPos = { windowMargin + (mainCanvasSize.x / 2),
						status.getHeight() + (windowMargin * 2) + (mainCanvasSize.y / 2) };

	mainCanvasRect.set(mainCanvasPos.x - (mainCanvasSize.x / 2),
		mainCanvasPos.y - (mainCanvasSize.y / 2),
		mainCanvasSize.x,
		mainCanvasSize.y);

	canvasScrollX = 0;
	canvasScrollY = 0;

	/// Main canvas scrollable container

	/// Container should be mainCanvas size by default
	canvasContainer.set(
		mainCanvasRect.getLeft(),
		mainCanvasRect.getTop(),
		mainCanvasRect.getWidth(),
		mainCanvasRect.getHeight());

	canvasContainerFbo.allocate(canvasContainer.getWidth(), canvasContainer.getHeight(), GL_RGBA);
	canvasContainerFbo.begin();
	ofClear(255, 255, 255, 0);
	canvasContainerFbo.end();

	/// Brush canvas

	setBrushCanvasRect();

	canvasContainerMaxSz = {
		ofGetWidth() - windowMargin * 2 - scrollbar.getSize(),
		ofGetHeight() - (windowMargin * 4) - brushCanvasRect.getHeight() - status.getHeight() - 100 };

	cout << "canvasContainerMaxSz: " << canvasContainerMaxSz << endl;

	/// Canvas transparent grid bgs

	mainCanvasBgFbo.allocate(mainCanvasSize.x, mainCanvasSize.y, GL_RGB);
	mainCanvasBgFbo.begin();
	ofClear(255, 255, 255, 0);
	mainCanvasBgFbo.end();

	brushCanvasBgFbo.allocate(brushCanvasDisplaySize.x, brushCanvasDisplaySize.y, GL_RGB);
	brushCanvasBgFbo.begin();
	ofClear(255, 255, 255, 0);
	brushCanvasBgFbo.end();

	makeMainCanvasBg();
	makeBrushCanvasBg();

	prevBrushCanvasGridPoint = { -100,-100 };

	/// Canvas FBOs

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

	/// ----- Saved brushes

	brushMenuPos =
	{ (brushCanvasRect.getRight() + windowMargin) + (brushCanvasComputeSize.x / 2),
		brushCanvasRect.getTop() + (brushCanvasComputeSize.y / 2) };
	brushMenuRects.resize(numSavedBrushes);

	for (int i = 0; i < brushMenuRects.size(); i++) {
		brushMenuRects[i].setSize(
			brushCanvasComputeSize.x,
			brushCanvasComputeSize.y);
	}

	setBrushMenuPos();

	brushMenuFbos.resize(numSavedBrushes);
	for (int i = 0; i < brushMenuFbos.size(); i++) {
		brushMenuFbos[i].allocate(brushCanvasComputeSize.x, brushCanvasComputeSize.y, GL_RGBA);
		brushMenuFbos[i].begin();
		ofClear(255, 255, 255, 0);
		brushMenuFbos[i].end();
	}

	loadBrushesFromFile();
	int i = 0;
	loadBrush(i);

	/// ------ GUI

	setupGui();

	/// ----- Status bar

	fpsTxtLen = (((fpsLabel.size() + ofToString(ofGetFrameRate()).substr(0, 2).size()) * 8) + (windowMargin * 2));
	statusBarMinLen = canvasPanel.getPosition().x + canvasPanel.getWidth();
	status.setup({ mainCanvasRect.getLeft() + fpsTxtLen,
		windowMargin }, canvasContainer.getWidth() - fpsTxtLen, statusBarMinLen);

	/// ----- Modals

	/// Set canvas dims modal

	canvasDimsModal.setup(maxTexSz);
	ofAddListener(canvasDimsModal.setCanvasDims, this, &ofApp::setCanvasDims);

	/// Load painting modal

	loadPaintingModal.setup();
	ofAddListener(loadPaintingModal.loadPainting, this, &ofApp::loadPainting);

	/// ----- Scrollbar

	scrollbar.setup(canvasContainer, mainCanvasRect);
	isCanvasTooBigX = false;
	isCanvasTooBigY = false;

	status.say("Welcome artist ^ - ^");
} /// end setup

//--------------------------------------------------------------
void ofApp::setupGui() {
	/// Brush panel

	brushPanelPos = { brushMenuPos.x + brushMenuRect.getWidth() - windowMargin,
		brushCanvasRect.getTop() };

	brushPanel.setup();
	brushPanel.setPosition(brushPanelPos);

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
	canvasPanel.add(loadPaintingBtn.setup(loadPaintingBtnTxt));
	canvasPanel.add(setSizeLabel.setup(setSizeLabelTxt));
	canvasPanel.add(setSizeBtn.setup(setSizeBtnTxt));

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

	loadPaintingBtn.setBackgroundColor(bgCol);
	loadPaintingBtn.setTextColor(ofColor::black);
	loadPaintingBtn.setFillColor(ofColor::black);
	loadPaintingBtn.setBorderColor(ofColor::black);

	setSizeLabel.setBackgroundColor(ofColor::black);
	setSizeLabel.setTextColor(ofColor::white);
	setSizeLabel.setFillColor(ofColor::black);
	setSizeLabel.setBorderColor(ofColor::black);

	setSizeBtn.setBackgroundColor(bgCol);
	setSizeBtn.setTextColor(ofColor::black);
	setSizeBtn.setFillColor(ofColor::black);
	setSizeBtn.setBorderColor(ofColor::black);

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
	loadPaintingBtn.addListener(this, &ofApp::showLoadPaintingModal);
	setSizeBtn.addListener(this, &ofApp::ShowSetSizeModal);

	/// Main canvas Pt 2 panel

	canvas2PanelPos = { canvasPanel.getPosition().x + canvasPanel.getWidth() + windowMargin,
		brushCanvasRect.getTop() };

	canvas2Panel.setup();
	canvas2Panel.setPosition(canvas2PanelPos);
	canvas2Panel.add(canvas2PanelTitle.setup(canvas2PanelTitleStr));
	canvas2Panel.add(toggleGridLabel.setup(toggleGridLabelTxt));
	canvas2Panel.add(toggleGridBtn.setup(toggleGridBtnTxt, false));

	toggleGridBtn.setBackgroundColor(bgCol);
	toggleGridBtn.setTextColor(ofColor::black);
	toggleGridBtn.setFillColor(ofColor::black);
	toggleGridBtn.setBorderColor(ofColor::black);
}

//--------------------------------------------------------------
void ofApp::setGuiPos() {
	brushPanelPos = { brushMenuPos.x + brushMenuRect.getWidth() - windowMargin,
		brushCanvasRect.getTop() };
	brushPanel.setPosition(brushPanelPos);

	canvasPanelPos = { brushPanel.getPosition().x + brushPanel.getWidth() + windowMargin,
		brushCanvasRect.getTop() };
	canvasPanel.setPosition(canvasPanelPos);

	canvas2PanelPos = { canvasPanel.getPosition().x + canvasPanel.getWidth() + windowMargin,
		brushCanvasRect.getTop() };
	canvas2Panel.setPosition(canvas2PanelPos);
}

//--------------------------------------------------------------
void ofApp::exit() {
	saveBrushBtn.removeListener(this, &ofApp::saveBrushBtnAction);
	setAnchorBtn_topLeft.removeListener(this, &ofApp::setBrushAnchor_topLeft);
	setAnchorBtn_center.removeListener(this, &ofApp::setBrushAnchor_center);
	clearBrushBtn.removeListener(this, &ofApp::clearBrushCanvas);
	clearCanvasBtn.removeListener(this, &ofApp::clearMainCanvas);
	setCanvasBgBtn.removeListener(this, &ofApp::fillMainCanvas);
	savePaintingBtn.removeListener(this, &ofApp::savePainting);
	loadPaintingBtn.removeListener(this, &ofApp::showLoadPaintingModal);
	setSizeBtn.removeListener(this, &ofApp::ShowSetSizeModal);

	canvasDimsModal.exit();
	ofRemoveListener(canvasDimsModal.setCanvasDims, this, &ofApp::setCanvasDims);

	loadPaintingModal.exit();
	ofRemoveListener(loadPaintingModal.loadPainting, this, &ofApp::loadPainting);
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

	canvasDimsModal.update();
	loadPaintingModal.update();
	scrollbar.update(canvasContainer, mainCanvasRect);
}

//--------------------------------------------------------------
void ofApp::draw() {
	ofBackground(ofColor::lightGray - 20);

	ofSetColor(ofColor::black);
	ofDrawBitmapString(fpsLabel + ofToString(ofGetFrameRate()).substr(0, 2),
		mainCanvasRect.getLeft(),
		status.getBottom() - 5);

	/// ----- Draw main canvas

	ofSetRectMode(OF_RECTMODE_CORNER);

	/// Canvas container
	ofSetColor(0, 255, 0);
	ofDrawRectangle(canvasContainer);

	/// Canvas border
	ofSetColor(canvasBorderCol);
	ofDrawRectangle(canvasContainer.getLeft() - 1,
		canvasContainer.getTop() - 1,
		canvasContainer.getWidth() + 2,
		canvasContainer.getHeight() + 2);

	/// Canvas
	ofSetRectMode(OF_RECTMODE_CORNER);
	ofSetColor(canvasBgCol);
	canvasContainerFbo.begin(); {
		mainCanvasBgFbo.draw(canvasScrollX, canvasScrollY);
	}canvasContainerFbo.end();
	ofSetRectMode(OF_RECTMODE_CENTER);

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

	ofNoFill();
	for (int i = 0; i < brushMenuRects.size(); i++) {
		/// Border
		if (i == selectedBrush) {
			ofSetColor(selectionHighlight);
			ofDrawRectangle(brushMenuRects[i].getPosition().x,
				brushMenuRects[i].getPosition().y,
				brushMenuRects[i].getWidth() + 6,
				brushMenuRects[i].getHeight() + 6);
		}
		else {
			int borderSz = 2;
			if (hoveredBrush == i) borderSz = 6;
			ofSetColor(canvasBorderCol);
			ofDrawRectangle(brushMenuRects[i].getPosition().x,
				brushMenuRects[i].getPosition().y,
				brushMenuRects[i].getWidth() + borderSz,
				brushMenuRects[i].getHeight() + borderSz);
		}
		ofSetColor(ofColor::white);
		ofDrawRectangle(brushMenuRects[i]);
		brushMenuFbos[i].draw(brushMenuRects[i]);

		/// Selected brush highlight
	}

	/// ----- Brush paint

	ofFill();
	if (bPaintingInBrushCanvas) updateBrushCanvas();
	ofSetColor(ofColor::white);
	brushCanvasFbo.draw(brushCanvasRect.getLeft() + (brushCanvasDisplaySize.x / 2),
		brushCanvasRect.getTop() + (brushCanvasDisplaySize.x / 2));

	/// ----- Main canvas paint

	if (bPaintingInMainCanvas) updateMainCanvas();
	canvasContainerFbo.begin(); {
		ofSetRectMode(OF_RECTMODE_CORNER);
		mainCanvasFbo.draw(canvasScrollX, canvasScrollY);
	}canvasContainerFbo.end();

	ofSetRectMode(OF_RECTMODE_CORNER);

	canvasContainerFbo.draw(canvasContainer.getLeft(), canvasContainer.getTop());

	ofSetRectMode(OF_RECTMODE_CENTER);

	/// ----- Scrollbar

	ofSetRectMode(OF_RECTMODE_CORNER);

	scrollbar.draw(isCanvasTooBigX, isCanvasTooBigY);

	/// ----- Draw UI

	drawGuis();

	/// ----- Modals

	ofSetRectMode(OF_RECTMODE_CORNER);

	canvasDimsModal.draw();

	loadPaintingModal.draw(selectionHighlight);
}

//--------------------------------------------------------------
void ofApp::setBrushMenuPos() {
	brushMenuPos =
	{ (brushCanvasRect.getRight() + windowMargin) + (brushCanvasComputeSize.x / 2),
		brushCanvasRect.getTop() + (brushCanvasComputeSize.y / 2) };

	int rowLength = 5;
	int x = 0, y = 0;
	for (int i = 0; i < brushMenuRects.size(); i++) {
		if (i > 0) {
			if (i % rowLength == 0) {
				x -= rowLength - 1;
				y++;
			}
			else x++;
		}
		brushMenuRects[i].setPosition(
			brushMenuPos.x + ((brushMenuRects[i].getWidth() + brushMenuPadding) * x),
			brushMenuPos.y + ((brushMenuRects[i].getHeight() + brushMenuPadding) * y));
	}

	brushMenuRect.set(brushMenuPos.x,
		brushMenuPos.y,
		(brushMenuRects[0].getWidth() + brushMenuPadding) * rowLength,
		((brushMenuRects[0].getHeight() + brushMenuPadding) * (brushMenuRects.size() / rowLength))); /// number of rows should be brushMenuRects.size() / rowLength
}

//--------------------------------------------------------------
void ofApp::drawGuis() {
	if (canvasDimsModal.window.isVisible()) return;
	else if (loadPaintingModal.window.isVisible()) return;

	brushPanel.draw();
	canvasPanel.draw();
	canvas2Panel.draw();
	brushAnchor.draw();
}

//--------------------------------------------------------------
void ofApp::ShowSetSizeModal() {
	canvasDimsModal.setTextFields(ofToString(mainCanvasFbo.getWidth()), ofToString(mainCanvasFbo.getHeight()));
	canvasDimsModal.window.toggleVisible(true);
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
	/// First check if a modal is active
	if (canvasDimsModal.window.isVisible()) return;
	else if (loadPaintingModal.window.isVisible()) return;

	brushCanvasFbo.begin();
	ofClear(255, 255, 255, 0);
	brushCanvasFbo.end();

	status.say("Brush canvas cleared!", StatusBar::UrgencyLevel::URGENCY_LEVEL_WARNING);
}

//--------------------------------------------------------------
void ofApp::setBrushAnchor_topLeft(bool& b) {
	/// First check if a modal is active
	if (canvasDimsModal.window.isVisible()) return;
	else if (loadPaintingModal.window.isVisible()) return;

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
	/// First check if a modal is active
	if (canvasDimsModal.window.isVisible()) return;
	else if (loadPaintingModal.window.isVisible()) return;

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
	int x = (ofGetMouseX() - mainCanvasRect.getLeft()) - canvasScrollX;
	int y = (ofGetMouseY() - mainCanvasRect.getTop()) - canvasScrollY;

	if (toggleGridBtn) {
		glm::vec2 anchor = { x + (brushCanvasComputeSize.x / 2),
			y + (brushCanvasComputeSize.y / 2) };

		glm::vec2 p;
		p.x = xPosInGrid(brushCanvasComputeSize.x + 1, anchor.x);
		p.y = yPosInGrid(brushCanvasComputeSize.y + 1, anchor.y);

		if (p == prevBrushCanvasGridPoint) return;

		mainCanvasFbo.begin();
		brush.draw(p.x, p.y, 26, 26);
		mainCanvasFbo.end();

		prevBrushCanvasGridPoint = p;
	}
	else {
		glm::vec2 anchor = { x + (brushCanvasComputeSize.x / 2),
				y + (brushCanvasComputeSize.y / 2) };
		if (bAnchorCenter) {
			anchor = { x,y };
		}

		mainCanvasFbo.begin();
		brush.draw(anchor.x, anchor.y, 26, 26);
		mainCanvasFbo.end();
	}
}

//--------------------------------------------------------------
void ofApp::clearMainCanvas() {
	/// First check if a modal is active
	if (canvasDimsModal.window.isVisible()) return;
	else if (loadPaintingModal.window.isVisible()) return;

	mainCanvasFbo.begin();
	ofClear(255, 255, 255, 0);
	mainCanvasFbo.end();

	status.say("Canvas cleared!", StatusBar::UrgencyLevel::URGENCY_LEVEL_WARNING);
}

//--------------------------------------------------------------
void ofApp::fillMainCanvas() {
	/// First check if a modal is active
	if (canvasDimsModal.window.isVisible()) return;
	else if (loadPaintingModal.window.isVisible()) return;

	mainCanvasFbo.begin();
	ofClear(fillRed, fillGreen, fillBlue, 255);
	mainCanvasFbo.end();
}

//--------------------------------------------------------------
void ofApp::setCanvasDims(int& i) {
	if (canvasDimsModal.getWidthInput().find_first_not_of("0123456789") != std::string::npos) {
		status.say("Invalid width input: whole numbers within range only! Action cancelled.", StatusBar::UrgencyLevel::URGENCY_LEVEL_WARNING);
		return;
	}
	else if (canvasDimsModal.getHeightInput().find_first_not_of("0123456789") != std::string::npos) {
		status.say("Invalid height input: whole numbers within range only! Action cancelled.", StatusBar::UrgencyLevel::URGENCY_LEVEL_WARNING);
		return;
	}

	resizeCanvas(
		ofToInt(canvasDimsModal.getWidthInput()),
		ofToInt(canvasDimsModal.getHeightInput()));
}

//--------------------------------------------------------------
void ofApp::resizeCanvas(int w, int h) {
	canvasScrollX = 0;
	canvasScrollY = 0;

	if (w < 1) w = 1;
	else if (w > maxTexSz) w = maxTexSz;

	if (h < 1) h = 1;
	else if (h > maxTexSz) h = maxTexSz;

	ofFbo mainCanvasFboBackup = mainCanvasFbo;

	mainCanvasSize = { w, h };
	mainCanvasFbo.allocate(mainCanvasSize.x, mainCanvasSize.y, GL_RGBA);

	mainCanvasFbo.begin();
	ofClear(255, 255, 255, 0);
	mainCanvasFboBackup.draw(0, 0);
	mainCanvasFbo.end();

	mainCanvasBgFbo.allocate(mainCanvasSize.x, mainCanvasSize.y, GL_RGB);
	mainCanvasBgFbo.begin();
	ofClear(255, 255, 255, 0);
	mainCanvasBgFbo.end();

	makeMainCanvasBg();
	mainCanvasPos = { windowMargin + (mainCanvasSize.x / 2),
						status.getHeight() + (windowMargin * 2) + (mainCanvasSize.y / 2) };
	mainCanvasRect.set(mainCanvasPos.x - (mainCanvasSize.x / 2),
		mainCanvasPos.y - (mainCanvasSize.y / 2),
		mainCanvasSize.x,
		mainCanvasSize.y);

	/// Recheck if canvas.height is > canvasContainerMaxSz

	if (mainCanvasRect.getHeight() <= canvasContainerMaxSz.y) {
		canvasContainer.height = mainCanvasRect.getHeight();
		isCanvasTooBigY = false;
	}
	else if (mainCanvasRect.getHeight() > canvasContainerMaxSz.y) {
		canvasContainer.height = canvasContainerMaxSz.y;
		isCanvasTooBigY = true;
	}

	if (mainCanvasRect.getWidth() <= canvasContainerMaxSz.x) {
		canvasContainer.width = mainCanvasRect.getWidth();
		isCanvasTooBigX = false;
	}
	else if (mainCanvasRect.getWidth() > canvasContainerMaxSz.x) {
		canvasContainer.width = canvasContainerMaxSz.x;
		isCanvasTooBigX = true;
	}

	canvasContainerFbo.allocate(canvasContainer.getWidth(), canvasContainer.getHeight(), GL_RGBA);
	canvasContainerFbo.begin();
	ofClear(255, 255, 255, 0);
	canvasContainerFbo.end();

	setBrushCanvasRect();
	setBrushMenuPos();
	setGuiPos();
	scrollbar.setup(canvasContainer, mainCanvasRect);
	makeMainCanvasBg();

	/// Reset scrollbar length
	status.setLength(canvasContainer.getWidth() - fpsTxtLen);
}

//--------------------------------------------------------------
void ofApp::setBrushCanvasRect() {
	brushCanvasPos = { windowMargin + (brushCanvasDisplaySize.y / 2),
						 canvasContainer.getBottom() + scrollbar.getSize() + windowMargin + (brushCanvasDisplaySize.y / 2) };

	brushCanvasRect.set(brushCanvasPos.x - (brushCanvasDisplaySize.x / 2),
		brushCanvasPos.y - (brushCanvasDisplaySize.y / 2),
		brushCanvasDisplaySize.x,
		brushCanvasDisplaySize.y);
}

//--------------------------------------------------------------
void ofApp::savePainting() {
	/// First check if a modal is active
	if (canvasDimsModal.window.isVisible()) return;
	else if (loadPaintingModal.window.isVisible()) return;

	ofPixels p;
	mainCanvasFbo.getTexture().readToPixels(p);
	string filename = ofGetTimestampString("%F_%H-%M-%S");
	ofDirectory paintingsFolder(ofToDataPath("paintings/"));
	if (!paintingsFolder.exists()) {
		paintingsFolder.create(true);
	}
	ofImage img;
	img.setFromPixels(p);
	img.saveImage(ofToDataPath("paintings/" + filename + ".png"));
	status.say("Beautiful!!!!!!!!!! Painting saved @ \"" + paintingsFolder.getAbsolutePath() + "\\" + filename + ".png" + "\" :D",
		StatusBar::UrgencyLevel::URGENCY_LEVEL_PARTY);
}

//--------------------------------------------------------------
void ofApp::showLoadPaintingModal() {
	loadPaintingModal.window.toggleVisible(true);
	loadPaintingModal.loadPaintingDirectory();
}

//--------------------------------------------------------------
void ofApp::loadPainting(int& i) {
	/// First check if a modal is active
	/*if (window.isVisible()) return;
	else if (loadPaintingModal.isVisible()) return;*/

	string filename = loadPaintingModal.getSelectedFilename();
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

	///Set canvas to painting size
	resizeCanvas(img.getWidth(), img.getHeight());

	ofPixels p;
	img.getTexture().readToPixels(p);

	mainCanvasFbo.begin();
	ofClear(255, 255, 255, 0);
	mainCanvasFbo.end();
	mainCanvasFbo.getTexture().loadData(p);

	loadPaintingModal.resetFilename();
	status.say("Painting loaded!");
}

//--------------------------------------------------------------
void ofApp::saveBrushBtnAction() {
	/// First check if a modal is currently active
	if (canvasDimsModal.window.isVisible()) return;
	else if (loadPaintingModal.window.isVisible()) return;

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
	brushMenuFbos[selectedBrush].getTextureReference().loadData(p);
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
	/// Read every pixel of brushMenuFbos[brushId] into pix
	std::cout << "Loading brush " << brushId << endl;
	ofPixels pix, biggifiedPix;
	brushCanvasFbo.getTexture().setTextureMinMagFilter(GL_NEAREST, GL_NEAREST);
	brushMenuFbos[brushId].getTexture().readToPixels(pix);

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
				if (xx >= brushMenuRects[0].getWidth()) xx = 0;
			}
			biggifiedPix.setColor(x, y, pix.getColor(xx, yy));
		}
	}

	brushCanvasFbo.getTextureReference().setTextureMinMagFilter(GL_NEAREST, GL_NEAREST);
	brushCanvasFbo.getTextureReference().loadData(biggifiedPix);

	status.say("Brush loaded!");
	updateBrush();
}

//--------------------------------------------------------------
void ofApp::loadBrushesFromFile() {
	int i = 0;
	for (auto fbo : brushMenuFbos) {
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
	result.x = (int)(result.x / gridSize) * gridSize;
	result.y = (int)(result.y / gridSize) * gridSize;
	return result;
}

//--------------------------------------------------------------
int ofApp::xPosInGrid(int gridSize, int x)
{
	x = (int)(x / gridSize) * gridSize;
	return x;
}

//--------------------------------------------------------------
int ofApp::yPosInGrid(int gridSize, int y)
{
	y = (int)(y / gridSize) * gridSize;
	return y;
}

//--------------------------------------------------------------
void ofApp::makeMainCanvasBg() {
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
}

//--------------------------------------------------------------
void ofApp::makeBrushCanvasBg()
{
	/// ----- Brush canvas

	brushCanvasBgFbo.begin();
	bool columnStartsGray = false;
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
	loadPaintingModal.keyPressed(key);
	canvasDimsModal.keyPressed(key);
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key) {
}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y) {
	int savedBrushRectX = x + (brushMenuRects[0].getWidth() / 2);
	int savedBrushRectY = y + (brushMenuRects[0].getHeight() / 2);

	for (int i = 0; i < brushMenuRects.size(); i++) {
		if (brushMenuRects[i].inside(savedBrushRectX, savedBrushRectY)) {
			hoveredBrush = i;
			return;
		}
		else hoveredBrush = -1;
	}

	/// ----- Scrollbars
	scrollbar.mouseMoved(x, y);
}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button) {
	if (canvasDimsModal.window.isVisible()) return;
	else if (loadPaintingModal.window.isVisible()) return;

	/// --- Scrollbars
	scrollbar.mouseDragged(x, y, canvasScrollX, canvasScrollY);
	if (scrollbar.isSelectedX()) return;
	else if (scrollbar.isSelectedY()) return;

	bPaintingInBrushCanvas = brushCanvasRect.inside(x, y) ? 1 : 0;
	bPaintingInMainCanvas = mainCanvasRect.inside(x, y) ? 1 : 0;
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button) {
	/// ----- Modals
	if (canvasDimsModal.window.isVisible()) {
		canvasDimsModal.window.mousePressed(x, y);
		return;
	}
	else if (loadPaintingModal.window.isVisible()) {
		loadPaintingModal.window.mousePressed(x, y);
		loadPaintingModal.mousePressed(x, y);
		return;
	}

	/// --- Scrollbars
	scrollbar.mousePressed(x, y, canvasScrollX, canvasScrollY);
	if (scrollbar.isSelectedX()) return;
	else if (scrollbar.isSelectedY()) return;

	/// ----- Canvas painting
	bPaintingInBrushCanvas = brushCanvasRect.inside(x, y) ? 1 : 0;
	bPaintingInMainCanvas = mainCanvasRect.inside(x, y) ? 1 : 0;
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button) {
	bPaintingInBrushCanvas = false;
	bPaintingInMainCanvas = false;

	/// ----- Modals
	if (canvasDimsModal.window.isVisible()) {
		canvasDimsModal.window.mouseReleased(x, y);
		return;
	}
	else if (loadPaintingModal.window.isVisible()) {
		loadPaintingModal.window.mouseReleased(x, y);
		loadPaintingModal.mouseReleased(x, y);
		return;
	}

	/// ----- Brush selection
	if (!scrollbar.isSelectedX() && !scrollbar.isSelectedY()) {
		int savedBrushRectX = x + (brushMenuRects[0].getWidth() / 2);
		int savedBrushRectY = y + (brushMenuRects[0].getHeight() / 2);

		for (int i = 0; i < brushMenuRects.size(); i++) {
			if (brushMenuRects[i].inside(savedBrushRectX, savedBrushRectY)) {
				selectedBrush = i;
				loadBrush(i);
				return;
			}
		}
	}

	/// --- Scrollbars
	scrollbar.mouseReleased();
}

//--------------------------------------------------------------
void ofApp::mouseScrolled(int x, int y, float scrollX, float scrollY) {
	int speed = 10;

	if (canvasContainer.inside(x, y)) {
		if (mainCanvasRect.getHeight() > canvasContainer.getHeight()) { /// if canvas is bigger than max canvas size
			canvasScrollY += scrollY * speed;

			int maxScrollY = (mainCanvasRect.getHeight() - canvasContainerFbo.getHeight()) * -1;
			if (canvasScrollY > 0) canvasScrollY = 0;
			else if (canvasScrollY < maxScrollY) canvasScrollY = maxScrollY;
		}

		if (mainCanvasRect.getWidth() > canvasContainer.getWidth()) { /// if canvas is bigger than max canvas size
			canvasScrollX += scrollX * speed;

			int maxScrollX = (mainCanvasRect.getWidth() - canvasContainerFbo.getWidth()) * -1;
			if (canvasScrollX > 0) canvasScrollX = 0;
			else if (canvasScrollX < maxScrollX) canvasScrollX = maxScrollX;
		}

		scrollbar.mouseScrolled(canvasScrollX, canvasScrollY);
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
	//cout << "Window resized." << endl;

	canvasContainerMaxSz = {
		ofGetWidth() - windowMargin * 2 - scrollbar.getSize(),
		ofGetHeight() - (windowMargin * 4) - brushCanvasRect.getHeight() - status.getHeight() - 100 };

	/// Recheck if canvas.height is > canvasContainerMaxSz

	if (mainCanvasRect.getHeight() <= canvasContainerMaxSz.y) {
		canvasContainer.height = mainCanvasRect.getHeight();
		isCanvasTooBigY = false;
	}
	else if (mainCanvasRect.getHeight() > canvasContainerMaxSz.y) {
		canvasContainer.height = canvasContainerMaxSz.y;
		isCanvasTooBigY = true;
	}

	if (mainCanvasRect.getWidth() <= canvasContainerMaxSz.x) {
		canvasContainer.width = mainCanvasRect.getWidth();
		isCanvasTooBigX = false;
	}
	else if (mainCanvasRect.getWidth() > canvasContainerMaxSz.x) {
		canvasContainer.width = canvasContainerMaxSz.x;
		isCanvasTooBigX = true;
	}

	canvasContainerFbo.allocate(canvasContainer.getWidth(), canvasContainer.getHeight(), GL_RGBA);
	canvasContainerFbo.begin();
	ofClear(255, 255, 255, 0);
	canvasContainerFbo.end();

	canvasScrollX = 0;
	canvasScrollY = 0;
	scrollbar.setup(canvasContainer, mainCanvasRect);
	setBrushCanvasRect();
	setBrushMenuPos();
	setGuiPos();

	/// Reset scrollbar length
	status.setLength(canvasContainer.getWidth() - fpsTxtLen);

	/// Reset modal center pos
	canvasDimsModal.resetCenterPos();
	loadPaintingModal.resetCenterPos();
}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg) {
}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo) {
}
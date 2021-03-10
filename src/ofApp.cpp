#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup() {
	ofDisableAntiAliasing();
	ofLogToConsole();

	mainCanvasPos = { windowMargin + (mainCanvasSize.x / 2),
						(windowMargin * 2) + (mainCanvasSize.y / 2) };
	brushCanvasPos = { windowMargin + (brushCanvasDisplaySize.y / 2),
						 (windowMargin * 2) + mainCanvasSize.y + windowMargin + (brushCanvasDisplaySize.y / 2) };

	mainCanvasRect.set(mainCanvasPos.x - (mainCanvasSize.x / 2),
		mainCanvasPos.y - (mainCanvasSize.y / 2),
		mainCanvasSize.x,
		mainCanvasSize.y);
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

	/// TODO: Load brushes from file

	/// ------ GUI

	colorPanelPos = { brushCanvasRect.getRight() + windowMargin,
		brushCanvasRect.getTop() };

	saveBrushBtn.addListener(this, &ofApp::updateBrush);
	clearBrushBtn.addListener(this, &ofApp::clearBrushCanvas);

	colorPanel.setup();
	colorPanel.setPosition(colorPanelPos);

	colorPanel.add(brushPanelTitle.setup(brushPanelTitleStr));
	colorPanel.add(saveBrushBtn.setup(saveBrushButtonTxt));
	colorPanel.add(colorPreview.setup(colorLabel));
	colorPanel.add(red.setup("<-> R", 0, 0, 255));
	colorPanel.add(green.setup("<-> G", 0, 0, 255));
	colorPanel.add(blue.setup("<-> B", 0, 0, 255));
	colorPanel.add(erase.setup("< Eraser", false));
	colorPanel.add(clearBrushBtn.setup(clearBrushBtnTxt));

	ofColor bgCol = ofColor::lightGray;

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
	clearBrushBtn.setBackgroundColor(bgCol);
	clearBrushBtn.setTextColor(ofColor::black);
	clearBrushBtn.setFillColor(ofColor::black);
	clearBrushBtn.setBorderColor(ofColor::black);
} /// end setup

//--------------------------------------------------------------
void ofApp::exit() {
	saveBrushBtn.removeListener(this, &ofApp::updateBrush);
	clearBrushBtn.removeListener(this, &ofApp::clearBrushCanvas);
}

//--------------------------------------------------------------
void ofApp::update() {
	drawCol = ofColor(red, green, blue, erase ? 0 : 255);
	//colorPanel.setHeaderBackgroundColor(drawCol);
	colorPreview.setBackgroundColor(drawCol);
}

//--------------------------------------------------------------
void ofApp::draw() {
	//ofBackground(ofColor::lightGray);
	ofSetColor(ofColor::red);
	ofDrawBitmapString("FPS: " + ofToString(ofGetFrameRate()), windowMargin, 15);

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

	/// ----- Saved brushes

	for (int i = 0; i < savedBrushRects.size(); i++) {
		/// Border
		ofSetColor(canvasBorderCol);
		ofDrawRectangle(savedBrushRects[i].getPosition().x,
			savedBrushRects[i].getPosition().y,
			savedBrushRects[i].getWidth() + 2,
			savedBrushRects[i].getHeight() + 2);
		ofSetColor(ofColor::white);
		ofDrawRectangle(savedBrushRects[i]);
		//savedBrushFbos[i].draw(savedBrushRects[i]);
	}

	savedBrushFbos[0].draw(savedBrushRects[0]);

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

	colorPanel.draw();
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
}

//--------------------------------------------------------------
void ofApp::updateMainCanvas() {
	int x = ofGetMouseX();
	int y = ofGetMouseY();

	mainCanvasFbo.begin();
	brush.draw(x + (brushCanvasMagnify / 2), y - brushCanvasMagnify, 26, 26);
	mainCanvasFbo.end();
}

//--------------------------------------------------------------
void ofApp::updateBrush() {
	ofPixels pix, culledPix;

	brushCanvasFbo.getTexture().setTextureMinMagFilter(GL_NEAREST, GL_NEAREST);
	brushCanvasFbo.getTexture().readToPixels(pix);
	culledPix.allocate(brush.getWidth(), brush.getHeight(), pix.getImageType());

	for (int y = 0; y < brushCanvasFbo.getTexture().getWidth(); y += brushCanvasMagnify) { // Read as horizontal rows
		for (int x = 0; x < brushCanvasFbo.getTexture().getHeight(); x += brushCanvasMagnify) {
			culledPix.setColor(x / brushCanvasMagnify, y / brushCanvasMagnify, pix.getColor(x, y));
		}
	}

	brush.getTextureReference().setTextureMinMagFilter(GL_NEAREST, GL_NEAREST);
	brush.getTextureReference().loadData(culledPix);

	cout << "Brush updated!" << endl;

	saveBrush(culledPix);
}

void ofApp::saveBrush(ofPixels& p) {
	savedBrushFbos[0].getTextureReference().loadData(p);
	cout << "Brush saved!" << endl;
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

	mainCanvasBgFbo.begin();
	bool columnStartsGray = false;
	for (int x = 0; x < mainCanvasSize.x; x += 10) {
		if (x % 20 == 0 || x == 0) columnStartsGray = true;
		else columnStartsGray = false;
		for (int y = 0; y < mainCanvasSize.y; y += 10) {
			if (y % 20 == 0 || y == 0) {
				if (columnStartsGray) ofSetColor(ofColor::lightGray);
				else ofSetColor(ofColor::white);
			}
			else {
				if (columnStartsGray) ofSetColor(ofColor::white);
				else ofSetColor(ofColor::lightGray);
			}
			//ofSetColor(ofColor::green);
			ofDrawRectangle(x, y, 10, 10);
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
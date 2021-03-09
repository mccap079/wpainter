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

	/// ------ GUI

	updateBrushButton.setup(
		{ brushCanvasRect.getRight() + windowMargin,
		brushCanvasRect.getTop() },
		updateBrushButtonTxt);
	ofAddListener(updateBrushButton.onRelease, this, &ofApp::updateBrush);

	colorPanel.setup();
	colorPanel.setPosition(
		{ brushCanvasRect.getRight() + windowMargin,
		updateBrushButton.getBottom() + windowMargin, });
	colorPanel.setSize(updateBrushButton.getWidth(), colorPanel.getHeight());

	colorPanel.add(red.setup("R", 0, 0, 255));
	colorPanel.add(green.setup("G", 0, 0, 255));
	colorPanel.add(blue.setup("B", 0, 0, 255));
	red.setSize(colorPanel.getWidth(), red.getHeight());
	green.setSize(colorPanel.getWidth(), green.getHeight());
	blue.setSize(colorPanel.getWidth(), blue.getHeight());
} /// end setup

//--------------------------------------------------------------
void ofApp::update() {
	drawCol = ofColor(red, green, blue);
	colorPanel.setHeaderBackgroundColor(drawCol);
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

	/// ----- Brush paint

	if (bPaintingInBrushCanvas) updateBrushCanvas();
	brushCanvasFbo.draw(brushCanvasRect.getLeft() + (brushCanvasDisplaySize.x / 2),
		brushCanvasRect.getTop() + (brushCanvasDisplaySize.x / 2));

	/// ----- Main canvas paint

	if (bPaintingInMainCanvas) updateMainCanvas();
	mainCanvasFbo.draw(mainCanvasRect.getCenter().x,
		mainCanvasRect.getCenter().y);

	/// ----- Draw UI

	updateBrushButton.draw();
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

	if (p != prevBrushCanvasGridPoint) {
		ofSetColor(drawCol);
		brushCanvasFbo.begin();
		ofDrawRectangle(p.x + (brushCanvasMagnify / 2),
			p.y + (brushCanvasMagnify / 2),
			brushCanvasMagnify,
			brushCanvasMagnify);
		brushCanvasFbo.end();
	}
	prevBrushCanvasGridPoint = p;
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
void ofApp::updateBrush(int& i) {
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
	updateBrushButton.mousePressed(x, y);
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button) {
	bPaintingInBrushCanvas = false;
	bPaintingInMainCanvas = false;
	updateBrushButton.mouseReleased(x, y);
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
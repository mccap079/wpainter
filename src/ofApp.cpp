#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup() {
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

	drawCol = ofColor::red;
	prevBrushCanvasGridPoint = { -100,-100 };

	brushCanvasFbo.allocate(brushCanvasDisplaySize.x, brushCanvasDisplaySize.y, GL_RGBA);
	brushCanvasFbo.begin();
	ofClear(255, 255, 255, 0);
	brushCanvasFbo.end();

	brushFbo.allocate(brushCanvasComputeSize.x, brushCanvasComputeSize.y, GL_RGBA);
	brushFbo.begin();
	ofClear(255, 255, 255, 0);
	brushFbo.end();

	mainCanvasFbo.allocate(mainCanvasSize.x, mainCanvasSize.y, GL_RGBA);
	mainCanvasFbo.begin();
	ofClear(255, 255, 255, 0);
	mainCanvasFbo.end();

	updateBrushButtonPos = { brushCanvasRect.getRight() + windowMargin,
		brushCanvasRect.getTop() };

	updateBrushButton.set(updateBrushButtonPos.x,
		updateBrushButtonPos.y,
		updateBrushButtonSize.x,
		updateBrushButtonSize.y);

	brush.allocate(brushCanvasComputeSize.x, brushCanvasComputeSize.y, OF_IMAGE_COLOR_ALPHA);
	int i = 0;
	while (i < brush.getPixelsRef().size()) {
		brush.getPixelsRef()[i] = 255;
		i++;
	}
}

//--------------------------------------------------------------
void ofApp::update() {
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
	//ofDrawRectangle(mainCanvasPos.x, mainCanvasPos.y, mainCanvasSize.x, mainCanvasSize.y);
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
	//ofDrawRectangle(brushCanvasPos.x, brushCanvasPos.y, brushCanvasSize.x, brushCanvasSize.y);
	brushCanvasBgFbo.draw(brushCanvasPos.x,
		brushCanvasPos.y,
		brushCanvasDisplaySize.x,
		brushCanvasDisplaySize.y);

	/// ----- Draw UI

	ofSetColor(ofColor::black);
	ofDrawRectangle(updateBrushButton.getCenter().x,
		updateBrushButton.getCenter().y,
		updateBrushButtonSize.x + 3,
		updateBrushButtonSize.y + 3);
	ofSetColor(ofColor::darkGray);
	ofDrawRectangle(updateBrushButton.getCenter(), updateBrushButtonSize.x, updateBrushButtonSize.y);
	ofSetColor(ofColor::black);
	ofDrawBitmapString(updateBrushButtonTxt, updateBrushButton.getLeft() + 7, updateBrushButton.getBottom() - 7);

	ofSetColor(ofColor::red);
	ofDrawRectangle(updateBrushButton);

	/// ----- Brush paint

	if (bPaintingInBrushCanvas) updateBrushCanvas();
	brushCanvasFbo.draw(brushCanvasRect.getLeft() + (brushCanvasDisplaySize.x / 2),
		brushCanvasRect.getTop() + (brushCanvasDisplaySize.x / 2));

	/// ----- Main canvas paint

	if (bPaintingInMainCanvas) updateMainCanvas();
	mainCanvasFbo.draw(mainCanvasRect.getCenter().x,
		mainCanvasRect.getCenter().y);
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
	/*ofPixels pix;
	pix =
		pix.resize(25, 25, OF_INTERPOLATE_NEAREST_NEIGHBOR);
	brushCanvasFbo.getTextureReference().setFromPixels(pix);*/
	brush.draw(x + (brushCanvasMagnify / 2), y - brushCanvasMagnify);
	mainCanvasFbo.end();
}

//--------------------------------------------------------------
void ofApp::updateBrush() {
	ofFloatPixels pix;
	ofFloatPixels culledPix;
	culledPix.allocate(25, 25, OF_IMAGE_COLOR_ALPHA);
	brushCanvasFbo.getTextureReference().readToPixels(pix);
	int i = 0;
	for (int p = 0; p < pix.size(); p += brushCanvasMagnify) {
		brush.getPixelsRef()[i] = pix[p];
		i++;
	}
	//brushFbo.getTextureReference().loadData(culledPix);
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
			//ofSetColor(ofColor::green);
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
	if (updateBrushButton.inside(x, y)) {
		updateBrush();
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
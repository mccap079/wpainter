#pragma once

#include "ofMain.h"
#include "Button.h"
#include "ofxGui.h"

class ofApp : public ofBaseApp {
public:
	void setup();
	void update();
	void draw();
	void exit();

	void keyPressed(int key);
	void keyReleased(int key);
	void mouseMoved(int x, int y);
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased(int x, int y, int button);
	void mouseEntered(int x, int y);
	void mouseExited(int x, int y);
	void windowResized(int w, int h);
	void dragEvent(ofDragInfo dragInfo);
	void gotMessage(ofMessage msg);

	/// Canvas stuff

	void makeCanvasBg();
	ofFbo mainCanvasBgFbo, brushCanvasBgFbo;

	const int windowMargin = 10;
	const glm::vec2 mainCanvasSize = { 960, 600 };
	const int brushCanvasMagnify = 8;
	const glm::vec2 brushCanvasComputeSize = { 25, 25 };
	const glm::vec2 brushCanvasDisplaySize = { brushCanvasComputeSize.x * brushCanvasMagnify,
											   brushCanvasComputeSize.y * brushCanvasMagnify };
	const ofColor canvasBgCol = ofColor::white;
	const ofColor canvasBorderCol = ofColor::black;

	ofRectangle mainCanvasRect, brushCanvasRect;
	glm::vec2 mainCanvasPos, brushCanvasPos;

	/// Brush drawing stuff

	void updateBrushCanvas();
	void updateBrush();
	void clearBrushCanvas();

	glm::vec2 posInGrid(int gridSize, int x, int y);
	glm::vec2 prevBrushCanvasGridPoint;
	ofFbo brushCanvasFbo;
	ofImage brush;

	bool bPaintingInBrushCanvas = false;

	/// Canvas drawing stuff

	void updateMainCanvas();

	ofFbo mainCanvasFbo;

	bool bPaintingInMainCanvas = false;

	/// Color selection
	const string brushPanelTitleStr = "<<< BRUSH SETTINGS";
	const string updateBrushButtonTxt = "< Update brush";
	const string saveBrushButtonTxt = "< Save brush";
	const string colorLabel = "Set color:";
	const string clearBrushBtnTxt = "< Clear brush canvas";

	ofColor drawCol = ofColor::black;
	ofxPanel colorPanel;
	ofxLabel brushPanelTitle;
	ofxButton saveBrushBtn;
	ofxLabel colorPreview;
	ofxFloatSlider red, green, blue;
	ofxToggle erase;
	ofxButton clearBrushBtn;
	glm::vec2 colorPanelPos;

	/// Saved brushes collection

	void saveBrush(ofPixels& p);

	const int numSavedBrushes = 30;
	glm::vec2 savedBrushesPos;
	const int savedBrushesPadding = windowMargin;
	int selectedBrush = 0;
	vector<ofFbo> savedBrushFbos;
	vector<ofRectangle> savedBrushRects;
};

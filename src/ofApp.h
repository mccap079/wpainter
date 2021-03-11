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
	void clearMainCanvas();
	void fillMainCanvas();

	ofFbo mainCanvasFbo;

	bool bPaintingInMainCanvas = false;

	/// Brush GUI
	const string brushPanelTitleStr = "<<< BRUSH SETTINGS";
	const string updateBrushButtonTxt = "< Update brush";
	const string saveBrushButtonTxt = "< Save brush";
	const string colorLabel = "Set color:";
	const string clearBrushBtnTxt = "< Clear brush canvas";

	ofColor drawCol = ofColor::black;
	ofxPanel brushPanel;
	ofxLabel brushPanelTitle;
	ofxButton saveBrushBtn;
	ofxLabel colorPreview;
	ofxFloatSlider red, green, blue;
	ofxToggle erase;
	ofxButton clearBrushBtn;
	glm::vec2 colorPanelPos;

	/// Main canvas GUI
	const string canvasPanelTitleStr = "^^^ CANVAS SETTINGS";
	const string savePaintingBtnTxt = "< Export painting";
	const string loadPaintingBtnTxt = "< Import painting";
	const string canvasBgLabelTxt = "Fill canvas color:";
	const string setCanvasBgTxt = "< Set fill color";
	const string clearCanvasBtnTxt = "< Clear canvas";
	ofxPanel canvasPanel;
	glm::vec2 canvasPanelPos;
	ofxLabel canvasPanelTitle;
	ofxButton savePaintingBtn;
	ofxButton loadPaintingBtn;
	ofxButton setCanvasBgBtn;
	ofxLabel bgColorLabel;
	ofxFloatSlider fillRed, fillGreen, fillBlue;
	ofxButton clearCanvasBtn;

	/// Saved brushes collection

	void saveBrush(ofPixels& p);
	void loadBrush(int& brushId);
	void saveBrushToFile(ofPixels& p);
	void loadBrushesFromFile();

	const int numSavedBrushes = 30;
	glm::vec2 savedBrushesPos;
	const int savedBrushesPadding = windowMargin;
	int selectedBrush = 0;
	int hoveredBrush = -1;
	vector<ofFbo> savedBrushFbos;
	vector<ofRectangle> savedBrushRects;
	ofColor selectionHighlight;
};

/// When brush is selected from collection, it should automatically be set to `brush`
/// 
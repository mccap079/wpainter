#pragma once

#include "ofMain.h"
#include "Button.h"
#include "ofxGui.h"
#include "ScrollBar.h"
#include "BrushAnchor.h"

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
	void setBrushAnchor_topLeft(bool& b);
	void setBrushAnchor_center(bool& b);

	glm::vec2 posInGrid(int gridSize, int x, int y);
	glm::vec2 prevBrushCanvasGridPoint;
	ofFbo brushCanvasFbo;
	ofImage brush;

	bool bPaintingInBrushCanvas = false;

	BrushAnchor brushAnchor;

	/// Canvas drawing stuff

	void updateMainCanvas();
	void clearMainCanvas();
	void fillMainCanvas();
	void savePainting();
	void getTextFieldContent(string& filename);
	void loadPainting();

	ofFbo mainCanvasFbo;

	bool bPaintingInMainCanvas = false;

	/// Brush GUI
	const string brushPanelTitleStr = "<<< BRUSH SETTINGS";
	const string updateBrushButtonTxt = "< Update brush";
	const string saveBrushButtonTxt = "< Save brush";
	const string colorLabel = "Set color:";
	const string setAnchorLabelTxt = "Set anchor:";
	const string setAnchorBtnTxt_topLeft = "< Top left";
	const string setAnchorBtnTxt_center = "< Center";
	const string startOverLabelTxt = "Start over:";
	const string clearBrushBtnTxt = "< Clear brush canvas";

	ofColor drawCol = ofColor::black;
	ofxPanel brushPanel;
	ofxLabel brushPanelTitle;
	ofxButton saveBrushBtn;
	ofxLabel colorPreview;
	ofxFloatSlider red, green, blue;
	ofxToggle erase;
	ofxLabel setAnchorLabel;
	ofxToggle setAnchorBtn_topLeft;
	ofxToggle setAnchorBtn_center;
	ofxLabel startOverLabel_brushCanvas;
	ofxButton clearBrushBtn;
	glm::vec2 colorPanelPos;
	bool bAnchorCenter = false;

	/// Main canvas GUI
	const string canvasPanelTitleStr = "^^^ CANVAS SETTINGS";
	const string savePaintingBtnTxt = "< Export painting";
	const string loadPaintingLabelTxt = "Import painting:";
	const string loadPaintingBtnTxt = "< Import";
	const string canvasBgLabelTxt = "Fill canvas color:";
	const string setCanvasBgTxt = "< Fill canvas";
	const string clearCanvasBtnTxt = "< Clear canvas";
	ofxPanel canvasPanel;
	glm::vec2 canvasPanelPos;
	ofxLabel canvasPanelTitle;
	ofxButton savePaintingBtn;
	ofxLabel loadPaintingLabel;
	ofxTextField loadPaintingField;
	ofxButton loadPaintingBtn;
	ofxButton setCanvasBgBtn;
	ofxLabel bgColorLabel;
	ofxFloatSlider fillRed, fillGreen, fillBlue;
	ofxLabel startOverLabel_mainCanvas;
	ofxButton clearCanvasBtn;

	string loadPaintingFilename;

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
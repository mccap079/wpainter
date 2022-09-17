#pragma once

#include "ofMain.h"
#include "Button.h"
#include "ofxGui.h"
#include "ofxGuiExtended2.h"
#include "ScrollBar.h"
#include "BrushAnchor.h"
#include "StatusBar.h"
#include "ModalWindow.h"
#include "SetCanvasDimsModal.h"
#include "LoadPaintingModal.h"

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
	void mouseScrolled(int x, int y, float scrollX, float scrollY);
	void mouseEntered(int x, int y);
	void mouseExited(int x, int y);
	void windowResized(int w, int h);
	void dragEvent(ofDragInfo dragInfo);
	void gotMessage(ofMessage msg);

	/// Canvas stuff

	void makeMainCanvasBg();
	void makeBrushCanvasBg();
	ofFbo mainCanvasBgFbo, brushCanvasBgFbo;

	const int windowMargin = 10;
	glm::vec2 mainCanvasSize = { 960, 580 };
	const int brushCanvasMagnify = 10;
	const glm::vec2 brushCanvasComputeSize = { 24, 24 }; ///TODO: Make 24, allow larger pngs to be loaded but crop them at 24
	const glm::vec2 brushCanvasDisplaySize = { brushCanvasComputeSize.x * brushCanvasMagnify,
											   brushCanvasComputeSize.y * brushCanvasMagnify };
	const ofColor canvasBgCol = ofColor::white;
	const ofColor canvasBorderCol = ofColor::black;

	ofRectangle mainCanvasRect, brushCanvasRect;
	glm::vec2 mainCanvasPos, brushCanvasPos;

	/// Canvas scrolling
	ScrollBar scrollbar;
	glm::vec2 canvasContainerMaxSz;
	ofRectangle canvasContainer;
	ofFbo canvasContainerFbo;
	int maxTexSz = 0;
	int canvasScrollX, canvasScrollY;
	bool isCanvasTooBigX, isCanvasTooBigY;

	/// Brush drawing stuff

	void setBrushCanvasRect();
	void updateBrushCanvas();
	void saveBrushBtnAction();
	void updateBrush();
	void clearBrushCanvas();
	void setBrushAnchor_topLeft(bool& b);
	void setBrushAnchor_center(bool& b);

	glm::vec2 posInGrid(int gridSize, int x, int y);
	int yPosInGrid(int gridSize, int y);
	int xPosInGrid(int gridSize, int x);
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

	ofFbo mainCanvasFbo;

	bool bPaintingInMainCanvas = false;

	/// ----- Guis

	void setupGui();
	void setGuiPos();
	void drawGuis();

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
	glm::vec2 brushPanelPos;
	bool bAnchorCenter = false;

	/// Main canvas GUI
	const string canvasPanelTitleStr = "^^^ CANVAS SETTINGS";
	const string savePaintingBtnTxt = "< Export painting";
	const string loadPaintingLabelTxt = "Import painting:";
	const string loadPaintingBtnTxt = "< Import";
	const string setSizeLabelTxt = "Set canvas size:";
	const string setSizeBtnTxt = "< Set size...";
	const string canvasBgLabelTxt = "Fill canvas color:";
	const string setCanvasBgTxt = "< Fill canvas";
	const string clearCanvasBtnTxt = "< Clear canvas";
	ofxPanel canvasPanel;
	glm::vec2 canvasPanelPos;
	ofxLabel canvasPanelTitle;
	ofxButton savePaintingBtn;
	ofxLabel loadPaintingLabel;
	ofxButton loadPaintingBtn;
	ofxLabel setSizeLabel;
	ofxButton setSizeBtn;
	ofxButton setCanvasBgBtn;
	ofxLabel bgColorLabel;
	ofxFloatSlider fillRed, fillGreen, fillBlue;
	ofxLabel startOverLabel_mainCanvas;
	ofxButton clearCanvasBtn;

	/// Main canvas GUI pt 2
	const string canvas2PanelTitleStr = "CANVAS SETTINGS CONT.";
	const string toggleGridLabelTxt = "Canvas grid:";
	const string toggleGridBtnTxt = "< Toggle grid mode";
	ofxPanel canvas2Panel;
	glm::vec2 canvas2PanelPos;
	ofxLabel canvas2PanelTitle;
	ofxLabel toggleGridLabel;
	ofxToggle toggleGridBtn;

	/// Saved brushes collection

	void saveBrush(ofPixels& p);
	void loadBrush(int& brushId);
	void saveBrushToFile(ofPixels& p);
	void loadBrushesFromFile();
	void setBrushMenuPos();
	void addNewBrushMenuSlot();

	ofRectangle brushMenuRect;
	int numSavedBrushes = 30;
	glm::vec2 brushMenuPos;
	const int brushMenuPadding = windowMargin;
	int selectedBrush = 0;
	int hoveredBrush = -1;
	vector<ofFbo> brushMenuFbos;
	vector<ofRectangle> brushMenuRects;
	ofColor selectionHighlight;

	/// Status bars

	StatusBar status;
	int fpsTxtLen, statusBarMinLen;
	const string fpsLabel = "FPS: ";

	/// Set canvas size modal
	void ShowSetSizeModal();
	void setCanvasDims(int& i);
	void resizeCanvas(int w, int h);
	CanvasDimsModal canvasDimsModal;

	///Load painting modal
	void showLoadPaintingModal();
	void loadPainting(int& i);

	LoadPaintingModal loadPaintingModal;
};
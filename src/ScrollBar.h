#pragma once
#include "ofMain.h"

class ScrollBar {
public:
	//--------------------------------------------------------------
	void setup(ofRectangle& canvasContainer, ofRectangle& mainCanvasRect) {
		/// <----->
		scrollbarContainerX.set(
			canvasContainer.getBottomLeft().x,
			canvasContainer.getBottomLeft().y + scrollbarMargin,
			canvasContainer.getWidth(),
			sz
		);

		float normalizedPercentWidth = canvasContainer.getWidth() / mainCanvasRect.getWidth();

		handleX.setSize(ofMap(normalizedPercentWidth, 0, 1, 0, scrollbarContainerX.getWidth() - (handleMargin * 2)),
			sz - (handleMargin * 2));

		/// ^
		/// |
		/// v
		scrollbarContainerY.set(
			canvasContainer.getTopRight().x + scrollbarMargin,
			canvasContainer.getTopRight().y,
			sz,
			canvasContainer.getHeight()
		);

		float normalizedPercentHeight = canvasContainer.getHeight() / mainCanvasRect.getHeight();

		handleY.setSize(sz - (handleMargin * 2),
			ofMap(normalizedPercentHeight, 0, 1, 0, scrollbarContainerY.getHeight() - (handleMargin * 2)));
	}

	//--------------------------------------------------------------
	void update(ofRectangle& mainCanvasRect, int canvasScrollX, int canvasScrollY) {
		int scrollPosX = ofMap(-canvasScrollX, 0, mainCanvasRect.getWidth(), 0, scrollbarContainerX.getWidth() - handleMargin);
		handleX.setPosition(
			scrollbarContainerX.getLeft() + scrollPosX + handleMargin,
			scrollbarContainerX.getTop() + handleMargin);

		int scrollPosY = ofMap(-canvasScrollY, 0, mainCanvasRect.getHeight(), 0, scrollbarContainerY.getHeight() - handleMargin);
		handleY.setPosition(
			scrollbarContainerY.getLeft() + handleMargin,
			scrollbarContainerY.getTop() + scrollPosY + handleMargin);
	}

	//--------------------------------------------------------------
	void draw(bool& isCanvasTooBigX, bool& isCanvasTooBigY) {
		if (isCanvasTooBigX) {
			/// ----- Draw scrollbar along bottom of canvas
			/// Border
			ofSetColor(ofColor::black);
			ofFill();
			ofDrawRectangle(scrollbarContainerX.getPosition().x - 1,
				scrollbarContainerX.getPosition().y - 1,
				scrollbarContainerX.getWidth() + 2,
				scrollbarContainerX.getHeight() + 2);
			/// Container
			ofSetColor(containerCol);
			ofDrawRectangle(scrollbarContainerX);
			/// Handle border
			ofSetColor(handleBorderCol);
			ofDrawRectangle(handleX.getPosition().x - 1,
				handleX.getPosition().y - 1,
				handleX.getWidth() + 2,
				handleX.getHeight() + 2);
			/// Handle
			ofSetColor(handleCol);
			ofDrawRectangle(handleX);
		}
		if (isCanvasTooBigY) {
			/// ----- Draw scrollbar along right side of canvas
			/// Border
			ofSetColor(ofColor::black);
			ofFill();
			ofDrawRectangle(scrollbarContainerY.getPosition().x - 1,
				scrollbarContainerY.getPosition().y - 1,
				scrollbarContainerY.getWidth() + 2,
				scrollbarContainerY.getHeight() + 2);
			/// Container
			ofSetColor(containerCol);
			ofDrawRectangle(scrollbarContainerY);
			/// Handle border
			ofSetColor(handleBorderCol);
			ofDrawRectangle(handleY.getPosition().x - 1,
				handleY.getPosition().y - 1,
				handleY.getWidth() + 2,
				handleY.getHeight() + 2);
			/// Handle
			ofSetColor(handleCol);
			ofDrawRectangle(handleY);
		}
	}

	//--------------------------------------------------------------
	void mouseMoved(int x, int y) {
		if (handleX.inside(x, y)) {
			handleCol = handleCol_hover;
			handleBorderCol = handleBorderCol_hover;
		}
		else {
			handleCol = handleCol_default;
			handleBorderCol = handleBorderCol_default;
		}
	}

	//--------------------------------------------------------------
	void mousePressed(int x, int y) {
		if (handleX.inside(x, y)) {
			handleCol = handleCol_selected;
			handleBorderCol = handleBorderCol_selected;
		}
		else {
			handleCol = handleCol_default;
			handleBorderCol = handleBorderCol_default;
		}
	}

private:
	const int sz = 20;
	const int minHandleSz = 20;
	const int handleMargin = 2;
	const int scrollbarMargin = 4;
	const ofColor containerCol = ofColor::darkGray;
	const ofColor handleCol_default = ofColor::lightGray;
	const ofColor handleCol_hover = ofColor::white;
	const ofColor handleCol_selected = ofColor::black;
	ofColor handleCol = handleCol_default;
	const ofColor handleBorderCol_default = ofColor::black;
	const ofColor handleBorderCol_hover = ofColor::black;
	const ofColor handleBorderCol_selected = ofColor::white;
	ofColor handleBorderCol = handleBorderCol_default;
	ofRectangle scrollbarContainerX, scrollbarContainerY;
	ofRectangle handleX, handleY;
	ofRectangle topArrowBox, bottomArrowBox, leftArrowBox, rightArrowBox;
};
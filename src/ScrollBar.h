#pragma once
#include "ofMain.h"

class ScrollBar {
public:
	void setup(ofRectangle& canvasContainer, ofRectangle& mainCanvasRect) {
		/// <----->
		scrollbarContainerX.set(
			canvasContainer.getBottomLeft().x,
			canvasContainer.getBottomLeft().y,
			canvasContainer.getWidth(),
			sz
		);

		float normalizedPercentWidth = canvasContainer.getWidth() / mainCanvasRect.getWidth();

		handleX.set(
			scrollbarContainerX.getTopLeft().x,
			scrollbarContainerX.getTopLeft().y,
			ofMap(normalizedPercentWidth, 0, 1, minHandleSz, scrollbarContainerX.getWidth()),
			sz);

		/// ^
		/// |
		/// v
		scrollbarContainerY.set(
			canvasContainer.getTopRight().x,
			canvasContainer.getTopRight().y,
			sz,
			canvasContainer.getHeight()
		);

		float normalizedPercentHeight = canvasContainer.getHeight() / mainCanvasRect.getHeight();

		handleY.set(
			scrollbarContainerY.getTopLeft().x,
			scrollbarContainerY.getTopLeft().y,
			sz,
			ofMap(normalizedPercentHeight, 0, 1, minHandleSz, scrollbarContainerY.getHeight()));
	}

	void update(ofRectangle& mainCanvasRect, int canvasScrollX, int canvasScrollY) {
		int scrollPosX = ofMap(-canvasScrollX, 0, mainCanvasRect.getWidth(), 0, scrollbarContainerX.getWidth());
		handleX.setPosition(
			scrollbarContainerY.getLeft() + scrollPosX,
			scrollbarContainerX.getTop());
		//cout << "scrollPosX: " << scrollPosX << endl;

		int scrollPosY = ofMap(-canvasScrollY, 0, mainCanvasRect.getHeight(), 0, scrollbarContainerY.getHeight());
		handleY.setPosition(
			scrollbarContainerY.getLeft(),
			scrollbarContainerY.getTop() + scrollPosY);
	}

	void draw(bool& isCanvasTooBigX, bool& isCanvasTooBigY) {
		if (isCanvasTooBigX) {
			///draw scrollbar along bottom of canvas
			ofSetColor(ofColor::red);
			ofFill();
			ofDrawRectangle(scrollbarContainerX);
			ofSetColor(ofColor::blue);
			ofDrawRectangle(handleX);
		}
		if (isCanvasTooBigY) {
			///draw scrollbar along right side of canvas
			ofSetColor(ofColor::green);
			ofFill();
			ofDrawRectangle(scrollbarContainerY);
			ofSetColor(ofColor::blue);
			ofDrawRectangle(handleY);
		}
	}

private:
	const int sz = 20;
	const int minHandleSz = 20;
	ofRectangle scrollbarContainerX, scrollbarContainerY;
	ofRectangle handleX, handleY;
	ofRectangle topArrowBox, bottomArrowBox, leftArrowBox, rightArrowBox;
};
#pragma once
#include "ofMain.h"

class ScrollBar {
public:
	//--------------------------------------------------------------
	void setup(ofRectangle& canvasContainer, ofRectangle& mainCanvasRect) {
		m_canvasContainer = canvasContainer;
		m_mainCanvasRect = mainCanvasRect;
		/// <----->
		m_scrollbarContainer_x.set(
			canvasContainer.getBottomLeft().x,
			canvasContainer.getBottomLeft().y + m_scrollbarMargin,
			canvasContainer.getWidth(),
			m_sz
		);

		float normalizedPercentWidth = canvasContainer.getWidth() / m_mainCanvasRect.getWidth();

		m_handle_x.setSize(ofMap(normalizedPercentWidth, 0, 1, 0, m_scrollbarContainer_x.getWidth() - (m_handleMargin * 2)),
			m_sz - (m_handleMargin * 2));

		m_handle_x.setPosition(
			m_scrollbarContainer_x.getLeft() + m_handleMargin,
			m_scrollbarContainer_x.getTop() + m_handleMargin);

		/// ^
		/// |
		/// v
		m_scrollbarContainer_y.set(
			canvasContainer.getTopRight().x + m_scrollbarMargin,
			canvasContainer.getTopRight().y,
			m_sz,
			canvasContainer.getHeight()
		);
		
		float normalizedPercentHeight = canvasContainer.getHeight() / m_mainCanvasRect.getHeight();

		m_handle_y.setSize(m_sz - (m_handleMargin * 2),
			ofMap(normalizedPercentHeight, 0, 1, 0, m_scrollbarContainer_y.getHeight() - (m_handleMargin * 2)));

		m_handle_y.setPosition(
			m_scrollbarContainer_y.getLeft() + m_handleMargin,
			m_scrollbarContainer_y.getTop() + m_handleMargin);

		m_isHandleSelected_x = false;
		m_isHandleSelected_y = false;
	}

	//--------------------------------------------------------------
	void update(ofRectangle& canvasContainer, ofRectangle& mainCanvasRect) {
		m_canvasContainer = canvasContainer;
		m_mainCanvasRect = mainCanvasRect;
	}

	//--------------------------------------------------------------
	void draw(bool& isCanvasTooBigX, bool& isCanvasTooBigY) {
		if (isCanvasTooBigX) {
			/// ----- Draw scrollbar along bottom of canvas
			/// Border
			ofSetColor(ofColor::black);
			ofFill();
			ofDrawRectangle(m_scrollbarContainer_x.getPosition().x - 1,
				m_scrollbarContainer_x.getPosition().y - 1,
				m_scrollbarContainer_x.getWidth() + 2,
				m_scrollbarContainer_x.getHeight() + 2);
			/// Container
			ofSetColor(m_containerCol);
			ofDrawRectangle(m_scrollbarContainer_x);
			/// Handle border
			ofSetColor(m_handleBorderCol_x);
			ofDrawRectangle(m_handle_x.getPosition().x - 1,
				m_handle_x.getPosition().y - 1,
				m_handle_x.getWidth() + 2,
				m_handle_x.getHeight() + 2);
			/// Handle
			ofSetColor(m_handleCol_x);
			ofDrawRectangle(m_handle_x);
		}
		if (isCanvasTooBigY) {
			/// ----- Draw scrollbar along right side of canvas
			/// Border
			ofSetColor(ofColor::black);
			ofFill();
			ofDrawRectangle(m_scrollbarContainer_y.getPosition().x - 1,
				m_scrollbarContainer_y.getPosition().y - 1,
				m_scrollbarContainer_y.getWidth() + 2,
				m_scrollbarContainer_y.getHeight() + 2);
			/// Container
			ofSetColor(m_containerCol);
			ofDrawRectangle(m_scrollbarContainer_y);
			/// Handle border
			ofSetColor(m_handleBorderCol_y);
			ofDrawRectangle(m_handle_y.getPosition().x - 1,
				m_handle_y.getPosition().y - 1,
				m_handle_y.getWidth() + 2,
				m_handle_y.getHeight() + 2);
			/// Handle
			ofSetColor(m_handleCol_y);
			ofDrawRectangle(m_handle_y);
		}
	}

	//--------------------------------------------------------------
	void mouseMoved(int x, int y) {
		if (m_handle_x.inside(x, y)) {
			m_handleCol_x = m_handleCol_hover;
			m_handleBorderCol_x = m_handleBorderCol_hover;
		}
		else {
			m_handleCol_x = m_handleCol_default;
			m_handleBorderCol_x = m_handleBorderCol_default;
		}

		if (m_handle_y.inside(x, y)) {
			m_handleCol_y = m_handleCol_hover;
			m_handleBorderCol_y = m_handleBorderCol_hover;
		}
		else {
			m_handleCol_y = m_handleCol_default;
			m_handleBorderCol_y = m_handleBorderCol_default;
		}
	}

	//--------------------------------------------------------------
	void mousePressed(int x, int y, int& canvasScrollX, int& canvasScrollY) {
		if (m_handle_x.inside(x, y)) {
			m_handleCol_x = m_handleCol_selected;
			m_handleBorderCol_x = m_handleBorderCol_selected;
			m_isHandleSelected_x = true;
		}
		else {
			m_handleCol_x = m_handleCol_default;
			m_handleBorderCol_x = m_handleBorderCol_default;
			m_isHandleSelected_x = false;
		}

		if (m_handle_y.inside(x, y)) {
			m_handleCol_y = m_handleCol_selected;
			m_handleBorderCol_y = m_handleBorderCol_selected;
			m_isHandleSelected_y = true;
		}
		else {
			m_handleCol_y = m_handleCol_default;
			m_handleBorderCol_y = m_handleBorderCol_default;
			m_isHandleSelected_y = false;
		}

		if (m_scrollbarContainer_x.inside(x, y)) {
			int totalScrollAmt = (m_mainCanvasRect.getWidth() - m_canvasContainer.getWidth()) * -1;
			int pos = ofMap(x, m_scrollbarContainer_x.getLeft() + m_minHandleSz, m_scrollbarContainer_x.getRight() - m_minHandleSz, 0, totalScrollAmt);

			if (pos > 0) pos = 0;
			else if (pos < totalScrollAmt) pos = totalScrollAmt;

			moveHandleX(pos);

			canvasScrollX = pos;
		}

		if (m_scrollbarContainer_y.inside(x, y)) {
			int totalScrollAmt = (m_mainCanvasRect.getHeight() - m_canvasContainer.getHeight()) * -1;
			int pos = ofMap(y, m_scrollbarContainer_y.getTop() + m_minHandleSz, m_scrollbarContainer_y.getBottom() - m_minHandleSz, 0, totalScrollAmt);

			if (pos > 0) pos = 0;
			else if (pos < totalScrollAmt) pos = totalScrollAmt;

			moveHandleY(pos);

			canvasScrollY = pos;
		}
	}

	//--------------------------------------------------------------
	void mouseDragged(int x, int y, int& canvasScrollX, int& canvasScrollY) {
		if (m_isHandleSelected_x) {
			int totalScrollAmt = (m_mainCanvasRect.getWidth() - m_canvasContainer.getWidth()) * -1;
			int pos = ofMap(x, m_scrollbarContainer_x.getLeft() + m_minHandleSz, m_scrollbarContainer_x.getRight() - m_minHandleSz, 0, totalScrollAmt);

			if (pos > 0) pos = 0;
			else if (pos < totalScrollAmt) pos = totalScrollAmt;

			moveHandleX(pos);

			canvasScrollX = pos;
		}
		if (m_isHandleSelected_y) {
			int totalScrollAmt = (m_mainCanvasRect.getHeight() - m_canvasContainer.getHeight()) * -1;
			int pos = ofMap(y, m_scrollbarContainer_y.getTop() + m_minHandleSz, m_scrollbarContainer_y.getBottom() - m_minHandleSz, 0, totalScrollAmt);

			if (pos > 0) pos = 0;
			else if (pos < totalScrollAmt) pos = totalScrollAmt;

			moveHandleY(pos);

			canvasScrollY = pos;
		}
	}

	//--------------------------------------------------------------
	void mouseReleased() {
		m_isHandleSelected_y = false;
		m_isHandleSelected_x = false;
	}

	//--------------------------------------------------------------
	void mouseScrolled(int& canvasScrollX, int& canvasScrollY) {
		moveHandleX(canvasScrollX);
		moveHandleY(canvasScrollY);
	}

	//--------------------------------------------------------------
	bool isSelectedX() {
		return m_isHandleSelected_x;
	}

	//--------------------------------------------------------------
	bool isSelectedY() {
		return m_isHandleSelected_y;
	}

	//--------------------------------------------------------------
	int getSize() {
		return m_sz;
	}

private:
	//--------------------------------------------------------------
	void moveHandleX(int canvasScrollX) {
		int scrollPosX = ofMap(-canvasScrollX, 0, m_mainCanvasRect.getWidth(), 0, m_scrollbarContainer_x.getWidth() - m_handleMargin);

		m_handle_x.setPosition(
			m_scrollbarContainer_x.getLeft() + scrollPosX + m_handleMargin,
			m_scrollbarContainer_x.getTop() + m_handleMargin);
	}

	//--------------------------------------------------------------
	void moveHandleY(int canvasScrollY) {
		int scrollPosY = ofMap(-canvasScrollY, 0, m_mainCanvasRect.getHeight(), 0, m_scrollbarContainer_y.getHeight() - m_handleMargin);

		m_handle_y.setPosition(
			m_scrollbarContainer_y.getLeft() + m_handleMargin,
			m_scrollbarContainer_y.getTop() + scrollPosY + m_handleMargin);
	}

	const int m_sz = 20;
	const int m_minHandleSz = 20;
	const int m_handleMargin = 2;
	const int m_scrollbarMargin = 4;
	const ofColor m_containerCol = ofColor::darkGray;
	const ofColor m_handleCol_default = ofColor::lightGray;
	const ofColor m_handleCol_hover = ofColor::white;
	const ofColor m_handleCol_selected = ofColor::black;
	ofColor m_handleCol_x = m_handleCol_default;
	ofColor m_handleCol_y = m_handleCol_default;
	const ofColor m_handleBorderCol_default = ofColor::black;
	const ofColor m_handleBorderCol_hover = ofColor::black;
	const ofColor m_handleBorderCol_selected = ofColor::white;
	ofColor m_handleBorderCol_x = m_handleBorderCol_default;
	ofColor m_handleBorderCol_y = m_handleBorderCol_default;
	ofRectangle m_scrollbarContainer_x, m_scrollbarContainer_y;
	ofRectangle m_handle_x, m_handle_y;
	ofRectangle m_topArrowBox, m_bottomArrowBox, m_leftArrowBox, m_rightArrowBox;
	ofRectangle m_mainCanvasRect, m_canvasContainer;
	bool m_isHandleSelected_x, m_isHandleSelected_y;
};
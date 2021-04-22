#pragma once
#include "ofMain.h"
#include "ModalWindow.h"

class LoadPaintingModal {
public:

	//--------------------------------------------------------------
	void setup() {
		window.setup({ ofGetWidth() / 2, ofGetHeight() / 2 });
		window.setSize(m_windowSz);
		ofJson loadPaintingModalSettings{ {
				"width",
			m_windowSz.x,
			-window.getBorderSz() * 2} };

		m_loadPaintingFilename = "null";
		m_thumbWidth = m_windowSz.x / 2;
		m_thumbHeight = m_lineHeight * m_scrollAmt;

		ofAddListener(window.okBtnPressed, this, &LoadPaintingModal::callLoadPainting);
	};

	//--------------------------------------------------------------
	void exit() {
		ofRemoveListener(window.okBtnPressed, this, &LoadPaintingModal::callLoadPainting);
	};

	//--------------------------------------------------------------
	void update() {
		window.update();

		if (window.isVisible())
			if (m_paintingsDir.size() > 0) m_loadPaintingFilename = m_paintingsDir.getName(m_loadPaintingListIdx);
	};

	//--------------------------------------------------------------
	void draw(ofColor& selectionHighlight) {
		window.draw();
		if (window.isVisible()) drawLoadPaintingMenu(selectionHighlight);
	};

	//--------------------------------------------------------------
	void drawLoadPaintingMenu(ofColor& selectionHighlight) {
		glm::vec2 topLeft = { window.getPos().x - window.getWidth() / 2 + window.getBorderSz(),
				window.getPos().y - window.getHeight() / 2 + window.getBorderSz() };

		scrollSignalRect_top.set(
			topLeft.x,
			topLeft.y,
			m_windowSz.x / 2 - window.getBorderSz() * 2,
			m_lineHeight);

		scrollSignalRect_btm.set(
			topLeft.x,
			topLeft.y + m_scrollAmt * m_lineHeight + 20,
			m_windowSz.x / 2 - window.getBorderSz() * 2,
			m_lineHeight);

		ofSetColor(ofColor::white);
		ofFill();
		ofDrawRectangle(
			scrollSignalRect_top.getLeft(),
			scrollSignalRect_top.getBottom(),
			scrollSignalRect_top.getWidth(),
			m_lineHeight * m_scrollAmt);

		ofSetColor(scrollBtnColor_top);
		ofFill();
		ofDrawRectangle(scrollSignalRect_top);
		ofSetColor(ofColor::white);
		string s = "^^^ UP ARROW";
		ofDrawBitmapString(
			s,
			scrollSignalRect_top.getCenter().x - (s.length() * 8) / 2,
			scrollSignalRect_top.getCenter().y + 5);

		ofSetColor(scrollBtnColor_btm);
		ofFill();
		ofDrawRectangle(scrollSignalRect_btm);
		ofSetColor(ofColor::white);
		s = "vvv DOWN ARROW";
		ofDrawBitmapString(
			s,
			scrollSignalRect_btm.getCenter().x - (s.length() * 8) / 2,
			scrollSignalRect_btm.getCenter().y + 5);

		ofFill();
		ofSetColor(ofColor::red);
		ofDrawRectangle(topLeft.x,
			topLeft.y + (m_lineHeight - 10) / 2 + (m_loadPaintingListIdx - m_scrollView_first) * m_lineHeight + 10,
			scrollSignalRect_top.getWidth(),
			scrollSignalRect_top.getHeight());

		if (m_paintingsDir.size() > 0) {
			ofSetColor(ofColor::white);
			m_thumbnails[m_loadPaintingListIdx].draw(
				window.getPos().x + window.getWidth() / 2 - window.getBorderSz() - m_thumbnails[m_loadPaintingListIdx].getWidth(),
				window.getPos().y - window.getHeight() / 2 + window.getBorderSz());
		}

		ofSetColor(ofColor::gray);
		for (int i = m_scrollView_first; i < m_scrollView_last; i++) {
			if (i < m_scrollView_first + m_scrollAmt) {
				if (i == m_loadPaintingListIdx) {
					ofSetColor(ofColor::white);
				}
				else {
					ofSetColor(ofColor::black);
				}
				string fileInfo = ofToString(i + 1) + ": " + m_paintingsDir.getName(i);
				ofDrawBitmapString(fileInfo, topLeft.x, topLeft.y + m_lineHeight + (i - m_scrollView_first) * m_lineHeight + 10);
			}
		}
	}

	//--------------------------------------------------------------
	void loadPaintingDirectory() {
		m_paintingsDir.listDir(ofToDataPath("paintings/"));
		m_paintingsDir.allowExt("png");
		m_paintingsDir.sort(); // in linux the file system doesn't return file lists ordered in alphabetical order

		//allocate the vector to have as many ofImages as files
		if (m_paintingsDir.size())
			m_thumbnails.assign(m_paintingsDir.size(), ofImage());

		// you can now iterate through the files and load them into the ofImage vector
		for (int i = 0; i < (int)m_paintingsDir.size(); i++) {
			m_thumbnails[i].load(m_paintingsDir.getPath(i));
			int w = m_thumbnails[i].getWidth();
			int h = m_thumbnails[i].getHeight();
			if (m_thumbnails[i].getWidth() >= m_thumbnails[i].getHeight()) {
				float scaleRatio = (float)w / (float)m_thumbWidth;
				float m_thumbHeight = (float)h / (float)scaleRatio;

				ofPixels pix;
				pix = m_thumbnails[i].getPixels();
				pix.resize(m_thumbWidth, m_thumbHeight, OF_INTERPOLATE_NEAREST_NEIGHBOR);
				m_thumbnails[i].setFromPixels(pix);
			}
			else {
				float scaleRatio = h / m_thumbHeight;
				float m_thumbWidth = w / scaleRatio;
				m_thumbnails[i].resize(m_thumbWidth, m_thumbHeight);
			}
		}

		m_loadPaintingListIdx = 0;
		m_scrollView_first = 0;
		if (m_paintingsDir.size() >= m_scrollAmt) m_scrollView_last = m_scrollAmt;
		else m_scrollView_last = m_paintingsDir.size();
	}

	//--------------------------------------------------------------
	string getSelectedFilename() {
		return m_loadPaintingFilename;
	}

	//--------------------------------------------------------------
	void resetFilename() {
		m_loadPaintingFilename = "null";
	}

	//--------------------------------------------------------------
	void callLoadPainting(int& i) {
		ofNotifyEvent(loadPainting, i);
	};

	//--------------------------------------------------------------
	int getWindowWidth() {
		return m_windowSz.x;
	};

	//--------------------------------------------------------------
	int getWindowHeight() {
		return m_windowSz.y;
	};

	//--------------------------------------------------------------
	void mousePressed(int x, int y) {
		if (scrollSignalRect_top.inside(x, y)) {
			scrollBtnColor_top = scrollBtnColor_pressed;
		}

		if (scrollSignalRect_btm.inside(x, y)) {
			scrollBtnColor_btm = scrollBtnColor_pressed;
		}
	}

	//--------------------------------------------------------------
	void mouseReleased(int x, int y) {
		if (scrollSignalRect_top.inside(x, y)) {
			scrollBtnColor_top = scrollBtnColor_inactive;
			moveSelectionUp();
		}

		if (scrollSignalRect_btm.inside(x, y)) {
			scrollBtnColor_btm = scrollBtnColor_inactive;
			moveSelectionDown();
		}
	}

	//--------------------------------------------------------------
	void keyPressed(int key) {
		if (!window.isVisible()) return;

		switch (key) {
		case OF_KEY_UP:
			moveSelectionUp();

			break;
		case OF_KEY_DOWN:
			moveSelectionDown();
			break;
		}

		window.keyPressed(key);
	}

	//--------------------------------------------------------------
	void moveSelectionUp() {
		if (m_paintingsDir.size() <= 0) return;

		m_loadPaintingListIdx--;
		if (m_loadPaintingListIdx < 0) {
			m_loadPaintingListIdx = m_paintingsDir.size() - 1;
			if (m_scrollAmt > m_paintingsDir.size()) m_scrollView_first = 0; /// If directory is smaller than scrollable window, m_scrollviewFirst = 0
			else m_scrollView_last = (m_paintingsDir.size() - 1) - m_scrollAmt; /// If directory is larger than scrollable window, m_scrollVIewFirst = directory size - scrollViewSz
			m_scrollView_last = m_paintingsDir.size();
		}

		if (m_loadPaintingListIdx >= m_scrollView_last) {
			m_scrollView_first++;
			m_scrollView_last++;
		}
		else if (m_loadPaintingListIdx < m_scrollView_first) {
			m_scrollView_first--;
			m_scrollView_last--;
		}
	}

	//--------------------------------------------------------------
	void moveSelectionDown() {
		if (m_paintingsDir.size() <= 0) return;

		m_loadPaintingListIdx++;
		if (m_loadPaintingListIdx >= m_paintingsDir.size()) {
			m_loadPaintingListIdx = 0;
			m_scrollView_first = 0;
			if (m_scrollAmt > m_paintingsDir.size()) m_scrollView_last = m_paintingsDir.size(); /// If directory is smaller than scrollable window, m_scrollViewLast = directory sz
			else m_scrollView_last = m_scrollAmt; /// If directory is larger than scrollable window, m_scrollViewLast = m_scrollAmt
		}

		if (m_loadPaintingListIdx >= m_scrollView_last) {
			m_scrollView_first++;
			m_scrollView_last++;
		}
		else if (m_loadPaintingListIdx < m_scrollView_first) {
			m_scrollView_first--;
			m_scrollView_last--;
		}
	}

	//--------------------------------------------------------------
	ModalWindow window;
	ofEvent<int> loadPainting;
private:
	const glm::vec2 m_windowSz = { 500, 500 };

	ofDirectory m_paintingsDir;
	string m_loadPaintingFilename;
	vector<ofImage> m_thumbnails;
	int m_loadPaintingListIdx;
	int m_thumbWidth;
	int m_thumbHeight;

	const int m_lineHeight = 20;
	const int m_scrollAmt = 21;
	int m_scrollView_first, m_scrollView_last;
	ofRectangle scrollSignalRect_top, scrollSignalRect_btm;

	ofColor scrollBtnColor_top = ofColor::black;
	ofColor scrollBtnColor_btm = ofColor::black;
	ofColor scrollBtnColor_inactive = ofColor::black;
	ofColor scrollBtnColor_pressed = ofColor::white;
};
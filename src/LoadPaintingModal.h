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

		loadPaintingFilename = "null";

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
			if (paintingsDir.size() > 0) loadPaintingFilename = paintingsDir.getName(loadPaintingListIdx);
	};

	//--------------------------------------------------------------
	void draw() {
		window.draw();
		if (window.isVisible()) drawLoadPaintingMenu();
	};

	//--------------------------------------------------------------
	void drawLoadPaintingMenu() {
		glm::vec2 topLeft = { window.getPos().x - window.getWidth() / 2 + window.getBorderSz(),
				window.getPos().y - window.getHeight() / 2 + window.getBorderSz() };

		if (paintingsDir.size() > 0) {
			ofSetColor(ofColor::white);
			thumbnails[loadPaintingListIdx].draw(
				window.getPos().x + window.getWidth() / 2 - window.getBorderSz() - thumbnails[loadPaintingListIdx].getWidth(),
				window.getPos().y - window.getHeight() / 2 + window.getBorderSz());
		}

		ofSetColor(ofColor::gray);
		for (int i = 0; i < (int)paintingsDir.size(); i++) {
			if (i == loadPaintingListIdx) {
				ofSetColor(ofColor::red);
			}
			else {
				ofSetColor(ofColor::black);
			}
			string fileInfo = ofToString(i + 1) + ": " + paintingsDir.getName(i);
			ofDrawBitmapString(fileInfo, topLeft.x, topLeft.y + i * 20 + 10);
		}
	}

	//--------------------------------------------------------------
	void loadPaintingDirectory() {
		paintingsDir.listDir(ofToDataPath("paintings/"));
		paintingsDir.allowExt("png");
		paintingsDir.sort(); // in linux the file system doesn't return file lists ordered in alphabetical order

		//allocate the vector to have as many ofImages as files
		if (paintingsDir.size()) {
			thumbnails.assign(paintingsDir.size(), ofImage());
		}

		// you can now iterate through the files and load them into the ofImage vector
		for (int i = 0; i < (int)paintingsDir.size(); i++) {
			thumbnails[i].load(paintingsDir.getPath(i));
			int w = thumbnails[i].getWidth();
			int h = thumbnails[i].getHeight();
			if (thumbnails[i].getWidth() >= thumbnails[i].getHeight()) {
				float scaleRatio = w / thumbWidth;
				float thumbHeight = h / scaleRatio;
				thumbnails[i].resize(thumbWidth, thumbHeight);
			}
			else {
				float scaleRatio = h / thumbHeight;
				float thumbWidth = w / scaleRatio;
				thumbnails[i].resize(thumbWidth, thumbHeight);
			}
		}
		loadPaintingListIdx = 0;
	}

	//--------------------------------------------------------------
	string getSelectedFilename() {
		return loadPaintingFilename;
	}

	//--------------------------------------------------------------
	void resetFilename() {
		loadPaintingFilename = "null";
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
	void keyPressed(int key) {
		if (window.isVisible()) {
			switch (key) {
			case OF_KEY_UP:
				if (paintingsDir.size() > 0) {
					loadPaintingListIdx--;
					if (loadPaintingListIdx < 0) loadPaintingListIdx = paintingsDir.size() - 1;
				}
				break;
			case OF_KEY_DOWN:
				if (paintingsDir.size() > 0) {
					loadPaintingListIdx++;
					loadPaintingListIdx %= paintingsDir.size();
				}
				break;
			}
		}
	}

	//--------------------------------------------------------------
	ModalWindow window;
	ofEvent<int> loadPainting;
private:
	const glm::vec2 m_windowSz = { 500, 500 };

	ofDirectory paintingsDir;
	string loadPaintingFilename;
	vector<ofImage> thumbnails;
	int loadPaintingListIdx;
	const int thumbWidth = 200;
	const int thumbHeight = 250;
};
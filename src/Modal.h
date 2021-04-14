#pragma once
#include "ofMain.h"
#include "Button.h"

class Modal {
public:

	ofEvent<int> okBtnPressed;

	void setup(glm::vec2 pos) {
		m_pos_visible = pos;
		m_pos = m_pos_invisible;
		okButton.setup(m_pos, "OK");
		cancelButton.setup(m_pos, "CANCEL");
		m_isVisible = false;
		animState = ANIM_STATE_NONE;

		ofAddListener(cancelButton.onRelease, this, &Modal::cancelButtonPressed);
		ofAddListener(okButton.onRelease, this, &Modal::okButtonPressed);

#ifdef TARGET_OPENGLES
		bgShader.load("shaders/shadersES2/modalBg");
#else
		if (ofIsGLProgrammableRenderer()) {
			bgShader.load("shaders/shadersGL3/modalBg");
			cout << "GL3" << endl;
		}
		else {
			bgShader.load("shaders/shadersGL2/modalBg");
			cout << "GL2" << endl;
		}
#endif
	}

	void exit() {
		ofRemoveListener(cancelButton.onRelease, this, &Modal::cancelButtonPressed);
		ofRemoveListener(okButton.onRelease, this, &Modal::okButtonPressed);
	}

	void update() {
		switch (animState) {
		case ANIM_STATE_FLASH: {
			if (ofGetFrameNum() % 2 == 0) m_borderCol_dark_flashOn = ofColor::white;
			else m_borderCol_dark_flashOn = ofColor::black;
			m_currentBorderCol_dark = m_borderCol_dark_flashOn;

			if (ofGetFrameNum() % 2 == 0) m_borderCol_shine_flashOn = ofColor::white;
			else m_borderCol_shine_flashOn = ofColor::black;
			m_currentBorderCol_shine = m_borderCol_shine_flashOn;

			if (ofGetElapsedTimef() > m_flashAnimStartTime + m_flashAnimDuration) {
				animState = ANIM_STATE_NONE;
			}
			break;
		}
		case ANIM_STATE_NONE: {
			m_currentBorderCol_dark = m_borderCol_dark_flashOff;
			m_currentBorderCol_shine = m_borderCol_shine_flashOff;
			break;
		}
		}
	}

	void draw(glm::vec2 sz) {
		draw(sz.x, sz.y);
	}

	void draw(float w, float h) {
		if (m_isVisible) m_pos = m_pos_visible;
		else m_pos = m_pos_invisible;

		ofPushStyle(); {
			ofSetRectMode(OF_RECTMODE_CENTER);

			if (m_isVisible) {
				bgShader.begin(); {
					ofDrawRectangle(ofGetWidth() / 2,
						ofGetHeight() / 2,
						ofGetWidth(),
						ofGetHeight());
				}bgShader.end();
			}

			/// 1px black border
			ofSetColor(ofColor::black);
			ofNoFill();
			ofDrawRectangle(m_pos, w + 1, h + 1);

			ofFill();
			/// Border shine
			ofSetColor(m_currentBorderCol_shine);
			ofDrawRectangle(m_pos, w, h);

			/// Border shadow
			ofSetColor(m_currentBorderCol_dark);
			ofSetPolyMode(OF_POLY_WINDING_ODD);	// this is the normal mode
			glm::vec2 topLeft = { m_pos.x - w / 2, m_pos.y - h / 2 };
			ofBeginShape();
			ofVertex(topLeft.x, topLeft.y);
			ofVertex(topLeft.x + m_borderSz, topLeft.y + m_borderSz);
			ofVertex(topLeft.x + w - m_borderSz, topLeft.y + h - m_borderSz);
			ofVertex(topLeft.x + w, topLeft.y + h);
			ofVertex(topLeft.x, topLeft.y + h);
			ofEndShape();

			/// bg
			ofSetColor(m_modalCol);
			ofDrawRectangle(m_pos, w - (m_borderSz * 2), h - (m_borderSz * 2));

			/// Buttons
			okButton.setPos({
				m_pos.x + w / 2 - m_borderSz - okButton.getWidth() - cancelButton.getWidth() - m_padding * 2,
				m_pos.y + h / 2 - okButton.getHeight() - m_padding });

			cancelButton.setPos({ okButton.getPos().x + okButton.getWidth() + m_padding,
				m_pos.y + h / 2 - cancelButton.getHeight() - m_padding });

			okButton.draw();
			cancelButton.draw();

			ofSetColor(ofColor::red);
		}ofPopStyle();
	}

	/// Sets position of modal when visible
	void setPos(glm::vec2 p) {
		m_pos_visible = p;
	}

	void toggleVisible(bool b) {
		m_isVisible = b;

		if (b) {
			m_flashAnimStartTime = ofGetElapsedTimef();
			animState = ANIM_STATE_FLASH;
		}
		else animState = ANIM_STATE_NONE;
	}

	bool isVisible() {
		return m_isVisible;
	}

	/// Gets position of modal (whether visible or not)
	glm::vec2 getPos() {
		return m_pos;
	}

	int getBorderSz() {
		return m_borderSz;
	}

	void mousePressed(int x, int y) {
		if (!m_isVisible) return;

		okButton.mousePressed(x, y);
		cancelButton.mousePressed(x, y);
	}

	void mouseReleased(int x, int y) {
		if (!m_isVisible) return;

		okButton.mouseReleased(x, y);
		cancelButton.mouseReleased(x, y);
	}

private:

	void cancelButtonPressed(int& i) {
		toggleVisible(false);
	}

	void okButtonPressed(int& i) {
		/// Set new canvas dims here
		ofNotifyEvent(okBtnPressed, i);
		toggleVisible(false);
	}

	glm::vec2 m_pos, m_pos_visible;
	const glm::vec2 m_pos_invisible = { -1000,-1000 };
	ofColor m_modalCol = ofColor::lightGray;
	ofColor m_borderCol_dark_flashOff = ofColor::darkGray;
	ofColor m_borderCol_dark_flashOn = ofColor::black;
	ofColor m_borderCol_shine_flashOff = ofColor::white;
	ofColor m_borderCol_shine_flashOn = ofColor::black;
	ofColor m_currentBorderCol_dark = m_borderCol_dark_flashOff;
	ofColor m_currentBorderCol_shine = m_borderCol_shine_flashOff;

	Button okButton, cancelButton;

	const int m_borderSz = 5;
	const int m_padding = 10;

	bool m_isVisible;

	enum AnimState {
		ANIM_STATE_NONE = 0,
		ANIM_STATE_FLASH
	};

	AnimState animState;
	float m_flashAnimStartTime;
	float m_flashAnimDuration = 0.5;

	ofShader bgShader;
};
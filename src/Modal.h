#pragma once
#include "ofMain.h"
#include "Button.h"

class Modal {
public:

	ofEvent<int> okBtnPressed;

	void setup(string name, glm::vec2 pos) {
		m_pos_visible = pos;
		m_pos = m_pos_invisible;
		m_okButton.setup(m_pos, "OK");
		m_cancelButton.setup(m_pos, "CANCEL");
		m_isVisible = false;
		m_animState = ANIM_STATE_NONE;

		cout << "Setting up " << name << " modal" << endl;

		ofAddListener(m_cancelButton.onRelease, this, &Modal::m_cancelButtonPressed);
		ofAddListener(m_okButton.onRelease, this, &Modal::m_okButtonPressed);

#ifdef TARGET_OPENGLES
		m_bgShader.load("shaders/shadersES2/modalBg");
#else
		if (ofIsGLProgrammableRenderer()) {
			m_bgShader.load("shaders/shadersGL3/modalBg");
			cout << "GL3" << endl;
		}
		else {
			m_bgShader.load("shaders/shadersGL2/modalBg");
			cout << "GL2" << endl;
		}
#endif
	}

	void exit() {
		ofRemoveListener(m_cancelButton.onRelease, this, &Modal::m_cancelButtonPressed);
		ofRemoveListener(m_okButton.onRelease, this, &Modal::m_okButtonPressed);
	}

	void update() {
		switch (m_animState) {
		case ANIM_STATE_IN: {
			if (ofGetFrameNum() % 2 == 0) m_borderCol_dark_flashOn = ofColor::white;
			else m_borderCol_dark_flashOn = ofColor::black;
			m_currentBorderCol_dark = m_borderCol_dark_flashOn;

			if (ofGetFrameNum() % 2 == 0) m_borderCol_shine_flashOn = ofColor::white;
			else m_borderCol_shine_flashOn = ofColor::black;
			m_currentBorderCol_shine = m_borderCol_shine_flashOn;

			m_shaderVal = ofMap(ofGetElapsedTimef(),
				m_animStartTime,
				m_animStartTime + m_bgAnimTime,
				0,
				9);

			if (ofGetElapsedTimef() > m_animStartTime + m_animDuration) {
				m_animState = ANIM_STATE_HOLD;
			}
			break;
		}
		case ANIM_STATE_HOLD: {
			m_currentBorderCol_dark = m_borderCol_dark_flashOff;
			m_currentBorderCol_shine = m_borderCol_shine_flashOff;
			break;
		}
		case ANIM_STATE_OUT: {
			m_shaderVal = ofMap(ofGetElapsedTimef(),
				m_animStartTime,
				m_animStartTime + m_bgAnimTime,
				9,
				0);

			if (ofGetElapsedTimef() > m_animStartTime + m_animDuration) {
				m_animState = ANIM_STATE_NONE;
				m_isVisible = false;
			}
			break;
		}
		case ANIM_STATE_NONE: {
			m_currentBorderCol_dark = m_borderCol_dark_flashOff;
			m_currentBorderCol_shine = m_borderCol_shine_flashOff;
			break;
		}
		}

		if (m_animState != ANIM_STATE_NONE) {
			m_shaderTranslateVal += ofGetLastFrameTime() * 30;
			if (m_shaderTranslateVal >= 9.0) m_shaderTranslateVal = 0.0;
		}
	}

	void draw(glm::vec2 sz) {
		draw(sz.x, sz.y);
	}

	void draw(float w, float h) {
		if (m_isVisible && m_animState != ANIM_STATE_OUT) m_pos = m_pos_visible;
		else m_pos = m_pos_invisible;

		ofPushStyle(); {
			ofSetRectMode(OF_RECTMODE_CENTER);

			if (m_isVisible) {
				m_bgShader.begin(); {
					m_bgShader.setUniform1f("animVal", m_shaderVal);
					m_bgShader.setUniform1f("translateVal", m_shaderTranslateVal);
					ofDrawRectangle(ofGetWidth() / 2,
						ofGetHeight() / 2,
						ofGetWidth(),
						ofGetHeight());
				} m_bgShader.end();
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
			m_okButton.setPos({
				m_pos.x + w / 2 - m_borderSz - m_okButton.getWidth() - m_cancelButton.getWidth() - m_padding * 2,
				m_pos.y + h / 2 - m_okButton.getHeight() - m_padding });

			m_cancelButton.setPos({ m_okButton.getPos().x + m_okButton.getWidth() + m_padding,
				m_pos.y + h / 2 - m_cancelButton.getHeight() - m_padding });

			m_okButton.draw();
			m_cancelButton.draw();

			ofSetColor(ofColor::red);
		}ofPopStyle();
	}

	/// Sets position of modal when visible
	void setPos(glm::vec2 p) {
		m_pos_visible = p;
	}

	void toggleVisible(bool b) {
		if (b) {
			m_isVisible = b;
			m_animStartTime = ofGetElapsedTimef();
			m_animState = ANIM_STATE_IN;
			m_shaderTranslateVal = 0.0;
		}
		else {
			cout << "toggleVisible(false)" << endl;
			m_animStartTime = ofGetElapsedTimef();
			m_animState = ANIM_STATE_OUT;
		}
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

		m_okButton.mousePressed(x, y);
		m_cancelButton.mousePressed(x, y);
	}

	void mouseReleased(int x, int y) {
		if (!m_isVisible) return;

		cout << "Modal::mouseReleased" << endl;
		m_okButton.mouseReleased(x, y);
		m_cancelButton.mouseReleased(x, y);
	}

private:

	void m_cancelButtonPressed(int& i) {
		cout << "m_cancelButtonPressed" << endl;
		toggleVisible(false);
	}

	void m_okButtonPressed(int& i) {
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

	Button m_okButton, m_cancelButton;

	const int m_borderSz = 5;
	const int m_padding = 10;

	bool m_isVisible;

	enum AnimState {
		ANIM_STATE_NONE = 0,
		ANIM_STATE_IN,
		ANIM_STATE_HOLD,
		ANIM_STATE_OUT
	};

	AnimState m_animState;
	float m_animStartTime;
	const float m_animDuration = 0.5;

	ofShader m_bgShader;
	float m_shaderVal, m_shaderTranslateVal;
	const float m_bgAnimTime = m_animDuration / 3;
};
#pragma once
#include "ofMain.h"

class StatusBar {
	enum FlashAnimState {
		ANIM_STATE_NONE = 0,
		ANIM_STATE_FLASH
	};

public:
	enum UrgencyLevel {
		URGENCY_LEVEL_NORMAL = 0,
		URGENCY_LEVEL_WARNING,
		URGENCY_LEVEL_PARTY
	};

	void setup(glm::vec2 pos, float length) {
		m_pos = pos;
		m_length = length;
		m_barRect.set(pos.x, pos.y, length, 20);
		m_txtStartPos = m_pos.x + m_length - m_borderSz;
		m_txtEndPos = m_pos.x + m_borderSz;
		m_flashAnimState = ANIM_STATE_NONE;
	};
	void reset() {
		m_txtBuffer.clear();
		m_txtStr = "";
	};
	void say(string msg, UrgencyLevel ul = URGENCY_LEVEL_NORMAL) {
		reset();

		m_urgencyLevel = ul;

		for (int i = 0; i < msg.size(); i++)
			m_txtBuffer.push_back(msg[i]);
        
		m_txtPos = {
			m_txtStartPos,
			m_pos.y + m_height - m_borderSz - 3 };

		m_flashAnimStartTime = ofGetElapsedTimef();
		m_flashAnimState = ANIM_STATE_FLASH;
		m_floatCounter = 0.0;
		m_prevFloatCounterVal = -1;
	};
	void update() {
		switch (m_urgencyLevel) {
		case URGENCY_LEVEL_NORMAL: break;
		case URGENCY_LEVEL_WARNING: break;
		case URGENCY_LEVEL_PARTY:
			if (m_flashAnimState != ANIM_STATE_NONE) {
				int hueBg = ofGetFrameNum() % 255;
				int hueTxt = (ofGetFrameNum() + 127) % 255;
				m_bgCols[m_urgencyLevel].setHsb(hueBg, 255, 255);
				m_txtCols[m_urgencyLevel].setHsb(hueTxt, 255, 255);
				m_flashAnimDuration = 2.0;
			}
			break;
		}

		switch (m_flashAnimState) {
		case ANIM_STATE_NONE:
			m_currentBgCol = m_bgCols[m_bgCols.size() - 1];
			m_currentTxtCol = m_txtCols[m_txtCols.size() - 1];
			break;
		case ANIM_STATE_FLASH:

			if (ofGetFrameNum() % 2 == 0) {
				m_currentBgCol = m_bgCols[m_urgencyLevel];
				m_currentTxtCol = m_txtCols[m_urgencyLevel];
			}
			else {
				m_currentBgCol = m_bgCols[m_bgCols.size() - 1];
				m_currentTxtCol = m_txtCols[m_txtCols.size() - 1];
			}

			if (ofGetElapsedTimef() > m_flashAnimStartTime + m_flashAnimDuration) {
				m_flashAnimState = ANIM_STATE_NONE;
			}
			break;
		}
        

		if (!m_txtStr.empty()) m_floatCounter += ofGetLastFrameTime() * m_speed;;

		if ((int)m_floatCounter != m_prevFloatCounterVal) {
			m_txtPos.x -= m_charLen;
            
            /// Add chars from buffer if the buffer still has chars
            if (!m_txtBuffer.empty()) {
                m_txtStr += m_txtBuffer.front();
                m_txtBuffer.pop_front();
            }
            
			/// Erase chars that pass the end of the status bar
			if (m_txtPos.x <= m_txtEndPos) {
				m_txtStr.erase(0, 1);
				m_txtPos.x += m_charLen;
			}
			m_prevFloatCounterVal = (int)m_floatCounter;
		}
	};

	void draw() {
		ofPushStyle();
		ofSetRectMode(OF_RECTMODE_CORNER);

		// Border
		ofFill();
		ofSetColor(m_borderCol_light);
		ofDrawRectangle(m_pos, m_length, m_height);

		// Border shadow
		ofSetColor(m_borderCol_dark);
		ofSetPolyMode(OF_POLY_WINDING_ODD);	// this is the normal mode
		ofBeginShape();
		ofVertex(m_pos.x, m_pos.y);
		ofVertex(m_pos.x + m_borderSz, m_pos.y + m_borderSz);
		ofVertex(m_pos.x + m_length - m_borderSz, m_pos.y + m_height - m_borderSz);
		ofVertex(m_pos.x + m_length, m_pos.y + m_height);
		ofVertex(m_pos.x, m_pos.y + m_height);
		ofEndShape();

		ofSetColor(m_currentBgCol);
		ofDrawRectangle(m_pos.x + m_borderSz,
			m_pos.y + m_borderSz,
			m_length - (m_borderSz * 2),
			m_height - (m_borderSz * 2));
		ofPopStyle();

		if (!m_txtStr.empty()) {
			ofSetColor(m_currentTxtCol);
			ofDrawBitmapString(m_txtStr, m_txtPos.x, m_txtPos.y);
		}
	};
	float getHeight() {
		return m_height;
	}
    float getBottom(){
        return m_barRect.getBottom();
    }
private:
	glm::vec2 m_pos;
	float m_length;
	const float m_height = 20;
	UrgencyLevel m_urgencyLevel;
	ofRectangle m_barRect;

	ofColor m_borderCol_light = { 235 };
	ofColor m_borderCol_dark = ofColor::darkGray;
	ofColor m_currentBgCol = ofColor::white;
	ofColor m_currentTxtCol = ofColor::black;

	vector<ofColor> m_bgCols = {
		ofColor::black, /// flashing normal
		ofColor::red,	/// flashing warning
		ofColor::purple,/// flahing party
		ofColor::white, /// default
	};

	vector<ofColor> m_txtCols = {
		ofColor::white, /// flashing normal
		ofColor::white,	/// flashing warning
		ofColor::yellow,/// flahing party
		ofColor::black, /// default
	};

	const int m_borderSz = 3;

	std::deque<char> m_txtBuffer;
	string m_txtStr;
	glm::vec2 m_txtPos;
	int m_txtStartPos, m_txtEndPos;
	const int m_charLen = 8;

	FlashAnimState m_flashAnimState;
	float m_flashAnimStartTime;
	float m_flashAnimDuration = 0.5;

	float m_floatCounter;
	int m_prevFloatCounterVal;
	const float m_speed = 10.0;
};

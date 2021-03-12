#pragma once
#include "ofMain.h"

enum AnimState {
	ANIM_STATE_NONE = 0,
	ANIM_STATE_FLASH
};

class BrushAnchor {
public:
	void setup(glm::vec2 brushCanvasPos, glm::vec2 brushCanvasSize) {
		m_canvasPos = brushCanvasPos;
		m_canvasSize = brushCanvasSize;
		m_pos = { m_canvasPos.x - (m_canvasSize.x / 2),  m_canvasPos.y - (m_canvasSize.y / 2) };
		animState = ANIM_STATE_NONE;
		col = ofColor::black;
	};
	void animate(bool& bCenter) {
		if (bCenter) m_pos = m_canvasPos;
		else m_pos = { m_canvasPos.x - (m_canvasSize.x / 2),  m_canvasPos.y - (m_canvasSize.y / 2) };

		animState = ANIM_STATE_FLASH;
		animStartTime = ofGetElapsedTimef();
	};
	void update() {
		if (animState == ANIM_STATE_FLASH) {
			if (ofGetFrameNum() % 2 == 0) col = ofColor::black;
			else col = ofColor::white;

			if (ofGetElapsedTimef() > animStartTime + animDuration) {
				animState = ANIM_STATE_NONE;
			}
		}
	};
	void draw() {
		if (animState == ANIM_STATE_FLASH) {
			ofPushStyle();
			ofSetColor(col);
			ofFill();
			ofDrawRectangle(m_pos.x, m_pos.y, 10, 50);
			ofDrawRectangle(m_pos.x, m_pos.y, 50, 10);
			ofPopStyle();
		}
	};
private:
	glm::vec2 m_pos, m_canvasPos, m_canvasSize;
	ofColor col;
	AnimState animState;
	float animStartTime;
	const float animDuration = 1.0;
};
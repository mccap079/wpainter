#pragma once
#include "ofMain.h"
#include "Button.h"

class Modal {
public:
	void setup() {
		okButton.setup(m_pos, "OK");
		cancelButton.setup(m_pos, "CANCEL");
	}

	void draw(glm::vec2 sz) {
		draw(sz.x, sz.y);
	}

	void draw(float w, float h) {
		m_pos = { ofGetWidth() / 2, ofGetHeight() / 2 };

		ofPushStyle(); {
			ofSetRectMode(OF_RECTMODE_CENTER);

			/// 1px black border
			ofSetColor(ofColor::black);
			ofNoFill();
			ofDrawRectangle(m_pos, w + 1, h + 1);

			ofFill();
			/// Border shine
			ofSetColor(ofColor::white);
			ofDrawRectangle(m_pos, w, h);

			/// Border shadow
			ofSetColor(m_borderCol);
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
			okButton.setPos({ m_pos.x - okButton.getWidth() - m_padding / 2,
			m_pos.y + h / 2 - okButton.getHeight() - m_padding });

			cancelButton.setPos({ m_pos.x + m_padding / 2,
				m_pos.y + h / 2 - cancelButton.getHeight() - m_padding });

			okButton.draw();
			cancelButton.draw();
		}ofPopStyle();
	}

	glm::vec2 getPos() {
		return m_pos;
	}

	int getBorderSz() {
		return m_borderSz;
	}

private:
	glm::vec2 m_pos;
	ofColor m_modalCol = ofColor::lightGray;
	ofColor m_borderCol = ofColor::darkGray;

	Button okButton, cancelButton;

	const int m_borderSz = 5;
	const int m_padding = 10;
};
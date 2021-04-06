#pragma once
#pragma once
#include "ofMain.h"

class Modal {
public:
	void draw(glm::vec2 sz) {
		draw(sz.x, sz.y);
	}

	void draw(float w, float h) {
		glm::vec2 p = { ofGetWidth() / 2, ofGetHeight() / 2 };

		ofPushStyle(); {
			ofSetRectMode(OF_RECTMODE_CENTER);

			/// 1px black border
			ofSetColor(ofColor::black);
			ofNoFill();
			ofDrawRectangle(p, w + 1, h + 1);

			ofFill();
			/// Border shine
			ofSetColor(ofColor::white);
			ofDrawRectangle(p, w, h);

			/// Border shadow
			ofSetColor(m_borderCol);
			ofSetPolyMode(OF_POLY_WINDING_ODD);	// this is the normal mode
			glm::vec2 topLeft = { p.x - w / 2, p.y - h / 2 };
			ofBeginShape();
			ofVertex(topLeft.x, topLeft.y);
			ofVertex(topLeft.x + m_borderSz, topLeft.y + m_borderSz);
			ofVertex(topLeft.x + w - m_borderSz, topLeft.y + h - m_borderSz);
			ofVertex(topLeft.x + w, topLeft.y + h);
			ofVertex(topLeft.x, topLeft.y + h);
			ofEndShape();

			/// bg
			ofSetColor(m_modalCol);
			ofDrawRectangle(p, w - (m_borderSz * 2), h - (m_borderSz * 2));
		}ofPopStyle();
	}
private:
	ofColor m_modalCol = ofColor::lightGray;
	ofColor m_borderCol = ofColor::darkGray;

	const int m_borderSz = 5;
};
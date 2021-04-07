#pragma once

class Button {
private:
	const int padding = 8;

	ofRectangle m_btnRect;
	glm::vec2 m_pos = { 0,0 };
	glm::vec2 charSz = { 8,10 };
	glm::vec2 m_size = { 110, 25 };
	string m_text = "";

	ofColor m_bgColor = ofColor::darkGray;
	ofColor m_txtColor = ofColor::black;
	ofColor m_bgColor_pressed = ofColor::lightGray;
	ofColor m_txtColor_pressed = ofColor::black;
	ofColor m_currentCol_bg = m_bgColor;
	ofColor m_currentCol_txt = m_txtColor;

	void setRect() {
		m_size =
		{
			(m_text.length() * charSz.x) + (padding * 2),
			charSz.y + (padding * 2)
		};
		m_btnRect.set(
			m_pos.x,
			m_pos.y,
			m_size.x,
			m_size.y);
	}
public:
	void setup(glm::vec2 pos, string txt) {
		m_pos = pos;
		m_text = txt;
		setRect();
	}
	void draw() {
		ofSetColor(ofColor::black);
		ofDrawRectangle(m_btnRect.getCenter().x,
			m_btnRect.getCenter().y,
			m_size.x + 3,
			m_size.y + 3);
		ofSetColor(m_currentCol_bg);
		ofDrawRectangle(m_btnRect.getCenter(),
			m_size.x,
			m_size.y);
		ofSetColor(m_currentCol_txt);
		ofDrawBitmapString(m_text,
			m_btnRect.getLeft() + 7,
			m_btnRect.getBottom() - 7);
	};
	void mousePressed(int x, int y) {
		if (m_btnRect.inside(x, y)) {
			m_currentCol_bg = m_bgColor_pressed;
			m_currentCol_txt = m_txtColor_pressed;
		}
	}
	void mouseReleased(int x, int y) {
		if (m_btnRect.inside(x, y)) {
			int i = 0;
			ofNotifyEvent(onRelease, i);
		}

		m_currentCol_bg = m_bgColor;
		m_currentCol_txt = m_txtColor;
	}

	// Top left corner
	void setPos(glm::vec2 pos) {
		m_pos = pos;
		setRect();
	}

	glm::vec2 getPos() {
		return m_pos;
	}

	void setText(string txt) {
		m_text = txt;
		setRect();
	}

	float getBottom() {
		return m_btnRect.getBottom();
	}

	float getWidth() {
		return m_btnRect.getWidth();
	}

	float getHeight() {
		return m_btnRect.getHeight();
	}

	bool isReleased = false;
	ofEvent<int> onRelease;
};
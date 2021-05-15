#pragma once
#include "ofMain.h"
#include "ModalWindow.h"

class CanvasDimsModal {
public:

	//--------------------------------------------------------------
	void setup(int maxTexSz) {
		window.setup({ ofGetWidth() / 2, ofGetHeight() / 2 });
		window.setSize(m_windowSz);

		ofJson setCanvasDimsModalSettings = { {"width", m_windowSz.x - window.getBorderSz() * 2} };

		m_canvasDimsContainer = m_canvasDimsGui.addContainer("Set canvas dims", setCanvasDimsModalSettings);
		m_canvasDimsContainer->setup();
		m_canvasDimsContainer->setPosition(window.getPos());
		m_canvasDimsContainer->add(m_canvasDimsLabel.set("Set canvas size", ""));
		m_canvasDimsContainer->add<ofxGuiTextField>(m_setWidthInput.set("X", "1-" + ofToString(maxTexSz)));
		m_canvasDimsContainer->add<ofxGuiTextField>(m_setHeightInput.set("Y", "1-" + ofToString(maxTexSz)));

		m_canvasDimsContainer->setBackgroundColor({ 100,100,100,255 });

		ofAddListener(window.okBtnPressed, this, &CanvasDimsModal::callSetCanvasDims);
	};

	//--------------------------------------------------------------
	void exit() {
		ofRemoveListener(window.okBtnPressed, this, &CanvasDimsModal::callSetCanvasDims);
	};

	//--------------------------------------------------------------
	void update() {
		window.update();
	};

	//--------------------------------------------------------------
	void draw() {
		window.draw();

		ofSetRectMode(OF_RECTMODE_CORNER);
		m_canvasDimsContainer->setPosition({
			window.getPos().x - m_canvasDimsContainer->getWidth() / 2,
			window.getPos().y - (m_windowSz.y / 2) + window.getBorderSz() });
	};

	//--------------------------------------------------------------
	void callSetCanvasDims(int& i) {
		ofNotifyEvent(setCanvasDims, i);
	};

	//--------------------------------------------------------------
	void setTextFields(string& xField, string& yField) {
		m_setWidthInput = xField;
		m_setHeightInput = yField;
	}

	//--------------------------------------------------------------
	void keyPressed(int key) {
		if (window.isVisible()) window.keyPressed(key);
	}

	//--------------------------------------------------------------
	string getWidthInput() { return m_setWidthInput.get(); };
	string getHeightInput() { return m_setHeightInput.get(); };

	ofEvent<int> setCanvasDims;
	ModalWindow window;
private:
	glm::vec2 m_windowSz = { 250, 150 };
	ofxGui m_canvasDimsGui;
	ofxGuiContainer* m_canvasDimsContainer;
	ofParameter<string> m_canvasDimsLabel;
	ofParameter<string> m_setWidthInput, m_setHeightInput;
};
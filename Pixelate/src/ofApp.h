#pragma once

#include "ofMain.h"
#include "ofxSimpleSerial.h"
#include "ofxXmlSettings.h"

class ofApp : public ofBaseApp{
	
public:
    
    void setup();
    void update();
    void draw();
    
    void keyPressed(int key);
    void keyReleased(int key);
    void mouseMoved(int x, int y );
    void mouseDragged(int x, int y, int button);
    void mousePressed(int x, int y, int button);
    void mouseReleased(int x, int y, int button);
    void windowResized(int w, int h);
    void dragEvent(ofDragInfo dragInfo);
    void gotMessage(ofMessage msg);
    
    //for serial events
    void		onNewMessage(string & message);
    bool		requestRead;
    string message;
    
    ofVideoGrabber 		vidGrabber;
    unsigned char * 	videoInverted;
    ofTexture			videoTexture;
    int 				camWidth;
    int 				camHeight;
    int videoScale;
    float cols;
    float rows;
    ofImage bottomRight;
    ofImage topRight;
    ofImage bottomLeft;
    ofImage topLeft;
    ofImage region;
    ofxXmlSettings xml;
    //ofImage bottomRight;
    //ofSerial	serial;
    ofxSimpleSerial serial;
};

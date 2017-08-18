#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
	
	camWidth 		= 1920;	// try to grab at this size.
	camHeight 		= 1080;
    bufferLimit = 400;
    grab = play = headBack = pGrab = false;
    frameIndex = 0;
    imageBuffer.reserve(1600);
    ofSetLogLevel(OF_LOG_SILENT);
	vector<ofVideoDevice> devices = vidGrabber.listDevices();
	
    for(int i = 0; i < devices.size(); i++){
		cout << devices[i].id << ": " << devices[i].deviceName;
        if( devices[i].bAvailable ){
            cout << endl;
        }else{
            cout << " - unavailable " << endl;
        }
	}
    xml.loadFile("settings.xml");

    string serialName="";
    
    int deviceId=0;
    int baudRate =0;
    int frameRate = 0;
	
    xml.pushTag("settings");
    
    if(xml.getNumTags("deviceId") > 0) {
        deviceId = xml.getValue("deviceId", 0);
    }
    if(xml.getNumTags("serialName") > 0) {
        serialName = xml.getValue("serialName", "test");
    }
    if(xml.getNumTags("baudRate") > 0) {
        baudRate = xml.getValue("baudRate", 0);
    }
    if(xml.getNumTags("frameRate") > 0) {
        frameRate = xml.getValue("frameRate", 0);
    }
    
	xml.popTag();
    cout<<"settings "<<serialName<<" "<<deviceId<<" "<<baudRate<<" "<<frameRate<<endl;
    
    
    //    vidGrabber.setAnchorPoint(100,100);
	vidGrabber.setDeviceID(deviceId);
	vidGrabber.setDesiredFrameRate((frameRate));
	vidGrabber.initGrabber(camWidth,camHeight);
	
	videoInverted 	= new unsigned char[camWidth*camHeight*3];
	videoTexture.allocate(camWidth,camHeight, GL_RGB);
    lastSavedFrame.allocate(camWidth,camHeight, GL_RGB);
	ofSetVerticalSync(true);
    ofSetFrameRate(frameRate);
    
    serial.setup(serialName, (baudRate));
	serial.startContinuousRead();
	ofAddListener(serial.NEW_MESSAGE,this,&ofApp::onNewMessage);
	
	message = "";
    
    
}
void ofApp::onNewMessage(string & message)
{
	cout << "onNewMessage, message: " << message << "\n";
	
	if (message == "s"){
		grab = true;
	}
    if (message== "p"){
        if (play) {
           headBack = true;
        }
		play = true;
	}
    if (message== "r"){
		reverse = true;
	}

}

//--------------------------------------------------------------
void ofApp::update(){
	
	vidGrabber.update();
    
}

//--------------------------------------------------------------
void ofApp::draw(){
	ofSetHexColor(0xffffff);
    
    if (pGrab!=grab) {
        mostRecentFrameIndex=0;
    }
    
	if(grab&&vidGrabber.isFrameNew()&& imageBuffer.size()<bufferLimit){
        
        ofTexture mytexture;

        mytexture.allocate(vidGrabber.getWidth(), vidGrabber.getHeight(), GL_RGB);
        mytexture.loadData(vidGrabber.getPixels(),vidGrabber.getWidth(), vidGrabber.getHeight(), GL_RGB);
//        imageBuffer.push_back(mytexture);
        lastSavedFrame.loadData(vidGrabber.getPixels(),vidGrabber.getWidth(), vidGrabber.getHeight(), GL_RGB);
        std::vector<ofTexture>::iterator it;
        it = imageBuffer.begin()+mostRecentFrameIndex;
        imageBuffer.insert(it, mytexture);
        mostRecentFrameIndex++;
        
        
        //uncomment for serial version
        //grab = false;
        cout<<"buff size"<<imageBuffer.size()<<" framenum "<<ofGetFrameNum()<<endl;
    }
    
    if (reverse) {
        if (imageBuffer.size()>0) {
            imageBuffer.erase(imageBuffer.begin()+imageBuffer.size()-1);
        }
        cout<<imageBuffer.size()<<endl;
        reverse=0;
    }
    
    
    if(!play){
        //vidGrabber.draw(0,0);
        lastSavedFrame.draw(0, 0);

    }
    else{
        if (imageBuffer.size()>0) {
            if (headBack) {
                frameIndex=0;
                headBack=false;
            }
            
            imageBuffer[frameIndex].draw(0,0);
            frameIndex++;
            if(frameIndex>=imageBuffer.size()){
                frameIndex=0;
                play = false;
            }
        }else{
            play =false;
        }
    }
    
    ofDrawBitmapString(ofToString(ofGetFrameRate()), 20,20);
    pGrab = grab;
   // grab=false;
}


//--------------------------------------------------------------
void ofApp::keyPressed  (int key){
    
	if (key == 's' || key == 'S'){
		grab = true;
	}
    if (key == 'p' || key == 'P'){
        if (play) {
            headBack = true;
        }

		play = true;
	}
    if (key == 'r' || key == 'R'){
		reverse = true;
	}
    if (key == 'f' || key == 'F'){
		ofToggleFullscreen();
	}
	
	
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){
	if (key == 's' || key == 'S'){
		grab = false;
	}
}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){
    videoScale = ofMap(x, 0, ofGetWidth(), 1, 100);
    if(videoScale<=0){
        videoScale==1;
    }
	
}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){
	
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
	
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){
    
}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){
    
}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){
    
}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 
    
}

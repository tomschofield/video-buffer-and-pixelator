#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
	
	camWidth 		= 1600;	// try to grab at this size. This should be written over by the settings file
	camHeight 		= 1200;
    
    //similarly this should be written over by the settings file
    bufferLimit = 400;
    drawDebugInfo = grab = play = headBack = pGrab = false;
    
    //these are our marker points in the buffer
    frameIndex = 0;
    mostRecentFrameIndex = 0;
    reverseReferencePoint = 0;
    
    //try saving a bunch of image memory
    imageBuffer.reserve(410);
    
    //this is the number of seconds before we saveout the video
    timeOutThresh = 5;
    sequenceHasChanged = false;
    ofSetLogLevel(OF_LOG_SILENT);
	
    vector<ofVideoDevice> devices = vidGrabber.listDevices();
	//lets see what video devices are available
    for(int i = 0; i < devices.size(); i++){
		cout << devices[i].id << ": " << devices[i].deviceName;
        if( devices[i].bAvailable ){
            cout << endl;
        }else{
            cout << " - unavailable " << endl;
        }
	}
    
    //get everything from out settings file
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
    if(xml.getNumTags("camWidth") > 0) {
        camWidth = xml.getValue("camWidth", 0);
    }
    if(xml.getNumTags("camHeight") > 0) {
        camHeight = xml.getValue("camHeight", 0);
    }
    if(xml.getNumTags("bufferLimit") > 0) {
        bufferLimit = xml.getValue("bufferLimit", 0);
    }
    if(xml.getNumTags("drawDebugInfo") > 0) {
        drawDebugInfo = xml.getValue("drawDebugInfo", 0);
    }
    if(xml.getNumTags("saveVideo") > 0) {
        saveVideo = xml.getValue("saveVideo", 0);
    }
    if(xml.getNumTags("timeOutSecs") > 0) {
        timeOutThresh = xml.getValue("timeOutSecs", 0);
    }
    
    
    
	xml.popTag();

    //print the settings
    cout<<"settings "<<serialName<<" "<<deviceId<<" "<<baudRate<<" "<<frameRate<<" "<<camWidth<<" "<< camHeight <<" "<<bufferLimit<<endl;

    
	vidGrabber.setDeviceID(deviceId);
	vidGrabber.setDesiredFrameRate((frameRate));
	vidGrabber.initGrabber(camWidth,camHeight);
	
	videoInverted 	= new unsigned char[camWidth*camHeight*3];
	videoTexture.allocate(camWidth,camHeight, GL_RGB);
    lastSavedFrame.allocate(camWidth,camHeight, GL_RGB);
    
    //stop shearing
	ofSetVerticalSync(true);
    ofSetFrameRate(frameRate);
    
    serial.setup(serialName, (baudRate));
	serial.startContinuousRead();
	ofAddListener(serial.NEW_MESSAGE,this,&ofApp::onNewMessage);
	
	message = "";
    myWidth = ofGetWidth();
    ofToggleFullscreen();
    
}

//our serial listener
void ofApp::onNewMessage(string & message)
{
	cout << "onNewMessage, message: " << message << "\n";
	
	if (message == "s"){
		grab = true;
        play = false;
	}
    if (message== "p"){
        if (play) {
           headBack = true;
        }
		play = true;
	}
    if (message== "r"){
		reverse = true;
        play = false;
	}
    timeOfLastMessage = ofGetElapsedTimeMillis();
    sequenceHasChanged = true;
}

//--------------------------------------------------------------
void ofApp::update(){
	//try and get a new frame
	vidGrabber.update();
    if (ofGetElapsedTimeMillis()-timeOfLastMessage>=timeOutThresh*1000) {
        saveSequence();
        
    }
    
}

//--------------------------------------------------------------
void ofApp::draw(){
	ofSetHexColor(0xffffff);

    //adjust for discrepancy between cam size and display width
    ofTranslate((ofGetScreenWidth()-camWidth)*0.5,0);

    //if we've got a grab message and this is a new frame
	if(grab&&vidGrabber.isFrameNew()){
        
            ofTexture mytexture;

            mytexture.allocate(vidGrabber.getWidth(), vidGrabber.getHeight(), GL_RGB);
            mytexture.loadData(vidGrabber.getPixels(),vidGrabber.getWidth(), vidGrabber.getHeight(), GL_RGB);
            
        
            std::vector<ofTexture>::iterator it;
            
            //check we don't insert before the start of the buffer
            if(mostRecentFrameIndex<1) mostRecentFrameIndex =0;
        
            if(mostRecentFrameIndex<bufferLimit){
                //stick the frame in after the most recent one
                it = imageBuffer.begin()+mostRecentFrameIndex;
                imageBuffer.insert(it, mytexture);
        
                lastSavedFrame.loadData(vidGrabber.getPixels(),vidGrabber.getWidth(), vidGrabber.getHeight(), GL_RGB);
                mostRecentFrameIndex++;
                
            }
        
            //reverseReference point points to the end of the most recently recorded clip - this is what is removed if we crank backwards
            reverseReferencePoint = mostRecentFrameIndex;
            
        
        
            if(imageBuffer.size()>bufferLimit){
                cout<<"erasing end of buffer";
                imageBuffer.erase(imageBuffer.begin()+imageBuffer.size()-1);
            }
        grab = false;
        //cout<<"buff size"<<imageBuffer.size()<<" framenum "<<ofGetFrameNum()<<endl;

    }
    //if we get a reverese message
    if (reverse) {
        //check we don't try and remove before the start of the buffer
        if(reverseReferencePoint<1) reverseReferencePoint =0;
        //check we actually have a buffer
        if (imageBuffer.size()>0) {
            //update the display frame
            lastSavedFrame =imageBuffer[reverseReferencePoint-1];
            //remove the frame from the buffer at the last recorded to position
            imageBuffer.erase(imageBuffer.begin()+reverseReferencePoint);
            reverseReferencePoint--;
            if(reverseReferencePoint<0)reverseReferencePoint=0;
            mostRecentFrameIndex = reverseReferencePoint;
        }
        reverse=false;
    }
    
    
    if(!play){
        //if we aren't playing back then show the display frame
        lastSavedFrame.draw(0, 0);

    }
    else{
        //once we've played for the first time we call this a 'clip' so lets jump to inserting before this clip
        mostRecentFrameIndex=0;//reverseReferencePoint;
        //play from the beginning
        if (imageBuffer.size()>0) {
            if (headBack) {
                frameIndex=0;
                headBack=false;
            }
           // if(frameIndex<imageBuffer.size()){
            imageBuffer[frameIndex].draw(0,0);
            frameIndex++;
            //}
            if(frameIndex>=imageBuffer.size()){
                frameIndex=0;
                play = false;
            }
        }else{
            play =false;
        }
    }
    if(drawDebugInfo){
        ofSetColor(150);
        string report = "current buffer position: ";
        report+=ofToString(mostRecentFrameIndex);
        report+="\nframeRate: ";
        report+=ofToString(ofGetFrameRate());
        report+="\nbuffer size: ";
        report+=ofToString(imageBuffer.size());
        report+="\nposition from which to remove frames: ";
        report+=ofToString(reverseReferencePoint);
                   
        ofDrawBitmapString(report, 120,20);
    }
    pGrab = grab;
   // grab=false;
}


//--------------------------------------------------------------
void ofApp::keyPressed  (int key){
    timeOfLastMessage = ofGetElapsedTimeMillis();
    sequenceHasChanged = true;
	if (key == 's' || key == 'S'){
		grab = true;
        play = false;
	}
    if (key == 'p' || key == 'P'){
        if (play) {
            headBack = true;
        }

		play = true;
	}
    if (key == 'r' || key == 'R'){
		reverse = true;
        play = false;
	}
    if (key == 'f' || key == 'F'){
		ofToggleFullscreen();
	}
    if (key == 'v' || key == 'V'){
		saveSequence();
	}
	
	
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){
	if (key == 's' || key == 'S'){
		grab = false;
	}
    
}
//--------------------------------------------------------------
void ofApp::saveSequence(){
    if(saveVideo&&sequenceHasChanged){
        cout<<"saving sequence"<<endl;
        ofImage temp;
        if(imageBuffer.size()>0){
        temp.allocate(imageBuffer[0].getWidth(), imageBuffer[0].getHeight(), OF_IMAGE_COLOR);

            for (int i=0; i<imageBuffer.size(); i++) {
                ofPixels pix;
                imageBuffer[i].readToPixels(pix);
                
                temp.setFromPixels(pix);
              
                ofSaveImage(temp, "grabs/test"+ofGetTimestampString() +".jpg");//,
            }
        }
        timeOfLastMessage = ofGetElapsedTimeMillis();
        sequenceHasChanged = false;
    }
	

}


//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){
	
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

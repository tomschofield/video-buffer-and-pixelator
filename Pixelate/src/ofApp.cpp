#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
	
	camWidth 		= ofGetWidth();	// try to grab at this size.
	camHeight 		= ofGetHeight();
    
    videoScale = 3;//8.0;
    cols = camWidth/videoScale;
    rows = camHeight/videoScale;
    
    xml.loadFile("settings.xml");
    ofSetLogLevel(OF_LOG_SILENT);
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
    
   
    
    
    cout<<"settings : "<<serialName<<" "<<deviceId<<" "<<baudRate<<" "<<frameRate<<endl;


	
    //we can now get back a list of devices.
	vector<ofVideoDevice> devices = vidGrabber.listDevices();
	
    for(int i = 0; i < devices.size(); i++){
		cout << devices[i].id << ": " << devices[i].deviceName;
        if( devices[i].bAvailable ){
            cout << endl;
        }else{
            cout << " - unavailable " << endl;
        }
	}
    //    vidGrabber.setAnchorPoint(100,100);
	vidGrabber.setDeviceID((deviceId));
	vidGrabber.setDesiredFrameRate(frameRate);
	vidGrabber.initGrabber(camWidth,camHeight);
	ofSetFrameRate(frameRate);
	videoInverted 	= new unsigned char[camWidth*camHeight*3];
	//videoTexture.allocate(camWidth,camHeight, GL_RGB);
    
    bottomRight.allocate(vidGrabber.getWidth(), vidGrabber.getHeight(), OF_IMAGE_COLOR);
    topRight.allocate(vidGrabber.getWidth(), vidGrabber.getHeight(), OF_IMAGE_COLOR);
    bottomLeft.allocate(vidGrabber.getWidth(), vidGrabber.getHeight(), OF_IMAGE_COLOR);
    topLeft.allocate(vidGrabber.getWidth(), vidGrabber.getHeight(), OF_IMAGE_COLOR);
    
	ofSetVerticalSync(true);
    
    serial.setup(serialName, (baudRate));
	serial.startContinuousRead();
	ofAddListener(serial.NEW_MESSAGE,this,&ofApp::onNewMessage);
	
	message = "";
    ofToggleFullscreen();
}
//serial listener
void ofApp::onNewMessage(string & message)
{
	cout << "onNewMessage, message: " << message << "\n";
	
	videoScale = ofMap(ofToInt(message), 0, 1023, 1.0, ofGetHeight());
}
//--------------------------------------------------------------
void ofApp::update(){
//	if(requestRead)
//    {
//		serial.sendRequest();
//		requestRead = false;
//    }
    
	
	vidGrabber.update();
   
}

//--------------------------------------------------------------
void ofApp::draw(){
    ofBackground(100,100,100);
    //if(vidGrabber.isFrameNew()){
	ofSetHexColor(0xffffff);
    
    cout<<" videoScale "<<videoScale<<endl;
    
    ofColor c;
    int index =0;
    float red =0;
    float green =0;
    float blue=0;
    
    int index1 =0;
    float red1 =0;
    float green1 =0;
    float blue1=0;
    
    int index2 =0;
    float red2 =0;
    float green2 =0;
    float blue2=0;
    
    int index3 =0;
    float red3 =0;
    float green3 =0;
    float blue3=0;
    
    float numDivisions =0.0;
    float midDivision =0.0;
    unsigned char * regionPixels;
    int totalRegionPixels =0;
    int totalPixels;
    unsigned char * pixels;
    float pixCount =0.0;
    float difference =ofGetScreenWidth()-ofGetWidth();
    
    float midX = (difference*0.5) ;
    
    //flip the image by translating to the right and horizontally flipping it
    ofTranslate( camWidth,0);//ofGetWidth(), 0);
    //this is to centre it on the screen
    ofTranslate((ofGetScreenWidth()-camWidth)*0.5,0);
    cout<<difference<<endl;
    
    ofScale(-1, 1);
    
    
    //now the pixelation - essentially I split the image into 4 quarters and iterate outwards from the centre
    ofPushMatrix();
    /////////////////////////////////////    /////////////////////////////////////

    //only do this if we are on a block size of 3 or above. if it's smaller just show the video
    if (videoScale>=3.0 &&videoScale>0) {
                /////////////////////////////////////    /////////////////////////////////////
        
        ofPushMatrix();
        
        //grab the whole image
        topRight.setFromPixels(vidGrabber.getPixels(), vidGrabber.getWidth(), vidGrabber.getHeight(), OF_IMAGE_COLOR);
        
        //trim just the bit we are interested in - the top right
        topRight.crop(ofGetWidth()/2,0,ofGetWidth()/2,ofGetHeight()/2);// camWidth/2, camHeight/2);
        
        //grab the pixels
        pixels = topRight.getPixels();
        
        //shift over to where we want to draw
        ofTranslate(ofGetWidth()/2, 0);
        
        //iterate outwards from teh middle
        for(int x=0;x<topRight.getWidth();x+=videoScale){
            for(int y=topRight.getHeight()-1;y>=0;y-=videoScale){
                red =0;
                green =0;
                blue=0;
                

                //occassionally it drops a pixel so lets keep track of how many we have added to our average
                pixCount=0;
                //grab this region
                region.cropFrom(topRight, x, y-videoScale, videoScale, videoScale);
                regionPixels = region.getPixels();
                totalRegionPixels = (region.getWidth())*(region.getHeight())*3;
                //get an average
                for (int i=0; i<totalRegionPixels; i+=3) {
                    red+=regionPixels[i];
                    green+=regionPixels[i+1];
                    blue+=regionPixels[i+2];
                    pixCount++;
                }
                red/=pixCount;//(totalRegionPixels/3.0);
                green/=pixCount;//(totalRegionPixels/3.0);
                blue/=pixCount;//(totalRegionPixels/3.0);
                //draw a rect with this average colour
                c =ofColor(ofClamp(red,0,255 ),ofClamp(green,0,255),ofClamp(blue,0,255));
                ofSetColor(c);
                ofRect(x,y-videoScale,videoScale,videoScale+1);
                
            }
        }
        ofPopMatrix();
        //repeat!
        
        /////////////////////////////////////    /////////////////////////////////////
        ofPushMatrix();

        topLeft.setFromPixels(vidGrabber.getPixels(), vidGrabber.getWidth(), vidGrabber.getHeight(), OF_IMAGE_COLOR);
        topLeft.crop(0,0,ofGetWidth()/2,ofGetHeight()/2);// camWidth/2, camHeight/2);
        
        pixels = topLeft.getPixels();

        int xCounter = 0;
        int xdistRemaining =0;
        for(int x=topLeft.getWidth()-1;x>=0;x-=videoScale){
            for(int y=topLeft.getHeight()-1;y>=0;y-=videoScale){
                red =0;
                green =0;
                blue=0;

                pixCount=0;
                region.cropFrom(topLeft, x-videoScale, y-videoScale, videoScale, videoScale);
                regionPixels = region.getPixels();
                totalRegionPixels = (region.getWidth())*(region.getHeight())*3;
                for (int i=0; i<totalRegionPixels; i+=3) {
                    red+=regionPixels[i];
                    green+=regionPixels[i+1];
                    blue+=regionPixels[i+2];
                    pixCount++;
                }
                red/=pixCount;//(totalRegionPixels/3.0);
                green/=pixCount;//(totalRegionPixels/3.0);
                blue/=pixCount;//(totalRegionPixels/3.0);
           
                c =ofColor(ofClamp(red,0,255 ),ofClamp(green,0,255),ofClamp(blue,0,255));
                ofSetColor(c);
                ofRect(x-videoScale,y-videoScale,videoScale+1,videoScale+1);
                
            }
            xCounter++;
        }
        
       

        ofPopMatrix();
        
        
        
        
        /////////////////////////////////////    /////////////////////////////////////
        
        ofPushMatrix();
        
        bottomRight.setFromPixels(vidGrabber.getPixels(), vidGrabber.getWidth(), vidGrabber.getHeight(), OF_IMAGE_COLOR);
        bottomRight.crop(ofGetWidth()/2,ofGetHeight()/2,(ofGetWidth()/2),(ofGetHeight()/2));// camWidth/2, camHeight/2);
        
        numDivisions = (0.5*ofGetWidth())/videoScale;
        midDivision = numDivisions/2;
        
        totalPixels = (bottomRight.getWidth())*(bottomRight.getHeight())*3;
        pixels = bottomRight.getPixels();
        ofTranslate(ofGetWidth()/2, ofGetHeight()/2);
        pixCount = 0;
        
        for(int x=0;x<bottomRight.getWidth();x+=videoScale){
            for(int y=0;y<bottomRight.getHeight();y+=videoScale){
                red =0;
                green =0;
                blue=0;
                
                int xScale = bottomRight.getWidth() - (x*videoScale);
                int yScale = bottomRight.getHeight() - (y*videoScale);
                
//                if(xScale<videoScale || yScale <videoScale){
                    region.cropFrom(bottomRight, x, y, videoScale, videoScale);
//                }
//                else{
                
   //                 region.cropFrom(bottomRight, x, y, xScale, yScale);
//                }
            

                regionPixels = region.getPixels();
                totalRegionPixels = (region.getWidth())*(region.getHeight())*3;
                pixCount=0;
                
                for (int i=0; i<totalRegionPixels; i+=3) {
                    red+=regionPixels[i];
                    green+=regionPixels[i+1];
                    blue+=regionPixels[i+2];
                    pixCount++;
                    
                    //                    if(x==0&&y==0){
                    //                        cout<<"pixCount "<<pixCount<<" rgb: "<<(int)regionPixels[i]<<" "<<(int)regionPixels[i+1]<<" "<<(int)regionPixels[i+2]<<endl;
                    //                    }
                    
                    
                }


                
                red/=(pixCount);//region.getWidth()*region.getHeight();// (totalRegionPixels/3.0);
                green/=(pixCount);//region.getWidth()*region.getHeight();//(totalRegionPixels/3.0);
                blue/=(pixCount);//region.getWidth()*region.getHeight();//(totalRegionPixels/3.0);
                
                //red = green = blue = 255;
                c =ofColor(ofClamp(red,0,255 ),ofClamp(green,0,255),ofClamp(blue,0,255));
                
                ofSetColor(c);
                ofRect(x,y,videoScale,videoScale);
                
            }
        }
        
        ofPopMatrix();
        
        
        /////////////////////////////////////    /////////////////////////////////////
        
        
        ofPushMatrix();
        bottomLeft.setFromPixels(vidGrabber.getPixels(), vidGrabber.getWidth(), vidGrabber.getHeight(), OF_IMAGE_COLOR);
        bottomLeft.crop(0,ofGetHeight()/2,ofGetWidth()/2,ofGetHeight()/2);// camWidth/2, camHeight/2);
        
        pixels = bottomLeft.getPixels();
        ofTranslate(0, ofGetHeight()/2);
        
        for(int x=bottomLeft.getWidth()-1;x>=0;x-=videoScale){
            for(int y=0;y<bottomLeft.getHeight()-1;y+=videoScale){
                red =0;
                green =0;
                blue=0;
                pixCount=0;
                region.cropFrom(bottomLeft, x-videoScale, y, videoScale, videoScale);
                regionPixels = region.getPixels();
                totalRegionPixels = (region.getWidth())*(region.getHeight())*3;
                for (int i=0; i<totalRegionPixels; i+=3) {
                    red+=regionPixels[i];
                    green+=regionPixels[i+1];
                    blue+=regionPixels[i+2];
                    pixCount++;
                }
                red/=pixCount;//(totalRegionPixels/3.0);
                green/=pixCount;//(totalRegionPixels/3.0);
                blue/=pixCount;//(totalRegionPixels/3.0);
                
                c =ofColor(ofClamp(red,0,255 ),ofClamp(green,0,255),ofClamp(blue,0,255));
                ofSetColor(c);
                ofRect(x-videoScale,y,videoScale+1,videoScale+1);
                
            }
        }
        ofPopMatrix();

               ofSetColor(0, 0, 0);
        ofPopMatrix();
    }
    else{
        
        vidGrabber.draw(0,0);
    }
    ofDrawBitmapString(ofToString(ofGetFrameRate()), 30, 30);
}


//--------------------------------------------------------------
void ofApp::keyPressed  (int key){
	
	// in fullscreen mode, on a pc at least, the
	// first time video settings the come up
	// they come up *under* the fullscreen window
	// use alt-tab to navigate to the settings
	// window. we are working on a fix for this...
	
	// Video settings no longer works in 10.7
	// You'll need to compile with the 10.6 SDK for this
    // For Xcode 4.4 and greater, see this forum post on instructions on installing the SDK
    // http://forum.openframeworks.cc/index.php?topic=10343
	if (key == 's' || key == 'S'){
		vidGrabber.videoSettings();
	}
    if (key == 'f' || key == 'F'){
        ofToggleFullscreen();
    }
    
	
	
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){
	
}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){
//    videoScale = x/2;//ofMap(x, 0, ofGetWidth(), 1.0, ofGetWidth());///15);
//    if(videoScale<=0){
//        videoScale==1;
//    }
	
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

#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
	
	camWidth 		= ofGetWidth();	// try to grab at this size.
	camHeight 		= ofGetHeight();
    
    videoScale = 20.0;//8.0;
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
}

void ofApp::onNewMessage(string & message)
{
	cout << "onNewMessage, message: " << message << "\n";
	
	videoScale = ofMap(ofToInt(message), 0, 1023, 1.0, ofGetHeight());
}
//--------------------------------------------------------------
void ofApp::update(){
	if(requestRead)
    {
		serial.sendRequest();
		requestRead = false;
    }
    
	ofBackground(100,100,100);
	vidGrabber.update();
   
}

//--------------------------------------------------------------
void ofApp::draw(){
    //if(vidGrabber.isFrameNew()){
	ofSetHexColor(0xffffff);
    
    cout<<" videoScale "<<videoScale<<endl;
    
    ofColor c;
    int index =0;
    float red =0;
    float green =0;
    float blue=0;
    
    int index1 =0;
    int red1 =0;
    int green1 =0;
    int blue1=0;
    
    int index2 =0;
    int red2 =0;
    int green2 =0;
    int blue2=0;
    
    int index3 =0;
    int red3 =0;
    int green3 =0;
    int blue3=0;
    
    
    unsigned char * regionPixels;
    int totalRegionPixels =0;
    
    ofPushMatrix();
    /////////////////////////////////////    /////////////////////////////////////
//    
//    ofTranslate(ofGetWidth()/2, ofGetHeight()/2);
//    ofScale(1.3,1.3);
//    ofTranslate(-ofGetWidth()/2, -ofGetHeight()/2);
    
    if (videoScale>=3.0 &&videoScale>0) {
        ofPushMatrix();
        
        bottomRight.setFromPixels(vidGrabber.getPixels(), vidGrabber.getWidth(), vidGrabber.getHeight(), OF_IMAGE_COLOR);
        bottomRight.crop(ofGetWidth()/2,ofGetHeight()/2,ofGetWidth()/2,ofGetHeight()/2);// camWidth/2, camHeight/2);
        
        float numDivisions = (0.5*ofGetWidth())/videoScale;
        float midDivision = numDivisions/2;
        
        int totalPixels = (bottomRight.getWidth())*(bottomRight.getHeight())*3;
        unsigned char * pixels = bottomRight.getPixels();
        //bottomRight.draw(0,0);
        ofTranslate(ofGetWidth()/2, ofGetHeight()/2);
        float pixCount = 0;
        for(int x=0;x<bottomRight.getWidth();x+=videoScale){
            for(int y=0;y<bottomRight.getHeight();y+=videoScale){
//                if(videoScale>20){
                
                    region.cropFrom(bottomRight, x, y, videoScale, videoScale);
                  //  region.resize(1, 1);
                //region.setFromPixels(bottomRight.getPixels(), bottomRight.getWidth(), bottomRight.getHeight(), OF_IMAGE_COLOR);
                //region.crop( x, y, videoScale, videoScale);
                //    cout<<"region.isAllocated()"<<region.isAllocated()<<endl;
                    regionPixels = region.getPixels();
                    totalRegionPixels = (region.getWidth())*(region.getHeight())*3;
                    pixCount=0;
//
//                    
                    for (int i=0; i<totalRegionPixels; i+=3) {
                        //if (regionPixels[i]>0 && regionPixels[i]<256 && regionPixels[i+1]>0 && regionPixels[i+1]<256 && regionPixels[i+2]>0 &&regionPixels[i+2]<256)  {
                            red+=regionPixels[i];
                            green+=regionPixels[i+1];
                            blue+=regionPixels[i+2];
                            pixCount++;
    //                    }else{
    //                        cout<<"weird pixel "<<regionPixels[i]<<" "<<regionPixels[i+1]<<" "<<regionPixels[i+2]<<endl;
    //                    }
                        
                        
    //                    }
//
                    }
    //                if(pixCount!=totalRegionPixels/3){
    //                cout<<"pixCount "<<pixCount<<" totalRegionPixels "<<totalRegionPixels/3 <<endl;
    //                }

                    
                    red/=(pixCount);//region.getWidth()*region.getHeight();// (totalRegionPixels/3.0);
                    green/=(pixCount);//region.getWidth()*region.getHeight();//(totalRegionPixels/3.0);
                    blue/=(pixCount);//region.getWidth()*region.getHeight();//(totalRegionPixels/3.0);
//                }
//                else{
//                    index =3*( x +(y*bottomRight.getWidth()));
//                    red =pixels[index];
//                    green =pixels[1+index];//(y + x*camWidth)]  ;
//                    blue =pixels[2+index];//(y + x*camWidth)];
//
//                }
                
//                //bottom left
//
//                //bottom right
//                index1 =3*( videoScale + x +(y*bottomRight.getWidth()));
//                red1 =pixels[index1];
//                green1 =pixels[1+index1];//(y + x*camWidth)]  ;
//                blue1 =pixels[2+index1];//(y + x*camWidth)];
//                
//                //top left
//                index2 =3*( x +((y+videoScale)*bottomRight.getWidth()));
//                
//                index2 = min(index2,totalPixels-1 );
//                red2 =pixels[index2];
//                green2 =pixels[1+index2];//(y + x*camWidth)]  ;
//                blue2 =pixels[2+index2];//(y + x*camWidth)];
//                
//                //top right
//                index3 =3*(videoScale+ x +((y+videoScale)*bottomRight.getWidth()));
//                index3 = min(index3,totalPixels-1 );
//                red3 =pixels[index3];
//                green3 =pixels[1+index3];//(y + x*camWidth)]  ;
//                blue3 =pixels[2+index3];//(y + x*camWidth)];

                c =ofColor(ofClamp(red,0,255 ),ofClamp(green,0,255),ofClamp(blue,0,255));
//               //  c =ofColor((red+red1+red2+red3)*0.25  ,(green+green1+green2+green3)*0.25,(blue+blue1+blue2+blue3)*0.25);
//                
//                
                ofSetColor(c);
                ofRect(x,y,videoScale,videoScale);

                //region.draw(x,y,videoScale,videoScale);
            }
        }
        
        //do the last 2 lines
        
        
        
        ofPopMatrix();
        /////////////////////////////////////    /////////////////////////////////////
         red =0;
         green =0;
         blue=0;

        
        
        ofPushMatrix();
        //ofImage topRight;
        //topRight.allocate(vidGrabber.getWidth(), vidGrabber.getHeight(), OF_IMAGE_COLOR);
        topRight.setFromPixels(vidGrabber.getPixels(), vidGrabber.getWidth(), vidGrabber.getHeight(), OF_IMAGE_COLOR);
        topRight.crop(ofGetWidth()/2,0,ofGetWidth()/2,ofGetHeight()/2);// camWidth/2, camHeight/2);
        
        pixels = topRight.getPixels();
        //bottomRight.draw(0,0);
        ofTranslate(ofGetWidth()/2, 0);
        
        for(int x=0;x<topRight.getWidth();x+=videoScale){
            for(int y=topRight.getHeight()-1;y>=0;y-=videoScale){
                

                pixCount=0;
                region.cropFrom(topRight, x, y-videoScale, videoScale, videoScale);
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
                ofRect(x,y-videoScale,videoScale,videoScale+1);
                
            }
        }
        ofPopMatrix();
        
        red =0;
        green =0;
        blue=0;
        

        /////////////////////////////////////    /////////////////////////////////////
        ofPushMatrix();

        topLeft.setFromPixels(vidGrabber.getPixels(), vidGrabber.getWidth(), vidGrabber.getHeight(), OF_IMAGE_COLOR);
        topLeft.crop(0,0,ofGetWidth()/2,ofGetHeight()/2);// camWidth/2, camHeight/2);
        
        pixels = topLeft.getPixels();

        int xCounter = 0;
        int xdistRemaining =0;
        for(int x=topLeft.getWidth()-1;x>=0;x-=videoScale){
            for(int y=topLeft.getHeight()-1;y>=0;y-=videoScale){
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
                ofSetColor(0);

                
            }
            xCounter++;
        }
        
       

        ofPopMatrix();
        red =0;
        green =0;
        blue=0;
        

        /////////////////////////////////////    /////////////////////////////////////
        
        
        ofPushMatrix();
           bottomLeft.setFromPixels(vidGrabber.getPixels(), vidGrabber.getWidth(), vidGrabber.getHeight(), OF_IMAGE_COLOR);
        bottomLeft.crop(0,ofGetHeight()/2,ofGetWidth()/2,ofGetHeight()/2);// camWidth/2, camHeight/2);
        
        pixels = bottomLeft.getPixels();
        ofTranslate(0, ofGetHeight()/2);
        
        for(int x=bottomLeft.getWidth()-1;x>=0;x-=videoScale){
            for(int y=0;y<bottomLeft.getHeight()-1;y+=videoScale){
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
    videoScale = ofMap(x, 0, ofGetWidth(), 1.0, ofGetHeight());///15);
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

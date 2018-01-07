//
//  GloveCapture.cpp
//  DataGloveMiniProject
//
//  Created by Terry Clark on 02/01/2018.
//

#include "GloveCapture.hpp"

GloveCapture::GloveCapture(){
    //Serial Setup for accessing glove data
    serial.listDevices();
    serial.setup(0, 9600);
    serial.startContinuousRead();
    ofAddListener(serial.NEW_MESSAGE,this,&GloveCapture::onNewMessage);
    
    message = "";
    
    //Glove Gui
    gloveGui = new ofxDatGui( ofxDatGuiAnchor::TOP_RIGHT );
    gloveGui->setAssetPath(""); //this has to be done due to bug with gui
    
    gloveGui->setPosition(0,0);
    gloveGui->addHeader("Glove Params");
    gloveGui->addToggle("Calibrate");
    gloveGui->addButton("Preset Calibration");

    //Setup GloveGui Events
    gloveGui->onToggleEvent(this, &GloveCapture::onToggleEvent);
    gloveGui->onButtonEvent(this, &GloveCapture::onButtonEvent);

    //Glove Params
    for (unsigned i = 0; i < FEATURES; i++){
        rawValues.push_back(0.0);
        //Initialise min and max as opposite values for the calibration to correctly find the overal maximum of the glove.
        minVals.push_back(numeric_limits<float>::max());
        maxVals.push_back(numeric_limits<float>::min());
    }

    //Mapping Glove data to and from values
    mapFrom = 0;
    mapTo =   10;
    
    bCalibrateGlove = false;
    bPresetCalibration = false;
    bufferSize = 128;
}

//--------------------------------------------------------------

void GloveCapture::calibrate(float _vals[7]){
    for (unsigned i = 0; i < FEATURES; i++){
        //Keep reseting the values to the least and the maximum range
        if (_vals[i] > maxVals.at(i)){
            maxVals.at(i) = _vals[i];
        }
        
        if (_vals[i] < minVals.at(i)){
            minVals.at(i) = _vals[i];
        }
    }
}

//--------------------------------------------------------------

//I used a preset calibration as the glove itself is fault, when I tap my hand to hard it will glitch and the finger reading will be incorrect
void GloveCapture::usePresetCalibration(){
    minVals[0] = -13;  maxVals[0] = 13;
    minVals[1] = -17;  maxVals[1] = 17;
    minVals[2] = -14;  maxVals[2] = 14;
    minVals[3] = 639; maxVals[3] = 800;
    minVals[4] = 765; maxVals[4] = 800;
    minVals[5] = 60;  maxVals[5] = 170;
    minVals[6] = 710; maxVals[6] = 750;
    bPresetCalibration = false;
}

//--------------------------------------------------------------

void GloveCapture::update(){
    if(ofGetFrameNum() % SENDMSG == 0){
        serial.sendRequest();//This will send a request to serial every 5 frames
        
        //Capture all raw glove values
        for (int i = 0; i < FEATURES; i++){
            gloveVals[i] = rawValues[i];
        }
        
        for (int i = 3; i < FEATURES; i++){
            //Raw data sent to lopass filter
            gloveVals[i] = filter.lopass(gloveVals[i], 1);

            //Make RMS the new finger value
            gloveVals[i] = rms(gloveVals[i]);
        }
            
        
        if (bCalibrateGlove){
            calibrate(gloveVals);
        }
        
        if (bPresetCalibration){
            usePresetCalibration();
        }
        
        //Map and Clamp finger indicies in array (3 to 6)
        for(int i = 3; i < FEATURES; i++){
            gloveVals[i] = ofMap(gloveVals[i], minVals[i], maxVals[i], mapFrom, mapTo);
            gloveVals[i] = ofClamp(gloveVals[i], mapFrom, mapTo);
        }
        
    }
}

//--------------------------------------------------------------

double GloveCapture::rms(double _value){
    double rmsVal = 0.0;
    //RMS for fingers only as accelerometer is smooth enough
    for (int i = 0; i < bufferSize; i++){
        //Sum up the glove vals for the entire buffer range of
        rmsVal += _value * _value;
    }
    
    //Square root the total rms value divide by the buffer
    return sqrt(rmsVal / bufferSize);
}

//--------------------------------------------------------------

void GloveCapture::draw(int x, int y){
    ofPushStyle();
    {
        ofSetColor(255);
        for(int i = 0; i < FEATURES; i++){
            ofDrawBitmapString(featureNames[i], x , y + (20 * i));
            ofDrawBitmapString("Min: " + ofToString(minVals[i]), x + 80, y + (20*i));
            ofDrawBitmapString("Max: " + ofToString(maxVals[i]), x + 160, y + (20*i));
            ofDrawBitmapString("Value: " + ofToString(gloveVals[i]), x + 240, y + (20*i));
        }
    }
    ofPopStyle();
}

//--------------------------------------------------------------

vector<double> GloveCapture::getData(){
    vector<double> data;
    for(int i = 0; i < FEATURES; i++){
        data.push_back(gloveVals[i]);
    }
    return data;
}
//--------------------------------------------------------------

void GloveCapture::onToggleEvent(ofxDatGuiToggleEvent e){
    string guiLabel = e.target->getLabel();
    
    if (guiLabel == "Calibrate") {
        bCalibrateGlove = e.target->getChecked();
        
        if (bCalibrateGlove){
            for(int i = 0; i <FEATURES; i++){
                minVals[i] = numeric_limits<float>::max();
                maxVals[i] = numeric_limits<float>::min();
            }
        }
    }
}

//--------------------------------------------------------------

void GloveCapture::onButtonEvent(ofxDatGuiButtonEvent e){
    string guiLabel = e.target->getLabel();
    
    if (guiLabel == "Preset Calibration")
        bPresetCalibration = e.target->getEnabled();
}

//--------------------------------------------------------------

void GloveCapture::onNewMessage(string & message)
{
    //Split out the input message from serial into a vector
    //cout << "onNewMessage, message: " << message << "\n";
    vector<string> input = ofSplitString(message, ",");
    
    for (int i = 0 ; i < input.size(); i++){
        rawValues.at(i) = ofToFloat(input[i]);
    }
}




//  GloveCapture.hpp
//  DataGloveMiniProject
//
//  Created by Terry Clark on 02/01/2018.
//

#pragma once

#include "ofMain.h"
#include "ofxSimpleSerial.h"
#include "ofxMaxim.h"
#include "ofxDatGui.h"

#define FEATURES 7
#define SENDMSG 4

class GloveCapture{
private:
    void onNewMessage(string & message);
    void onToggleEvent(ofxDatGuiToggleEvent e);
    void onButtonEvent(ofxDatGuiButtonEvent e);
    
    //Serial Setup
    ofxSimpleSerial serial;
    
    //Glove Interface;
    ofxDatGui* gloveGui;
    
    vector<float> rawValues;
    //For Glove Features
    string featureNames[7] = {  "accelX",
                                "accelY",
                                "accelZ",
                                "finger1",
                                "finger2",
                                "finger3",
                                "finger4"};
    string message;
    float mapTo;
    float mapFrom;
    float gloveVals[7];
    bool bCalibrateGlove;
    bool bPresetCalibration;
    int bufferSize;
    maxiFilter filter;
    
public:
    GloveCapture();
   
    void calibrate(float _vals[7]);
    void usePresetCalibration();
    void update();
    void draw(int x, int y);
    
    double rms(double _value);
    
    vector<double> getData();
    vector<double> minVals;
    vector<double> maxVals;
};

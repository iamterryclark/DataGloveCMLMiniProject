//
//  MachineLearning.hpp
//  DataGloveMiniProject
//
//  Created by Terry Clark on 03/01/2018.
//

#pragma once

#include "ofMain.h"
#include "ofxRapidLib.h"
#include "GloveCapture.hpp"
#include "GrainSynthesis.hpp"

class MachineLearning{
private:
    GloveCapture* glove;
    GrainSynthesis* grainSynth;
    
    //Machine Learning Gui
    ofxDatGui* mlGui;
    void onButtonEvent(ofxDatGuiButtonEvent e);
    void onToggleEvent(ofxDatGuiToggleEvent e);

public:
    MachineLearning(GloveCapture * _glove, GrainSynthesis * _granSynth);
    void update();
    void draw(int x, int y);
    void grainSynthReset();
    
    //Gesture Recognition
    classification knn;
    vector<trainingExample> trainingSet;
    double classLabel;
    int gestureNum;
    
    //Regression
    vector<trainingExample> trainingSetReg;
    trainingExample tempExample;
    regression myReg;
    string mlStatus = "";
    
    bool bCaptureRegression, bCaptureGesture;
    bool bTrainRegression, bTrainGestures;
    bool bRunRegression,  bRunGestures;
};

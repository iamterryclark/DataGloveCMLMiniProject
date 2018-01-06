
//
//  MachineLearning.cpp
//  DataGloveMiniProject
//
//  Created by Terry Clark on 03/01/2018.
//

#include "MachineLearning.hpp"

MachineLearning::MachineLearning(GloveCapture * _glove, GrainSynthesis * _grainSynth) :
glove(_glove),
grainSynth(_grainSynth)
{
    mlGui = new ofxDatGui( ofxDatGuiAnchor::TOP_RIGHT );
    mlGui->setAssetPath("");
    mlGui->setPosition(ofGetWidth()/2-mlGui->getWidth()/2, 0);
    
    //Setup Gui Sliders/Buttons
    mlGui->addHeader("Gesture Gui");
    
    /*Gestures for: 1) Record,
                    2) Set Playhead,
                    3) Manipulate Granular Synth
    */
    
    mlGui->addLabel(" :: Gesture Recognition :: ");
    mlGui->addToggle("Static Hand");
    mlGui->addToggle("Record Voice");
    mlGui->addToggle("Set Playhead");
    mlGui->addToggle("Manipulate Synth");
    mlGui->addBreak()->setHeight(10.0f);
    mlGui->addLabel(" :: Regression :: ");
    mlGui->addToggle("Regression");
    mlGui->addBreak()->setHeight(10.0f);
    mlGui->addLabel(" :: Clear Out :: ");
    mlGui->addButton("Clear All");
    mlGui->addBreak()->setHeight(10.0f);

    mlGui->onButtonEvent(this, &MachineLearning::onButtonEvent);
    mlGui->onToggleEvent(this, &MachineLearning::onToggleEvent);
    
    //Machine Learning Params
    bCaptureGesture = bCaptureRegression =
    bTrainGestures = bTrainRegression =
    bRunGestures = bRunRegression = false;
}

//--------------------------------------------------------------

void MachineLearning::update(){
    
    //Capture data every 5 frames (as this is same as the the glove)
    if (ofGetFrameNum() % SENDMSG == 0){
        vector<double> gloveData = glove->getData();
        vector<double> gloveLabel;
        
//
// :: Testing / Recording Phase ::
//

        //If we are recording a gesture
        if (bCaptureGesture){
            trainingExample tempExample;
            
            tempExample.input = { gloveData[3], gloveData[4],
                                  gloveData[5], gloveData[6] };
            tempExample.output = { (double)gestureNum };
            trainingSet.push_back(tempExample);
            bRunGestures = false;
        }
        
        //If we want to record regression
        if (bCaptureRegression){
            tempExample.input = { gloveData[0], gloveData[1], gloveData[2] };
            tempExample.output = {
                grainSynth->grainGui->getSlider("Grain Rate")->getValue(),
                grainSynth->grainGui->getSlider("Grain Length")->getValue(),
                grainSynth->grainGui->getSlider("Grain Speed")->getValue(),
                grainSynth->grainGui->getSlider("Grain Overlaps")->getValue()
            };
            trainingSetReg.push_back(tempExample);
            bRunGestures = false;
        }

//
// :: Training Phase ::
//
        if (bTrainGestures) {
            if(trainingSet.size() > 0) {
                knn.train(trainingSet);
                cout << "Gesture Number " + ofToString(gestureNum) + " Trained" << endl;
                bTrainGestures = false;
                bRunGestures = true;
            } else {
                cout << "Gestures Not Trained" << endl;
            }
        }
        
        if (bTrainRegression) {
            //Training Phase for NN Regression
            if(trainingSetReg.size() > 0){
                myReg.train(trainingSetReg);
                cout << "Regression Trained " << endl;
                bTrainRegression = false;
                bRunGestures = true;
            } else {
                cout << "Regression Not Trained" << endl;
            }
        }

//
// :: Run Phase ::
//
        if (bRunGestures) {
            float playheadSlider = 0;
            classLabel = knn.run({  gloveData[3],
                                    gloveData[4],
                                    gloveData[5],
                                    gloveData[6] })[0];
            int classNum = (int)classLabel;
            vector<double> output;
            
            switch(classNum){
                case 0:
                    grainSynthReset();
                    break;
                case 1: // Record Live Voice Input
                    grainSynth->recLiveInput = true;
                    grainSynth->setPHPosition = false;
                    
                    grainSynth->grainGui->getToggle("Record Live Input")->setChecked(true);
                    grainSynth->grainGui->getToggle("Set Playhead Position")->setChecked(false);
                    break;
                case 2: // Set Playhead Mode
                    grainSynth->setPHPosition = true;
                    grainSynth->recLiveInput = false;
                    
                    grainSynth->grainGui->getToggle("Set Playhead Position")->setChecked(true);
                    grainSynth->grainGui->getToggle("Record Live Input")->setChecked(false);

                    playheadSlider = ofMap(gloveData[0], glove->maxVals[0], glove->minVals[0], 0, 1 );
                    grainSynth->grainGui->getSlider("Playhead Position")->setValue(playheadSlider);
                    grainSynth->playHead = playheadSlider;
                    break;
                case 3: // Change Synth Params
                    grainSynthReset();
                    
                    if (bRunRegression){
                        
                        output = myReg.run({ gloveData[0], gloveData[1], gloveData[2] });
                        
                        //For Visual Purposes
                        grainSynth->grainGui->getSlider("Grain Rate")->setValue(output[0]);
                        grainSynth->grainGui->getSlider("Grain Length")->setValue(output[1]);
                        grainSynth->grainGui->getSlider("Grain Speed")->setValue(output[2]);
                        grainSynth->grainGui->getSlider("Grain Overlaps")->setValue((int)output[3]);
                        
                        //Actually Changing the Values
                        grainSynth->grainRate = output[0];
                        grainSynth->grainLength = output[1];
                        grainSynth->grainSpeed = output[2];
                        grainSynth->grainOverlaps = output[3];
                    }
                    break;
            }
        }
    }
}

//--------------------------------------------------------------

void MachineLearning::grainSynthReset(){
    grainSynth->recLiveInput = false;
    grainSynth->setPHPosition = false;
    grainSynth->grainGui->getToggle("Record Live Input")->setChecked(false);
    grainSynth->grainGui->getToggle("Set Playhead Position")->setChecked(false);
}

//--------------------------------------------------------------

void MachineLearning::draw(int x, int y){
    ofDrawBitmapString("Training Set Gestures : " + ofToString(trainingSet.size()), x, y + 20);
    ofDrawBitmapString("Training Set Reg : " + ofToString(trainingSetReg.size()), x,  y + 40);
    ofDrawBitmapString("Running ML : " + ofToString(bool(bRunGestures)), x, y + 60);
    ofDrawBitmapString("Capture Gesture : " + ofToString(bool(bCaptureGesture)), x, y + 80);
    ofDrawBitmapString("Capture Regression : " + ofToString(bool(bCaptureRegression)), x, y + 100);
    ofDrawBitmapString("ML Status : " + mlStatus, x, y + 120);
    ofDrawBitmapString("Gesture Class Result : " + ofToString(classLabel), x, y + 140);
}

//--------------------------------------------------------------

void MachineLearning::onToggleEvent(ofxDatGuiToggleEvent e){
    string guiLabel = e.target->getLabel();

    mlStatus = guiLabel + " - Activated";
    
    if (guiLabel == "Static Hand"){
        gestureNum = 0;
        bCaptureGesture = e.target->getChecked();
        if (!bCaptureGesture) bTrainGestures = true;
    } else if (guiLabel == "Record Voice"){
        gestureNum = 1;
        bCaptureGesture = e.target->getChecked();
        if (!bCaptureGesture) bTrainGestures = true;
    } else if( guiLabel == "Set Playhead"){
        gestureNum = 2;
        bCaptureGesture = e.target->getChecked();
        if (!bCaptureGesture) bTrainGestures = true;
    } else if( guiLabel == "Manipulate Synth"){
        gestureNum = 3;
        bCaptureGesture = e.target->getChecked();
        if (!bCaptureGesture) bTrainGestures = true;
    } else if (guiLabel == "Regression"){
        bCaptureRegression = e.target->getChecked();
        if(!bCaptureRegression) bTrainRegression = true;
    }
    
}

//--------------------------------------------------------------

void MachineLearning::onButtonEvent(ofxDatGuiButtonEvent e){
    string guiLabel = e.target->getLabel();
    
    mlStatus = guiLabel + " - Activated";
    
    if (guiLabel == "Clear All" ){
        trainingSetReg.clear();
        trainingSet.clear();
    } else if (guiLabel == "Regression Train"){
        bTrainRegression = true;
    } else if (guiLabel == "Gesture Train"){
        bTrainGestures = true;
    }
}

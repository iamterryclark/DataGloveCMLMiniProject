//
//  GrainSynthesis.cpp
//  DataGloveMiniProject
//
//  Created by Terry Clark on 28/12/2017.
//

#include "GrainSynthesis.hpp"

void GrainSynthesis::setup(int _sampleRate, int _bufferSize)
{
    //For Drawing
    curXpos = curYpos = 0.0;
    prevXpos = prevYpos = 0.0;
    
    //Set inital values for live recording
    setPHPosition = false;
    recLiveInput = false;
    updatePlayheadEvent = false;
    recMix = 0.0;
    
    //Set initial values to sample stretcher
    grainRate = 1.0;
    grainLength = 0.25;
    grainSpeed = 0.9;
    grainOverlaps = 4;
    
    sampleRate = _sampleRate;
    bufferSize = _bufferSize;
    
    recMix = 0.0;
    playHead = 0.0;
    volume = 0.9;
    
    // load sample data
    sample.load(ofToDataPath("400Frames(60fps).wav"));
    // setup pointer to sample stretcher
    sampleStretcher = new maxiTimePitchStretch<windowFunction, maxiSample>(&sample);
    ofxMaxiSettings::setup(sampleRate, 2, bufferSize);
    
    //Set where the gui will be positioned
    grainGui = new ofxDatGui( ofxDatGuiAnchor::TOP_RIGHT );
    grainGui->setAssetPath(""); //this has to be done due to bug with gui
    
    //Setup Gui Sliders/Buttons
    grainGui->addHeader("Granular Synthesis");
    grainGui->addSlider("Grain Rate", -1.5, 1.5, grainRate);
    grainGui->addSlider("Grain Length", 0.01, 0.2, grainLength);
    grainGui->addSlider("Grain Speed", 0.0, 2.0, grainSpeed);
    grainGui->addSlider("Grain Overlaps", 1, 20, grainOverlaps);
    grainGui->addSlider("Playhead Position", 0.0, 1.0, playHead);
    grainGui->addSlider("Volume", 0.0, 1.0, volume);
    grainGui->addToggle("Set Playhead Position");
    grainGui->addToggle("Record Live Input");
   // gui->addBreak();
    
    //Setup GrainGui Events
    grainGui->onToggleEvent(this, &GrainSynthesis::onToggleEvent);
    grainGui->onSliderEvent(this, &GrainSynthesis::onSliderEvent);
}

void GrainSynthesis::updatePlayhead(){
    if(!setPHPosition) {
        if(updatePlayheadEvent){
            sampleStretcher->setPosition(ofMap(sample.getRecordPosition(),0,LENGTH,0.0,1.0));
        }
        updatePlayheadEvent = false;
    } else {
        updatePlayheadEvent = true;
    }
}

void GrainSynthesis::audioReceived(float * input, int bufferSize, int nChannels){
    for (unsigned i = 0; i < bufferSize; i++) {
        sample.loopRecord(input[i*nChannels], recLiveInput, recMix);
    }
}

void GrainSynthesis::audioRequested(float * output, int bufferSize, int nChannels){
    for (unsigned i = 0; i < bufferSize; i++){
    // loop over all samples in buffer
        if(setPHPosition){
            sampleStretcher->setPosition(playHead);
        }
        
        // read wave data from sample file wia sample stretcher
        sound = sampleStretcher->play(grainSpeed, grainRate, grainLength, (int)grainOverlaps);
        
        // mix current sample to stereo channels.
        // panning of 0.5 sets sample output equally to both channels. 0.0 = left, 1.0 = right
        myMix.stereo(sound, mixerOutput, 0.5);
        
        if (recLiveInput){
            // write sample data to buffer
            output[i*nChannels] = 0.0; // left channel
            output[i*nChannels+1] = 0.0; // right channel
        } else {
            // write sample data to buffer
            output[i*nChannels] = mixerOutput[0] * volume; // left channel
            output[i*nChannels+1] = mixerOutput[1] * volume; // right channel
        }
    }
}

void GrainSynthesis::draw(){
    ofBackground(0);
    ofSetColor(255);
    ofFill();
    
    const float waveformWidth  = ofGetWidth() - 40;
    const float waveformHeight = 200;
    
    float top = ofGetHeight() - waveformHeight - 20;
    float left = 20;
    
    // draw the audio waveform
    for(int i= 0; i < LENGTH; i+=bufferSize){
        curXpos = ofMap(i,0,LENGTH,left,waveformWidth+20);
        curYpos = ofMap(sample.temp[i],-32768,32768, top, waveformHeight+top);
        ofSetColor(ofColor::yellow);
        ofDrawEllipse(curXpos, curYpos, 2, 2);
        ofDrawLine(curXpos, curYpos, prevXpos, prevYpos);
        if(i < LENGTH-bufferSize){
            prevXpos = curXpos;
            prevYpos = curYpos;
        } else {
            prevXpos = left;
            prevYpos = waveformHeight + top;
        }
    }
    
    // draw a playhead over the waveform
    ofSetColor(ofColor::white);
    ofDrawLine(left + sampleStretcher->getNormalisedPosition() * waveformWidth, top,
           left + sampleStretcher->getNormalisedPosition() * waveformWidth, top + waveformHeight);
    ofDrawBitmapString("PlayHead", left + sampleStretcher->getNormalisedPosition() * waveformWidth-69, top+30);
    
    // Draw Current Recording Position
    float sampRecPerc = (float)sample.getRecordPosition() / (float)LENGTH;
    ofSetColor(ofColor::red);
    ofDrawLine(left + sampRecPerc * waveformWidth, top, left + sampRecPerc * waveformWidth, top + waveformHeight);
    ofDrawBitmapString("RecPos", left + sampRecPerc * waveformWidth-52, top+15);
    
    // draw a frame around the whole thing
    ofSetColor(ofColor::white);
    ofNoFill();
    ofDrawRectangle(left, top, waveformWidth, waveformHeight);
}

//--------------------------------------------------------------
void GrainSynthesis::onSliderEvent(ofxDatGuiSliderEvent e){
    string guiLabel = e.target->getLabel();
    if(guiLabel == "Grain Rate"){
        grainRate = e.value;
    } else if(guiLabel == "Grain Length"){
        grainLength = e.value;
    } else if(guiLabel == "Grain Speed"){
        grainSpeed = e.value;
    } else if(guiLabel == "Grain Overlaps"){
        grainOverlaps = e.value;
    } else if(guiLabel == "Playhead Position"){
        playHead = e.value;
    } else if(guiLabel == "Volume"){
        volume = e.value;
    }
}

//--------------------------------------------------------------
void GrainSynthesis::onToggleEvent(ofxDatGuiToggleEvent e){
    string guiLabel = e.target->getLabel();
    
    if(guiLabel == "Set Playhead Position"){
        setPHPosition = e.target->getChecked();
    } else if(guiLabel == "Record Live Input"){
        recLiveInput = e.target->getChecked();
    }
}




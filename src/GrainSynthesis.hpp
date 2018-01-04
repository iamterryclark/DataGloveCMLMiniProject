#pragma once

//
//  GrainSynthesis.hpp
//  DataGloveMiniProject
//
//  Created by Terry Clark on 28/12/2017.
//

/*
 References:
- Granular Synthesis:
Sample Granular Synthesis Examples from Mick Grierson
 
 - Example Project with Live Audio and
 Granular Synthesis:
 https://github.com/JoshuaBatty/LiveAudioGranularSynthesis-Maxim
 */

#include "ofMain.h"
#include "ofxMaxim.h"
#include "ofxDatGui.h"

// type definition of window function to easily switch between options.
// https://en.wikipedia.org/wiki/Window_function
// available options in ofxMaxim - see maxiGrains.h for details:
// - hannWinFunctor
// - hammingWinFunctor
// - cosineWinFunctor
// - rectWinFunctor
// - triangleWinFunctor
// - triangleNZWinFunctor
// - blackmanHarrisWinFunctor
// - blackmanNutallWinFunctor
// - gaussianWinFunctor
typedef blackmanNutallWinFunctor windowFunction;

#define LENGTH 294000

class GrainSynthesis {

public:
    void setup(int _sampleRate, int _bufferSize);
    void updatePlayhead();
    void audioReceived(float * input, int bufferSize, int nChannels);
    void audioRequested(float * output, int bufferSize, int nChannels);
    void draw();
    
    //GUI Events
    void onDropdownEvent(ofxDatGuiDropdownEvent e);
    void onSliderEvent(ofxDatGuiSliderEvent e);
    void onToggleEvent(ofxDatGuiToggleEvent e);
    
    //Maxim Stuff
    maxiSample sample;
    maxiTimePitchStretch<windowFunction, maxiSample> *sampleStretcher;
    maxiMix myMix;
    
    //Gui Setup
    ofxDatGui* grainGui;
    
    //Granular Synth Params
    double grainRate;
    double grainLength;
    double grainSpeed;
    int grainOverlaps;
    
    //Recording Params
    bool setPHPosition;
    bool updatePlayheadEvent;
    bool recLiveInput;
    float playHead;
    float recMix;
   
    //Output
    double sound;
    double mixerOutput[2];
    float volume;
    int sampleRate;
    int bufferSize;

    //Drawing Waveform
    float curXpos, curYpos;
    float prevXpos, prevYpos;
   
};

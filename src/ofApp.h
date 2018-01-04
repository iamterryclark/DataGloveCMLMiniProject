#pragma once

/*
    DataGloveMiniProject

    // Components:
    - Input: Glove with conductive material on each finger
    - Output: Live Granular Synthesis on voice
 
    // References:
    - Recording Audio: Demystifying OpenFrameworks Book page. 172

*/

#include "ofMain.h"
#include "GrainSynthesis.hpp"
#include "GloveCapture.hpp"
#include "MachineLearning.hpp"

class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();

        void audioIn(float * input, int bufferSize, int nChannels);
        void audioOut(float * output, int bufferSize, int nChannels);
    
        //Audio Setup
        ofSoundStream soundStream;

        //Audio Constants
        int sampleRate; //Sample rate of sound
        int bufferSize; //How much of sample will be processed
    
        //Granular Synthesis
        GrainSynthesis grainSynthesis;
    
        //Glove Capture Device
        GloveCapture* glove;
    
        //Machine Learning
        MachineLearning* ml;
 
};

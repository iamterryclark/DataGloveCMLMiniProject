#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    ofSetFrameRate(60);
    ofSetWindowTitle("Data Glove, Machine Learning, Granular Synthesis");
    //ofSetWindowShape(2048, 1536);
    // set audio sample rate to standard high resolution: 44.1kHz => 44100 samples per second
    sampleRate = 44100;
    // set number of samples per buffer
    bufferSize = 256;

    //See list of audio devices
    //soundStream.printDeviceList();
    
    //Start the sound output in stereo (2 channels)
    //and sound input in mono (1 channel)
    // 2 output channels,
    // 1 input channels
    // 44100 samples per second
    // 256 samples per buffer
    // 4 num buffers (latency)
    soundStream.setup(this, 2, 1, sampleRate, bufferSize, 4);
    soundStream.setDeviceID(0);
    
    grainSynthesis.setup(sampleRate, bufferSize);
    
    glove = new GloveCapture();
    ml = new MachineLearning(glove, &grainSynthesis);
    
}

//--------------------------------------------------------------

void ofApp::update(){
    grainSynthesis.updatePlayhead();
    glove->update();
    ml->update();
}

//--------------------------------------------------------------

void ofApp::draw(){
    grainSynthesis.draw();
    glove->draw(10, 120);
    ml->draw(10, 300);
}

//--------------------------------------------------------------

void ofApp::audioIn(float * input, int bufferSize, int nChannels){
    grainSynthesis.audioReceived(input, bufferSize, nChannels);
}

//--------------------------------------------------------------

void ofApp::audioOut(float * output, int bufferSize, int nChannels){
    grainSynthesis.audioRequested(output, bufferSize, nChannels);
}



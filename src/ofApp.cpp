#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    oscReceiver.setup(PORT);
    
    // initialize to HUB_IP for heartbeat?
    oscSender.setup(HUB_IP, PORT);
    
    // initialize interval time
    lastCallHomeTime = 0;
    

}

//--------------------------------------------------------------
void ofApp::update(){
    
    int time = ofGetElapsedTimeMillis();
    
    if (time - lastCallHomeTime > CALL_HOME_INTERVAL) {
        lastCallHomeTime = time;
        callHome();
    }
    
    
    // check for waiting messages
	while(oscReceiver.hasWaitingMessages()){
		// get the next message
		ofxOscMessage m;
		oscReceiver.getNextMessage(&m);
        
        // handle heartbeat
        if(m.getAddress() == "/pulse"){
			handlePulse(&m);
		}
        
        // example of handling a /mouse/pressed message
        if(m.getAddress() == "/mouse/pressed"){
			cout << "mouse pressed at " << m.getArgAsInt32(0) << ", " << m.getArgAsInt32(1) << endl;
		}
	}
}

void ofApp::callHome() {
    cout << "calling home" << endl;
    
    ofxOscMessage m;
	m.setAddress("/call/home");
	oscSender.sendMessage(m);
}

void ofApp::handlePulse(ofxOscMessage *m) {
    // the pulse message passes along all client IPs that have registered with the hub
    // so that we can register them in each individual client node, removing the reliance
    // on the central hub to pass messages.
    
    cout << "pulse received" << endl;
    
    int numArgs = m->getNumArgs();
    for(int i=0; i<numArgs; i++) {
        string ip = m->getArgAsString(i);
        
        // Check to see if this IP already is registered as a client
        map<string, oscClient>::iterator it = clients.find(ip);
        oscClient cl;
        if(it == clients.end()) {
            // no client found for this IP, add it to the map of clients.
            cout << "registering new client" << endl;
            clientIps.insert(ip);
            ofxOscSender & _sender = clients[ip].sender;
            _sender.setup(ip, PORT);
        }
    }
}

void ofApp::sendMessage(ofxOscMessage &m) {
    
    // here we iterate through the registered clients, sending the heartbeat message to them all.
    
    for(map<string, oscClient>::iterator i = clients.begin(); i != clients.end(); i++) {
        ofxOscSender & _sender = i->second.sender;
        cout << "sending " << m.getAddress() << " to " << i->first << endl;
        _sender.sendMessage(m);
    }
}

//--------------------------------------------------------------
void ofApp::draw(){

}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    cout << "key pressed: " << key << endl;
    ofxOscMessage m;
    m.setAddress("/key/pressed");
    m.addIntArg(key);
    sendMessage(m);
    

    
    // the key is a note
    if(96 < key && key < 104) {
        
        cout << "note is a key!" << endl;
        
        int midiNote;
        bool noteIsSet;
        ofxOscMessage noteMessage;

        noteMessage.setAddress("/key/note");
        noteIsSet = false;
        
        switch(key) {
            // a
            case 97: midiNote = 69; noteIsSet = true; break;
            // b
            case 98: midiNote = 71; noteIsSet = true; break;
            // c
            case 99: midiNote = 60; noteIsSet = true; break;
            // d
            case 100: midiNote = 62; noteIsSet = true; break;
            // e
            case 101: midiNote = 64; noteIsSet = true; break;
            // f
            case 102: midiNote = 65; noteIsSet = true; break;
            // g
            case 103: midiNote = 67; noteIsSet = true; break;
        }
        
        if(noteIsSet) {
            cout << "sending MIDI value " << midiNote << endl;
            noteMessage.addIntArg(midiNote);
            sendMessage(noteMessage);
        }
    }
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
    cout << "mouse pressed locally" << endl;
    ofxOscMessage m;
    m.setAddress("/mouse/pressed");
    m.addIntArg(x);
    m.addIntArg(y);
    sendMessage(m);

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

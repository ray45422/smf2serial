#include "midilib.h"
struct Listener: MIDILib::MIDIEventListener{
	void noteOn(MIDILib::Note note) override{
		if(note.channel != 0){
			return;
		}
		tone(11,note.freq);
	}
	void noteOff(MIDILib::Note note) override{
		if(note.channel != 0){
			return;
		}
		noTone(11);
	}
};

Listener listener;

MIDILib::MIDI midi(&listener);
void setup() {
	Serial.begin(115200);
	Serial1.begin(115200);
}
void loop() {
	if(Serial.available()){
		byte a = Serial.read();
		midi.receive(a);
	}
	midi.tick();
}

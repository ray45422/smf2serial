#include "midilib.h"
char myChannel = 0;
char lastNote = -1;
struct Listener: MIDILib::MIDIEventListener{
	void noteOn(MIDILib::Note note, byte *rawData, byte dataLen) override{
		Serial.println(note.note);
		if(note.channel != myChannel){
			return;
		}
		if(lastNote == -1){
			tone(11,note.freq);
			lastNote = note.note;
		}
	}
	void noteOff(MIDILib::Note note, byte *rawData, byte dataLen) override{
		if(note.channel != myChannel){
			return;
		}
		if(lastNote == note.note){
			noTone(11);
			lastNote = -1;
		}
	}
};

Listener listener;

MIDILib::MIDI midi(&listener);
void setup() {
	Serial.begin(115200);
}
void loop() {
	if(Serial.available()){
		byte a = Serial.read();
		midi.receive(a);
	}
	midi.tick();
}

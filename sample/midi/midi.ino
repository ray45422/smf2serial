#include "midilib.h"
class Listener: MIDILib::MIDIEventListener{
	void noteOn(uint16_t freq, byte velocity) override{
		tone(11,freq);
	}
	void noteOff(uint16_t freq, byte velocity) override{
		noTone(11);
	}
};

Listener* listener = new Listener;

MIDILib::MIDI* midi = new MIDILib::MIDI((MIDILib::MIDIEventListener*)listener);
void setup() {
	Serial.begin(115200);
}
void loop() {
	if(Serial.available()){
		byte a = Serial.read();
		midi->receive(a);
	}
	midi->tick();
}

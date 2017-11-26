#include "midilib.h"
char myChannel = 0;
char slaves = 1;
char lastNotes[16];
char lastNote = -1;
void slaveSend(char channel, byte* data, byte len){
	if(MIDILib::isChannelMessage(data[0])){
		data[0] = (data[0] & 0xf0) | (channel & 0x0f);
	}
	for(int i = 0; i < len; i++){
		Serial1.write(data[i]);
	}
}
bool isPlaying(char note){
	for(int i = 0; i < slaves; ++i){
		if(lastNotes[i] == note){
			return true;
		}
	}
	return lastNote == note;
}
struct Listener: MIDILib::MIDIEventListener{
	void noteOn(MIDILib::Note note, byte* rawData, byte dataLen) override{
		if(note.channel != myChannel){
			return;
		}
		if(isPlaying(note.note)){
			return;
		}
		for(int i = 0; i < slaves; ++i){
			if(lastNotes[i] == -1){
				slaveSend(i, rawData, dataLen);
				lastNotes[i] = note.note;
				return;
			}
		}
		if(lastNote == -1){
			tone(11,note.freq);
			lastNote = note.note;
		}
	}
	void noteOff(MIDILib::Note note, byte* rawData, byte dataLen) override{
		if(note.channel != myChannel){
			return;
		}
		for(int i = 0; i < slaves; ++i){
			if(lastNotes[i] == note.note){
				slaveSend(i, rawData, dataLen);
				lastNotes[i] = -1;
				return;
			}
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
	for(int i = 0; i < 16; ++i){
		lastNotes[i] = -1;
	}
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

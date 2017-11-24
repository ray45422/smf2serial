#include<Arduino.h>
#include "midilib.h"
namespace MIDILib{
	bool isStatus(uint8_t data){
		return (data & 0x80) >> 7;
	}
	bool isData(uint8_t data){
		return !isStatus(data);
	}
	bool isChannelMessage(uint8_t data){
		return isStatus(data) && (data <= 0xEF);
	}
	bool isSystemMessage(uint8_t data){
		return isStatus(data) && (data > 0xEF);
	}
	MIDI::MIDI(MIDIEventListener* listener){
		this->listener = listener;
	}
	void MIDI::receive(uint8_t data){
		if(isStatus(data)){
			type = data & 0xf0;
			subType = data & 0x0f;
			this->data[0] = data;
			index = 1;
		}else{
			this->data[index++] = data;
		}
	}
	void MIDI::tick(){
		if(index == 0){
			return;
		}
		Note note;
		switch(type){
			case NoteOn:
				if(index < 3){
					return;
				}
				note.channel = subType;
				note.note = data[1];
				note.velocity = data[2];
				note.freq = applyPitch(note.note, note.channel);
				listener->noteOn(note, data, index);
				break;
			case NoteOff:
				if(index < 2){
					return;
				}
				note.channel = subType;
				note.note = data[1];
				note.velocity = data[2];
				note.freq = applyPitch(note.note, note.channel);
				listener->noteOff(note, data, index);
				break;
			case PitchBend:
				if(index < 3){
					return;
				}
				pitchBend[subType] = (data[2] << 7 | (data[1] & 0x7f)) - 8192;
				break;
		}
		index = 0;
	}
	uint16_t MIDI::applyPitch(uint8_t note, uint8_t channel){
		double a = pitchBend[channel]/8192.0;
		a = pow((1.0 + pitchRange / 12.0), a);
		return (int)(note2Freq(note) * a);
	}
	uint16_t MIDI::note2Freq(uint8_t note){
		return (int)(440 * pow(2, (note - 69) /12.0));
	}
}

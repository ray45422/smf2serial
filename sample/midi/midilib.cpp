#include<Arduino.h>
#include "midilib.h"
namespace MIDILib{
	bool isStatus(uint8_t data){
		return (data & 0x80) >> 7;
	}
	bool isData(uint8_t data){
		return !isStatus(data);
	}
	MIDI::MIDI(MIDIEventListener* listener){
		this->listener = listener;
	}
	void MIDI::receive(uint8_t data){
		if(isStatus(data)){
			type = data & 0xf0;
			subType = data & 0x0f;
			index = 0;
		}else{
			this->data[index++] = data;
		}
	}
	void MIDI::tick(){
		if(index == 0){
			return;
		}
		if(subType != 0){
			return;
		}
		switch(type){
			case NoteOn:
				if(index < 2){
					break;
				}
				listener->noteOn(note2Freq(data[0]), data[1]);
				break;
			case NoteOff:
				if(index < 1){
					break;
				}
				listener->noteOff(note2Freq(data[0]), data[1]);
				break;
			case PitchBend:
				if(index < 2){
					break;
				}
				pitchBend = (data[1] << 7 | (data[0] & 0x7f)) - 8192;
				break;
		}
	}
	uint16_t MIDI::note2Freq(uint8_t note){
		double a = pitchBend/8192.0;
		a = pow((1.0 + pitchRange / 12.0), a);
		return (int)(440 * pow(2, (note - 69) /12.0) * a);
	}
}

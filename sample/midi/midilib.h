#include<Arduino.h>
namespace MIDILib{
	enum MIDIEventType{
		NoteOff = 0x80,
		NoteOn = 0x90,
		PitchBend = 0xe0,
	};
	struct Note{
		uint8_t channel;
		uint8_t note;
		uint16_t freq;
		uint8_t velocity;
	};
	struct MIDIEventListener{
		virtual void noteOn(Note note) = 0;
		virtual void noteOff(Note note) = 0;
	};
	bool isStatus(uint8_t data);
	bool isData(uint8_t data);
	class MIDI{
	private:
		uint8_t type;
		uint8_t subType;
		uint8_t data[3];
		uint8_t index = 0;
		MIDIEventListener* listener;
		Note note;
		int pitchBend[16] = {};
		uint8_t pitchRange = 12;
	public:
		MIDI(MIDIEventListener* listener);
		void tick();
		void receive(byte data);
		uint16_t applyPitch(uint8_t note, uint8_t channel);
		uint16_t note2Freq(uint8_t note);
	};
}

#include<math.h>
namespace MIDILib{
	enum MIDIEventType{
		NoteOff = 0x80,
		NoteOn = 0x90,
		PitchBend = 0xe0,
	};
	struct Note{
		unsigned char channel;
		unsigned char note;
		unsigned int freq;
		unsigned char velocity;
	};
	struct MIDIEventListener{
		virtual void noteOn(Note note, unsigned char* rawData, unsigned char  dataLen) = 0;
		virtual void noteOff(Note note, unsigned char* rawData, unsigned char dataLen) = 0;
	};
	bool isStatus(unsigned char data);
	bool isData(unsigned char data);
	bool isChannelMessage(unsigned char data);
	bool isSystemMessage(unsigned char data);
	class MIDI{
	private:
		unsigned char type;
		unsigned char subType;
		unsigned char data[3];
		unsigned char index = 0;
		MIDIEventListener* listener;
		Note note;
		int pitchBend[16] = {};
		unsigned char pitchRange = 12;
	public:
		MIDI(MIDIEventListener* listener);
		void tick();
		void receive(unsigned char data);
		unsigned int applyPitch(unsigned char note, unsigned char channel);
		unsigned int note2Freq(unsigned char note);
	};
}

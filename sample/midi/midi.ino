enum MIDIEvent{
	NoteOff = 0x80,
	NoteOn = 0x90,
};
void setup() {
	Serial.begin(115200);
}
byte eventMSB;
byte eventLSB;
char data[5];
byte index = 0;
char lastNote = -1;
void loop() {
	if(Serial.available()){
		byte a = Serial.read();
		if(isStatus(a)){
			eventMSB = a & 0xf0;
			eventLSB = a & 0x0f;
			index = 0;
		}else{
			data[index++] = a;
		}
	}
	if(index == 0){
		return;
	}
	switch(eventMSB){
		case NoteOn:
			if(index < 2 || lastNote != -1){
				break;
			}
			if(lastNote == -1){
				tone(11, note2Freq(data[0]));
				lastNote = data[0];
			}
			break;
		case NoteOff:
			if(index < 1){
				break;
			}
			if(lastNote == data[0]){
				noTone(11);
				lastNote = -1;
			}
			break;
	}
}
bool isStatus(byte data){
	return (data & 0x80) >> 7;
}
bool isData(byte data){
	return !isStatus(data);
}
uint16_t note2Freq(byte note){
	return (int)(440 * pow(2, (note - 69) /12.0));
}

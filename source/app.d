import std.stdio;
import std.math;
import std.conv;
import std.algorithm;
import std.datetime;
import std.datetime.stopwatch: StopWatch;
import std.getopt;
import core.thread;
import midifile.midifile;
import serial.device;

SerialPort port;
uint microsecondsPerTick = 1000;
void main(string[] args){
	string devicePath = "/dev/ttyACM0";
	string filePath = "data/test.mid";
	getopt(args,
		"dev", &devicePath);
	devicePath.writeln;
	if(args.length >= 2){
		filePath = args[1];
	}
	port = new SerialPort(devicePath, dur!("msecs")(1000), dur!("msecs")(1000));
	port.speed(BaudRate.BR_115200);
	Thread.sleep(dur!("msecs")(500));
	MIDIFile midi = new MIDIFile(filePath);
	auto tracks = midi.tracks.length;
	uint[] readIndex = new uint[tracks];
	Duration offsetTime = Duration.zero;
	ulong offsetTick = 0;
	ulong curTick = 0;
	StopWatch sw;
	sw.start();
	while(true){
		ulong nextTick = ulong.max;
		uint continueCount = 0;
		for(uint i; i < tracks; i++){
			if(midi.tracks[i].length <= readIndex[i]){
				continueCount++;
				continue;
			}
			MIDIEvent event = midi.tracks[i][readIndex[i]];
			if(dur!"usecs"((event.tick - offsetTick)*microsecondsPerTick) < (sw.peek - offsetTime)){
				eventExec(event);
				switch(event.subType){
					case MIDIEvents.Tempo:
					offsetTime = sw.peek;
					offsetTick = curTick;
					event.tempo.microsecondsPerBeat.writeln;
					midi.ticksPerBeat.writeln;
					(event.tempo.microsecondsPerBeat / midi.ticksPerBeat).writeln;
					microsecondsPerTick = event.tempo.microsecondsPerBeat / midi.ticksPerBeat;
					break;
				default:
					break;
				}
				readIndex[i]++;
			}
			if(curTick <= event.tick){
				nextTick = min(nextTick, event.tick);
			}
		}
		if(continueCount == tracks){
			break;
		}
		curTick = nextTick;
	}
}
void eventExec(MIDIEvent event){
	if(event.note.channel != 0){
		return;
	}
	auto note = event.note;
	switch(event.type){
		case MIDIEventType.NoteOn:
			byte[] data = new byte[3];
			data[0] = cast(byte)(0x90 | note.channel);
			data[1] = cast(byte)note.note;
			data[2] = cast(byte)note.velocity;
			port.write(data);
			//data.dump;
			break;
		case MIDIEventType.NoteOff:
			byte[] data = new byte[2];
			data[0] = cast(byte)(0x80 | note.channel);
			data[1] = cast(byte)note.note;
			port.write(data);
			//data.dump;
			break;
		default:
			break;
	}
}
int noteToFreq(int note){
	return (440 * pow(2, (note - 69) /12.0)).to!int;
}
void dump(byte[] data){
	foreach(byte d; data){
		"%2x".writef(d);
	}
	writeln();
}

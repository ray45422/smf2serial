import std.stdio;
import std.math;
import std.conv;
import std.array;
import std.algorithm;
import std.algorithm.sorting: sort;
import std.algorithm.mutation: SwapStrategy;
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
	if(args.length >= 2){
		filePath = args[1];
	}
	MIDIFile midi = new MIDIFile(filePath);
	MIDIEvent[] events;
	Duration offsetTime = Duration.zero;
	ulong offsetTick = 0;
	foreach(MIDIEvent[] eventss; midi.tracks){
		foreach(MIDIEvent event; eventss){
			events ~= event;
		}
	}
	events.sort!("a.tick < b.tick", SwapStrategy.stable);
	port = new SerialPort(devicePath, dur!("msecs")(1000), dur!("msecs")(1000));
	port.speed(BaudRate.BR_115200);
	Thread.sleep(dur!("msecs")(500));
	StopWatch sw;
	sw.start();
	foreach(MIDIEvent event; events){
		while(dur!"usecs"((event.tick - offsetTick)*microsecondsPerTick) > (sw.peek - offsetTime)){
		}
		eventExec(event);
		switch(event.subType){
			case MIDIEvents.Tempo:
				offsetTime = sw.peek;
				offsetTick = event.tick;
				microsecondsPerTick = event.tempo.microsecondsPerBeat / midi.ticksPerBeat;
				break;
			default:
				break;
		}
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
		case MIDIEventType.PitchWheel:
			int pitch = event.pitch.pitch + 8192;
			byte[] data = new byte[3];
			data[0] = cast(byte)(0xE0 | note.channel);
			data[1] = cast(byte)(pitch & 0x7f);
			data[2] = cast(byte)(pitch >> 7);
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


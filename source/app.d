import std.stdio;
import std.math;
import std.conv;
import std.algorithm.sorting: sort;
import std.algorithm.mutation: SwapStrategy;
import std.datetime.stopwatch: StopWatch;
import std.getopt;
import core.thread;
import midifile.midifile;
import serial.device;

SerialPort port;
double microsecondsPerTick = 1000.0;
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
	ulong offsetTick = 0;
	foreach(MIDIEvent[] eventss; midi.tracks){
		foreach(MIDIEvent event; eventss){
			events ~= event;
		}
	}
	events.sort!("a.tick < b.tick", SwapStrategy.stable);
	port = new SerialPort(devicePath, dur!("msecs")(1000), dur!("msecs")(1000));
	port.speed(BaudRate.BR_115200);
	Thread.sleep(dur!("msecs")(1000));
	StopWatch sw;
	sw.start();
	foreach(MIDIEvent event; events){
		Duration d = (dur!"usecs"((event.tick - offsetTick) * cast(long)microsecondsPerTick) - sw.peek);
		if(d > Duration.zero){
			Thread.sleep(d);
		}
		eventExec(event);
		switch(event.subType){
			case MIDIEvents.Tempo:
				sw.reset();
				offsetTick = event.tick;
				microsecondsPerTick = event.tempo.microsecondsPerBeat / midi.ticksPerBeat;
				break;
			default:
				break;
		}
	}
}
void eventExec(MIDIEvent event){
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
			byte[] data = new byte[3];
			data[0] = cast(byte)(0x80 | note.channel);
			data[1] = cast(byte)note.note;
			data[2] = 0;
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


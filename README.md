# OpTwo
Two Operator FM (or PM) oscillator for KORG NTS-1


## What's this
Two operator FM (Frequency Modulation) or PM (Phase Modulation) oscillator for KORG NTS-1.
The "FM" or "PM" algorithm in OpTwo is implemented by my recognition, so it may differs from existing synthesizer or theory.


## Prerequisite
[logue-sdk](https://github.com/korginc/logue-sdk)

If you use pre-built binary, logue-sdk is not required. Simply transfer [OpTwo.ntkdigunit](https://github.com/kachine/nts1OpTwo/raw/main/OpTwo.ntkdigunit) to your NTS-1 using NTS-1 digital Librarian software.


## How to build
You have to change PLATFORMDIR variable in Makefile, it should correspond to where you installed logue-sdk (something like below).
```Makefile
PLATFORMDIR = ${PATHTO}/logue-sdk/platform/nutekt-digital
```

Then, simply type "make" to build this project.
```sh
$ make
```


## How to use
There are 6 paramters for this oscillator:
- Operator 1 waveform

This parameter selects the waveform of operator 1 (the carrier).
Existing early generation FM synthesizer is fixed to sine wave whereas OpTwo can select from several waveforms.
Choose "WAV1" by pressing OSC and tweaking TYPE knob, then input by B knob.
|Waveform|Value(in display)|
|---|---|
|Sine wave|1|
|Triangle wave|2|
|Square wave|3|
|Sawtooth wave|4|

- Operator 2 waveform

This parameter selects the waveform of operator 2 (the modulator).
Choose "WAV2" by pressing OSC and tweaking TYPE knob, then input by B knob.
Available value is same as Operator 1.

- Feedback

This parameter controls the feedback level of modulator (OP2).
Choose "FB" by pressing OSC and tweaking TYPE knob, then input by B knob.

- Modulation mode

This parameter switches the modulation mode.
Choose "MODE" by pressing OSC and tweaking TYPE knob, then input by B knob.
|Mode|Value(in display)|
|---|---|
|FM|1|
|PM|2|

- Ratio

This parameter controls the modulator (OP2) frequency ratio. Actual ratio varies nonliner like DX21(OPP).
Mapped to OSC mode A knob(SHPE) and MIDI CC#54
The parameter range is limited within 0 to 63 which is a half of the knob, or MIDI CC#54 value 63(, greater value is treated as 63). And actual value is mapped to 0.5 to 25.95.

- Level

This parameter controls the modulator (OP2) level.
Mapped to OSC mode B knob(ALT) and MIDI CC#55

If you use NTS-1's built-in LFO, S(shape) is effective for modulator (OP2) level modulation. Ofcourse, P(pitch) is also effective for pitch modulation.


---
The names of products or companies are the trademarks or registered trademarks of their respective companies. These companies have no relation to this project.

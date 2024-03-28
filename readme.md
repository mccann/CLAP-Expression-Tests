# CLAP Expression Tests

Two minimal plugins to highlight what I believe is a bug with Bitwig's (as of 5.1.6) implementation of CLAP plugin protocol with respect to expression parameters on generated notes.

Two plugins are included
 - **Note Generator** -- generate the problematic note event sequences
 - **Note Logger** --  view CLAP events that are received on an input port

## Expectation

NOTE_ON and NOTE_EXPRESSION sent on the same sample should turn off any expression parameter smoothing by the host.

It is reasonable that an instrument might perform smoothing, but if and how should be determined by "device" that is interpreting notes. 

This is spoken directly to in clap spec: https://github.com/free-audio/clap/blob/main/include/clap/events.h#L171

> A plugin which receives a note expression at the same sample as a NOTE_ON event should apply that expression to all generated samples.
> A plugin which receives a note expression after a NOTE_ON event should initiate the voice with default values and then apply the note expression when received. 
> A plugin may make a choice to smooth note expression streams.

## Reality

Bitwig (as of 5.1.6) appears to apply smoothing to *pressure* and *brightness* parameters from a default value to the set value even when a value is set in the same sample-frame as the NOTE_ON. (Note, I didn't test all parameter types, but did observe that it doesn't happen with *tuning*)

 
## Steps to reproduce

1. Compile plugins
    - Only tested with MacOS Sonoma, XCode 15's commandline tools compiler.
2. Open Bitwig (5.1.6)
3. Use Bitwig's File Browser to navigate to the ``build/note-generator`` and ``build/note-logger`` directories and locate the plugins
4. Create an Instrument Track with **Note Generator** before **Note Logger** in the signal chain.
5. Open the GUI for each.
6. Press the "Send Note with XXXXX" button on **Note Generator** 
7. Observe that the events it sent vs which it received are different
    - Some differences are negligible, others are problematic


#### "Send Note with Tuning"

Tuning appears to work as expected.

Note Generator:
```
# Note On, and Note Expression at sample 0 of the same block
001446:256-0000:0 NOTE_ON              note_id:     -1 port: 0 channel: 0 key: 60 | 1.000000 VELOCITY
001446:256-0000:0 NOTE_EXPRESSION      note_id:     -1 port: 0 channel: 0 key: 60 | 1.000000 TUNING
# A second later, Note Off
001619:256-0000:0 NOTE_OFF             note_id:     -1 port: 0 channel: 0 key: 60 | 1.000000 VELOCITY
```

Note Logger: 
```
# Note On, and Note Expression at sample 0 of the same block (can't compare block# between plugins)
001441:256-0000:0 NOTE_ON              note_id:     16 port: 0 channel: 0 key: 60 | 1.000000 VELOCITY
001441:256-0000:0 NOTE_EXPRESSION      note_id:     -1 port: 0 channel: 0 key: 60 | 1.000000 TUNING
# Bitwig appears to insert this additional expression event, redundantly, shouldn't hurt anything
001442:256-0000:0 NOTE_EXPRESSION      note_id:     -1 port: 0 channel: 0 key: 60 | 1.000000 TUNING     
001614:256-0000:0 NOTE_OFF             note_id:     -1 port: 0 channel: 0 key: 60 | 1.000000 VELOCITY
```


#### "Send Note with Pressure | Brightness"

Pressure and Brightness do not work as expected.

 Note Generator for Pressure (Brightness looks the same)
```
# Note On, and Note Expression at sample 0 of the same block
043013:256-0000:0 NOTE_ON              note_id:     -1 port: 0 channel: 0 key: 64 | 1.000000 VELOCITY   
043013:256-0000:0 NOTE_EXPRESSION      note_id:     -1 port: 0 channel: 0 key: 64 | 1.000000 PRESSURE
# A second later, Note Off
043186:256-0000:0 NOTE_OFF             note_id:     -1 port: 0 channel: 0 key: 64 | 1.000000 VELOCITY
```

 Note Logger for Pressure
```
# Note On, and Note Expression at sample 0 of the same block ( so far so good! )
043008:256-0000:0 NOTE_ON              note_id:     32 port: 0 channel: 0 key: 64 | 1.000000 VELOCITY
043008:256-0000:0 NOTE_EXPRESSION      note_id:     -1 port: 0 channel: 0 key: 64 | 1.000000 PRESSURE
# Bitwig "smoothing values" inserted at sample 0 of subsequent blocks
043009:256-0000:0 NOTE_EXPRESSION      note_id:     -1 port: 0 channel: 0 key: 64 | 0.289921 PRESSURE
043010:256-0000:0 NOTE_EXPRESSION      note_id:     -1 port: 0 channel: 0 key: 64 | 0.579841 PRESSURE
043011:256-0000:0 NOTE_EXPRESSION      note_id:     -1 port: 0 channel: 0 key: 64 | 0.869762 PRESSURE
# Bitwig inserted at non-zero sample, my guess is that this is the end of the smoothing window
043011:256-0114:0 NOTE_EXPRESSION      note_id:     -1 port: 0 channel: 0 key: 64 | 1.000000 PRESSURE
# Bitwig inserts one more at block start, after window end.
043012:256-0000:0 NOTE_EXPRESSION      note_id:     -1 port: 0 channel: 0 key: 64 | 1.000000 PRESSURE
# A second later, Note Off
043181:256-0000:0 NOTE_OFF             note_id:     -1 port: 0 channel: 0 key: 64 | 1.000000 VELOCITY  
```


### Note about code

These were built to be small and focus on a minimal functionality, not intended to be examples of robust plugins.
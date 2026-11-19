# MIDI

Note-on hits the snare and sets Shell pitch from the note. A4 is 440 Hz, the usual pact with physics. Up to four hits overlap, so flams and rolls keep the previous strike instead of killing it. Several note-ons in one buffer all fire.

The **Tune** knob does not follow that note. It stays put for Standalone Play, the waveform, and WAV export, so you can keep a written pitch on the UI while MIDI plays another.

**Vel** lives on Shell. Default is on: quieter notes are quieter, shorter, and darker. At 0 the knob ignores velocity. Velocity 127 and the Play button match the full sound.

Length snaps use host BPM (Loop BPM in standalone, or 120 if the host has none).

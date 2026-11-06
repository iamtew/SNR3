# SNR3 DSP notes

- **Shell**: two decaying sines (fundamental + ~1.76×), short body, small baked chirp. Not Kick Drop / P.Hold.
- **Wires**: bandpassed noise around Tone.
- **Stick**: short HP noise burst.
- **Sample**: linear interpolate at file rate vs playback rate. No pitch shift.
- **Duck**: cosine dip on Shell, timed from Stick delay.

Length snap: Free uses decay; 1/4 1/8 1/16 follow host or Loop BPM.

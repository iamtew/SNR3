# SNR3 DSP notes

- **Shell**: two decaying partials (fundamental + ~1.76×), short body, small baked chirp. Not Kick Drop / P.Hold. Shape Sine vs Tri (`asin`) on those partials. Harm knob is extra 2nd at 2× Tune; Style Snap/Hard add on top.
- **Style**: Classic / Fat / Snap / Tight / Hard. Multiplies stick time/gain and shell/wires decay on trigger. Knobs stay put.
- **Wires**: bandpassed noise, HPF/LPF (old Tone was the band centre).
- **Stick**: short HP noise burst.
- **Sample**: linear interpolate at file rate vs playback rate. No pitch shift.
- **Duck**: cosine dip on Shell, timed from Stick delay.

Length snap: Free uses decay; 1/4 1/8 1/16 follow host or Loop BPM.

# SNR3 DSP notes

- **Shell**: two decaying partials (fundamental + ~1.76×), short body, small baked chirp. Not Kick Drop / P.Hold. Shape Sine vs Tri (`asin`) on those partials. Harm knob is extra 2nd at 2× Tune; Style Snap/Hard add on top.
- **Style**: Classic / Fat / Snap / Tight / Hard. Multiplies stick time/gain and shell/wires decay on trigger. Knobs stay put.
- **Wires**: bandpassed noise, HPF/LPF (old Tone was the band centre). Color = pink mix. Buzz = 1.2 ms feedforward comb.
- **Stick**: short HP noise burst. Hard = extra 8 kHz HP mix. Click = ~0.8 ms spike.
- **Sample**: linear interpolate at file rate vs playback rate. No pitch shift. Start / Reverse / Decay (0 = off).
- **Pan / Drive**: equal-power pan after Width extras; Drive is soft tanh, 0 = skip.
- **Duck**: cosine dip on Shell, timed from Stick delay.

Length snap: Free uses decay; 1/4 1/8 1/16 follow host or Loop BPM.

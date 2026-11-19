# SNR3 DSP notes

- **Shell**: 8 circular-membrane partials, faster decay on higher modes. Harm adds a true 2nd and stretches the ratios. Chirp / Fall are the start pitch drop (default ~5 semitones over 8 ms). Shape Sine vs Tri on the fundamental only. Stick also injects a short impulse into the head. Not Kick Drop / P.Hold.
- **Style**: Classic / Fat / Snap / Tight / Hard. Multiplies stick time/gain and shell/wires decay on trigger. Knobs stay put.
- **Wires**: bandpassed noise, HPF/LPF. Color = pink mix. Buzz = feedback comb rattle. Amplitude follows a one-pole of |Shell|.
- **Stick**: short HP noise burst (mix layer) plus the shell impulse above. Hard = extra 8 kHz HP mix. Click = ~0.8 ms spike.
- **Sample**: linear interpolate at file rate vs playback rate. No pitch shift. Start / Reverse / Decay (0 = off).
- **Pan / Drive**: equal-power pan after Width extras; Drive is soft tanh, 0 = skip.
- **Duck**: cosine dip on Shell, timed from Stick delay.
- **Vel**: scales layer amps, shortens decays, darkens Shell (tone + fewer high modes). Default is on. Play / vel 127 is the full hit.
- **Polyphony**: 4 overlapping stacks (flams / rolls). Same-block note-ons all fire.

Length snap: Free uses decay; 1/4 1/8 1/16 follow host or Loop BPM.

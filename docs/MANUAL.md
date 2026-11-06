# SNR3 manual

Snare one-shot. VST3 + Standalone. Stupid Audio. MIT.

## Main

Waveform is the layered mix you hear on Play, before post FX. Select a rail layer to overlay that stem. Mix is mute / solo / gain / phase / Delay or Pre / Send. Duck / Time dips Shell when Stick hits.

MIDI note-on retriggers and sets Shell pitch (A4 = 440 Hz). Tune is for Standalone Play, the waveform, and WAV export.

## Layers

- **Shell** — pitched body (Tune, Decay, Tone, Harm, Length, Attack). Vel / Width are global and live here.
- **Wires** — snare-wire noise (Decay, HPF, LPF, Length, Attack, Pre)
- **Stick** — beater / click (Decay, Tone, Length, Attack, Pre)
- **Sample** — user WAV/AIFF/FLAC, linear resample, no pitch. HPF, LPF, Length, Attack, Pre. Missing file = silence.

Length Auto follows the global Length snap (or Decay when that is Free). Attack is fade-in after Delay / Pre.

Style: Classic / Fat / Snap / Tight / Hard. Scales stick and decays; knobs stay put. Shape: Sine / Tri on Shell. Dist Shape on the FX page is a different control.

Length: Free / 1/4 / 1/8 / 1/16 + Hold.

## Effects

Dist → Delay → Reverb send, then 5-band post EQ. Dry / FX mix.

## Standalone

Play, Loop + BPM. Export WAV (24-bit, Norm −0.1 dBTP default on). Drag the waveform for the same WAV plus a `.snr3p` patch.

**Init Patch** (title bar) — a factory preset shows as Category / Name. Click for Init / Save / Save As / Export / Import, a factory category, or your saved list. Chevrons step the factory bank, then any extra patches. Your files live in `%AppData%\Stupid Audio\SNR3\Patches` as `.snr3p`. Saving a factory name stores your copy there; delete that file to hear the factory sound again.

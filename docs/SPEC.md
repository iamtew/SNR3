# SNR3 spec

This file is the product spec.

Snare one-shot. Similar to K1X, not a copy. Aimed at snares. Stupid Audio. MIT. Not a circuit clone.

- Hybrid snare: thin synthetic core + first-class user Sample
- Default: 909-ish (short stick, mid wires, short shell)
- Named factory bank in the patch menu (909, 808, 707, and the rest)
- Not a circuit clone, not a K1X rename

## Product

- VST3 + Standalone
- Standalone: Play (one-shot), Loop + BPM (hosts hide these; they use MIDI)
- WAV export + Norm (peak −0.1 dBTP, default on); drag waveform for the same WAV + patch
- Hybrid voice: thin synthetic core + first-class Sample (user WAV/AIFF/FLAC, no factory library; missing file = silence)
- MIDI note-on retriggers and sets Shell pitch (A4 = 440 Hz). Tune knob is for Standalone Play, waveform, and WAV export.

Default sound: 909-ish snare (short stick, mid wires, short shell). Named factory bank in the patch menu. Style combo is Classic / Fat / Snap / Tight / Hard (not K1X 808 / 707 machine names).

## Layers

Mix mute / solo / gain / phase, Delay / Pre, Send — same contract as K1X Mix.

- **Shell** — pitched body
- **Wires** — snare-wire noise
- **Stick** — beater / click
- **Sample** — user file, linear resample, no pitch shift (same as K1X Sample)

Duck / Time: dip Shell when Stick hits. Extra stem buses: Snare + Slot 1–3 (pre-Width, pre-FX). Main Output is always the full mix.

## Port from K1X (behavior, snare names)

Port patterns. Do not copy `KickVoice` / kick-named files and rename.

- Length snap: Free / 1/4 / 1/8 / 1/16 + Hold
- Per-layer EQ
- Effects page: Dist → Delay → Reverb send + 5-band post EQ
- Patches: `%AppData%\Stupid Audio\SNR3\Patches`, extension `.snr3p`
- About + GitHub update check (title-bar name + version, like TEW03 / K1X)
- Window scale 40–150%, saved with the session
- `just build-dist`, GitHub Actions Win/Linux/Mac, MSI, `docs/` gh-pages

## Do not port

- CMake (Projucer only)
- Kick Style bank names (808 / 707) and kick factory presets
- Kick Drop / P.Hold as the default snare model
- Commercial plugin code
- Allocations, locks, or system calls in `processBlock`

## Stack

- JUCE, Projucer workflow only
- C++17, APVTS for all parameters
- Just + justfile
- Jucer `version=` is the binary, About dialog, git tag, and `just build-dist` name. Tags match that string (no `v` prefix).

## UI

Warm brass, not K1X blue. Do not invent another palette.

- `#1a1410` dark brown-black
- `#c4a35a` brass
- `#e8dcc8` cream
- `#8a3a22` rust accent
- `#000000` black
- `#ffffff` white

`assets/logo.png` is the Affinity export. Source of truth. Never overwrite. `just icon` / `just docs-logo` derive the rest.

Clean interface. Waveform preview is the layered mix you hear on Play, before post FX. Editing a slot overlays that layer.

## Layout

```
src/
  PluginProcessor.h/.cpp
  PluginEditor.h/.cpp
  dsp/SnareVoice.h SnareRender.h PostFx.h Distortion.h EqBiquad.h ParametricEq.h WavExport.h
  parameters/ParameterIDs.h ParameterLayout.h
  ui/
docs/
justfile
SNR3.jucer
```

README stays small. LICENSE is MIT. `docs/` is the gh-pages site.

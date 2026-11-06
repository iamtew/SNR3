# SNR3 - Snare Drum Generator

Snare one-shot. Similar to K1X, not a copy. Aimed at snares. Stupid Audio. MIT. Not a circuit clone.

This file is the spec. Future agents bootstrap the project from it. Do not invent a parallel spec.

## Paths

- This repo: `F:\OneDrive_Mikael\OneDrive\src\SNR3`
- K1X (steal patterns): `C:\Users\Mikael\src\K1X` (OneDrive mirror: `F:\OneDrive_Mikael\OneDrive\src\K1X`)
- TEW03 (About / docs site): `C:\Users\Mikael\src\TEW03`
- JUCE: `C:\Users\Mikael\JUCE`

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

`assets/logo.png` is the Affinity export (source of truth, never overwrite) once it exists. Until then: text wordmark. `just icon` / `just docs-logo` derive the rest, same idea as K1X.

Clean interface. Waveform preview is the layered mix you hear on Play, before post FX. Editing a slot overlays that layer.

## Layout (when bootstrapping)

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

README stays small. LICENSE is MIT. `docs/` is the gh-pages site (markdown rendered like TEW03 / K1X).

## Bootstrap steps

Stop at a playable one-shot + WAV export. Factory bank and `assets/logo.png` are in. Write `AGENTS.md` and `README.md` when the project exists, from this file.

1. Projucer: SNR3, VST3 + Standalone, C++17, company Stupid Audio. Put sources under `src/`.
2. Copy recipes and structure from K1X `justfile`, `.github/workflows/dist.yml`, and docs site shell. Retarget names. Do not copy `src/dsp/KickVoice.h`.
3. Parameter IDs for Shell / Wires / Stick / Sample plus shared Mix / FX / EQ / Length.
4. Real-time voice + offline render used by waveform, Play, and WAV export (K1X `KickRender` pattern).
5. Editor: waveform, layer rail, Mix, Effects, About.
6. Playable one-shot + WAV export. Then stop.

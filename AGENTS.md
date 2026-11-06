# SNR3 – Cursor Rules

Snare one-shot VST3 + Standalone. Similar to K1X, not a copy. Aimed at snares. Stupid Audio. MIT. Not a circuit clone.

## Stack
- JUCE (Projucer workflow only – no CMake)
- C++17, real-time safe: no allocations, no locks, no system calls in processBlock
- APVTS for all parameters
- Just + justfile
- `assets/logo.png` is the Affinity export. Source of truth. Never overwrite. `just icon` / `just docs-logo` derive the rest.

## Layout
src/
 PluginProcessor.h/.cpp
 PluginEditor.h/.cpp
 dsp/SnareVoice.h SnareRender.h PostFx.h Distortion.h EqBiquad.h ParametricEq.h WavExport.h
 parameters/ParameterIDs.h ParameterLayout.h
 ui/
docs/
justfile
SNR3.jucer

## Version
- Jucer `version=` is the binary, About dialog, git tag, and `just build-dist` name.
- Git tags must match that string (`0.0.1`). No `v` prefix.

## Do NOT
- Introduce CMake
- Allocate on the audio thread
- Copy commercial plugin code
- Copy `KickVoice` / kick-named files and rename
- Use K1X 808 / 707 machine names on the Style combo (SNR3 Style is Classic / Fat / Snap / Tight / Hard)

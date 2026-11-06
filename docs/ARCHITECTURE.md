# SNR3 architecture

Projucer only. C++17. APVTS. No allocations in `processBlock`.

```
MIDI / Play → SnareStack (Shell, Wires, Stick, Sample)
  → per-layer EQ → Mix amps → Width
  → PostFx (Dist → Delay → Rev) → post EQ → Output
Snare bus = four layers pre-Width, pre-FX
Slot 1–3 buses exist and stay silent until extra voices exist
```

Offline `renderShot` / `renderWithPost` share the same stack as the audio thread. Waveform uses `renderShot` (pre post FX).

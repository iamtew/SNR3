# SNR3 architecture

Projucer only. C++17. APVTS. No allocations in `processBlock`.

```
MIDI / Play → SnareBank (4× SnareStack: Shell, Wires, Stick, Sample)
  → per-layer EQ → Drive → Mix amps → Width → Pan extra
  → PostFx (Dist → Delay → Rev) → post EQ → Output
Snare bus = four layers pre-Width, pre-FX
```

Offline `renderShot` / `renderWithPost` share the same stack as the audio thread. Waveform uses `renderShot` (pre post FX).

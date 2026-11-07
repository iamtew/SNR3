# Layers

Four voices, one snare. Each layer has Pan and Drive. Drive 0 is bypass. Pan centre is unchanged. **Length** Auto follows the global Length snap, or Decay when that is Free. **Attack** is a fade-in after Delay / Pre.

Vel and Width are global and live on Shell, because putting them on every layer would be a joke at your expense.

Each layer also has its own EQ in the toolbar: five bands, drag a node, wheel for Q, double-click to reset a band.

## Shell

The pitched body.

- **Tune / Decay / Tone / Harm / Length / Attack**
- **Vel / Width** — global, parked here so you can find them.

## Wires

Snare-wire noise: bandpassed hiss, plus a short comb rattle if you want it to buzz like it means it.

- **Decay / HPF / LPF / Length / Attack / Pre**
- **Buzz** — a short comb rattle.
- **Color** — tilts white toward pink.

## Stick

The beater / click.

- **Decay / Tone / Length / Attack / Pre**
- **Hard** — extra brightness.
- **Click** — a separate ~0.8 ms spike, for people who think the stick was not clicky enough.

## Sample

Your file, not a factory library. WAV / AIFF / FLAC, linear resample, no pitch. A missing file is silence, not a crash, which is the polite option.

- **HPF / LPF / Length / Attack / Pre / Start / Rev / Decay**
- Decay Off plays the file through. Start skips into it.

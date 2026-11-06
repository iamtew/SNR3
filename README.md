<img src="assets/logo.png" alt="SNR3" width="160">

# SNR3

**SNR3** is a snare one-shot (909-ish default: short stick, mid wires, short shell). Hybrid voice: thin synthetic core plus a user Sample. Mix, per-layer EQ, parallel post FX, Style / Shape, length snap, WAV export with Norm.

VST3 and standalone (Play / Loop / WAV export). Stupid Audio. [MIT](LICENSE). Site: [docs/](docs/index.html). Manual: [docs/MANUAL.md](docs/MANUAL.md).

## Release

Windows, Linux, and macOS files come from GitHub Actions (`.github/workflows/dist.yml`). Run it from the Actions tab for artifacts only, or push a numeric tag such as `0.0.1` (no `v` prefix) to build and publish a GitHub Release with the files attached. After a tag release, the job writes `docs/releases.json` (latest asset filenames, no source tarballs) so the site can link Installer / VST3 / Standalone files directly. `<version>` is the `version` in `SNR3.jucer`.

The `dist` artifact holds:

- `SNR3-<version>-win-x64-vst3.zip`
- `SNR3-<version>-win-x64-standalone.zip`
- `SNR3-<version>-win-x64-installer.msi`
- `SNR3-<version>-linux-x64-vst3.zip`
- `SNR3-<version>-linux-x64-standalone.zip`
- `SNR3-<version>-mac-universal-vst3.zip`
- `SNR3-<version>-mac-universal-standalone.zip`

The MSI installs the VST3, the standalone app, or both. Defaults are `Common Files\VST3` and `Program Files\Stupid Audio\SNR3`; each folder can be changed. The standalone install adds a Start menu shortcut.

The Mac zips are ad-hoc signed, not notarized. Gatekeeper blocks them until you right-click Open, or run `xattr -dr com.apple.quarantine` on the `.vst3` and the `.app`. The Linux build uses the system ALSA, FreeType, fontconfig, and X11 libraries.

`just build-dist` on Windows still writes the three Windows files only. Building the MSI needs the .NET 8 SDK.

`just docs-screenshots` overwrites `docs/main.png`, `docs/effects.png`, and `docs/about.png`. Needs a debug standalone (`just build-debug`) and an interactive desktop.

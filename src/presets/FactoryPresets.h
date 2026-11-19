#pragma once

// Factory snares. initPatch() runs first, then these voice values, then a
// matching FactoryFx / FactoryEq row if the name is listed there.
// Categories stay in one contiguous run — the patch menu opens one submenu each.
// Length: 0=Free 1=1/4 2=1/8 3=1/16. Style: 0 Classic 1 Fat 2 Snap 3 Tight 4 Hard.
// Shape: 0 Sine 1 Tri. Sample gain stays 0 (no factory file).

#include <cstddef>

namespace snr3
{

struct FactoryPreset
{
    const char* category;
    const char* name;
    float tune, shellDec, shellTone, wiresDec, wiresTone, stickDec, stickTone;
    float shellGain, wiresGain, stickGain;
    float level, duck, duckTime;
    int length, style, shape;
    float harm, chirp, buzz, color, stickHard, stickClick;
};

// dst < 0 = distortion off. dstType 0..3 = Soft/Hard/Fold/Diode.
// dlyDiv < 0 = delay off (else 0..3 = 1/16..1/2, sync stays on).
// rev < 0 = reverb off (else room size).
struct FactoryFx
{
    const char* name;
    float dry, wet, dst;
    int dstType;
    int dlyDiv;
    float dlyFb;
    float rev, revDamp;
};

// type2 == 0 leaves band 2 at init. Types: 1 peak, 2 low shelf, 3 high shelf, 4 HP, 5 LP.
struct FactoryEq
{
    const char* name;
    int type;
    float hz, db, q;
    int type2;
    float hz2, db2, q2;
};

inline constexpr FactoryPreset factoryPresets[] = {
    // 909 — mid shell, real wires, short stick, Classic unless named otherwise
    { "909", "Punch",     185.f, 0.18f, 3500.f, 0.22f, 3500.f,  8.0f, 2500.f, 1.00f, 1.00f, 1.00f, 0.75f, 0.22f, 2.0f, 0, 0, 0, 0.00f, 0.35f, 0.25f, 0.00f, 0.00f, 0.10f },
    { "909", "Short",     205.f, 0.08f, 4200.f, 0.10f, 5400.f,  3.5f, 3600.f, 0.82f, 0.88f, 1.00f, 0.76f, 0.32f, 1.2f, 0, 2, 0, 0.00f, 0.28f, 0.22f, 0.05f, 0.10f, 0.35f },
    { "909", "Long",      160.f, 0.42f, 2400.f, 0.72f, 2200.f, 12.0f, 1800.f, 1.00f, 1.00f, 0.62f, 0.72f, 0.12f, 3.5f, 0, 1, 0, 0.08f, 0.22f, 0.40f, 0.15f, 0.00f, 0.00f },
    { "909", "Click",     225.f, 0.10f, 5200.f, 0.12f, 6800.f,  2.2f, 5200.f, 0.65f, 0.62f, 1.00f, 0.74f, 0.42f, 1.0f, 0, 2, 0, 0.00f, 0.40f, 0.18f, 0.00f, 0.25f, 0.70f },
    { "909", "Soft",      168.f, 0.34f, 1800.f, 0.38f, 1700.f, 16.0f, 1200.f, 1.00f, 0.80f, 0.32f, 0.76f, 0.06f, 4.0f, 0, 1, 0, 0.00f, 0.12f, 0.20f, 0.25f, 0.00f, 0.00f },
    { "909", "Rolling",   178.f, 0.24f, 2800.f, 0.55f, 3000.f,  9.0f, 2200.f, 0.92f, 1.00f, 0.58f, 0.75f, 0.14f, 2.8f, 0, 0, 0, 0.05f, 0.30f, 0.48f, 0.10f, 0.00f, 0.08f },
    // 808 — Fat, noise-led, pinker, less 909 rattle
    { "808", "Fat",       118.f, 0.48f, 1400.f, 0.38f, 1800.f, 14.0f, 1100.f, 0.85f, 0.95f, 0.40f, 0.72f, 0.10f, 3.5f, 0, 1, 0, 0.12f, 0.18f, 0.10f, 0.45f, 0.00f, 0.00f },
    { "808", "Rim",       280.f, 0.06f, 6200.f, 0.05f, 7000.f,  2.5f, 4800.f, 0.28f, 0.12f, 1.00f, 0.74f, 0.45f, 0.9f, 0, 2, 1, 0.00f, 0.45f, 0.05f, 0.00f, 0.35f, 0.80f },
    { "808", "Long Tail", 105.f, 0.62f, 1200.f, 0.95f, 1500.f, 16.0f, 1000.f, 0.80f, 1.00f, 0.28f, 0.68f, 0.08f, 4.5f, 0, 1, 0, 0.10f, 0.12f, 0.12f, 0.50f, 0.00f, 0.00f },
    { "808", "Bounce",    148.f, 0.26f, 2200.f, 0.28f, 2600.f,  8.0f, 2000.f, 0.78f, 0.82f, 0.70f, 0.73f, 0.20f, 2.2f, 0, 1, 0, 0.08f, 0.38f, 0.15f, 0.30f, 0.05f, 0.15f },
    { "808", "Boom",       95.f, 0.55f, 1000.f, 0.32f, 1600.f, 18.0f,  900.f, 1.00f, 0.55f, 0.38f, 0.70f, 0.08f, 4.0f, 0, 1, 0, 0.18f, 0.15f, 0.08f, 0.40f, 0.00f, 0.00f },
    { "808", "Deep",       82.f, 0.72f,  700.f, 0.42f, 1100.f, 22.0f,  650.f, 1.00f, 0.40f, 0.22f, 0.70f, 0.05f, 5.0f, 0, 1, 0, 0.22f, 0.08f, 0.06f, 0.55f, 0.00f, 0.00f },
    // 707 — thin, metallic, Tri, click toys
    { "707", "Dry",       245.f, 0.10f, 6200.f, 0.12f, 7000.f,  3.5f, 4500.f, 0.48f, 0.70f, 1.00f, 0.75f, 0.30f, 1.4f, 0, 3, 1, 0.05f, 0.22f, 0.28f, 0.00f, 0.20f, 0.40f },
    { "707", "Snap",      270.f, 0.06f, 7800.f, 0.08f, 8500.f,  1.8f, 6200.f, 0.38f, 0.55f, 1.00f, 0.74f, 0.42f, 0.9f, 0, 2, 1, 0.00f, 0.35f, 0.22f, 0.00f, 0.30f, 0.65f },
    { "707", "Electro",   215.f, 0.14f, 5200.f, 0.16f, 5800.f,  5.0f, 3800.f, 0.62f, 0.78f, 0.90f, 0.72f, 0.26f, 1.8f, 0, 3, 1, 0.10f, 0.28f, 0.32f, 0.08f, 0.15f, 0.30f },
    { "707", "Thin",      320.f, 0.05f, 9000.f, 0.06f, 9800.f,  1.2f, 7500.f, 0.22f, 0.48f, 1.00f, 0.74f, 0.38f, 0.8f, 0, 2, 1, 0.00f, 0.20f, 0.18f, 0.00f, 0.25f, 0.75f },
    { "707", "Metallic",  340.f, 0.12f, 8500.f, 0.18f,11000.f,  2.8f, 8000.f, 0.35f, 0.85f, 0.95f, 0.68f, 0.28f, 1.3f, 0, 3, 1, 0.22f, 0.18f, 0.55f, 0.00f, 0.20f, 0.45f },
    { "707", "Tick",      380.f, 0.04f,10000.f, 0.05f,11500.f,  0.7f, 9000.f, 0.12f, 0.28f, 1.00f, 0.76f, 0.50f, 0.6f, 0, 2, 1, 0.00f, 0.15f, 0.10f, 0.00f, 0.40f, 0.90f },
    // EDM — Snap/Hard, duck, short shell
    { "EDM", "Festival",  192.f, 0.12f, 4800.f, 0.14f, 5200.f,  4.5f, 3400.f, 0.78f, 0.80f, 1.00f, 0.64f, 0.38f, 1.4f, 0, 4, 0, 0.12f, 0.32f, 0.18f, 0.05f, 0.35f, 0.40f },
    { "EDM", "Big Room",  170.f, 0.10f, 4200.f, 0.12f, 4600.f,  3.8f, 3200.f, 0.75f, 0.72f, 1.00f, 0.60f, 0.42f, 1.2f, 0, 4, 0, 0.15f, 0.28f, 0.15f, 0.08f, 0.40f, 0.45f },
    { "EDM", "Future",    158.f, 0.20f, 3000.f, 0.24f, 3400.f,  7.0f, 2400.f, 0.88f, 0.68f, 0.78f, 0.70f, 0.24f, 2.2f, 0, 2, 0, 0.18f, 0.42f, 0.12f, 0.12f, 0.20f, 0.25f },
    { "EDM", "House",     198.f, 0.16f, 3800.f, 0.20f, 4000.f,  6.5f, 2700.f, 0.85f, 0.92f, 0.85f, 0.74f, 0.26f, 1.8f, 0, 2, 0, 0.05f, 0.30f, 0.22f, 0.05f, 0.15f, 0.22f },
    { "EDM", "Trance",    210.f, 0.28f, 3600.f, 0.36f, 3000.f,  9.5f, 2100.f, 0.90f, 0.82f, 0.62f, 0.74f, 0.16f, 3.0f, 0, 0, 0, 0.08f, 0.22f, 0.28f, 0.10f, 0.05f, 0.10f },
    { "EDM", "Progressive", 175.f, 0.26f, 2600.f, 0.34f, 2400.f, 11.0f, 1800.f, 0.95f, 0.75f, 0.55f, 0.75f, 0.14f, 3.2f, 0, 1, 0, 0.10f, 0.18f, 0.20f, 0.18f, 0.00f, 0.05f },
    // Techno
    { "Techno", "Warehouse", 195.f, 0.10f, 4000.f, 0.12f, 4200.f,  4.0f, 3000.f, 0.72f, 0.78f, 0.92f, 0.72f, 0.32f, 1.6f, 0, 3, 0, 0.00f, 0.25f, 0.18f, 0.00f, 0.20f, 0.30f },
    { "Techno", "Dub",       152.f, 0.38f, 1600.f, 0.68f, 1400.f, 14.0f, 1200.f, 1.00f, 0.90f, 0.35f, 0.74f, 0.08f, 4.0f, 0, 1, 0, 0.08f, 0.12f, 0.22f, 0.35f, 0.00f, 0.00f },
    { "Techno", "Hypnotic",  168.f, 0.32f, 2000.f, 0.48f, 1800.f, 12.0f, 1400.f, 0.85f, 0.88f, 0.38f, 0.76f, 0.08f, 4.5f, 0, 1, 0, 0.12f, 0.10f, 0.30f, 0.22f, 0.00f, 0.00f },
    { "Techno", "Peak Time", 205.f, 0.09f, 5400.f, 0.11f, 5800.f,  3.2f, 4000.f, 0.70f, 0.75f, 1.00f, 0.66f, 0.40f, 1.2f, 0, 4, 0, 0.10f, 0.35f, 0.16f, 0.00f, 0.45f, 0.50f },
    { "Techno", "Industrial", 142.f, 0.16f, 2400.f, 0.18f, 2000.f,  6.0f, 1800.f, 0.78f, 0.58f, 0.88f, 0.58f, 0.28f, 2.0f, 0, 4, 1, 0.35f, 0.20f, 0.25f, 0.15f, 0.40f, 0.30f },
    { "Techno", "Berlin",    180.f, 0.20f, 3000.f, 0.28f, 2800.f,  7.5f, 2200.f, 0.80f, 0.95f, 0.68f, 0.73f, 0.18f, 2.4f, 0, 3, 0, 0.06f, 0.22f, 0.35f, 0.08f, 0.10f, 0.15f },
    // Hardcore
    { "Hardcore", "Gabber",     215.f, 0.06f, 6000.f, 0.08f, 6500.f,  2.2f, 5000.f, 0.58f, 0.62f, 1.00f, 0.52f, 0.45f, 1.0f, 0, 4, 0, 0.20f, 0.30f, 0.12f, 0.00f, 0.70f, 0.55f },
    { "Hardcore", "Frenchcore", 240.f, 0.05f, 7200.f, 0.06f, 7800.f,  1.5f, 6200.f, 0.45f, 0.58f, 1.00f, 0.50f, 0.50f, 0.8f, 0, 4, 1, 0.18f, 0.28f, 0.10f, 0.00f, 0.80f, 0.70f },
    { "Hardcore", "Terror",     155.f, 0.08f, 3800.f, 0.10f, 4200.f,  3.5f, 3000.f, 0.72f, 0.50f, 1.00f, 0.48f, 0.42f, 1.4f, 0, 4, 0, 0.30f, 0.25f, 0.15f, 0.10f, 0.65f, 0.45f },
    { "Hardcore", "Uptempo",    265.f, 0.04f, 8000.f, 0.05f, 8500.f,  1.0f, 7000.f, 0.35f, 0.48f, 1.00f, 0.55f, 0.48f, 0.7f, 0, 4, 1, 0.10f, 0.22f, 0.08f, 0.00f, 0.75f, 0.80f },
    { "Hardcore", "Hardstyle",  188.f, 0.18f, 3600.f, 0.20f, 3800.f,  6.0f, 2600.f, 0.85f, 0.70f, 0.85f, 0.60f, 0.30f, 1.8f, 0, 4, 0, 0.15f, 0.28f, 0.14f, 0.05f, 0.50f, 0.35f },
    { "Hardcore", "Rawstyle",   160.f, 0.12f, 2800.f, 0.14f, 3000.f,  4.5f, 2400.f, 0.80f, 0.58f, 0.92f, 0.52f, 0.35f, 1.6f, 0, 4, 1, 0.28f, 0.22f, 0.18f, 0.12f, 0.60f, 0.40f },
    // Distorted — ugly before the box
    { "Distorted", "Fuzz",       148.f, 0.24f, 1800.f, 0.26f, 1600.f,  9.0f, 1500.f, 0.88f, 0.62f, 0.70f, 0.52f, 0.18f, 2.6f, 0, 4, 0, 0.40f, 0.18f, 0.22f, 0.30f, 0.35f, 0.15f },
    { "Distorted", "Clip",       202.f, 0.08f, 5600.f, 0.10f, 6000.f,  3.0f, 4500.f, 0.60f, 0.68f, 1.00f, 0.48f, 0.38f, 1.2f, 0, 4, 0, 0.12f, 0.32f, 0.12f, 0.00f, 0.55f, 0.50f },
    { "Distorted", "Saturation", 172.f, 0.26f, 2400.f, 0.30f, 2200.f, 10.0f, 1800.f, 0.95f, 0.75f, 0.58f, 0.64f, 0.14f, 3.0f, 0, 1, 0, 0.22f, 0.20f, 0.18f, 0.25f, 0.15f, 0.08f },
    { "Distorted", "Chainsaw",   228.f, 0.07f, 6800.f, 0.10f, 7200.f,  2.5f, 5600.f, 0.48f, 0.78f, 1.00f, 0.48f, 0.40f, 1.0f, 0, 4, 1, 0.45f, 0.28f, 0.35f, 0.00f, 0.50f, 0.40f },
    { "Distorted", "Cone",       118.f, 0.42f,  700.f, 0.22f, 1000.f, 18.0f,  650.f, 1.00f, 0.32f, 0.40f, 0.55f, 0.10f, 4.5f, 0, 1, 0, 0.30f, 0.10f, 0.08f, 0.45f, 0.10f, 0.00f },
    { "Distorted", "Overdrive",  198.f, 0.14f, 3400.f, 0.16f, 3600.f,  5.5f, 2600.f, 0.80f, 0.80f, 0.88f, 0.60f, 0.26f, 1.8f, 0, 4, 0, 0.18f, 0.28f, 0.16f, 0.08f, 0.40f, 0.28f },
    // Hip-Hop
    { "Hip-Hop", "Boom Bap",   208.f, 0.14f, 3000.f, 0.18f, 2800.f,  6.5f, 2200.f, 0.80f, 0.85f, 0.80f, 0.73f, 0.24f, 2.0f, 0, 0, 0, 0.05f, 0.25f, 0.18f, 0.28f, 0.08f, 0.15f },
    { "Hip-Hop", "Lo-Fi",      132.f, 0.36f, 1100.f, 0.40f,  900.f, 16.0f,  800.f, 1.00f, 0.50f, 0.28f, 0.70f, 0.08f, 4.5f, 0, 1, 0, 0.08f, 0.08f, 0.12f, 0.60f, 0.00f, 0.00f },
    { "Hip-Hop", "Drill",      135.f, 0.12f, 2400.f, 0.10f, 2800.f,  3.5f, 2000.f, 0.82f, 0.55f, 0.92f, 0.62f, 0.38f, 1.4f, 0, 3, 0, 0.08f, 0.35f, 0.10f, 0.15f, 0.25f, 0.35f },
    { "Hip-Hop", "West Coast", 112.f, 0.55f, 1300.f, 0.62f, 1200.f, 18.0f, 1000.f, 1.00f, 0.62f, 0.28f, 0.70f, 0.06f, 5.0f, 0, 1, 0, 0.12f, 0.12f, 0.10f, 0.40f, 0.00f, 0.00f },
    { "Hip-Hop", "Knock",      205.f, 0.09f, 4800.f, 0.11f, 5000.f,  3.2f, 3600.f, 0.70f, 0.65f, 1.00f, 0.70f, 0.40f, 1.2f, 0, 2, 0, 0.00f, 0.38f, 0.14f, 0.05f, 0.30f, 0.55f },
    { "Hip-Hop", "Modern",     155.f, 0.20f, 2600.f, 0.18f, 2800.f,  6.0f, 2000.f, 0.90f, 0.58f, 0.72f, 0.70f, 0.22f, 2.2f, 0, 2, 0, 0.06f, 0.28f, 0.08f, 0.10f, 0.15f, 0.20f },
    // Punch
    { "Punch", "Knockout",  188.f, 0.10f, 5200.f, 0.12f, 5400.f,  3.2f, 4000.f, 0.72f, 0.70f, 1.00f, 0.66f, 0.45f, 1.2f, 0, 4, 0, 0.08f, 0.38f, 0.14f, 0.00f, 0.40f, 0.50f },
    { "Punch", "Smack",     238.f, 0.08f, 6800.f, 0.10f, 7400.f,  2.2f, 5600.f, 0.48f, 0.58f, 1.00f, 0.70f, 0.46f, 1.0f, 0, 2, 0, 0.00f, 0.42f, 0.12f, 0.00f, 0.35f, 0.65f },
    { "Punch", "Finger",    300.f, 0.05f, 8200.f, 0.06f, 8800.f,  1.2f, 7200.f, 0.22f, 0.32f, 1.00f, 0.74f, 0.48f, 0.7f, 0, 2, 1, 0.00f, 0.30f, 0.08f, 0.00f, 0.20f, 0.85f },
    { "Punch", "Transient", 210.f, 0.06f, 7600.f, 0.08f, 8000.f,  1.6f, 6400.f, 0.52f, 0.58f, 1.00f, 0.70f, 0.52f, 0.8f, 0, 2, 0, 0.00f, 0.45f, 0.10f, 0.00f, 0.45f, 0.75f },
    { "Punch", "Dry Hit",   192.f, 0.12f, 3800.f, 0.12f, 4000.f,  5.0f, 2600.f, 0.82f, 0.70f, 0.82f, 0.76f, 0.20f, 1.8f, 0, 3, 0, 0.00f, 0.22f, 0.10f, 0.00f, 0.15f, 0.20f },
    { "Punch", "Gated",     215.f, 0.10f, 5200.f, 0.12f, 5400.f,  4.0f, 3600.f, 0.68f, 0.72f, 0.95f, 0.72f, 0.34f, 1.4f, 3, 2, 0, 0.05f, 0.32f, 0.16f, 0.00f, 0.30f, 0.40f },
    // Weird
    { "Weird", "Laser",  285.f, 0.28f, 7200.f, 0.12f, 8500.f,  6.0f, 6000.f, 0.42f, 0.28f, 0.88f, 0.70f, 0.12f, 2.5f, 0, 2, 1, 0.55f, 0.85f, 0.08f, 0.00f, 0.25f, 0.40f },
    { "Weird", "Hollow",  88.f, 0.62f,  500.f, 0.48f,  700.f, 24.0f,  500.f, 0.65f, 0.22f, 0.22f, 0.74f, 0.06f, 6.0f, 0, 1, 0, 0.35f, 0.08f, 0.05f, 0.20f, 0.00f, 0.00f },
    { "Weird", "Rubber", 128.f, 0.70f, 1200.f, 0.85f, 1100.f, 28.0f,  900.f, 0.88f, 0.42f, 0.28f, 0.72f, 0.08f, 6.5f, 0, 1, 0, 0.40f, 0.55f, 0.15f, 0.25f, 0.00f, 0.00f },
    { "Weird", "Alien",  255.f, 0.16f, 4800.f, 0.28f, 6200.f,  5.0f, 4400.f, 0.40f, 0.90f, 0.88f, 0.58f, 0.22f, 1.8f, 0, 3, 1, 0.50f, 0.48f, 0.45f, 0.00f, 0.20f, 0.35f },
    { "Weird", "Water",  122.f, 0.48f, 1400.f, 0.80f, 1600.f, 20.0f, 1100.f, 0.78f, 0.85f, 0.22f, 0.74f, 0.08f, 5.0f, 0, 1, 0, 0.15f, 0.18f, 0.40f, 0.40f, 0.00f, 0.00f },
    { "Weird", "Zap",    395.f, 0.04f,11000.f, 0.05f,12000.f,  0.6f, 9500.f, 0.18f, 0.35f, 1.00f, 0.62f, 0.55f, 0.5f, 0, 2, 1, 0.25f, 0.90f, 0.05f, 0.00f, 0.50f, 0.95f },
    // Tight
    { "Tight", "Sixteenth", 202.f, 0.08f, 4800.f, 0.10f, 5000.f,  3.5f, 3600.f, 0.72f, 0.72f, 0.95f, 0.74f, 0.32f, 1.4f, 3, 3, 0, 0.00f, 0.28f, 0.16f, 0.00f, 0.20f, 0.35f },
    { "Tight", "Eighth",    172.f, 0.14f, 3000.f, 0.16f, 3200.f,  6.0f, 2200.f, 0.82f, 0.78f, 0.72f, 0.74f, 0.20f, 2.0f, 2, 3, 0, 0.00f, 0.22f, 0.18f, 0.05f, 0.10f, 0.15f },
    { "Tight", "Clicky",    268.f, 0.06f, 7400.f, 0.08f, 7800.f,  1.8f, 6200.f, 0.40f, 0.52f, 1.00f, 0.72f, 0.42f, 1.0f, 3, 3, 1, 0.00f, 0.32f, 0.12f, 0.00f, 0.30f, 0.70f },
    { "Tight", "Minimal",   182.f, 0.12f, 2000.f, 0.08f, 1800.f,  7.0f, 1500.f, 0.80f, 0.32f, 0.48f, 0.76f, 0.12f, 2.2f, 0, 3, 0, 0.00f, 0.15f, 0.08f, 0.10f, 0.05f, 0.08f },
    { "Tight", "Perc",      310.f, 0.05f, 8200.f, 0.06f, 8800.f,  1.4f, 7000.f, 0.25f, 0.42f, 1.00f, 0.74f, 0.40f, 0.8f, 3, 3, 1, 0.00f, 0.25f, 0.10f, 0.00f, 0.25f, 0.75f },
    { "Tight", "Grid",      190.f, 0.12f, 3800.f, 0.14f, 4000.f,  4.5f, 2800.f, 0.76f, 0.80f, 0.85f, 0.73f, 0.26f, 1.6f, 2, 3, 0, 0.00f, 0.24f, 0.16f, 0.00f, 0.18f, 0.25f },
    // Hard
    { "Hard", "Stomp",      148.f, 0.24f, 2200.f, 0.22f, 2000.f,  8.5f, 1600.f, 0.95f, 0.65f, 0.80f, 0.62f, 0.22f, 2.4f, 0, 4, 0, 0.18f, 0.22f, 0.16f, 0.15f, 0.45f, 0.20f },
    { "Hard", "Rock",       215.f, 0.12f, 4600.f, 0.16f, 5000.f,  5.0f, 3400.f, 0.72f, 0.82f, 0.95f, 0.66f, 0.32f, 1.6f, 0, 4, 0, 0.10f, 0.32f, 0.22f, 0.00f, 0.50f, 0.40f },
    { "Hard", "Metal",      248.f, 0.08f, 7000.f, 0.12f, 7800.f,  2.5f, 5800.f, 0.48f, 0.78f, 1.00f, 0.58f, 0.40f, 1.2f, 0, 4, 1, 0.22f, 0.28f, 0.35f, 0.00f, 0.65f, 0.55f },
    { "Hard", "Compressed", 182.f, 0.14f, 2200.f, 0.16f, 2300.f,  7.5f, 1800.f, 0.92f, 0.78f, 0.68f, 0.55f, 0.16f, 2.4f, 0, 4, 0, 0.12f, 0.18f, 0.14f, 0.12f, 0.35f, 0.15f },
    { "Hard", "Aggressive", 205.f, 0.08f, 5600.f, 0.10f, 6000.f,  3.5f, 4200.f, 0.68f, 0.72f, 1.00f, 0.52f, 0.40f, 1.3f, 0, 4, 0, 0.20f, 0.35f, 0.16f, 0.00f, 0.70f, 0.50f },
    { "Hard", "Live",       175.f, 0.28f, 3400.f, 0.38f, 3200.f, 11.0f, 2200.f, 0.90f, 0.95f, 0.72f, 0.70f, 0.14f, 3.2f, 0, 4, 0, 0.08f, 0.25f, 0.30f, 0.08f, 0.35f, 0.18f },
};

inline constexpr FactoryFx factoryFx[] = {
    { "Festival",   0.90f, 0.40f, 0.35f, 0, -1, 0.35f, -1.f,  0.40f },
    { "Big Room",   0.82f, 0.55f, 0.40f, 0, -1, 0.35f, 0.28f, 0.45f },
    { "House",      1.00f, 0.18f, -1.f,  0, -1, 0.35f, 0.22f, 0.50f },
    { "Trance",     0.95f, 0.32f, -1.f,  0, -1, 0.35f, 0.48f, 0.40f },
    { "Warehouse",  1.00f, 0.22f, -1.f,  0, -1, 0.35f, 0.18f, 0.55f },
    { "Dub",        1.00f, 0.35f, -1.f,  0,  2, 0.40f, -1.f,  0.40f },
    { "Industrial", 0.85f, 0.55f, 0.70f, 0, -1, 0.35f, -1.f,  0.40f },
    { "Gabber",     0.70f, 0.80f, 0.85f, 0, -1, 0.35f, -1.f,  0.40f },
    { "Frenchcore", 0.55f, 0.90f, 0.95f, 0, -1, 0.35f, -1.f,  0.40f },
    { "Terror",     0.60f, 0.85f, 0.90f, 0, -1, 0.35f, -1.f,  0.40f },
    { "Rawstyle",   0.65f, 0.75f, 0.80f, 0, -1, 0.35f, -1.f,  0.40f },
    { "Fuzz",       0.50f, 1.00f, 0.75f, 3, -1, 0.35f, -1.f,  0.40f },
    { "Clip",       0.40f, 1.00f, 0.95f, 1, -1, 0.35f, -1.f,  0.40f },
    { "Saturation", 0.80f, 0.45f, 0.45f, 0, -1, 0.35f, -1.f,  0.40f },
    { "Chainsaw",   0.45f, 0.90f, 0.85f, 2, -1, 0.35f, -1.f,  0.40f },
    { "Cone",       0.55f, 0.80f, 0.70f, 0, -1, 0.35f, -1.f,  0.40f },
    { "Overdrive",  0.75f, 0.50f, 0.55f, 1, -1, 0.35f, -1.f,  0.40f },
    { "Lo-Fi",      0.88f, 0.28f, 0.32f, 0, -1, 0.35f, -1.f,  0.40f },
    { "Laser",      1.00f, 0.40f, -1.f,  0,  0, 0.50f, -1.f,  0.40f },
    { "Hollow",     0.90f, 0.45f, -1.f,  0, -1, 0.35f, 0.70f, 0.60f },
    { "Rubber",     0.88f, 0.38f, -1.f,  0,  1, 0.42f, -1.f,  0.40f },
    { "Alien",      0.80f, 0.55f, -1.f,  0,  1, 0.35f, 0.55f, 0.35f },
    { "Water",      0.85f, 0.50f, -1.f,  0, -1, 0.35f, 0.65f, 0.50f },
    { "Zap",        1.00f, 0.35f, -1.f,  0,  0, 0.25f, -1.f,  0.40f },
    { "Live",       1.00f, 0.22f, -1.f,  0, -1, 0.35f, 0.35f, 0.55f },
};

inline constexpr FactoryEq factoryEq[] = {
    { "Deep",       2,  90.f,  4.f, 0.70f, 1, 320.f, -3.f, 1.2f },
    { "Boom",       2, 110.f,  5.f, 0.70f, 1, 400.f, -3.f, 1.2f },
    { "Hardstyle",  1, 400.f, -5.f, 1.40f, 3, 5000.f, 3.f, 0.7f },
    { "Hollow",     1, 250.f, -8.f, 0.60f, 5, 1200.f, 0.f, 0.7f },
    { "Compressed", 1, 200.f,  3.f, 0.70f, 1, 4000.f, -2.f, 0.8f },
};

inline constexpr int factoryPresetCount = (int) (sizeof factoryPresets / sizeof factoryPresets[0]);
inline constexpr int factoryFxCount = (int) (sizeof factoryFx / sizeof factoryFx[0]);
inline constexpr int factoryEqCount = (int) (sizeof factoryEq / sizeof factoryEq[0]);

constexpr bool factorySame (const char* a, const char* b)
{
    if (a == nullptr || b == nullptr)
        return false;
    while (*a != '\0' && *a == *b)
    {
        ++a;
        ++b;
    }
    return *a == *b;
}

constexpr const FactoryPreset* findFactoryPreset (const char* name)
{
    for (int i = 0; i < factoryPresetCount; ++i)
        if (factorySame (factoryPresets[i].name, name))
            return &factoryPresets[i];
    return nullptr;
}

constexpr const FactoryFx* findFactoryFx (const char* name)
{
    for (int i = 0; i < factoryFxCount; ++i)
        if (factorySame (factoryFx[i].name, name))
            return &factoryFx[i];
    return nullptr;
}

constexpr const FactoryEq* findFactoryEq (const char* name)
{
    for (int i = 0; i < factoryEqCount; ++i)
        if (factorySame (factoryEq[i].name, name))
            return &factoryEq[i];
    return nullptr;
}

constexpr bool factoryInRange (float v, float lo, float hi)
{
    return v >= lo && v <= hi;
}

// Names unique, categories one run each, knobs inside the layout.
constexpr bool factoryBankOk()
{
    if (factoryPresetCount != 72)
        return false;
    for (int i = 0; i < factoryPresetCount; ++i)
    {
        const FactoryPreset& p = factoryPresets[i];
        if (p.category == nullptr || p.category[0] == '\0' || p.name == nullptr || p.name[0] == '\0')
            return false;
        for (int j = 0; j < i; ++j)
            if (factorySame (p.name, factoryPresets[j].name))
                return false;
        if (i > 0 && ! factorySame (p.category, factoryPresets[i - 1].category))
            for (int j = 0; j < i; ++j)
                if (factorySame (p.category, factoryPresets[j].category))
                    return false;
        if (! factoryInRange (p.tune, 80.f, 400.f)
            || ! factoryInRange (p.shellDec, 0.04f, 0.8f)
            || ! factoryInRange (p.shellTone, 400.f, 12000.f)
            || ! factoryInRange (p.wiresDec, 0.04f, 1.2f)
            || ! factoryInRange (p.wiresTone, 400.f, 12000.f)
            || ! factoryInRange (p.stickDec, 0.5f, 40.f)
            || ! factoryInRange (p.stickTone, 400.f, 12000.f)
            || ! factoryInRange (p.shellGain, 0.f, 1.f)
            || ! factoryInRange (p.wiresGain, 0.f, 1.f)
            || ! factoryInRange (p.stickGain, 0.f, 1.f)
            || ! factoryInRange (p.level, 0.f, 1.f)
            || ! factoryInRange (p.duck, 0.f, 1.f)
            || ! factoryInRange (p.duckTime, 0.5f, 8.f)
            || p.length < 0 || p.length > 3
            || p.style < 0 || p.style > 4
            || p.shape < 0 || p.shape > 1
            || ! factoryInRange (p.harm, 0.f, 1.f)
            || ! factoryInRange (p.chirp, 0.f, 1.f)
            || ! factoryInRange (p.buzz, 0.f, 1.f)
            || ! factoryInRange (p.color, 0.f, 1.f)
            || ! factoryInRange (p.stickHard, 0.f, 1.f)
            || ! factoryInRange (p.stickClick, 0.f, 1.f))
            return false;
    }
    for (int i = 0; i < factoryFxCount; ++i)
    {
        const FactoryFx& f = factoryFx[i];
        if (findFactoryPreset (f.name) == nullptr)
            return false;
        for (int j = 0; j < i; ++j)
            if (factorySame (f.name, factoryFx[j].name))
                return false;
        if (! factoryInRange (f.dry, 0.f, 1.f) || ! factoryInRange (f.wet, 0.f, 1.f))
            return false;
        if (f.dst < 0.f)
        {
            if (f.dst != -1.f)
                return false;
        }
        else if (! factoryInRange (f.dst, 0.f, 1.f) || f.dstType < 0 || f.dstType > 3)
            return false;
        if (f.dlyDiv < 0)
        {
            if (f.dlyDiv != -1)
                return false;
        }
        else if (f.dlyDiv > 3 || ! factoryInRange (f.dlyFb, 0.f, 1.f))
            return false;
        if (f.rev < 0.f)
        {
            if (f.rev != -1.f)
                return false;
        }
        else if (! factoryInRange (f.rev, 0.f, 1.f) || ! factoryInRange (f.revDamp, 0.f, 1.f))
            return false;
    }
    for (int i = 0; i < factoryEqCount; ++i)
    {
        const FactoryEq& e = factoryEq[i];
        if (findFactoryPreset (e.name) == nullptr)
            return false;
        for (int j = 0; j < i; ++j)
            if (factorySame (e.name, factoryEq[j].name))
                return false;
        if (e.type < 1 || e.type > 5 || ! factoryInRange (e.hz, 20.f, 20000.f)
            || ! factoryInRange (e.db, -12.f, 12.f) || ! factoryInRange (e.q, 0.1f, 10.f))
            return false;
        if (e.type2 == 0)
            continue;
        if (e.type2 < 1 || e.type2 > 5 || ! factoryInRange (e.hz2, 20.f, 20000.f)
            || ! factoryInRange (e.db2, -12.f, 12.f) || ! factoryInRange (e.q2, 0.1f, 10.f))
            return false;
    }
    return true;
}

} // namespace snr3

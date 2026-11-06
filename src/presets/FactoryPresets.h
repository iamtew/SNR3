#pragma once

// Factory snares. initPatch() runs first, then these voice values, then a
// matching FactoryFx / FactoryEq row if the name is listed there.
// Categories stay in one contiguous run — the patch menu opens one submenu each.
// Length: 0=Free 1=1/4 2=1/8 3=1/16. Sample gain stays 0 (no factory file).

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
    int length;
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
    // 909
    { "909", "Punch",     185.f, 0.18f, 3500.f, 0.22f, 3500.f,  8.0f, 2500.f, 1.00f, 1.00f, 1.00f, 0.75f, 0.25f, 2.0f, 0 },
    { "909", "Short",     200.f, 0.08f, 4200.f, 0.10f, 4800.f,  5.0f, 3200.f, 0.90f, 0.85f, 1.00f, 0.76f, 0.30f, 1.5f, 0 },
    { "909", "Long",      165.f, 0.42f, 2800.f, 0.55f, 2600.f, 10.0f, 2000.f, 1.00f, 1.00f, 0.80f, 0.72f, 0.18f, 3.0f, 0 },
    { "909", "Click",     220.f, 0.12f, 5000.f, 0.14f, 6200.f,  3.5f, 4500.f, 0.75f, 0.70f, 1.00f, 0.74f, 0.40f, 1.2f, 0 },
    { "909", "Soft",      170.f, 0.28f, 2400.f, 0.32f, 2200.f, 12.0f, 1600.f, 1.00f, 0.85f, 0.55f, 0.76f, 0.12f, 3.5f, 0 },
    { "909", "Rolling",   180.f, 0.24f, 3000.f, 0.38f, 2800.f,  9.0f, 2200.f, 0.95f, 1.00f, 0.70f, 0.75f, 0.20f, 2.5f, 0 },
    // 808
    { "808", "Fat",       120.f, 0.35f, 1800.f, 0.18f, 2200.f, 10.0f, 1400.f, 1.00f, 0.55f, 0.70f, 0.72f, 0.15f, 3.0f, 0 },
    { "808", "Rim",       250.f, 0.10f, 4500.f, 0.08f, 5000.f,  4.0f, 3800.f, 0.55f, 0.40f, 1.00f, 0.74f, 0.35f, 1.5f, 0 },
    { "808", "Long Tail", 110.f, 0.55f, 1400.f, 0.70f, 1600.f, 12.0f, 1200.f, 1.00f, 0.70f, 0.50f, 0.68f, 0.10f, 4.0f, 0 },
    { "808", "Bounce",    155.f, 0.22f, 2600.f, 0.20f, 3000.f,  7.0f, 2200.f, 0.90f, 0.65f, 0.85f, 0.73f, 0.22f, 2.0f, 0 },
    { "808", "Boom",      100.f, 0.48f, 1200.f, 0.25f, 1800.f, 14.0f, 1000.f, 1.00f, 0.45f, 0.60f, 0.70f, 0.12f, 3.5f, 0 },
    { "808", "Deep",       90.f, 0.60f,  900.f, 0.30f, 1400.f, 16.0f,  800.f, 1.00f, 0.35f, 0.45f, 0.70f, 0.08f, 4.5f, 0 },
    // 707
    { "707", "Dry",       230.f, 0.12f, 5500.f, 0.14f, 6000.f,  4.5f, 4000.f, 0.70f, 0.75f, 1.00f, 0.75f, 0.28f, 1.8f, 0 },
    { "707", "Snap",      260.f, 0.08f, 7000.f, 0.10f, 7500.f,  2.5f, 5500.f, 0.55f, 0.60f, 1.00f, 0.74f, 0.40f, 1.2f, 0 },
    { "707", "Electro",   210.f, 0.16f, 4800.f, 0.18f, 5200.f,  6.0f, 3500.f, 0.80f, 0.85f, 0.95f, 0.72f, 0.25f, 2.0f, 0 },
    { "707", "Thin",      300.f, 0.06f, 8000.f, 0.08f, 8500.f,  2.0f, 6500.f, 0.40f, 0.55f, 1.00f, 0.74f, 0.35f, 1.0f, 0 },
    { "707", "Metallic",  320.f, 0.14f, 7500.f, 0.16f, 9000.f,  3.0f, 7000.f, 0.50f, 0.70f, 1.00f, 0.68f, 0.32f, 1.5f, 0 },
    { "707", "Tick",      340.f, 0.05f, 9000.f, 0.06f, 9500.f,  1.2f, 8000.f, 0.30f, 0.45f, 1.00f, 0.76f, 0.45f, 0.8f, 0 },
    // EDM
    { "EDM", "Festival",  190.f, 0.16f, 4000.f, 0.20f, 4500.f,  6.0f, 3000.f, 0.90f, 0.90f, 1.00f, 0.68f, 0.30f, 1.8f, 0 },
    { "EDM", "Big Room",  175.f, 0.14f, 3800.f, 0.16f, 4200.f,  5.0f, 2800.f, 0.85f, 0.80f, 1.00f, 0.64f, 0.35f, 1.5f, 0 },
    { "EDM", "Future",    165.f, 0.22f, 3200.f, 0.28f, 3600.f,  8.0f, 2400.f, 0.95f, 0.75f, 0.85f, 0.70f, 0.22f, 2.5f, 0 },
    { "EDM", "House",     195.f, 0.18f, 3600.f, 0.22f, 3800.f,  7.5f, 2600.f, 0.90f, 0.95f, 0.90f, 0.74f, 0.24f, 2.0f, 0 },
    { "EDM", "Trance",    205.f, 0.26f, 3400.f, 0.32f, 3200.f,  9.0f, 2200.f, 0.92f, 0.88f, 0.75f, 0.74f, 0.18f, 2.8f, 0 },
    { "EDM", "Progressive", 178.f, 0.24f, 2800.f, 0.30f, 2600.f, 10.0f, 2000.f, 0.95f, 0.80f, 0.70f, 0.75f, 0.16f, 3.0f, 0 },
    // Techno
    { "Techno", "Warehouse", 188.f, 0.15f, 3600.f, 0.18f, 3800.f,  6.5f, 2800.f, 0.85f, 0.90f, 0.95f, 0.72f, 0.28f, 2.0f, 0 },
    { "Techno", "Dub",       160.f, 0.32f, 2000.f, 0.45f, 1800.f, 11.0f, 1500.f, 1.00f, 0.80f, 0.55f, 0.74f, 0.12f, 3.5f, 0 },
    { "Techno", "Hypnotic",  172.f, 0.28f, 2200.f, 0.36f, 2000.f, 10.0f, 1600.f, 0.90f, 0.85f, 0.50f, 0.76f, 0.10f, 4.0f, 0 },
    { "Techno", "Peak Time", 198.f, 0.12f, 4800.f, 0.14f, 5200.f,  4.5f, 3600.f, 0.80f, 0.85f, 1.00f, 0.68f, 0.35f, 1.5f, 0 },
    { "Techno", "Industrial", 150.f, 0.20f, 2800.f, 0.22f, 2400.f,  7.0f, 2000.f, 0.85f, 0.70f, 0.90f, 0.62f, 0.25f, 2.2f, 0 },
    { "Techno", "Berlin",    182.f, 0.18f, 3200.f, 0.22f, 3000.f,  8.0f, 2400.f, 0.88f, 0.92f, 0.80f, 0.73f, 0.22f, 2.2f, 0 },
    // Hardcore
    { "Hardcore", "Gabber",     210.f, 0.08f, 5500.f, 0.10f, 6000.f,  3.0f, 4500.f, 0.70f, 0.75f, 1.00f, 0.55f, 0.40f, 1.2f, 0 },
    { "Hardcore", "Frenchcore", 230.f, 0.06f, 6500.f, 0.08f, 7000.f,  2.2f, 5500.f, 0.60f, 0.70f, 1.00f, 0.52f, 0.45f, 1.0f, 0 },
    { "Hardcore", "Terror",     170.f, 0.10f, 4000.f, 0.12f, 4500.f,  4.0f, 3200.f, 0.80f, 0.65f, 1.00f, 0.50f, 0.38f, 1.5f, 0 },
    { "Hardcore", "Uptempo",    250.f, 0.05f, 7000.f, 0.06f, 7500.f,  1.8f, 6000.f, 0.50f, 0.60f, 1.00f, 0.58f, 0.42f, 0.9f, 0 },
    { "Hardcore", "Hardstyle",  185.f, 0.20f, 3800.f, 0.22f, 4000.f,  7.0f, 2800.f, 0.90f, 0.80f, 0.90f, 0.62f, 0.28f, 2.0f, 0 },
    { "Hardcore", "Rawstyle",   165.f, 0.14f, 3200.f, 0.16f, 3400.f,  5.5f, 2600.f, 0.85f, 0.70f, 0.95f, 0.55f, 0.32f, 1.8f, 0 },
    // Distorted
    { "Distorted", "Fuzz",       155.f, 0.22f, 2200.f, 0.24f, 2000.f,  8.0f, 1800.f, 0.90f, 0.70f, 0.80f, 0.55f, 0.20f, 2.5f, 0 },
    { "Distorted", "Clip",       195.f, 0.10f, 5000.f, 0.12f, 5500.f,  4.0f, 4000.f, 0.70f, 0.75f, 1.00f, 0.50f, 0.35f, 1.5f, 0 },
    { "Distorted", "Saturation", 175.f, 0.24f, 2800.f, 0.28f, 2600.f,  9.0f, 2000.f, 0.95f, 0.80f, 0.70f, 0.64f, 0.18f, 2.8f, 0 },
    { "Distorted", "Chainsaw",   220.f, 0.08f, 6000.f, 0.10f, 6500.f,  3.0f, 5000.f, 0.55f, 0.80f, 1.00f, 0.50f, 0.38f, 1.2f, 0 },
    { "Distorted", "Cone",       130.f, 0.35f,  900.f, 0.20f, 1200.f, 14.0f,  800.f, 1.00f, 0.40f, 0.55f, 0.58f, 0.12f, 4.0f, 0 },
    { "Distorted", "Overdrive",  200.f, 0.16f, 3600.f, 0.18f, 3800.f,  6.5f, 2800.f, 0.85f, 0.85f, 0.90f, 0.62f, 0.25f, 2.0f, 0 },
    // Hip-Hop
    { "Hip-Hop", "Boom Bap",   200.f, 0.16f, 3400.f, 0.20f, 3600.f,  7.0f, 2600.f, 0.85f, 0.90f, 0.95f, 0.73f, 0.28f, 2.0f, 0 },
    { "Hip-Hop", "Lo-Fi",      145.f, 0.30f, 1400.f, 0.35f, 1200.f, 12.0f, 1100.f, 1.00f, 0.60f, 0.50f, 0.72f, 0.10f, 4.0f, 0 },
    { "Hip-Hop", "Drill",      140.f, 0.18f, 2600.f, 0.16f, 3000.f,  5.5f, 2200.f, 0.90f, 0.70f, 0.90f, 0.64f, 0.30f, 1.8f, 0 },
    { "Hip-Hop", "West Coast", 125.f, 0.45f, 1600.f, 0.50f, 1400.f, 14.0f, 1200.f, 1.00f, 0.55f, 0.45f, 0.70f, 0.08f, 4.5f, 0 },
    { "Hip-Hop", "Knock",      198.f, 0.12f, 4200.f, 0.14f, 4500.f,  4.5f, 3200.f, 0.80f, 0.75f, 1.00f, 0.70f, 0.35f, 1.5f, 0 },
    { "Hip-Hop", "Modern",     150.f, 0.22f, 2200.f, 0.24f, 2400.f,  8.0f, 1800.f, 0.95f, 0.70f, 0.75f, 0.70f, 0.20f, 2.5f, 0 },
    // Punch
    { "Punch", "Knockout",  185.f, 0.12f, 4800.f, 0.14f, 5000.f,  4.0f, 3600.f, 0.80f, 0.80f, 1.00f, 0.68f, 0.38f, 1.4f, 0 },
    { "Punch", "Smack",     230.f, 0.10f, 6200.f, 0.12f, 6800.f,  3.0f, 5000.f, 0.60f, 0.70f, 1.00f, 0.70f, 0.40f, 1.2f, 0 },
    { "Punch", "Finger",    280.f, 0.06f, 7500.f, 0.08f, 8000.f,  1.8f, 6500.f, 0.40f, 0.50f, 1.00f, 0.74f, 0.42f, 0.9f, 0 },
    { "Punch", "Transient", 205.f, 0.08f, 7000.f, 0.10f, 7500.f,  2.2f, 5800.f, 0.65f, 0.70f, 1.00f, 0.70f, 0.45f, 1.0f, 0 },
    { "Punch", "Dry Hit",   190.f, 0.14f, 4000.f, 0.16f, 4200.f,  6.0f, 2800.f, 0.85f, 0.80f, 0.85f, 0.76f, 0.22f, 2.0f, 0 },
    { "Punch", "Gated",     210.f, 0.12f, 5000.f, 0.14f, 5200.f,  5.0f, 3400.f, 0.75f, 0.80f, 0.95f, 0.72f, 0.30f, 1.6f, 3 },
    // Weird
    { "Weird", "Laser",  270.f, 0.30f, 6000.f, 0.18f, 7000.f,  8.0f, 5000.f, 0.55f, 0.40f, 0.90f, 0.70f, 0.15f, 3.0f, 0 },
    { "Weird", "Hollow",  95.f, 0.50f,  700.f, 0.40f,  900.f, 18.0f,  600.f, 0.70f, 0.30f, 0.35f, 0.74f, 0.08f, 5.0f, 0 },
    { "Weird", "Rubber", 140.f, 0.55f, 1500.f, 0.60f, 1400.f, 20.0f, 1200.f, 0.90f, 0.50f, 0.40f, 0.72f, 0.10f, 5.5f, 0 },
    { "Weird", "Alien",  260.f, 0.18f, 4500.f, 0.22f, 5500.f,  6.0f, 4000.f, 0.50f, 0.85f, 0.95f, 0.60f, 0.28f, 2.0f, 0 },
    { "Weird", "Water",  135.f, 0.40f, 1600.f, 0.55f, 1800.f, 15.0f, 1400.f, 0.85f, 0.75f, 0.40f, 0.74f, 0.10f, 4.5f, 0 },
    { "Weird", "Zap",    360.f, 0.06f, 9000.f, 0.08f,10000.f,  0.8f, 8000.f, 0.35f, 0.50f, 1.00f, 0.62f, 0.48f, 0.6f, 0 },
    // Tight
    { "Tight", "Sixteenth", 200.f, 0.10f, 4500.f, 0.12f, 4800.f,  4.5f, 3400.f, 0.80f, 0.80f, 0.95f, 0.74f, 0.30f, 1.6f, 3 },
    { "Tight", "Eighth",    175.f, 0.16f, 3200.f, 0.18f, 3400.f,  7.0f, 2400.f, 0.88f, 0.85f, 0.80f, 0.74f, 0.22f, 2.2f, 2 },
    { "Tight", "Clicky",    255.f, 0.08f, 6800.f, 0.10f, 7200.f,  2.5f, 5600.f, 0.55f, 0.65f, 1.00f, 0.72f, 0.38f, 1.2f, 3 },
    { "Tight", "Minimal",   185.f, 0.14f, 2400.f, 0.12f, 2200.f,  8.0f, 1800.f, 0.85f, 0.50f, 0.60f, 0.76f, 0.15f, 2.5f, 0 },
    { "Tight", "Perc",      290.f, 0.06f, 7500.f, 0.08f, 8000.f,  2.0f, 6200.f, 0.40f, 0.55f, 1.00f, 0.74f, 0.35f, 1.0f, 3 },
    { "Tight", "Grid",      192.f, 0.14f, 4000.f, 0.16f, 4200.f,  5.5f, 3000.f, 0.82f, 0.85f, 0.90f, 0.73f, 0.26f, 1.8f, 2 },
    // Hard
    { "Hard", "Stomp",      160.f, 0.22f, 2800.f, 0.24f, 2600.f,  8.0f, 2000.f, 0.95f, 0.75f, 0.85f, 0.64f, 0.22f, 2.5f, 0 },
    { "Hard", "Rock",       210.f, 0.14f, 4200.f, 0.16f, 4500.f,  6.0f, 3200.f, 0.80f, 0.85f, 0.95f, 0.68f, 0.30f, 1.8f, 0 },
    { "Hard", "Metal",      235.f, 0.10f, 6000.f, 0.12f, 6500.f,  3.5f, 5000.f, 0.60f, 0.80f, 1.00f, 0.60f, 0.38f, 1.4f, 0 },
    { "Hard", "Compressed", 185.f, 0.12f, 2500.f, 0.14f, 2600.f,  7.0f, 2000.f, 0.90f, 0.80f, 0.75f, 0.58f, 0.20f, 2.2f, 0 },
    { "Hard", "Aggressive", 200.f, 0.10f, 5000.f, 0.12f, 5400.f,  4.5f, 3800.f, 0.75f, 0.80f, 1.00f, 0.55f, 0.36f, 1.5f, 0 },
    { "Hard", "Live",       178.f, 0.26f, 3600.f, 0.32f, 3400.f, 10.0f, 2400.f, 0.92f, 0.95f, 0.80f, 0.70f, 0.18f, 3.0f, 0 },
};

inline constexpr FactoryFx factoryFx[] = {
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
    { "Laser",      1.00f, 0.40f, -1.f,  0,  0, 0.50f, -1.f,  0.40f },
    { "Hollow",     0.90f, 0.45f, -1.f,  0, -1, 0.35f, 0.70f, 0.60f },
    { "Alien",      0.80f, 0.55f, -1.f,  0,  1, 0.35f, 0.55f, 0.35f },
    { "Water",      0.85f, 0.50f, -1.f,  0, -1, 0.35f, 0.65f, 0.50f },
    { "Zap",        1.00f, 0.35f, -1.f,  0,  0, 0.25f, -1.f,  0.40f },
    { "Live",       1.00f, 0.22f, -1.f,  0, -1, 0.35f, 0.35f, 0.55f },
};

inline constexpr FactoryEq factoryEq[] = {
    { "Deep",      2,  90.f,  4.f, 0.70f, 1, 320.f, -3.f, 1.2f },
    { "Boom",      2, 110.f,  5.f, 0.70f, 1, 400.f, -3.f, 1.2f },
    { "Hardstyle", 1, 400.f, -5.f, 1.40f, 3, 5000.f, 3.f, 0.7f },
    { "Hollow",    1, 250.f, -8.f, 0.60f, 5, 1200.f, 0.f, 0.7f },
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
            || p.length < 0 || p.length > 3)
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

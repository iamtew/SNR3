#pragma once

// Factory snares. initPatch() runs first, then these voice values, then a
// matching FactoryFx / FactoryEq row if the name is listed there.
// Categories stay in one contiguous run — the patch menu opens one submenu each.
// Length: 0=Free 1=1/4 2=1/8 3=1/16. Style: 0 Classic 1 Fat 2 Snap 3 Tight 4 Hard.
// Shape: 0 Sine 1 Tri. Sample gain stays 0 (no factory file). Names unique.

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
    // 909
    { "909", "Punch",  185.f, 0.18f, 3500.f, 0.22f, 3500.f,  8.0f, 2500.f, 1.00f, 1.00f, 1.00f, 0.75f, 0.22f, 2.0f, 0, 0, 0, 0.00f, 0.35f, 0.25f, 0.00f, 0.00f, 0.10f },
    { "909", "Crisp",  210.f, 0.10f, 4800.f, 0.14f, 5600.f,  4.0f, 3800.f, 0.78f, 0.92f, 1.00f, 0.74f, 0.32f, 1.3f, 0, 2, 0, 0.00f, 0.30f, 0.22f, 0.00f, 0.15f, 0.40f },
    { "909", "Open",   168.f, 0.32f, 2600.f, 0.48f, 2400.f, 10.0f, 1800.f, 0.95f, 1.00f, 0.70f, 0.73f, 0.14f, 3.0f, 0, 1, 0, 0.06f, 0.22f, 0.38f, 0.12f, 0.00f, 0.05f },
    { "909", "Rim",    230.f, 0.08f, 5400.f, 0.10f, 6200.f,  2.8f, 4800.f, 0.45f, 0.40f, 1.00f, 0.74f, 0.40f, 1.0f, 0, 2, 0, 0.00f, 0.38f, 0.12f, 0.00f, 0.30f, 0.65f },
    { "909", "Soft",   172.f, 0.30f, 2000.f, 0.34f, 1800.f, 14.0f, 1300.f, 1.00f, 0.78f, 0.38f, 0.76f, 0.08f, 3.8f, 0, 1, 0, 0.00f, 0.14f, 0.22f, 0.22f, 0.00f, 0.00f },
    { "909", "Roll",   178.f, 0.22f, 2800.f, 0.58f, 3000.f,  8.5f, 2100.f, 0.90f, 1.00f, 0.55f, 0.75f, 0.12f, 2.6f, 0, 0, 0, 0.04f, 0.28f, 0.50f, 0.10f, 0.00f, 0.08f },
    // 808 — noise clap / rim, not a sub kick
    { "808", "Fat",     165.f, 0.28f, 1800.f, 0.42f, 2200.f, 12.0f, 1400.f, 0.70f, 1.00f, 0.48f, 0.72f, 0.12f, 3.2f, 0, 1, 0, 0.10f, 0.18f, 0.12f, 0.40f, 0.00f, 0.05f },
    { "808", "Rimshot", 275.f, 0.06f, 6400.f, 0.05f, 7200.f,  2.4f, 5000.f, 0.28f, 0.10f, 1.00f, 0.74f, 0.44f, 0.9f, 0, 2, 1, 0.00f, 0.42f, 0.05f, 0.00f, 0.35f, 0.78f },
    { "808", "Clap",    190.f, 0.16f, 2800.f, 0.36f, 3400.f,  8.0f, 2200.f, 0.35f, 1.00f, 0.55f, 0.72f, 0.18f, 2.4f, 0, 1, 0, 0.08f, 0.20f, 0.08f, 0.50f, 0.05f, 0.12f },
    { "808", "Long",    158.f, 0.38f, 1600.f, 0.72f, 1800.f, 14.0f, 1200.f, 0.55f, 1.00f, 0.32f, 0.70f, 0.10f, 4.0f, 0, 1, 0, 0.08f, 0.14f, 0.14f, 0.45f, 0.00f, 0.00f },
    { "808", "Tight",   200.f, 0.10f, 3200.f, 0.14f, 3800.f,  5.0f, 2800.f, 0.50f, 0.78f, 0.85f, 0.74f, 0.28f, 1.6f, 0, 3, 0, 0.04f, 0.28f, 0.10f, 0.25f, 0.12f, 0.22f },
    { "808", "Box",     175.f, 0.14f, 2400.f, 0.18f, 2600.f,  6.5f, 2000.f, 0.62f, 0.70f, 0.72f, 0.75f, 0.20f, 2.0f, 0, 3, 0, 0.00f, 0.22f, 0.08f, 0.20f, 0.08f, 0.15f },
    // 707
    { "707", "Dry",      248.f, 0.10f, 6200.f, 0.12f, 7000.f,  3.4f, 4500.f, 0.48f, 0.68f, 1.00f, 0.75f, 0.30f, 1.4f, 0, 3, 1, 0.04f, 0.22f, 0.26f, 0.00f, 0.18f, 0.38f },
    { "707", "Snap",     272.f, 0.06f, 7800.f, 0.08f, 8500.f,  1.8f, 6200.f, 0.38f, 0.55f, 1.00f, 0.74f, 0.42f, 0.9f, 0, 2, 1, 0.00f, 0.34f, 0.20f, 0.00f, 0.28f, 0.62f },
    { "707", "Thin",     318.f, 0.05f, 9000.f, 0.06f, 9800.f,  1.2f, 7500.f, 0.22f, 0.48f, 1.00f, 0.74f, 0.38f, 0.8f, 0, 2, 1, 0.00f, 0.20f, 0.16f, 0.00f, 0.22f, 0.72f },
    { "707", "Tick",     355.f, 0.04f,10000.f, 0.05f,11000.f,  0.7f, 8800.f, 0.15f, 0.30f, 1.00f, 0.76f, 0.48f, 0.6f, 0, 2, 1, 0.00f, 0.16f, 0.10f, 0.00f, 0.38f, 0.88f },
    { "707", "Metallic", 330.f, 0.12f, 8500.f, 0.18f,10800.f,  2.8f, 8000.f, 0.35f, 0.85f, 0.92f, 0.68f, 0.26f, 1.3f, 0, 3, 1, 0.20f, 0.18f, 0.52f, 0.00f, 0.18f, 0.42f },
    { "707", "Electro",  220.f, 0.14f, 5200.f, 0.16f, 5800.f,  5.0f, 3800.f, 0.58f, 0.78f, 0.90f, 0.72f, 0.24f, 1.8f, 0, 3, 1, 0.10f, 0.26f, 0.30f, 0.08f, 0.14f, 0.28f },
    // Acoustic kit snares
    { "Acoustic", "Piccolo",   290.f, 0.10f, 6200.f, 0.16f, 7000.f,  4.0f, 4800.f, 0.55f, 0.85f, 1.00f, 0.74f, 0.30f, 1.4f, 0, 2, 0, 0.00f, 0.28f, 0.28f, 0.00f, 0.20f, 0.40f },
    { "Acoustic", "Concert",   155.f, 0.36f, 2200.f, 0.52f, 2000.f, 12.0f, 1600.f, 1.00f, 0.90f, 0.55f, 0.74f, 0.10f, 3.8f, 0, 1, 0, 0.08f, 0.18f, 0.32f, 0.18f, 0.00f, 0.05f },
    { "Acoustic", "Maple",     188.f, 0.22f, 3200.f, 0.28f, 3400.f,  8.0f, 2400.f, 0.92f, 0.88f, 0.82f, 0.75f, 0.18f, 2.2f, 0, 0, 0, 0.04f, 0.28f, 0.24f, 0.08f, 0.08f, 0.18f },
    { "Acoustic", "Brass",     205.f, 0.18f, 4200.f, 0.32f, 4800.f,  6.5f, 3200.f, 0.72f, 0.95f, 0.90f, 0.72f, 0.22f, 1.8f, 0, 0, 1, 0.12f, 0.26f, 0.40f, 0.00f, 0.15f, 0.25f },
    { "Acoustic", "Vintage",   170.f, 0.26f, 2400.f, 0.34f, 2200.f, 10.0f, 1800.f, 0.95f, 0.75f, 0.58f, 0.74f, 0.12f, 2.8f, 0, 1, 0, 0.06f, 0.16f, 0.20f, 0.28f, 0.00f, 0.08f },
    { "Acoustic", "Fat Head",  162.f, 0.34f, 1800.f, 0.30f, 1600.f, 11.0f, 1400.f, 1.00f, 0.70f, 0.62f, 0.73f, 0.14f, 3.2f, 0, 1, 0, 0.10f, 0.20f, 0.18f, 0.22f, 0.05f, 0.08f },
    // Rim
    { "Rim", "Sidestick",  240.f, 0.06f, 5800.f, 0.05f, 5000.f,  3.0f, 4200.f, 0.22f, 0.08f, 1.00f, 0.74f, 0.38f, 1.0f, 0, 2, 1, 0.00f, 0.25f, 0.00f, 0.00f, 0.20f, 0.70f },
    { "Rim", "Edge",       255.f, 0.08f, 6400.f, 0.10f, 6800.f,  2.5f, 5000.f, 0.35f, 0.30f, 1.00f, 0.74f, 0.36f, 1.1f, 0, 2, 0, 0.00f, 0.32f, 0.10f, 0.00f, 0.28f, 0.60f },
    { "Rim", "Ping",       310.f, 0.07f, 8200.f, 0.12f, 9000.f,  2.0f, 7000.f, 0.28f, 0.45f, 1.00f, 0.70f, 0.32f, 0.9f, 0, 2, 1, 0.15f, 0.22f, 0.35f, 0.00f, 0.25f, 0.55f },
    { "Rim", "Rim Crack",  268.f, 0.08f, 7200.f, 0.08f, 7600.f,  2.2f, 5800.f, 0.40f, 0.22f, 1.00f, 0.70f, 0.42f, 1.0f, 0, 4, 0, 0.05f, 0.38f, 0.08f, 0.00f, 0.45f, 0.72f },
    { "Rim", "Bell",       340.f, 0.10f, 9000.f, 0.14f,10000.f,  2.6f, 7800.f, 0.30f, 0.55f, 0.95f, 0.68f, 0.28f, 1.2f, 0, 3, 1, 0.22f, 0.18f, 0.42f, 0.00f, 0.18f, 0.40f },
    { "Rim", "Center",     195.f, 0.16f, 3600.f, 0.12f, 3200.f,  6.0f, 2400.f, 0.90f, 0.45f, 0.88f, 0.75f, 0.22f, 1.8f, 0, 0, 0, 0.00f, 0.30f, 0.12f, 0.05f, 0.12f, 0.22f },
    // Wires
    { "Wires", "Loose",    175.f, 0.20f, 2400.f, 0.70f, 2200.f,  9.0f, 1800.f, 0.72f, 1.00f, 0.50f, 0.74f, 0.10f, 3.0f, 0, 1, 0, 0.04f, 0.18f, 0.55f, 0.20f, 0.00f, 0.05f },
    { "Wires", "Cranked",  198.f, 0.12f, 4000.f, 0.16f, 5200.f,  5.0f, 3600.f, 0.65f, 0.95f, 0.85f, 0.74f, 0.26f, 1.6f, 0, 2, 0, 0.00f, 0.28f, 0.42f, 0.00f, 0.15f, 0.30f },
    { "Wires", "Off",      185.f, 0.20f, 3000.f, 0.08f, 2800.f,  8.0f, 2200.f, 1.00f, 0.00f, 0.90f, 0.75f, 0.20f, 2.0f, 0, 0, 0, 0.00f, 0.28f, 0.00f, 0.00f, 0.10f, 0.20f },
    { "Wires", "Buzz",     182.f, 0.18f, 2800.f, 0.45f, 3600.f,  7.0f, 2400.f, 0.70f, 1.00f, 0.62f, 0.73f, 0.14f, 2.4f, 0, 0, 0, 0.06f, 0.22f, 0.72f, 0.08f, 0.00f, 0.10f },
    { "Wires", "Gut",      168.f, 0.24f, 1800.f, 0.38f, 1600.f, 10.0f, 1400.f, 0.85f, 0.82f, 0.48f, 0.74f, 0.10f, 3.2f, 0, 1, 0, 0.08f, 0.16f, 0.28f, 0.35f, 0.00f, 0.00f },
    { "Wires", "Whisper",  192.f, 0.14f, 3600.f, 0.22f, 4200.f,  5.5f, 2800.f, 0.48f, 0.55f, 0.40f, 0.76f, 0.12f, 2.0f, 0, 3, 0, 0.00f, 0.18f, 0.18f, 0.10f, 0.00f, 0.08f },
    // Gated
    { "Gated", "80s",        200.f, 0.12f, 4200.f, 0.16f, 4400.f,  5.0f, 3000.f, 0.80f, 0.85f, 0.90f, 0.70f, 0.28f, 1.6f, 3, 2, 0, 0.05f, 0.30f, 0.18f, 0.00f, 0.20f, 0.30f },
    { "Gated", "Room",       188.f, 0.16f, 3400.f, 0.22f, 3200.f,  7.0f, 2400.f, 0.88f, 0.90f, 0.78f, 0.72f, 0.20f, 2.2f, 2, 0, 0, 0.04f, 0.24f, 0.22f, 0.08f, 0.10f, 0.18f },
    { "Gated", "Plate",      205.f, 0.14f, 4000.f, 0.20f, 4200.f,  6.0f, 2800.f, 0.78f, 0.88f, 0.82f, 0.72f, 0.22f, 1.8f, 2, 2, 0, 0.06f, 0.26f, 0.20f, 0.00f, 0.15f, 0.22f },
    { "Gated", "Short Gate", 215.f, 0.08f, 5000.f, 0.10f, 5200.f,  3.5f, 3600.f, 0.70f, 0.75f, 0.95f, 0.74f, 0.34f, 1.2f, 3, 3, 0, 0.00f, 0.32f, 0.14f, 0.00f, 0.22f, 0.40f },
    { "Gated", "Arena",      178.f, 0.18f, 3000.f, 0.26f, 2800.f,  8.0f, 2200.f, 0.90f, 0.92f, 0.75f, 0.70f, 0.16f, 2.6f, 1, 1, 0, 0.08f, 0.22f, 0.24f, 0.10f, 0.12f, 0.15f },
    { "Gated", "Slap",       228.f, 0.10f, 5600.f, 0.12f, 6000.f,  3.0f, 4200.f, 0.55f, 0.60f, 1.00f, 0.72f, 0.38f, 1.1f, 3, 2, 0, 0.00f, 0.36f, 0.12f, 0.00f, 0.30f, 0.50f },
    // Funk
    { "Funk", "Ghost",     210.f, 0.08f, 3800.f, 0.10f, 4000.f,  3.5f, 2800.f, 0.40f, 0.48f, 0.55f, 0.62f, 0.22f, 1.6f, 0, 3, 0, 0.00f, 0.22f, 0.12f, 0.05f, 0.08f, 0.18f },
    { "Funk", "Disco",     195.f, 0.14f, 3600.f, 0.18f, 4000.f,  5.5f, 2600.f, 0.72f, 0.80f, 0.88f, 0.74f, 0.24f, 1.8f, 0, 2, 0, 0.00f, 0.28f, 0.16f, 0.00f, 0.15f, 0.28f },
    { "Funk", "Tight Pop", 218.f, 0.10f, 4400.f, 0.12f, 4800.f,  4.0f, 3200.f, 0.62f, 0.70f, 0.95f, 0.74f, 0.30f, 1.4f, 0, 3, 0, 0.00f, 0.30f, 0.14f, 0.00f, 0.18f, 0.35f },
    { "Funk", "Dry Funk",  192.f, 0.12f, 3200.f, 0.12f, 3000.f,  6.0f, 2200.f, 0.82f, 0.68f, 0.80f, 0.76f, 0.18f, 2.0f, 0, 3, 0, 0.00f, 0.22f, 0.10f, 0.00f, 0.10f, 0.18f },
    { "Funk", "Clave",     265.f, 0.06f, 7000.f, 0.08f, 6400.f,  2.2f, 5200.f, 0.25f, 0.18f, 1.00f, 0.74f, 0.36f, 0.9f, 0, 2, 1, 0.00f, 0.28f, 0.06f, 0.00f, 0.22f, 0.62f },
    { "Funk", "Pop",       202.f, 0.12f, 4000.f, 0.14f, 4200.f,  5.0f, 2800.f, 0.75f, 0.72f, 0.90f, 0.74f, 0.26f, 1.6f, 0, 2, 0, 0.04f, 0.28f, 0.16f, 0.00f, 0.16f, 0.28f },
    // Rock
    { "Rock", "Backbeat",    185.f, 0.18f, 3400.f, 0.24f, 3600.f,  7.5f, 2400.f, 0.90f, 0.92f, 0.88f, 0.72f, 0.20f, 2.2f, 0, 0, 0, 0.06f, 0.28f, 0.26f, 0.05f, 0.18f, 0.22f },
    { "Rock", "Stadium",     178.f, 0.22f, 3000.f, 0.30f, 2800.f,  9.0f, 2000.f, 0.92f, 0.95f, 0.78f, 0.70f, 0.16f, 2.8f, 0, 1, 0, 0.08f, 0.22f, 0.28f, 0.10f, 0.20f, 0.18f },
    { "Rock", "Punk",        210.f, 0.10f, 4800.f, 0.12f, 5200.f,  4.0f, 3600.f, 0.68f, 0.70f, 1.00f, 0.62f, 0.34f, 1.4f, 0, 4, 0, 0.10f, 0.32f, 0.16f, 0.00f, 0.50f, 0.40f },
    { "Rock", "Metal",       238.f, 0.08f, 6800.f, 0.12f, 7600.f,  2.8f, 5600.f, 0.50f, 0.78f, 1.00f, 0.58f, 0.38f, 1.2f, 0, 4, 1, 0.18f, 0.28f, 0.32f, 0.00f, 0.60f, 0.52f },
    { "Rock", "Snare Crack", 225.f, 0.10f, 5600.f, 0.12f, 6000.f,  3.5f, 4200.f, 0.58f, 0.62f, 1.00f, 0.68f, 0.36f, 1.3f, 0, 4, 0, 0.08f, 0.35f, 0.14f, 0.00f, 0.45f, 0.55f },
    { "Rock", "Live",        182.f, 0.24f, 3200.f, 0.34f, 3000.f, 10.0f, 2100.f, 0.88f, 0.95f, 0.72f, 0.70f, 0.14f, 3.0f, 0, 0, 0, 0.06f, 0.24f, 0.30f, 0.08f, 0.22f, 0.16f },
    // Hip-Hop snares, not subs
    { "Hip-Hop", "Dusty",  178.f, 0.18f, 2400.f, 0.22f, 2200.f,  8.0f, 1800.f, 0.82f, 0.78f, 0.70f, 0.72f, 0.18f, 2.4f, 0, 0, 0, 0.04f, 0.18f, 0.16f, 0.32f, 0.05f, 0.12f },
    { "Hip-Hop", "Trap",   215.f, 0.10f, 4200.f, 0.12f, 4800.f,  3.8f, 3200.f, 0.55f, 0.62f, 0.95f, 0.68f, 0.36f, 1.3f, 0, 2, 0, 0.06f, 0.32f, 0.12f, 0.08f, 0.22f, 0.40f },
    { "Hip-Hop", "Chop",   198.f, 0.08f, 3800.f, 0.10f, 4000.f,  4.5f, 2800.f, 0.70f, 0.58f, 0.88f, 0.72f, 0.30f, 1.5f, 3, 3, 0, 0.00f, 0.28f, 0.10f, 0.10f, 0.18f, 0.28f },
    { "Hip-Hop", "Vinyl",  170.f, 0.22f, 1600.f, 0.26f, 1400.f, 10.0f, 1200.f, 0.88f, 0.60f, 0.45f, 0.70f, 0.10f, 3.2f, 0, 1, 0, 0.06f, 0.12f, 0.14f, 0.48f, 0.00f, 0.05f },
    { "Hip-Hop", "Layer",  188.f, 0.16f, 3000.f, 0.20f, 3400.f,  6.5f, 2200.f, 0.85f, 0.80f, 0.82f, 0.72f, 0.22f, 2.0f, 0, 0, 0, 0.08f, 0.26f, 0.18f, 0.12f, 0.12f, 0.20f },
    { "Hip-Hop", "Knock",  205.f, 0.10f, 4600.f, 0.12f, 5000.f,  3.5f, 3400.f, 0.68f, 0.62f, 1.00f, 0.70f, 0.38f, 1.2f, 0, 2, 0, 0.00f, 0.36f, 0.12f, 0.05f, 0.28f, 0.50f },
    // Electro snares
    { "Electro", "Digital",    240.f, 0.08f, 6400.f, 0.10f, 7200.f,  2.5f, 5200.f, 0.40f, 0.55f, 1.00f, 0.68f, 0.34f, 1.1f, 0, 2, 1, 0.15f, 0.30f, 0.18f, 0.00f, 0.30f, 0.45f },
    { "Electro", "Noise",      200.f, 0.12f, 2800.f, 0.32f, 4200.f,  6.0f, 2400.f, 0.22f, 1.00f, 0.50f, 0.70f, 0.18f, 2.0f, 0, 1, 0, 0.08f, 0.16f, 0.10f, 0.40f, 0.08f, 0.12f },
    { "Electro", "Body",       168.f, 0.26f, 2200.f, 0.18f, 2000.f,  9.0f, 1600.f, 1.00f, 0.48f, 0.55f, 0.72f, 0.16f, 2.8f, 0, 1, 0, 0.12f, 0.22f, 0.12f, 0.18f, 0.08f, 0.10f },
    { "Electro", "Noise Clap", 195.f, 0.10f, 3600.f, 0.28f, 4800.f,  5.0f, 3000.f, 0.28f, 0.95f, 0.70f, 0.70f, 0.26f, 1.6f, 0, 2, 0, 0.06f, 0.24f, 0.08f, 0.30f, 0.15f, 0.22f },
    { "Electro", "Sharp",      255.f, 0.06f, 7600.f, 0.08f, 8200.f,  1.8f, 6400.f, 0.32f, 0.50f, 1.00f, 0.72f, 0.40f, 0.9f, 0, 2, 1, 0.00f, 0.32f, 0.12f, 0.00f, 0.35f, 0.70f },
    { "Electro", "Tiny",       320.f, 0.05f, 8800.f, 0.06f, 9400.f,  1.2f, 7800.f, 0.18f, 0.38f, 1.00f, 0.74f, 0.42f, 0.7f, 0, 3, 1, 0.00f, 0.22f, 0.08f, 0.00f, 0.28f, 0.80f },
    // Weird snares
    { "Weird", "Ring",          250.f, 0.28f, 4800.f, 0.36f, 6200.f,  6.0f, 3600.f, 0.55f, 0.80f, 0.50f, 0.70f, 0.12f, 2.8f, 0, 0, 1, 0.40f, 0.20f, 0.55f, 0.00f, 0.10f, 0.18f },
    { "Weird", "Paper",         230.f, 0.08f, 5000.f, 0.10f, 4200.f,  4.0f, 3200.f, 0.35f, 0.22f, 0.85f, 0.74f, 0.28f, 1.4f, 0, 3, 0, 0.00f, 0.18f, 0.05f, 0.20f, 0.12f, 0.35f },
    { "Weird", "Trash",         210.f, 0.12f, 3600.f, 0.18f, 3000.f,  5.5f, 2400.f, 0.60f, 0.70f, 0.88f, 0.58f, 0.24f, 1.8f, 0, 4, 1, 0.30f, 0.22f, 0.28f, 0.15f, 0.40f, 0.30f },
    { "Weird", "Spring",        198.f, 0.20f, 2800.f, 0.50f, 3400.f,  8.0f, 2000.f, 0.50f, 0.95f, 0.45f, 0.70f, 0.12f, 3.0f, 0, 0, 0, 0.18f, 0.25f, 0.65f, 0.10f, 0.00f, 0.10f },
    { "Weird", "Sympathetic",   172.f, 0.40f, 2000.f, 0.80f, 1800.f, 12.0f, 1400.f, 0.70f, 0.88f, 0.35f, 0.72f, 0.08f, 4.5f, 0, 1, 0, 0.22f, 0.12f, 0.48f, 0.18f, 0.00f, 0.00f },
    { "Weird", "Splash",        260.f, 0.10f, 6400.f, 0.22f, 7800.f,  3.0f, 5000.f, 0.32f, 0.75f, 0.80f, 0.70f, 0.22f, 1.5f, 0, 2, 1, 0.12f, 0.40f, 0.20f, 0.00f, 0.22f, 0.45f },
};

inline constexpr FactoryFx factoryFx[] = {
    { "80s",          0.85f, 0.55f, -1.f,  0, -1, 0.35f, 0.42f, 0.35f },
    { "Room",         1.00f, 0.28f, -1.f,  0, -1, 0.35f, 0.22f, 0.50f },
    { "Plate",        0.95f, 0.40f, -1.f,  0, -1, 0.35f, 0.38f, 0.28f },
    { "Arena",        0.88f, 0.48f, -1.f,  0, -1, 0.35f, 0.62f, 0.45f },
    { "Slap",         1.00f, 0.22f, -1.f,  0, -1, 0.35f, 0.16f, 0.40f },
    { "Disco",        1.00f, 0.18f, -1.f,  0, -1, 0.35f, 0.20f, 0.50f },
    { "Stadium",      0.90f, 0.42f, -1.f,  0, -1, 0.35f, 0.55f, 0.50f },
    { "Punk",         0.80f, 0.45f, 0.55f, 1, -1, 0.35f, -1.f,  0.40f },
    { "Metal",        0.75f, 0.50f, 0.62f, 1, -1, 0.35f, -1.f,  0.40f },
    { "Live",         1.00f, 0.24f, -1.f,  0, -1, 0.35f, 0.32f, 0.55f },
    { "Vinyl",        0.92f, 0.22f, 0.22f, 0, -1, 0.35f, -1.f,  0.40f },
    { "Digital",      0.85f, 0.35f, 0.40f, 2, -1, 0.35f, -1.f,  0.40f },
    { "Trash",        0.70f, 0.60f, 0.70f, 3, -1, 0.35f, -1.f,  0.40f },
    { "Spring",       0.90f, 0.38f, -1.f,  0,  1, 0.38f, -1.f,  0.40f },
    { "Sympathetic",  0.88f, 0.40f, -1.f,  0, -1, 0.35f, 0.55f, 0.40f },
    { "Splash",       0.92f, 0.32f, -1.f,  0, -1, 0.35f, 0.28f, 0.30f },
    { "Concert",      1.00f, 0.30f, -1.f,  0, -1, 0.35f, 0.48f, 0.55f },
};

inline constexpr FactoryEq factoryEq[] = {
    { "Piccolo",   4, 400.f,  0.f, 0.70f, 3, 6000.f,  3.f, 0.7f },
    { "Fat Head",  2, 180.f,  3.f, 0.70f, 1,  350.f, -2.f, 1.0f },
    { "Vinyl",     5, 6500.f, 0.f, 0.70f, 2,  200.f,  2.f, 0.7f },
    { "Body",      2, 160.f,  4.f, 0.70f, 1, 4000.f, -2.f, 0.8f },
    { "Metallic",  3, 7000.f,  4.f, 0.70f, 1,  800.f, -3.f, 1.0f },
    { "Off",       4, 200.f,  0.f, 0.70f, 0,    0.f,  0.f, 0.7f },
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

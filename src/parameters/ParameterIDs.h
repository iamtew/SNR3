#pragma once

namespace ParamID
{
inline constexpr int eqBandCount = 5;
inline constexpr int layerCount = 4; // Shell, Wires, Stick, Sample
inline constexpr int slotCount = 3;  // silent extra buses for now

constexpr const char* tune      = "tune";
constexpr const char* shellDec  = "shellDec";
constexpr const char* shellTone = "shellTone";
constexpr const char* wiresDec  = "wiresDec";
constexpr const char* wiresTone = "wiresTone"; // kept for old patches; DSP uses wiresHp/Lp
constexpr const char* wiresHp   = "wiresHp";
constexpr const char* wiresLp   = "wiresLp";
constexpr const char* stickDec  = "stickDec";
constexpr const char* stickTone = "stickTone";
constexpr const char* sampleHp  = "sampleHp";
constexpr const char* sampleLp  = "sampleLp";
constexpr const char* sampleLen = "sampleLen";
constexpr const char* sampleStart = "sampleStart";
constexpr const char* sampleRev   = "sampleRev";
constexpr const char* sampleDec   = "sampleDec";
constexpr const char* wiresBuzz  = "wiresBuzz";
constexpr const char* wiresColor = "wiresColor";
constexpr const char* stickHard  = "stickHard";
constexpr const char* stickClick = "stickClick";
constexpr const char* shellLen  = "shellLen";
constexpr const char* wiresLen  = "wiresLen";
constexpr const char* stickLen  = "stickLen";
constexpr const char* level     = "level";
constexpr const char* length    = "length";
constexpr const char* holdMode  = "holdMode";
constexpr const char* style     = "style";
constexpr const char* bodyShape = "bodyShape";
constexpr const char* harm      = "harm";
constexpr const char* velAmt    = "velAmt";
constexpr const char* width     = "width";
constexpr const char* duckDepth = "duckDepth";
constexpr const char* duckTime  = "duckTime";

inline constexpr const char* lyGain[layerCount] = { "ly0Gain", "ly1Gain", "ly2Gain", "ly3Gain" };
inline constexpr const char* lyMute[layerCount] = { "ly0Mute", "ly1Mute", "ly2Mute", "ly3Mute" };
inline constexpr const char* lySolo[layerCount] = { "ly0Solo", "ly1Solo", "ly2Solo", "ly3Solo" };
inline constexpr const char* lyFlip[layerCount] = { "ly0Flip", "ly1Flip", "ly2Flip", "ly3Flip" };
inline constexpr const char* lySend[layerCount] = { "ly0Send", "ly1Send", "ly2Send", "ly3Send" };
inline constexpr const char* lyDelay[layerCount] = { "ly0Delay", "ly1Delay", "ly2Delay", "ly3Delay" };
inline constexpr const char* lyAtk[layerCount] = { "ly0Atk", "ly1Atk", "ly2Atk", "ly3Atk" };
inline constexpr const char* lyPan[layerCount] = { "ly0Pan", "ly1Pan", "ly2Pan", "ly3Pan" };
inline constexpr const char* lyDrive[layerCount] = { "ly0Drive", "ly1Drive", "ly2Drive", "ly3Drive" };

inline constexpr const char* lyEqOn[layerCount] = { "ly0EqOn", "ly1EqOn", "ly2EqOn", "ly3EqOn" };
inline constexpr const char* lyEqType[layerCount][eqBandCount] = {
    { "ly0Eq1Type", "ly0Eq2Type", "ly0Eq3Type", "ly0Eq4Type", "ly0Eq5Type" },
    { "ly1Eq1Type", "ly1Eq2Type", "ly1Eq3Type", "ly1Eq4Type", "ly1Eq5Type" },
    { "ly2Eq1Type", "ly2Eq2Type", "ly2Eq3Type", "ly2Eq4Type", "ly2Eq5Type" },
    { "ly3Eq1Type", "ly3Eq2Type", "ly3Eq3Type", "ly3Eq4Type", "ly3Eq5Type" }
};
inline constexpr const char* lyEqFreq[layerCount][eqBandCount] = {
    { "ly0Eq1Freq", "ly0Eq2Freq", "ly0Eq3Freq", "ly0Eq4Freq", "ly0Eq5Freq" },
    { "ly1Eq1Freq", "ly1Eq2Freq", "ly1Eq3Freq", "ly1Eq4Freq", "ly1Eq5Freq" },
    { "ly2Eq1Freq", "ly2Eq2Freq", "ly2Eq3Freq", "ly2Eq4Freq", "ly2Eq5Freq" },
    { "ly3Eq1Freq", "ly3Eq2Freq", "ly3Eq3Freq", "ly3Eq4Freq", "ly3Eq5Freq" }
};
inline constexpr const char* lyEqGain[layerCount][eqBandCount] = {
    { "ly0Eq1Gain", "ly0Eq2Gain", "ly0Eq3Gain", "ly0Eq4Gain", "ly0Eq5Gain" },
    { "ly1Eq1Gain", "ly1Eq2Gain", "ly1Eq3Gain", "ly1Eq4Gain", "ly1Eq5Gain" },
    { "ly2Eq1Gain", "ly2Eq2Gain", "ly2Eq3Gain", "ly2Eq4Gain", "ly2Eq5Gain" },
    { "ly3Eq1Gain", "ly3Eq2Gain", "ly3Eq3Gain", "ly3Eq4Gain", "ly3Eq5Gain" }
};
inline constexpr const char* lyEqQ[layerCount][eqBandCount] = {
    { "ly0Eq1Q", "ly0Eq2Q", "ly0Eq3Q", "ly0Eq4Q", "ly0Eq5Q" },
    { "ly1Eq1Q", "ly1Eq2Q", "ly1Eq3Q", "ly1Eq4Q", "ly1Eq5Q" },
    { "ly2Eq1Q", "ly2Eq2Q", "ly2Eq3Q", "ly2Eq4Q", "ly2Eq5Q" },
    { "ly3Eq1Q", "ly3Eq2Q", "ly3Eq3Q", "ly3Eq4Q", "ly3Eq5Q" }
};

constexpr const char* fxDry      = "fxDry";
constexpr const char* fxWet      = "fxWet";
constexpr const char* fxDstOn    = "fxDstOn";
constexpr const char* fxDstType  = "fxDstType";
constexpr const char* fxDstDrive = "fxDstDrive";
constexpr const char* fxDstTone  = "fxDstTone";
constexpr const char* fxDstShape = "fxDstShape";
constexpr const char* fxDlyOn    = "fxDlyOn";
constexpr const char* fxDlyTime  = "fxDlyTime";
constexpr const char* fxDlyFb    = "fxDlyFb";
constexpr const char* fxDlyCut   = "fxDlyCut";
constexpr const char* fxDlySync  = "fxDlySync";
constexpr const char* fxDlyDiv   = "fxDlyDiv";
constexpr const char* fxRevOn       = "fxRevOn";
constexpr const char* fxRevSize     = "fxRevSize";
constexpr const char* fxRevDamp     = "fxRevDamp";
constexpr const char* fxRevWidth    = "fxRevWidth";
constexpr const char* fxRevPredelay = "fxRevPredelay";
constexpr const char* fxRevSync     = "fxRevSync";
constexpr const char* fxRevDiv      = "fxRevDiv";

inline constexpr const char* postEqOn = "postEqOn";
inline constexpr const char* postEqType[eqBandCount] = {
    "postEq1Type", "postEq2Type", "postEq3Type", "postEq4Type", "postEq5Type"
};
inline constexpr const char* postEqFreq[eqBandCount] = {
    "postEq1Freq", "postEq2Freq", "postEq3Freq", "postEq4Freq", "postEq5Freq"
};
inline constexpr const char* postEqGain[eqBandCount] = {
    "postEq1Gain", "postEq2Gain", "postEq3Gain", "postEq4Gain", "postEq5Gain"
};
inline constexpr const char* postEqQ[eqBandCount] = {
    "postEq1Q", "postEq2Q", "postEq3Q", "postEq4Q", "postEq5Q"
};
}

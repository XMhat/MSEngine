/* == PCMDEF.HPP =========================================================== **
** ######################################################################### **
** ## MS-ENGINE              Copyright (c) MS-Design, All Rights Reserved ## **
** ######################################################################### **
** ## A class to help store and access PCM data and its metadata.         ## **
** ######################################################################### **
** ========================================================================= */
#pragma once                           // Only one incursion allowed
/* ------------------------------------------------------------------------- */
namespace IPcmLib {                    // Start of private module namespace
/* -- Dependencies --------------------------------------------------------- */
using namespace IFlags;                using namespace IMemory::P;
using namespace IOal::P;               using namespace IStd::P;
using namespace Lib::OpenAL;
/* ------------------------------------------------------------------------- */
namespace P {                          // Start of public module namespace
/* -- Loading flags -------------------------------------------------------- */
BUILD_FLAGS(Pcm,
  /* -- Commands (Only used in 'Pcm' class) -------------------------------- */
  // No loading flags                  Force load as WAV
  PL_NONE                {0x00000000}, PL_FCE_WAV             {0x00000001},
  // Force load as CAF                 Force load as OGG
  PL_FCE_CAF             {0x00000002}, PL_FCE_OGG             {0x00000004},
  // Force load as MP3
  PL_FCE_MP3             {0x00000008},
  /* -- Private flags (Only used in 'PcmData' class) ----------------------- */
  // Bitmap is dynamically created
  PL_DYNAMIC             {0x00000010},
  /* -- Mask bits ---------------------------------------------------------- */
  PL_MASK{ PL_FCE_WAV|PL_FCE_CAF|PL_FCE_OGG|PL_FCE_MP3 }
);/* -- Variables ---------------------------------------------------------- */
class PcmData :                        // Audio data structure
  /* ----------------------------------------------------------------------- */
  public PcmFlags                      // Shared with 'Pcm' class if needed
{ /* ----------------------------------------------------------------------- */
  unsigned int     uiRate,             // Samples per second (Frequency/Hz)
                   uiChannels,         // Channels per sample
                   uiBits;             // Bits per channel
  ALenum           eFormat,            // StrFormat type for openal
                   eSFormat;           // Single channel format for openal
  array<Memory,2>  aPcm;               // Pcm data (aPcmR used if stereo)
  size_t           stAlloc;            // Bytes allocated
  /* -- Public variables ------------------------------------------- */ public:
  Memory           &aPcmL,             // First Pcm channel (Mono or left)
                   &aPcmR;             // Second Pcm channel (Right stereo)
  /* ----------------------------------------------------------------------- */
  size_t GetAlloc(void) const { return stAlloc; }
  /* ----------------------------------------------------------------------- */
  void ClearData(void)
    { for(size_t stIndex = 0; stIndex < aPcm.size(); ++stIndex)
        aPcm[stIndex].DeInit(); }
  /* ----------------------------------------------------------------------- */
  unsigned int GetRate(void) const { return uiRate; }
  /* ----------------------------------------------------------------------- */
  void SetRate(unsigned int uiNRate) { uiRate = uiNRate; }
  /* ----------------------------------------------------------------------- */
  unsigned int GetChannels(void) const { return uiChannels; }
  /* ----------------------------------------------------------------------- */
  void SetChannels(unsigned int uiNChannels) { uiChannels = uiNChannels; }
  /* ----------------------------------------------------------------------- */
  bool SetChannelsSafe(unsigned int uiNChannels)
    { SetChannels(uiNChannels); return uiNChannels >= 1 && uiNChannels <= 2; }
  /* ----------------------------------------------------------------------- */
  unsigned int GetBits(void) const { return uiBits; }
  /* ----------------------------------------------------------------------- */
  void SetBits(unsigned int uiNBits) { uiBits = uiNBits; }
  /* ----------------------------------------------------------------------- */
  ALenum GetFormat(void) const { return eFormat; }
  /* ----------------------------------------------------------------------- */
  ALenum GetSFormat(void) const { return eSFormat; }
  /* ----------------------------------------------------------------------- */
  bool ParseOALFormat(void)
    { return Oal::GetOALType(GetChannels(), GetBits(), eFormat, eSFormat); }
  /* ----------------------------------------------------------------------- */
#define FH(n, f) \
  bool Is ## n(void) const { return FlagIsSet(f); } \
  bool IsNot ## n(void) const { return !Is ## n(); } \
  void Set ## n(bool bState=true) { FlagSetOrClear(f, bState); } \
  void Clear ## n(void) { Set ## n(false); }
  /* ----------------------------------------------------------------------- */
  FH(Dynamic, PL_DYNAMIC)              // Is/Set/ClearDynamic
  /* ----------------------------------------------------------------------- */
#undef FH                              // Done with this macro
  /* -- Set allocated data size -------------------------------------------- */
  void SetAlloc(const size_t stNAlloc) { stAlloc = stNAlloc; }
  /* ----------------------------------------------------------------------- */
  void SetSlot(Memory &mData) { aPcmL = StdMove(mData); }
  /* ----------------------------------------------------------------------- */
  void ResetAllData(void)
  { // Reset all data
    uiRate = uiChannels = uiBits = 0;
    eFormat = eSFormat = AL_NONE;
    // Deinit all channel memory
    ClearData();
  }
  /* ----------------------------------------------------------------------- */
  void PcmDataSwap(PcmData &pcmRef)
  { // Swap data members
    FlagSwap(pcmRef);
    swap(uiRate, pcmRef.uiRate);
    swap(uiChannels, pcmRef.uiChannels);
    swap(uiBits, pcmRef.uiBits);
    swap(eFormat, pcmRef.eFormat);
    swap(eSFormat, pcmRef.eSFormat);
    swap(stAlloc, pcmRef.stAlloc);
    // Swap all channeld ata
    for(size_t stIndex = 0; stIndex < aPcm.size(); ++stIndex)
      aPcm[stIndex].SwapMemory(StdMove(pcmRef.aPcm[stIndex]));
  }
  /* -- Default constructor ------------------------------------------------ */
  PcmData(void) :
    /* -- Initialisers ----------------------------------------------------- */
    PcmFlags{ PL_NONE },               // Flags not initialised
    uiRate(0),                         // Rate not initialised
    uiChannels(0),                     // Channels not initialised
    uiBits(0),                         // Bits per channel not initialised
    eFormat(AL_NONE),                  // StrFormat not initialised
    eSFormat(AL_NONE),                 // Singal channel format not initialised
    stAlloc(0),                        // No memory allocated
    aPcmL(aPcm.front()),               // Alias of first pcm channel
    aPcmR(aPcm.back())                 // Alias of second pcm channel
    /* -- No code ---------------------------------------------------------- */
    { }
};/* ----------------------------------------------------------------------- */
}                                      // End of public module namespace
/* ------------------------------------------------------------------------- */
}                                      // End of private module namespace
/* == EoF =========================================================== EoF == */

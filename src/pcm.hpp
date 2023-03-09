/* == PCM.HPP ============================================================== */
/* ######################################################################### */
/* ## MS-ENGINE              Copyright (c) MS-Design, All Rights Reserved ## */
/* ######################################################################### */
/* ## This module defines a class that can load sound files and then      ## */
/* ## can optionally be sent to openal for playback.                      ## */
/* ######################################################################### */
/* ========================================================================= */
#pragma once                           // Only one incursion allowed
/* -- Module namespace ----------------------------------------------------- */
namespace IfPcm {                      // Keep declarations neatly categorised
/* -- Includes ------------------------------------------------------------- */
using namespace IfPcmFormat;           // Using pcmformat interface
using namespace IfAsset;               // Using asset interface
/* -- Loading flags -------------------------------------------------------- */
BUILD_FLAGS(Pcm,
  /* -- Commands ----------------------------------------------------------- */
  // No loading flags                  Force load as WAV
  PL_NONE                {0x00000000}, PL_FCE_WAV             {0x10000000},
  // Force load as CAF                 Force load as OGG
  PL_FCE_CAF             {0x20000000}, PL_FCE_OGG             {0x40000000},
  // Force load as MP3
  PL_FCE_MP3             {0x80000000},
  /* -- Mask bits ---------------------------------------------------------- */
  PL_MASK{ PL_FCE_WAV|PL_FCE_CAF|PL_FCE_OGG|PL_FCE_MP3 }
);/* == Pcm collector and member class ===================================== */
BEGIN_ASYNCCOLLECTORDUO(Pcms, Pcm, CLHelperUnsafe, ICHelperUnsafe),
  /* -- Base classes ------------------------------------------------------- */
  public AsyncLoader<Pcm>,             // For loading Pcm's off main-thread
  public Lockable,                     // Lua garbage collector instruction
  public PcmFlags                      // Pcm load settings
{ /* -- Private variables ----------------------------------------- */ private:
  PcmData          pcmData;            // Audio data
  /* -- Get pcm datas ---------------------------------------------- */ public:
  Memory &GetData(void) { return pcmData.aPcm; }
  Memory &GetData2(void) { return pcmData.aPcm2; }
  size_t GetUsage(void) const { return pcmData.aPcm.Size(); }
  size_t GetUsage2(void) const { return pcmData.aPcm2.Size(); }
  size_t GetTotalUsage(void) const { return GetUsage() + GetUsage2(); }
  void ClearData(void) { pcmData.aPcm.DeInit(); pcmData.aPcm2.DeInit(); }
  unsigned int GetRate(void) const { return pcmData.uiRate; }
  unsigned int GetChannels(void) const { return pcmData.uiChannels; }
  unsigned int GetBits(void) const { return pcmData.uiBits; }
  ALenum GetFormat(void) const { return pcmData.eFormat; }
  ALenum GetSFormat(void) const { return pcmData.eSFormat; }
  bool IsDynamic(void) const { return pcmData.bDynamic; }
  size_t GetSize(void) const { return pcmData.stFile; }
  /* -- Split a stereo waveform into two seperate channels ----------------- */
  void Split(void)
  { // Set size of PCM sample
    pcmData.stFile = pcmData.aPcm.Size();
    // If pcm data only is single channel, we don't need to split channels.
    if(pcmData.uiChannels == 1) return;
    // If the right channel was already filled then we don't need to do it
    if(!pcmData.aPcm2.Empty()) return;
    // Move pcm file data to a temporary array
    const Memory aTemp{ move(pcmData.aPcm) };
    // Initialise buffers, half the size since we're only splitting
    Memory &aPcmL = pcmData.aPcm;      // Reference to left channel data
    aPcmL.InitBlank(aTemp.Size()/2);
    Memory &aPcmR = pcmData.aPcm2;     // Reference to right channel data
    aPcmR.InitBlank(aPcmL.Size());
    // Iterate through the samples
    for(size_t stIndex = 0,
               stSubIndex = 0,
               stBytes = pcmData.uiBits / 8,
               stStep = pcmData.uiChannels * stBytes;
               stIndex < aTemp.Size();
               stIndex += stStep,
               stSubIndex += stBytes)
    { // De-interleave into seperate channels
      aPcmL.Write(stSubIndex, aTemp.Read(stIndex, stBytes), stBytes);
      aPcmR.Write(stSubIndex, aTemp.Read(stIndex + stBytes, stBytes), stBytes);
    }
  }
  /* -- Load sample from memory -------------------------------------------- */
  void LoadData(FileMap &fC)
  { // Force load a certain type of audio (for speed?) but in Async mode,
    // force detection doesn't really matter as much, but overall, still
    // needed if speed is absolutely neccesary.
    if     (FlagIsSet(PL_FCE_WAV)) PcmLoadFile(0, fC, pcmData);
    else if(FlagIsSet(PL_FCE_CAF)) PcmLoadFile(1, fC, pcmData);
    else if(FlagIsSet(PL_FCE_OGG)) PcmLoadFile(2, fC, pcmData);
    else if(FlagIsSet(PL_FCE_MP3)) PcmLoadFile(3, fC, pcmData);
    // Auto detection of pcm audio
    else PcmLoadFile(fC, pcmData);
    // Split into two channels if audio in stereo
    Split();
  }
  /* -- Reload data -------------------------------------------------------- */
  void ReloadData(void)
    { FileMap fmData{ AssetExtract(IdentGet()) }; LoadData(fmData); }
  /* -- Load pcm from memory asynchronously -------------------------------- */
  void InitAsyncArray(lua_State*const lS)
  { // Need 6 parameters (class pointer was already pushed onto the stack);
    CheckParams(lS, 7);
    // Get and check parameters
    const string strF{ GetCppStringNE(lS, 1, "Identifier") };
    Asset &aData = *GetPtr<Asset>(lS, 2, "Asset");
    FlagReset(GetFlags(lS, 3, PL_NONE, PL_MASK, "Flags"));
    CheckFunction(lS, 4, "ErrorFunc");
    CheckFunction(lS, 5, "ProgressFunc");
    CheckFunction(lS, 6, "SuccessFunc");
    // Is dynamic because it was not loaded from disk
    pcmData.bDynamic = true;
    // Load sample from memory asynchronously
    AsyncInitArray(lS, strF, "pcmarray", move(aData));
  }
  /* -- Load pcm from file asynchronously ---------------------------------- */
  void InitAsyncFile(lua_State*const lS)
  { // Need 5 parameters (class pointer was already pushed onto the stack);
    CheckParams(lS, 6);
    // Get and check parameters
    const string strF{ GetCppFileName(lS, 1, "File") };
    FlagReset(GetFlags(lS, 2, PL_NONE, PL_MASK, "Flags"));
    CheckFunction(lS, 3, "ErrorFunc");
    CheckFunction(lS, 4, "ProgressFunc");
    CheckFunction(lS, 5, "SuccessFunc");
    // Load sample from file asynchronously
    AsyncInitFile(lS, strF, "pcmfile");
  }
  /* -- Init from file ----------------------------------------------------- */
  void InitFile(const string &strFilename, const PcmFlagsConst &lfLF)
  { // Set the loading flags
    FlagReset(lfLF);
    // Load the file normally
    SyncInitFileSafe(strFilename);
  }
  /* -- Init from array ---------------------------------------------------- */
  void InitArray(const string &strName, Memory &&mbD,
    const PcmFlagsConst &lfLF)
  { // Is dynamic because it was not loaded from disk
    pcmData.bDynamic = true;
    // Set the loading flags
    FlagReset(lfLF);
    // Load the array normally
    SyncInitArray(strName, move(mbD));
  }
  /* -- Load audio file from raw memory ------------------------------------ */
  void InitRaw(const string &strN, Memory &&aData, const unsigned int uiR,
    const unsigned int uiC, const unsigned int uiB)
  { // Validate parameters
    if(uiR < 1 || uiR > 5644800) XC("Bogus sample rate!",
      "Identifier", strN, "Rate", uiR);
    if(uiC < 1 || uiC > 2) XC("Bogus channels per sample!",
      "Identifier", strN, "Channels", uiC);
    if(uiB < 1 || uiB > 4 || !IsPow2(uiB)) XC("Bogus bits-per-channel!",
      "Identifier", strN, "Bits", uiB);
    // Calculate bytes per sample
    const size_t stBytesPerSample =
      pcmData.uiRate * pcmData.uiChannels * pcmData.uiBits;
    if(aData.Size() != stBytesPerSample)
      XC("Argument not valid for specified array!",
        "Identifier",     strN,             "Rate", uiR,
        "Channels",       uiC,              "Bits", uiB,
        "BytesPerSample", stBytesPerSample, "Size", aData.Size());
    // Set members
    IdentSet(strN);
    pcmData.bDynamic = true;
    pcmData.aPcm.SwapMemory(move(aData));
    pcmData.uiRate = uiR;
    pcmData.uiChannels = uiC;
    pcmData.uiBits = uiB;
    // Check that format is supported in OpenAL
    if(!Oal::GetOALType(pcmData.uiChannels, pcmData.uiBits, pcmData.eFormat,
      pcmData.eSFormat)) XC("Format not supported by AL!",
        "Identifier", IdentGet(),
        "Channels",   pcmData.uiChannels, "Bits", pcmData.uiBits);
    // Split audio into two channels if audio in stereo
    Split();
    // Load succeeded so register the block
    CollectorRegister();
  }
  /* ----------------------------------------------------------------------- */
  void SwapPcm(Pcm &pcmOtherRef)
  { // Swap data members
    swap(pcmData.uiRate, pcmOtherRef.pcmData.uiRate);
    swap(pcmData.uiChannels, pcmOtherRef.pcmData.uiChannels);
    swap(pcmData.uiBits, pcmOtherRef.pcmData.uiBits);
    swap(pcmData.eFormat, pcmOtherRef.pcmData.eFormat);
    swap(pcmData.eSFormat, pcmOtherRef.pcmData.eSFormat);
    swap(pcmData.bDynamic, pcmOtherRef.pcmData.bDynamic);
    swap(pcmData.stFile, pcmOtherRef.pcmData.stFile);
    pcmData.aPcm.SwapMemory(move(pcmOtherRef.pcmData.aPcm));
    pcmData.aPcm2.SwapMemory(move(pcmOtherRef.pcmData.aPcm2));
    // Swap flags, async, lua lock data and registration
    FlagSwap(pcmOtherRef);
    IdentSwap(pcmOtherRef);
    LockSwap(pcmOtherRef);
    CollectorSwapRegistration(pcmOtherRef);
  }
  /* -- Constructor -------------------------------------------------------- */
  Pcm(void) :                          // Default onstructor
    /* -- Initialisation of members ---------------------------------------- */
    ICHelperPcm{ *cPcms },             // Initially unregistered
    AsyncLoader<Pcm>{ this,            // Setup async loader with this class
      EMC_MP_PCM },                    // ...and the event id for this class
    PcmFlags{ PL_NONE }                // No loading flags right now
    /* -- No code ---------------------------------------------------------- */
    { }
  /* -- Constructor -------------------------------------------------------- */
  Pcm(                                 // Constructor to MOVE from other object
    /* -- Parameters ------------------------------------------------------- */
    Pcm &&pcmOther                     // Other Pcm reference to swap with
    ): /* -- Initialisation of members ------------------------------------- */
    Pcm()                              // Default initialisation of members
    /* -- Swap members with other class ------------------------------------ */
    { SwapPcm(pcmOther); }
  /* -- Destructor (override) ---------------------------------------------- */
  ~Pcm(void) { AsyncCancel(); }
  /* ----------------------------------------------------------------------- */
  DELETECOPYCTORS(Pcm);                // Disable copy constructor and operator
};/* -- End-of-collector --------------------------------------------------- */
END_ASYNCCOLLECTOR(Pcms, Pcm, PCM);
/* -- End of module namespace ---------------------------------------------- */
};                                     // End of interface
/* == EoF =========================================================== EoF == */

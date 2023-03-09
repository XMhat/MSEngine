/* == PCM.HPP ============================================================== */
/* ######################################################################### */
/* ## MS-ENGINE              Copyright (c) MS-Design, All Rights Reserved ## */
/* ######################################################################### */
/* ## This module defines a class that can load sound files and then      ## */
/* ## can optionally be sent to openal for playback.                      ## */
/* ######################################################################### */
/* ========================================================================= */
#pragma once                           // Only one incursion allowed
/* ------------------------------------------------------------------------- */
namespace IfPcm {                      // Start of module namespace
/* -- Includes ------------------------------------------------------------- */
using namespace IfPcmFormat;           // Using pcmformat namespace
using namespace IfAsset;               // Using asset namespace
/* == Pcm collector and member class ======================================= */
BEGIN_ASYNCCOLLECTORDUO(Pcms, Pcm, CLHelperUnsafe, ICHelperUnsafe),
  /* -- Base classes ------------------------------------------------------- */
  public AsyncLoader<Pcm>,             // For loading Pcm's off main-thread
  public Lockable,                     // Lua garbage collector instruction
  public PcmData                       // Pcm data
{ /* -- Split a stereo waveform into two seperate channels --------- */ public:
  void Split(void)
  { // If pcm data only is single channel, we don't need to split channels.
    if(GetChannels() == 1) return;
    // If the right channel was already filled then we don't need to do it
    if(!aPcmR.Empty()) return;
    // Move pcm file data to a temporary array
    const Memory aTemp{ StdMove(aPcmL) };
    // Initialise buffers, half the size since we're only splitting
    aPcmL.InitBlank(aTemp.Size()/2);
    aPcmR.InitBlank(aPcmL.Size());
    // Iterate through the samples
    for(size_t stIndex = 0,
               stSubIndex = 0,
               stBytes = GetBits() / 8,
               stStep = GetChannels() * stBytes;
               stIndex < aTemp.Size();
               stIndex += stStep,
               stSubIndex += stBytes)
    { // De-interleave into seperate channels
      aPcmL.Write(stSubIndex, aTemp.Read(stIndex, stBytes), stBytes);
      aPcmR.Write(stSubIndex, aTemp.Read(stIndex + stBytes, stBytes), stBytes);
    }
  }
  /* -- Split a stereo waveform and set allocation size -------------------- */
  void SplitAndSetAlloc(void)
  { // Split audio into two channels if audio in stereo
    Split();
    // Set allocated size
    SetAlloc(aPcmL.Size() + aPcmR.Size());
  }
  /* -- Load sample from memory -------------------------------------------- */
  void AsyncReady(FileMap &fC)
  { // Force load a certain type of audio (for speed?) but in Async mode,
    // force detection doesn't really matter as much, but overall, still
    // needed if speed is absolutely neccesary.
    if     (FlagIsSet(PL_FCE_WAV)) PcmLoadFile(0, fC, *this);
    else if(FlagIsSet(PL_FCE_CAF)) PcmLoadFile(1, fC, *this);
    else if(FlagIsSet(PL_FCE_OGG)) PcmLoadFile(2, fC, *this);
    else if(FlagIsSet(PL_FCE_MP3)) PcmLoadFile(3, fC, *this);
    // Auto detection of pcm audio
    else PcmLoadFile(fC, *this);
    // Split into two channels if audio in stereo
    SplitAndSetAlloc();
  }
  /* -- Reload data -------------------------------------------------------- */
  void ReloadData(void)
  { // Load the file from disk or archive
    FileMap fmData{ AssetExtract(IdentGet()) };
    // Reset memory usage to zero
    SetAlloc(0);
    // Run codecs and filters on it
    AsyncReady(fmData);
  }
  /* -- Load pcm from memory asynchronously -------------------------------- */
  void InitAsyncArray(lua_State*const lS)
  { // Need 6 parameters (class pointer was already pushed onto the stack);
    CheckParams(lS, 7);
    // Get and check parameters
    const string strF{ GetCppStringNE(lS, 1, "Identifier") };
    Asset &aData = *GetPtr<Asset>(lS, 2, "Asset");
    FlagReset(GetFlags(lS, 3, PL_MASK, "Flags"));
    CheckFunction(lS, 4, "ErrorFunc");
    CheckFunction(lS, 5, "ProgressFunc");
    CheckFunction(lS, 6, "SuccessFunc");
    // Is dynamic because it was not loaded from disk
    SetDynamic();
    // Load sample from memory asynchronously
    AsyncInitArray(lS, strF, "pcmarray", StdMove(aData));
  }
  /* -- Load pcm from file asynchronously ---------------------------------- */
  void InitAsyncFile(lua_State*const lS)
  { // Need 5 parameters (class pointer was already pushed onto the stack);
    CheckParams(lS, 6);
    // Get and check parameters
    const string strF{ GetCppFileName(lS, 1, "File") };
    FlagReset(GetFlags(lS, 2, PL_MASK, "Flags"));
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
    SetDynamic();
    // Set the loading flags
    FlagReset(lfLF);
    // Load the array normally
    SyncInitArray(strName, StdMove(mbD));
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
    const size_t stBytesPerSample = GetRate() * GetChannels() * GetBits();
    if(aData.Size() != stBytesPerSample)
      XC("Argument not valid for specified array!",
         "Identifier",     strN,             "Rate", uiR,
         "Channels",       uiC,              "Bits", uiB,
         "BytesPerSample", stBytesPerSample, "Size", aData.Size());
    // Set members
    IdentSet(strN);
    SetDynamic();
    aPcmL.SwapMemory(StdMove(aData));
    SetRate(uiR);
    SetChannels(uiC);
    SetBits(uiB);
    // Check that format is supported in OpenAL
    if(!ParseOALFormat())
      XC("Format not supported by AL!",
         "Identifier", IdentGet(),
         "Channels",   GetChannels(), "Bits", GetBits());
    // Split audio into two channels if audio in stereo
    SplitAndSetAlloc();
    // Load succeeded so register the block
    CollectorRegister();
  }
  /* ----------------------------------------------------------------------- */
  void SwapPcm(Pcm &pcmRef)
  { // Swap datas
    IdentSwap(pcmRef);
    LockSwap(pcmRef);
    CollectorSwapRegistration(pcmRef);
    PcmDataSwap(pcmRef);
  }
  /* -- Constructor -------------------------------------------------------- */
  Pcm(void) :                          // Default onstructor
    /* -- Initialisers ----------------------------------------------------- */
    ICHelperPcm{ *cPcms },             // Initially unregistered
    IdentCSlave{ cParent.CtrNext() },  // Initialise identification number
    AsyncLoader<Pcm>{ this,            // Setup async loader with this class
      EMC_MP_PCM }                     // ...and the event id for this class
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
  DELETECOPYCTORS(Pcm)                 // Disable copy constructor and operator
};/* -- End-of-collector --------------------------------------------------- */
END_ASYNCCOLLECTOR(Pcms, Pcm, PCM);
/* ------------------------------------------------------------------------- */
};                                     // End of module namespace
/* == EoF =========================================================== EoF == */

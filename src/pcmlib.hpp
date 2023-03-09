/* == PCMLIB.HPP =========================================================== */
/* ######################################################################### */
/* ## MS-ENGINE              Copyright (c) MS-Design, All Rights Reserved ## */
/* ######################################################################### */
/* ## This module manages all the different audio types we support in the ## */
/* ## engine.                                                             ## */
/* ######################################################################### */
/* ========================================================================= */
#pragma once                           // Only one incursion allowed
/* -- Module namespace ----------------------------------------------------- */
namespace IfPcmLib {                   // Keep declarations neatly categorised
/* -- Includes ------------------------------------------------------------- */
using namespace IfOal;                 // Using oal interface
using namespace IfFileMap;             // Using filemap interface
/* -- Variables ------------------------------------------------------------ */
struct PcmData                         // Audio data structure
{ /* ----------------------------------------------------------------------- */
  unsigned int     uiRate;             // Samples per second (Frequency/Hz)
  unsigned int     uiChannels;         // Channels per sample
  unsigned int     uiBits;             // Bits per channel
  ALenum           eFormat;            // Format type for openal
  ALenum           eSFormat;           // Single channel format for openal
  bool             bDynamic;           // Sample is not static?
  size_t           stFile;             // The size of the pcm data
  Memory           aPcm, aPcm2;        // Pcm data (aPcm2 used if stereo)
};/* ----------------------------------------------------------------------- */
/* == Pcm libraries collector class ======================================== */
BEGIN_COLLECTOREX(PcmFmts, PcmFmt, CLHelperUnsafe, size_t stId);
/* == Pcm format object class ============================================== */
BEGIN_MEMBERCLASS(PcmFmts, PcmFmt, ICHelperUnsafe)
{ /* -- Typedefs -------------------------------------------------- */ private:
  typedef bool (&CBLFunc)(FileMap&, PcmData&);
  typedef bool (&CBSFunc)(const FStream&, const PcmData&);
  /* -- Variables ---------------------------------------------------------- */
  const size_t     stId;               // Unique id number
  const char*const cpName;             // Name of plugin
  const char*const cpExt;              // Default extension of plugin type
  CBLFunc          acLoader;           // Loader callback
  CBSFunc          acSaver;            // Saver callback
  /* -- Unsupported callbacks ---------------------------------------------- */
  static bool NoLoader(FileMap&, PcmData&) { return false; }
  static bool NoSaver(const FStream&, const PcmData&) { return false; }
  /* -- Get members ------------------------------------------------ */ public:
  CBLFunc     GetLoader(void) const { return acLoader; }
  CBSFunc     GetSaver(void) const { return acSaver; }
  const char *GetName(void) const { return cpName; }
  const char *GetExt(void) const { return cpExt; }
  size_t      GetId(void) const { return stId; }
  bool        HaveLoader(void) const { return acLoader != NoLoader; }
  bool        HaveSaver(void) const { return acSaver != NoSaver; }
  /* -- Constructor -------------------------------------------------------- */
  explicit PcmFmt(                     // Constructor to initialise plugin
    /* -- Parameters ------------------------------------------------------- */
    const char*const cpNewName,        // Name of plugin
    const char*const cpNewExt,         // Default extension
    CBLFunc acNewLoader=NoLoader,      // Loader function (static)
    CBSFunc acNewSaver=NoSaver         // Saver function (static)
    ): /* -- Initialisers -------------------------------------------------- */
    ICHelperPcmFmt{ *cPcmFmts, this }, // Register pcm format in collector
    stId(cPcmFmts->stId++),            // Set unique identification number
    cpName(cpNewName),                 // Set name of plugin
    cpExt(cpNewExt),                   // Set default extension of plugin
    acLoader{ acNewLoader },           // Set loader function
    acSaver{ acNewSaver }              // Set saver function
    /* -- No code ---------------------------------------------------------- */
    { }
  /* ----------------------------------------------------------------------- */
  DELETECOPYCTORS(PcmFmt);             // Omit copy constructor for safety
};/* -- End ---------------------------------------------------------------- */
END_COLLECTOREX(PcmFmts,,,,stId(0));
/* -- Load audio using a specific type ------------------------------------- */
static void PcmLoadFile(const size_t stFId, FileMap &fC, PcmData &auD)
{ // Ignore if plugin is invalid
  if(stFId >= cPcmFmts->size())
    XC("Invalid format!", "Identifier", fC.IdentGet(), "FormatId", stFId,
                          "Maximum",    cPcmFmts->size());
  // Get plugin class. We already checked if the index was valid
  PcmFmt &pCref = **next(cPcmFmts->cbegin(), static_cast<ssize_t>(stFId));
  // Capture exceptions
  try
  { // Load the image and if loaded successfully?
    if(pCref.GetLoader()(fC, auD))
    { // Log that we loaded the pcm and return
      LW(LH_INFO, "Pcm loaded '$' directly as $<$>! ($;$;$;$$;$;$;$$)",
        fC.IdentGet(), pCref.GetExt(), stFId, auD.uiRate, auD.uiChannels,
        auD.uiBits, hex, auD.eFormat, auD.eSFormat, TrueOrFalse(auD.bDynamic),
        hex, auD.stFile);
      return;
    } // Could not detect format so throw error
    throw runtime_error{ "Unable to load sound!" };
  } // Error occured. Error used as title
  catch(const exception &E)
  { // Throw an error with the specified reason
    XC(E.what(),
      "Identifier", fC.IdentGet(),  "Size",     fC.Size(),
      "Position",   fC.FileMapTell(), "FormatId", stFId,
      "Plugin",     pCref.GetName());
  }
}
/* -- Load a bitmap and automatically detect type -------------------------- */
static void PcmLoadFile(FileMap &fC, PcmData &auD)
{ // For each plugin registered
  for(const PcmFmt*const pCptr : *cPcmFmts)
  { // Get reference to plugin
    const PcmFmt &pCref = *pCptr;
    // Capture exceptions
    try
    { // Load the bitmap if we loaded successfully?
      if(pCref.GetLoader()(fC, auD))
      { // Log that we loaded and return
        LW(LH_INFO, "Pcm loaded '$' as $! ($;$;$;$$;$;$;$$)", fC.IdentGet(),
          pCref.GetExt(), auD.uiRate, auD.uiChannels, auD.uiBits, hex,
          auD.eFormat, auD.eSFormat, TrueOrFalse(auD.bDynamic),
          dec, auD.stFile);
        return;
      }
    } // Error occured. Error used as title
    catch(const exception &E)
    { // Throw an error with the specified reason
      XC(E.what(), "Identifier", fC.IdentGet(),  "Size",   fC.Size(),
                   "Position",   fC.FileMapTell(), "Plugin", pCref.GetName());
    } // Rewind stream position and reset other members to try next filter
    fC.FileMapRewind();
    auD.uiRate = auD.uiChannels = auD.uiBits = 0;
    auD.eFormat = auD.eSFormat = AL_NONE;
    auD.stFile = 0;
    auD.aPcm.DeInit();
    auD.aPcm2.DeInit();
  } // Could not detect so throw error
  XC("Unable to determine sound format!", "Identifier", fC.IdentGet());
}
/* -- End of module namespace ---------------------------------------------- */
};                                     // End of interface
/* == EoF =========================================================== EoF == */

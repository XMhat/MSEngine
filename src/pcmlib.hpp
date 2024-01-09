/* == PCMLIB.HPP =========================================================== **
** ######################################################################### **
** ## MS-ENGINE              Copyright (c) MS-Design, All Rights Reserved ## **
** ######################################################################### **
** ## This module manages all the different audio types we support in the ## **
** ## engine.                                                             ## **
** ######################################################################### **
** ========================================================================= */
#pragma once                           // Only one incursion allowed
/* ------------------------------------------------------------------------- */
namespace IPcmLib {                    // Start of private module namespace
/* -- Dependencies --------------------------------------------------------- */
using namespace ICollector::P;         using namespace IError::P;
using namespace IFileMap::P;           using namespace IFStream::P;
using namespace IIdent::P;             using namespace ILog::P;
using namespace IPcmLib::P;            using namespace IString::P;
using namespace ISysUtil::P;
/* ------------------------------------------------------------------------- */
namespace P {                          // Start of public module namespace
/* == Pcm libraries collector class ======================================== */
BEGIN_COLLECTOR(PcmFmts, PcmFmt, CLHelperUnsafe)
/* == Pcm format object class ============================================== */
BEGIN_MEMBERCLASS(PcmFmts, PcmFmt, ICHelperUnsafe)
{ /* -- Typedefs -------------------------------------------------- */ private:
  typedef bool (&CBLFunc)(FileMap&, PcmData&);
  typedef bool (&CBSFunc)(const FStream&, const PcmData&);
  /* -- Variables ---------------------------------------------------------- */
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
    IdentCSlave{ cParent.CtrNext() },  // Initialise identification number
    cpName(cpNewName),                 // Set name of plugin
    cpExt(cpNewExt),                   // Set default extension of plugin
    acLoader{ acNewLoader },           // Set loader function
    acSaver{ acNewSaver }              // Set saver function
    /* -- No code ---------------------------------------------------------- */
    { }
  /* ----------------------------------------------------------------------- */
  DELETECOPYCTORS(PcmFmt)              // Omit copy constructor for safety
};/* -- End ---------------------------------------------------------------- */
END_COLLECTOR(PcmFmts)                 // Finish collector class
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
      cLog->LogInfoExSafe("Pcm loaded '$' directly as $<$>! ($;$;$;$$;$;$;$$)",
        fC.IdentGet(), pCref.GetExt(), stFId, auD.GetRate(), auD.GetChannels(),
        auD.GetBits(), hex, auD.GetFormat(), auD.GetSFormat(),
        StrFromBoolTF(auD.IsDynamic()), hex, auD.GetAlloc());
      return;
    } // Could not detect format so throw error
    throw runtime_error{ "Unable to load sound!" };
  } // Error occured. Error used as title
  catch(const exception &E)
  { // Throw an error with the specified reason
    XC(E.what(), "Identifier", fC.IdentGet(),    "Size",     fC.Size(),
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
        cLog->LogInfoExSafe("Pcm loaded '$' as $! ($;$;$;$$;$;$;$$)",
          fC.IdentGet(), pCref.GetExt(), auD.GetRate(), auD.GetChannels(),
          auD.GetBits(), hex, auD.GetFormat(), auD.GetSFormat(),
          StrFromBoolTF(auD.IsDynamic()), dec, auD.GetAlloc());
        return;
      }
    } // Error occured. Error used as title
    catch(const exception &E)
    { // Throw an error with the specified reason
      XC(E.what(), "Identifier", fC.IdentGet(),    "Size",   fC.Size(),
                   "Position",   fC.FileMapTell(), "Plugin", pCref.GetName());
    } // Rewind stream position
    fC.FileMapRewind();
    // Reset all pcm data read to load again
    auD.ResetAllData();
  } // Could not detect so throw error
  XC("Unable to determine sound format!", "Identifier", fC.IdentGet());
}
/* ------------------------------------------------------------------------- */
}                                      // End of public module namespace
/* ------------------------------------------------------------------------- */
}                                      // End of private module namespace
/* == EoF =========================================================== EoF == */

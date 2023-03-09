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
/* -- Pcm formats collector class as a vector for direct access ------------ */
CTOR_BEGIN_CUSTCTR(PcmLibs, PcmLib, vector, CLHelperUnsafe)
/* -- Pcm format object class ---------------------------------------------- */
CTOR_MEM_BEGIN_CSLAVE(PcmLibs, PcmLib, ICHelperUnsafe)
{ /* -- Typedefs -------------------------------------------------- */ private:
  typedef bool (&CBLFunc)(FileMap&, PcmData&);
  typedef bool (&CBSFunc)(const FStream&, const PcmData&);
  /* -- Variables ---------------------------------------------------------- */
  const string_view strvName,          // Name of plugin
                    strvExt;           // Default extension of plugin type
  CBLFunc           cblfFunc;          // Loader callback
  CBSFunc           cbsfFunc;          // Saver callback
  const PcmFormat   pfId;              // Image format id
  /* -- Check id number ---------------------------------------------------- */
  PcmFormat CheckId(const PcmFormat pfNId)
  { // The id should match the collector count
    const size_t stExpect = cParent->size() - 1;
    if(pfNId == stExpect) return pfNId;
    // Make sure the ImageFormats match the codec construction order!
    XC("Internal error: PCM format id mismatch!",
       "Id",     pfNId,    "Expect",    stExpect,
       "Filter", strvName, "Extension", strvExt);
  }
  /* -- Unsupported callbacks ---------------------------------------------- */
  static bool NoLoader(FileMap&, PcmData&) { return false; }
  static bool NoSaver(const FStream&, const PcmData&) { return false; }
  /* -- Get members ------------------------------------------------ */ public:
  CBLFunc GetLoader(void) const { return cblfFunc; }
  CBSFunc GetSaver(void) const { return cbsfFunc; }
  const string_view &GetName(void) const { return strvName; }
  const string_view &GetExt(void) const { return strvExt; }
  bool HaveLoader(void) const { return cblfFunc != NoLoader; }
  bool HaveSaver(void) const { return cbsfFunc != NoSaver; }
  /* -- Constructor -------------------------------------------------------- */
  explicit PcmLib(                     // Constructor to initialise plugin
    /* -- Required arguments ----------------------------------------------- */
    const PcmFormat pfNId,             // The unique PCM codec id
    const string_view &strvNName,      // Name of plugin
    const string_view &strvNExt,       // Default extension
    CBLFunc cblfNFunc=NoLoader,        // Loader function (static)
    CBSFunc cbsfNFunc=NoSaver          // Saver function (static)
    ): /* -- Initialisers -------------------------------------------------- */
    ICHelperPcmLib{ cPcmLibs, this },  // Register pcm format in collector
    IdentCSlave{ cParent->CtrNext() }, // Initialise identification number
    strvName(strvNName),               // Set name of plugin
    strvExt(strvNExt),                 // Set default extension of plugin
    cblfFunc(cblfNFunc),               // Set loader function
    cbsfFunc(cbsfNFunc),               // Set saver function
    pfId(CheckId(pfNId))               // Set unique id for this codec
    /* -- No code ---------------------------------------------------------- */
    { }
  /* ----------------------------------------------------------------------- */
  DELETECOPYCTORS(PcmLib)              // Omit copy constructor for safety
};/* -- End of objects collector (reserve and set limit for formats) ------- */
CTOR_END(PcmLibs, PcmLib, reserve(PFMT_MAX); CollectorSetLimit(PFMT_MAX),)
/* -- Load audio using a specific type ------------------------------------- */
static void PcmLoadFile(const PcmFormat pfId, FileMap &fmData, PcmData &pdData)
{ // Get plugin class. We already checked if the index was valid
  const PcmLib &plRef = *cPcmLibs->at(pfId);
  // Capture exceptions
  try
  { // Load the image, log and return if loaded successfully
    if(plRef.GetLoader()(fmData, pdData))
      return cLog->LogInfoExSafe(
        "Pcm loaded '$' directly as $<$>! ($;$;$;$$;$;$;$$)",
        fmData.IdentGet(), plRef.GetExt(), pfId, pdData.GetRate(),
        pdData.GetChannels(), pdData.GetBits(), hex, pdData.GetFormat(),
        pdData.GetSFormat(), StrFromBoolTF(pdData.IsDynamic()), hex,
        pdData.GetAlloc());
    // Could not detect format so throw error
    throw runtime_error{ "Unable to load sound!" };
  } // Error occured. Error used as title
  catch(const exception &E)
  { // Throw an error with the specified reason
    XC(E.what(), "Identifier", fmData.IdentGet(),
                 "Size",       fmData.MemSize(),
                 "Position",   fmData.FileMapTell(),
                 "FormatId",   pfId,
                 "Plugin",     plRef.GetName());
  }
}
/* -- Load a bitmap and automatically detect type -------------------------- */
static void PcmLoadFile(FileMap &fmData, PcmData &pdData)
{ // For each plugin registered
  for(const PcmLib*const plPtr : *cPcmLibs)
  { // Get reference to plugin
    const PcmLib &plRef = *plPtr;
    // Capture exceptions
    try
    { // Load the bitmap, log and return if we loaded successfully
      if(plRef.GetLoader()(fmData, pdData))
        return cLog->LogInfoExSafe("Pcm loaded '$' as $! ($;$;$;$$;$;$;$$)",
          fmData.IdentGet(), plRef.GetExt(), pdData.GetRate(),
          pdData.GetChannels(), pdData.GetBits(), hex, pdData.GetFormat(),
          pdData.GetSFormat(), StrFromBoolTF(pdData.IsDynamic()), dec,
          pdData.GetAlloc());
    } // Error occured. Error used as title
    catch(const exception &E)
    { // Throw an error with the specified reason
      XC(E.what(), "Identifier", fmData.IdentGet(),
                   "Size",       fmData.MemSize(),
                   "Position",   fmData.FileMapTell(),
                   "Plugin",     plRef.GetName());
    } // Rewind stream position
    fmData.FileMapRewind();
    // Reset all pcm data read to load again
    pdData.ResetAllData();
  } // Could not detect so throw error
  XC("Unable to determine sound format!", "Identifier", fmData.IdentGet());
}
/* ------------------------------------------------------------------------- */
}                                      // End of public module namespace
/* ------------------------------------------------------------------------- */
}                                      // End of private module namespace
/* == EoF =========================================================== EoF == */

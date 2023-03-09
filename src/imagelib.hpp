/* == IMAGELIB.HPP ========================================================= */
/* ######################################################################### */
/* ## MS-ENGINE              Copyright (c) MS-Design, All Rights Reserved ## */
/* ######################################################################### */
/* ## This file manages all the different image types we support in the   ## */
/* ## engine. The actual image class will query this manager to test      ## */
/* ## To use this, you build your class and derive your class with this   ## */
/* ## class to enable the plugin to be used by the Image class            ## */
/* ######################################################################### */
/* ========================================================================= */
#pragma once                           // Only one incursion allowed
/* ------------------------------------------------------------------------- */
namespace IfImageLib {                 // Start of module namespace
/* -- Includes ------------------------------------------------------------- */
using namespace IfImageDef;            // Using image definitions namespace
using namespace IfCollector;           // Using collector namespace
using namespace IfFileMap;             // Using filemap namespace
/* == Image libraries collector class ====================================== */
BEGIN_COLLECTOR(ImageFmts, ImageFmt, CLHelperUnsafe)
/* == Image libraries format object class ================================== */
BEGIN_MEMBERCLASS(ImageFmts, ImageFmt, ICHelperUnsafe)
{ /* -- Typedefs -------------------------------------------------- */ private:
  typedef bool (&CBLFunc)(FileMap&, ImageData&);
  typedef bool (&CBSFunc)(const FStream&, const ImageData&, const ImageSlot&);
  /* -- Variables ---------------------------------------------------------- */
  const char*const cpExt;              // Default extension of plugin type
  const char*const cpName;             // Name of plugin
  CBLFunc          icLoader;           // Loader callback
  CBSFunc          icSaver;            // Saver callback
  /* -- Unsupported callbacks----------------------------------------------- */
  static bool NoLoader(FileMap&, ImageData&) { return false; }
  static bool NoSaver(const FStream&, const ImageData&, const ImageSlot&)
    { return false; }
  /* -- Get members ------------------------------------------------ */ public:
  const char *GetName(void) const    { return cpName; }
  const char *GetExt(void) const     { return cpExt; }
  CBLFunc     GetLoader(void) const  { return icLoader; }
  bool        HaveLoader(void) const { return icLoader != NoLoader; }
  CBSFunc     GetSaver(void) const   { return icSaver; }
  bool        HaveSaver(void) const  { return icSaver != NoSaver; }
  /* -- Constructors ------------------------------------------------------- */
  explicit ImageFmt(const char*const cpN, const char*const cpE,
    CBLFunc &icL=NoLoader, CBSFunc &icS=NoSaver) :
    /* -- Initialisers ----------------------------------------------------- */
    ICHelperImageFmt{ *cImageFmts,     // Register filter in filter list
      this },                          // Initialise filter parent
    IdentCSlave{ cParent.CtrNext() },  // Initialise identification number
    cpExt(cpE),                        // Set extension for filter
    cpName(cpN),                       // Set name for filter
    icLoader{ icL },                   // Set loader function
    icSaver{ icS }                     // Set saver function
    /* -- No code ---------------------------------------------------------- */
    { }
  /* ----------------------------------------------------------------------- */
  DELETECOPYCTORS(ImageFmt)            // Omit copy constructor for safety
};/* -- End of objects collector ------------------------------------------- */
END_COLLECTOR(ImageFmts)
/* -- Save a image using a specific type ----------------------------------- */
static void ImageSave(const size_t stFId, const string &strN,
  const ImageData &ifD, const ImageSlot &sData)
{ // Ignore if plugin is invalid
  if(stFId >= cImageFmts->size())
    XC("Format invalid!",
       "Identifier", strN, "FormatId", stFId, "Maximum", cImageFmts->size());
  // Get plugin class
  const ImageFmt &iCref = **next(cImageFmts->cbegin(),
    static_cast<ssize_t>(stFId));
  // Set filename so we can delete it if it fails
  const string strNX{ Append(strN, '.', iCref.GetExt()) };
  bool bCreated = false;
  // Capture exceptions
  try
  { // Create file on disk and report error if failed. Closed automatically
    // when leaving this scope. If there is an exception then the stream is
    // automatically closed
    if(FStream fC{ strNX, FStream::FM_W_B })
    { // Created file
      bCreated = true;
      // Save the image and log the result if succeeded?
      if(iCref.GetSaver()(fC, ifD, sData))
      { // Log that we saved the image successfully and return
        cLog->LogInfoExSafe("Image saved '$' as $<$>! ($x$x$)",
          strNX, iCref.GetExt(), stFId, sData.DimGetWidth(),
          sData.DimGetHeight(), ifD.GetBitsPerPixel());
        return;
      } // Could not detect format so throw error
      throw runtime_error{ "Failed to save image!" };
    } // Failed to create file
    XCL("Failed to create file!", "File", strNX);
  } // Error occured. Error used as title
  catch(const exception &E)
  { // Remove file if created
    if(bCreated) DirFileUnlink(strNX);
    // Throw an error with the specified reason
    XC(E.what(), "Identifier", strNX, "FormatId", stFId,
                 "Plugin",     iCref.GetName());
  }
}
/* -- Load a image using a specific type ----------------------------------- */
static void ImageLoad(const size_t stFId, FileMap &fC, ImageData &ifD)
{ // Ignore if plugin is invalid
  if(stFId >= cImageFmts->size())
    XC("Plugin invalid!", "Identifier", fC.IdentGet(), "FormatId", stFId,
                          "Maximum",    cImageFmts->size());
  // Get plugin class
  ImageFmt &iCref = **next(cImageFmts->cbegin(), static_cast<ssize_t>(stFId));
  // Capture exceptions
  try
  { // Load the image and if succeeded?
    if(iCref.GetLoader()(fC, ifD))
      // Log that we loaded the image and return
      return cLog->LogInfoExSafe("Image loaded '$' directly as $<$>! ($x$x$)",
        fC.IdentGet(), iCref.GetExt(), stFId, ifD.DimGetWidth(),
        ifD.DimGetHeight(), ifD.GetBitsPerPixel());
    // Could not detect format so throw error
    throw runtime_error{ "Unable to load image!" };
  } // Error occured. Error used as title
  catch(const exception &E)
  { // Throw an error with the specified reason
    XC(E.what(), "Identifier", fC.IdentGet(),  "Size",     fC.Size(),
                 "Position",   fC.FileMapTell(), "FormatId", stFId,
                 "Plugin",     iCref.GetName());
  }
}
/* -- Load a image and automatically detect type --------------------------- */
static void ImageLoad(FileMap &fC, ImageData &ifD)
{ // For each plugin registered
  for(ImageFmt*const iCptr : *cImageFmts)
  { // Get reference to plugin
    ImageFmt &iCref = *iCptr;
    // Capture exceptions
    try
    { // Load the image and log and return if we loaded successfully?
      if(iCref.GetLoader()(fC, ifD))
        // Log taht we loaded the image and return
        return cLog->LogInfoExSafe("Image loaded '$' ($x$x$) as $!",
          fC.IdentGet(), ifD.DimGetWidth(), ifD.DimGetHeight(),
          ifD.GetBitsPerPixel(), iCref.GetExt());
    } // Error occured. Error used as title
    catch(const exception &E)
    { // Throw an error with the specified reason
      XC(E.what(), "Identifier", fC.IdentGet(),    "Size",   fC.Size(),
                   "Position",   fC.FileMapTell(), "Plugin", iCref.GetName());
    } // Rewind stream position
    fC.FileMapRewind();
    // Reset other members to try next filter
    ifD.ResetAllData();
  } // Could not detect so throw error
  XC("Unable to determine image format!", "Identifier", fC.IdentGet());
}
/* ------------------------------------------------------------------------- */
static int ImageSwapPixels(char*const cpSrc, const size_t stSrc,
  const size_t stPixel, const size_t stSwapA, const size_t stSwapB)
{ // Check parameters
  if(!stSrc) return 0;
  if(!cpSrc) return -1;
  if(stSwapA >= stPixel) return -1;
  if(stSwapB >= stPixel) return -2;
  if(stPixel >= stSrc) return -3;
  if(stSwapA == stSwapB) return -4;
  // Do swap the pixels
  for(size_t stPos = 0; stPos < stSrc; stPos += stPixel)
  { // Calculate source position and copy character at source
    const size_t stPosA = stPos+stSwapA;
    const char cByte = cpSrc[stPosA];
    // Calculate the destination position and do the swap
    const size_t stPosB = stPos+stSwapB;
    cpSrc[stPosA] = cpSrc[stPosB];
    cpSrc[stPosB] = cByte;
  } // Success
  return 0;
}
/* ------------------------------------------------------------------------- */
};                                     // End of module namespace
/* == EoF =========================================================== EoF == */

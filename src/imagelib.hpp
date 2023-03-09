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
/* -- Module namespace ----------------------------------------------------- */
namespace IfImageLib {                 // Keep declarations neatly categorised
/* -- Includes ------------------------------------------------------------- */
using namespace IfMemory;              // Using memory interface
using namespace IfCollector;           // Using collector interface
using namespace IfFileMap;             // Using filemap interface
/* -- Typedefs ------------------------------------------------------------- */
struct ImageSlot                       // Image slot (support for anims)
{ /* ----------------------------------------------------------------------- */
  Memory           memData;            // Image data file and header
  unsigned int     uiWidth, uiHeight;  // Image dimensions
}; /* ---------------------------------------------------------------------- */
typedef vector<ImageSlot> SlotList;    // list of bitmaps
/* ------------------------------------------------------------------------- */
struct ImageData                       // Image data structure
{ /* ----------------------------------------------------------------------- */
  unsigned int     uiWidth, uiHeight;  // Image dimensions (slot 0, W=Flags)
  unsigned int     uiBitsPerPixel;     // Image bits per pixel
  unsigned int     uiBytesPerPixel;    // Image bytes per pixel
  GLenum           glPixelType;        // Image colour-byte bits
  /* ----------------------------------------------------------------------- */
  bool             bMipmaps;           // Image has mipmaps, not slides
  bool             bReverse;           // Image has reversed byte order
  bool             bCompressed;        // Image is compressed with DXT
  bool             bDynamic;           // Image was not loaded from disk
  /* ----------------------------------------------------------------------- */
  size_t           stAlloc;            // Image data allocated in slots
  size_t           stFile;             // Image size of all slots
  SlotList         sData;              // Image slots data
};/* ----------------------------------------------------------------------- */
/* == Image libraries collector class ====================================== */
BEGIN_COLLECTOREX(ImageFmts, ImageFmt, CLHelperUnsafe, size_t stId);
/* == Image libraries format object class ================================== */
BEGIN_MEMBERCLASS(ImageFmts, ImageFmt, ICHelperUnsafe)
{ /* -- Typedefs -------------------------------------------------- */ private:
  typedef bool (&CBLFunc)(FileMap&, ImageData&);
  typedef bool (&CBSFunc)(const FStream&, const ImageData&, const ImageSlot&);
  /* -- Variables ---------------------------------------------------------- */
  const size_t     stId;               // Unique id number
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
  size_t      GetId(void) const      { return stId; }
  CBLFunc     GetLoader(void) const  { return icLoader; }
  bool        HaveLoader(void) const { return icLoader != NoLoader; }
  CBSFunc     GetSaver(void) const   { return icSaver; }
  bool        HaveSaver(void) const  { return icSaver != NoSaver; }
  /* -- Constructors ------------------------------------------------------- */
  explicit ImageFmt(const char*const cpN, const char*const cpE,
    CBLFunc &icL=NoLoader, CBSFunc &icS=NoSaver) :
    /* -- Initialisation of members ---------------------------------------- */
    ICHelperImageFmt{ *cImageFmts,     // Register filter in filter list
      this },                          // Initialise filter parent
    stId(cImageFmts->stId++),          // Set unique id for filter
    cpExt(cpE),                        // Set extension for filter
    cpName(cpN),                       // Set name for filter
    icLoader{ icL },                   // Set loader function
    icSaver{ icS }                     // Set saver function
    /* -- No code ---------------------------------------------------------- */
    { }
  /* ----------------------------------------------------------------------- */
  DELETECOPYCTORS(ImageFmt);           // Omit copy constructor for safety
};/* -- End of objects collector ------------------------------------------- */
END_COLLECTOREX(ImageFmts,,,,stId(0));
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
        LW(LH_INFO, "Image saved '$' as $<$>! ($x$x$)", strNX, iCref.GetExt(),
          stFId, sData.uiWidth, sData.uiHeight, ifD.uiBitsPerPixel);
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
  { // Load the image and log the result if succeeded?
    if(iCref.GetLoader()(fC, ifD))
    { // Log that we loaded the image and return
      LW(LH_INFO, "Image loaded '$' directly as $<$>! ($x$x$)", fC.IdentGet(),
        iCref.GetExt(), stFId, ifD.uiWidth, ifD.uiHeight, ifD.uiBitsPerPixel);
      return;
    } // Could not detect format so throw error
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
      { // Log taht we loaded the image and return
        LW(LH_INFO, "Image loaded '$' ($x$x$) as $!", fC.IdentGet(),
          ifD.uiWidth, ifD.uiHeight, ifD.uiBitsPerPixel, iCref.GetExt());
        return;
      }
    } // Error occured. Error used as title
    catch(const exception &E)
    { // Throw an error with the specified reason
      XC(E.what(), "Identifier", fC.IdentGet(),  "Size",   fC.Size(),
                   "Position",   fC.FileMapTell(), "Plugin", iCref.GetName());
    } // Rewind stream position and reset other members to try next filter
    fC.FileMapRewind();
    ifD.uiWidth = ifD.uiHeight = ifD.uiBitsPerPixel = ifD.uiBytesPerPixel = 0;
    ifD.stAlloc = ifD.stFile = 0;
    ifD.bMipmaps = ifD.bReverse = ifD.bCompressed = false;
    ifD.glPixelType = GL_NONE;
    ifD.sData.clear();
  } // Could not detect so throw error
  XC("Unable to determine image format!", "Identifier", fC.IdentGet());
}
/* -- SwapComponents ---------------------- Swap components in an image -- */
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
/* -- End of module namespace ---------------------------------------------- */
};                                     // End of interface
/* == EoF =========================================================== EoF == */

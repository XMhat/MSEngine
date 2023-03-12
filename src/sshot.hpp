/* == SSHOT.HPP ============================================================ */
/* ######################################################################### */
/* ## MS-ENGINE              Copyright (c) MS-Design, All Rights Reserved ## */
/* ######################################################################### */
/* ## Contains the class to help with creating screenshots                ## */
/* ######################################################################### */
/* ========================================================================= */
#pragma once                           // Only one incursion allowed
/* ------------------------------------------------------------------------- */
namespace IfSShot {                    // Start of module namespace
/* -- Includes ------------------------------------------------------------- */
using namespace IfThread;              // Using thread namespace
using namespace IfImage;               // Using image namespace
using namespace IfFboMain;             // Using fbomain namespace
/* -- Core fbo wrapper class ----------------------------------------------- */
static class SShot final :             // Members initially private
  /* -- Base classes ------------------------------------------------------- */
  public Thread,                       // Process in background
  public Image                         // Image data class
{ /* -- Screenshot thread -------------------------------------------------- */
  size_t           stFormatId;         // Screenshot id to use
  /* -- Fbo dumper thread callback ----------------------------------------- */
  int DumpThread(const Thread &)
  { // Code to return
    int iReturn;
    // Capture exceptions
    try
    { // Reverse pixels or they will be upside down
      ReversePixels();
      // Save the image to disk
      SaveFile(Image::IdentGet(), 0, stFormatId);
      // Success
      iReturn = 1;
    } // exception occured?
    catch(const exception &E)
    { // Report error
      cLog->LogErrorExSafe("(SCREENSHOT WRITER THREAD EXCEPTION) $", E.what());
      // Errored return code
      iReturn = -1;
    } // Free the memory created with the bitmap
    clear();
    // Return the code specified
    return iReturn;
  }
  /* -- Capture screenshot from FBO -------------------------------- */ public:
  bool DumpFBO(const Fbo &fboRef, const string &strFN={})
  { // Cancel if thread is still running
    if(ThreadIsRunning()) return false;
    // DeInit old thread, we need to reuse it
    ThreadDeInit();
    // Log procedure
    cLog->LogDebugExSafe(
      "SShot '$' grabbing back buffer to write to screenshot...",
      fboRef.IdentGet());
    // Allocate storage (Writing as RGB 24-bit).
    const BitDepth bdBPP = BD_RGB;
    const GLenum eMode = GL_RGB;
    const size_t stBytesPerPixel = BD_RGB / 8;
    Memory mBuffer{ fboRef.DimGetWidth<size_t>() *
      fboRef.DimGetHeight<size_t>() * stBytesPerPixel };
    // Bind the fbo
    GL(cOgl->BindFBO(fboRef.uiFBO),
      "Failed to bind FBO to dump!",
      "Identifier", fboRef.IdentGet(), "Id", fboRef.uiFBO);
    // Bind the texture in the fbo
    GL(cOgl->BindTexture(fboRef.uiFBOtex),
      "Failed to bind FBO texture to dump!",
      "Identifier", fboRef.IdentGet(), "Id", fboRef.uiFBOtex);
    // Read into buffer
    GL(cOgl->ReadTexture(eMode, mBuffer.Ptr<GLvoid>()),
      "Failed to read FBO pixel data!",
      "Identifier", fboRef.IdentGet(), "Mode", eMode);
    // Get new filename or original filename
    Image::IdentSet(strFN.empty() ?
      Append(cCVars->GetInternalStrSafe(APP_SHORTNAME),
        cmSys.FormatTime("-%Y%m%d-%H%M%S")) : strFN);
    // Log status
    cLog->LogDebugExSafe("SShot '$' screen capture to '$' ($x$x$)...",
      fboRef.IdentGet(), Image::IdentGet(), fboRef.DimGetWidth(),
      fboRef.DimGetHeight(), bdBPP);
    // Setup raw image
    InitRaw(Image::IdentGet(), std::move(mBuffer),
      fboRef.DimGetWidth<unsigned int>(), fboRef.DimGetHeight<unsigned int>(),
      bdBPP, eMode);
    // Launch thread to write the screenshot to disk in the background
    ThreadStart();
    // Success
    return true;
  }
  /* -- Dump main fbo ------------------------------------------------------ */
  void DumpMain(void) { DumpFBO(cFboMain->fboMain); }
  /* -- Default constructor ------------------------------------------------ */
  SShot(void) :                        // No parameters
    /* -- Initialisation of members ---------------------------------------- */
    Thread{ "sshot",                   // Prepare screenshot thread
      bind(&SShot::DumpThread,         // Dump thread entry function
        this, _1) },                   // Send this class pointer
    stFormatId(0)                      // Not truly initialised yet
    /* -- No code ---------------------------------------------------------- */
    { }
  /* -- Set image format type ---------------------------------------------- */
  CVarReturn SetScreenShotType(const size_t stId)
    { return CVarSimpleSetIntNGE(stFormatId, stId, cImageFmts->size()); }
  /* ----------------------------------------------------------------------- */
  DELETECOPYCTORS(SShot);              // Supress copy constructor for safety
  /* ----------------------------------------------------------------------- */
} *cSShot = nullptr;                   // Pointer to static class
/* ------------------------------------------------------------------------- */
};                                     // End of module namespace
/* == EoF =========================================================== EoF == */

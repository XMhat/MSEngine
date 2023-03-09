/* == SSHOT.HPP ============================================================ */
/* ######################################################################### */
/* ## MS-ENGINE              Copyright (c) MS-Design, All Rights Reserved ## */
/* ######################################################################### */
/* ## Contains the class to help with creating screenshots                ## */
/* ######################################################################### */
/* ========================================================================= */
#pragma once                           // Only one incursion allowed
/* -- Module namespace ----------------------------------------------------- */
namespace IfSShot {                    // Keep declarations neatly categorised
/* -- Includes ------------------------------------------------------------- */
using namespace IfThread;              // Using thread interface
using namespace IfImage;               // Using image interface
using namespace IfFboMain;             // Using fbomain interface
/* == Core fbo wrapper class =============================================== */
static class SShot :
  /* -- Base classes ------------------------------------------------------- */
  public Thread,                       // Process in background
  public Image,                        // Image data class
  public string                        // Filename to save to
{ /* -- Screenshot thread ------------------------------------------ */ public:
  size_t           stScreenShotId;     // Screenshot id to use
  /* == Fbo dumper thread callback ========================================= */
  int DumpThread(const Thread &)
  { // Code to return
    int iReturn;
    // Capture exceptions
    try
    { // Reverse pixels or they will be upside down
      ReversePixels();
      // Save the image to disk
      SaveFile(data(), 0, stScreenShotId);
      // Success
      iReturn = 1;
    } // exception occured?
    catch(const exception &E)
    { // Report error
      LW(LH_ERROR, "(SCREENSHOT WRITER THREAD EXCEPTION) $", E.what());
      // Errored return code
      iReturn = -1;
    } // Free the memory created with the bitmap
    ClearData();
    // Return the code specified
    return iReturn;
  }
  /* -- Capture screenshot from FBO ---------------------------------------- */
  bool DumpFBO(const Fbo &fboRef, const string &strFN={})
  { // Cancel if thread is still running
    if(ThreadIsRunning()) return false;
    // DeInit old thread, we need to reuse it
    ThreadDeInit();
    // Log procedure
    LW(LH_DEBUG, "SShot '$' grabbing back buffer to write to screenshot...",
      fboRef.IdentGet());
    // Allocate storage (Writing as RGB 24-bit).
    const unsigned int uiBitsPerPixel = 24;
    const GLenum eMode = GL_RGB;
    const size_t stBytesPerPixel = uiBitsPerPixel / 8;
    Memory aOut{ static_cast<size_t>(fboRef.stFBOWidth * fboRef.stFBOHeight) *
      stBytesPerPixel };
    // Bind the fbo
    GL(cOgl->BindFBO(fboRef.uiFBO),
      "Failed to bind FBO to dump!",
      "Identifier", fboRef.IdentGet(), "Id", fboRef.uiFBO);
    // Bind the texture in the fbo
    GL(cOgl->BindTexture(fboRef.uiFBOtex),
      "Failed to bind FBO texture to dump!",
      "Identifier", fboRef.IdentGet(), "Id", fboRef.uiFBOtex);
    // Read into buffer
    GL(cOgl->ReadTexture(eMode, aOut.Ptr<GLvoid>()),
      "Failed to read FBO pixel data!",
      "Identifier", fboRef.IdentGet(), "Mode", eMode);
    // Get new filename or original filename
    assign(strFN.empty() ? Append(cCVars->GetInternalStrSafe(APP_SHORTNAME),
      cmSys.FormatTime("-%Y%m%d-%H%M%S")) : strFN);
    // Log status
    LW(LH_DEBUG, "SShot '$' screen capture to '$' ($x$x$)...",
      fboRef.IdentGet(), data(), fboRef.stFBOWidth, fboRef.stFBOHeight,
      uiBitsPerPixel);
    // Setup raw image
    InitRaw(c_str(), move(aOut), static_cast<unsigned int>(fboRef.stFBOWidth),
      static_cast<unsigned int>(fboRef.stFBOHeight), uiBitsPerPixel, eMode);
    // Launch thread to write the screenshot to disk in the background
    ThreadStart();
    // Success
    return true;
  }
  /* -- Dump main fbo ------------------------------------------------------ */
  void DumpMain(void) { DumpFBO(cFboMain->fboMain); }
  /* -- Initialise fbos using a different constructor ---------------------- */
  SShot(void) :
    /* -- Initialisation of members ---------------------------------------- */
    Thread{ "sshot",                   // Prepare screenshot thread
      bind(&SShot::DumpThread,         // Dump thread entry function
        this, _1) },                   // Send this class pointer
    stScreenShotId(0)                  // Start screenshot id from zero
    /* -- No code ---------------------------------------------------------- */
    { }
  /* ----------------------------------------------------------------------- */
  DELETECOPYCTORS(SShot);              // Supress copy constructor for safety
  /* -- FboCore::End ------------------------------------------------------- */
} *cSShot = nullptr;                   // Pointer to static class
/* == Set image format type ================================================ */
static CVarReturn SetScreenShotType(const size_t stId)
  { return CVarSimpleSetIntNGE(cSShot->stScreenShotId, stId,
      cImageFmts->size()); }
/* -- End of module namespace ---------------------------------------------- */
};                                     // End of interface
/* == EoF =========================================================== EoF == */

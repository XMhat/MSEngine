/* == SSHOT.HPP ============================================================ **
** ######################################################################### **
** ## MS-ENGINE              Copyright (c) MS-Design, All Rights Reserved ## **
** ######################################################################### **
** ## Contains the class to help with creating screenshots                ## **
** ######################################################################### **
** ========================================================================= */
#pragma once                           // Only one incursion allowed
/* ------------------------------------------------------------------------- */
namespace ISShot {                     // Start of private module namespace
/* -- Dependencies --------------------------------------------------------- */
using namespace IClock::P;             using namespace ICVarDef::P;
using namespace IFbo::P;               using namespace IFboCore::P;
using namespace IImage::P;             using namespace IImageDef::P;
using namespace IImageFormat::P;       using namespace IImageLib::P;
using namespace ILog::P;               using namespace IMemory::P;
using namespace IOgl::P;               using namespace IStd::P;
using namespace IString::P;            using namespace ISystem::P;
using namespace ISysUtil::P;           using namespace IThread::P;
using namespace Lib::OS::GlFW;
/* ------------------------------------------------------------------------- */
namespace P {                          // Start of public module namespace
/* -- Core fbo wrapper class ----------------------------------------------- */
static class SShot final :             // Members initially private
  /* -- Base classes ------------------------------------------------------- */
  public Thread,                       // Process in background
  public Image                         // Image data class
{ /* -- Screenshot thread -------------------------------------------------- */
  ImageFormat          ifFormatId;     // Screenshot id to use
  /* -- Fbo dumper thread callback ----------------------------------------- */
  int DumpThread(const Thread &)
  { // Code to return
    int iReturn;
    // Capture exceptions
    try
    { // Reverse pixels or they will be upside down
      ReversePixels();
      // Save the image to disk
      SaveFile(Image::IdentGet(), 0, ifFormatId);
      // Success
      iReturn = 1;
    } // exception occured?
    catch(const exception &E)
    { // Report error
      cLog->LogErrorExSafe("(SCREENSHOT WRITER THREAD EXCEPTION) $", E.what());
      // Errored return code
      iReturn = -1;
    } // Free the memory created with the bitmap
    ResetAllData();
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
    const size_t stBytesPerPixel = bdBPP / 8;
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
    GL(cOgl->ReadTexture(eMode, mBuffer.MemPtr<GLvoid>()),
      "Failed to read FBO pixel data!",
      "Identifier", fboRef.IdentGet(), "Mode", eMode);
    // Get new filename or original filename
    Image::IdentSet(strFN.empty() ? StrAppend(cSystem->GetGuestShortTitle(),
      cmSys.FormatTime("-%Y%m%d-%H%M%S")) : strFN);
    // Log status
    cLog->LogDebugExSafe("SShot '$' screen capture to '$' ($x$x$)...",
      fboRef.IdentGet(), Image::IdentGet(), fboRef.DimGetWidth(),
      fboRef.DimGetHeight(), bdBPP);
    // Setup raw image
    InitRaw(Image::IdentGet(), StdMove(mBuffer),
      fboRef.DimGetWidth<unsigned int>(), fboRef.DimGetHeight<unsigned int>(),
      bdBPP, eMode);
    // Launch thread to write the screenshot to disk in the background
    ThreadStart();
    // Success
    return true;
  }
  /* -- Dump main fbo ------------------------------------------------------ */
  void DumpMain(void) { DumpFBO(cFboCore->fboMain); }
  /* -- Default constructor ------------------------------------------------ */
  SShot(void) :                        // No parameters
    /* -- Initialisers ----------------------------------------------------- */
    Thread{ "sshot", STP_LOW,          // Prepare low perf screenshot thread
      bind(&SShot::DumpThread,         // Dump thread entry function
        this, _1) },                   // Send this class pointer
    ifFormatId(IFMT_MAX)               // Not truly initialised yet
    /* -- No code ---------------------------------------------------------- */
    { }
  /* -- Set image format type ---------------------------------------------- */
  CVarReturn SetScreenShotType(const ImageFormat ifId)
    { return CVarSimpleSetIntNGE(ifFormatId, ifId, IFMT_MAX); }
  /* ----------------------------------------------------------------------- */
  DELETECOPYCTORS(SShot)               // Supress copy constructor for safety
  /* ----------------------------------------------------------------------- */
} *cSShot = nullptr;                   // Pointer to static class
/* ------------------------------------------------------------------------- */
}                                      // End of public module namespace
/* ------------------------------------------------------------------------- */
}                                      // End of private module namespace
/* == EoF =========================================================== EoF == */

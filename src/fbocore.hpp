/* == FBOCORE.HPP ========================================================== **
** ######################################################################### **
** ## MS-ENGINE              Copyright (c) MS-Design, All Rights Reserved ## **
** ######################################################################### **
** ## Contains the main drawing fbo's and commands. It would be nice to   ## **
** ## have this in the FBOs collector but we can't due to C++ limitations ## **
** ## of initialising static FBO classes when it hasn't been defined yet. ## **
** ######################################################################### **
** ========================================================================= */
#pragma once                           // Only one incursion allowed
/* ------------------------------------------------------------------------- */
namespace IFboCore {                   // Start of private module namespace
/* -- Dependencies --------------------------------------------------------- */
using namespace IClock::P;             using namespace ICVar::P;
using namespace ICVarDef::P;           using namespace ICVarLib::P;
using namespace IEvtMain::P;           using namespace IFboDef::P;
using namespace IFbo::P;               using namespace IGlFW::P;
using namespace ILog::P;               using namespace IOgl::P;
using namespace IShader::P;            using namespace IShaders::P;
using namespace IStd::P;               using namespace IString::P;
using namespace ISysUtil::P;           using namespace ITimer::P;
using namespace IUtil::P;              using namespace Lib::OS::GlFW;
/* ------------------------------------------------------------------------- */
namespace P {                          // Start of public module namespace
/* == Main fbo class ======================================================= */
static class FboCore final :           // The main fbo operations manager
  /* -- Base classes ------------------------------------------------------- */
  public FboColour,                    // Backbuffer clear colour
  public FboBlend,                     // Default blending mode
  public DimGLInt                      // Fbo dimensions
{ /* -- Private variables ----------------------------------------- */ private:
  GLfloat          fOrthoMinimum,      // Minimum orthangal matrix ratio
                   fOrthoMaximum,      // Maximum orthangal matrix ratio
                   fOrthoWidth,        // Requested ortho width
                   fOrthoHeight;       // Requested ortho height
  bool             bDraw,              // Should we draw the main fbo flag
                   bSimpleMatrix,      // Use simple not automatic matrix
                   bLockViewport,      // Lock the viewport?
                   bClearBuffer;       // Clear back buffer?
  /* -- Core fbos -------------------------------------------------- */ public:
  Fbo              fboConsole,         // Console fbo class
                   fboMain;            // Primary fbo class
  /* -- Render timings ----------------------------------------------------- */
  ClkTimePoint     ctStart, ctEnd;     // Rendering frame start/end time
  double           dRTFPS;             // Rendered frames per second
  /* -- Draw flags --------------------------------------------------------- */
  bool CanDraw(void) const { return bDraw; }
  bool CannotDraw(void) const { return !CanDraw(); }
  void SetDraw(void) { bDraw = true; }
  void ClearDraw(void) { bDraw = false; }
  /* -- Get members -------------------------------------------------------- */
  GLfloat GetOrthoWidth(void) const { return fOrthoWidth; }
  GLfloat GetOrthoHeight(void) const { return fOrthoHeight; }
  /* -- Reset backbuffer clear colour to colour stored in cvar ------------- */
  void ResetClearColour(void)
  { // Set main backbuffer colour
    fboMain.FboResetClearColour();
    fboConsole.FboResetClearColour();
    // Commit the default backbuffer clear colour
    cOgl->SetClearColourInt(
      cCVars->GetInternal<unsigned int>(VID_CLEARCOLOUR));
  }
  /* -- Perform rendering to the back buffer ----------------------- */ public:
  void SwapBuffers(void)
  { // Unbind current FBO so we select the back buffer to draw to
    cOgl->BindFBO();
    // Set the first active texture unit
    cOgl->ActiveTexture();
    // Bind the texture attached to the fbo
    cOgl->BindTexture(fboMain.uiFBOtex);
    // Select our basic 3D transform shader
    cOgl->UseProgram(cShaderCore->sh3D.GetProgram());
    // Set the viewport of the FBO size
    cOgl->SetViewport(DimGetWidth(), DimGetHeight());
    // Set the default alpha blending mode
    cOgl->SetBlendIfChanged(*this);
    // Clear back buffer if main fbo has alpha
    if(fboMain.FboIsTransparencyEnabled()) cOgl->SetAndClear(*this);
    // Set normal fill poly mode
    cOgl->SetPolygonMode(GL_FILL);
    // Buffer the interlaced triangle data
    cOgl->BufferStaticData(fboMain.FboItemGetDataSize(),
      fboMain.FboItemGetData());
    // Specify format of the interlaced triangle data
    cOgl->VertexAttribPointer(A_COORD, stCompsPerCoord, 0,
      fboMain.FboItemGetTCIndex());
    cOgl->VertexAttribPointer(A_VERTEX, stCompsPerPos, 0,
      fboMain.FboItemGetVIndex());
    cOgl->VertexAttribPointer(A_COLOUR, stCompsPerColour, 0,
      fboMain.FboItemGetCIndex());
    // Blit the two triangles
    cOgl->DrawArraysTriangles(stTwoTriangles);
    // Swap buffers
    cGlFW->WinSwapGLBuffers();
    // Update memory
    cOgl->UpdateVRAMAvailable();
    // Clear any existing errors
    cOgl->GetError();
    // Grab end rendering time, calc fps and then reset starting render time
    ctEnd = cmHiRes.GetTime();
    dRTFPS = 1.0 / ClockTimePointRangeToDouble(ctEnd, ctStart);
    ctStart = ctEnd;
  }
  /* -- Render the main fbo ------------------------------------------------ */
  void Render(void)
  { // Finish and render the main fbo
    FinishMain();
    // Render all the fbos
    FboRender();
    // We don't want to swap if the guest has chosen not to draw but we should
    // at least wait if the timer isn't already waiting or the GPU is going to
    // be locked at 100% in situations where the engine is in standby mode with
    // an empty tick function.
    if(CannotDraw()) return cTimer->TimerForceWait();
    // Clear redraw flag
    ClearDraw();
    // Swap the buffers
    SwapBuffers();
  }
  /* -- Blits the console fbo to main fbo ---------------------------------- */
  void BlitConsoleToMain(void) { fboMain.FboBlit(fboConsole); }
  /* -- Finish main fbo and add it to render list -------------------------- */
  void FinishMain(void) { fboMain.FboFinishAndRender(); }
  /* -- Set main fbo as active fbo to draw too ----------------------------- */
  void ActivateMain(void) { fboMain.FboSetActive(); }
  /* -- Called from main tick incase we need to keep catching up frames ---- */
  void RenderFbosAndFlushMain(void)
  { // Render all the user queud fbos and flush them
    FboRender();
    // Flush main fbo
    fboMain.FboFlush();
  }
  /* -- De-initialise all fbos --------------------------------------------- */
  void DeInitAllObjectsAndBuiltIns(void)
  { // Temporary de-init all user objects
    FboDeInit();
    // Temporary de-init the console and main static fbo classes too
    fboConsole.FboDeInit();
    fboMain.FboDeInit();
  }
  /* -- Destroy all fbo's -------------------------------------------------- */
  void DestroyAllObjectsAndBuiltIns(void)
  { // Destroy all the fbos. Note this doesn't destroy the static fbo's as they
    // were unregistered in the init function below.
    cFbos->CollectorDestroyUnsafe();
    // Deinit the console and main static fbo classes
    fboConsole.FboDeInit();
    fboMain.FboDeInit();
  }
  /* -- Sent when the window is resized/main fbo needs autosized --- */ public:
  bool AutoMatrix(const GLfloat fWidth, const GLfloat fHeight,
    const bool bForce)
  { // Some variables we'll need to do some calculations
    GLfloat fAspect, fAddWidth, fLeft, fTop, fRight, fBottom;
    // Use a simple matrix. The simple matrix means that the aspect ratio
    // will not be automatically calculated based on window size.
    if(bSimpleMatrix)
    { // Set aspect based on window size and clamp it to allowable matrix
      fAspect = UtilClamp(fWidth / fHeight, fOrthoMinimum, fOrthoMaximum);
      // We're not adding any width, keep it classic style
      fAddWidth = 0.0f;
      // Set bounds
      fLeft = fTop = 0.0f;
      fRight = fWidth;
      fBottom = fHeight;
    } // Don't use simple matrix?
    else
    { // Calculate aspect ratio. We want to clamp the ratio the guests
      // specified minimum and maximum aspect ratio and to also keep the size
      // within the specified multiple value to prevent cracks appearing in
      // between tiles.
      fAspect = UtilClamp(static_cast<GLfloat>(DimGetWidth()) /
                          static_cast<GLfloat>(DimGetHeight()),
                          fOrthoMinimum, fOrthoMaximum) / 1.333333f;
      // For some unknown reason we could be sent invalid values so we need to
      // make sure we ignore this value to prevent error handlers triggering.
      if(fAspect != fAspect) fAspect = 1.0f;
      // Calculate additional width over the 4:3 aspect ratio
      fAddWidth = UtilMaximum(((fWidth * fAspect) - fWidth) / 2.0f, 0.0f);
      // Calculate bounds for stage
      fLeft = floorf(-fAddWidth);
      fRight = floorf(fWidth + fAddWidth);
      // Set top and bottom stage bounds
      fTop = 0.0f;
      fBottom = fHeight;
    } // If the viewport didn't change?
    if(UtilIsFloatEqual(fLeft, fboMain.fcStage.GetCoLeft()) &&
       UtilIsFloatEqual(fTop, fboMain.fcStage.GetCoTop()) &&
       UtilIsFloatEqual(fRight, fboMain.fcStage.GetCoRight()) &&
       UtilIsFloatEqual(fBottom, fboMain.fcStage.GetCoBottom()))
    { // Return if we're not forcing the change
      if(!bForce) return false;
    } // Viewport changed?
    else
    { // Save requested ortho size incase viewport changes
      fOrthoWidth = fWidth;
      fOrthoHeight = fHeight;
      // Set stage bounds for drawing
      fboMain.FboSetOrtho(fLeft, fTop, fRight, fBottom);
    } // Calculate new fbo width and height
    const GLsizei siFBOWidth = static_cast<GLsizei>(fRight - fLeft),
                  siFBOHeight = static_cast<GLsizei>(fBottom - fTop);
    // No point changing anything if the bounds are the same and if the fbo
    // needs updating? Also ignore if opengl isn't initialised as the GLfW FB
    // reset window event might be sent before we've initialised it!
    if((siFBOWidth != static_cast<GLsizei>(fboMain.GetCoRight()) ||
        siFBOHeight != static_cast<GLsizei>(fboMain.GetCoBottom()) ||
        bForce) && cOgl->IsGLInitialised())
    { // Re-initialise the main framebuffer
      fboMain.FboInit("main", siFBOWidth, siFBOHeight);
      // Log computations
      cLog->LogDebugExSafe("Fbo main matrix reinitialised as $x$ [$] "
        "(D=$x$,A=$<$-$>,AW=$,S=$:$:$:$).",
        fboMain.GetCoRight(), fboMain.GetCoBottom(),
        StrFromRatio(fboMain.GetCoRight(), fboMain.GetCoBottom()),
          fWidth, fHeight, fAspect, fOrthoMinimum, fOrthoMaximum, fAddWidth,
          fLeft, fTop, fRight, fBottom);
      // Everything changed
      return true;
    } // Re-initialisation required?
    cLog->LogDebugExSafe("Fbo main matrix recalculated! "
      "(D=$x$,A=$<$-$>,AW=$,S=$:$:$:$).",
      fWidth, fHeight, fAspect, fOrthoMinimum, fOrthoMaximum, fAddWidth,
      fLeft, fTop, fRight, fBottom);
    // Only bounds were changed
    return false;
  }
  /* -- Sent when the window is resized ------------------------------------ */
  bool AutoViewport(const GLsizei siWidth, const GLsizei siHeight,
    const bool bForce=false)
  { // Return if the viewport size did not change
    if(DimGetWidth() == siWidth && DimGetHeight() == siHeight) return false;
    // Chosen viewport to store
    GLsizei siUseWidth, siUseHeight;
    // Lock viewport to ortho?
    if(bLockViewport)
    { // Lock viewport to ortho
      siUseWidth = static_cast<GLsizei>(GetOrthoWidth());
      siUseHeight = static_cast<GLsizei>(GetOrthoHeight());
    } // Lock viewport to requested size?
    else
    { // Lock viewport to requested size clamped to a minimum of 1x1.
      siUseWidth = UtilMaximum(1, siWidth);
      siUseHeight = UtilMaximum(1, siHeight);
    } // Set the new viewport
    DimSet(siUseWidth, siUseHeight);
    // Log event
    cLog->LogDebugExSafe(
      "Fbo processing automatrix size of $x$ to backbuffer...",
      siUseWidth, siUseHeight);
    // Update matrix because the window's aspect ratio may have changed
    const bool bResult = AutoMatrix(GetOrthoWidth(), GetOrthoHeight(), bForce);
    // Inform lua scripts that they should redraw the framebuffer
    if(bResult) cEvtMain->Add(EMC_LUA_REDRAW);
    // Return result
    return bResult;
  }
  /* -- Init console fbo --------------------------------------------------- */
  void InitConsoleFBO(void)
  { // Initialise the console fbo for the console object
    fboConsole.FboInit("console",
      fboMain.DimGetWidth(), fboMain.DimGetHeight());
  }
  /* -- Set main fbo float reserve ----------------------------------------- */
  CVarReturn SetFloatReserve(const size_t stCount)
    { return BoolToCVarReturn(fboMain.FboReserveTriangles(stCount)); }
  /* -- Set main fbo command reserve --------------------------------------- */
  CVarReturn SetCommandReserve(const size_t stCount)
    { return BoolToCVarReturn(fboMain.FboReserveCommands(stCount)); }
  /* -- Set main fbo filters (cvar event) ---------------------------------- */
  CVarReturn SetFilter(const OglFilterEnum ofeV)
  { // Check value
    if(ofeV >= OF_MAX) return DENY;
    // Set filtering of main and console fbo
    fboMain.FboSetFilter(ofeV);
    fboConsole.FboSetFilter(ofeV);
    // Accept the change anyway if opengl not initialised
    if(cOgl->IsGLNotInitialised()) return ACCEPT;
    // Commit the filters
    fboMain.FboCommitFilter();
    fboConsole.FboCommitFilter();
    // Ask LUA to tell guest to redraw if needed
    cEvtMain->Add(EMC_LUA_REDRAW);
    // Return success
    return ACCEPT;
  }
  /* -- Set back buffer clear ---------------------------------------------- */
  CVarReturn SetBackBufferClear(const bool bState)
    { return CVarSimpleSetInt(bClearBuffer, bState); }
  /* -- Set back buffer clear colour --------------------------------------- */
  CVarReturn SetBackBufferClearColour(const unsigned int uiColour)
    { SetColourInt(uiColour); return ACCEPT; }
  /* -- Set minimum orthagonal matrix ratio -------------------------------- */
  CVarReturn SetMinOrtho(const GLfloat fMinimum)
    { return CVarSimpleSetIntNLG(fOrthoMinimum,
        fMinimum, 1.0f, fOrthoMaximum); }
  /* -- Set maximum orthagonal matrix ratio -------------------------------- */
  CVarReturn SetMaxOrtho(const GLfloat fMaximum)
    { return CVarSimpleSetIntNLG(fOrthoMaximum,
        fMaximum, fOrthoMinimum, 2.0f); }
  /* -- Set viewport lock -------------------------------------------------- */
  CVarReturn SetLockViewport(const bool bState)
    { return CVarSimpleSetInt(bLockViewport, bState); }
  /* -- Set simple matrix -------------------------------------------------- */
  CVarReturn SetSimpleMatrix(const bool bState)
    { return CVarSimpleSetInt(bSimpleMatrix, bState); }
  /* -- Initialise fbos using a different constructor ---------------------- */
  FboCore(void) :
    /* -- Initialisers ----------------------------------------------------- */
    fOrthoMinimum(1.0f),               fOrthoMaximum(2.0f),
    fOrthoWidth(0.0f),                 fOrthoHeight(0.0f),
    bDraw(false),                      bSimpleMatrix(false),
    bLockViewport(false),              bClearBuffer(false),
    fboConsole{ GL_RGBA8, true },      fboMain{ GL_RGB8, true },
    dRTFPS(0)
    /* -- Set pointer to main fbo ------------------------------------------ */
    { cFbos->fboMain = &fboMain; }
  /* -- Destructor --------------------------------------------------------- */
  DTORHELPER(~FboCore, DestroyAllObjectsAndBuiltIns())
  /* -- FboCore::End ------------------------------------------------------- */
} *cFboCore = nullptr;                 // Pointer to static class
/* ------------------------------------------------------------------------- */
}                                      // End of public module namespace
/* ------------------------------------------------------------------------- */
}                                      // End of private module namespace
/* == EoF =========================================================== EoF == */

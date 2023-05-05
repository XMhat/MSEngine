/* == FBOMAIN.HPP ========================================================== */
/* ######################################################################### */
/* ## MS-ENGINE              Copyright (c) MS-Design, All Rights Reserved ## */
/* ######################################################################### */
/* ## Contains the main drawing fbo's and commands. It would be nice to   ## */
/* ## have this in the FBOs collector but we can't due to C++ limitations ## */
/* ## of initialising static FBO classes when it hasn't been defined yet. ## */
/* ######################################################################### */
/* ========================================================================= */
#pragma once                           // Only one incursion allowed
/* ------------------------------------------------------------------------- */
namespace IfFboMain {                  // Start of module namespace
/* -- Includes ------------------------------------------------------------- */
using namespace IfFbo;                 // Using fbo namespace
/* == Main fbo class ======================================================= */
static class FboMain final :           // The main fbo operations manager
  /* -- Base classes ------------------------------------------------------- */
  public FboColour,                    // Backbuffer clear colour
  public FboBlend,                     // Default blending mode
  public FboViewport                   // Viewport co-ordinates
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
  /* -- Set backbuffer viewport by width and height ------------------------ */
  void SetViewportWH(const GLsizei stW, const GLsizei stH)
    { SetCoLeft(0); SetCoTop(0); SetCoRight(stW); SetCoBottom(stH); }
  /* -- Set backbuffer viewport -------------------------------------------- */
  void SetViewport(const GLint iL, const GLint iT,
                   const GLsizei stW, const GLsizei stH)
    { SetCoLeft(iL); SetCoTop(iT); SetCoRight(stW); SetCoBottom(stH); }
  /* -- Reset backbuffer clear colour to colour stored in cvar ------------- */
  void ResetClearColour(void)
  { // Set main backbuffer colour
    fboMain.ResetClearColour();
    fboConsole.ResetClearColour();
    // Commit the default backbuffer clear colour
    cOgl->SetClearColourInt(
      cCVars->GetInternalSafe<unsigned int>(VID_CLEARCOLOUR));
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
    cOgl->UseProgram(cFboBase->sh3D.GetProgram());
    // Set the viewport of the FBO size
    cOgl->SetViewport(*this);
    // Set the default alpha blending mode
    cOgl->SetBlendIfChanged(*this);
    // Clear back buffer if main fbo has alpha
    if(fboMain.IsTransparencyEnabled()) cOgl->SetAndClear(*this);
    // Buffer the interlaced triangle data
    cOgl->BufferStaticData(fboMain.GetDataSize(), fboMain.GetData());
    // Specify format of the interlaced triangle data
    cOgl->VertexAttribPointer(A_COORD, stCompsPerCoord, 0,
      fboMain.GetTCIndex());
    cOgl->VertexAttribPointer(A_VERTEX, stCompsPerPos, 0,
      fboMain.GetVIndex());
    cOgl->VertexAttribPointer(A_COLOUR, stCompsPerColour, 0,
      fboMain.GetCIndex());
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
  void BlitConsoleToMain(void) { fboMain.Blit(fboConsole); }
  /* -- Finish main fbo and add it to render list -------------------------- */
  void FinishMain(void) { fboMain.FinishAndRender(); }
  /* -- Set main fbo as active fbo to draw too ----------------------------- */
  void ActivateMain(void) { fboMain.SetActive(); }
  /* -- Called from main tick incase we need to keep catching up frames ---- */
  void RenderFbosAndFlushMain(void)
  { // Render all the user queud fbos and flush them
    FboRender();
    // Flush main fbo
    fboMain.Flush();
  }
  /* -- De-initialise all fbos --------------------------------------------- */
  void DeInitAllObjectsAndBuiltIns(void)
  { // Temporary de-init all user objects
    FboDeInit();
    // Temporary de-init the console and main static fbo classes too
    fboConsole.DeInit();
    fboMain.DeInit();
  }
  /* -- Destroy all fbo's -------------------------------------------------- */
  void DestroyAllObjectsAndBuiltIns(void)
  { // Destroy all the fbos. Note this doesn't destroy the static fbo's as they
    // were unregistered in the init function below.
    cFbos->CollectorDestroyUnsafe();
    // Deinit the console and main static fbo classes
    fboConsole.DeInit();
    fboMain.DeInit();
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
      fAspect = Clamp(fWidth / fHeight, fOrthoMinimum, fOrthoMaximum);
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
      fAspect = Clamp(static_cast<GLfloat>(GetCoRight()) /
                      static_cast<GLfloat>(GetCoBottom()),
                      fOrthoMinimum, fOrthoMaximum) / 1.333333f;
      // For some unknown reason we could be sent invalid values so we need to
      // make sure we ignore this value to prevent error handlers triggering.
      if(fAspect != fAspect) fAspect = 1.0f;
      // Calculate additional width over the 4:3 aspect ratio
      fAddWidth = Maximum(((fWidth * fAspect) - fWidth) / 2.0f, 0.0f);
      // Calculate bounds for stage
      fLeft = floorf(-fAddWidth);
      fRight = floorf(fWidth + fAddWidth);
      // Set top and bottom stage bounds
      fTop = 0.0f;
      fBottom = fHeight;
    } // If the viewport didn't change?
    if(IsFloatEqual(fLeft, fboMain.fcStage.GetCoLeft()) &&
       IsFloatEqual(fTop, fboMain.fcStage.GetCoTop()) &&
       IsFloatEqual(fRight, fboMain.fcStage.GetCoRight()) &&
       IsFloatEqual(fBottom, fboMain.fcStage.GetCoBottom()))
    { // Return if we're not forcing the change
      if(!bForce) return false;
    } // Viewport changed?
    else
    { // Save requested ortho size incase viewport changes
      fOrthoWidth = fWidth;
      fOrthoHeight = fHeight;
      // Set stage bounds for drawing
      fboMain.SetOrtho(fLeft, fTop, fRight, fBottom);
    } // Calculate new fbo width and height
    const GLsizei stFBOWidth = static_cast<GLsizei>(fRight - fLeft),
                  stFBOHeight = static_cast<GLsizei>(fBottom - fTop);
    // No point changing anything if the bounds are the same and if the fbo
    // needs updating? Also ignore if opengl isn't initialised as the GLfW FB
    // reset window event might be sent before we've initialised it!
    if((stFBOWidth != static_cast<GLsizei>(fboMain.GetCoRight()) ||
        stFBOHeight != static_cast<GLsizei>(fboMain.GetCoBottom()) ||
        bForce) && cOgl->IsGLInitialised())
    { // Re-initialise the main framebuffer
      fboMain.Init("main", stFBOWidth, stFBOHeight);
      // Log computations
      cLog->LogDebugExSafe("Fbo main matrix reinitialised as $x$ [$] "
        "(D=$x$,A=$<$-$>,AW=$,S=$:$:$:$).",
        fboMain.GetCoRight(), fboMain.GetCoBottom(),
        ToRatio(fboMain.GetCoRight(), fboMain.GetCoBottom()),
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
  bool DoAutoViewport(const GLsizei stWidth, const GLsizei stHeight)
  { // Set the viewport
    SetViewportWH(stWidth, stHeight);
    // Log event
    cLog->LogDebugExSafe(
      "Fbo processing automatrix size of $x$ to backbuffer...",
      stWidth, stHeight);
    // Update matrix because the window's aspect ratio may have changed
    const bool bResult = AutoMatrix(GetOrthoWidth(), GetOrthoHeight(), false);
    // Inform lua scripts that they should redraw the framebuffer
    if(bResult) cEvtMain->Add(EMC_LUA_REDRAW);
    // Return result
    return bResult;
  }
  /* -- Sent when the window is resized ------------------------------------ */
  bool AutoViewport(const GLsizei stWidth, const GLsizei stHeight)
  { // If the viewport size changed?
    return GetCoRight() == stWidth && GetCoBottom() == stHeight ?
      // Return nothing changed
      false :
      // Else if the viewport is locked?
      (bLockViewport ?
        // Use current ortho width and height
        DoAutoViewport(static_cast<GLsizei>(GetOrthoWidth()),
                       static_cast<GLsizei>(GetOrthoHeight())) :
        // Do what the user wants instead but clamp the width and height
        DoAutoViewport(Maximum(1, stWidth), Maximum(1, stHeight)));
  }
  /* -- Init console fbo --------------------------------------------------- */
  void InitConsoleFBO(void)
  { // Initialise the console fbo for the console object
    fboConsole.Init("console", fboMain.DimGetWidth(), fboMain.DimGetHeight());
  }
  /* -- Set main fbo float reserve ----------------------------------------- */
  CVarReturn SetFloatReserve(const size_t stCount)
    { return BoolToCVarReturn(fboMain.ReserveTriangles(stCount)); }
  /* -- Set main fbo command reserve --------------------------------------- */
  CVarReturn SetCommandReserve(const size_t stCount)
    { return BoolToCVarReturn(fboMain.ReserveCommands(stCount)); }
  /* -- Set main fbo filters (cvar event) ---------------------------------- */
  CVarReturn SetFilter(const size_t stV)
  { // Check value
    if(stV >= OF_MAX) return DENY;
    // Set filtering of main and console fbo
    fboMain.SetFilter(stV);
    fboConsole.SetFilter(stV);
    // Accept the change anyway if opengl not initialised
    if(cOgl->IsGLNotInitialised()) return ACCEPT;
    // Commit the filters
    fboMain.CommitFilter();
    fboConsole.CommitFilter();
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
  FboMain(void) :
    /* -- Initialisers ----------------------------------------------------- */
    fOrthoMinimum(1.0f),               fOrthoMaximum(2.0f),
    fOrthoWidth(0.0f),                 fOrthoHeight(0.0f),
    bDraw(false),                      bSimpleMatrix(false),
    bLockViewport(false),              bClearBuffer(false),
    fboConsole{ GL_RGBA8 },            fboMain{ GL_RGB8 },
    dRTFPS(0)
    /* -- Set pointer to main fbo ------------------------------------------ */
    { cFbos->fboMain = &fboMain; }
  /* -- Destructor --------------------------------------------------------- */
  DTORHELPER(~FboMain, DestroyAllObjectsAndBuiltIns())
  /* -- FboCore::End ------------------------------------------------------- */
} *cFboMain = nullptr;                    // Pointer to static class
/* ------------------------------------------------------------------------- */
};                                     // End of module namespace
/* == EoF =========================================================== EoF == */

/* == FBOBASE.HPP ========================================================== */
/* ######################################################################### */
/* ## MS-ENGINE              Copyright (c) MS-Design, All Rights Reserved ## */
/* ######################################################################### */
/* ## Variables to help with fbos, rendering threading and context.       ## */
/* ######################################################################### */
#pragma once                           // Only one incursion allowed
/* ------------------------------------------------------------------------- */
namespace IfFboBase {                  // Start of module namespace
/* -- Includes ------------------------------------------------------------- */
using namespace IfShader;              // Using shader namespace
/* == Fbo base class ======================================================= */
static class FboBase final
{ /* -- 3D shader references --------------------------------------- */ public:
  array<Shader,3> sh3DBuiltIns;        // list of built-in 3D shaders
  Shader          &sh3D;               // Basic 3D transformation shader
  Shader          &sh3DYUV;            // 3D YUV transformation shader
  Shader          &sh3DYUVK;           // 3D YUV ckey transformation shader
  /* -- 2D shader references ----------------------------------------------- */
  array<Shader,5> sh2DBuiltIns;        // list of built-in 2D shaders
  Shader          &sh2D;               // 2D-3D transformation shader
  Shader          &sh2DBGR;            // 2D BGR-3D transformation shader
  Shader          &sh2D8;              // 2D LUM-3D transformation shader
  Shader          &sh2D8Pal;           // 2D LUMPAL-3D transformation shader
  Shader          &sh2D16;             // 2D LUMAL-3D transformation shader
  /* -- De-initialise built in shaders ------------------------------------- */
  void DeInitShaders(void)
  { // De-init built-in shaders
    cLog->LogDebugExSafe(
      "FboBase de-initialising $ built-in 3D shader objects...",
      sh3DBuiltIns.size());
    for_each(sh3DBuiltIns.rbegin(), sh3DBuiltIns.rend(),
      [](Shader &shS) { shS.DeInit(); });
    cLog->LogDebugExSafe(
      "FboBase de-initialising $ built-in 2D shader objects...",
      sh2DBuiltIns.size());
    for_each(sh2DBuiltIns.rbegin(), sh2DBuiltIns.rend(),
      [](Shader &shS) { shS.DeInit(); });
    cLog->LogInfoExSafe("FboBase de-initialised $ built-in shader objects.",
      sh3DBuiltIns.size() + sh2DBuiltIns.size());
  }
  /* -- Initialise built-in shaders ---------------------------------------- */
  void InitShaders(void)
  { // Setup 3D shaders
    cLog->LogDebugExSafe(
      "FboBase initialising $ built-in 3D shader objects...",
      sh3DBuiltIns.size());
    Init3DShader(sh3D);
    Init3DYUVShader(sh3DYUV);
    Init3DYUVKShader(sh3DYUVK);
    cLog->LogDebugExSafe(
      "FboBase initialising $ built-in 2D shader objects...",
      sh2DBuiltIns.size());
    // Setup 2D shaders
    Init2DShader(sh2D);
    Init2DBGRShader(sh2DBGR);
    Init2D8Shader(sh2D8);
    Init2D8PalShader(sh2D8Pal);
    Init2D16Shader(sh2D16);
    cLog->LogInfoExSafe("FboBase initialised $ built-in shader objects.",
      sh3DBuiltIns.size() + sh2DBuiltIns.size());
  }
  /* ----------------------------------------------------------------------- */
  FboBase(void) :                      // No parameters
    /* -- Initialisers ----------------------------------------------------- */
    sh3D{ sh3DBuiltIns[0] },           sh3DYUV{ sh3DBuiltIns[1] },
    sh3DYUVK{ sh3DBuiltIns[2] },       sh2D{ sh2DBuiltIns[0] },
    sh2DBGR{ sh2DBuiltIns[1] },        sh2D8{ sh2DBuiltIns[2] },
    sh2D8Pal{ sh2DBuiltIns[3] },       sh2D16{ sh2DBuiltIns[4] }
    /* -- Code ------------------------------------------------------------- */
    { }                                // No code
  /* ----------------------------------------------------------------------- */
  DELETECOPYCTORS(FboBase)             // No copy constructor
  /* ----------------------------------------------------------------------- */
} *cFboBase = nullptr;                 // Global access
/* ------------------------------------------------------------------------- */
};                                     // End of module namespace
/* == EoF =========================================================== EoF == */

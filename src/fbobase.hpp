/* == FBOBASE.HPP ========================================================== */
/* ######################################################################### */
/* ## MS-ENGINE              Copyright (c) MS-Design, All Rights Reserved ## */
/* ######################################################################### */
/* ## Variables to help with fbos, rendering threading and context.       ## */
/* ######################################################################### */
#pragma once                           // Only one incursion allowed
/* -- Module namespace ----------------------------------------------------- */
namespace IfFboBase {                  // Keep declarations neatly categorised
/* -- Includes ------------------------------------------------------------- */
using namespace IfShader;              // Using shader interface
/* == Fbo base class ======================================================= */
static class FboBase
{ /* -- 3D shader references --------------------------------------- */ public:
  array<Shader,3> sh3DBuiltIns;        // list of built-in 3D shaders
  Shader          &sh3D;               // Basic 3D transformation shader
  Shader          &sh3DYUV;            // 3D YUV transformation shader
  Shader          &sh3DYUVK;           // 3D YUV ckey transformation shader
  /* -- 2D shader references ----------------------------------------------- */
  array<Shader,4> sh2DBuiltIns;        // list of built-in 2D shaders
  Shader          &sh2D;               // 2D-3D transformation shader
  Shader          &sh2DBGR;            // 2D BGR-3D transformation shader
  Shader          &sh2D8;              // 2D LUM-3D transformation shader
  Shader          &sh2D16;             // 2D LUMAL-3D transformation shader
  /* -- De-initialise built in shaders ------------------------------------- */
  void DeInitShaders(void)
  { // De-init built-in shaders
    LW(LH_DEBUG, "FboBase de-initialising $ built-in 3D shader objects...",
      sh3DBuiltIns.size());
    for_each(sh3DBuiltIns.rbegin(), sh3DBuiltIns.rend(),
      [](Shader &shS) { shS.DeInit(); });
    LW(LH_DEBUG, "FboBase de-initialising $ built-in 2D shader objects...",
      sh2DBuiltIns.size());
    for_each(sh2DBuiltIns.rbegin(), sh2DBuiltIns.rend(),
      [](Shader &shS) { shS.DeInit(); });
    LW(LH_INFO, "FboBase de-initialised $ built-in shader objects.",
      sh3DBuiltIns.size() + sh2DBuiltIns.size());
  }
  /* -- Initialise built-in shaders ---------------------------------------- */
  void InitShaders(void)
  { // Load built-in shaders
    LW(LH_DEBUG, "FboBase initialising $ built-in 3D shader objects...",
      sh3DBuiltIns.size());
    Init3DShader(sh3D);
    Init3DYUVShader(sh3DYUV);
    Init3DYUVKShader(sh3DYUVK);
    LW(LH_DEBUG, "FboBase initialising $ built-in 2D shader objects...",
      sh2DBuiltIns.size());
    Init2DShader(sh2D);
    Init2DBGRShader(sh2DBGR);
    Init2D8Shader(sh2D8);
    Init2D16Shader(sh2D16);
    LW(LH_INFO, "FboBase initialised $ built-in shader objects.",
      sh3DBuiltIns.size() + sh2DBuiltIns.size());
    }
  /* ----------------------------------------------------------------------- */
  FboBase(void) :
    /* --------------------------------------------------------------------- */
    sh3D{ sh3DBuiltIns[0] },           sh3DYUV{ sh3DBuiltIns[1] },
    sh3DYUVK{ sh3DBuiltIns[2] },       sh2D{ sh2DBuiltIns[0] },
    sh2DBGR{ sh2DBuiltIns[1] },        sh2D8{ sh2DBuiltIns[2] },
    sh2D16{ sh2DBuiltIns[3] }
    /* --------------------------------------------------------------------- */
    { }
  /* ----------------------------------------------------------------------- */
  DELETECOPYCTORS(FboBase);            // No copy constructor
  /* ----------------------------------------------------------------------- */
} *cFboBase = nullptr;                 // Global access
/* -- End of module namespace ---------------------------------------------- */
};                                     // End of interface
/* == EoF =========================================================== EoF == */

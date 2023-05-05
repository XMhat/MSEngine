/* == FBO.HPP ============================================================== */
/* ######################################################################### */
/* ## MS-ENGINE              Copyright (c) MS-Design, All Rights Reserved ## */
/* ######################################################################### */
/* ## This the file handles rendering of 2d fbos and triangles in opengl  ## */
/* ##                                                                     ## */
/* ## TODO: Add custom shaders and expose the functions to LUA. Probably  ## */
/* ##       only allow them to be attached to custom FBO's so the guest   ## */
/* ##       can't override the built in shaders.                          ## */
/* ######################################################################### */
#pragma once                           // Only one incursion allowed
/* ------------------------------------------------------------------------- */
namespace IfFbo {                      // Start of module namespace
/* -- Includes ------------------------------------------------------------- */
using namespace IfFboBase;             // Using fbobase namespace
/* == Fbo collector class for collector data and custom variables ========== */
BEGIN_COLLECTOREX2(Fbos, Fbo, CLHelperUnsafe, // Build 'Fbos' collector/child
/* -- Fbo collector variables ---------------------------------------------- */
struct OrderItem :                     /* Order item structure              */\
  /* -- Base classes ------------------------------------------------------- */
  public FboRenderItem                 /* Order structure                   */\
{ /* -- Variables --------------------------------------------------------- */\
  Fbo             *fboDest;            /* Reference to fbo to draw to       */\
  GLsizei          stVertices;         /* No. of vertices in fbo gtlData.   */\
  ssize_t          stCommands;         /* No. of commands in fbo gclData.   */\
  /* -- Init constructor --------------------------------------------------- */
  OrderItem(const FboRenderItem &friOther, Fbo*const fboNDest,
    const GLsizei stNVertices, const ssize_t stNCommands) : \
    /* -- Initialisers ----------------------------------------------------- */
    FboRenderItem{ friOther },         fboDest(fboNDest), \
    stVertices(stNVertices),           stCommands(stNCommands) \
    /* -- No code ---------------------------------------------------------- */
    { }
}; /* --------------------------------------------------------------------- */\
Fbo               *fboActive;          /* Pointer to active FBO object      */\
Fbo               *fboMain;            /* Pointer to main FBO object        */\
/* -- Fbo order list ------------------------------------------------------- */
typedef vector<OrderItem> OrderVec;    /* Vector of Fbo render details      */\
OrderVec           ovActive;           /* Active fbo order list to render   */\
);/* ----------------------------------------------------------------------- */
typedef Fbos::OrderItem                FboOrderItem;  // Fbo order item
typedef Fbos::OrderVec                 FboOrderVec;   // " vector for items
typedef Fbos::OrderVec::const_iterator FboOrderVecIt; // "   "    iterator
/* == Fbo variables class ================================================== */
class FboVariables :                   // Fbo variables class
  /* ----------------------------------------------------------------------- */
  // Only put vars used in the Fbo class in here. This is an optimisation so
  // we do not have to initialise all these variables more than once as we have
  // more than one constructor in the main Fbo class.
  /* -- Base classes ------------------------------------------------------- */
  public FboItem,                      // Fbo item class
  public FboRenderItem                 // Render data
{ /* -- Texture ---------------------------------------------------- */ public:
  size_t           stFilterId;         // Chosen filter value
  GLint            iMinFilter,         // Frame buffer minification filter
                   iMagFilter,         // Frame buffer magnification filter
                   iWrapMode,          // Frame buffer wrapping mode
                   iPixFormat;         // Frame buffer pixel format
  /* ----------------------------------------------------------------------- */
  FboTriList       ftlActive;          // Triangles list
  FboCmdList       fclActive;          // Commands list
  /* -- Buffers ------------------------------------------------------------ */
  size_t           stGLArrayOff;       // Current rendering offset
  GLuint           uiTextureCache;     // Last GL texture id used
  GLuint           uiTexUnitCache;     // Last GL multi-texture unit id used
  GLuint           uiShaderCache;      // Shader currently selected
  size_t           stTrianglesLast;    // Triangles before last cache change
  size_t           stTrianglesFrame;   // Triangles this frame
  size_t           stCommandsFrame;    // Commands this frame
  size_t           stRenderCounter;    // Times rendered
  /* -- Variables ---------------------------------------------------------- */
  GLuint           uiFBOtex;           // Frame buffer texture name
  FboFloatCoords   fcStage;            // Stage co-ordinates
  /* -- Constructor -------------------------------------------------------- */
  explicit FboVariables(const GLint iPF) :
    /* -- Initialisers ----------------------------------------------------- */
    stFilterId(0),                     iMinFilter(GL_NEAREST),
    iMagFilter(GL_NEAREST),            iWrapMode(GL_CLAMP_TO_EDGE),
    iPixFormat(iPF),                   stGLArrayOff(0),
    uiTextureCache(0),                 uiTexUnitCache(0),
    uiShaderCache(0),                  stTrianglesLast(0),
    stTrianglesFrame(0),               stCommandsFrame(0),
    stRenderCounter(0),                uiFBOtex(0)
    /* --------------------------------------------------------------------- */
    { }
  /* ----------------------------------------------------------------------- */
  DELETECOPYCTORS(FboVariables)        // No copy constructors
};/* ----------------------------------------------------------------------- */
/* == Fbo object class ===================================================== */
BEGIN_MEMBERCLASS(Fbos, Fbo, ICHelperUnsafe),
  /* -- Base classes ------------------------------------------------------- */
  public Lockable,                     // Lua lockable class
  public Ident,                        // Identifier class
  public FboVariables                  // Fbo variables
{ /* -- Select texture --------------------------------------------- */ public:
  void BindTexture(void)
    { GL(cOgl->BindTexture(uiFBOtex),
        "Failed to select FBO texture as active!",
        "Identifier", IdentGet(), "Texture", uiFBOtex); }
  /* -- (Un)bind framebuffer ----------------------------------------------- */
  void Bind(void)
    { GL(cOgl->BindFBO(uiFBO), "Failed to select active FBO!",
        "Identifier", IdentGet(), "FBO", uiFBO); }
  /* -- Bind buffer and texture -------------------------------------------- */
  void BindAndTexture(void) { Bind(); BindTexture(); }
  /* -- Set FBO transparency (must be called before initialisation) -------- */
  void SetTransparency(const bool bState)
    { iPixFormat = bState ? GL_RGBA8 : GL_RGB8; }
  bool IsTransparencyEnabled(void) const { return iPixFormat == GL_RGBA8; }
  /* -- Flush the vertex buffer and queue ---------------------------------- */
  void ClearLists(void) { ftlActive.clear(); fclActive.clear(); }
  /* -- Flush queue -------------------------------------------------------- */
  void Flush(void)
  { // Flush the vertex buffer and queue
    ClearLists();
    // Reset counters and caches
    uiTextureCache = uiTexUnitCache = uiShaderCache = 0;
    stTrianglesLast = stGLArrayOff = 0;
  }
  /* -- Force a finish and reset ------------------------------------------- */
  void ResetCache(const GLuint uiT, const GLuint uiTU, const GLuint uiSC)
  { // Update texture cache id, shader class, and texture unit id
    uiTextureCache = uiT;
    uiShaderCache = uiSC;
    uiTexUnitCache = uiTU;
    // Reset triangles in current triangle count
    stTrianglesLast = GetTrisNow();
    // Update current offset of buffer for next finish command
    stGLArrayOff = stTrianglesLast * sizeof(FboTri);
  }
  /* -- Set main fbo command reserve --------------------------------------- */
  bool ReserveCommands(const size_t stCount)
    { return ReserveList(fclActive, stCount); }
  /* -- Set main fbo float reserve ----------------------------------------- */
  bool ReserveTriangles(const size_t stCount)
    { return ReserveList(ftlActive, stCount); }
  /* -- Return number of commands parsed last frame ---------------- */ public:
  size_t GetCmdsNow(void) const { return fclActive.size(); }
  size_t GetCmds(void) const { return stCommandsFrame; }
  size_t GetCmdsReserved(void) const { return fclActive.capacity(); }
  /* -- Return number of triangles parsed last frame ----------------------- */
  size_t GetTrisCmd(void) const { return GetTrisNow() - stTrianglesLast; }
  size_t GetTrisNow(void) const { return ftlActive.size(); }
  size_t GetTris(void) const { return stTrianglesFrame; }
  size_t GetTrisReserved(void) const { return ftlActive.capacity(); }
  /* -- Activate this fbo -------------------------------------------------- */
  void SetActive(void) { cParent.fboActive = this; }
  /* -- Perform rendering inside the FBO to the texture -------------------- */
  void Render(const FboOrderItem &oiRef)
  { // Select our FBO as render target
    cOgl->BindFBO(oiRef.uiFBO);
    // Set the viewport of the framebuffer to the total visible pixels
    cOgl->SetViewportWH(oiRef.DimGetWidth(), oiRef.DimGetHeight());
    // Set blending mode
    cOgl->SetBlendIfChanged(oiRef);
    // Clear the fbo if requested
    if(oiRef.bClear) cOgl->SetAndClear(oiRef);
    // No point in continuing if there are no vertices
    if(!oiRef.stVertices) return;
    // For each 2D shader...
    for(Shader &shBuiltIn : cFboBase->sh2DBuiltIns)
      shBuiltIn.UpdateOrtho(oiRef);
    // Buffer the new vertex data
    cOgl->BufferStaticData(oiRef.stVertices, ftlActive.data());
    // For each command in this order
    for(auto fclIt{ fclActive.cbegin() },
             fclItEnd{ next(fclIt, oiRef.stCommands) };
             fclIt < fclItEnd;
           ++fclIt)
    { // Get command data
      const FboCmd &fcData = *fclIt;
      // Set texture, texture unit and shader program
      cOgl->ActiveTexture(fcData.uiTUId);
      cOgl->BindTexture(fcData.uiTexId);
      cOgl->UseProgram(fcData.uiPrgId);
      // Prepare data arrays
      cOgl->VertexAttribPointer(A_COORD,
        stCompsPerCoord, stBytesPerVertex, fcData.vpTCOffset);
      cOgl->VertexAttribPointer(A_VERTEX,
        stCompsPerPos, stBytesPerVertex, fcData.vpVOffset);
      cOgl->VertexAttribPointer(A_COLOUR,
        stCompsPerColour, stBytesPerVertex, fcData.vpCOffset);
      // Blit array
      cOgl->DrawArraysTriangles(fcData.uiVertices);
    }
  }
  /* -- Render and flush if the last reference ----------------------------- */
  void RenderAndFlush(const FboOrderItem &oiRef)
  { // Render the fbo
    Render(oiRef);
    // Flush lists if reference is zero
    if(!--stRenderCounter) Flush();
  }
  /* -- Finished with drawing in the FBO ----------------------------------- */
  void FinishQueue(void)
  { // Push the data we need to render the array
    fclActive.push_back({
      uiTexUnitCache,                                            // uiTUId
      uiTextureCache,                                            // uiTexId
      uiShaderCache,                                             // uiPrgId
      reinterpret_cast<GLvoid*>(stGLArrayOff + stOffsetTxcData), // vpTCOffset
      reinterpret_cast<GLvoid*>(stGLArrayOff + stOffsetPosData), // vpVOffset
      reinterpret_cast<GLvoid*>(stGLArrayOff + stOffsetColData), // vpCOffset
      static_cast<GLsizei>(GetTrisCmd() * stVertexPerTriangle),  // uiVertices
    });
  }
  /* -- Finish and render the graphics ------------------------------------- */
  void FinishAndRender(void)
  { // Finish writing to the arrays
    FinishQueue();
    // Set current triangle and frame count
    stTrianglesFrame = GetTrisNow();
    stCommandsFrame = GetCmdsNow();
    // Add current count to fbo rendering queue
    cParent.ovActive.push_back({ *this, this,
      static_cast<GLsizei>(stTrianglesFrame * sizeof(FboTri)),
      IntOrMax<ssize_t>(stCommandsFrame) });
    // Incrememnt number of times this fbo is referenced in the active list,
    // this is so when the reference counter is reduced the zero, the triangles
    // and command lists are permitted to clear on FboRender().
    ++stRenderCounter;
  }
  /* -- Finish the queue without checking and reset cache ------------------ */
  void FinishAndReset(const GLuint uiT, const GLuint uiTU, const GLuint uiSC)
  { // If we have triangles? check see if we finished using this texture name
    FinishQueue();
    // Reset the cache
    ResetCache(uiT, uiTU, uiSC);
  }
  /* -- Check caches and start a new command if ids changed ---------------- */
  void CheckCache(const GLuint uiT, const GLuint uiTU, const GLuint uiSC)
  { // If...
    if(uiT != uiTextureCache ||        // ...texture id not changed -or-
       uiSC != uiShaderCache ||        // ...shader not changed -or-
       uiTU != uiTexUnitCache)         // ...texture unit not changed
      // Finish and reset cache
      FinishAndReset(uiT, uiTU, uiSC);
  }
  /* -- Blit the specified texture into the FBO ---------------------------- */
  void Blit(const GLuint uiTex, const TriPosData &fV, const TriCoordData &fTC,
    const TriColData &fC, const GLuint uiTexU, Shader*const shProgram)
  { // If this is the first triangle in this command, we just init the cache
    if(stTrianglesLast == GetTrisNow())
      ResetCache(uiTex, uiTexU, shProgram->GetProgram());
    // Check if texture id/unit or program changed and finish previous list
    else CheckCache(uiTex, uiTexU, shProgram->GetProgram());
    // Structure to insert into our contiguous array buffer. Note that this
    // method produces less assembler output instead of extending the vector
    // and copying the data into the last element and removing push_back.
    // Add completed triangle to list of arrays
    ftlActive.push_back({{
      // <--TexCoord---> <-Vertex (2D)-> <-------Colour (RGBA)------->
      {{ fTC[0],fTC[1] },{ fV[0],fV[1] },{ fC[0],fC[1],fC[ 2],fC[ 3] }}, // P1
      {{ fTC[2],fTC[3] },{ fV[2],fV[3] },{ fC[4],fC[5],fC[ 6],fC[ 7] }}, // P2
      {{ fTC[4],fTC[5] },{ fV[4],fV[5] },{ fC[8],fC[9],fC[10],fC[11] }}  // P3
      // <--TexCoord---> <-Vertex (2D)-> <-------Colour (RGBA)------->
    }});
  }
  /* -- Blit the specified triangle of the specifed fbo to this fbo -------- */
  void BlitTri(Fbo &fboSrc, const size_t stId)
    { Blit(fboSrc.uiFBOtex, fboSrc.GetVData(stId), fboSrc.GetTCData(stId),
        fboSrc.GetCData(stId), 0, &cFboBase->sh2D); }
  /* -- Blit the specified fbo texture into this fbo ----------------------- */
  void Blit(Fbo &fboSrc)
    { for(size_t stId = 0; stId < stTrisPerQuad; ++stId)
        BlitTri(fboSrc, stId); }
  /* -- Blit the specified quad into this fbo ------------------------------ */
  void Blit(FboItem &fboiSrc, const GLuint uiTex, const GLuint uiTexU,
    Shader*const shProgram)
      { for(size_t stId = 0; stId < stTrisPerQuad; ++stId)
          Blit(uiTex, fboiSrc.GetVData(stId), fboiSrc.GetTCData(stId),
            fboiSrc.GetCData(stId), uiTexU, shProgram); }
  /* -- Set wrapping mode -------------------------------------------------- */
  void SetWrap(const GLint iM) { iWrapMode = iM; }
  /* -- Commit wrapping mode ----------------------------------------------- */
  void CommitWrap(void)
  { // Select our fbo and texture
    BindAndTexture();
    // Structure of the wrapping types
    struct Procedure { const GLenum eWrap; const char cWrap; };
    typedef array<const Procedure, 3> Procedures;
    // Procedures to perform
    static const Procedures sProcedures{{
      { GL_TEXTURE_WRAP_S, 'S' },
      { GL_TEXTURE_WRAP_T, 'T' },
      { GL_TEXTURE_WRAP_R, 'R' }
    }};
    // Set to repeat and disable filter
    for(const Procedure &pItem : sProcedures)
      GL(cOgl->SetTexParam(pItem.eWrap, iWrapMode),
        "Failed to set texture wrapping mode!",
        "Identifier", IdentGet(), "Mode", iWrapMode, "Type", pItem.cWrap);
  }
  /* -- Set filtering by ID ------------------------------------------------ */
  size_t GetFilter(void) const { return stFilterId; }
  void SetFilter(const size_t stId)
  { // Translate our filter id to min and mag filter
    cOgl->SetFilterById(stId, iMinFilter, iMagFilter);
    // Record filter id
    stFilterId = stId;
  }
  /* -- Commit new filter modes -------------------------------------------- */
  void CommitFilter(void)
  { // Select our fbo and texture
    BindAndTexture();
    // Set filtering
    GL(cOgl->SetTexParam(GL_TEXTURE_MIN_FILTER, iMinFilter),
      "Failed to set minification filter!",
      "Identifier", IdentGet(), "MinFilter", iMinFilter,
      "MagFilter",  iMagFilter);
    GL(cOgl->SetTexParam(GL_TEXTURE_MAG_FILTER, iMagFilter),
      "Failed to set magnification filter!",
      "Identifier", IdentGet(), "MinFilter", iMinFilter,
      "MagFilter",  iMagFilter);
  }
  /* -- Set filtering by ID and commit ------------------------------------- */
  void SetFilterCommit(const size_t stId)
  { // New filters
    SetFilter(stId);
    // Apply the filters
    CommitFilter();
  }
  /* -- Set backbuffer blending mode --------------------------------------- */
  void SetBlend(const size_t stSFactorRGB, const size_t stDFactorRGB,
    const size_t stSFactorA, const size_t stDFactorA)
  { // OpenGL blending flags
    typedef array<const GLenum, OB_MAX> BlendFunctions;
    static const BlendFunctions aBlends
    { // ## RGB & Alpha Blend Factors StringId
      GL_ZERO,                         // 00 (0,0,0)            0      Z
      GL_ONE,                          // 01 (1,1,1)            1      O
      GL_SRC_COLOR,                    // 02 (Rs,Gs,Bs)         As     S_C
      GL_ONE_MINUS_SRC_COLOR,          // 03 (1,1,1)-(Rs,Gs,Bs) 1-As   O_M_S_C
      GL_DST_COLOR,                    // 04 (Rd,Gd,Bd)         Ad     D_C
      GL_ONE_MINUS_DST_COLOR,          // 05 (1,1,1)-(Rd,Gd,Bd) 1-Ad   O_M_D_C
      GL_SRC_ALPHA,                    // 06 (As,As,As)         As     S_A
      GL_ONE_MINUS_SRC_ALPHA,          // 07 (1,1,1)-(As,As,As) 1-As   O_M_S_A
      GL_DST_ALPHA,                    // 08 (Ad,Ad,Ad)         Ad     D_A
      GL_ONE_MINUS_DST_ALPHA,          // 09 (1,1,1)-(Ad,Ad,Ad) 1-Ad   O_M_D_A
      GL_CONSTANT_COLOR,               // 10 (Rc,Gc,Bc)         Ac     C_C
      GL_ONE_MINUS_CONSTANT_COLOR,     // 11 (1,1,1)-(Rc,Gc,Bc) 1-Ac   O_M_C_C
      GL_CONSTANT_ALPHA,               // 12 (Ac,Ac,Ac)         Ac     C_A
      GL_ONE_MINUS_CONSTANT_ALPHA,     // 13 (1,1,1)-(Ac,Ac,Ac) 1-Ac   O_M_C_A
      GL_SRC_ALPHA_SATURATE            // 14 (i,i,i)            1      S_A_S
    };
    // Lookup values and set
    SetSrcRGB(aBlends[stSFactorRGB]);
    SetDstRGB(aBlends[stDFactorRGB]);
    SetSrcAlpha(aBlends[stSFactorA]);
    SetDstAlpha(aBlends[stDFactorA]);
  }
  /* -- Set clear state ---------------------------------------------------- */
  void SetClear(const bool bState) { bClear = bState; }
  bool IsClearEnabled(void) const { return bClear; }
  /* -- Set clear colour for the FBO --------------------------------------- */
  void SetClearColour(const GLfloat fRed, const GLfloat fGreen,
    const GLfloat fBlue, const GLfloat fAlpha)
  { SetColourRed(fRed);   SetColourGreen(fGreen);
    SetColourBlue(fBlue); SetColourAlpha(fAlpha); }
  /* -- Set clear colour for the FBO as integer (console cvar) ------------- */
  void SetClearColourInt(const unsigned int uiC) { SetColourInt(uiC); }
  /* -- Reset clear colour ------------------------------------------------- */
  void ResetClearColour(void) { ResetColour(); }
  /* -- Set ortho for drawing ---------------------------------------------- */
  void SetOrtho(const GLfloat fLeft, const GLfloat fTop,
                const GLfloat fRight, const GLfloat fBottom)
  { // Set new stage bounds
    fcStage.SetCoLeft(fLeft);
    fcStage.SetCoTop(fTop);
    fcStage.SetCoRight(fRight);
    fcStage.SetCoBottom(fBottom);
    // Set ortho top-left co-ordinates
    SetCoLeft(fcStage.GetCoLeft() < 0 ? -fcStage.GetCoLeft() :
                                         fcStage.GetCoLeft());
    SetCoTop(fcStage.GetCoTop() < 0 ? -fcStage.GetCoTop() :
                                       fcStage.GetCoTop());
  }
  /* -- Reserve floats for vertex array ------------------------------------ */
  bool Reserve(const size_t stTri, const size_t stCmd)
    { return ReserveCommands(stCmd) && ReserveTriangles(stTri); }
  /* -- ReInitialise ------------------------------------------------------- */
  void ReInit(void) { Init(IdentGet(), DimGetWidth(), DimGetHeight()); }
  /* -- DeInitialise ------------------------------------------------------- */
  void DeInit(void)
  { // Remove as active fbo if set
    if(cFbos->fboActive == this) cFbos->fboMain->SetActive();;
    // Flush active triangle and command lists
    Flush();
    // Have FBO texture?
    if(uiFBOtex)
    { // Mark texture for deletion (explanation at top)
      cOgl->SetDeleteTexture(uiFBOtex);
      // Log the de-initialised
      cLog->LogDebugExSafe("Fbo '$' at $ removed texture $.",
        IdentGet(), uiFBO, uiFBOtex);
      // Done with this texture handle
      uiFBOtex = 0;
    } // Have FBO?
    if(uiFBO)
    { // Mark fbo for deletion (explanation at top)
      cOgl->SetDeleteFbo(uiFBO);
      // Log the de-initialised
      cLog->LogDebugExSafe("Fbo '$' removed from $.", IdentGet(), uiFBO);
      // Done with this fbo handle
      uiFBO = 0;
    } // Return if fbo is not in the fbo order list
    if(!stRenderCounter) return;
    stRenderCounter = 0;
    // Get reference to order list and return if empty
    FboOrderVec &fpvList = cParent.ovActive;
    if(fpvList.empty()) return;
    // Find the fbo in the order list and erase it if we found it? Erase from
    // list if this is our fbo else try next fbo
    for(FboOrderVecIt fboIt{ fpvList.cbegin() }; fboIt != fpvList.cend();)
      if(fboIt->fboDest == this) fboIt = fpvList.erase(fboIt);
      else ++fboIt;
  }
  /* -- Initialise --------------------------------------------------------- */
  void Init(const string &strID, const GLsizei stW, const GLsizei stH)
  { // De-initialise old FBO first.
    DeInit();
    // Set identifier.
    IdentSet(strID);
    // Say we're initialising the frame buffer.
    cLog->LogDebugExSafe("Fbo initialising a $x$ object '$'...",
      stW, stH, IdentGet());
    // Record dimensions and clamp texture size to maximum supported size.
    DimSet(Minimum(cOgl->MaxTexSize<GLsizei>(), stW),
           Minimum(cOgl->MaxTexSize<GLsizei>(), stH));
    // If dimensions are different we need to tell the user that
    if(DimGetWidth() != stW || DimGetHeight() != stH)
      cLog->LogWarningExSafe(
        "Fbo '$' dimensions exceed renderer limit ($x$ > $^2)!",
        IdentGet(), stW, stH, cOgl->MaxTexSize());
    // Generate framebuffer and throw error if failed.
    GL(cOgl->CreateFBO(&uiFBO), "Failed to create framebuffer!",
      "Identifier", IdentGet(), "Width", stW, "Height", stH);
    // Generate the FBO, bind the FBO, generate the texture for the FBO,
    // commit the filter and wrapping setting for the FBO and verify that
    // the FBO was setup properly. Bind the newly created framebuffer.
    Bind();
    // Generate texture name for FBO.
    GL(cOgl->CreateTexture(&uiFBOtex),
     "Failed to create texture for framebuffer!",
     "Identifier", IdentGet(), "Width",  stW,
     "Height",     stH,        "Buffer", &uiFBOtex);
    // Bind the texture so we can set it up
    BindTexture();
    // nullptr means reserve texture memory but to not copy any data to it
    GL(cOgl->UploadTexture(0, DimGetWidth(), DimGetHeight(), iPixFormat,
      GL_BGR, nullptr),
        "Failed to allocate video memory for framebuffer texture!",
        "Identifier",  IdentGet(),    "Texture", uiFBOtex,
        "Width",       DimGetWidth(), "Height",  DimGetHeight(),
        "PixelFormat", iPixFormat);
    // Attach 2D texture to this FBO
    GL(cOgl->AttachTexFBO(uiFBOtex),
      "Failed to attach texture to framebuffer!",
      "Identifier", IdentGet(), "Texture", uiFBOtex);
    // Make sure framebuffer generation is complete successfully
    const GLenum uiError = cOgl->VerifyFBO();
    if(uiError != GL_FRAMEBUFFER_COMPLETE)
      XC("Failed to complete framebuffer!",
         "Identifier", IdentGet(), "Error", uiError);
    // Set total requested width and height
    SetCoRight(static_cast<GLfloat>(stW));
    SetCoBottom(static_cast<GLfloat>(stH));
    // Commit the guest requested filter and wrapping texture properties
    CommitFilter();
    CommitWrap();
    // Say we've initialised the frame buffer
    cLog->LogDebugExSafe("Fbo initialised '$' at $ (S=$x$;A=$;T=$).",
      IdentGet(), uiFBO, DimGetWidth(), DimGetHeight(), ToRatio(GetCoRight(),
      GetCoBottom()), uiFBOtex);
  }
  /* -- Constructor -------------------------------------------------------- */
  Fbo(void) :                          // No parameters
    /* -- Initialisers ----------------------------------------------------- */
    ICHelperFbo{ *cFbos, this },       // Initially registered
    IdentCSlave{ cParent.CtrNext() },  // Initialise identification number
    Lockable{ false },                 // Freeable by Lua GC
    FboVariables{ GL_RGBA8 }           // Has alpha channel
    /* -- Code ------------------------------------------------------------- */
    { }                                // Do nothing else
  /* -- Constructor WITHOUT registration (used for core Fbos)--------------- */
  explicit Fbo(const GLint iPF) :      // Pixel format requested
    /* -- Initialisers ----------------------------------------------------- */
    ICHelperFbo{ *cFbos },             // Initially unregistered
    IdentCSlave{ cParent.CtrNext() },  // Initialise identification number
    Lockable{ true },                  // Not freeable by Lua GC
    FboVariables{ iPF }                // Creator chooses if has alpha channel
    /* -- Code ------------------------------------------------------------- */
    { }                                // Do nothing else
  /* -- Destructor --------------------------------------------------------- */
  ~Fbo(void) { DeInit(); }
  /* ----------------------------------------------------------------------- */
  DELETECOPYCTORS(Fbo)                 // Disable copy constructor and operator
};/* ----------------------------------------------------------------------- */
END_COLLECTOREX(Fbos,,,,fboActive(nullptr),fboMain(nullptr))
/* ========================================================================= */
static void FboRender(void)
{ // If there are fbo's in the queue?
  if(!cFbos->ovActive.empty())
  { // For each FBO, reset the texture cache of the fbo if neccesary
    for(const FboOrderItem &oiRef : cFbos->ovActive)
      oiRef.fboDest->RenderAndFlush(oiRef);
    // Clear the fbo order list
    cFbos->ovActive.clear();
  } // Free textures and fbo's marked for deletion
  cOgl->DeleteTexturesAndFboHandles();
}
/* ========================================================================= */
static void FboReInit(void)
{ // Ignore if no fbos
  if(cFbos->empty()) return;
  // Re-init all fbos and log pre/post operation.
  cLog->LogDebugExSafe("Fbos re-initialising $ objects...", cFbos->size());
  for(Fbo*const fCptr : *cFbos) fCptr->ReInit();
  cLog->LogInfoExSafe("Fbos re-initialised $ objects.", cFbos->size());
}
/* ========================================================================= */
static void FboDeInit(void)
{ // Ignore if no fbos
  if(cFbos->empty()) return;
  // De-init all fbos (NOT destroy them!) and log pre/post operation.
  cLog->LogDebugExSafe("Fbos de-initialising $ objects...", cFbos->size());
  for(Fbo*const fCptr : *cFbos) fCptr->DeInit();
  cLog->LogInfoExSafe("Fbos de-initialised $ objects.", cFbos->size());
}
/* -- Set fbo render order reserve ----------------------------------------- */
static CVarReturn FboSetOrderReserve(const size_t stCount)
  { return BoolToCVarReturn(ReserveList(cFbos->ovActive, stCount)); }
/* -- Get active FBO ------------------------------------------------------- */
static Fbo *FboActive(void) { return cFbos->fboActive; }
/* ------------------------------------------------------------------------- */
};                                     // End of module namespace
/* == EoF =========================================================== EoF == */

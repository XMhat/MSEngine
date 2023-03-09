/* == OGL.HPP ============================================================== */
/* ######################################################################### */
/* ## MS-ENGINE              Copyright (c) MS-Design, All Rights Reserved ## */
/* ######################################################################### */
/* ## Allows loading and execution of OpenGL functions.                   ## */
/* ######################################################################### */
/* ========================================================================= */
#pragma once                           // Only one incursion allowed
/* -- Module namespace ----------------------------------------------------- */
namespace IfOgl {                      // Keep declarations neatly categorised
/* -- Includes ------------------------------------------------------------- */
using namespace IfCollector;           // Using collector interface
using namespace IfFbo;                 // Using fbo interface
using namespace IfGlFW;                // Using glfw interface
using namespace IfCVar;                // Using cvar interface
/* -- Types ---------------------------------------------------------------- */
typedef vector<GLuint>            GLUIntVector;   // vector of GLuints
typedef StrVector::const_iterator GLUIntVectorIt; // " const iterator
typedef vector<GLint>             GLIntVector;    // vector of GLints
/* -- Macros --------------------------------------------------------------- */
#define GLLEX(ECF,EF,F,M,...) \
  { F; const GLenum glError = EF(); \
    if(glError != GL_NO_ERROR) \
      LW(LH_WARNING, "GL call failed: " M " ($/$$).", ## __VA_ARGS__, \
        ECF(glError), hex, glError); }
#define GLEX(ECF,EF,F,M,...) \
  { F; const GLenum glError = EF(); \
    if(glError != GL_NO_ERROR) \
      XC("GL call failed: " M, "Code", glError, \
        "Reason", ECF(glError), ## __VA_ARGS__); }
#define GLCEX(ECF,EF,M,...) \
  { const GLenum glError = EF(); \
    if(glError != GL_NO_ERROR) \
      XC("GL call failed: " M, "Code", glError, \
        "Reason", ECF(glError), ## __VA_ARGS__); }
#define GLL(F,M,...) \
  GLLEX(cOgl->GetGLErr, cOgl->sAPI.glGetError, F, M, ## __VA_ARGS__);
#define GL(F,M,...) \
  GLEX(cOgl->GetGLErr, cOgl->sAPI.glGetError, F, M, ## __VA_ARGS__);
#define GLC(M,...) \
  GLCEX(cOgl->GetGLErr, cOgl->sAPI.glGetError, M, ## __VA_ARGS__);
/* -- OpenGL flags --------------------------------------------------------- */
BUILD_FLAGS(Ogl,
  /* ----------------------------------------------------------------------- */
  // No flags                          OpenGL context initialised?
  GFL_NONE               {0x00000000}, GFL_INITIALISED        {0x00000001},
  // Either of the below commands?     Have nVidia memory information?
  GFL_HAVEMEM            {0x00000002}, GFL_HAVENVMEM          {0x00000004},
  // Have ATI memory avail info?       VSync is enabled?
  GFL_HAVEATIMEM         {0x00000008}, GFL_VSYNC              {0x00000010},
  // Devices shares memory with system
  GFL_SHARERAM           {0x00000020},
  /* -- Mask bits ---------------------------------------------------------- */
  GFL_MASK(GFL_INITIALISED|GFL_HAVEMEM|GFL_HAVENVMEM|GFL_HAVEATIMEM|GFL_VSYNC);
);/* ----------------------------------------------------------------------- */
enum OglFilterEnum                     // Available filter combinations
{ /* ----------------------------------------------------------------------- */
  OF_N_N,     OF_N_L,    OF_L_N,      OF_L_L,      OF_NM_MAX,
  OF_N_N_MM_N=OF_NM_MAX, OF_L_N_MM_N, OF_N_N_MM_L, OF_L_N_MM_L,
  OF_N_L_MM_N,           OF_L_L_MM_N, OF_N_L_MM_L, OF_L_L_MM_L,
  OF_MAX,
};/* ----------------------------------------------------------------------- */
enum OglBlendEnum                      // Available blend combinations
{ /* ----------------------------------------------------------------------- */
  OB_Z,   OB_O,       OB_S_C,   OB_O_M_S_C, OB_D_C, OB_O_M_D_C,
  OB_S_A, OB_O_M_S_A, OB_D_A,   OB_O_M_D_A, OB_C_C, OB_O_M_C_C,
  OB_C_A, OB_O_M_C_A, OB_S_A_S, OB_MAX
};
/* -- OpenGL manager class ------------------------------------------------- */
static class Ogl :                     // OGL class for OpenGL use simplicity
  /* -- Sub-classes -------------------------------------------------------- */
  private IHelper,                     // Initialisation helper
  public FboColour,                    // OpenGL colour
  public FboBlend,                     // OpenGL blend
  public FboViewport,                  // OpenGL viewport
  public OglFlags                      // OpenGL init flags
{ /* -- Macros ---------------------------------------------------- */ private:
  DELETECOPYCTORS(Ogl);                // Do not need defaults
  /* -- Defines ------------------------------------------------------------ */
  #define IGLL(F,M,...)  GLLEX(GetGLErr, sAPI.glGetError, F, M, ## __VA_ARGS__)
  #define IGL(F,M,...)   GLEX(GetGLErr, sAPI.glGetError, F, M, ## __VA_ARGS__)
  #define IGLC(M,...)    GLCEX(GetGLErr, sAPI.glGetError, M, ## __VA_ARGS__)
  /* -- Bindings cache --------------------------------------------- */ public:
  GLuint           uiActiveFbo;        // Currently selected FBO name cache
  GLuint           uiActiveProgram;    // Currently active shader program
  GLuint           uiActiveTexture;    // Currently bound texture
  GLuint           uiActiveTUnit;      // Currently active texture unit
  GLuint           uiActiveVao;        // Currently active vertex array object
  GLuint           uiActiveVbo;        // Currently active vertex buffer object
  /* -- Variables ------------------------------------------------- */ private:
  GLuint           uiTexSize;          // Maximum reported texture size
  GLuint           uiPackAlign;        // Default pack alignment
  GLuint           uiUnpackAlign;      // Default Unpack alignment
  GLint            iUnpackRowLength;   // Default unpack row length
  GLuint           uiMaxVertexAttribs; // Maximum vertex attributes per shader
  GLuint           uiTexUnits;         // Texture units count
  GLuint64         qwMinVRAM;          // Minimum VRAM required
  GLuint64         qwTotalVRAM;        // Maximum VRAM supported
  GLuint64         qwFreeVRAM;         // Current VRAM available
  /* -- Information -------------------------------------------------------- */
  string           strRenderer;        // GL renderer string
  string           strVersion;         // GL version string
  string           strVendor;          // GL vendor string
  const IdMap<GLenum> imOGLCodes;      // OpenGL codes
  /* -- Handles ------------------------------------------------------------ */
  GLuint           uiVAO, uiVBO;       // Default vertex array/buffer object
  /* -- Delayed destruction ------------------------------------------------ */
  /* Because LUA garbage collection could zap a texture or fbo class at any  */
  /* time, we need to delay deletion of textures and FBO handles in OpenGL   */
  /* so that a framebuffer can select/draw without binding non-existant      */
  /* handles. Contents will be destroyed after all drawing is completed!     */
  GLUIntVector     vTextures;          // Textures to destroy
  GLUIntVector     vFbos;              // Fbos to destroy
  /* -- OpenGL functions (put in struct to zero easy) -------------- */ public:
  struct OpenGLAPI                     // OpenGL API functions
  { /* -- Callback type -------------- Function name ----------------------- */
    PFNGLACTIVETEXTUREPROC             glActiveTexture;
    PFNGLATTACHSHADERPROC              glAttachShader;
    PFNGLBINDATTRIBLOCATIONPROC        glBindAttribLocation;
    PFNGLBINDBUFFERPROC                glBindBuffer;
    PFNGLBINDFRAMEBUFFERPROC           glBindFramebuffer;
    PFNGLBINDTEXTUREPROC               glBindTexture;
    PFNGLBINDVERTEXARRAYPROC           glBindVertexArray;
    PFNGLBLENDFUNCSEPARATEPROC         glBlendFuncSeparate;
    PFNGLBUFFERDATAPROC                glBufferData;
    PFNGLCHECKFRAMEBUFFERSTATUSPROC    glCheckFramebufferStatus;
    PFNGLCLEARCOLORPROC                glClearColor;
    PFNGLCLEARPROC                     glClear;
    PFNGLCOMPILESHADERPROC             glCompileShader;
    PFNGLCOMPRESSEDTEXIMAGE2DPROC      glCompressedTexImage2D;
    PFNGLCREATEPROGRAMPROC             glCreateProgram;
    PFNGLCREATESHADERPROC              glCreateShader;
    PFNGLDELETEBUFFERSPROC             glDeleteBuffers;
    PFNGLDELETEFRAMEBUFFERSPROC        glDeleteFramebuffers;
    PFNGLDELETEPROGRAMPROC             glDeleteProgram;
    PFNGLDELETESHADERPROC              glDeleteShader;
    PFNGLDELETETEXTURESPROC            glDeleteTextures;
    PFNGLDELETEVERTEXARRAYSPROC        glDeleteVertexArrays;
    PFNGLDETACHSHADERPROC              glDetachShader;
    PFNGLDISABLEPROC                   glDisable;
    PFNGLDISABLEVERTEXATTRIBARRAYPROC  glDisableVertexAttribArray;
    PFNGLDRAWARRAYSPROC                glDrawArrays;
    PFNGLENABLEPROC                    glEnable;
    PFNGLENABLEVERTEXATTRIBARRAYPROC   glEnableVertexAttribArray;
    PFNGLFRAMEBUFFERTEXTURE2DPROC      glFramebufferTexture2D;
    PFNGLGENBUFFERSPROC                glGenBuffers;
    PFNGLGENERATEMIPMAPPROC            glGenerateMipmap;
    PFNGLGENFRAMEBUFFERSPROC           glGenFramebuffers;
    PFNGLGENTEXTURESPROC               glGenTextures;
    PFNGLGENVERTEXARRAYSPROC           glGenVertexArrays;
    PFNGLGETERRORPROC                  glGetError;
    PFNGLGETFLOATVPROC                 glGetFloatv;
    PFNGLGETINTEGERVPROC               glGetIntegerv;
    PFNGLGETPROGRAMINFOLOGPROC         glGetProgramInfoLog;
    PFNGLGETPROGRAMIVPROC              glGetProgramiv;
    PFNGLGETSHADERINFOLOGPROC          glGetShaderInfoLog;
    PFNGLGETSHADERIVPROC               glGetShaderiv;
    PFNGLGETSTRINGIPROC                glGetStringi;
    PFNGLGETSTRINGPROC                 glGetString;
    PFNGLGETTEXIMAGEPROC               glGetTexImage;
    PFNGLGETUNIFORMLOCATIONPROC        glGetUniformLocation;
    PFNGLHINTPROC                      glHint;
    PFNGLISENABLEDPROC                 glIsEnabled;
    PFNGLLINKPROGRAMPROC               glLinkProgram;
    PFNGLPIXELSTOREIPROC               glPixelStorei;
    PFNGLPOLYGONMODEPROC               glPolygonMode;
    PFNGLREADBUFFERPROC                glReadBuffer;
    PFNGLREADPIXELSPROC                glReadPixels;
    PFNGLSHADERSOURCEPROC              glShaderSource;
    PFNGLTEXIMAGE2DPROC                glTexImage2D;
    PFNGLTEXPARAMETERIPROC             glTexParameteri;
    PFNGLTEXSUBIMAGE2DPROC             glTexSubImage2D;
    PFNGLUNIFORM1IPROC                 glUniform1i;
    PFNGLUNIFORM4FPROC                 glUniform4f;
    PFNGLUSEPROGRAMPROC                glUseProgram;
    PFNGLVERTEXATTRIBPOINTERPROC       glVertexAttribPointer;
    PFNGLVIEWPORTPROC                  glViewport;
    /* --------------------------------------------------------------------- */
  } sAPI;                              // API functions list
  /* -- Flag setter ----------------------------------------------- */ private:
  void SetFlagExt(const char*const cpName, const OglFlagsConst &glFlag)
    { FlagSetOrClear(glFlag, HaveExtension(cpName)); }
  /* -- Load GL capabilities ----------------------------------------------- */
  void DetectCapabilities(void)
  { // Current current OpenGL strings
    strVendor = GetString<char>(GL_VENDOR);
    strRenderer = GetString<char>(GL_RENDERER);
    strVersion = GetString<char>(GL_VERSION);
    // Get vendor specific memory info extensions
    SetFlagExt("GL_NVX_gpu_memory_info", GFL_HAVENVMEM);
    SetFlagExt("GL_ATI_meminfo", GFL_HAVEATIMEM);
    // Set flag if have either
    FlagSet(FlagIsSet(GFL_HAVENVMEM) || FlagIsSet(GFL_HAVEATIMEM) ?
      GFL_HAVEMEM : GFL_SHARERAM|GFL_HAVEMEM);
    // Cache maximum texture size (Minimum hardware support for 3.2 is 1024^2)
    uiTexSize = GetInteger<GLuint>(GL_MAX_TEXTURE_SIZE);
    uiMaxVertexAttribs = GetInteger<GLuint>(GL_MAX_VERTEX_ATTRIBS);
    // Cache texture unit count
    uiTexUnits = GetInteger<GLuint>(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS);
    // Cache current default pack alignment size
    uiPackAlign = GetInteger<GLuint>(GL_PACK_ALIGNMENT);
    uiUnpackAlign = GetInteger<GLuint>(GL_UNPACK_ALIGNMENT);
    iUnpackRowLength = GetInteger<GLint>(GL_UNPACK_ROW_LENGTH);
    // Cache current default viewport
    const auto aViewport{ GetIntegerArray<4>(GL_VIEWPORT) };
    SetCoLeft(aViewport[0]);
    SetCoTop(aViewport[1]);
    SetCoRight(static_cast<GLsizei>(aViewport[2]));
    SetCoBottom(static_cast<GLsizei>(aViewport[3]));
    // Load current clear colour
    GetFloatArray<4>(GL_COLOR_CLEAR_VALUE, GetColourMemory());
    // Load current blending settings
    SetSrcRGB(GetInteger<GLenum>(GL_BLEND_SRC_RGB));
    SetDstRGB(GetInteger<GLenum>(GL_BLEND_DST_RGB));
    SetSrcAlpha(GetInteger<GLenum>(GL_BLEND_SRC_ALPHA));
    SetDstAlpha(GetInteger<GLenum>(GL_BLEND_DST_ALPHA));
  }
  /* -- Zero index to hint helper for cvars -------------------------------- */
  static GLenum SHIndexToEnum(const size_t stIndex)
  { // Parameters available to translate
    static const array<const GLenum, 4>
      eCmds{ GL_TRUE, GL_DONT_CARE, GL_FASTEST, GL_NICEST };
    // Return position or invalid
    return stIndex < eCmds.size() ? eCmds[stIndex] : GL_NONE;
  }
  /* -- Load GL extensions ------------------------------------------------- */
  const char *LoadFunctions(void)
  { // Helper macro
    #define GETPTR(v,t) \
      sAPI.v = reinterpret_cast<t>(cGlFW->GetProcAddress(#v)); \
      if(!sAPI.v) return #v;
    // Get basic ARB functions
    GETPTR(glActiveTexture, PFNGLACTIVETEXTUREPROC);
    GETPTR(glBindTexture, PFNGLBINDTEXTUREPROC);
    GETPTR(glBlendFuncSeparate, PFNGLBLENDFUNCSEPARATEPROC);
    GETPTR(glClear, PFNGLCLEARPROC);
    GETPTR(glClearColor, PFNGLCLEARCOLORPROC);
    GETPTR(glCompressedTexImage2D, PFNGLCOMPRESSEDTEXIMAGE2DPROC);
    GETPTR(glDeleteTextures, PFNGLDELETETEXTURESPROC);
    GETPTR(glDisable, PFNGLDISABLEPROC);
    GETPTR(glDrawArrays, PFNGLDRAWARRAYSPROC);
    GETPTR(glEnable, PFNGLENABLEPROC);
    GETPTR(glGenerateMipmap, PFNGLGENERATEMIPMAPPROC);
    GETPTR(glGenTextures, PFNGLGENTEXTURESPROC);
    GETPTR(glGetError, PFNGLGETERRORPROC);
    GETPTR(glGetFloatv, PFNGLGETFLOATVPROC);
    GETPTR(glGetIntegerv, PFNGLGETINTEGERVPROC);
    GETPTR(glGetString, PFNGLGETSTRINGPROC);
    GETPTR(glGetStringi, PFNGLGETSTRINGIPROC);
    GETPTR(glGetTexImage, PFNGLGETTEXIMAGEPROC);
    GETPTR(glHint, PFNGLHINTPROC);
    GETPTR(glIsEnabled, PFNGLISENABLEDPROC);
    GETPTR(glPixelStorei, PFNGLPIXELSTOREIPROC);
    GETPTR(glPolygonMode, PFNGLPOLYGONMODEPROC);
    GETPTR(glReadBuffer, PFNGLREADBUFFERPROC);
    GETPTR(glReadPixels, PFNGLREADPIXELSPROC);
    GETPTR(glTexImage2D, PFNGLTEXIMAGE2DPROC);
    GETPTR(glTexParameteri, PFNGLTEXPARAMETERIPROC);
    GETPTR(glTexSubImage2D, PFNGLTEXSUBIMAGE2DPROC);
    GETPTR(glViewport, PFNGLVIEWPORTPROC);
    // Shader functions
    GETPTR(glAttachShader, PFNGLATTACHSHADERPROC);
    GETPTR(glBindAttribLocation, PFNGLBINDATTRIBLOCATIONPROC);
    GETPTR(glCompileShader, PFNGLCOMPILESHADERPROC);
    GETPTR(glCreateProgram, PFNGLCREATEPROGRAMPROC);
    GETPTR(glCreateShader, PFNGLCREATESHADERPROC);
    GETPTR(glDeleteProgram, PFNGLDELETEPROGRAMPROC);
    GETPTR(glDeleteShader, PFNGLDELETESHADERPROC);
    GETPTR(glDetachShader, PFNGLATTACHSHADERPROC);
    GETPTR(glGetProgramInfoLog, PFNGLGETPROGRAMINFOLOGPROC);
    GETPTR(glGetProgramiv, PFNGLGETPROGRAMIVPROC);
    GETPTR(glGetShaderInfoLog, PFNGLGETSHADERINFOLOGPROC);
    GETPTR(glGetShaderiv, PFNGLGETSHADERIVPROC);
    GETPTR(glGetUniformLocation, PFNGLGETUNIFORMLOCATIONPROC);
    GETPTR(glLinkProgram, PFNGLLINKPROGRAMPROC);
    GETPTR(glShaderSource, PFNGLSHADERSOURCEPROC);
    GETPTR(glUniform1i, PFNGLUNIFORM1IPROC);
    GETPTR(glUniform4f, PFNGLUNIFORM4FPROC);
    GETPTR(glUseProgram, PFNGLUSEPROGRAMPROC);
    // Vertex Buffer Object (VBO) functions
    GETPTR(glBindBuffer, PFNGLBINDBUFFERPROC);
    GETPTR(glBufferData, PFNGLBUFFERDATAPROC);
    GETPTR(glDeleteBuffers, PFNGLDELETEBUFFERSPROC);
    GETPTR(glGenBuffers, PFNGLGENBUFFERSPROC);
    // Vertex Memory Object (VAO) functions
    GETPTR(glBindVertexArray, PFNGLBINDVERTEXARRAYPROC);
    GETPTR(glDeleteVertexArrays, PFNGLDELETEVERTEXARRAYSPROC);
    GETPTR(glDisableVertexAttribArray, PFNGLDISABLEVERTEXATTRIBARRAYPROC);
    GETPTR(glEnableVertexAttribArray, PFNGLENABLEVERTEXATTRIBARRAYPROC);
    GETPTR(glGenVertexArrays, PFNGLGENVERTEXARRAYSPROC);
    GETPTR(glVertexAttribPointer, PFNGLVERTEXATTRIBPOINTERPROC);
    // Frame buffer functions
    GETPTR(glBindFramebuffer, PFNGLBINDFRAMEBUFFERPROC);
    GETPTR(glCheckFramebufferStatus, PFNGLCHECKFRAMEBUFFERSTATUSPROC);
    GETPTR(glDeleteFramebuffers, PFNGLDELETEFRAMEBUFFERSPROC);
    GETPTR(glFramebufferTexture2D, PFNGLFRAMEBUFFERTEXTURE2DPROC);
    GETPTR(glGenFramebuffers, PFNGLGENFRAMEBUFFERSPROC);
    // Done with this
    #undef GETPTR
    // Log functions initialised
    LW(LH_DEBUG, "OGL loaded $ function addresses.",
      sizeof(sAPI) / sizeof(void*));
    // Return success
    return nullptr;
  }
  /* == OpenGL features ============================================ */ public:
  bool HaveExtension(const char*const cpName) const
    { return !!glfwExtensionSupported(cpName); }
  /* ----------------------------------------------------------------------- */
  template<typename RetType=decltype(uiTexSize)>
    RetType MaxTexSize(void) const { return static_cast<RetType>(uiTexSize); }
  /* ----------------------------------------------------------------------- */
  GLuint MaxVertexAttribs(void) const { return uiMaxVertexAttribs; }
  /* ----------------------------------------------------------------------- */
  GLuint PackAlign(void) const { return uiPackAlign; }
  /* ----------------------------------------------------------------------- */
  GLuint UnpackAlign(void) const { return uiUnpackAlign; }
  /* ----------------------------------------------------------------------- */
  GLint UnpackRowLength(void) const { return iUnpackRowLength; }
  /* ----------------------------------------------------------------------- */
  void VertexAttribPointer(const GLuint uiAttrib, const GLint iSize,
    const GLsizei stStride, const GLvoid*const vpBuffer) const
      { sAPI.glVertexAttribPointer(uiAttrib, iSize, GL_FLOAT, GL_FALSE,
          stStride, vpBuffer); }
  /* ----------------------------------------------------------------------- */
  void SetPixelStore(const GLenum eId, const GLint iValue) const
    { sAPI.glPixelStorei(eId, iValue); }
  /* ----------------------------------------------------------------------- */
  void SetPackAlignment(const GLint iValue) const
    { SetPixelStore(GL_PACK_ALIGNMENT, iValue); }
  /* ----------------------------------------------------------------------- */
  void SetUnpackAlignment(const GLint iValue) const
    { SetPixelStore(GL_UNPACK_ALIGNMENT, iValue); }
  /* ----------------------------------------------------------------------- */
  void SetUnpackRowLength(const GLint iValue) const
    { SetPixelStore(GL_UNPACK_ROW_LENGTH, iValue); }
  /* ----------------------------------------------------------------------- */
  void ReadBuffer(const GLenum uiBuffer) const
    { sAPI.glReadBuffer(uiBuffer); }
  /* ----------------------------------------------------------------------- */
  void ReadPixels(const GLint iLeft, const GLint iTop, const GLsizei stWidth,
    const GLsizei stHeight, GLvoid*const vpBuffer) const
      { sAPI.glReadPixels(iLeft, iTop, stWidth, stHeight, GL_RGB,
          GL_UNSIGNED_BYTE, vpBuffer); }
  /* -- Texture functions -------------------------------------------------- */
  void ReadTexture(const GLenum eFormat, GLvoid*const vpBuffer) const
    { sAPI.glGetTexImage(GL_TEXTURE_2D, 0, eFormat, GL_UNSIGNED_BYTE,
        vpBuffer); }
  /* ----------------------------------------------------------------------- */
  void UploadTexture(const GLint iLevel, const GLsizei stWidth,
    const GLsizei stHeight, const GLint iFormat, const GLenum eType,
    const GLvoid*const vpBuffer) const
      { sAPI.glTexImage2D(GL_TEXTURE_2D, iLevel, iFormat, stWidth, stHeight, 0,
          eType, GL_UNSIGNED_BYTE, vpBuffer); }
  /* ----------------------------------------------------------------------- */
  void UploadCompressedTexture(const GLint iLevel, const GLenum eFormat,
    const GLsizei stWidth, const GLsizei stHeight, const GLsizei stSize,
    const GLvoid*const vpBuffer) const
      { sAPI.glCompressedTexImage2D(GL_TEXTURE_2D, iLevel, eFormat, stWidth,
          stHeight, 0, stSize, vpBuffer); }
  /* ----------------------------------------------------------------------- */
  void UploadTextureSub(const GLint iLeft, const GLint iTop,
    const GLsizei stWidth, const GLsizei stHeight, const GLenum ePixFormat,
    const GLvoid*const vpBuffer) const
      { sAPI.glTexSubImage2D(GL_TEXTURE_2D, 0, iLeft, iTop, stWidth, stHeight,
          ePixFormat, GL_UNSIGNED_BYTE, vpBuffer); }
  /* ----------------------------------------------------------------------- */
  void UploadTextureSub(const GLsizei stWidth, const GLsizei stHeight,
    const GLenum ePixFormat, const GLvoid*const vpBuffer) const
      { UploadTextureSub(0, 0, stWidth, stHeight, ePixFormat, vpBuffer); }
  /* ----------------------------------------------------------------------- */
  void GenerateMipmaps(void) const { sAPI.glGenerateMipmap(GL_TEXTURE_2D); }
  /* ----------------------------------------------------------------------- */
  void CreateTextures(const GLsizei stCount, GLuint*const uipTexture) const
    { sAPI.glGenTextures(stCount, uipTexture); }
  /* ----------------------------------------------------------------------- */
  template<class ListType>void CreateTextures(ListType &lIds) const
    { CreateTextures(static_cast<GLsizei>(lIds.size()), lIds.data()); }
  /* ----------------------------------------------------------------------- */
  void CreateTexture(GLuint*const uipTexture) const
    { CreateTextures(1, uipTexture); }
  /* ----------------------------------------------------------------------- */
  GLuint GetCurrentTexture(void) const
    { return GetInteger<GLuint>(GL_TEXTURE_BINDING_2D); }
  /* ----------------------------------------------------------------------- */
  void ActiveTexture(const GLuint uiTexUnit=0)
  { // If we're already the active texture then don't set it
    if(uiActiveTUnit == uiTexUnit) return;
    // Texture unit id lookup table
    static const array<const GLenum,3>
      uiaTexUnit{ GL_TEXTURE0, GL_TEXTURE1, GL_TEXTURE2 };
    // Activate texture
    sAPI.glActiveTexture(uiaTexUnit[uiTexUnit]);
    // Update the selected texture unit
    uiActiveTUnit = uiTexUnit;
  }
  /* ----------------------------------------------------------------------- */
  void DeleteTexture(const GLsizei stCount, const GLuint*const uipTexture)
  { // Check each texture that is about to be deleted
    for(GLsizei stIndex = 0; stIndex < stCount; ++stIndex)
    { // Keep scanning until we find a bound texture
      if(uiActiveTexture != uipTexture[stIndex]) continue;
      // Reset currently bound texture
      uiActiveTexture = 0;
      // No need to check any others
      break;
    } // Delete the textures
    sAPI.glDeleteTextures(stCount, uipTexture);
  }
  /* ----------------------------------------------------------------------- */
  void BindTexture(const GLuint uiTexture=0)
  { // Ignore if already bound
    if(uiTexture == uiActiveTexture) return;
    // Bind the texture
    sAPI.glBindTexture(GL_TEXTURE_2D, uiTexture);
    // Set active texture
    uiActiveTexture = uiTexture;
  }
  /* -- Create multiple framebuffer objects -------------------------------- */
  void CreateFBOs(const GLsizei stCount, GLuint*const uipFbo) const
    { sAPI.glGenFramebuffers(stCount, uipFbo); }
  /* -- Create one framebuffer object -------------------------------------- */
  void CreateFBO(GLuint*const uipFbo) const { CreateFBOs(1, uipFbo); }
  /* ----------------------------------------------------------------------- */
  void BindFBO(const GLuint uiFbo=0)
  { // If we're already the active FBO, ignore
    if(uiActiveFbo == uiFbo) return;
    // Bind the FBO
    sAPI.glBindFramebuffer(GL_FRAMEBUFFER, uiFbo);
    // Cache the active fbo
    uiActiveFbo = uiFbo;
  }
  /* ----------------------------------------------------------------------- */
  void DeleteFBO(const GLsizei stCount, const GLuint*const uipFbo)
  { // Check each fbo that is about to be deleted
    for(GLsizei stIndex = 0; stIndex < stCount; ++stIndex)
    { // Keep scanning until we find a cached fbo
      if(uipFbo[stIndex] != uiActiveFbo) continue;
      // Reset currently selected fbo
      uiActiveFbo = 0;
      // No need to check any others
      break;
    } // Delete the fbo
    sAPI.glDeleteFramebuffers(stCount, uipFbo);
  }
  /* ----------------------------------------------------------------------- */
  void AttachTexFBO(const GLuint uiTId) const
    { sAPI.glFramebufferTexture2D(GL_FRAMEBUFFER,
      GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, uiTId, 0); }
  /* ----------------------------------------------------------------------- */
  GLuint VerifyFBO(void) const
    { return sAPI.glCheckFramebufferStatus(GL_FRAMEBUFFER); }
  /* -- GLSL functions ----------------------------------------------------- */
  GLuint CreateShader(const GLenum eType) const
    { return sAPI.glCreateShader(eType); }
  /* ----------------------------------------------------------------------- */
  void DeleteShader(const GLuint uiShader) const
    { sAPI.glDeleteShader(uiShader); }
  /* ----------------------------------------------------------------------- */
  void ShaderSource(const GLuint uiShader, const GLchar*const cpCode) const
    { sAPI.glShaderSource(uiShader, 1, &cpCode, nullptr); }
  /* ----------------------------------------------------------------------- */
  void CompileShader(const GLuint uiShader) const
    { sAPI.glCompileShader(uiShader); }
  /* ----------------------------------------------------------------------- */
  void AttachShader(const GLenum uiProgram, const GLuint uiShader) const
    { sAPI.glAttachShader(uiProgram, uiShader); }
  /* ----------------------------------------------------------------------- */
  void DetachShader(const GLenum uiProgram, const GLuint uiShader) const
    { sAPI.glDetachShader(uiProgram, uiShader); }
  /* -- Get compilation status --------------------------------------------- */
  GLenum GetCompileStatus(const GLuint uiProgram) const
    { return GetShaderInt<GLenum>(uiProgram, GL_COMPILE_STATUS); }
  /* ----------------------------------------------------------------------- */
  GLsizei GetShaderInfoLog(const GLuint uiPId, const GLsizei stMaxLength,
    GLchar*const cpDest) const
  { // Storage for bytes written to string
    GLsizei stLength;
    // Do the query
    sAPI.glGetShaderInfoLog(uiPId, stMaxLength, &stLength, cpDest);
    // Return result
    return stLength;
  }
  /* -- Get compilation failure reason ------------------------------------- */
  const string GetCompileFailureReason(const GLuint uiId) const
  { // Make string to store the message and return generic string if empty
    string sErrMsg;
    sErrMsg.resize(GetShaderInt<size_t>(uiId, GL_INFO_LOG_LENGTH));
    if(sErrMsg.empty()) return "No reason";
    // Get the error message
    sErrMsg.resize(static_cast<size_t>(
      GetShaderInfoLog(uiId, static_cast<GLsizei>(sErrMsg.size()),
        ToNonConstCast<GLchar*>(sErrMsg.data()))));
    // Get error and if an error occured put it as a failure reason
    const GLenum glError = GetError();
    if(glError != GL_NO_ERROR || sErrMsg.empty())
      return Format("Problem getting reason (0x$$)", hex, glError);
    // Error message should have a carriage return/line feed so remove it
    for(unsigned int uiCount = 0; uiCount < 2; ++uiCount)
      if(!sErrMsg.empty()) sErrMsg.pop_back();
    // Return the string
    return sErrMsg;
  }
  /* -- Get linker status -------------------------------------------------- */
  GLenum GetLinkStatus(const GLuint uiProgram) const
    { return GetProgramInt<GLenum>(uiProgram, GL_LINK_STATUS); }
  /* ----------------------------------------------------------------------- */
  GLsizei GetProgramInfoLog(const GLuint uiProgram, const GLsizei stMaxLength,
    GLchar*const cpDest) const
  { // Perform query and store in a variable and return it
    GLsizei stLength;
    sAPI.glGetProgramInfoLog(uiProgram, stMaxLength, &stLength, cpDest);
    return stLength;
  }
  /* -- Get linker failure reason ------------------------------------------ */
  const string GetLinkFailureReason(const GLuint uiProgram) const
  { // Make string to store the message and return generic string if empty
    string sErrMsg;
    sErrMsg.resize(GetProgramInt<size_t>(uiProgram, GL_INFO_LOG_LENGTH));
    if(sErrMsg.empty()) return "No reason";
    // Get the error message and resize string as a result
    sErrMsg.resize(static_cast<size_t>(GetProgramInfoLog(uiProgram,
      static_cast<GLsizei>(sErrMsg.size()),
      ToNonConstCast<GLchar*>(sErrMsg.data()))));
    // Get error and if an error occured put it as a failure reason
    const GLenum glError = GetError();
    if(glError != GL_NO_ERROR || sErrMsg.empty())
      return Format("Problem getting reason ($$)", hex, glError);
    // Error message should have a carriage return/line feed so remove it
    for(int iI = 0; iI < 2 && !sErrMsg.empty(); ++iI) sErrMsg.pop_back();
    // Return the string
    return sErrMsg;
  }
  /* ----------------------------------------------------------------------- */
  template<typename IntegerType>
    const IntegerType GetShaderInt(const GLuint uiShader,
      const GLenum eCmd) const
  { // Storage for result
    IntegerType tResult;
    // Do the query
    IGL(sAPI.glGetShaderiv(uiShader, eCmd, reinterpret_cast<GLint*>(&tResult)),
      "Get shader integer failed!", "Shader", uiShader, "Enum", eCmd);
    // return the result
    return tResult;
  }
  /* ----------------------------------------------------------------------- */
  GLuint CreateProgram(void) const { return sAPI.glCreateProgram(); }
  /* ----------------------------------------------------------------------- */
  template<typename IntegerType>
    const IntegerType GetProgramInt(const GLuint uiProgram,
      const GLenum eCmd) const
  { // Storage for result
    IntegerType tResult;
    // Do the query
    IGL(sAPI.glGetProgramiv(uiProgram, eCmd,
      reinterpret_cast<GLint*>(&tResult)),
        "Get program integer failed!", "Program", uiProgram, "Enum", eCmd);
    // return the result
    return tResult;
  }
  /* ----------------------------------------------------------------------- */
  void DeleteProgram(const GLuint uiProgram)
  { // Delete the program
    sAPI.glDeleteProgram(uiProgram);
    // If it was the active program then reset the cache
    if(uiActiveProgram == uiProgram) uiActiveProgram = 0;
  }
  /* ----------------------------------------------------------------------- */
  void LinkProgram(const GLuint uiProgram) const
    { sAPI.glLinkProgram(uiProgram); }
  /* ----------------------------------------------------------------------- */
  GLuint GetProgram(void) const
    { return GetInteger<GLuint>(GL_CURRENT_PROGRAM); }
  /* ----------------------------------------------------------------------- */
  void UseProgram(const GLuint uiProgram=0)
  { // Ignore if we already have the program selected
    if(uiProgram == uiActiveProgram) return;
    // Activate the shader program
    sAPI.glUseProgram(uiProgram);
    // Update the active program
    uiActiveProgram = uiProgram;
  }
  /* ----------------------------------------------------------------------- */
  GLint GetUniformLocation(const GLuint uiProgram,
    const GLchar*const cpValue) const
      { return sAPI.glGetUniformLocation(uiProgram, cpValue); }
  /* ----------------------------------------------------------------------- */
  void BindAttribLocation(const GLuint uiProgram, const GLuint uiAttrib,
    const GLchar*const cpValue) const
      { sAPI.glBindAttribLocation(uiProgram, uiAttrib, cpValue); }
  /* -- Assign a integer value to a uniform -------------------------------- */
  void Uniform(const GLint iUniform, const GLint iValue) const
    { sAPI.glUniform1i(iUniform, iValue); }
  /* -- Assign four floats to a uniform ------------------------------------ */
  void Uniform(const GLint iUniform, const GLfloat fV1, const GLfloat fV2,
    const GLfloat fV3, const GLfloat fV4) const
      { sAPI.glUniform4f(iUniform, fV1, fV2, fV3, fV4); }
  /* -- Create multiple vertex buffer objects ------------------------------ */
  void GenVertexBuffers(const GLsizei stCount, GLuint*const uipVbo) const
    { sAPI.glGenBuffers(stCount, uipVbo); }
  /* -- Create one vertex buffer object ------------------------------------ */
  void GenVertexBuffer(GLuint*const uipVbo) const
    { GenVertexBuffers(1, uipVbo); }
  /* -- Bind vertex buffer object ------------------------------------------ */
  void BindVertexBuffer(const GLenum eTarget, const GLuint uiVbo)
  { // Ignore if this vbo is already selected
    if(uiVbo == uiActiveVbo) return;
    // Bind the vbo
    sAPI.glBindBuffer(eTarget, uiVbo);
    // Is active vbo
    uiActiveVbo = uiVbo;
  }
  /* -- Bind vertex buffer object ------------------------------------------ */
  void BindStaticVertexBuffer(const GLuint uiVbo)
    { BindVertexBuffer(GL_ARRAY_BUFFER, uiVbo); }
  /* -- Delete multiple vertex buffer objects ------------------------------ */
  void DeleteVertexBuffers(const GLsizei stCount, const GLuint*const uipVbo)
  { // Check each vbo deleted
    for(GLsizei stIndex = 0; stIndex < stCount; ++stIndex)
    { // Keep scanning until we find a bound vbo
      if(uipVbo[stIndex] != uiActiveVbo) continue;
      // Reset currently bound vbo
      uiActiveVbo = 0;
      // No need to check any others
      break;
    } // Delete the vbo's
    sAPI.glDeleteBuffers(stCount, uipVbo);
  }
  /* -- Delete one vertex buffer object ------------------------------------ */
  void DeleteVertexBuffer(const GLuint uipVbo)
    { DeleteVertexBuffers(1, &uipVbo); }
  /* -- Generate vertex array objects -------------------------------------- */
  void GenVertexArrays(const GLsizei stCount, GLuint*const uipVao) const
    { sAPI.glGenVertexArrays(stCount, uipVao); }
  /* -- Generate a single vertex array object ------------------------------ */
  void GenVertexArray(GLuint*const uipVao) const
    { GenVertexArrays(1, uipVao); }
  /* -- Bind vertex array object ------------------------------------------- */
  void BindVertexArray(const GLuint uiVao)
  { // Ignore if this vao is already selected
    if(uiVao == uiActiveVao) return;
    // Bind the vertex array
    sAPI.glBindVertexArray(uiVao);
    // Is active vao
    uiActiveVao = uiVao;
  }
  /* -- Delete multiple vertex array objects ------------------------------- */
  void DeleteVertexArrays(const GLsizei stCount, const GLuint*const uipVao)
  { // Check each vao deleted
    for(GLsizei stIndex = 0; stIndex < stCount; ++stIndex)
    { // Keep scanning until we find a bound vao
      if(uipVao[stIndex] != uiActiveVao) continue;
      // Reset currently bound vao
      uiActiveVao = 0;
      // No need to check any others
      break;
    } // Delete the vao's
    sAPI.glDeleteVertexArrays(stCount, uipVao);
  }
  /* -- Delete single vertex array object ---------------------------------- */
  void DeleteVertexArray(const GLuint uipSrc)
    { DeleteVertexArrays(1, &uipSrc); }
  /* -- Data buffering functions ------------------------------------------- */
  void BufferData(const GLenum eTarget, const GLsizei stSize,
    const GLvoid*const vpBuffer, const GLenum eUsage) const
      { sAPI.glBufferData(eTarget, stSize, vpBuffer, eUsage); }
  /* ----------------------------------------------------------------------- */
  void BufferStaticData(const GLsizei stSize, const GLvoid*const vpBuffer)
    { BufferData(GL_ARRAY_BUFFER, stSize, vpBuffer, GL_STREAM_DRAW); }
  /* -- Clear bound fbo or back buffer ------------------------------------- */
  void ClearBuffer(void) const { sAPI.glClear(GL_COLOR_BUFFER_BIT); }
  /* -- Set clear colour --------------------------------------------------- */
  void CommitClearColour(void) const
   { sAPI.glClearColor(GetColourRed(),  GetColourGreen(),
                       GetColourBlue(), GetColourAlpha()); }
  /* ----------------------------------------------------------------------- */
  void SetClearColourInt(const unsigned int uiColour)
    { SetColourInt(uiColour); CommitClearColour(); }
  /* -- Set clear colour (applies to all fbos) ----------------------------- */
  void SetClearColourIfChanged(const FboColour &fcData)
    { if(SetColour(fcData)) CommitClearColour(); }
  /* ----------------------------------------------------------------------- */
  void SetAndClear(const FboColour &cCol)
    { SetClearColourIfChanged(cCol); ClearBuffer(); }
  /* ----------------------------------------------------------------------- */
  GLenum GetError(void) const { return sAPI.glGetError(); }
  /* ----------------------------------------------------------------------- */
  void DrawArrays(const GLenum eMode, const GLint iFirst,
    const GLsizei stCount) const
      { sAPI.glDrawArrays(eMode, iFirst, stCount); }
  /* ----------------------------------------------------------------------- */
  void DrawArraysTriangles(const GLsizei stCount) const
    { DrawArrays(GL_TRIANGLES, 0, stCount); }
  /* ----------------------------------------------------------------------- */
  void CommitViewport(void)
    { sAPI.glViewport(GetCoLeft(), GetCoTop(), GetCoRight(), GetCoBottom()); }
  /* ----------------------------------------------------------------------- */
  void SetViewportWH(const GLsizei stWidth, const GLsizei stHeight)
    { if(SetCoords(0, 0, stWidth, stHeight)) CommitViewport(); }
  /* ----------------------------------------------------------------------- */
  void SetViewport(const FboViewport &fcData)
    { if(SetCoords(fcData)) CommitViewport(); }
  /* -- Get openGL float array --------------------------------------------- */
  template<size_t stCount>
    void GetFloatArray(const GLenum eId, GLfloat *const fpDest) const
      { IGL(sAPI.glGetFloatv(eId, fpDest),
          "Get float array failed!", "Index", eId, "Count", stCount); }
  /* -- Get openGL float array --------------------------------------------- */
  template<size_t stCount, class ArrayType = array<GLfloat, stCount>>
    const ArrayType GetFloatArray(const GLenum eId) const
  { // Create array to return
    ArrayType aData;
    // Get specified value for enum and store it
    GetFloatArray<stCount>(eId, aData.data());
    // Return array
    return aData;
  }
  /* -- Get openGL int array ----------------------------------------------- */
  template<size_t stCount, class ArrayType = array<GLint, stCount>>
    const ArrayType GetIntegerArray(const GLenum eId) const
  { // Create array to return
    ArrayType aData;
    // Get specified value for enum and store it
    IGL(sAPI.glGetIntegerv(eId, aData.data()),
      "Get integer array failed!", "Index", eId, "Count", stCount);
    // Return array
    return aData;
  }
  /* -- Get openGL int ----------------------------------------------------- */
  template<typename IntegerType = GLint>
    IntegerType GetInteger(const GLenum eId) const
      { return static_cast<IntegerType>(GetIntegerArray<1>(eId)[0]); }
  /* -- Get openGL string -------------------------------------------------- */
  template<typename PtrType = GLubyte>
    const PtrType* GetString(const GLenum eId) const
  { // Get the variable and throw error if occured
    const GLubyte*const ucpStr = sAPI.glGetString(eId);
    IGLC("Get string failed!", "Index", eId);
    // Sanity check actual string
    if(!ucpStr || !*ucpStr)
      XC("Invalid string returned!", "Index", eId, "String", ucpStr);
    // Return result
    return reinterpret_cast<const PtrType*>(ucpStr);
  }
  /* -- Do delete all the marked fbo handles ------------------------------- */
  void DeleteMarkedFboHandles(void)
  { // Delete the fbos
    IGLL(DeleteFBO(static_cast<GLsizei>(vFbos.size()), vFbos.data()),
      "OGL failed to destroy $ fbo handles!", vFbos.size());
    // Clear the list
    vFbos.clear();
  }
  /* -- Do delete all the marked texture handles --------------------------- */
  void DeleteMarkedTextureHandles(void)
  { // Delete the marked textures
    IGLL(DeleteTexture(static_cast<GLsizei>(vTextures.size()),
      vTextures.data()), "OGL failed to destroy $ texture handles!",
      vTextures.size());
    // Clear the list
    vTextures.clear();
  }
  /* -- Enable vertex attribute array ------------------------------ */ public:
  void EnableVertexAttribArray(const GLuint uiAId) const
    { sAPI.glEnableVertexAttribArray(uiAId); }
  /* -- Disable vertex attribute array ------------------------------------- */
  void DisableVertexAttribArray(const GLuint uiAId) const
    { sAPI.glDisableVertexAttribArray(uiAId); }
  /* -- Enable an extension ------------------------------------------------ */
  void EnableExtension(const GLenum eParam) const
  { // Ignore if already disabled
    if(sAPI.glIsEnabled(eParam)) return;
    // Enable the extension and log result
    IGL(sAPI.glEnable(eParam), "Enable extension failed!", "eParam", eParam);
    LW(LH_DEBUG, "OGL enabled extension 0x$$.", hex, eParam);
  }
  /* -- Disable an extension ----------------------------------------------- */
  void DisableExtension(const GLenum eParam) const
  { // Ignore if already disabled
    if(!sAPI.glIsEnabled(eParam)) return;
    // Disable the extension and log result
    IGL(sAPI.glDisable(eParam), "Disable extension failed!", "eParam", eParam);
    LW(LH_DEBUG, "OGL disabled extension 0x$$.", hex, eParam);
  }
  /* -- Commit blending algorithms ----------------------------------------- */
  void CommitBlend(void)
    { sAPI.glBlendFuncSeparate(GetSrcRGB(), GetDstRGB(),
                               GetSrcAlpha(), GetDstAlpha()); }
  /* -- Set blending algorithms -------------------------------------------- */
  void SetBlendIfChanged(const FboBlend &fbOther)
    { if(SetBlend(fbOther)) CommitBlend(); }
  /* -- Set texture parameter (No error checking needed) ------------------- */
  void SetTexParam(const GLenum eVar, const GLint iVal) const
    { sAPI.glTexParameteri(GL_TEXTURE_2D, eVar, iVal); }
  /* -- Update hint -------------------------------------------------------- */
  void SetHint(const GLenum eType, const GLenum eMode) const
  { // Get opengl hint and throw if not failed else set hint
    IGL(sAPI.glHint(eType, eMode), "Failed to set hint!",
      "Target", eType, "Value", eMode);
    LW(LH_DEBUG, "OGL set hint 0x$$ to 0x$.", hex, eType, eMode);
  }
  /* -- Update polygon rendering mode -------------------------------------- */
  void SetPolygonMode(const GLenum eType, const GLenum eMode) const
  { // Set polygon mode and log result
    IGL(sAPI.glPolygonMode(eType, eMode), "Failed to set polygon mode!",
      "Buffer", eType, "Mode", eMode);
    LW(LH_DEBUG, "OGL set polymode to 0x$$/0x$.", hex, eType, eMode);
  }
  /* -- GL is initialised? ------------------------------------------------- */
  bool IsGLInitialised(void) const { return FlagIsSet(GFL_INITIALISED); }
  bool IsGLNotInitialised(void) const { return !IsGLInitialised(); }
  void SetInitialised(const bool bState, const bool bForce=false)
  { // De-Initialising?
    if(!bState)
    { // If not initialised?
      if(IsGLNotInitialised())
      { // Only destructor allowed to continue
        if(bForce) return;
        // Show error
        XC("OGL is not initialised!");
      } // Log as de-initialised
      LW(LH_INFO, "OGL set to de-initialised.");
      // Return cleared flag
      return FlagClear(GFL_INITIALISED);
    } // Initialised and already initialised? Throw error
    if(!bForce && IsGLInitialised()) XC("OGL was already initialised!");
    // Detect GL capabilities
    DetectCapabilities();
    // Show change in state
    LW(LH_INFO, "OGL set initialised with capabilities 0x$$...\n"
                "- Renderer: $.\n"
                "- Version: $.\n"
                "- Vendor: $.",
      hex, FlagGet(), GetRenderer(), GetVersion(), GetVendor());
    // If debug log level?
    if(cLog->HasLevel(LH_DEBUG))
    { // Get number of extensions and return if we're not interested in them?
      const GLuint uiExts = GetExtensionCount();
      // Report device selected and basic capabilities
      cLog->WriteStringSafe(LH_DEBUG,
        Format("- Clear colour: $,$,$,$; " "Blend: $,$,$,$.\n"
               "- Viewport: $,$,$,$; "     "Maximum texture size: $$^2.\n"
               "- Pack alignment: $; "     "Unpack alignment: $.\n"
               "- Unpack row length: $; "  "Vertex attributes: $.\n"
               "- Texture units: $; "      "Extensions count: $.",
        GetColourRed(),     GetColourGreen(),   GetColourBlue(),
        GetColourAlpha(),   GetSrcRGB(),        GetDstRGB(),
        GetSrcAlpha(),      GetDstAlpha(),      GetCoLeft(),
        GetCoTop(),         GetCoRight(),       GetCoBottom(),
        dec,                MaxTexSize(),       PackAlign(),
        UnpackAlign(),      UnpackRowLength(),  MaxVertexAttribs(),
        uiTexUnits,         uiExts));
      // Build sorted list of extensions and log them all
      StrUIntMap mExts;
      for(GLuint uiI = 0; uiI < uiExts; ++uiI)
        mExts.insert({ GetExtension(uiI), uiI });
      for(const auto &mI : mExts)
        cLog->WriteStringSafe(LH_DEBUG,
          Format("- Have extension '$' (#$).", mI.first, mI.second));
    } // Show warning if not enough texture units
    if(uiTexUnits < 3)
      LW(LH_WARNING, "Ogl detected only $ of the three texture units that "
        "are required for video playback!");
    // Set the initialised flag
    FlagSet(GFL_INITIALISED);
  }
  /* -- Set texture mode by filter id -------------------------------------- */
  void SetFilterById(const size_t stId, GLint &iMin, GLint &iMag) const
  { // Filter table
    static const array<const array<const GLint, 2>, OF_NM_MAX> tfList{
    { // Point/Bilinear filtering options
      { GL_NEAREST, GL_NEAREST }, { GL_NEAREST, GL_LINEAR }, // 00-01
      { GL_LINEAR,  GL_NEAREST }, { GL_LINEAR,  GL_LINEAR }, // 02-03
    } };
    // Lookup table and set values
    iMag = tfList[stId][0];
    iMin = tfList[stId][1];
  }
  /* -- Set texture mode by filter id -------------------------------------- */
  void SetMipMapFilterById(const size_t stId, GLint &iMin, GLint &iMag) const
  { // Filter table
    static const array<const array<const GLint, 2>, OF_MAX> tfList{
    { // Point/Bilinear filtering options
      { GL_NEAREST, GL_NEAREST }, { GL_NEAREST, GL_LINEAR }, // 00-01
      { GL_LINEAR,  GL_NEAREST }, { GL_LINEAR,  GL_LINEAR }, // 02-03
      // Point/Bilinear/Mipmap/Trilinear filtering options
      { GL_NEAREST, GL_NEAREST_MIPMAP_NEAREST }, // 04
      { GL_LINEAR,  GL_NEAREST_MIPMAP_NEAREST }, // 05
      { GL_NEAREST, GL_NEAREST_MIPMAP_LINEAR },  // 06
      { GL_LINEAR,  GL_NEAREST_MIPMAP_LINEAR },  // 07
      { GL_NEAREST, GL_LINEAR_MIPMAP_NEAREST },  // 08
      { GL_LINEAR,  GL_LINEAR_MIPMAP_NEAREST },  // 09
      { GL_NEAREST, GL_LINEAR_MIPMAP_LINEAR },   // 10
      { GL_LINEAR,  GL_LINEAR_MIPMAP_LINEAR }    // 11
    } };
    // Lookup table and set values
    iMag = tfList[stId][0];
    iMin = tfList[stId][1];
  }
  /* -- Extensions --------------------------------------------------------- */
  GLuint GetExtensionCount(void) const
    { return GetInteger<GLuint>(GL_NUM_EXTENSIONS); }
  /* ----------------------------------------------------------------------- */
  const char *GetExtension(const GLuint uiI) const
    { return reinterpret_cast<const char*>
        (sAPI.glGetStringi(GL_EXTENSIONS, uiI)); }
  /* -- Read OpenGL renderer data ------------------------------------------ */
  const string &GetVendor(void) const { return strVendor; }
  const string &GetRenderer(void) const { return strRenderer; }
  const string &GetVersion(void) const { return strVersion; }
  /* ----------------------------------------------------------------------- */
  bool IsVSyncEnabled(void) { return FlagIsSet(GFL_VSYNC); }
  /* -- Update texture destroy list size ----------------------------------- */
  CVarReturn SetTexDListReserve(const size_t stCount)
    { return BoolToCVarReturn(ReserveList(vTextures, stCount)); }
  /* -- Update fbo destroy list size --------------------------------------- */
  CVarReturn SetFboDListReserve(const size_t stCount)
    { return BoolToCVarReturn(ReserveList(vFbos, stCount)); }
  /* -- Update minimum VRAM ------------------------------------------------ */
  CVarReturn SetMinVRAM(const GLuint64 qwValue)
    { return CVarSimpleSetInt(qwMinVRAM, qwValue); }
  /* -- Update hints ------------------------------------------------------- */
  CVarReturn SetQCompressHint(const size_t stMode)
  { // Ignore if no context, but still succeeded
    if(IsGLNotInitialised()) return ACCEPT;
    // Get new value to set and return false if invalid
    switch(const GLenum eNew = SHIndexToEnum(stMode))
    { // Invalid parameter or zero
      case GL_NONE: case GL_TRUE: return DENY;
      // Anything else, set the hint
      default: SetHint(GL_TEXTURE_COMPRESSION_HINT, eNew); break;
    } // Succeeded
    return ACCEPT;
  }
  /* -- Update hints ------------------------------------------------------- */
  CVarReturn SetQPolygonHint(const size_t stMode)
  { // Ignore if no context, but still succeeded
    if(IsGLNotInitialised()) return ACCEPT;
    // Get new value to set and return false if invalid
    switch(const GLenum eNew = SHIndexToEnum(stMode))
    { // Invalid parameter
      case GL_NONE: return DENY;
      // If feature is to be disabled? Disable it
      case GL_TRUE: DisableExtension(GL_POLYGON_SMOOTH); break;
      // Anything else, set the hint and enable it
      default: SetHint(GL_POLYGON_SMOOTH_HINT, eNew);
               EnableExtension(GL_POLYGON_SMOOTH); break;
    } // Succeeded
    return ACCEPT;
  }
  /* -- Update hints ------------------------------------------------------- */
  CVarReturn SetQLineHint(const size_t stMode)
  { // Ignore if no context, but still succeeded
    if(IsGLNotInitialised()) return ACCEPT;
    // Get new value to set and return false if invalid
    switch(const GLenum eNew = SHIndexToEnum(stMode))
    { // Invalid parameter
      case GL_NONE: return DENY;
      // If feature is to be disabled? Disable it
      case GL_TRUE: DisableExtension(GL_LINE_SMOOTH); break;
      // Anything else, set the hint and enable it
      default: SetHint(GL_LINE_SMOOTH_HINT, eNew);
               EnableExtension(GL_LINE_SMOOTH); break;
    } // Succeeded
    return ACCEPT;
  }
  /* -- Update hints ------------------------------------------------------- */
  CVarReturn SetQShaderHint(const size_t stMode)
  { // Ignore if no context, but still succeeded
    if(IsGLNotInitialised()) return ACCEPT;
    // Get new value to set and return false if invalid
    switch(const GLenum eNew = SHIndexToEnum(stMode))
    { // Invalid parameter or zero
      case GL_NONE: case GL_TRUE: return DENY;
      // Anything else, set the hint
      default: SetHint(GL_FRAGMENT_SHADER_DERIVATIVE_HINT, eNew); break;
    } // Succeeded
    return ACCEPT;
  }
  /* -- Update polygon rendering mode -------------------------------------- */
  CVarReturn SetPolygonMode(const bool bWireframe)
  { // Ignore if no context, but still succeeded
    if(IsGLNotInitialised()) return ACCEPT;
    // Get state
    SetPolygonMode(GL_FRONT_AND_BACK, bWireframe ? GL_LINE : GL_FILL);
    // Succeeded
    return ACCEPT;
  }
  /* -- Reset all binds ---------------------------------------------------- */
  void ResetBinds(void)
  { // Unbind active texture
    ActiveTexture();
    // Unbind shader program
    UseProgram();
    // Unbind texture
    BindTexture();
    // Unbind fbo and select main back buffer
    BindFBO();
    // Select default vertex buffer
    BindStaticVertexBuffer(uiVBO);
    // Select default vertex array
    BindVertexArray(uiVAO);
  }
  /* -- Mark an array of textures for deletion ----------------------------- */
  template<class List>void SetDeleteTextures(const List &vT)
    { vTextures.insert(vTextures.cend(), vT.cbegin(), vT.cend()); }
  /* -- Mark a single texture for deletion --------------------------------- */
  void SetDeleteTexture(const GLuint uiI)
    { vTextures.emplace_back(uiI); }
  /* -- Mark an fbo for deletion ------------------------------------------- */
  void SetDeleteFbo(const GLuint uiI)
    { vFbos.emplace_back(uiI); }
  /* -- Do delete all the marked textures and fbos ------------------------- */
  void DeleteTexturesAndFboHandles(void)
  { // Have textures to delete?
    if(!vTextures.empty()) DeleteMarkedTextureHandles();
    // Have fbos to delete?
    if(!vFbos.empty()) DeleteMarkedFboHandles();
  }
  /* -- Get available and total VRAM --------------------------------------- */
  GLuint64 GetVRAMFree(void) { return qwFreeVRAM; }
  GLuint64 GetVRAMTotal(void) { return qwTotalVRAM; }
  /* -- Get free memory on nvidia cards ------------------------------------ */
  void UpdateVRAMAvailableNV(void)
  { // - OG macro name: GPU_MEMORY_INFO_CURRENT_AVAILABLE_VIDMEM_NVX
    // - https://www.khronos.org/registry/OpenGL/extensions/
    //     NVX/NVX_gpu_memory_info.txt
    qwFreeVRAM = GetInteger<GLuint64>(0x9049) * 1024;
  }
  /* -- Get free memory on ATI cards --------------------------------------- */
  void UpdateVRAMAvailableATI(void)
  { // - OG macro name: TEXTURE_FREE_MEMORY_ATI
    // - https://www.khronos.org/registry/OpenGL/extensions/
    //     ATI/ATI_meminfo.txt
    qwFreeVRAM = static_cast<GLuint64>(GetIntegerArray<4>(0x87FC)[0]) * 1024;
    // Update total if higher
    if(qwFreeVRAM > qwTotalVRAM) qwTotalVRAM = qwFreeVRAM;
  }
  /* -- Get free memory on shared system memory renderer ------------------- */
  void UpdateVRAMAvailableShared(void)
  { // Update system memory usage data
    cSystem->UpdateMemoryUsageData();
    // Get system values
    qwFreeVRAM = cSystem->RAMFree();
  }
  /* -- Get memory information --------------------------------------------- */
  void UpdateVRAMAvailable(void)
  { // Have NVIDIA free memory?
    if(FlagIsSet(GFL_HAVENVMEM)) UpdateVRAMAvailableNV();
    // Have ATI free memory
    else if(FlagIsSet(GFL_HAVEATIMEM)) UpdateVRAMAvailableATI();
    // Have shared memory
    else if(FlagIsSet(GFL_SHARERAM)) UpdateVRAMAvailableShared();
  }
  /* -- Verify RAM constraints --------------------------------------------- */
  void VerifyVRAMConstraints(void)
  { // Have nVIDIA memory info?
    if(FlagIsSet(GFL_HAVENVMEM))
    { // Get total video memory
      // - OG macro name: GPU_MEMORY_INFO_TOTAL_AVAILABLE_MEMORY_NVX
      qwTotalVRAM = static_cast<GLuint64>(GetInteger<GLuint>(0x9048)) * 1024;
      // Update available VRAM
      UpdateVRAMAvailableNV();
      // Report VRAM information to log
      LW(LH_DEBUG, "- Using NVIDIA memory functions.");
    } // Have ATI memory info?
    else if(FlagIsSet(GFL_HAVEATIMEM))
    { // No total video memory on ATI cards so let UpdateVRAMAvailable set it.
      qwTotalVRAM = 0;
      // Update available VRAM
      UpdateVRAMAvailableATI();
      // Report VRAM information to log
      LW(LH_DEBUG, "- Using ATI memory functions (no total available).");
    } // Device shares memory with system?
    else if(FlagIsSet(GFL_SHARERAM))
    { // Update total memory information
      qwTotalVRAM = cSystem->RAMTotal();
      // Update memory information
      UpdateVRAMAvailableShared();
      // Report VRAM information to log
      LW(LH_DEBUG, "- Renderer shares memory with system.");
    } // Have no memory information command?
    else
    { // Set maximum total VRAM
      qwTotalVRAM = qwFreeVRAM = numeric_limits<GLuint64>::max();
      // Report VRAM information to log
      LW(LH_WARNING, "- Video memory data functions not available.");
      // No need to check anything
      return;
    } // Load free VRAM and if we don't have enough free VRAM? Throw error
    if(qwMinVRAM && qwFreeVRAM < qwMinVRAM)
      XC("There is not enough video memory available. Close any "
         "running applications consuming it and try running again!",
         "Available", qwFreeVRAM,      "Total",  qwTotalVRAM,
         "Percent",   MakePercentage(qwFreeVRAM, qwTotalVRAM),
         "Required",  qwMinVRAM,       "Needed", qwMinVRAM - qwFreeVRAM);
    // Report VRAM information to log
    LW(LH_INFO, "- Total VRAM: $ bytes ($).\n"
                "- Available VRAM: $ bytes ($).",
      qwTotalVRAM, ToBytesStr(qwTotalVRAM), qwFreeVRAM,
      ToBytesStr(qwFreeVRAM));
  }
  /* --------------------------------------------------------------- */ public:
  template<typename IntType>const string &GetGLErr(const IntType itCode) const
    { return imOGLCodes.Get(static_cast<GLenum>(itCode)); }
  /* -- Set context -------------------------------------------------------- */
  void SetContext(void) { cGlFW->SetContext(); }
  /* -- Release context ---------------------------------------------------- */
  void ReleaseContext(void) { cGlFW->ReleaseContext(); }
  /* -- Initialise --------------------------------------------------------- */
  void Init(const int, const bool bForce=false)
  { // Class initialised
    if(!bForce) IHInitialise();
    // Log class initialising
    LW(LH_DEBUG, "OGL subsystem initialising...");
    // Set context
    SetContext();
    // Load GL functions
    if(const char*const cpErr = LoadFunctions())
      XCS("Failed to load OpenGL export!", "Function", cpErr);
    // Clear error that Wine or GLFW might have caused
    if(const GLenum eError = sAPI.glGetError())
      LW(LH_DEBUG, "Ogl cleared host caused error code! ($/$$).",
        GetGLErr(eError), hex, eError);
    // OpenGL is now initialised
    SetInitialised(true, bForce);
    // Check that there is enough VRAM available if requested
    VerifyVRAMConstraints();
    // Init vsync
    FlagSetOrClear(GFL_VSYNC, cCVars->GetInternalSafe<bool>(VID_VSYNC));
    cGlFW->SetVSync(IsVSyncEnabled());
    // Enable extensions
    EnableExtension(GL_BLEND);
    EnableExtension(GL_DITHER);
    // Set hints. Indicates the accuracy of the derivative calculation for the
    // GL shading language fragment processing built-in functions: dFdx, dFdy,
    // and fwidth.
    SetQShaderHint(cCVars->GetInternalSafe<size_t>(VID_QSHADER));
    // Indicates the sampling quality of antialiased lines. If a larger filter
    // function is applied, hinting GL_NICEST can result in more pixel
    // fragments being generated during rasterization.
    SetQLineHint(cCVars->GetInternalSafe<size_t>(VID_QLINE));
    // Indicates the sampling quality of antialiased polygons. Hinting
    // GL_NICEST can result in more pixel fragments being generated during
    // rasterization, if a larger filter function is applied.
    SetQPolygonHint(cCVars->GetInternalSafe<size_t>(VID_QPOLYGON));
    // Indicates the quality and performance of the compressing texture images.
    // Hinting GL_FASTEST indicates that texture images should be compressed as
    // quickly as possible, while GL_NICEST indicates that texture images
    // should be compressed with as little image quality loss as possible.
    // GL_NICEST should be selected if the texture is to be retrieved by
    // glGetCompressedTexImage for reuse.
    SetQCompressHint(cCVars->GetInternalSafe<size_t>(VID_QCOMPRESS));
    // Set polygon mode
    SetPolygonMode(cCVars->GetInternalSafe<bool>(VID_WIREFRAME));
    // Set window size limits. Let the guest choose the minimum and the maximum
    // must not exceed the video cards maximum texture size or possibly BOOM!
    cEvtMain->Add(EMC_WIN_LIMITS,
      cCVars->GetInternalSafe<int>(WIN_WIDTHMIN),
      cCVars->GetInternalSafe<int>(WIN_HEIGHTMIN),
      MaxTexSize(), MaxTexSize());
    // Set pack alignment for grabbing screenshots and unpack alignment for
    // uploading odd sized textures.
    IGL(SetPackAlignment(1), "Set byte pack alignment failed!");
    IGL(SetUnpackAlignment(1), "Set byte unpack alignment failed!");
    // Create and bind VAO
    IGL(GenVertexArray(&uiVAO), "Generate default VAO failed!");
    IGL(BindVertexArray(uiVAO), "Bind default VAO failed!", "Index", uiVAO);
    // Create and bind VBO
    IGL(GenVertexBuffer(&uiVBO), "Generate default VBO failed!");
    IGL(BindStaticVertexBuffer(uiVBO), "Bind default VBO failed!",
      "Index", uiVBO);
    // Log class initialising
    LW(LH_INFO, "OGL subsystem initialised.");
  }
  /* -- DeInitialise ------------------------------------------------------- */
  void DeInit(const bool bForce=false)
  { // Ignore if class not initialised
    if(IHNotDeInitialise()) return;
    // OpenGL is now de-initialised
    SetInitialised(false, bForce);
    // Log class initialising
    LW(LH_DEBUG, "OGL subsystem de-initialising...");
    // Reset all binds
    ResetBinds();
    // Have textures to delete?
    if(!vTextures.empty())
    { // Delete the texture handles
      LW(LH_DEBUG, "OGL deleting $ marked texture handles...",
        vTextures.size())
      DeleteMarkedTextureHandles();
      LW(LH_INFO, "OGL deleted marked texture handles.");
    } // Have fbos to delete?
    if(!vFbos.empty())
    { // Delete the fbo handles
      LW(LH_DEBUG, "OGL deleting $ marked fbo handles...", vFbos.size());
      DeleteMarkedFboHandles();
      LW(LH_INFO, "OGL deleted marked fbo handles.");
    } // Vertex buffer object created?
    if(uiVBO)
    { // Delete vertex buffer object
      LW(LH_DEBUG, "OGL deleting vertex buffer object $...", uiVBO);
      IGLL(DeleteVertexBuffer(uiVBO),
        "Failed to delete vertex buffer object!", "Index", uiVBO);
      LW(LH_INFO, "OGL deleted vertex buffer object $.", uiVBO);
      // Clear value
      uiVBO = 0;
    } // Vertex array object created?
    if(uiVAO)
    { // Delete vertex array object
      LW(LH_DEBUG, "OGL deleting vertex array object $...", uiVAO);
      IGLL(DeleteVertexArray(uiVAO),
        "Failed to delete vertex array object!", "Index", uiVAO);
      LW(LH_INFO, "OGL deleted vertex array object $.", uiVAO);
      // Clear value
      uiVAO = 0;
    } // Release opengl context
    ReleaseContext();
    // Init flags
    FlagReset(GFL_NONE);
    // Pack alignment and texture size and caches
    uiActiveFbo = uiActiveProgram = uiActiveTexture = uiActiveTUnit =
      uiActiveVao = uiActiveVbo = numeric_limits<GLuint>::max();
    uiPackAlign = uiTexUnits = uiMaxVertexAttribs = 0;
    // Reset viewport co-ordinates
    ResetCoords();
    // Set blank generic text for strings
    strVendor.clear();
    strVersion.clear();
    strRenderer.clear();
    // Log class initialising
    LW(LH_INFO, "OGL subsystem de-initialised.");
  }
  /* -- Constructor -------------------------------------------------------- */
  Ogl(void) :
    /* -- Initialisation of members ---------------------------------------- */
    IHelper{ __FUNCTION__ },           // Send name to InitHelper
    OglFlags{ GFL_NONE },              // Set no flags
    uiActiveFbo(                       // Select back buffer
      numeric_limits<GLuint>::max()),  // Maxed so values commit properly
    uiActiveProgram(uiActiveFbo),      // No active shader programme
    uiActiveTexture(uiActiveFbo),      // No active texture
    uiActiveTUnit(uiActiveFbo),        // No active texture unit
    uiActiveVao(uiActiveFbo),          // No active vertex array object
    uiActiveVbo(uiActiveFbo),          // No active vertex buffer object
    uiTexSize(0),                      // No maximum texture size
    uiPackAlign(0),                    // No pack align
    uiUnpackAlign(0),                  // No unpack align
    iUnpackRowLength(0),               // No unpack row length
    uiMaxVertexAttribs(0),             // No maximum vertex attributes
    uiTexUnits(0),                     // No maximum texture units
    qwMinVRAM(0),                      // No minimum vram
    qwTotalVRAM(0),                    // No total vram
    qwFreeVRAM(0),                     // No free vram
    imOGLCodes{{                       // Init error codes
      IDMAPSTR(GL_NO_ERROR),           IDMAPSTR(GL_INVALID_ENUM),
      IDMAPSTR(GL_INVALID_VALUE),      IDMAPSTR(GL_INVALID_OPERATION),
#ifndef __APPLE__
      IDMAPSTR(GL_STACK_OVERFLOW),     IDMAPSTR(GL_STACK_UNDERFLOW),
#endif
      IDMAPSTR(GL_OUT_OF_MEMORY)
    }, "GL_UNKNOWN" },
    uiVAO(0),                          // No default vertex array object
    uiVBO(0)                           // No default vertex buffer object
    /* -- No code ---------------------------------------------------------- */
    { }
  /* -- Destructor --------------------------------------------------------- */
  DTORHELPER(~Ogl, DeInit(true));
  /* -- Undefines ---------------------------------------------------------- */
  #undef IGLC                          // This macro was only for this class
  #undef IGL                           // This macro was only for this class
  #undef IGLL                          // This macro was only for this class
  /* ----------------------------------------------------------------------- */
} *cOgl = nullptr;                     // Pointer to static class
/* -- End of module namespace ---------------------------------------------- */
};                                     // End of interface
/* == EoF =========================================================== EoF == */

/* == SHADER.HPP =========================================================== */
/* ######################################################################### */
/* ## MS-ENGINE              Copyright (c) MS-Design, All Rights Reserved ## */
/* ######################################################################### */
/* ## This allows programs and shaders to be easily compiled, linked and  ## */
/* ## used.                                                               ## */
/* ######################################################################### */
/* ========================================================================= */
#pragma once                           // Only one incursion allowed
/* ------------------------------------------------------------------------- */
namespace IfShader {                   // Start of module namespace
/* -- Includes ------------------------------------------------------------- */
using namespace IfOgl;                 // Using ogl namespace
/* -- Public typedefs ------------------------------------------------------ */
enum ShaderUniformId {                 // Mandatory uniforms
  /* ----------------------------------------------------------------------- */
  U_ORTHO,                             // Ortho uniform vec4
  U_PALETTE,                           // Palette uniform vec4
  /* ----------------------------------------------------------------------- */
  U_MAX                                // Max no of mandatory uniforms
};/* ----------------------------------------------------------------------- */
enum ShaderAttributeId {               // Mandatory attributes
  /* ----------------------------------------------------------------------- */
  A_COORD,                             // TexCoord attribute vec2 array
  A_VERTEX,                            // Vertex attribute vec2 array
  A_COLOUR,                            // Colour attribute vec4 array
  /* ----------------------------------------------------------------------- */
  A_MAX                                // Max no of mandatory attributes
};/* ----------------------------------------------------------------------- */
typedef array<const string, 5> RoundList;
/* ------------------------------------------------------------------------- */
BEGIN_COLLECTOREX(Shaders, Shader, CLHelperUnsafe,
  /* ----------------------------------------------------------------------- */
  const RoundList  rList;              // Rounding strings list
  string           strSPRMethod;,      // Rounding method
);/* -- Shader list class -------------------------------------------------- */
class ShaderCell :                     // Members initially private
  /* -- Initialisers ------------------------------------------------------- */
  public Ident                         // Name of string
{ /* -- Private variables -------------------------------------------------- */
  const string     strCode;            // Shader name and code
  const GLenum     eType;              // Shader type
  const GLuint     uiShader;           // Created shader id
  /* -- Return code of shader -------------------------------------- */ public:
  const string &GetCode(void) const { return strCode; }
  /* -- Return length of shader code --------------------------------------- */
  size_t GetCodeLength(void) const { return GetCode().length(); }
  /* -- Return name of shader as C-String ---------------------------------- */
  const char *GetCodeCStr(void) const { return GetCode().c_str(); }
  /* -- Return type of shader ---------------------------------------------- */
  GLenum GetType(void) const { return eType; }
  /* -- Return shader id --------------------------------------------------- */
  GLuint GetHandle(void) const { return uiShader; }
  /* -- Default constructor ------------------------------------------------ */
  ShaderCell(const string &sNName,     // Specified new identifier
             const string &sNCode,     // Specified code to copmile
             const GLenum eNType,      // Specified GL type id of code
             const GLuint uiNShader) : // Specified GL shader id of code
    /* -- Initialisers ----------------------------------------------------- */
    Ident{ sNName },                   // Set specified name
    strCode{ sNCode },                 // Set code
    eType{ eNType },                   // Set type of shader
    uiShader{ uiNShader }              // Set shader handle
    /* -- No code ---------------------------------------------------------- */
    { }
};/* ----------------------------------------------------------------------- */
typedef list<ShaderCell> ShaderList;   // Shader cell list
/* ------------------------------------------------------------------------- */
BEGIN_MEMBERCLASS(Shaders, Shader, ICHelperUnsafe),
  /* -- Base classes ------------------------------------------------------- */
  public Lockable,                     // Lua garbage collector instruction
  public ShaderList                    // List of shader data in this program
{ /* -- Private typedefs --------------------------------------------------- */
  typedef array<GLint, U_MAX> UniList; // Uniform array list
  /* -- Private variables -------------------------------------------------- */
  GLuint           uiProgram;          // Shader program id
  UniList          aUniforms;          // Ids of mandatory uniforms we need
  bool             bLinked;            // Shader program has been linked
  /* -- Get program id number -------------------------------------- */ public:
  GLuint GetProgramId(void) const { return uiProgram; }
  /* -- SHader is linked? -------------------------------------------------- */
  bool IsLinked(void) const { return bLinked; }
  /* -- Verify the specified attribute is at the specified location -------- */
  void VerifyAttribLocation(const char *cpAttr, const GLuint uiI)
  { // Get attribute location
    GL(cOgl->BindAttribLocation(uiProgram, uiI, cpAttr),
      "Failed to get attribute location from shader!",
      "Attrib", cpAttr, "Program", uiProgram, "Index", uiI);
    // Enable the vertex attrib array. Keep an eye on this if you have problems
    // with glVertexAttribPointer. You'll have to restore Enable/Disable vertex
    // attrib pointers before both glDrawArrays calls if you add more shaders
    // and this call fails.
    GL(cOgl->EnableVertexAttribArray(uiI),
      "Failed to enable vertex attrib array!",
      "Attrib", cpAttr, "Program", uiProgram, "Index", uiI);
    // Report location in log
    cLog->LogDebugExSafe("Shader bound attribute '$' at location $.",
      cpAttr, uiI);
  }
  /* -- Verify the specified uniform is at the specified location ---------- */
  void VerifyUniformLocation(const char *cpUni,const size_t stI)
  { // Get attribute location
    GL(aUniforms[stI] = GetUniformLocation(cpUni),
      "Failed to get uniform location from shader!",
      "Uniform", cpUni, "Program", uiProgram, "Assign", stI);
    // Report location in log
    cLog->LogDebugExSafe("Shader attribute for '$' at location $ and index $.",
      cpUni, aUniforms[stI], stI);
  }
  /* -- Update palette ----------------------------------------------------- */
  void UpdatePalette(const size_t stSize, const GLfloat*const fpData) const
  { // Activate shader (no error checking)
    cOgl->UseProgram(GetProgram());
    // Commit palette
    cOgl->Uniform(GetUID(U_PALETTE), static_cast<GLsizei>(stSize), fpData);
  }
  /* -- Update shader ortho ------------------------------------------------ */
  void UpdateOrtho(const FboRenderItem &foiRef) const
  { // Activate shader (no error checking)
    cOgl->UseProgram(GetProgram());
    // Commit ortho bounds (no error checking)
    cOgl->Uniform(GetUID(U_ORTHO), foiRef.GetCoLeft(),
      foiRef.GetCoTop(), foiRef.GetCoRight(), foiRef.GetCoBottom());
  }
  /* -- Linkage ------------------------------------------------------------ */
  void Link(void)
  { // We want to make things simple and make sure every shader has a specific
    // attribute at a specific index so make sure these are in the correct
    // positions. This has to be done before the link.
    VerifyAttribLocation("texcoord", A_COORD);
    VerifyAttribLocation("vertex", A_VERTEX);
    VerifyAttribLocation("colour", A_COLOUR);
    // Do the link
    GL(cOgl->LinkProgram(uiProgram), "Link shader program failed!",
      "Program", uiProgram);
    // Get link result and show reason if failed
    if(cOgl->GetLinkStatus(uiProgram) == GL_FALSE)
      XC("Shader program linkage failed!", "Program", uiProgram,
        "Reason", cOgl->GetLinkFailureReason(uiProgram));
    // Program linked
    bLinked = true;
    // Activate
    Activate();
    // Verify uniforms are in the correct position
    VerifyUniformLocation("ortho", U_ORTHO);
  }
  /* -- Activate program --------------------------------------------------- */
  void Activate(void)
  { // Do activate the program
    GL(cOgl->UseProgram(uiProgram), "Failed to select shader program!",
      "Program", uiProgram);
  }
  /* -- Deselect program --------------------------------------------------- */
  void Deactivate(void)
    { if(cOgl->GetProgram() == uiProgram)
        GLL(cOgl->UseProgram(0), "Failed to deselect active program!",
          "Program", uiProgram); }
  /* -- Get shader program name -------------------------------------------- */
  GLuint GetProgram(void) const { return uiProgram; }
  /* -- Uniform value ------------------------------------------------------ */
  GLint GetUID(const size_t stI) const { return aUniforms[stI]; }
  /* -- Variable location -------------------------------------------------- */
  GLint GetUniformLocation(const char*const cpVar) const
    { return cOgl->GetUniformLocation(uiProgram, cpVar); }
  /* -- Shader initialiser ------------------------------------------------- */
  void AddShader(const string &strName, const GLenum eT, const string &strC)
  { // We need to deinit if we already linked
    if(bLinked) DeInit();
    // Create shader item and add it to list. We'll push it now so the
    // destructor can clean up any created data.
    const size_t stIndex = size();
    const ShaderCell &scItem = *insert(cend(), { strName,
      Append("#version 150\n", strC), eT, cOgl->CreateShader(eT) });
    // Check the shader
    if(!scItem.GetHandle())
      XC("Failed to create GL shader!",
         "Identifier", scItem.IdentGet(), "Type", scItem.GetType());
    // Set shader source code
    GL(cOgl->ShaderSource(scItem.GetHandle(), scItem.GetCodeCStr()),
      "Failed to set shader source code!",
        "Identifier", scItem.IdentGet(),  "Type",   scItem.GetType(),
        "Shader",     scItem.GetHandle(), "Source", scItem.GetCode());
    // Compile the shader source code
    GL(cOgl->CompileShader(scItem.GetHandle()),
      "Failed to compile shader source code!",
        "Type", eT, "Shader", scItem.GetHandle(), "Source", scItem.GetCode());
    // Get compiler result and show reason if failed
    if(cOgl->GetCompileStatus(scItem.GetHandle()) == GL_FALSE)
      XC("Shader compilation failed!",
        "Identifier", scItem.IdentGet(), "Program", uiProgram,
        "Type",       scItem.GetType(),  "Shader",  scItem.GetHandle(),
        "Reason",     cOgl->GetCompileFailureReason(scItem.GetHandle()));
    // If the program hasn't been setup yet?
    if(!uiProgram)
    { // Create the shader program, and if failed? We should bail out.
      uiProgram = cOgl->CreateProgram();
      if(!uiProgram) XC("Failed to create shader program!",
        "Identifier", scItem.IdentGet(),
        "Type",       scItem.GetType(), "Shader", scItem.GetHandle());
    } // Attach the shader to the program
    GL(cOgl->AttachShader(uiProgram, scItem.GetHandle()),
      "Failed to attach shader to program!",
        "Identifier", scItem.IdentGet(), "Program", uiProgram,
        "Type",       scItem.GetType(),  "Shader",  scItem.GetHandle());
    // Shader compiled
    cLog->LogDebugExSafe("Shader '$'($) compiled at index $ on program $.",
      scItem.IdentGet(), scItem.GetHandle(), stIndex, uiProgram);
  }
  /* -- Shader initialiser helper ------------------------------------------ */
  template<typename ...VarArgs>
    void AddShaderEx(const string &strName, const GLenum eT,
      const char*const cpFormat, const VarArgs& ...vaArgs)
  { AddShader(strName, eT, Format(cpFormat, vaArgs...)); }
  /* -- Add vertex shader with template and extra code --------------------- */
  void AddVertexShaderWith3DTemplate(const string &strName,
    const char*const cpCode)
  { // Add vertex shader program
    AddShaderEx(strName, GL_VERTEX_SHADER,
      // > The vertex shader is for modifying vertice coord data
      // Input parameters           (ONE TRIANGLE)  V1     V2     V3
      "in vec2 texcoord;"              // [3][2]=( {xy},  {xy},  {xy} )
      "in vec2 vertex;"                // [3][2]=( {xy},  {xy},  {xy} )
      "in vec4 colour;"                // [3][4]=({rgba},{rgba},{rgba})
      "out vec4 texcoordout;"          // Texcoords sent to frag shader
      "out vec4 colourout;"            // Colour multiplier sent to frag shader
      "uniform vec4 ortho;"            // Current ortho
      "void main(void){"               // Entry point
        "vec4 v = vec4(vertex.xy,0,1);"    // Store vertex
        "vec4 tc = vec4(texcoord.xy,0,0);" // Store texcoord
        "vec4 c = colour;"             // Store colour
        "$"                            // Custom code here
        "texcoordout = tc;"            // Set colour from glColorPointer
        "colourout = c;"               // Set colour
        "gl_Position = v;"             // Set vertex position
      "}", cpCode);
  }
  /* -- Add vertex shader with template ------------------------------------ */
  void AddVertexShaderWith3DTemplate(const string &strName)
    { AddVertexShaderWith3DTemplate(strName, cCommon->CBlank()); }
  /* -- Add fragment shader with template ---------------------------------- */
  void AddFragmentShaderWithTemplate(const string &strName,
    const char*const cpCode, const char*const cpHeader)
  { // Add fragmnet shader program
    AddShaderEx(strName, GL_FRAGMENT_SHADER,
      // > The fragment shader is for modifying actual pixel data
      // Input params    (ONE TRIANGLE, ?=spare)    V1     V2     V3
      "in vec4 texcoordout;"           // [3][4]=({xy??},{xy??},{xy??})
      "in vec4 colourout;"             // [3][4]=({rgba},{rgba},{rgba})
      "out vec4 pixel;"                // Pixel (RGBA) to set
      "uniform sampler2D tex;"                  // Input texture
      "$"                                       // Any extra header code
      "void main(void){"                        // Entry point
        "vec4 p = texture(tex,texcoordout.xy);" // Save current pixel
        "vec4 c = colourout;"                   // Save custom colour
        "$"                                     // Custom code goes here
        "pixel = p;"                            // Set actual pixel
      "}", cpHeader, cpCode);                   // Done
  }
  /* -- Add fragment shader with template ---------------------------------- */
  void AddFragmentShaderWithTemplate(const string &strName)
    { AddFragmentShaderWithTemplate(strName,
        cCommon->CBlank(), cCommon->CBlank()); }
  /* -- Add fragment shader with template ---------------------------------- */
  void AddFragmentShaderWithTemplate(const string &strName,
    const char*const cpCode)
  { AddFragmentShaderWithTemplate(strName, cpCode, cCommon->CBlank()); }
  /* -- Add vertex shader with template ------------------------------------ */
  void AddVertexShaderWith2DTemplate(const string &strName,
    const char*const cpCode)
  { // Add vertex shader program
    AddVertexShaderWith3DTemplate(strName, Format("$"
      "v[0] = -1.0+(((ortho[0]+$(v[0]))/ortho[2])*2.0);"  // X-coord
      "v[1] = -1.0+(((ortho[1]+$(v[1]))/ortho[3])*2.0);", // Y-coord
        cpCode, cShaders->strSPRMethod, cShaders->strSPRMethod).c_str());
  }
  /* -- Add vertex shader with template ------------------------------------ */
  void AddVertexShaderWith2DTemplate(const string &strName)
    { AddVertexShaderWith2DTemplate(strName, cCommon->CBlank()); }
  /* -- Detach and unlink shaders ------------------------------------------ */
  void DeInitShaders(void)
  { // Ignore if nothing in list
    if(empty()) return;
    // Until shader list is empty
    for(const ShaderCell &scItem : *this)
    { // Ignore if no shader
      if(!scItem.GetHandle()) continue;
      // If we have a program. Detach the shader if we have the program
      if(uiProgram)
        GLL(cOgl->DetachShader(uiProgram, scItem.GetHandle()),
          "Shader $ not detached from program $!",
          uiProgram, scItem.GetHandle());
      // Delete the shader
      GLL(cOgl->DeleteShader(scItem.GetHandle()),
        "Shader $ could not be deleted!", scItem.GetHandle());
    } // Done if no program
    if(!uiProgram) return;
    // Deselect program
    Deactivate();
    // Delete the program
    GLL(cOgl->DeleteProgram(uiProgram),
      "Shader program $ could not be deleted!", uiProgram);
  }
  /* -- Shader deinitialiser ----------------------------------------------- */
  void DeInit(void)
  { // Detatch, unlink and clear shaders list and program
    DeInitShaders();
    // Reset the identifier
    uiProgram = 0;
    // No longer linked
    bLinked = false;
    // Clear shader list
    clear();
  }
  /* -- Constructor -------------------------------------------------------- */
  Shader(void) :
    /* -- Initialisers ----------------------------------------------------- */
    ICHelperShader{ *cShaders, this }, // Register in Shaders list
    IdentCSlave{ cParent.CtrNext() },  // Initialise identification number
    uiProgram(0),                      // No program set
    bLinked(false)                     // Not linked
    /* -- No code ---------------------------------------------------------- */
    { }
  /* -- Destructor --------------------------------------------------------- */
  ~Shader(void) { DeInitShaders(); }
  /* ----------------------------------------------------------------------- */
  DELETECOPYCTORS(Shader)              // Disable copy constructor and operator
};/* ----------------------------------------------------------------------- */
END_COLLECTOREX(Shaders,,,,            // Finish shaders collector
  /* -- Initialisers ------------------------------------------------------- */
  rList{{                              // Initialise rounding strings list
    cCommon->Blank(),                          // [0] No rounding
    "floor",                           // [1] Floor rounding
    "ceil",                            // [2] Ceil rounding
    "round",                           // [3] Nearest whole number
    "roundEven"                        // [4] Nearest even number
  }}                                   // End of rounding strings list
);/* == Init built-in shaders ============================================== */
static void Init3DShader(Shader &sh3D)
{ // Add our basic 3D shader
  sh3D.LockSet();
  sh3D.AddVertexShaderWith3DTemplate("VERT-3D");
  sh3D.AddFragmentShaderWithTemplate("FRAG-3D RGB", "p = p * c;");
  sh3D.Link();
}
/* ========================================================================= */
static void Init2DShader(Shader &sh2D)
{ // Add our 2D to 3D transformation shader
  sh2D.LockSet();
  sh2D.AddVertexShaderWith2DTemplate("VERT-2D");
  sh2D.AddFragmentShaderWithTemplate("FRAG-2D RGB", "p = p * c;");
  sh2D.Link();
}
/* ========================================================================= */
static void Init2DBGRShader(Shader &sh2DBGR)
{ // Add our 2D to 3D BGR transformation shader
  sh2DBGR.LockSet();
  sh2DBGR.AddVertexShaderWith2DTemplate("VERT-2D");
  sh2DBGR.AddFragmentShaderWithTemplate("FRAG-2D BGR>RGB",
    "float r = p.r; p.r = p.b; p.b = r; p = p * c;");
  sh2DBGR.Link();
}
/* ========================================================================= */
static void Init2D8Shader(Shader &sh2D8)
{ // Add our 2D to 3D transformation shader with GL_LUMINANCE decoding
  sh2D8.LockSet();
  sh2D8.AddVertexShaderWith2DTemplate("VERT-2D");
  sh2D8.AddFragmentShaderWithTemplate("FRAG-2D LU>RGB",
    "p.r = p.g = p.b = p.r; p.a = 1.0; p = p * c;");
  sh2D8.Link();
}
/* ========================================================================= */
static void Init2D8PalShader(Shader &sh2D8Pal)
{ // Add our 2D to 3D transformation shader with GL_LUMINANCE decoding
  sh2D8Pal.LockSet();
  sh2D8Pal.AddVertexShaderWith2DTemplate("VERT-2D");
  sh2D8Pal.AddFragmentShaderWithTemplate("FRAG-2D PAL>RGB",
    "p = pal[int(p.r * 255)] * c;",    // Set pixel and modulate
    "uniform vec4 pal[256];");         // Global colour palette
  sh2D8Pal.Link();
  // We need the location of the palette
  sh2D8Pal.VerifyUniformLocation("pal", U_PALETTE);
}
/* ========================================================================= */
static void Init2D16Shader(Shader &sh2D16)
{ // Add our 2D to 3D transformation shader with GL_LUMINANCE_ALPHA decoding
  sh2D16.LockSet();
  sh2D16.AddVertexShaderWith2DTemplate("VERT-2D");
  sh2D16.AddFragmentShaderWithTemplate("FRAG-2D LUA>RGB",
    "p.a = p.g; p.g = p.b = p.r; p = p * c;");
  sh2D16.Link();
}
/* ========================================================================= */
static void Init3DYUVTemplate(Shader &sh3DYUV, const char *cpName,
  const char*const cpCode)
{ // Add YUV to RGB shaders
  sh3DYUV.LockSet();
  sh3DYUV.AddVertexShaderWith3DTemplate("VERT-3D");
  sh3DYUV.AddShaderEx(cpName, GL_FRAGMENT_SHADER,
    "in vec4 texcoordout;"             // Texture info
    "in vec4 colourout;"               // Colour info
    "out vec4 pixel;"                  // Pixel out
    "uniform sampler2D texY;"          // Multitex unit 0 for Y component data
    "uniform sampler2D texCb;"         // Multitex unit 1 for Cb component data
    "uniform sampler2D texCr;"         // Multitex unit 2 for Cr component data
    "void main(void){"
      "mediump vec3 yuv;"
      "lowp vec3 rgb;"
      "yuv.x = texture(texY,vec2(texcoordout)).r;"
      "yuv.y = texture(texCb,vec2(texcoordout)).r-0.5;"
      "yuv.z = texture(texCr,vec2(texcoordout)).r-0.5;"
      "rgb = mat3(1,1,1,0,-0.344,1.77,1.403,-0.714,0)*yuv;"
      "pixel = vec4(rgb,$);"
    "}", cpCode);
  sh3DYUV.Link();
  sh3DYUV.Activate();
  // For each texture unit
  static const array<const GLchar*const,3> acpCmp{ "texY", "texCb", "texCr" };
  for(size_t stIndex = 0; stIndex < acpCmp.size(); ++stIndex)
  { // Get location of specified variable in gpu shader and set to the
    // required texture unit.
    const GLchar*const cpUniform = acpCmp[stIndex];
    const GLint &iUniformId = sh3DYUV.GetUniformLocation(cpUniform);
    GLC("Failed to get uniform location from YUV shader!",
      "Variable", cpUniform, "Index", stIndex);
    GL(cOgl->Uniform(iUniformId, static_cast<GLint>(stIndex)),
      "Failed to set YUV uniform texture unit!",
      "Variable", cpUniform, "Index", stIndex, "Uniform", iUniformId);
  }
}
/* ========================================================================= */
static void Init3DYUVShader(Shader &sh3DYUV)
{ // Initialuse YUV 3D shader with basic conversion
  Init3DYUVTemplate(sh3DYUV, "FRAG-3D YUV>RGB",
    "texture(texCr,vec2(texcoordout)).a");
}
/* ========================================================================= */
static void Init3DYUVKShader(Shader &sh3DYUVK)
{ // Initialuse YUV 3D shader with colour key decoder
  Init3DYUVTemplate(sh3DYUVK, "FRAG-3D YUV>RGB>CK",
    "abs(colourout.r-rgb.r)<=colourout.a&&"
    "abs(colourout.g-rgb.g)<=colourout.a&&"
    "abs(colourout.b-rgb.b)<=colourout.a?0:1");
}
/* == Set rounding method for the shader =================================== */
static CVarReturn SetSPRoundingMethod(const size_t stMethod)
{ // Return if specified value is outrageous!
  if(stMethod >= cShaders->rList.size()) return DENY;
  cShaders->strSPRMethod = cShaders->rList[stMethod];
  // Success
  return ACCEPT;
}
/* ------------------------------------------------------------------------- */
};                                     // End of module namespace
/* == EoF =========================================================== EoF == */

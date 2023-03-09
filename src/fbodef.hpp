/* == FBODEF.HPP =========================================================== */
/* ######################################################################### */
/* ## MS-ENGINE              Copyright (c) MS-Design, All Rights Reserved ## */
/* ######################################################################### */
/* ## Definitions for the operatibility of framebuffer objects.           ## */
/* ######################################################################### */
/* ========================================================================= */
#pragma once                           // Only one incursion allowed
/* -- Module namespace ----------------------------------------------------- */
namespace IfFbo {                      // Keep declarations neatly categorised
/* -- Includes ------------------------------------------------------------- */
using namespace Library::GlFW;         // Using GLFW library functions
/* -- Defines -------------------------------------------------------------- */
#define VERTEXPERTRIANGLE            3 // Vertices used in a triangle (3 ofc!)
#define TWOTRIANGLES                   (VERTEXPERTRIANGLE * 2)
#define TRISPERQUAD                  2 // Triangles needed to make a quad
/* -- Defines for triangle texture co-ordinates data ----------------------- */
#define COMPSPERCOORD                2 // Floats used to define texcoord (XY)
#define FLOATSPERCOORD                 (VERTEXPERTRIANGLE * COMPSPERCOORD)
/* -- Defines for triangle position co-ordinates data----------------------- */
#define COMPSPERPOS                  2 // Floats used to define position (XY)
#define FLOATSPERPOS                   (VERTEXPERTRIANGLE * COMPSPERPOS)
/* -- Defines for colour intensity data ------------------------------------ */
#define COMPSPERCOLOUR               4 // Floats used to define colour (RGBA)
#define FLOATSPERCOLOUR                (VERTEXPERTRIANGLE * COMPSPERCOLOUR)
/* -- Totals --------------------------------------------------------------- */
#define FLOATSPERTRI        (FLOATSPERCOORD + FLOATSPERPOS + FLOATSPERCOLOUR)
#define FLOATSPERQUAD                  (FLOATSPERTRI * TRISPERQUAD)
/* -- OpenGL buffer structure ---------------------------------------------- */
#define FLOATSPERVERTEX (COMPSPERCOORD + COMPSPERPOS + COMPSPERCOLOUR)
#define BYTESPERVERTEX  (sizeof(GLfloat) * FLOATSPERVERTEX)
#define OFFSETTXCDATA   0
#define OFFSETPOSDATA   (sizeof(GLfloat) * COMPSPERCOORD)
#define OFFSETCOLDATA   (sizeof(GLfloat) * (COMPSPERCOORD + COMPSPERPOS))
/* -- Render command item -------------------------------------------------- */
struct FboCmd                          // Render command structure
{ /* ----------------------------------------------------------------------- */
  const GLuint       uiTUId;           // - Texture unit id
  const GLuint       uiTexId;          // - Texture id
  const GLuint       uiPrgId;          // - Shader program id
  const GLvoid*const vpTCOffset;       // - Texcoord buffer offset
  const GLvoid*const vpVOffset;        // - vector buffer offset
  const GLvoid*const vpCOffset;        // - Colour buffer offset
  const GLsizei      uiVertices;       // Total vertices to draw
};/* -- Commands ----------------------------------------------------------- */
typedef vector<FboCmd>             FboCmdList;   // Render command list
typedef FboCmdList::const_iterator FboCmdListIt; // " iterator
/* -- Single point data ---------------------------------------------------- */
typedef array<GLfloat,COMPSPERCOORD>  TriCoord;  // Coord data in triangle
typedef array<GLfloat,COMPSPERPOS>    TriVertex; // Position in triangle
typedef array<GLfloat,COMPSPERCOLOUR> TriColour; // Colour in triangle
/* -- One triangle data ---------------------------------------------------- */
struct FboVert                         // Formatted data for OpenGL
{ /* ----------------------------------------------------------------------- */
  TriCoord       faCoord;              // TexCoord specific data send
  TriVertex      faVertex;             // Vertex specific data to send
  TriColour      faColour;             // Colour specific data to send
};/* ----------------------------------------------------------------------- */
/* FboVert[0].TriCoord  =  8 bytes @ GLfloat[ 0] - Point 1 / Texcoord 1      */
/*     "     .TriVertex =  8 bytes @ GLfloat[ 8] -    "    / Vertex 1        */
/*     "     .TriColour = 16 bytes @ GLfloat[16] -    "    / Colour 1        */
/* FboVert[1].TriCoor   =  8 bytes @ GLfloat[32] - Point 2 / Texcoord 2      */
/*     "     .TriVertex =  8 bytes @ GLfloat[40] -    "    / Vertex 2        */
/*     "     .TriColour = 16 bytes @ GLfloat[48] -    "    / Colour 2        */
/* FboVert[2].TriCoord  =  8 bytes @ GLfloat[64] - Point 3 / Texcoord 3      */
/*     "     .TriVertex =  8 bytes @ GLfloat[72] -    "    / Vertex 3        */
/*     "     .TriColour = 16 bytes @ GLfloat[80] -    "    / Colour 3        */
/* +-- Single interlaced triangle --+- T(Vec2)=Texcoord(XY) -+-----+-----+-- */
/* + TTVVCCCC | TTVVCCCC | TTVVCCCC |  V(Vec2)=Vertex(XY)    | ... | ... |   */
/* +----------+----------+----------+- C(Vec4)=Colour(RGBA) -+-----+-----+-- */
typedef array<FboVert,VERTEXPERTRIANGLE> FboTri;     // All triangles data
typedef vector<FboTri>                   FboTriList; // Render triangles list
/* == Fbo colour class ===================================================== */
class FboColour                        // Do not inherit array<GLfloat,4>
{ /* -------------------------------------------------------------- */ private:
  array<GLfloat,4> aColour;            // Colour union
  /* --------------------------------------------------------------- */ public:
  GLfloat GetColourRed(void) const { return aColour[0]; }
  GLfloat GetColourGreen(void) const { return aColour[1]; }
  GLfloat GetColourBlue(void) const { return aColour[2]; }
  GLfloat GetColourAlpha(void) const { return aColour[3]; }
  GLfloat *GetColourMemory(void) { return aColour.data(); }
  /* ----------------------------------------------------------------------- */
  void SetColourRed(const GLfloat fR) { aColour[0] = fR; }
  void SetColourGreen(const GLfloat fG) { aColour[1] = fG; }
  void SetColourBlue(const GLfloat fB) { aColour[2] = fB; }
  void SetColourAlpha(const GLfloat fA) { aColour[3] = fA; }
  /* ----------------------------------------------------------------------- */
  void SetColourInt(const unsigned int uiValue)
  { // Need utility namespace for Normalise();
    using IfUtil::Normalise;
    // Strip bits and send to proper clear colour
    SetColourRed(Normalise<GLfloat,16>(uiValue));
    SetColourGreen(Normalise<GLfloat,8>(uiValue));
    SetColourBlue(Normalise<GLfloat>(uiValue));
    SetColourAlpha(Normalise<GLfloat,24>(uiValue));
  }
  /* ----------------------------------------------------------------------- */
  void SetColourRed(const FboColour &cO)
    { SetColourRed(cO.GetColourRed()); }
  void SetColourGreen(const FboColour &cO)
    { SetColourGreen(cO.GetColourGreen()); }
  void SetColourBlue(const FboColour &cO)
    { SetColourBlue(cO.GetColourBlue()); }
  void SetColourAlpha(const FboColour &cO)
    { SetColourAlpha(cO.GetColourAlpha()); }
  /* ----------------------------------------------------------------------- */
  void ResetColour(void) { aColour.fill(-1.0f); }
  /* ----------------------------------------------------------------------- */
  bool RedColourNotEqual(const FboColour &cO) const
    { return GetColourRed() != cO.GetColourRed(); }
  bool GreenColourNotEqual(const FboColour &cO) const
    { return GetColourGreen() != cO.GetColourGreen(); }
  bool BlueColourNotEqual(const FboColour &cO) const
    { return GetColourBlue() != cO.GetColourBlue(); }
  bool AlphaColourNotEqual(const FboColour &cO) const
    { return GetColourAlpha() != cO.GetColourAlpha(); }
  /* ----------------------------------------------------------------------- */
  bool SetColour(const FboColour &fcData)
  { // Red clear colour change?
    if(RedColourNotEqual(fcData))
    { // Update saved red value and other values if changed
      SetColourRed(fcData);
      if(GreenColourNotEqual(fcData)) SetColourGreen(fcData);
      if(BlueColourNotEqual(fcData)) SetColourBlue(fcData);
      if(AlphaColourNotEqual(fcData)) SetColourAlpha(fcData);
    } // Green clear colour changed?
    else if(GreenColourNotEqual(fcData))
    { // Update saved green value and other values if changed
      SetColourGreen(fcData);
      if(BlueColourNotEqual(fcData)) SetColourBlue(fcData);
      if(AlphaColourNotEqual(fcData)) SetColourAlpha(fcData);
    } // Blue clear colour changed?
    else if(BlueColourNotEqual(fcData))
    { // Update saved blue value and other values if changed
      SetColourBlue(fcData);
      if(AlphaColourNotEqual(fcData)) SetColourAlpha(fcData);
    } // Update alpha if cahnged
    else if(AlphaColourNotEqual(fcData)) SetColourAlpha(fcData);
    // No value was changed so return
    else return false;
    // Commit the new viewport
    return true;
  }
  /* -- Init constructor --------------------------------------------------- */
  FboColour(const GLfloat fR, const GLfloat fG, const GLfloat fB,
    const GLfloat fA) :
    /* -- Initialisation of members ---------------------------------------- */
    aColour{fR, fG, fB, fA}            // Copy intensity over
    /* -- No code ---------------------------------------------------------- */
    { }
  /* -- Default constructor ------------------------------------------------ */
  FboColour(void) :
    /* -- Initialisation of members ---------------------------------------- */
    FboColour{ 1.0f,1.0f,1.0f,1.0f }   // Set full Re/Gr/Bl/Alpha intensity
    /* -- No code ---------------------------------------------------------- */
    { }
};/* ----------------------------------------------------------------------- */
/* == Fbo blend class ====================================================== */
class FboBlend
{ /* -- Private variables ----------------------------------------- */ private:
  array<GLenum,4>  aBlend;             // Blend union
  /* --------------------------------------------------------------- */ public:
  GLenum GetSrcRGB(void) const { return aBlend[0]; }
  GLenum GetDstRGB(void) const { return aBlend[1]; }
  GLenum GetSrcAlpha(void) const { return aBlend[2]; }
  GLenum GetDstAlpha(void) const { return aBlend[3]; }
  GLenum *GetMemory(void) { return aBlend.data(); }
  /* ----------------------------------------------------------------------- */
  void SetSrcRGB(const GLenum eSrcRGB) { aBlend[0] = eSrcRGB; }
  void SetDstRGB(const GLenum eDstRGB) { aBlend[1] = eDstRGB; }
  void SetSrcAlpha(const GLenum eSrcAlpha) { aBlend[2] = eSrcAlpha; }
  void SetDstAlpha(const GLenum eDstAlpha) { aBlend[3] = eDstAlpha; }
  /* ----------------------------------------------------------------------- */
  void SetSrcRGB(const FboBlend &fbOther)
    { SetSrcRGB(fbOther.GetSrcRGB()); }
  void SetDstRGB(const FboBlend &fbOther)
    { SetDstRGB(fbOther.GetDstRGB()); }
  void SetSrcAlpha(const FboBlend &fbOther)
    { SetSrcAlpha(fbOther.GetSrcAlpha()); }
  void SetDstAlpha(const FboBlend &fbOther)
    { SetDstAlpha(fbOther.GetDstAlpha()); }
  /* ----------------------------------------------------------------------- */
  bool IsSrcRGBNotEqual(const FboBlend &cO) const
    { return GetSrcRGB() != cO.GetSrcRGB(); }
  bool IsDstRGBNotEqual(const FboBlend &cO) const
    { return GetDstRGB() != cO.GetDstRGB(); }
  bool IsSrcAlphaNotEqual(const FboBlend &cO) const
    { return GetSrcAlpha() != cO.GetSrcAlpha(); }
  bool IsDstAlphaNotEqual(const FboBlend &cO) const
    { return GetDstAlpha() != cO.GetDstAlpha(); }
  /* -- Set blending algorithms -------------------------------------------- */
  bool SetBlend(const FboBlend &fbOther)
  {  // Source RGB changed change?
    if(IsSrcRGBNotEqual(fbOther))
    { // Update source RGB blend value and other values if changed
      SetSrcRGB(fbOther);
      if(IsDstRGBNotEqual(fbOther)) SetDstRGB(fbOther);
      if(IsSrcAlphaNotEqual(fbOther)) SetSrcAlpha(fbOther);
      if(IsDstAlphaNotEqual(fbOther)) SetDstAlpha(fbOther);
    } // Destination RGB blend changed?
    else if(IsDstRGBNotEqual(fbOther))
    { // Update destination RGB blend value and other values if changed
      SetDstRGB(fbOther);
      if(IsSrcAlphaNotEqual(fbOther)) SetSrcAlpha(fbOther);
      if(IsDstAlphaNotEqual(fbOther)) SetDstAlpha(fbOther);
    } // Source alpha changed?
    else if(IsSrcAlphaNotEqual(fbOther))
    { // Update source alpha blend value and other values if changed
      SetSrcAlpha(fbOther);
      if(IsDstAlphaNotEqual(fbOther)) SetDstAlpha(fbOther);
    } // Destination alpha changed?
    else if(IsDstAlphaNotEqual(fbOther)) SetDstAlpha(fbOther);
    // No value was changed so return
    else return false;
    // Commit the new viewport
    return true;
  }
  /* -- Init constructor --------------------------------------------------- */
  FboBlend(const GLenum eSrcRGB, const GLenum eDstRGB, const GLenum eSrcAlpha,
    const GLenum eDstAlpha) :
    /* -- Initialisation of members ---------------------------------------- */
    aBlend{ eSrcRGB,                   // Copy blend source RGB
            eDstRGB,                   // Copy blend destination RGB
            eSrcAlpha,                 // Copy blend source Alpha
            eDstAlpha }                // Copy blend destination Alpha
    /* -- No code ---------------------------------------------------------- */
    { }
  /* -- Default constructor ------------------------------------------------ */
  FboBlend(void) :
    /* -- Initialisation of members ---------------------------------------- */
    FboBlend{ GL_SRC_ALPHA,            // Init blend source RGB
              GL_ONE_MINUS_SRC_ALPHA,  // Init blend destination RGB
              GL_ONE,                  // Init blend source Alpha
              GL_ONE_MINUS_SRC_ALPHA } // Init blend destination Alpha
    /* -- No code ---------------------------------------------------------- */
    { }
};/* ----------------------------------------------------------------------- */
/* == Fbo coords class ===================================================== */
template<typename Type1 = GLfloat, typename Type2 = Type1>class FboCoords
{ /* -- Private variables ----------------------------------------- */ private:
  Type1            tLeft,  tTop;       // Top left co-ordinates
  Type2            tRight, tBottom;    // Width and height
  /* --------------------------------------------------------------- */ public:
  Type1 GetCoLeft(void) const { return tLeft; }
  Type1 GetCoTop(void) const { return tTop; }
  Type2 GetCoRight(void) const { return tRight; }
  Type2 GetCoBottom(void) const { return tBottom; }
  /* ----------------------------------------------------------------------- */
  void SetCoLeft(const Type1 tNLeft) { tLeft = tNLeft; }
  void SetCoTop(const Type1 tNTop) { tTop = tNTop; }
  void SetCoRight(const Type2 tNRight) { tRight = tNRight; }
  void SetCoBottom(const Type2 tNBottom) { tBottom = tNBottom; }
  /* ----------------------------------------------------------------------- */
  void SetCoLeft(const FboCoords &fvOther)
    { SetCoLeft(fvOther.GetCoLeft()); }
  void SetCoTop(const FboCoords &fvOther)
    { SetCoTop(fvOther.GetCoTop()); }
  void SetCoRight(const FboCoords &fvOther)
    { SetCoRight(fvOther.GetCoRight()); }
  void SetCoBottom(const FboCoords &fvOther)
    { SetCoBottom(fvOther.GetCoBottom()); }
  /* ----------------------------------------------------------------------- */
  void ResetCoords(void) { tLeft = tTop = 0, tRight = tBottom = 0; }
  /* ----------------------------------------------------------------------- */
  bool SetCoords(const Type1 tNLeft, const Type1 tNTop,
                 const Type2 tNRight, const Type2 tNBottom)
  { // Viewport X origin different from cached?
    if(GetCoLeft() != tNLeft)
    { // Update viewport X value and other values if changed
      SetCoLeft(tNLeft);
      if(GetCoTop() != tNTop) SetCoTop(tNTop);
      if(GetCoRight() != tNRight) SetCoRight(tNRight);
      if(GetCoBottom() != tNBottom) SetCoBottom(tNBottom);
    } // Viewport Y origin different from cached?
    else if(GetCoTop() != tNTop)
    { // Update viewport Y value and other values if changed
      SetCoTop(tNTop);
      if(GetCoRight() != tNRight) SetCoRight(tNRight);
      if(GetCoBottom() != tNBottom) SetCoBottom(tNBottom);
    } // Viewport width different from cached?
    else if(GetCoRight() != tNRight)
    { // Update viewport width and other values if changed
      SetCoRight(tNRight);
      if(GetCoBottom() != tNBottom) SetCoBottom(tNBottom);
    } // Viewport height different from cached? Update it
    else if(GetCoBottom() != tNBottom) SetCoBottom(tNBottom);
    // No value was changed so return
    else return false;
    // Commit the new viewport
    return true;
  }
  /* ----------------------------------------------------------------------- */
  bool SetCoords(const FboCoords &fcData)
    { return SetCoords(fcData.GetCoLeft(), fcData.GetCoTop(),
                       fcData.GetCoRight(), fcData.GetCoBottom()); }
  /* -- Init constructor --------------------------------------------------- */
  FboCoords(const Type1 tNLeft, const Type1 tNTop, const Type2 tNRight,
    const Type2 tNBottom) :
    /* -- Initialisation of members ---------------------------------------- */
    tLeft(tNLeft),                     // X1 co-ordinate
    tTop(tNTop),                       // Y1 co-ordinate
    tRight(tNRight),                   // X2 co-ordinate
    tBottom(tNBottom)                  // Y2 co-ordinate
    /* --------------------------------------------------------------------- */
    { }
  /* -- Default constructor ------------------------------------------------ */
  FboCoords(void) :
    /* -- Initialisation of members ---------------------------------------- */
    FboCoords{ 0, 0, 0, 0 }            // Initialise co-ordinates
    /* --------------------------------------------------------------------- */
    { }
};/* ----------------------------------------------------------------------- */
typedef FboCoords<GLint,GLsizei> FboViewport; // Viewport co-ordinates
/* == Data required to complete a render of an fbo ========================= */
struct FboRenderItem :                 // Rendering item data class
  /* -- Base classes ------------------------------------------------------- */
  public FboColour,                    // Clear colour of the fbo
  public FboBlend,                     // Blend mode of the fbo
  public FboCoords<GLfloat>            // Ortho co-ordinates of the fbo
{ /* ----------------------------------------------------------------------- */
  GLuint           uiFBO;              // Fbo name
  bool             bClear;             // Clear the fbo?
  GLsizei          stFBOWidth,         // Frame buffer width as float.
                   stFBOHeight;        // Frame buffer height as float.
  /* -- Default constructor ------------------------------------------------ */
  FboRenderItem(void) :
    /* -- Initialisation of members ---------------------------------------- */
    uiFBO(0),                          // No fbo id yet
    bClear(true),                      // Clear fbo set
    stFBOWidth(0),                     // No width
    stFBOHeight(0)                     // No height
    /* --------------------------------------------------------------------- */
    { }
};/* ----------------------------------------------------------------------- */
/* -- End of module namespace ---------------------------------------------- */
};                                     // End of interface
/* == EoF =========================================================== EoF == */

/* == FBODEF.HPP =========================================================== */
/* ######################################################################### */
/* ## MS-ENGINE              Copyright (c) MS-Design, All Rights Reserved ## */
/* ######################################################################### */
/* ## Definitions for the operatibility of framebuffer objects.           ## */
/* ######################################################################### */
/* ========================================================================= */
#pragma once                           // Only one incursion allowed
/* ------------------------------------------------------------------------- */
namespace IfFbo {                      // Start of module namespace
/* -- Includes ------------------------------------------------------------- */
using namespace Lib::OS::GlFW;         // Using GLFW library functions
using namespace IfDim;                 // Using dimensions namespace
using namespace IfUtil;                // Using utility namespace
/* -- Defines -------------------------------------------------------------- */
constexpr static const size_t
  /* -- Defines to describe a simple triangle  ----------------------------- */
  stVertexPerTriangle = 3,             // Vertices used in a triangle (3 ofc!)
  stTwoTriangles      = (stVertexPerTriangle * 2),
  stTrisPerQuad       = 2,             // Triangles needed to make a quad
  /* -- Defines for triangle texture co-ordinates data --------------------- */
  stCompsPerCoord  = 2,                // Floats used to define texcoord (XY)
  stFloatsPerCoord = (stVertexPerTriangle * stCompsPerCoord),
  /* -- Defines for triangle position co-ordinates data--------------------- */
  stCompsPerPos  = 2,                  // Floats used to define position (XY)
  stFloatsPerPos = (stVertexPerTriangle * stCompsPerPos),
  /* -- Defines for colour intensity data ---------------------------------- */
  stCompsPerColour  = 4,               // Floats used to define colour (RGBA)
  stFloatsPerColour = (stVertexPerTriangle * stCompsPerColour),
  /* -- Totals ------------------------------------------------------------- */
  stFloatsPerTri  = (stFloatsPerCoord + stFloatsPerPos + stFloatsPerColour),
  stFloatsPerQuad = (stFloatsPerTri * stTrisPerQuad),
  /* -- OpenGL buffer structure -------------------------------------------- */
  stFloatsPerVertex = (stCompsPerCoord + stCompsPerPos + stCompsPerColour),
  stBytesPerVertex  = (sizeof(GLfloat) * stFloatsPerVertex),
  stOffsetTxcData   = 0,
  stOffsetPosData   = (sizeof(GLfloat) * stCompsPerCoord),
  stOffsetColData   = (sizeof(GLfloat) * (stCompsPerCoord + stCompsPerPos));
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
typedef array<GLfloat,stCompsPerCoord>  TriCoord;  // Coord data in triangle
typedef array<GLfloat,stCompsPerPos>    TriVertex; // Position in triangle
typedef array<GLfloat,stCompsPerColour> TriColour; // Colour in triangle
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
typedef array<FboVert,stVertexPerTriangle> FboTri;     // All triangles data
typedef vector<FboTri>                   FboTriList; // Render triangles list
/* == Fbo colour class ===================================================== */
class FboColour                        // Members initially private
{ /* -- Private typedefs --------------------------------------------------- */
  typedef array<GLfloat,4> FboRGBA;    // Array of RGBA floats
  FboRGBA           fboRGBA;           // Red + Green + Blue + Alpha values
  /* ----------------------------------------------------------------------- */
  const FboRGBA &GetColourConst(void) const { return fboRGBA; }
  FboRGBA &GetColour(void) { return fboRGBA; }
  /* --------------------------------------------------------------- */ public:
  GLfloat GetColourRed(void) const { return GetColourConst()[0]; }
  GLfloat GetColourGreen(void) const { return GetColourConst()[1]; }
  GLfloat GetColourBlue(void) const { return GetColourConst()[2]; }
  GLfloat GetColourAlpha(void) const { return GetColourConst()[3]; }
  GLfloat *GetColourMemory(void) { return GetColour().data(); }
  /* ----------------------------------------------------------------------- */
  void SetColourRed(const GLfloat fR) { GetColour()[0] = fR; }
  void SetColourGreen(const GLfloat fG) { GetColour()[1] = fG; }
  void SetColourBlue(const GLfloat fB) { GetColour()[2] = fB; }
  void SetColourAlpha(const GLfloat fA) { GetColour()[3] = fA; }
  /* ----------------------------------------------------------------------- */
  void SetColourRedInt(const unsigned int uiR)
    { SetColourRed(NormaliseEx<GLfloat>(uiR)); }
  void SetColourGreenInt(const unsigned int uiG)
    { SetColourGreen(NormaliseEx<GLfloat>(uiG)); }
  void SetColourBlueInt(const unsigned int uiB)
    { SetColourBlue(NormaliseEx<GLfloat>(uiB)); }
  void SetColourAlphaInt(const unsigned int uiA)
    { SetColourAlpha(NormaliseEx<GLfloat>(uiA)); }
  /* ----------------------------------------------------------------------- */
  void SetColourInt(const unsigned int uiValue)
  { // Strip bits and send to proper clear colour
    SetColourRed(NormaliseEx<GLfloat,16>(uiValue));
    SetColourGreen(NormaliseEx<GLfloat,8>(uiValue));
    SetColourBlue(NormaliseEx<GLfloat>(uiValue));
    SetColourAlpha(NormaliseEx<GLfloat,24>(uiValue));
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
  void ResetColour(void) { GetColour().fill(-1.0f); }
  /* ----------------------------------------------------------------------- */
  bool RedColourNotEqual(const FboColour &cO) const
    { return IsFloatNotEqual(GetColourRed(), cO.GetColourRed()); }
  bool GreenColourNotEqual(const FboColour &cO) const
    { return IsFloatNotEqual(GetColourGreen(), cO.GetColourGreen()); }
  bool BlueColourNotEqual(const FboColour &cO) const
    { return IsFloatNotEqual(GetColourBlue(), cO.GetColourBlue()); }
  bool AlphaColourNotEqual(const FboColour &cO) const
    { return IsFloatNotEqual(GetColourAlpha(), cO.GetColourAlpha()); }
  /* ----------------------------------------------------------------------- */
  template<typename IntType,
    class ArrayType=array<IntType,
      sizeof(FboRGBA)/sizeof(GLfloat)>>
    const ArrayType Cast(void) const
  { return { Denormalise<IntType>(GetColourRed()),
             Denormalise<IntType>(GetColourGreen()),
             Denormalise<IntType>(GetColourBlue()),
             Denormalise<IntType>(GetColourAlpha()) }; }
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
  /* -- Init constructor with RGBA ints ------------------------------------ */
  FboColour(const unsigned int uiR, const unsigned int uiG,
            const unsigned int uiB, const unsigned int uiA) :
    /* -- Initialisers ----------------------------------------------------- */
    fboRGBA{NormaliseEx<GLfloat>(uiR), // Copy and normalise red component
            NormaliseEx<GLfloat>(uiG), // Copy and normalise green component
            NormaliseEx<GLfloat>(uiB), // Copy and normalise blue component
            NormaliseEx<GLfloat>(uiA)} // Copy and normalise alpha component
    /* -- No code ---------------------------------------------------------- */
    { }
  /* -- Init constructor with RGB ints ------------------------------------- */
  FboColour(const unsigned int uiR, const unsigned int uiG,
            const unsigned int uiB) :
    /* -- Initialisers ----------------------------------------------------- */
    fboRGBA{NormaliseEx<GLfloat>(uiR), // Copy and normalise red component
            NormaliseEx<GLfloat>(uiG), // Copy and normalise green component
            NormaliseEx<GLfloat>(uiB), // Copy and normalise blue component
            1.0f }                     // Opaque alpha
    /* -- No code ---------------------------------------------------------- */
    { }
  /* -- Init constructor with RGBA bytes ----------------------------------- */
  FboColour(const uint8_t ucR, const uint8_t ucG,
            const uint8_t ucB, const uint8_t ucA) :
    /* -- Initialisers ----------------------------------------------------- */
    fboRGBA{Normalise<GLfloat>(ucR),   // Copy and normalise red component
            Normalise<GLfloat>(ucG),   // Copy and normalise green component
            Normalise<GLfloat>(ucB),   // Copy and normalise blue component
            Normalise<GLfloat>(ucA)}   // Copy and normalise alpha component
    /* -- No code ---------------------------------------------------------- */
    { }
  /* -- Init constructor --------------------------------------------------- */
  FboColour(const GLfloat fR, const GLfloat fG, const GLfloat fB,
    const GLfloat fA) :
    /* -- Initialisers ----------------------------------------------------- */
    fboRGBA{ fR, fG, fB, fA }          // Copy intensity over
    /* -- No code ---------------------------------------------------------- */
    { }
  /* -- Default constructor ------------------------------------------------ */
  FboColour(void) :
    /* -- Initialisers ----------------------------------------------------- */
    fboRGBA{ 1.0f,1.0f,1.0f,1.0f }     // Set full Re/Gr/Bl/Alpha intensity
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
    /* -- Initialisers ----------------------------------------------------- */
    aBlend{ eSrcRGB,                   // Copy blend source RGB
            eDstRGB,                   // Copy blend destination RGB
            eSrcAlpha,                 // Copy blend source Alpha
            eDstAlpha }                // Copy blend destination Alpha
    /* -- No code ---------------------------------------------------------- */
    { }
  /* -- Default constructor ------------------------------------------------ */
  FboBlend(void) :
    /* -- Initialisers ----------------------------------------------------- */
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
  typedef Coordinates<Type1> CoordType1; // Co-ordinate type one
  typedef Coordinates<Type2> CoordType2; // Co-ordinate type two
  /* ----------------------------------------------------------------------- */
  CoordType1       tXY1;               // Top left co-ordinates
  CoordType2       tXY2;               // Width and height
  /* --------------------------------------------------------------- */ public:
  Type1 GetCoLeft(void) const { return tXY1.CoordGetX(); }
  Type1 GetCoTop(void) const { return tXY1.CoordGetY(); }
  Type2 GetCoRight(void) const { return tXY2.CoordGetX(); }
  Type2 GetCoBottom(void) const { return tXY2.CoordGetY(); }
  /* ----------------------------------------------------------------------- */
  void SetCoLeft(const Type1 tNLeft) { tXY1.CoordSetX(tNLeft); }
  void SetCoTop(const Type1 tNTop) { tXY1.CoordSetY(tNTop); }
  void SetCoRight(const Type2 tNRight) { tXY2.CoordSetX(tNRight); }
  void SetCoBottom(const Type2 tNBottom) { tXY2.CoordSetY(tNBottom); }
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
  void ResetCoords(void) { tXY1.CoordSet(); tXY2.CoordSet(); }
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
  FboCoords(const Type1 tNLeft,        // Left (X1) co-ordinate
            const Type1 tNTop,         // Top (Y1) co-ordinate
            const Type2 tNRight,       // Right (X2) co-ordinate
            const Type2 tNBottom) :    // Bottom (Y2) co-ordinate
    /* -- Initialisers ----------------------------------------------------- */
    tXY1{ tNLeft, tNTop },             // X1 & Y1 co-ordinate
    tXY2{ tNRight, tNBottom }          // X2 & Y2 co-ordinate
    /* -- Code ------------------------------------------------------------- */
    { }                                // Do nothing else
  /* -- Default constructor ------------------------------------------------ */
  FboCoords(void) :                    // No parameters
    /* -- Initialisers ----------------------------------------------------- */
    FboCoords{ 0, 0, 0, 0 }            // Initialise co-ordinates
    /* --------------------------------------------------------------------- */
    { }                                // Do nothing else
};/* ----------------------------------------------------------------------- */
typedef FboCoords<GLint,GLsizei> FboViewport;    // Viewport co-ordinates
typedef FboCoords<GLfloat>       FboFloatCoords; // Coords made of floats
typedef Dimensions<GLsizei>      DimGLInt;       // Dimension of GLints
/* == Data required to complete a render of an fbo ========================= */
struct FboRenderItem :                 // Rendering item data class
  /* -- Base classes ------------------------------------------------------- */
  public FboColour,                    // Clear colour of the fbo
  public FboBlend,                     // Blend mode of the fbo
  public FboFloatCoords,               // Ortho co-ordinates of the fbo
  public DimGLInt                      // Fbo dimensions
{ /* ----------------------------------------------------------------------- */
  GLuint           uiFBO;              // Fbo name
  bool             bClear;             // Clear the fbo?
  /* -- Default constructor ------------------------------------------------ */
  FboRenderItem(void) :                // No parameters
    /* -- Initialisers ----------------------------------------------------- */
    uiFBO(0),                          // No fbo id yet
    bClear(true)                       // Clear fbo set
    /* -- No code ---------------------------------------------------------- */
    { }
};/* ----------------------------------------------------------------------- */
};                                     // End of module namespace
/* == EoF =========================================================== EoF == */

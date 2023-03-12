/* == FBOITEM.HPP ========================================================== */
/* ######################################################################### */
/* ## MS-ENGINE              Copyright (c) MS-Design, All Rights Reserved ## */
/* ######################################################################### */
/* ## Allows storage and manipulation of a quad (two trangles).           ## */
/* ######################################################################### */
/* ========================================================================= */
#pragma once                           // Only one incursion allowed
/* ------------------------------------------------------------------------- */
namespace IfFbo {                      // Start of module namespace
/* -- Includes ------------------------------------------------------------- */
using namespace IfUtil;                // Using util namespace
/* == Fbo item class ======================================================= */
class FboItem
{ /* -- Typedefs for types we need --------------------------------- */ public:
  typedef array<GLfloat,FLOATSPERCOORD>   TriCoordData;  // Triangle TexCoords
  typedef array<TriCoordData,TRISPERQUAD> QuadCoordData; // Quad tex-coord
  typedef array<GLfloat,FLOATSPERPOS>     TriPosData;    // Triangle positions
  typedef array<TriPosData,TRISPERQUAD>   QuadPosData;   // Quad position data
  typedef array<GLfloat,FLOATSPERCOLOUR>  TriColData;    // Triangl intensities
  typedef array<TriColData,TRISPERQUAD>   QuadColData;   // Quad colour data
  typedef array<GLfloat,FLOATSPERQUAD>    AllData;       // All data elements
  /* -- Variables ------------------------------------------------- */ private:
  union Quad                           // Render to texture Vertex array data
  { AllData        faData;             // Vertices to upload to VBO
    struct Parts { QuadCoordData qdCoord;       // Quad tex-coords data
                   QuadPosData   qdPos;         // Quad position data
                   QuadColData   qdColour; } c; // Quad colour mod data
  } sBuffer;                           // End of quad data union
  /* -- Saved data --------------------------------------------------------- */
  QuadColData      faCSave;            // Saved colour data
  /* -- Get specific data from specified triangle index ------------ */ public:
  QuadPosData &GetVData(void) { return sBuffer.c.qdPos; }
  TriPosData &GetVData(const size_t stT) { return GetVData()[stT]; }
  QuadCoordData &GetTCData(void) { return sBuffer.c.qdCoord; }
  TriCoordData &GetTCData(const size_t stT) { return GetTCData()[stT]; }
  QuadColData &GetCData(void) { return sBuffer.c.qdColour; }
  TriColData &GetCData(const size_t stT) { return GetCData()[stT]; }
  /* -- Set vertex bounds directly on one triangle ------------------------- */
  void SetVertexEx(const size_t stId, const TriPosData &tdNew)
    { memcpy(GetVData(stId).data(), tdNew.data(), sizeof(TriPosData)); }
  void SetTexCoordEx(const size_t stId, const TriCoordData &tdNew)
    { memcpy(GetTCData(stId).data(), tdNew.data(), sizeof(TriCoordData)); }
  void SetColourEx(const size_t stId, const TriColData &tdNew)
    { memcpy(GetCData(stId).data(), tdNew.data(), sizeof(TriColData)); }
  /* -- Set vertex bounds -------------------------------------------------- */
  void SetVertex(const GLfloat fX1, const GLfloat fY1,
                 const GLfloat fX2, const GLfloat fY2)
  { // Update the first triangle of the quad
    TriPosData &tdT1 = GetVData(0);
    tdT1[0] = fX1; tdT1[1] = fY1; tdT1[2] = fX2; // Triangle 1
    tdT1[3] = fY1; tdT1[4] = fX1; tdT1[5] = fY2; //     "
    // Update the second triangle of the quad
    TriPosData &tdT2 = GetVData(1);
    tdT2[0] = fX2; tdT2[1] = fY2; tdT2[2] = fX1; // Triangle 2
    tdT2[3] = fY2; tdT2[4] = fX2; tdT2[5] = fY1; //     "
  }
  /* -- Set vertex bounds and return it ------------------------------------ */
  const QuadPosData &SetAndGetVertex(const GLfloat fX1, const GLfloat fY1,
                                     const GLfloat fX2, const GLfloat fY2)
    { SetVertex(fX1, fY1, fX2, fY2); return GetVData(); }
  /* -- Set vertex bounds modified by normals horizontally ----------------- */
  void SetVertex(const GLfloat fX1, const GLfloat fY1,
                 const GLfloat fX2, const GLfloat fY2,
                 const GLfloat fML, const GLfloat fMR)
  { // Modify vertex based on horizotal scale normal (left edge)
    TriPosData &tdT1 = GetVData(0);
    tdT1[0] = fX2-((fX2-fX1)*fML);         tdT1[1] = fY1; // Triangle 1
    tdT1[2] = tdT1[0]+((fX2-tdT1[0])*fMR); tdT1[3] = fY1; //   V0 V1
    tdT1[4] = tdT1[0];                     tdT1[5] = fY2; //   V2 /
    // Modify vertex based on horizotal scale normal (right edge)
    TriPosData &tdT2 = GetVData(1);
    tdT2[0] = fX1-((fX1-fX2)*fMR);         tdT2[1] = fY2; // Triangle 2
    tdT2[2] = tdT2[0]+((fX1-tdT2[0])*fML); tdT2[3] = fY2; //    / V2
    tdT2[4] = tdT2[0];                     tdT2[5] = fY1; //   V1 V0
  }
  /* -- Set vertex bounds with modified left and right bounds and get ------ */
  const QuadPosData &SetAndGetVertex(const GLfloat fX1, const GLfloat fY1,
                                     const GLfloat fX2, const GLfloat fY2,
                                     const GLfloat fML, const GLfloat fMR)
    { SetVertex(fX1, fY1, fX2, fY2, fML, fMR); return GetVData(); }
  /* -- Set vertex bounds with angle --------------------------------------- */
  void SetVertex(const GLfloat fX1, const GLfloat fY1,
     const GLfloat fX2, const GLfloat fY2, const GLfloat fA)
  { // Denormalise the angle to radians                             (M_PI)
    const GLfloat fAR = fA * 2 * 3.141592653589793238462643383279502884F,
    // Calculate centre
    fXC = (fX2-fX1)/2, fYC = (fY2-fY1)/2,
    // Rotate vertices
    fC1 = atan2(-fYC,fXC)+fAR,         fC2 = atan2(-fYC,-fXC)+fAR,
    fC3 = atan2( fYC,fXC)+fAR,         fC4 = atan2( fYC,-fXC)+fAR,
    fC5 = sqrt(-fYC*-fYC+fXC*fXC),     fC6 = sqrt(-fYC*-fYC+-fXC*-fXC),
    fC7 = sqrt( fYC* fYC+fXC*fXC),     fC8 = sqrt( fYC* fYC+-fXC*-fXC),
    fCa = cos(fC2)*fC5,                fCb = sin(fC2)*fC5,
    fCc = cos(fC1)*fC6,                fCd = sin(fC1)*fC6,
    fCe = cos(fC4)*fC7,                fCf = sin(fC4)*fC7,
    fCg = cos(fC3)*fC8,                fCh = sin(fC3)*fC8;
    // Update the first triangle of the quad
    TriPosData &tdT1 = GetVData(0);
    tdT1[0] = fX1+fCa; tdT1[1] = fY1+fCb; // Vertex 1 / Triangle 1
    tdT1[2] = fX1+fCc; tdT1[3] = fY1+fCd; //   "    2 /     "    1
    tdT1[4] = fX1+fCe; tdT1[5] = fY1+fCf; //   "    3 /     "    1
    // Update the second triangle of the quad
    TriPosData &tdT2 = GetVData(1);
    tdT2[0] = fX1+fCg; tdT2[1] = fY1+fCh; // Vertex 1 / Triangle 2
    tdT2[2] = fX1+fCe; tdT2[3] = fY1+fCf; //   "    2 /     "    2
    tdT2[4] = fX1+fCc; tdT2[5] = fY1+fCd; //   "    3 /     "    2
  }
  /* -- Set vertex bounds and return it ------------------------------------ */
  const QuadPosData &SetAndGetVertex(const GLfloat fX1, const GLfloat fY1,
                                     const GLfloat fX2, const GLfloat fY2,
                                     const GLfloat fA)
    { SetVertex(fX1, fY1, fX2, fY2, fA); return GetVData(); }
  /* -- Set tex coords for FBO (Full and simple) --------------------------- */
  void SetTexCoord(const GLfloat fX1, const GLfloat fY1,
                   const GLfloat fX2, const GLfloat fY2)
  { // Set the texture coordinates of the first triangle
    TriCoordData &tdT1 = GetTCData(0);
    tdT1[0] = fX1; tdT1[1] = fY1;      // Vertex 1 of Triangle 1  V0 V1  T1
    tdT1[2] = fX2; tdT1[3] = fY1;      // Vertex 2 of Triangle 1  V2 /
    tdT1[4] = fX1; tdT1[5] = fY2;      // Vertex 3 of Triangle 1
    // Set the texture coordinates of the second triangle
    TriCoordData &tdT2 = GetTCData(1);
    tdT2[0] = fX2; tdT2[1] = fY2;      // Vertex 1 of Triangle 2
    tdT2[2] = fX1; tdT2[3] = fY2;      // Vertex 2 of Triangle 2   / V2
    tdT2[4] = fX2; tdT2[5] = fY1;      // Vertex 3 of Triangle 2  V1 V0  T2
  }
  /* -- Set tex coords for FBO based on horizontal scale normals ----------- */
  void SetTexCoord(const QuadCoordData &fTC, const GLfloat fML,
                   const GLfloat fMR)
  { // Update tex coords for triangle 1
    const TriCoordData &tdTS1 = fTC[0]; // Source
    TriCoordData &tdTD1 = GetTCData(0); // Destination
    tdTD1[0] = tdTS1[2]-((tdTS1[2]-tdTS1[0])*fML); tdTD1[1] = tdTS1[1];
    tdTD1[2] = tdTD1[0]+((tdTS1[2]-tdTD1[0])*fMR); tdTD1[3] = tdTS1[3];
    tdTD1[4] = tdTD1[0];                           tdTD1[5] = tdTS1[5];
    // Update tex coords for triangle 2
    const TriCoordData &tdTS2 = fTC[1]; // Source
    TriCoordData &tdTD2 = GetTCData(1); // Destination
    tdTD2[0] = tdTS2[2]-((tdTS2[2]-tdTS2[0])*fMR); tdTD2[1] = tdTS2[1];
    tdTD2[2] = tdTD2[0]+((tdTS2[2]-tdTD2[0])*fML); tdTD2[3] = tdTS2[3];
    tdTD2[4] = tdTD2[0];                           tdTD2[5] = tdTS2[5];
  }
  /* -- Set vertex bounds and return it ------------------------------------ */
  const QuadCoordData &SetAndGetCoord(const QuadCoordData &fTC,
    const GLfloat fML, const GLfloat fMR)
      { SetTexCoord(fTC, fML, fMR); return GetTCData(); }
  /* -- Set colour for FBO ------------------------------------------------- */
  void SetQuadRGBA(const GLfloat fR, const GLfloat fG,
                   const GLfloat fB, const GLfloat fA)
  { // Set the colour data of the first triangle
    TriColData &tdT1 = GetCData(0);
    tdT1[ 0] = fR; tdT1[ 1] = fG; tdT1[ 2] = fB; tdT1[ 3] = fA; // V1 of T1
    tdT1[ 4] = fR; tdT1[ 5] = fG; tdT1[ 6] = fB; tdT1[ 7] = fA; // V2 of T1
    tdT1[ 8] = fR; tdT1[ 9] = fG; tdT1[10] = fB; tdT1[11] = fA; // V3 of T1
    // Set the colour data of the second triangle
    TriColData &tdT2 = GetCData(1);
    tdT2[ 0] = fR; tdT2[ 1] = fG; tdT2[ 2] = fB; tdT2[ 3] = fA; // V1 of T2
    tdT2[ 4] = fR; tdT2[ 5] = fG; tdT2[ 6] = fB; tdT2[ 7] = fA; // V2 of T2
    tdT2[ 8] = fR; tdT2[ 9] = fG; tdT2[10] = fB; tdT2[11] = fA; // V3 of T2
  }
  /* -- Return static offset indexes --------------------------------------- */
  const GLvoid *GetTCIndex(void) const
    { return reinterpret_cast<GLvoid*>(0); }
  const GLvoid *GetVIndex(void) const
    { return reinterpret_cast<GLvoid*>(sizeof(sBuffer.c.qdCoord)); }
  const GLvoid *GetCIndex(void) const
    { return reinterpret_cast<GLvoid*>(sizeof(sBuffer.c.qdCoord) +
                                       sizeof(sBuffer.c.qdPos)); }
  /* -- Get data ----------------------------------------------------------- */
  const GLvoid *GetData(void) const { return sBuffer.faData.data(); }
  GLsizei GetDataSize(void) const { return sizeof(sBuffer.faData); }
  /* -- Get defaults as lookup table --------------------------------------- */
  const Quad &GetDefaultLookup(void) const
  { // This is equal to the following calls. It's just easier to memcpy the
    // whole table across then doing pointless calculation. Remember that the
    // Z is not being used so that co-ordinate is ignored and assumed zero.
    // - SetTexCoord(0, 0, 1, 1);
    // - SetVertex(-1, 1, 1, -1);
    // - SetRGBA(1, 1, 1, 1);
    static const Quad qData{{
      // QuadCoordData qdCoord (render the entire texture on the triangles)
      0.0, 0.0,  1.0, 0.0,  0.0, 1.0,  // Triangle 1/2 (Vertice 1, 2 and 3)
      1.0, 1.0,  0.0, 1.0,  1.0, 0.0,  //      "   2/2 (X,Y,    X,Y    X,Y)
      // QuadPosData qdPos (render the two triangles full-screen)
     -1.0, 1.0,  1.0, 1.0, -1.0,-1.0,  // Triangle 1/2 (Vertice 1, 2 and 3)
      1.0,-1.0, -1.0,-1.0,  1.0, 1.0,  //      "   2/2 (X,Y,    X,Y    X,Y)
      // QuadColData qdColour (all solid white intensity with no alpha)
      1.0, 1.0, 1.0, 1.0,              // Triangle 1/2 (R,G,B,A, Vertice 1/3)
      1.0, 1.0, 1.0, 1.0,              //      "       (   "        "    2/3)
      1.0, 1.0, 1.0, 1.0,              //      "       (   "        "    3/3)
      1.0, 1.0, 1.0, 1.0,              // Triangle 2/2 (R,G,B,A, Vertice 1/3)
      1.0, 1.0, 1.0, 1.0,              //      "       (   "        "    2/3)
      1.0, 1.0, 1.0, 1.0               //      "       (   "        "    3/3)
    }}; // Return the lookup table
    return qData;
  }
  /* -- Set defaults ------------------------------------------------------- */
  void SetDefaults(void) { sBuffer = GetDefaultLookup(); }
  /* -- Set colour components (0xAARRGGBB) --------------------------------- */
  void SetQuadRGBAInt(const unsigned int uiC)
    { SetQuadRGBA(NormaliseEx<GLfloat,16>(uiC), NormaliseEx<GLfloat, 8>(uiC),
                  NormaliseEx<GLfloat>(uiC), NormaliseEx<GLfloat,24>(uiC)); }
  void SetQuadRGB(const GLfloat fR, const GLfloat fG, const GLfloat fB)
    { SetQuadRed(fR); SetQuadGreen(fG); SetQuadBlue(fB); }
  void SetQuadRGBInt(const unsigned int uiC)
    { SetQuadRGB(NormaliseEx<GLfloat,16>(uiC), NormaliseEx<GLfloat,8>(uiC),
                 NormaliseEx<GLfloat>(uiC)); }
  /* -- Update red component ----------------------------------------------- */
  void SetQuadRed(const GLfloat fRed)
  { // Get references to triangle colour data and modify the red values
    TriColData &tdT1 = GetCData(0), &tdT2 = GetCData(1);
    tdT1[0] = tdT1[4] = tdT1[8] = tdT2[0] = tdT2[4] = tdT2[8] = fRed;
  }
  /* -- Update green component --------------------------------------------- */
  void SetQuadGreen(const GLfloat fGreen)
  { // Get references to triangle colour data and modify the green values
    TriColData &tdT1 = GetCData(0), &tdT2 = GetCData(1);
    tdT1[1] = tdT1[5] = tdT1[9] = tdT2[1] = tdT2[5] = tdT2[9] = fGreen;
  }
  /* -- Update blue component ---------------------------------------------- */
  void SetQuadBlue(const GLfloat fBlue)
  { // Get references to triangle colour data and modify the blue values
    TriColData &tdT1 = GetCData(0), &tdT2 = GetCData(1);
    tdT1[2] = tdT1[6] = tdT1[10] = tdT2[2] = tdT2[6] = tdT2[10] = fBlue;
  }
  /* -- Update alpha component --------------------------------------------- */
  void SetQuadAlpha(const GLfloat fAlpha)
  { // Get references to triangle colour data and modify the alpha values
    TriColData &tdT1 = GetCData(0), &tdT2 = GetCData(1);
    tdT1[3] = tdT1[7] = tdT1[11] = tdT2[3] = tdT2[7] = tdT2[11] = fAlpha;
  }
  /* -- Save and restore colour data --------------------------------------- */
  void PushQuadColour(void) { faCSave = GetCData(); }
  void PopQuadColour(void) { sBuffer.c.qdColour = faCSave; }
  /* -- Init with colour (from font) --------------------------------------- */
  explicit FboItem(const unsigned int uiC) { SetQuadRGBAInt(uiC); }
  /* ----------------------------------------------------------------------- */
  FboItem(void) :
    /* -- Initialisation of members ---------------------------------------- */
    sBuffer{ GetDefaultLookup() }
    /* -- No code ---------------------------------------------------------- */
    { }
  /* ----------------------------------------------------------------------- */
  DELETECOPYCTORS(FboItem);
};/* ----------------------------------------------------------------------- */
/* ------------------------------------------------------------------------- */
};                                     // End of module namespace
/* == EoF =========================================================== EoF == */

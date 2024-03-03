/* == FBOITEM.HPP ========================================================== **
** ######################################################################### **
** ## MS-ENGINE              Copyright (c) MS-Design, All Rights Reserved ## **
** ######################################################################### **
** ## Allows storage and manipulation of a quad (two trangles).           ## **
** ######################################################################### **
** ========================================================================= */
#pragma once                           // Only one incursion allowed
/* ------------------------------------------------------------------------- */
namespace IFboItem {                   // Start of private module namespace
/* ------------------------------------------------------------------------- */
using namespace IFboDef::P;            using namespace IUtil::P;
using namespace Lib::OS::GlFW;
/* ------------------------------------------------------------------------- */
namespace P {                          // Start of public module namespace
/* == Fbo item class ======================================================= */
class FboItem
{ /* ----------------------------------------------------------------------- */
  constexpr static const size_t
    stUInt8Bits  = sizeof(uint8_t) * 8,
    stUInt16Bits = sizeof(uint16_t) * 8,
    stUInt24Bits = stUInt8Bits + stUInt16Bits;
  /* -- Typedefs for types we need --------------------------------- */ public:
  typedef array<GLfloat, stFloatsPerCoord> TriCoordData; // Triangle TexCoords
  typedef array<TriCoordData, stTrisPerQuad> QuadCoordData; // Quad tex-coord
  typedef array<GLfloat, stFloatsPerPos> TriPosData; // Triangle positions
  typedef array<TriPosData, stTrisPerQuad> QuadPosData; // Quad position data
  typedef array<GLfloat, stFloatsPerColour> TriColData; // Triangle intensities
  typedef array<TriColData, stTrisPerQuad> QuadColData; // Quad colour data
  typedef array<GLfloat, stFloatsPerQuad> AllData; // All data elements
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
  QuadPosData &FboItemGetVData(void) { return sBuffer.c.qdPos; }
  TriPosData &FboItemGetVData(const size_t stT)
    { return FboItemGetVData()[stT]; }
  TriPosData &FboItemGetVDataT1(void) { return FboItemGetVData().front(); }
  TriPosData &FboItemGetVDataT2(void) { return FboItemGetVData().back(); }
  QuadCoordData &FboItemGetTCData(void) { return sBuffer.c.qdCoord; }
  TriCoordData &FboItemGetTCData(const size_t stT)
    { return FboItemGetTCData()[stT]; }
  TriCoordData &FboItemGetTCDataT1(void) { return FboItemGetTCData().front(); }
  TriCoordData &FboItemGetTCDataT2(void) { return FboItemGetTCData().back(); }
  QuadColData &FboItemGetCData(void) { return sBuffer.c.qdColour; }
  TriColData &FboItemGetCData(const size_t stT)
    { return FboItemGetCData()[stT]; }
  TriColData &FboItemGetCDataT1(void) { return FboItemGetCData().front(); }
  TriColData &FboItemGetCDataT2(void) { return FboItemGetCData().back(); }
  /* -- Set vertex bounds directly on one triangle ------------------------- */
  void FboItemSetVertexEx(const size_t stId, const TriPosData &tdNew)
    { memcpy(FboItemGetVData(stId).data(), tdNew.data(), sizeof(TriPosData)); }
  void FboItemSetTexCoordEx(const size_t stId, const TriCoordData &tdNew)
    { memcpy(FboItemGetTCData(stId).data(), tdNew.data(),
        sizeof(TriCoordData)); }
  void FboItemSetColourEx(const size_t stId, const TriColData &tdNew)
    { memcpy(FboItemGetCData(stId).data(), tdNew.data(), sizeof(TriColData)); }
  /* -- Set vertex bounds -------------------------------------------------- */
  void FboItemSetVertex(const GLfloat fX1, const GLfloat fY1,
    const GLfloat fX2, const GLfloat fY2)
  { // Update the first triangle of the quad
    TriPosData &tdT1 = FboItemGetVDataT1();
    tdT1[0] = fX1; tdT1[1] = fY1; tdT1[2] = fX2; // Triangle 1
    tdT1[3] = fY1; tdT1[4] = fX1; tdT1[5] = fY2; //     "
    // Update the second triangle of the quad
    TriPosData &tdT2 = FboItemGetVDataT2();
    tdT2[0] = fX2; tdT2[1] = fY2; tdT2[2] = fX1; // Triangle 2
    tdT2[3] = fY2; tdT2[4] = fX2; tdT2[5] = fY1; //     "
  }
  /* -- Set vertex co-ordinates and dimensions ----------------------------- */
  void FboItemSetVertexWH(const GLfloat fX, const GLfloat fY, const GLfloat fW,
    const GLfloat fH)
      { FboItemSetVertex(fX, fY, fX+fW, fY+fH); }
  /* -- Set vertex bounds and return it ------------------------------------ */
  const QuadPosData &FboItemSetAndGetVertex(const GLfloat fX1,
    const GLfloat fY1, const GLfloat fX2, const GLfloat fY2)
      { FboItemSetVertex(fX1, fY1, fX2, fY2); return FboItemGetVData(); }
  /* -- Set vertex bounds modified by normals horizontally ----------------- */
  void FboItemSetVertex(const GLfloat fX1, const GLfloat fY1,
    const GLfloat fX2, const GLfloat fY2, const GLfloat fML, const GLfloat fMR)
  { // Modify vertex based on horizotal scale normal (left edge)
    TriPosData &tdT1 = FboItemGetVDataT1();
    tdT1[0] = fX2-((fX2-fX1)*fML);         tdT1[1] = fY1; // Triangle 1
    tdT1[2] = tdT1[0]+((fX2-tdT1[0])*fMR); tdT1[3] = fY1; //   V0 V1
    tdT1[4] = tdT1[0];                     tdT1[5] = fY2; //   V2 /
    // Modify vertex based on horizotal scale normal (right edge)
    TriPosData &tdT2 = FboItemGetVDataT2();
    tdT2[0] = fX1-((fX1-fX2)*fMR);         tdT2[1] = fY2; // Triangle 2
    tdT2[2] = tdT2[0]+((fX1-tdT2[0])*fML); tdT2[3] = fY2; //    / V2
    tdT2[4] = tdT2[0];                     tdT2[5] = fY1; //   V1 V0
  }
  /* -- Set vertex bounds with modified left and right bounds and get ------ */
  const QuadPosData &FboItemSetAndGetVertex(const GLfloat fX1,
    const GLfloat fY1, const GLfloat fX2, const GLfloat fY2,
    const GLfloat fML, const GLfloat fMR)
      { FboItemSetVertex(fX1, fY1, fX2, fY2, fML, fMR);
        return FboItemGetVData(); }
  /* -- Set vertex bounds with angle --------------------------------------- */
  void FboItemSetVertex(const GLfloat fX1, const GLfloat fY1,
    const GLfloat fX2, const GLfloat fY2, const GLfloat fA)
  { // UtilDenormalise the angle to radians                          (M_PI)
    const GLfloat fAR = fA * 2.0f * 3.141592653589793238462643383279502884f,
    // Calculate centre
    fXC = (fX2-fX1)/2, fYC = (fY2-fY1)/2,
    // Rotate vertices
    fC1 = atan2f(-fYC,fXC)+fAR,         fC2 = atan2f(-fYC,-fXC)+fAR,
    fC3 = atan2f( fYC,fXC)+fAR,         fC4 = atan2f( fYC,-fXC)+fAR,
    fC5 = sqrtf(-fYC*-fYC+fXC*fXC),     fC6 = sqrtf(-fYC*-fYC+-fXC*-fXC),
    fC7 = sqrtf( fYC* fYC+fXC*fXC),     fC8 = sqrtf( fYC* fYC+-fXC*-fXC),
    fCa = cosf(fC2)*fC5,                fCb = sinf(fC2)*fC5,
    fCc = cosf(fC1)*fC6,                fCd = sinf(fC1)*fC6,
    fCe = cosf(fC4)*fC7,                fCf = sinf(fC4)*fC7,
    fCg = cosf(fC3)*fC8,                fCh = sinf(fC3)*fC8;
    // Update the first triangle of the quad
    TriPosData &tdT1 = FboItemGetVDataT1();
    tdT1[0] = fX1+fCa; tdT1[1] = fY1+fCb; // Vertex 1 / Triangle 1
    tdT1[2] = fX1+fCc; tdT1[3] = fY1+fCd; //   "    2 /     "    1
    tdT1[4] = fX1+fCe; tdT1[5] = fY1+fCf; //   "    3 /     "    1
    // Update the second triangle of the quad
    TriPosData &tdT2 = FboItemGetVDataT2();
    tdT2[0] = fX1+fCg; tdT2[1] = fY1+fCh; // Vertex 1 / Triangle 2
    tdT2[2] = fX1+fCe; tdT2[3] = fY1+fCf; //   "    2 /     "    2
    tdT2[4] = fX1+fCc; tdT2[5] = fY1+fCd; //   "    3 /     "    2
  }
  /* -- Set vertex with coords, dimensions and angle ----------------------- */
  void FboItemSetVertexWH(const GLfloat fX, const GLfloat fY, const GLfloat fW,
    const GLfloat fH, const GLfloat fA)
      { FboItemSetVertex(fX, fY, fX+fW, fY+fH, fA); }
  /* -- Set vertex bounds and return it ------------------------------------ */
  const QuadPosData &FboItemSetAndGetVertex(const GLfloat fX1,
    const GLfloat fY1, const GLfloat fX2, const GLfloat fY2, const GLfloat fA)
      { FboItemSetVertex(fX1, fY1, fX2, fY2, fA); return FboItemGetVData(); }
  /* -- Set tex coords for FBO (Full and simple) --------------------------- */
  void FboItemSetTexCoord(const GLfloat fX1, const GLfloat fY1,
    const GLfloat fX2, const GLfloat fY2)
  { // Set the texture coordinates of the first triangle
    TriCoordData &tdT1 = FboItemGetTCDataT1();
    tdT1[0] = fX1; tdT1[1] = fY1;      // Vertex 1 of Triangle 1  V0 V1  T1
    tdT1[2] = fX2; tdT1[3] = fY1;      // Vertex 2 of Triangle 1  V2 /
    tdT1[4] = fX1; tdT1[5] = fY2;      // Vertex 3 of Triangle 1
    // Set the texture coordinates of the second triangle
    TriCoordData &tdT2 = FboItemGetTCDataT2();
    tdT2[0] = fX2; tdT2[1] = fY2;      // Vertex 1 of Triangle 2
    tdT2[2] = fX1; tdT2[3] = fY2;      // Vertex 2 of Triangle 2   / V2
    tdT2[4] = fX2; tdT2[5] = fY1;      // Vertex 3 of Triangle 2  V1 V0  T2
  }
  /* -- Set texture coords and dimensions ---------------------------------- */
  void FboItemSetTexCoordWH(const GLfloat fX, const GLfloat fY,
    const GLfloat fW, const GLfloat fH)
      { FboItemSetTexCoord(fX, fY, fX+fW, fY+fH); }
  /* -- Set tex coords for FBO based on horizontal scale normals ----------- */
  void FboItemSetTexCoord(const QuadCoordData &fTC, const GLfloat fML,
    const GLfloat fMR)
  { // Update tex coords for triangle 1
    const TriCoordData &tdTS1 = fTC[0]; // Source
    TriCoordData &tdTD1 = FboItemGetTCDataT1(); // Destination
    tdTD1[0] = tdTS1[2]-((tdTS1[2]-tdTS1[0])*fML); tdTD1[1] = tdTS1[1];
    tdTD1[2] = tdTD1[0]+((tdTS1[2]-tdTD1[0])*fMR); tdTD1[3] = tdTS1[3];
    tdTD1[4] = tdTD1[0];                           tdTD1[5] = tdTS1[5];
    // Update tex coords for triangle 2
    const TriCoordData &tdTS2 = fTC[1]; // Source
    TriCoordData &tdTD2 = FboItemGetTCDataT2(); // Destination
    tdTD2[0] = tdTS2[2]-((tdTS2[2]-tdTS2[0])*fMR); tdTD2[1] = tdTS2[1];
    tdTD2[2] = tdTD2[0]+((tdTS2[2]-tdTD2[0])*fML); tdTD2[3] = tdTS2[3];
    tdTD2[4] = tdTD2[0];                           tdTD2[5] = tdTS2[5];
  }
  /* -- Set vertex bounds and return it ------------------------------------ */
  const QuadCoordData &FboItemSetAndGetCoord(const QuadCoordData &fTC,
    const GLfloat fML, const GLfloat fMR)
      { FboItemSetTexCoord(fTC, fML, fMR); return FboItemGetTCData(); }
  /* -- Set colour for FBO ------------------------------------------------- */
  void FboItemSetQuadRGBA(const GLfloat fR, const GLfloat fG, const GLfloat fB,
    const GLfloat fA)
  { // Set the colour data of the first triangle
    TriColData &tdT1 = FboItemGetCDataT1();
    tdT1[ 0] = fR; tdT1[ 1] = fG; tdT1[ 2] = fB; tdT1[ 3] = fA; // V1 of T1
    tdT1[ 4] = fR; tdT1[ 5] = fG; tdT1[ 6] = fB; tdT1[ 7] = fA; // V2 of T1
    tdT1[ 8] = fR; tdT1[ 9] = fG; tdT1[10] = fB; tdT1[11] = fA; // V3 of T1
    // Set the colour data of the second triangle
    TriColData &tdT2 = FboItemGetCDataT2();
    tdT2[ 0] = fR; tdT2[ 1] = fG; tdT2[ 2] = fB; tdT2[ 3] = fA; // V1 of T2
    tdT2[ 4] = fR; tdT2[ 5] = fG; tdT2[ 6] = fB; tdT2[ 7] = fA; // V2 of T2
    tdT2[ 8] = fR; tdT2[ 9] = fG; tdT2[10] = fB; tdT2[11] = fA; // V3 of T2
  }
  /* -- Return static offset indexes --------------------------------------- */
  const GLvoid *FboItemGetTCIndex(void) const
    { return reinterpret_cast<GLvoid*>(0); }
  const GLvoid *FboItemGetVIndex(void) const
    { return reinterpret_cast<GLvoid*>(sizeof(sBuffer.c.qdCoord)); }
  const GLvoid *FboItemGetCIndex(void) const
    { return reinterpret_cast<GLvoid*>
        (sizeof(sBuffer.c.qdCoord) + sizeof(sBuffer.c.qdPos)); }
  /* -- Get data ----------------------------------------------------------- */
  const GLvoid *FboItemGetData(void) const { return sBuffer.faData.data(); }
  GLsizei FboItemGetDataSize(void) const { return sizeof(sBuffer.faData); }
  /* -- Get defaults as lookup table --------------------------------------- */
  const Quad &FboItemGetDefaultLookup(void) const
  { // This is equal to the following calls. It's just easier to memcpy the
    // whole table across then doing pointless calculation. Remember that the
    // Z is not being used so that co-ordinate is ignored and assumed zero.
    // - SetTexCoord(0, 0, 1, 1);
    // - SetVertex(-1, 1, 1, -1);
    // - SetRGBA(1, 1, 1, 1);
    static const Quad qData{{
      // QuadCoordData qdCoord (render the entire texture on the triangles)
      0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, // Triangle 1/2 (Vertice 1, 2 and 3)
      1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, //      "   2/2 (X,Y,    X,Y    X,Y)
      // QuadPosData qdPos (render the two triangles full-screen)
     -1.0f, 1.0f, 1.0f, 1.0f,-1.0f,-1.0f, // Triangle 1/2 (Vertice 1, 2 and 3)
      1.0f,-1.0f,-1.0f,-1.0f, 1.0f, 1.0f, //      "   2/2 (X,Y,    X,Y    X,Y)
      // QuadColData qdColour (all solid white intensity with no alpha)
      1.0f, 1.0f, 1.0f, 1.0f,          // Triangle 1/2 (R,G,B,A, Vertice 1/3)
      1.0f, 1.0f, 1.0f, 1.0f,          //      "       (   "        "    2/3)
      1.0f, 1.0f, 1.0f, 1.0f,          //      "       (   "        "    3/3)
      1.0f, 1.0f, 1.0f, 1.0f,          // Triangle 2/2 (R,G,B,A, Vertice 1/3)
      1.0f, 1.0f, 1.0f, 1.0f,          //      "       (   "        "    2/3)
      1.0f, 1.0f, 1.0f, 1.0f           //      "       (   "        "    3/3)
    }}; // Return the lookup table
    return qData;
  }
  /* -- Set defaults ------------------------------------------------------- */
  void FboItemSetDefaults(void) { sBuffer = FboItemGetDefaultLookup(); }
  /* -- Set colour components (0xAARRGGBB) --------------------------------- */
  void FboItemSetQuadRGBAInt(const unsigned int uiC)
    { FboItemSetQuadRGBA(UtilNormaliseEx<GLfloat, stUInt16Bits>(uiC),
        UtilNormaliseEx<GLfloat, stUInt8Bits>(uiC),
        UtilNormaliseEx<GLfloat>(uiC),
        UtilNormaliseEx<GLfloat, stUInt24Bits>(uiC)); }
  /* -- Set colour components (0xRRGGBB) ----------------------------------- */
  void FboItemSetQuadRGB(const GLfloat fR, const GLfloat fG, const GLfloat fB)
    { FboItemSetQuadRed(fR); FboItemSetQuadGreen(fG); FboItemSetQuadBlue(fB); }
  /* -- Set colour components by integer ----------------------------------- */
  void FboItemSetQuadRGBInt(const unsigned int uiC)
    { FboItemSetQuadRGB(UtilNormaliseEx<GLfloat, stUInt16Bits>(uiC),
        UtilNormaliseEx<GLfloat, stUInt8Bits>(uiC),
        UtilNormaliseEx<GLfloat>(uiC)); }
  /* -- Update red component ----------------------------------------------- */
  void FboItemSetQuadRed(const GLfloat fRed)
  { // Get references to triangle colour data and modify the red values
    TriColData &tdT1 = FboItemGetCDataT1(), &tdT2 = FboItemGetCDataT2();
    tdT1[0] = tdT1[4] = tdT1[8] = tdT2[0] = tdT2[4] = tdT2[8] = fRed;
  }
  /* -- Update green component --------------------------------------------- */
  void FboItemSetQuadGreen(const GLfloat fGreen)
  { // Get references to triangle colour data and modify the green values
    TriColData &tdT1 = FboItemGetCDataT1(), &tdT2 = FboItemGetCDataT2();
    tdT1[1] = tdT1[5] = tdT1[9] = tdT2[1] = tdT2[5] = tdT2[9] = fGreen;
  }
  /* -- Update blue component ---------------------------------------------- */
  void FboItemSetQuadBlue(const GLfloat fBlue)
  { // Get references to triangle colour data and modify the blue values
    TriColData &tdT1 = FboItemGetCDataT1(), &tdT2 = FboItemGetCDataT2();
    tdT1[2] = tdT1[6] = tdT1[10] = tdT2[2] = tdT2[6] = tdT2[10] = fBlue;
  }
  /* -- Update alpha component --------------------------------------------- */
  void FboItemSetQuadAlpha(const GLfloat fAlpha)
  { // Get references to triangle colour data and modify the alpha values
    TriColData &tdT1 = FboItemGetCDataT1(), &tdT2 = FboItemGetCDataT2();
    tdT1[3] = tdT1[7] = tdT1[11] = tdT2[3] = tdT2[7] = tdT2[11] = fAlpha;
  }
  /* -- Save and restore colour data --------------------------------------- */
  void FboItemPushQuadColour(void) { faCSave = FboItemGetCData(); }
  void FboItemPopQuadColour(void) { sBuffer.c.qdColour = faCSave; }
  /* -- Init with colour (from font) --------------------------------------- */
  explicit FboItem(const unsigned int uiC) { FboItemSetQuadRGBAInt(uiC); }
  /* ----------------------------------------------------------------------- */
  FboItem(void) :
    /* -- Initialisers ----------------------------------------------------- */
    sBuffer{ FboItemGetDefaultLookup() }
    /* -- No code ---------------------------------------------------------- */
    { }
  /* ----------------------------------------------------------------------- */
  DELETECOPYCTORS(FboItem)
};/* ----------------------------------------------------------------------- */
}                                      // End of public module namespace
/* ------------------------------------------------------------------------- */
}                                      // End of private module namespace
/* == EoF =========================================================== EoF == */

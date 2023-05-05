/* == TEXTURE.HPP ========================================================== */
/* ######################################################################### */
/* ## MS-ENGINE              Copyright (c) MS-Design, All Rights Reserved ## */
/* ######################################################################### */
/* ## This module handles the loading and uploading of textures into      ## */
/* ## OpenGL. It will also handle some core drawing procedures inside the ## */
/* ## collector class too.                                                ## */
/* ######################################################################### */
/* ========================================================================= */
#pragma once                           // Only one incursion allowed
/* ------------------------------------------------------------------------- */
namespace IfTexture {                  // Start of module namespace
/* -- Includes ------------------------------------------------------------- */
using namespace IfFbo;                 // Using fbo namespace
using namespace IfImage;               // Using image namespace
/* -- Texture collector class for collector data and custom variables ------ */
BEGIN_COLLECTOR(Textures, Texture, CLHelperUnsafe)
/* ------------------------------------------------------------------------- */
class TextureVars :                    // All members initially private
  /* -- Base classes ------------------------------------------------------- */
  public FboItem,                      // Fbo item class with drawing co-ords
  public Image                         // Image class with raw image pixel data
{ /* -- Protected typedefs -------------------------------------- */ protected:
  typedef Dimensions<GLfloat> DimFloat;// Dimension using GLfloats
  /* -- Protected variables ------------------------------------------------ */
  GLint            iTexMinFilter,      // GL texture minification setting
                   iTexMagFilter,      // GL texture magnification setting
                   iMipmaps;           // Sub-image's are mipmaps (count)
  size_t           stTexFilter;        // Texture filter (for reference)
  /* -- Tile co-ordinates class ------------------------------------ */ public:
  struct CoordData :                   // All members are public
    /* -- Initialisers ----------------------------------------------------- */
    public DimFloat,                   // Dimensions of texture
    public QuadCoordData               // GL quad co-ords of two triangles
  { /* -- 2D tex-coord quad initialisation constructor --------------------- */
    CoordData(const GLfloat fWidth,    // Pixel width of tile   |---Y1---| ^
              const GLfloat fHeight,   // Pixel height of tile  |........| |
              const GLfloat fLeft,     // Normal X1 coord (0-1) X1......X2 H
              const GLfloat fTop,      // Normal Y1 coord (0-1) |........| |
              const GLfloat fRight,    // Normal X2 coord (0-1) |---Y2---| v
              const GLfloat fBottom) : // Normal Y2 coord (0-1) <----W--->
      /* -- Initialisers --------------------------------------------------- */
      DimFloat{ fWidth, fHeight },     // Initialise tile pixel dimensions
      QuadCoordData{ {                 // Initialise two GL triangle tex coords
        { fLeft,  fTop,                // [0][0-1] T1 @ XY1    XY1--XY2 XY3
          fRight, fTop,                // [0][2-3] T1 @ XY2     |.../   /|
          fLeft,  fBottom },           // [0][4-5] T1 @ XY3     |T1/   /.|
        { fRight, fBottom,             // [1][0-1] T2 @ XY1     |./   /T2|
          fLeft,  fBottom,             // [1][2-3] T2 @ XY2     |/   /...|
          fRight, fTop } } }           // [1][4-5] T2 @ XY3    XY3 XY2-XY1
      /* -- No code -------------------------------------------------------- */
      { }                              // Note that our shader handles Z co-ord
    /* -- Default constructor ---------------------------------------------- */
    CoordData(void) :                  // No parameters
      /* -- Initialisers --------------------------------------------------- */
      QuadCoordData{ {                 // Default init two GL tri tex coords
        { 0.0f, 0.0f, 0.0f,            // [0][0-2] Tri1\X1, Tri1\Y1, Tri1\X2
          0.0f, 0.0f, 0.0f },          // [0][3-5] Tri1\Y2, Tri1\X3, Tri1\Y3
        { 0.0f, 0.0f, 0.0f,            // [1][0-2] Tri2\X1, Tri2\Y1, Tri2\X2
          0.0f, 0.0f, 0.0f } } }       // [1][3-5] Tri2\Y2, Tri2\X3, Tri2\Y3
      /* -- No code -------------------------------------------------------- */
      { }                              // Note that our shader handles Z co-ord
  };/* --------------------------------------------------------------------- */
  typedef vector<CoordData> CoordList; // Tile coordinates data list
  typedef vector<CoordList> CoordsList;// A list of tile coords per sub-tex
  typedef Dimensions<GLuint> DimUInt;    // Dimension of GLuint's
  /* ----------------------------------------------------------------------- */
  CoordsList       clTiles;            // Texture coordinates for tiles
  GLUIntVector     vTexture;           // OpenGL texture handle list
  Shader          *shProgram;          // Default shader program to use
  DimUInt          duTile;             // Texture tile width and height
  DimFloat         dfPad,              // Texture tile padding (GL)
                   dfImage,            // Texture image width and height (GL)
                   dfTile;             // Texture tile width and height (GL)
  /* -- Constructor -------------------------------------------------------- */
  TextureVars(void) :                  // No parameters
    /* -- Initialisers ----------------------------------------------------- */
    iTexMinFilter(GL_NONE),            // No minification filter set yet
    iTexMagFilter(GL_NONE),            // No magnification filter set et
    iMipmaps(0),                       // No mipmaps yet
    stTexFilter(0),                    // No texture filter index set yet
    shProgram(nullptr)                 // No shader programme yet
    /* -- Code ------------------------------------------------------------- */
    { }                                // No code
  /* ----------------------------------------------------------------------- */
  DELETECOPYCTORS(TextureVars)         // No defaults
};/* ----------------------------------------------------------------------- */
BEGIN_MEMBERCLASSEX(Textures, Texture, ICHelperUnsafe, /* No IdentCSlave<> */),
  /* -- Base classes ------------------------------------------------------- */
  public TextureVars                   // Texture class variables
{ /* -- Return padding dimensions ---------------------------------- */ public:
  GLfloat GetPaddingWidth(void) const { return dfPad.DimGetWidth(); }
  GLfloat GetPaddingHeight(void) const { return dfPad.DimGetHeight(); }
  /* -- Return tile dimensions --------------------------------------------- */
  GLuint GetTileWidth(void) const { return duTile.DimGetWidth(); }
  GLuint GetTileHeight(void) const { return duTile.DimGetHeight(); }
  /* -- Return number of tiles --------------------------------------------- */
  size_t GetTileCount(const size_t stSubTexId=0) const
    { return clTiles[stSubTexId].size(); }
  /* -- Set tile count ----------------------------------------------------- */
  void SetTileCount(const size_t stCount, const size_t stSubTexId=0)
    { clTiles[stSubTexId].resize(stCount); }
  /* -- Return image float value dimensions -------------------------------- */
  GLfloat GetFWidth(void) const { return dfImage.DimGetWidth(); }
  GLfloat GetFHeight(void) const { return dfImage.DimGetHeight(); }
  /* -- Return the number of mipmaps in the texture ------------------------ */
  GLint GetMipmaps(void) const { return iMipmaps; }
  /* -- Return the current texture filter index setting -------------------- */
  size_t GetTexFilter(void) const { return stTexFilter; }
  /* -- Return the OpenGL texture name for the specified sub-textures ------ */
  GLuint GetSubName(const size_t stSubTexId=0) const
    { return vTexture[stSubTexId]; }
  /* -- Return number of sub-textures -------------------------------------- */
  size_t GetSubCount(void) const { return vTexture.size(); }
  /* -- Return a new tile -------------------------------------------------- */
  const CoordData NewTile(const GLfloat fL, const GLfloat fT, const GLfloat fR,
    const GLfloat fB, const GLfloat fW, const GLfloat fH)
  { const GLfloat fLeft =      fL/GetFWidth(),  fRight  =      fR/GetFWidth(),
                  fTop  = 1.0f-fT/GetFHeight(), fBottom = 1.0f-fB/GetFHeight();
    return { fW, fH, fLeft, fTop, fRight, fBottom }; }
  /* -- Set the texture co-ordinates of a tile ----------------------------- */
  void SetTile(const size_t stSubTexId, const size_t stTileId,
    const GLfloat fL, const GLfloat fT, const GLfloat fR, const GLfloat fB,
    const GLfloat fW, const GLfloat fH)
  { clTiles[stSubTexId][stTileId] = NewTile(fL, fT, fR, fB, fW, fH); }
  /* -- Set the texture co-ordinates of a tile ----------------------------- */
  void SetTile(const size_t stSubTexId, const size_t stTileId,
    const GLfloat fL, const GLfloat fT, const GLfloat fR, const GLfloat fB)
  { SetTile(stSubTexId, stTileId, fL, fT, fR, fB, fR-fL, fB-fT); }
  /* -- Set the texture co-ordinates of a tile that has reversed scanlines - */
  void SetTileR(const size_t stSubTexId, const size_t stTileId,
    const GLfloat fL,const GLfloat fT, const GLfloat fR,const GLfloat fB)
  { const GLfloat fNT = GetFHeight()-fT, fNB = GetFHeight()-fB;
    SetTile(stSubTexId, stTileId, fL, fNT, fR, fNB, fR-fL, fNT-fNB); }
  /* -- Set a tile based on reversal --------------------------------------- */
  void SetTileDOR(const size_t stSubTexId, const size_t stTileId,
    const GLfloat fL, const GLfloat fT, const GLfloat fR, const GLfloat fB)
  { if(IsReversed()) SetTileR(stSubTexId, stTileId, fL, fT, fR, fB);
                else SetTile(stSubTexId, stTileId, fL, fT, fR, fB); }
  /* -- Add a tile with width and height ----------------------------------- */
  void SetTileWH(const size_t stSubTexId, const size_t stTileId,
    const GLfloat fL, const GLfloat fT, const GLfloat fW, const GLfloat fH)
  { const GLfloat fNR = fL+fW, fNB = fT+fH;
    SetTile(stSubTexId, stTileId, fL, fT, fNR, fNB, fNR-fL, fNB-fT); }
  /* -- Add a reversed tile with width and height setting ------------------ */
  void SetTileRWH(const size_t stSubTexId, const size_t stTileId,
    const GLfloat fL, const GLfloat fT, const GLfloat fW, const GLfloat fH)
  { // Calculate new top, right and bottom and add the tile
    const GLfloat fNT = GetFHeight()-fT, fNB = fNT+fH, fNR = fL+fW;
    SetTile(stSubTexId, stTileId, fL, fT, fNR, fNB, fNR-fL, fNT-fNB);
  }
  /* -- Set a tile based on reversal with width and height setting --------- */
  void SetTileDORWH(const size_t stSubTexId, const size_t stTileId,
    const GLfloat fL, const GLfloat fT, const GLfloat fW, const GLfloat fH)
  { if(IsReversed()) SetTileRWH(stSubTexId, stTileId, fL, fT, fW, fH);
                else SetTileWH(stSubTexId, stTileId, fL, fT, fW, fH); }
  /* -- Do add a tile with custom width and height setting ----------------- */
  void AddTile(const size_t stSubTexId, const GLfloat fL, const GLfloat fT,
    const GLfloat fR, const GLfloat fB, const GLfloat fW, const GLfloat fH)
  { clTiles[stSubTexId].emplace_back(NewTile(fL, fT, fR, fB, fW, fH)); }
  /* -- Add a tile --------------------------------------------------------- */
  void AddTile(const size_t stSubTexId, const GLfloat fL, const GLfloat fT,
    const GLfloat fR, const GLfloat fB)
  { AddTile(stSubTexId, fL, fT, fR, fB, fR-fL, fB-fT); }
  /* -- Add a reversed tile ------------------------------------------------ */
  void AddTileR(const size_t stSubTexId, const GLfloat fL, const GLfloat fT,
    const GLfloat fR, const GLfloat fB)
  { const GLfloat fNT = GetFHeight()-fT, fNB = GetFHeight()-fB;
    AddTile(stSubTexId, fL, fNT, fR, fNB, fR-fL, fNT-fNB); }
  /* -- Add a tile based on reversal setting ------------------------------- */
  void AddTileDOR(const size_t stSubTexId, const GLfloat fL, const GLfloat fT,
    const GLfloat fR, const GLfloat fB)
  { if(IsReversed()) AddTileR(stSubTexId, fL, fT, fR, fB);
                else AddTile(stSubTexId, fL, fT, fR, fB); }
  /* -- Add a tile with width and height ----------------------------------- */
  void AddTileWH(const size_t stSubTexId, const GLfloat fL, const GLfloat fT,
    const GLfloat fW, const GLfloat fH)
  { const GLfloat fNR = fL+fW, fNB = fT+fH;
    AddTile(stSubTexId, fL, fT, fNR, fNB, fNR-fL, fNB-fT); }
  /* -- Add a reversed tile with width and height -------------------------- */
  void AddTileRWH(const size_t stSubTexId, const GLfloat fL, const GLfloat fT,
    const GLfloat fW, const GLfloat fH)
  { const GLfloat fNT = GetFHeight()-fT, fNB = fNT+fH, fNR = fL+fW;
    AddTile(stSubTexId, fL, fT, fNR, fNB, fNR-fL, fNT-fNB); }
  /* -- Add a tile with width and height based on reversal ----------------- */
  void AddTileDORWH(const size_t stSubTexId, const GLfloat fL,
    const GLfloat fT, const GLfloat fW, const GLfloat fH)
  { if(IsReversed()) AddTileRWH(stSubTexId, fL, fT, fW, fH);
                else AddTileWH(stSubTexId, fL, fT, fW, fH); }
  /* -- Generate mipmaps if needed ----------------------------------------- */
  void ReGenerateMipmaps(void)
  { // Only need to generate mipmaps if we're actually using mipmapping
    switch(iTexMinFilter)
    { // Any of the mipmapping settings? Generate mipmaps
      case GL_LINEAR_MIPMAP_LINEAR:  case GL_LINEAR_MIPMAP_NEAREST:
      case GL_NEAREST_MIPMAP_LINEAR: case GL_NEAREST_MIPMAP_NEAREST:
        GL(cOgl->GenerateMipmaps(),
          "Failed to generate mipmaps!", "Identifier", IdentGet());
      // Nothing special
      default: break;
    }
  }
  /* -- Configure the specified texture id --------------------------------- */
  void ConfigureTexture(const size_t stSubTexId=0)
  { // Start configuring the texture.
    GL(cOgl->SetTexParam(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE),
      "Could not set texture wrapping S!",
      "Identifier", IdentGet(), "Index", stSubTexId);
    GL(cOgl->SetTexParam(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE),
      "Could not set texture wrapping T!",
      "Identifier", IdentGet(), "Index", stSubTexId);
    GL(cOgl->SetTexParam(GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE),
      "Could not set texture wrapping R!",
      "Identifier", IdentGet(), "Index", stSubTexId);
    // Set filtering based on developers setting
    GL(cOgl->SetTexParam(GL_TEXTURE_MIN_FILTER, iTexMinFilter),
      "Could not set texture minifaction filter!",
      "Identifier", IdentGet(), "Index", stSubTexId,
      "MinFilter",  iTexMinFilter);
    GL(cOgl->SetTexParam(GL_TEXTURE_MAG_FILTER, iTexMagFilter),
      "Could not set texture magnification filter!",
      "Identifier", IdentGet(), "Index", stSubTexId,
      "MagFilter",  iTexMagFilter);
  }
  /* -- Load the specified compressed image -------------------------------- */
  void LoadCompressedTexture(const size_t stSubTexId, const GLint iMipLevel,
    const ImageSlot &sData)
  { // Upload pre-compressed texture to video ram
    GL(cOgl->UploadCompressedTexture(iMipLevel, GetPixelType(),
      sData.DimGetWidth<GLsizei>(), sData.DimGetHeight<GLsizei>(),
      sData.Size<GLsizei>(), sData.Ptr()),
        "Could not upload compressed texture to video ram!",
        "Identifier", IdentGet(),             "Index",  stSubTexId,
        "TexId",      GetSubName(stSubTexId), "Level",  iMipLevel,
        "Width",      sData.DimGetWidth(),    "Height", sData.DimGetHeight(),
        "XCFormat",   GetPixelType(),         "Size",   sData.Size(),
        "Data",       sData.Ptr<void>());
  }
  /* -- Load the specified image ------------------------------------------- */
  void LoadUncompressedTexture(const size_t stSubTexId, const GLint iMipLevel,
    const GLint iNICFormat, const GLenum eNXCFormat, const ImageSlot &sData)
  { // Upload uncompressed texture to video ram
    GL(cOgl->UploadTexture(iMipLevel, sData.DimGetWidth<GLsizei>(),
      sData.DimGetHeight<GLsizei>(), iNICFormat, eNXCFormat, sData.Ptr()),
        "Could not upload uncompressed texture to video ram!",
        "Identifier", IdentGet(),             "Index",     stSubTexId,
        "TexId",      GetSubName(stSubTexId), "Level",     iMipLevel,
        "Width",      sData.DimGetWidth(),    "Height",   sData.DimGetHeight(),
        "XCFormat",   GetPixelType(),         "NICFormat", iNICFormat,
        "NXCFormat",  eNXCFormat,             "Size",      sData.Size(),
        "Data",       sData.Ptr<void>());
  }
  /* -- Do create textures ------------------------------------------------- */
  void CreateTextureHandles(const size_t stCount)
  { // Resize array to fit texture names
    vTexture.resize(stCount);
    // Make OpenGL to generate texture names and throw error if failed
    GL(cOgl->CreateTextures(vTexture), "Failed to generate textures!",
      "Identifier", IdentGet(), "Count", stCount);
  }
  /* -- Do load texture from image class ----------------------------------- */
  void UploadTexture(const size_t stSlots, const bool bCompressed,
    const ImageSlot &sData, const GLint iNICFormat, const GLenum eNXCFormat)
  { // Reset previous marked for deletion flag
    FlagClear(TF_DELETE);
    // If no mipmaps in this bitmap?
    if(IsNotMipmaps())
    { // Image does not contain mipmaps?. No mipmaps
      iMipmaps = 0;
      // Create texture handles
      CreateTextureHandles(stSlots);
      // For each image slot
      for(size_t stSubTexId = 0; stSubTexId < stSlots; ++stSubTexId)
      { // Get next slot and verify that dimensions are different from slot 0
        const ImageSlot &sSlot = GetSlots()[stSubTexId];
        if(DimGetWidth() != sSlot.DimGetWidth() ||
           DimGetHeight() != sSlot.DimGetHeight())
          XC("Alternating image sizes are not supported!",
             "Identifier", IdentGet(), "LastWidth", DimGetWidth(),
             "LastHeight", DimGetHeight(),
             "ThisWidth",  sSlot.DimGetWidth(),
             "ThisHeight", sSlot.DimGetHeight());
        // Get texture id and bind it
        const unsigned int uiTexId = GetSubName(stSubTexId);
        GL(cOgl->BindTexture(uiTexId), "Texture id failed to bind!",
          "Identifier", IdentGet(), "Index", stSubTexId,
          "Maximum",    stSlots,    "TexId", uiTexId);
        // Configure the texture
        ConfigureTexture(stSubTexId);
        // Load the image
        if(bCompressed)
          LoadCompressedTexture(stSubTexId, 0, sSlot);
        else
          LoadUncompressedTexture(stSubTexId, 0, iNICFormat,
            eNXCFormat, sSlot);
        // Automatically generate mipmaps if requested. This has to be done
        // for each uploaded texture
        ReGenerateMipmaps();
      } // Done
      return;
    } // Bitmap has mipmaps so record number of mipmaps we have
    iMipmaps = IntOrMax<GLint>(stSlots);
    // Create only one texture handle as other slots are for mipmaps
    CreateTextureHandles(1);
    // Get texture id
    const unsigned int uiTexId = GetSubName();
    // Bind the texture id
    GL(cOgl->BindTexture(uiTexId), "Mipmapped texture failed to bind!",
      "Identifier", IdentGet(), "TexId", uiTexId);
    // Initialise mipmap counter
    GL(cOgl->SetTexParam(GL_TEXTURE_BASE_LEVEL, 0),
      "Failed to set max mipmap base level on texture!",
      "Identifier", IdentGet(), "TexId", uiTexId);
    GL(cOgl->SetTexParam(GL_TEXTURE_MAX_LEVEL, GetMipmaps()),
      "Failed to set max mipmap max level on texture!",
      "Identifier", IdentGet(), "TexId", uiTexId, "Count", GetMipmaps());
    // Configure the texture
    ConfigureTexture();
    // Upload first mipmap
    if(bCompressed) LoadCompressedTexture(0, 0, sData);
    else LoadUncompressedTexture(0, 0, iNICFormat, eNXCFormat, sData);
    // Done if there more slots to load
    if(stSlots <= 1) return;
    // Last mipmap size
    unsigned int uiLWidth = DimGetWidth(), uiLHeight = DimGetHeight();
    // Load all the other mipmaps
    for(size_t stSubTexId = 1; stSubTexId < stSlots; ++stSubTexId)
    { // Get next slot
      const ImageSlot &sSlot = GetSlotsConst()[stSubTexId];
      // Make sure size is smaller than the last
      if(sSlot.DimGetWidth() >= uiLWidth || sSlot.DimGetHeight() >= uiLHeight)
        XC("Specified mipmap is not smaller than the last!",
           "Identifier", IdentGet(), "LastWidth", uiLWidth,
           "LastHeight", uiLHeight,  "ThisWidth", sSlot.DimGetWidth(),
           "ThisHeight", sSlot.DimGetHeight());
      // Get mipmap level for opengl
      const GLint iMLevel = static_cast<GLint>(stSubTexId);
      // Load the mipmap
      if(bCompressed)
        LoadCompressedTexture(0, iMLevel, sSlot);
      else
        LoadUncompressedTexture(0, iMLevel, iNICFormat, eNXCFormat, sSlot);
      // Update last mipmap size
      uiLWidth = DimGetWidth();
      uiLHeight = DimGetHeight();
    }
  }
  /* -- Generate a generic tileset ----------------------------------------- */
  void GenerateTileset(void)
  { // Resize co-ords list to the number of usable slots we loaded
    clTiles.resize(GetSubCount());
    // Set override tile size from Image loader if specified
    if(duTileOR.DimIsSet()) duTile.DimSet(duTileOR);
    // Else clamp bounds to image size if unspecified or invalid size
    else if(duTile.DimIsNotSet() ||
       duTile.DimGetWidth() > DimGetWidth() ||
       duTile.DimGetHeight() > DimGetHeight())
      duTile.DimSet(*this);
    // Set tile dimensions as opengl float
    dfTile.DimSet(duTile.DimGetWidth<GLfloat>(),
                  duTile.DimGetHeight<GLfloat>());
    // Create clamped image size
    const GLfloat
      fTPSizeX = dfTile.DimGetWidth() + GetPaddingWidth(),
      fTPSizeY = dfTile.DimGetHeight() + GetPaddingHeight(),
      fBTSizeX = floorf(GetFWidth() / fTPSizeX) * fTPSizeX,
      fBTSizeY = floorf(GetFHeight() / fTPSizeY) * fTPSizeY;
    // Calculate number of tile needed
    const size_t stTilesMax = static_cast<size_t>(
      floorf(fBTSizeX / fTPSizeX) * floorf(fBTSizeY / fTPSizeY));
    // If there are a limited number of tiles? Enumerate slots...
    if(stTiles) for(size_t stSubTexId = 0;
                           stSubTexId < GetSubCount();
                         ++stSubTexId)
    { // Make sure theres enough memory allocated for each coord data
      clTiles[stSubTexId].reserve(stTiles);
      // Image is reversed?
      if(IsReversed())
        for(GLfloat fY = 0;
                    fY < fBTSizeY && GetTileCount(stSubTexId) < stTiles;
                    fY += fTPSizeY)
          for(GLfloat fX = 0;
                      fX < fBTSizeX && GetTileCount(stSubTexId) < stTiles;
                      fX += fTPSizeX)
            AddTileR(stSubTexId, fX, fY,
              fX + dfTile.DimGetWidth(), fY + dfTile.DimGetHeight());
      // Not reversed
      else for(GLfloat fY = 0;
                       fY < fBTSizeY && GetTileCount(stSubTexId) < stTiles;
                       fY += fTPSizeY)
        for(GLfloat fX = 0;
                    fX < fBTSizeX && GetTileCount(stSubTexId) < stTiles;
                    fX += fTPSizeX)
          AddTile(stSubTexId, fX, fY,
            fX + dfTile.DimGetWidth(), fY + dfTile.DimGetHeight());
      // Recover memory
      clTiles[stSubTexId].shrink_to_fit();
    } // No tiles overwrite
    else for(size_t stSubTexId = 0;
                    stSubTexId < GetSubCount();
                  ++stSubTexId)
    { // Make sure theres enough memory allocated for each coord data
      clTiles[stSubTexId].reserve(stTilesMax);
      // Image is reversed?
      if(IsReversed())
        for(GLfloat fY = 0; fY < fBTSizeY; fY += fTPSizeY)
          for(GLfloat fX = 0; fX < fBTSizeX; fX += fTPSizeX)
            AddTileR(stSubTexId, fX, fY,
              fX + dfTile.DimGetWidth(), fY + dfTile.DimGetHeight());
      // Not reversed
      else for(GLfloat fY = 0; fY < fBTSizeY; fY += fTPSizeY)
        for(GLfloat fX = 0; fX < fBTSizeX; fX += fTPSizeX)
          AddTile(stSubTexId, fX, fY,
            fX + dfTile.DimGetWidth(), fY + dfTile.DimGetHeight());
      // Recover memory
      clTiles[stSubTexId].shrink_to_fit();
    } // Log tiling data
    cLog->LogDebugExSafe("- Tile data: $/$ (B=$$x$;T=$x$;TP=$x$;BT=$x$).",
      GetTileCount(), GetSubCount(), fixed, setprecision(0), GetTileCount(),
      GetFWidth(), GetFHeight(), dfTile.DimGetWidth(),
      dfTile.DimGetHeight(), fTPSizeX, fTPSizeY, fBTSizeX, fBTSizeY);
  }
  /* -- Load texture from image class -------------------------------------- */
  void LoadFromImage(void)
  { // Get number of slots in this image and return if zero
    if(IsNoSlots()) XC("No data in image object!", "Identifier", IdentGet());
    // Get first slot
    const ImageSlot &sData = GetSlotsConst().front();
    // Copy image dimensions to texture dimensions
    DimSet(sData);
    // Set image dimensions as float for opengl
    dfImage.DimSet(DimGetWidth<GLfloat>(), DimGetHeight<GLfloat>());
    // Check width and height's are valid for the graphics device
    const unsigned int uiMaxSize = cOgl->MaxTexSize();
    if(sData.DimIsNotSet() || sData.DimGetWidth() > uiMaxSize ||
                              sData.DimGetHeight() > uiMaxSize)
      XC("Image size not supported by graphics hardware!",
         "Identifier", IdentGet(),           "Width",   sData.DimGetWidth(),
         "Height",     sData.DimGetHeight(), "Maximum", uiMaxSize);
    // Texture is uncompressed by default
    bool bCompressed = false;
    // Internal pixel type chosen
    GLint iICFormat;
    // What is the colour type as we need to handle it differently
    switch(GetBytesPerPixel())
    { // Real possible values
      case 1: iICFormat = GL_RED;  break; //  8-bpp (grayscale)
      case 2: iICFormat = GL_RG;   break; // 16-bpp (grayscale + alpha)
      case 3: iICFormat = GL_RGB;  break; // 24-bpp (RGB)
      case 4: iICFormat = GL_RGBA; break; // 32-bpp (RGBA)
      // Unknown, bail out
      default: XC("Unsupported texture colour type!",
        "Identifier",   IdentGet(), "BytesPerPixel", GetBytesPerPixel(),
        "BitsPerPixel", GetBitsPerPixel());
    } // Calculated internal and external format
    GLenum eNXCFormat;
    // Because we're using shaders we can trick OpenGL into giving it a
    // non-standard colour encoded raster image and the shaders can decode
    // it on the fly. This variable will redefine the external colour format
    // if neccesary on anything other than a RGB or RGBA image.
    switch(iICFormat)
    { // Texture is 8-bpp?
      case GL_RED:
        // Is palleted?
        if(IsPalette())
        { // Set palette shader
          shProgram = &cFboBase->sh2D8Pal;
          // Force no filtering and no mipmapping or we get the wrong colours
          stTexFilter = 0;
          iTexMinFilter = iTexMagFilter = GL_NEAREST;
        } // No palette
        else shProgram = &cFboBase->sh2D8;
        // Set requested external format
        eNXCFormat = GetPixelType();
        break;
      // Texture is 16-bpp?
      case GL_RG:
        // Set GL_LUMINANCE_ALPHA decoding shader
        shProgram = &cFboBase->sh2D16;
        eNXCFormat = GetPixelType();
        break;
      // Texture is 24 or 32-bpp? Whats the external format?
      case GL_RGB: case GL_RGBA: switch(GetPixelType())
      { // Compressed texture?
        case 0x83F1: case 0x83F2: case 0x83F3:
          // Set compressed texture
          eNXCFormat = GetPixelType();
          shProgram = &cFboBase->sh2D;
          bCompressed = true;
          break;
        // BGRA colour order type?
        case GL_BGRA:
          // Use BGR shader and redefine to RGBA.
          shProgram = &cFboBase->sh2DBGR;
          eNXCFormat = GL_RGBA;
          break;
        // BGR colour order type?
        case GL_BGR:
          // Use BGR shader and redefine to RGB.
          shProgram = &cFboBase->sh2DBGR;
          eNXCFormat = GL_RGB;
          break;
        // RGBA or RGB (Normal image).
        case GL_RGB: case GL_RGBA:
          // Use RGB shader. No format change.
          shProgram = &cFboBase->sh2D;
          eNXCFormat = GetPixelType();
          break;
        // Unknown external format
        default: XC("External colour type not acceptable!",
          "Identifier", IdentGet(), "XCFormat", GetPixelType());
      } // The pixel type is valid so break
      break;
      // Unknown internal format
      default: XC("Internal colour type not acceptable!",
        "Identifier", IdentGet(), "ICFormat", iICFormat,
        "XCFormat", GetPixelType());
    } // Set usable slots
    const size_t stSlots = GetSlotCount() - static_cast<size_t>(IsPalette());
    // Upload the texture
    UploadTexture(stSlots, bCompressed, sData, iICFormat, eNXCFormat);
    // Log progress
    cLog->LogDebugExSafe("Texture '$'[$:$;M:$;D:$x$] uploaded.", IdentGet(),
      bCompressed ? 'C' : 'U', stSlots, GetMipmaps(), sData.DimGetWidth(),
      sData.DimGetHeight());
  }
  /* -- Blit a triangle ---------------------------------------------------- */
  void BlitTri(const GLuint uiGLTexId, const TriCoordData &tcoData,
    const TriPosData &tpData, const TriColData &tcData)
  { FboActive()->Blit(uiGLTexId, tpData, tcoData, tcData, 0, shProgram); }
  /* -- Blit two triangles that form a square ------------------------------ */
  void BlitQuad(const GLuint uiGLTexId, const QuadCoordData &qcoData,
    const QuadPosData &qpData, const QuadColData &acData)
  { for(size_t stTriId = 0; stTriId < stTrisPerQuad; ++stTriId)
      BlitTri(uiGLTexId, qcoData[stTriId], qpData[stTriId],
        acData[stTriId]); }
  /* -- Blit with currently stored position -------------------------------- */
  void Blit(const size_t stSubTexId, const size_t stTileId)
    { BlitQuad(GetSubName(stSubTexId), clTiles[stSubTexId][stTileId],
        GetVData(), GetCData()); }
  /* -- Blit with currently stored position, texture and colour ------------ */
  void BlitStored(const size_t stSubTexId)
    { BlitQuad(GetSubName(stSubTexId), GetTCData(), GetVData(), GetCData()); }
  /* -- Blit specified triangle with currently stored position ------------- */
  void BlitT(const size_t stTriId, const size_t stTexId, const size_t stTileId)
    { BlitTri(GetSubName(stTexId), clTiles[stTexId][stTileId][stTriId],
        GetVData(stTriId), GetCData(stTriId)); }
  /* -- Blit quad with position and stored size ---------------------------- */
  void BlitLT(const size_t stSubTexId, const size_t stTileId, const GLfloat fX,
    const GLfloat fY)
  { const CoordData &tcItem = clTiles[stSubTexId][stTileId];
    BlitQuad(GetSubName(stSubTexId), tcItem,
      SetAndGetVertex(fX, fY, fX+tcItem.DimGetWidth(),
        fY+tcItem.DimGetHeight()), GetCData()); }
  /* -- Blit quad with custom colour (used by font) ------------------------ */
  void BlitLTRBC(const size_t stSubTexId, const size_t stTileId,
    const GLfloat fX1, const GLfloat fY1, const GLfloat fX2, const GLfloat fY2,
    const QuadColData &faC)
  { BlitQuad(GetSubName(stSubTexId), clTiles[stSubTexId][stTileId],
    SetAndGetVertex(fX1, fY1, fX2, fY2), faC); }
  /* -- Blit quad with free co-ordinates and width ------------------------- */
  void BlitLTRB(const size_t stSubTexId, const size_t stTileId,
    const GLfloat fX1, const GLfloat fY1, const GLfloat fX2, const GLfloat fY2)
  { BlitLTRBC(stSubTexId, stTileId, fX1, fY1, fX2, fY2, GetCData()); }
  /* -- Blit quad with truncated texcoord width and colour ----------------- */
  void BlitLTRBSC(const size_t stSubTexId, const size_t stTileId,
    const GLfloat fX1, const GLfloat fY1, const GLfloat fX2, const GLfloat fY2,
    const GLfloat fML, const GLfloat fMR, const QuadColData &faC)
  { BlitQuad(GetSubName(stSubTexId),
    SetAndGetCoord(clTiles[stSubTexId][stTileId], fML, fMR),
    SetAndGetVertex(fX1, fY1, fX2, fY2, fML, fMR), faC); }
  /* -- Blit quad with truncated texcoord width ---------------------------- */
  void BlitLTRBS(const size_t stSubTexId, const size_t stTileId,
    const GLfloat fX1, const GLfloat fY1, const GLfloat fX2,
    const GLfloat fY2, const GLfloat fML, const GLfloat fMR)
  { BlitLTRBSC(stSubTexId, stTileId, fX1, fY1, fX2, fY2, fML, fMR,
      GetCData()); }
  /* -- Blit quad with position, stored size and with angle ---------------- */
  void BlitLTA(const size_t stSubTexId, const size_t stTileId,
    const GLfloat fX, const GLfloat fY, const GLfloat fA)
  { const CoordData &tcItem = clTiles[stSubTexId][stTileId];
    BlitQuad(GetSubName(stSubTexId), tcItem,
      SetAndGetVertex(fX, fY, fX+tcItem.DimGetWidth(),
        fY+tcItem.DimGetHeight(), fA), GetCData()); }
  /* -- Blit quad with full bounds and angle ------------------------------- */
  void BlitLTRBA(const size_t stSubTexId, const size_t stTileId,
    const GLfloat fX1, const GLfloat fY1, const GLfloat fX2, const GLfloat fY2,
    const GLfloat fA)
  { BlitQuad(GetSubName(stSubTexId), clTiles[stSubTexId][stTileId],
    SetAndGetVertex(fX1, fY1, fX2, fY2, fA), GetCData()); }
  /* -- Blit all quads as full image --------------------------------------- */
  void BlitMulti(const GLuint uiColumns, const GLfloat fL, const GLfloat fT,
    const GLfloat fR, const GLfloat fB)
  { // Calculate perspective width
    const GLfloat
      fNewWidth = (fR - fL) / uiColumns,
      fNewHeight = (fB - fT) / (GetTileCount() / uiColumns);
    // For each image
    for(size_t stI = 0; stI < GetTileCount(); ++stI)
    { // Calculate X, Y co-ordinate
      const GLfloat
        fLeft = fL + (stI % uiColumns) * fNewWidth,
        fTop = fT + (stI / uiColumns) * fNewHeight;
      // Blit the texture
      BlitLTRB(0, stI, fLeft, fTop, fLeft+fNewWidth, fTop+fNewHeight);
    }
  }
  /* -- Convert co-ordinates to absolute position -------------------------- */
  static size_t CoordsToAbsolute(const size_t stPosX, const size_t stPosY,
    const size_t stWidth, const size_t stBytesPerColumn=1)
  { return ((stPosY * stWidth) + stPosX) * stBytesPerColumn; }
  /* -- Replace partial texture in VRAM from raw data ---------------------- */
  void UpdateEx(const GLuint uiTexId, const GLint iX, const GLint iY,
    const GLsizei uiW, const GLsizei uiH, const GLenum ePixType,
    const GLvoid*const vpData)
  { // Bind the specified texture
    GL(cOgl->BindTexture(uiTexId), "Failed to bind texture to update!",
      "Identifier", IdentGet(), "TexId", uiTexId);
    // Upload the texture area
    GL(cOgl->UploadTextureSub(iX, iY, uiW, uiH, ePixType, vpData),
      "Failed to update VRAM with image!",
      "Identifier", IdentGet(), "OffsetX", iX,
      "OffsetY",    iY,         "Width",   uiW,
      "Height",     uiH,        "SrcType", cOgl->GetPixelFormat(ePixType));
    // Whats the minification value? We might need to regenerate mipmaps! If
    // we already had mipmaps, they will be overwritten.
    ReGenerateMipmaps();
    // Write that we updated the VRAM
    cLog->LogDebugExSafe("Texture '$'[$<$x$>] updated at $x$ with type $!",
      IdentGet(), uiTexId, uiW, uiH, iX, iY, cOgl->GetPixelFormat(ePixType));
  }
  /* -- Replace partial texture in VRAM from partial raw data -------------- */
  void UpdateEx(const GLuint uiTexId, const GLint iX, const GLint iY,
    const GLsizei stW, const GLsizei stH, const GLenum ePixType,
    const GLvoid*const vpData, const GLsizei stStride)
  { // Set stride and alignment
    GL(cOgl->SetUnpackRowLength(stStride),
      "Failed to set unpack row length!",
      "Texture", IdentGet(), "Stride", stStride);
    // Update the texture
    UpdateEx(uiTexId, iX, iY, stW, stH, ePixType, vpData);
    // Reset stride and alignment
    GL(cOgl->SetUnpackRowLength(cOgl->UnpackRowLength()),
      "Failed to restore unpack row length!",
      "Texture", IdentGet(), "Stride", cOgl->UnpackRowLength());
  }
  /* -- Replace partial texture in VRAM from array ------------------------- */
  void UpdateEx(const size_t stSubTexId, Image &imS,
    const GLint iX, const GLint iY)
  { // Do the update
    UpdateEx(GetSubName(stSubTexId), iX, iY, imS.DimGetWidth<GLsizei>(),
      imS.DimGetHeight<GLsizei>(), imS.GetPixelType(),
      imS.GetSlotsConst().front().Ptr());
  }
  /* -- Replace texture in VRAM from array --------------------------------- */
  void Update(Image &imOther)
  { // Deinit existing texture
    DeInit();
    // Replace image
    SwapImage(imOther);
    // Reload texture
    ReloadTexture();
  }
  /* -- Download texture to array ------------------------------------------ */
  Image Download(const size_t stSubTexId, const BitDepth bdDDepth=BD_RGBA,
    const ByteDepth byDDepth=BY_RGBA, const GLenum ePixType=GL_RGBA) const
  { // Get texture id and bind it
    GL(cOgl->BindTexture(GetSubName(stSubTexId)),
      "Failed to bind texture to download!",
      "Identifier", IdentGet(), "Index", stSubTexId);
    // Create memory block for texture data and read RGBA texture into it
    Memory mOut{ DimGetWidth() * DimGetHeight() * byDDepth };
    GL(cOgl->ReadTexture(ePixType, mOut.Ptr<GLvoid>()),
      "Download texture failed!",
      "Identifier", IdentGet(), "Index", stSubTexId,
      "Format",     cOgl->GetPixelFormat(ePixType));
    // Return a newly created image class containing this data
    return Image{ IdentGet(), StdMove(mOut), DimGetWidth(), DimGetHeight(),
      bdDDepth, ePixType };
  }
  /* -- Download texture and dump it to disk ------------------------------- */
  void Dump(const size_t stSubTexId, const string &strFileName) const
    { Download(stSubTexId).SaveFile(strFileName, stSubTexId, 0); }
  /* -- Check if texture is initialised ------------------------------------ */
  bool IsNotInitialised(void) const { return vTexture.empty(); }
  bool IsInitialised(void) const { return !IsNotInitialised(); }
  /* -- Reload texture array as normal texture ----------------------------- */
  void ReloadTexture(void)
  { // If image was not loaded from disk? Just (re)load the image data
    // that already should be there and should NEVER be released.
    if(IsDynamic()) LoadFromImage();
    // Texture is static?
    else
    { // It can just be reloaded from disk
      ReloadData();
      // Load the image into memory
      LoadFromImage();
      // Remove all image data because we can just load it from file again
      // and theres no point taking up precious memory for it.
      ClearSlots();
    } // Error if no textures loaded
    if(IsNotInitialised()) XC("No textures loaded!", "Identifier", IdentGet());
    // Show what was loaded
    cLog->LogInfoExSafe("Texture loaded $ textures from '$'!",
      GetSubCount(), IdentGet());
  }
  /* -- Init from a image class ------------------------------------------- */
  void InitImage(Image &imgSrc, const GLuint uiTileWidth,
    const GLuint uiTileHeight, const GLuint uiPadX, const GLuint uiPadY,
    const size_t stFilter, const bool bGenerateTileset = true)
  { // Show filename
    cLog->LogDebugExSafe("Texture loading from image '$'.", imgSrc.IdentGet());
    // If source and destination image class are not the same?
    if(this != &imgSrc)
    { // Move image data over. The old image will be unusable so guest should
      // discard it.
      SwapImage(imgSrc);
      // The image passed in the arguments is usually still allocated by LUA
      // and will still be registered, so lets put a note in the image to show
      // that this function has nicked by this image class.
      imgSrc.IdentSet("!TEX!$!", IdentGet());
      // Image now being used in a Texture class
      ClearPurposeImage();
      SetPurposeTexture();
    } // We'll set flaot versions for faster calculations later on
    dfPad.DimSet(static_cast<GLfloat>(uiPadX), static_cast<GLfloat>(uiPadY));
    // Set filter
    stTexFilter = stFilter;
    cOgl->SetMipMapFilterById(stFilter, iTexMinFilter, iTexMagFilter);
    // Set default tint
    SetQuadRGBA(1, 1, 1, 1);
    // Initialise
    LoadFromImage();
    // Set specified tile dimensions and generate default tileset if needed
    duTile.DimSet(uiTileWidth, uiTileHeight);
    if(bGenerateTileset) GenerateTileset();
    // Remove all image data because we can just load it from file again
    // and theres no point taking up precious memory for it.
    if(IsNotDynamic()) ClearSlots();
  }
  /* -- Deinitialise ------------------------------------------------------- */
  void DeInit(void)
  { // Texture not loaded? return
    if(IsNotInitialised() || FlagIsSet(TF_DELETE)) return;
    // Mark texture for deletion (explanation in fbos class)
    cOgl->SetDeleteTextures(vTexture);
    // Texture has been marked for deleting
    FlagSet(TF_DELETE);
    // Report de-initialisation and texture memory
    cLog->LogInfoExSafe("Texture '$' with $ textures marked for deletion.",
      IdentGet(), GetSubCount());
  }
  /* -- Constructor (Initialisation then registration) --------------------- */
  Texture(void) :                      // No parameters
    /* -- Initialisers ----------------------------------------------------- */
    ICHelperTexture{ *cTextures,this } // Automatic (de)registration
    /* -- Code ------------------------------------------------------------- */
    { }                                // Do nothing else
  /* -- Constructor (No registration, base class of Font class) ------------ */
  explicit Texture(const bool) :       // Parameter does nothing
    /* -- Initialisers ----------------------------------------------------- */
    ICHelperTexture{ *cTextures }      // Initially unregistered
    /* -- Code ------------------------------------------------------------- */
    { }                                // Do nothing else
  /* -- Destructor (Unregistration then deinitialisation) ------------------ */
  ~Texture(void) { DeInit(); }
  /* ----------------------------------------------------------------------- */
  DELETECOPYCTORS(Texture)             // Supress copy constructor for safety
};/* -- Finish the collector ----------------------------------------------- */
END_COLLECTOR(Textures)                // Finish collector class
/* -- DeInit Textures ------------------------------------------------------ */
static void TextureDeInitTextures(void)
{ // Ignore if no textures
  if(cTextures->empty()) return;
  // De-init all the textures and log the pre/post init of them
  cLog->LogDebugExSafe("Textures de-initialising $ objects...",
    cTextures->size());
  for(Texture*const tCptr : *cTextures) tCptr->DeInit();
  cLog->LogInfoExSafe("Textures de-initialised $ objects.",
    cTextures->size());
}
/* -- Reload Textures ------------------------------------------------------ */
static void TextureReInitTextures(void)
{ // Ignore if no textures
  if(cTextures->empty()) return;
  // Reload all the textures and log the pre/post init of them
  cLog->LogDebugExSafe("Textures re-initialising $ objects...",
    cTextures->size());
  for(Texture*const tCptr : *cTextures) tCptr->ReloadTexture();
  cLog->LogInfoExSafe("Textures re-initialised $ objects.",
    cTextures->size());
}
/* ------------------------------------------------------------------------- */
};                                     // End of module namespace
/* == EoF =========================================================== EoF == */

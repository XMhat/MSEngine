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
/* -- Module namespace ----------------------------------------------------- */
namespace IfTexture {                  // Keep declarations neatly categorised
/* -- Includes ------------------------------------------------------------- */
using namespace IfFbo;                 // Using fbo interface
using namespace IfImage;               // Using image interface
/* -- Texture collector class for collector data and custom variables ------ */
BEGIN_COLLECTOR(Textures, Texture, CLHelperUnsafe);
/* == Texture variables class ============================================== */
// Only put vars used in the Texture class in here. This is an optimisation so
// we do not have to initialise all these variables more than once as we have
// more than one constructor in the main Texture class.
/* ------------------------------------------------------------------------- */
class TextureVars :                    // Texture variables class
  /* -- Base classes ------------------------------------------------------- */
  public FboItem,                      // Fbo item class
  public Lockable,                     // Lua lock class
  public Ident                         // Identifier class
{ /* -- Variables ----------------------------------------------- */ protected:
  GLint            iTexMinFilter,      // GL minification setting
                   iTexMagFilter;      // GL magnification setting
  bool             bReversed;          // Image data is reversed?
  GLint            iMipmaps;           // Sub-image's are mipmaps
  size_t           stTexFilter;        // Texture filter (for reference)
  /* --------------------------------------------------------------- */ public:
  struct CoordData                     // Tile coordinates data
  { GLfloat        fW, fH;             // Width and height of tile
    QuadCoordData  fTC; };             // Texture co-ordinates data
  typedef vector<CoordData> CoordList; // Tile coordinates data list
  /* ----------------------------------------------------------------------- */
  CoordList        tcList;             // Texture coordinates for tiles
  GLUIntVector     vTexture;           // Texture list
  GLuint           uiBWidth,uiBHeight, // Texture image width and height
                   uiTWidth,uiTHeight, // Texture tile width and height
                   uiBytesPerPixel,    // Texture bytes per pixel
                   uiBitsPerPixel;     // Texture bits per pixel
  Shader          *shProgram;          // Default shader program to use
  GLenum           glPixelType;        // Image pixel-type
  GLfloat          fPadX, fPadY,       // Texture tile padding
                   fBWidth, fBHeight,  // Texture image width and height
                   fTWidth, fTHeight;  // Texture tile width and height
  bool             bGenerateTileset;   // Automatically generate a tileset?
  bool             bMarkedForDeletion; // Texture has been marked for deletion
  /* ----------------------------------------------------------------------- */
  // Because openGL cannot preserve it's data in VRAM after a GPU or mode reset
  // we have to store any dynamic data for reloading. This image will make
  // sure that data is preserved. Anything static (i.e. from a file) does not
  // need to be preserved as we can just reload it from disk to save main RAM.
  /* ----------------------------------------------------------------------- */
  Image            imData;             // Image class
  /* -- Constructor -------------------------------------------------------- */
  TextureVars(void) :
    /* -- Initialisation of members ---------------------------------------- */
    iTexMinFilter(GL_NONE),            iTexMagFilter(GL_NONE),
    bReversed(false),                  iMipmaps(0),
    stTexFilter(0),                    uiBWidth(0),
    uiBHeight(0),                      uiTWidth(0),
    uiTHeight(0),                      uiBytesPerPixel(0),
    uiBitsPerPixel(0),                 shProgram(nullptr),
    glPixelType(GL_NONE),              fPadX(0),
    fPadY(0),                          fBWidth(0),
    fBHeight(0),                       fTWidth(0),
    fTHeight(0),                       bGenerateTileset(true),
    bMarkedForDeletion(false)
    /* -- No code ---------------------------------------------------------- */
    { }
  /* ----------------------------------------------------------------------- */
  DELETECOPYCTORS(TextureVars);        // No defaults
};/* ----------------------------------------------------------------------- */
/* == Texture object class ================================================= */
BEGIN_MEMBERCLASS(Textures, Texture, ICHelperUnsafe),
  /* -- Base classes ------------------------------------------------------- */
  public TextureVars                   // Texture class variables
{ /* -- Tileset information ---------------------------------------- */ public:
  GLfloat GetPaddingWidth(void) const { return fPadX; }
  GLfloat GetPaddingHeight(void) const { return fPadY; }
  GLuint GetTileWidth(void) const { return uiTWidth; }
  GLuint GetTileHeight(void) const { return uiTHeight; }
  size_t GetTileCount(void) const { return tcList.size(); }
  void SetTileCount(const size_t stC) { tcList.resize(stC); }
  /* -- Texture information ------------------------------------------------ */
  GLuint GetWidth(void) const { return uiBWidth; }
  GLfloat GetFWidth(void) const { return fBWidth; }
  GLuint GetHeight(void) const { return uiBHeight; }
  GLfloat GetFHeight(void) const { return fBHeight; }
  GLuint GetBitsPerPixel(void) const { return uiBitsPerPixel; }
  GLuint GetBytesPerPixel(void) const { return uiBytesPerPixel; }
  GLint GetMipmaps(void) const { return iMipmaps; }
  bool GetReversed(void) const { return bReversed; }
  size_t GetTexFilter(void) const { return stTexFilter; }
  GLenum GetPixelFormat(void) const { return glPixelType; }
  GLuint GetSubName(const size_t stIndex) const { return vTexture[stIndex]; }
  size_t GetSubCount(void) const { return vTexture.size(); }
  /* -- Return a new tile -------------------------------------------------- */
  const CoordData NewTile(const GLfloat fL, const GLfloat fT, const GLfloat fR,
    const GLfloat fB, const GLfloat fW, const GLfloat fH)
  { // Calculate absolute position as texture position (0<->1f)
    const GLfloat fLeft =   fL/fBWidth,   fRight =   fR/fBWidth,
                  fTop  = 1-fT/fBHeight, fBottom = 1-fB/fBHeight;
    // Set tile info
    return { fW, fH,                   // Width and height
      // Pre-cached texture co-ordinates
      { { { fLeft,  fTop,              // T0\X0 - X0 X1
            fRight, fTop,              // T0\X1 - X2 /
            fLeft,  fBottom },         // T0\X2 -
          { fRight, fBottom,           // T1\X0 -
            fLeft,  fBottom,           // T1\X1 -  / X2
            fRight, fTop    } } }      // T1\X2 - X1 X0
    };
  }
  /* -- Set a tile --------------------------------------------------------- */
  void SetTile(const size_t stId, const GLfloat fL, const GLfloat fT,
                                  const GLfloat fR, const GLfloat fB,
                                  const GLfloat fW, const GLfloat fH)
    { tcList[stId] = NewTile(fL, fT, fR, fB, fW, fH); }
  /* -- Set a reversed tile ------------------------------------------------ */
  void SetTile(const size_t stId, const GLfloat fL, const GLfloat fT,
                                  const GLfloat fR, const GLfloat fB)
    { SetTile(stId, fL, fT, fR, fB, fR-fL, fB-fT); }
  /* -- Set a reversed tile ------------------------------------------------ */
  void SetTileR(const size_t stId, const GLfloat fL,const GLfloat fT,
                                   const GLfloat fR,const GLfloat fB)
    { const GLfloat fNT = fBHeight-fT, fNB = fBHeight-fB;
      SetTile(stId, fL, fNT, fR, fNB, fR-fL, fNT-fNB); }
  /* -- Set a tile based on reversal --------------------------------------- */
  void SetTileDOR(const size_t stId, const GLfloat fL, const GLfloat fT,
                                     const GLfloat fR, const GLfloat fB)
    { if(bReversed) SetTileR(stId, fL, fT, fR, fB);
               else SetTile(stId, fL, fT, fR, fB); }
  /* -- Add a tile with width and height ----------------------------------- */
  void SetTileWH(const size_t stId, const GLfloat fL, const GLfloat fT,
                                    const GLfloat fW, const GLfloat fH)
  { // Calculate new right and bottom and add the tile
    const GLfloat fNR = fL+fW, fNB = fT+fH;
    SetTile(stId, fL, fT, fNR, fNB, fNR-fL, fNB-fT);
  }
  /* -- Add a reversed tile with width and height setting ------------------ */
  void SetTileRWH(const size_t stId, const GLfloat fL, const GLfloat fT,
                                     const GLfloat fW, const GLfloat fH)
  { // Calculate new top, right and bottom and add the tile
    const GLfloat fNT = fBHeight-fT, fNB = fNT+fH, fNR = fL+fW;
    SetTile(stId, fL, fT, fNR, fNB, fNR-fL, fNT-fNB);
  }
  /* -- Set a tile based on reversal with width and height setting --------- */
  void SetTileDORWH(const size_t stId, const GLfloat fL, const GLfloat fT,
                                       const GLfloat fW, const GLfloat fH)
    { if(bReversed) SetTileRWH(stId, fL, fT, fW, fH);
               else SetTileWH(stId, fL, fT, fW, fH); }
  /* -- Do add a tile with custom width and height setting ----------------- */
  void AddTile(const GLfloat fL, const GLfloat fT, const GLfloat fR,
               const GLfloat fB, const GLfloat fW, const GLfloat fH)
    { tcList.emplace_back(NewTile(fL, fT, fR, fB, fW, fH)); }
  /* -- Add a tile --------------------------------------------------------- */
  void AddTile(const GLfloat fL, const GLfloat fT,
               const GLfloat fR, const GLfloat fB)
    { AddTile(fL, fT, fR, fB, fR-fL, fB-fT); }
  /* -- Add a reversed tile ------------------------------------------------ */
  void AddTileR(const GLfloat fL, const GLfloat fT,
                const GLfloat fR, const GLfloat fB)
    { const GLfloat fNT = fBHeight-fT, fNB = fBHeight-fB;
      AddTile(fL, fNT, fR, fNB, fR-fL, fNT-fNB); }
  /* -- Add a tile based on reversal setting ------------------------------- */
  void AddTileDOR(const GLfloat fL, const GLfloat fT,
                  const GLfloat fR, const GLfloat fB)
    { if(bReversed) AddTileR(fL, fT, fR, fB);
               else AddTile(fL, fT, fR, fB); }
  /* -- Add a tile with width and height ----------------------------------- */
  void AddTileWH(const GLfloat fL, const GLfloat fT,
                 const GLfloat fW, const GLfloat fH)
  { // Calculate new right and bottom and add the tile
    const GLfloat fNR = fL+fW, fNB = fT+fH;
    AddTile(fL, fT, fNR, fNB, fNR-fL, fNB-fT);
  }
  /* -- Add a reversed tile with width and height -------------------------- */
  void AddTileRWH(const GLfloat fL, const GLfloat fT,
                  const GLfloat fW, const GLfloat fH)
  { // Calculate new top, right and bottom and add the tile
    const GLfloat fNT = fBHeight-fT, fNB = fNT+fH, fNR = fL+fW;
    AddTile(fL, fT, fNR, fNB, fNR-fL, fNT-fNB);
  }
  /* -- Add a tile with width and height based on reversal ----------------- */
  void AddTileDORWH(const GLfloat fL, const GLfloat fT,
                    const GLfloat fW, const GLfloat fH)
    { if(bReversed) AddTileRWH(fL, fT, fW, fH);
               else AddTileWH(fL, fT, fW, fH); }
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
  void ConfigureTexture(const size_t stIndex)
  { // Start configuring the texture.
    GL(cOgl->SetTexParam(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE),
      "Could not set texture wrapping S!",
      "Identifier", IdentGet(), "Index", stIndex);
    GL(cOgl->SetTexParam(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE),
      "Could not set texture wrapping T!",
      "Identifier", IdentGet(), "Index", stIndex);
    GL(cOgl->SetTexParam(GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE),
      "Could not set texture wrapping R!",
      "Identifier", IdentGet(), "Index", stIndex);
    // Set filtering based on developers setting
    GL(cOgl->SetTexParam(GL_TEXTURE_MIN_FILTER, iTexMinFilter),
      "Could not set texture minifaction filter!",
      "Identifier", IdentGet(), "Index", stIndex, "MinFilter",  iTexMinFilter);
    GL(cOgl->SetTexParam(GL_TEXTURE_MAG_FILTER, iTexMagFilter),
      "Could not set texture magnification filter!",
      "Identifier", IdentGet(), "Index", stIndex, "MagFilter",  iTexMagFilter);
  }
  /* -- Load the specified compressed image -------------------------------- */
  void LoadCompressedTexture(const size_t stIndex, const GLint iMipLevel,
    const ImageSlot &sData)
  { // Upload pre-compressed texture to video ram
    GL(cOgl->UploadCompressedTexture(iMipLevel,
      glPixelType,
      static_cast<GLsizei>(sData.uiWidth),
      static_cast<GLsizei>(sData.uiHeight),
      sData.memData.Size<GLsizei>(),
      sData.memData.Ptr()),
        "Could not upload compressed texture to video ram!",
        "Identifier", IdentGet(),             "Index",  stIndex,
        "TexId",      GetSubName(stIndex), "Level",  iMipLevel,
        "Width",      sData.uiWidth,       "Height", sData.uiHeight,
        "XCFormat",   glPixelType,         "Size",   sData.memData.Size(),
        "Data",       sData.memData.Ptr<void>());
  }
  /* -- Load the specified image ------------------------------------------- */
  void LoadUncompressedTexture(const size_t stIndex, const GLint iMipLevel,
    const GLint iNICFormat, const GLenum eNXCFormat, const ImageSlot &sData)
  { // Upload uncompressed texture to video ram
    GL(cOgl->UploadTexture(iMipLevel,
      static_cast<GLsizei>(sData.uiWidth),
      static_cast<GLsizei>(sData.uiHeight),
      iNICFormat,
      eNXCFormat,
      sData.memData.Ptr()),
        "Could not upload uncompressed texture to video ram!",
        "Identifier", IdentGet(),             "Index",     stIndex,
        "TexId",      GetSubName(stIndex), "Level",     iMipLevel,
        "Width",      sData.uiWidth,       "Height",    sData.uiHeight,
        "XCFormat",   glPixelType,         "NICFormat", iNICFormat,
        "NXCFormat",  eNXCFormat,          "Size",      sData.memData.Size(),
        "Data",       sData.memData.Ptr<void>());
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
    bMarkedForDeletion = false;
    // If no mipmaps in this bitmap?
    if(!imData.IsMipmap())
    { // Image does not contain mipmaps?. No mipmaps
      iMipmaps = 0;
      // Create texture handles
      CreateTextureHandles(stSlots);
      // For each image slot
      for(size_t stIndex = 0; stIndex < stSlots; ++stIndex)
      { // Get next slot and verify that dimensions are different from slot 0
        const ImageSlot &sSlot = imData.GetSlot(stIndex);
        if(uiBWidth != sSlot.uiWidth || uiBHeight != sSlot.uiHeight)
          XC("Alternating image sizes are not supported!",
             "Identifier", IdentGet(),   "LastWidth",  uiBWidth,
             "LastHeight", uiBHeight, "ThisWidth",  sSlot.uiWidth,
             "ThisHeight", sSlot.uiHeight);
        // Get texture id and bind it
        const unsigned int uiTexId = GetSubName(stIndex);
        GL(cOgl->BindTexture(uiTexId), "Texture id failed to bind!",
          "Identifier", IdentGet(), "Index", stIndex,
          "Maximum",    stSlots, "TexId", uiTexId);
        // Configure the texture
        ConfigureTexture(stIndex);
        // Load the image
        if(bCompressed)
          LoadCompressedTexture(stIndex, 0, sSlot);
        else
          LoadUncompressedTexture(stIndex, 0, iNICFormat, eNXCFormat, sSlot);
        // Automatically generate mipmaps if requested. This has to be done
        // for each uploaded texture
        ReGenerateMipmaps();
      } // Done
      return;
    } // Bitmap has mipmaps so record number of bitmaps we have
    iMipmaps = static_cast<GLint>(stSlots);
    // Create only one texture handle as other slots are for mipmaps
    CreateTextureHandles(1);
    // Get texture id
    const unsigned int uiTexId = GetSubName(0);
    // Bind the texture id
    GL(cOgl->BindTexture(uiTexId), "Mipmapped texture failed to bind!",
      "Identifier", IdentGet(), "TexId", uiTexId);
    // Initialise mipmap counter
    GL(cOgl->SetTexParam(GL_TEXTURE_BASE_LEVEL, 0),
      "Failed to set max mipmap base level on texture!",
      "Identifier", IdentGet(), "TexId", uiTexId);
    GL(cOgl->SetTexParam(GL_TEXTURE_MAX_LEVEL, iMipmaps),
      "Failed to set max mipmap max level on texture!",
      "Identifier", IdentGet(), "TexId", uiTexId, "Count", iMipmaps);
    // Configure the texture
    ConfigureTexture(0);
    // Upload first mipmap
    if(bCompressed) LoadCompressedTexture(0, 0, sData);
    else LoadUncompressedTexture(0, 0, iNICFormat, eNXCFormat, sData);
    // Done if there more slots to load
    if(stSlots <= 1) return;
    // Last mipmap size
    unsigned int uiLWidth = uiBWidth, uiLHeight = uiBHeight;
    // Load all the other mipmaps
    for(size_t stIndex = 1; stIndex < stSlots; ++stIndex)
    { // Get next slot
      const ImageSlot &sSlot = imData.GetSlot(stIndex);
      // Make sure size is smaller than the last
      if(sSlot.uiWidth >= uiLWidth || sSlot.uiHeight >= uiLHeight)
        XC("Specified mipmap is not smaller than the last!",
           "Identifier", IdentGet(),   "LastWidth",  uiLWidth,
           "LastHeight", uiLHeight, "ThisWidth",  sSlot.uiWidth,
           "ThisHeight", sSlot.uiHeight);
      // Load the mipmap
      if(bCompressed)
        LoadCompressedTexture(0, static_cast<GLint>(stIndex), sSlot);
      else
        LoadUncompressedTexture(0, static_cast<GLint>(stIndex),
          iNICFormat, eNXCFormat, sSlot);
      // Update last mipmap size
      uiLWidth = uiBWidth;
      uiLHeight = uiBHeight;
    }
  }
  /* -- Load texture from image class -------------------------------------- */
  void LoadFromImage(void)
  { // Get number of slots in this image and return if zero
    if(imData.NoSlots())
      XC("No data in image object!", "Identifier", IdentGet());
    // Set pixel information
    uiBytesPerPixel = imData.GetBytesPP();
    uiBitsPerPixel = imData.GetBitsPP();
    glPixelType = imData.GetPixelType();
    // Store reversed setting of first slot
    bReversed = imData.IsReverse();
    // Get first slot
    const ImageSlot &sData = imData.GetFirstSlot();
    // Set image dimensions
    uiBWidth = sData.uiWidth;
    uiBHeight = sData.uiHeight;
    fBWidth = static_cast<GLfloat>(uiBWidth);
    fBHeight = static_cast<GLfloat>(uiBHeight);
    // Check width and height's are valid for the graphics device
    const unsigned int uiMaxSize = cOgl->MaxTexSize();
    if(sData.uiWidth < 1         || sData.uiHeight < 1 ||
       sData.uiWidth > uiMaxSize || sData.uiHeight > uiMaxSize)
      XC("Image size not supported by graphics hardware!",
        "Identifier", IdentGet(),        "Width",   sData.uiWidth,
        "Height",     sData.uiHeight, "Maximum", uiMaxSize);
    // Texture is uncompressed by default
    bool bCompressed = false;
    // Internal pixel type chosen
    GLint iICFormat;
    // What is the colour type as we need to handle it differently
    switch(uiBytesPerPixel)
    { // Real possible values
      case 1: iICFormat = GL_RED;  break; //  8-bpp (grayscale)
      case 2: iICFormat = GL_RG;   break; // 16-bpp (grayscale + alpha)
      case 3: iICFormat = GL_RGB;  break; // 24-bpp (RGB)
      case 4: iICFormat = GL_RGBA; break; // 32-bpp (RGBA)
      // Unknown, bail out
      default: XC("Unsupported texture colour type!",
        "Identifier",   IdentGet(), "BytesPerPixel", uiBytesPerPixel,
        "BitsPerPixel", uiBitsPerPixel);
    } // Calculated internal and external format
    GLint iNICFormat;
    GLenum eNXCFormat;
    // Because we're using shaders we can trick OpenGL into giving it a
    // non-standard colour encoded raster image and the shaders can decode
    // it on the fly. This variable will redefine the external colour format
    // if neccesary on anything other than a RGB or RGBA image.
    switch(iICFormat)
    { // Texture is 8-bpp? Set GL_LUMINANCE decoding shader
      case GL_RED: shProgram = &cFboBase->sh2D8;
                   eNXCFormat = glPixelType;
                   iNICFormat = iICFormat;
                   break;
      // Texture is 16-bpp? Set GL_LUMINANCE_ALPHA decoding shader
      case GL_RG: shProgram = &cFboBase->sh2D16;
                  eNXCFormat = glPixelType;
                  iNICFormat = iICFormat;
                  break;
      // Texture is 24 or 32-bpp? Whats the external format?
      case GL_RGB: case GL_RGBA: switch(glPixelType)
      { // Compressed texture
        case 0x83F1:
        case 0x83F2:
        case 0x83F3: iNICFormat = iICFormat;
                     eNXCFormat = glPixelType;
                     shProgram = &cFboBase->sh2D;
                     bCompressed = true;
                     break;
        // BGRA (Windows image). Use BGR shader and redefine to RGBA
        case GL_BGRA: shProgram = &cFboBase->sh2DBGR;
                      eNXCFormat = GL_RGBA;
                      iNICFormat = iICFormat;
                      break;
        // BGR (Windows image). Use BGR shader and redefine to RGB
        case GL_BGR: shProgram = &cFboBase->sh2DBGR;
                     eNXCFormat = GL_RGB;
                     iNICFormat = iICFormat;
                     break;
        // RGBA or RGB (Normal image). Use RGB shader. No format change.
        case GL_RGB:
        case GL_RGBA: shProgram = &cFboBase->sh2D;
                      eNXCFormat = glPixelType;
                      iNICFormat = iICFormat;
                      break;
        // Unknown external format
        default: XC("External colour type not acceptable!",
          "Identifier", IdentGet(), "XCFormat", glPixelType);
      } // The pixel type is valid so break
      break;
      // Unknown internal format
      default: XC("Internal colour type not acceptable!",
        "Identifier", IdentGet(), "ICFormat", iICFormat,
        "XCFormat", glPixelType);
    } // Auto-generate tileset?
    if(bGenerateTileset)
    { // No need to generate it again
      bGenerateTileset = false;
      // If tile dimensions are not set, use the image size
      if(!uiTWidth) uiTWidth = uiBWidth;
      if(!uiTHeight) uiTHeight = uiBHeight;
      fTWidth = static_cast<GLfloat>(uiTWidth);
      fTHeight = static_cast<GLfloat>(uiTHeight);
      // Tile width and height are equal or greater than image image
      if(uiTWidth >= uiBWidth && uiTHeight >= uiBHeight)
      { // Add default tile the size of the image
        if(bReversed) AddTileR(0, 0, fBWidth, fBHeight);
        else AddTile(0, 0, fBWidth, fBHeight);
      } // Tile dimensions are valid
      else
      { // Create clamped image size
        const GLfloat
          fTPSizeX = fTWidth+fPadX,
          fTPSizeY = fTHeight+fPadY,
          fBTSizeX = floor(fBWidth/fTPSizeX)*fTPSizeX,
          fBTSizeY = floor(fBHeight/fTPSizeY)*fTPSizeY;
        // Make sure theres enough memory allocated for each coord data
        tcList.reserve(static_cast<size_t>(
          floor(fBTSizeX/fTPSizeX)*floor(fBTSizeY/fTPSizeY)));
        // Image is reversed?
        if(bReversed)
          for(GLfloat fY = 0; fY < fBTSizeY; fY += fTPSizeY)
            for(GLfloat fX = 0; fX < fBTSizeX; fX += fTPSizeX)
              AddTileR(fX, fY, fX+fTWidth, fY+fTHeight);
        // Not reversed
        else for(GLfloat fY = 0; fY < fBTSizeY; fY += fTPSizeY)
          for(GLfloat fX = 0; fX < fBTSizeX; fX += fTPSizeX)
            AddTile(fX, fY, fX+fTWidth, fY+fTHeight);
        // Log tiling data
        LW(LH_DEBUG, "- Tile data: $ (B=$$x$;T=$x$;TP=$x$;BT=$x$).",
          tcList.size(), setprecision(0), tcList.size(), fBWidth,
          fBHeight, fTWidth, fTHeight, fTPSizeX, fTPSizeY, fBTSizeX, fBTSizeY);
      }
    } // Upload the texture
    UploadTexture(imData.GetSlots(),
      bCompressed, sData, iNICFormat, eNXCFormat);
    // Log progress
    LW(LH_DEBUG, "Texture '$'[$:$;M:$;D:$x$] uploaded.", IdentGet(),
      bCompressed ? 'C' : 'U', imData.GetSlots(), iMipmaps, sData.uiWidth,
      sData.uiHeight);
  }
  /* -- Blit a triangle ---------------------------------------------------- */
  void BlitTri(const GLuint uiTex, const TriCoordData &tcoData,
    const TriPosData &tpData, const TriColData &tcData)
      { FboActive()->Blit(uiTex, tpData, tcoData, tcData, 0, shProgram); }
  /* -- Blit two triangles that form a square ------------------------------ */
  void BlitQuad(const GLuint uiTex, const QuadCoordData &qcoData,
    const QuadPosData &qpData, const QuadColData &acData)
      { for(size_t stId = 0; stId < TRISPERQUAD; ++stId)
          BlitTri(uiTex, qcoData[stId], qpData[stId], acData[stId]); }
  /* -- Blit with currently stored position -------------------------------- */
  void Blit(const size_t stTexId, const size_t stSubTexId)
    { BlitQuad(GetSubName(stTexId), tcList[stSubTexId].fTC,
        GetVData(), GetCData()); }
  /* -- Blit with currently stored position, texture and colour ------------ */
  void BlitStored(const size_t stTexId)
    { BlitQuad(GetSubName(stTexId), GetTCData(), GetVData(), GetCData()); }
  /* -- Blit specified triangle with currently stored position ------------- */
  void BlitT(const size_t stTriId, const size_t stTexId,
     const size_t stSubTexId)
       { BlitTri(GetSubName(stTexId), tcList[stSubTexId].fTC[stTriId],
           GetVData(stTriId), GetCData(stTriId)); }
  /* -- Blit quad with position and stored size ---------------------------- */
  void BlitLT(const size_t stIndex, const size_t stSubIndex,
    const GLfloat fX, const GLfloat fY)
  { // Get specified texture size, and blit with new size
    const CoordData &tcItem = tcList[stSubIndex];
    BlitQuad(GetSubName(stIndex), tcItem.fTC,
      SetAndGetVertex(fX, fY, fX+tcItem.fW, fY+tcItem.fH), GetCData());
  }
  /* -- Blit quad with custom colour (used by font) ------------------------ */
  void BlitLTRBC(const size_t stIndex, const size_t stSubIndex,
    const GLfloat fX1, const GLfloat fY1, const GLfloat fX2,
    const GLfloat fY2, const QuadColData &faC)
      { BlitQuad(GetSubName(stIndex), tcList[stSubIndex].fTC,
        SetAndGetVertex(fX1, fY1, fX2, fY2), faC); }
  /* -- Blit quad with free co-ordinates and width ------------------------- */
  void BlitLTRB(const size_t stIndex, const size_t stSubIndex,
    const GLfloat fX1, const GLfloat fY1, const GLfloat fX2,
    const GLfloat fY2)
      { BlitLTRBC(stIndex, stSubIndex, fX1, fY1, fX2, fY2, GetCData()); }
  /* -- Blit quad with truncated texcoord width and colour ----------------- */
  void BlitLTRBSC(const size_t stIndex, const size_t stSubIndex,
    const GLfloat fX1, const GLfloat fY1, const GLfloat fX2,
    const GLfloat fY2, const GLfloat fML, const GLfloat fMR,
    const QuadColData &faC)
      { BlitQuad(GetSubName(stIndex), SetAndGetCoord(tcList[stSubIndex].fTC,
        fML, fMR), SetAndGetVertex(fX1, fY1, fX2, fY2, fML, fMR), faC);
  }
  /* -- Blit quad with truncated texcoord width ---------------------------- */
  void BlitLTRBS(const size_t stIndex, const size_t stSubIndex,
    const GLfloat fX1, const GLfloat fY1, const GLfloat fX2,
    const GLfloat fY2, const GLfloat fML, const GLfloat fMR)
      { BlitLTRBSC(stIndex, stSubIndex, fX1, fY1, fX2, fY2, fML, fMR,
         GetCData()); }
  /* -- Blit quad with position, stored size and with angle ---------------- */
  void BlitLTA(const size_t stIndex, const size_t stSubIndex,
    const GLfloat fX, const GLfloat fY, const GLfloat fA)
  { // Get default texture co-cordinates and set vertex data with rotation
    const CoordData &tcItem = tcList[stSubIndex];
    BlitQuad(GetSubName(stIndex), tcItem.fTC,
      SetAndGetVertex(fX, fY, fX+tcItem.fW, fY+tcItem.fH, fA), GetCData());
  }
  /* -- Blit quad with full bounds and angle ------------------------------- */
  void BlitLTRBA(const size_t stIndex, const size_t stSubIndex,
    const GLfloat fX1, const GLfloat fY1, const GLfloat fX2,
    const GLfloat fY2, const GLfloat fA)
      { BlitQuad(GetSubName(stIndex), tcList[stSubIndex].fTC,
        SetAndGetVertex(fX1, fY1, fX2, fY2, fA), GetCData()); }
  /* -- Blit all quads as full image --------------------------------------- */
  void BlitMulti(const GLuint uiColumns, const GLfloat fL,
    const GLfloat fT, const GLfloat fR, const GLfloat fB)
  { // Calculate perspective width
    const GLfloat
      fNewWidth = (fR - fL) / uiColumns,
      fNewHeight = (fB - fT) / (tcList.size() / uiColumns);
    // For each image
    for(size_t stI = 0; stI < tcList.size(); ++stI)
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
  void UpdateEx(const GLuint uiTexId,
    const GLint iX, const GLint iY, const GLsizei uiW, const GLsizei uiH,
    const GLenum ePixType, const GLvoid*const vpData)
  { // Bind the specified texture
    GL(cOgl->BindTexture(uiTexId), "Failed to bind texture to update!",
      "Identifier", IdentGet(), "TexId", uiTexId);
    // Upload the texture area
    GL(cOgl->UploadTextureSub(iX, iY, uiW, uiH, ePixType, vpData),
      "Failed to update VRAM with image!",
      "Identifier", IdentGet(), "OffsetX", iX,
      "OffsetY",    iY,      "Width",   uiW,
      "Height",     uiH,     "SrcType", ePixType);
    // Whats the minification value? We might need to regenerate mipmaps! If
    // we already had mipmaps, they will be overwritten.
    ReGenerateMipmaps();
  }
  /* -- Replace partial texture in VRAM from partial raw data -------------- */
  void UpdateEx(const GLuint uiTexId,
    const GLint iX, const GLint iY, const GLsizei stW, const GLsizei stH,
    const GLenum ePixType, const GLvoid*const vpData, const GLsizei stStride)
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
  void UpdateEx(const size_t stId, Image &imS,
    const GLint iX, const GLint iY)
  { // Do the update
    UpdateEx(GetSubName(stId), iX, iY, imS.GetWidth<GLsizei>(),
      imS.GetHeight<GLsizei>(), imS.GetPixelType(), imS.GetData().Ptr());
  }
  /* -- Replace texture in VRAM from array --------------------------------- */
  void Update(Image &imOther)
  { // Deinit existing texture
    DeInit();
    // Replace image
    imData.SwapImage(imOther);
    // Reload texture
    ReloadTexture();
  }
  /* -- Download texture to array ------------------------------------------ */
  Image Download(const size_t stId) const
    { return Download(stId, glPixelType); }
  Image Download(const size_t stId, const GLenum eF) const
  { // Get texture id and bind it
    const GLuint uiTexId = GetSubName(stId);
    GL(cOgl->BindTexture(uiTexId), "Failed to bind texture to download!",
      "Identifier", IdentGet(), "Index", stId);
    // Create memory block for texture data and read RGBA texture into it
    Memory mOut{ uiBWidth * uiBHeight * 4 };
    GL(cOgl->ReadTexture(GL_RGBA, mOut.Ptr<GLvoid>()),
      "Download texture failed!",
      "Identifier", IdentGet(), "Index", stId, "Format", eF);
    // Return a newly created image class containing this data
    return Image{ IdentGet(), move(mOut), uiBWidth, uiBHeight, 32, GL_RGBA };
  }
  /* -- Download texture and dump it to disk ------------------------------- */
  void Dump(const size_t stId, const string &strFN) const
    { Download(stId).SaveFile(strFN, stId, 0); }
  /* -- Check if texture is initialised ------------------------------------ */
  bool IsNotInitialised(void) const { return vTexture.empty(); }
  /* -- Reload texture array as normal texture ----------------------------- */
  void ReloadTexture(void)
  { // If image was not loaded from disk? Just (re)load the image data
    // that already should be there and should NEVER be released.
    if(imData.IsDynamic()) LoadFromImage();
    // Texture is static?
    else
    { // It can just be reloaded from disk
      imData.ReloadData();
      // Load the image into memory
      LoadFromImage();
      // Remove all image data because we can just load it from file again
      // and theres no point taking up precious memory for it.
      imData.ClearData();
    } // Error if no textures loaded
    if(IsNotInitialised())
      XC("No textures loaded!", "Identifier", IdentGet());
    // Show what was loaded
    LW(LH_INFO, "Texture loaded $ textures from '$'!",
      vTexture.size(), IdentGet());
  }
  /* -- Init from a image class ------------------------------------------- */
  void InitImage(Image &imgSrc, const GLuint uiTileWidth,
    const GLuint uiTileHeight, const GLuint uiPadX, const GLuint uiPadY,
    const size_t stFilter)
  { // Show filename
    LW(LH_DEBUG, "Texture loading from image '$'.", imgSrc.IdentGet());
    // If source and destination image class are not the same?
    if(&imData != &imgSrc)
    { // Move image data over. The old image will be unusable so guest should
      // discard it.
      imData.SwapImage(imgSrc);
      // The image passed in the arguments is usually still allocated by LUA
      // and will still be registered, so lets put a note in the image to show
      // that this function has nicked by this image class.
      imgSrc.IdentSet(Format("!TEX!$!", imData.IdentGet()));
    } // Set texture name
    IdentSet(imData.IdentGet());
    // If tile dimensions are setSet size of each tile in the texture. a value
    // of 0 means the size of the image will be the tile size.
    uiTWidth = uiTileWidth;
    uiTHeight = uiTileHeight;
    // We'll set flaot versions for faster calculations later on
    fPadX = static_cast<GLfloat>(uiPadX);
    fPadY = static_cast<GLfloat>(uiPadY);
    // Set filter
    stTexFilter = stFilter;
    cOgl->SetMipMapFilterById(stFilter, iTexMinFilter, iTexMagFilter);
    // Set default tint
    SetQuadRGBA(1, 1, 1, 1);
    // Initialise
    LoadFromImage();
    // Remove all image data because we can just load it from file again
    // and theres no point taking up precious memory for it.
    if(!imData.IsDynamic()) imData.ClearData();
  }
  /* -- Deinitialise ------------------------------------------------------- */
  void DeInit(void)
  { // Texture not loaded? return
    if(IsNotInitialised() || bMarkedForDeletion) return;
    // Mark texture for deletion (explanation in fbos class)
    cOgl->SetDeleteTextures(vTexture);
    // Texture has been marked for deleting
    bMarkedForDeletion = true;
    // Report de-initialisation and texture memory
    LW(LH_INFO, "Texture '$' with $ textures marked for deletion.",
      IdentGet(), vTexture.size());
  }
  /* -- Constructor (Initialisation then registration) --------------------- */
  Texture(void) :                      // No parameters
    /* -- Initialisation of members ---------------------------------------- */
    ICHelperTexture{ *cTextures, this }// Automatic (de)registration
    /* --------------------------------------------------------------------- */
    { }                                // Do nothing else
  /* -- Constructor (No registration) -------------------------------------- */
  explicit Texture(const bool) :       // Parameter does nothing
    /* -- Initialisation of members ---------------------------------------- */
    ICHelperTexture{ *cTextures }      // Initially unregistered
    /* --------------------------------------------------------------------- */
    { }                                // Do nothing else
  /* -- Destructor (Unregistration then deinitialisation) ------------------ */
  ~Texture(void) { DeInit(); }
  /* ----------------------------------------------------------------------- */
  DELETECOPYCTORS(Texture);            // Supress copy constructor for safety
};/* -- Finish the collector ----------------------------------------------- */
END_COLLECTOR(Textures);
/* -- DeInit Textures ------------------------------------------------------ */
static void TextureDeInitTextures(void)
{ // Ignore if no textures
  if(cTextures->empty()) return;
  // De-init all the textures and log the pre/post init of them
  LW(LH_DEBUG, "Textures de-initialising $ objects...", cTextures->size());
  for(Texture*const tCptr : *cTextures) tCptr->DeInit();
  LW(LH_INFO, "Textures de-initialised $ objects.", cTextures->size());
}
/* -- Reload Textures ------------------------------------------------------ */
static void TextureReInitTextures(void)
{ // Ignore if no textures
  if(cTextures->empty()) return;
  // Reload all the textures and log the pre/post init of them
  LW(LH_DEBUG, "Textures re-initialising $ objects...", cTextures->size());
  for(Texture*const tCptr : *cTextures) tCptr->ReloadTexture();
  LW(LH_INFO, "Textures re-initialised $ objects.", cTextures->size());
}
/* -- End of module namespace ---------------------------------------------- */
};                                     // End of interface
/* == EoF =========================================================== EoF == */

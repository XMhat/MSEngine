/* == FONT.HPP ============================================================= */
/* ######################################################################### */
/* ## MS-ENGINE              Copyright (c) MS-Design, All Rights Reserved ## */
/* ######################################################################### */
/* ## This module allows loading and drawing of beautifully rendered      ## */
/* ## fonts using the FreeType library.                                   ## */
/* ######################################################################### */
/* ========================================================================= */
#pragma once                           // Only one incursion allowed
/* -- Module namespace ----------------------------------------------------- */
namespace IfFont {                     // Keep declarations neatly categorised
/* -- Includes ------------------------------------------------------------- */
using namespace IfTexture;             // Using texture interface
using namespace IfBin;                 // Using bin interface
using namespace IfFtf;                 // Using ftf interface
/* ------------------------------------------------------------------------- */
BUILD_FLAGS(Font,
  /* ----------------------------------------------------------------------- */
  // No flags                          Use image glyph size for advancement
  FF_NONE                {0x00000000}, FF_USEGLYPHSIZE        {0x00000001},
  // Do floor() on advance width       Do ceil() on advance width
  FF_FLOORADVANCE        {0x00000002}, FF_CEILADVANCE         {0x00000004},
  // Do round() on advance width       True stroke but more buggy
  FF_ROUNDADVANCE        {0x00000008}, FF_STROKETYPE2         {0x00000010},
  /* ----------------------------------------------------------------------- */
  FF_MASK{ FF_USEGLYPHSIZE|FF_FLOORADVANCE|FF_CEILADVANCE|FF_ROUNDADVANCE|
           FF_STROKETYPE2 }
);/* == Font collector class for collector data and custom variables ======= */
BEGIN_COLLECTOR(Fonts, Font, CLHelperUnsafe);
/* == Font Variables Class ================================================= */
// Only put vars used in the Font class in here. This is an optimisation so
// we do not have to initialise all these variables more than once as we have
// more than one constructor in the main Font class.
/* ------------------------------------------------------------------------- */
class FontVariables :                  // Font variables class
  /* -- Base classes ------------------------------------------------------- */
  public Texture                       // Texture class
{ /* --------------------------------------------------------------- */ public:
  struct Glyph                         // Glyph data
  { /* --------------------------------------------------------------------- */
    bool           bLoaded;            // 0=sprite; 1=ft unloaded; 2=ft loaded
    GLfloat        fW, fH, fAdvance;   // Width, height and advance of glyph
    GLfloat        fXMin, fYMin;       // Top-left position of glyph
    GLfloat        fXMax, fYMax;       // Bottom-right position of glyph
  };/* -- Variables -------------------------------------------------------- */
  vector<Glyph>    glData;             // Glyph and outline data
  size_t           stMultiplier;       // 1 if no outline, 2 if outline
  GLfloat          fCharSpacing;       // Character spacing adjustment
  GLfloat          fCharSpacingScale;  // Character spacing scaled size
  GLfloat          fLineSpacing;       // Text line spacing
  GLfloat          fLineSpacingHeight; // Height of line spacing plus height
  GLfloat          fSWidth, fSHeight;  // Scaled font width and height
  GLfloat          fGSize;             // Scaled font texture glyph size
  GLfloat          fScale;             // Font scale
  GLfloat          fGPad, fGPadScaled; // Glyph position adjustment
  Texture         *tGlyphs;            // Texture for print calls only
  FboItem          rgbaOutline;        // Outline colour
  GLuint           uiFilter;           // Selected texture filter
  GLuint           uiPadding;          // Padding after each glyph
  /* -- Freetype handles --------------------------------------------------- */
  Pack<GLint>      binData;            // FT packed characters in image
  Ftf              ftfData;            // FT font data
  FontFlags        ffFTLoadFlags;      // FT load flags
  GLfloat          fFWidth, fFHeight;  // Font dimensions as float for opengl
  FT_ULong         ulDefaultChar;      // Default fallback character
  /* -- Reload texture parameters ------------------------------------------ */
  enum RTCmd { RT_NONE, RT_FULL, RT_PARTIAL } rtCmd; // Reload texture command
  GLuint           uiRTX1, uiRTX2;     // Reload X start and end position
  GLuint           uiRTY1, uiRTY2;     // Reload Y start and end position
  /* ----------------------------------------------------------------------- */
  FontVariables(void) :
    /* --------------------------------------------------------------------- */
    Texture{ true },                   stMultiplier(0),
    fCharSpacing(0.0f),                fCharSpacingScale(0.0f),
    fLineSpacing(0.0f),                fLineSpacingHeight(0.0f),
    fSWidth(0.0f),                     fSHeight(0.0f),
    fGSize(0.0f),                      fScale(0.0f),
    fGPad(0.0f),                       fGPadScaled(0.0f),
    tGlyphs(nullptr),                  rgbaOutline{ 0xFF000000 },
    uiFilter(0),                       uiPadding(0),
    ffFTLoadFlags{ FF_NONE },          fFWidth(0.0f),
    fFHeight(0.0f),                    ulDefaultChar('?'),
    rtCmd(RT_NONE),                    uiRTX1(numeric_limits<GLuint>::max()),
    uiRTX2(0),                         uiRTY1(numeric_limits<GLuint>::max()),
    uiRTY2(0)
    /* --------------------------------------------------------------------- */
    { }
  /* ----------------------------------------------------------------------- */
  DELETECOPYCTORS(FontVariables);      // No copy constructors
};/* ----------------------------------------------------------------------- */
/* == Font Class (which inherits a Texture) ================================ */
BEGIN_MEMBERCLASS(Fonts, Font, ICHelperUnsafe),
  /* -- Base classes ------------------------------------------------------- */
  public FontVariables                 // Font variables class
{ /* -- Stroker functors for DoInitFTChar() ----------------------- */ private:
  class StrokerFuncGlyphNoOutline      // Default no-outline class
  { /* ------------------------------------------------------------ */ private:
    const FT_Error ftErr;              // Error code
    /* ------------------------------------------------------------- */ public:
    FT_Error Result(void) const { return ftErr; }
    /* --------------------------------------------------------------------- */
    StrokerFuncGlyphNoOutline(FT_Glyph&, FT_Stroker) : ftErr(0) { }
    /* --------------------------------------------------------------------- */
    explicit StrokerFuncGlyphNoOutline(const FT_Error ftE) : ftErr(ftE) { }
  };/* -- Full outline required -------------------------------------------- */
  class StrokerFuncGlyphPlusOutline : public StrokerFuncGlyphNoOutline {
    public: StrokerFuncGlyphPlusOutline(FT_Glyph &gD, FT_Stroker ftS) :
      StrokerFuncGlyphNoOutline{ FreeType::ApplyStrokerFull(gD, ftS) } {} };
  /* -- Outside outline required ------------------------------------------- */
  class StrokerFuncGlyphPlusOutlineOutside: public StrokerFuncGlyphNoOutline {
    public: StrokerFuncGlyphPlusOutlineOutside(FT_Glyph &gD, FT_Stroker ftS) :
      StrokerFuncGlyphNoOutline{ FreeType::ApplyStrokerOutside(gD, ftS) } {} };
  /* -- Inside outline required -------------------------------------------- */
  class StrokerFuncGlyphPlusOutlineInside : public StrokerFuncGlyphNoOutline {
    public: StrokerFuncGlyphPlusOutlineInside(FT_Glyph &gD, FT_Stroker ftS) :
      StrokerFuncGlyphNoOutline{ FreeType::ApplyStrokerInside(gD, ftS) } {} };
  /* -- Do load character function ----------------------------------------- */
  template<class StrokerFunc>
    void DoInitFTChar(FT_GlyphSlot &ftgData, const size_t stPos,
      const size_t stChar, const GLfloat fAdvance)
  { // Move The Face's Glyph Into A Glyph Object to get outline
    FT_Glyph gData;
    FT(FT_Get_Glyph(ftgData, &gData),
      "Failed to get glyph!", "Identifier", IdentGet(), "Glyph", stChar);
    // Put in autorelease ptr to autorelease
    typedef unique_ptr<FT_GlyphRec_,
      function<decltype(FT_Done_Glyph)>> GlyphPtr;
    if(GlyphPtr gPtr{ gData, FT_Done_Glyph })
    { // Apply glyph border if requested
      FT(StrokerFunc(gData, ftfData.GetStroker()).Result(),
        "Failed to apply outline to glyph!",
        "Identifier", IdentGet(), "Glyph", stChar);
      // Convert The Glyph To A Image.
      FT(FT_Glyph_To_Bitmap(&gData, FT_RENDER_MODE_NORMAL, nullptr, true),
        "Failed to render glyph to image!",
        "Identifier", IdentGet(), "Glyph", stChar);
      // The above function will have modified the glyph address and freed the
      // old one so let's release the old one (not freeing it) and update it so
      // unique_ptr dtor knows to destroy the new one.
      gPtr.release();
      gPtr.reset(gData);
      // Get outline glyph and texcoord data
      Glyph &glItem = glData[stPos];
      // Access image information and if has no width or height?
      const FT_Bitmap &bData =
        reinterpret_cast<FT_BitmapGlyph>(gData)->bitmap;
      if(bData.width <= 0 || bData.rows <= 0)
      { // Push generic glyph data
        glItem.bLoaded = true;
        glItem.fW = fFWidth;
        glItem.fH = fFHeight;
        glItem.fAdvance = fAdvance;
        // The bounds are already zero so no need to set them. Also, the tile
        // coords data is already set to zero so no need to set those too.
      } // Not a blank glyph
      else
      { // Get glyph outline origins
        FT_BBox bbData;
        FT_Glyph_Get_CBox(gData, FT_GLYPH_BBOX_PIXELS, &bbData);
        // Set glyph data
        glItem = {
          true,                                      // bLoaded
          static_cast<GLfloat>(bData.width),         // fW
          static_cast<GLfloat>(bData.rows),          // fH
          fAdvance,                                  // fAdvance
          static_cast<GLfloat>(bbData.xMin),         // fXMin
         -static_cast<GLfloat>                       // fYmin
            (static_cast<int>(bData.rows)+bbData.yMin)+fFHeight,
          static_cast<GLfloat>(bbData.xMax),         // fXMax
         -static_cast<GLfloat>(bbData.yMin)+fFHeight // fYMax
        };
        // Calculate size plus padding and return if size not set
        const GLuint uiWidth = bData.width + uiPadding,
                     uiHeight = bData.rows + uiPadding;
        // Get image slot and data we're writing to
        ImageSlot &imdSlot = imData.GetFirstSlot();
        Memory &aSrc = imdSlot.memData;
        // Get texcoord data
        CoordData &tcItem = tcList[stPos];
        // Put this glyph in the bin packer and if succeeded
        Pack<GLint>::Rect binRect{ binData.Insert(uiWidth, uiHeight) };
        if(binRect.iH > 0)
        { // The result rect will include padding so remove it
          binRect.iW -= static_cast<int>(uiPadding);
          binRect.iH -= static_cast<int>(uiPadding);
          // Copy the glyph to texture atlast
          GlyphToTexture(binRect, bData, tcItem, aSrc);
          // Full redraw not already specified?
          if(rtCmd != RT_FULL)
          { // Set partial redraw
            rtCmd = RT_PARTIAL;
            // Set lowest most left bound
            if(static_cast<decltype(uiRTX1)>(binRect.iX) < uiRTX1)
              uiRTX1 = static_cast<decltype(uiRTX1)>(binRect.iX);
            // Set lowest most top bound
            if(static_cast<decltype(uiRTY1)>(binRect.iY) < uiRTY1)
              uiRTY1 = static_cast<decltype(uiRTY1)>(binRect.iY);
            // Set highest most right bound
            const GLuint uiX2 = static_cast<GLuint>(binRect.iX + binRect.iW);
            if(uiX2 > uiRTX2) uiRTX2 = uiX2;
            // Set highest most bottom bound
            const GLuint uiY2 = static_cast<GLuint>(binRect.iY + binRect.iH);
            if(uiY2 > uiRTY2) uiRTY2 = uiY2;
          }
        } // Failed
        else
        { // Get next biggest size from bounds
          const GLuint uiSize = GetMaxTexSizeFromBounds(binData.Width(),
            binData.Height(), uiWidth, uiHeight, 2);
          const GLuint uiMaximum = cOgl->MaxTexSize();
          // Makle sure the size is supported by graphics
          if(uiSize > uiMaximum)
            XC("Cannot grow font texture any further due to GPU limitation!",
               "BinWidth",     bData.width, "BinHeight",     bData.rows,
               "BinWidth+Pad", uiWidth,     "BinHeight+Pad", uiHeight,
               "Requested",    uiSize,      "Maximum",       uiMaximum);
          // Double the size taking into account that the requested glyph size
          // must fit inside it as well and the video card maximum texture
          // space must support the glyph as well. Then try placing it in the
          // bin again and if it still failed then there is nothing more we can
          // do for now.
          binData.Resize(uiSize, uiSize);
          binRect = binData.Insert(uiWidth, uiHeight);
          if(binRect.iH <= 0)
            XC("No texture space left for glyph!",
               "Identifier", IdentGet(),           "Glyph",   stChar,
               "Width",      bData.width,       "Height",  bData.rows,
               "Occupancy", binData.Occupancy(),"Maximum", cOgl->MaxTexSize());
          // THe result rect will include padding so remove it
          binRect.iW -= static_cast<int>(uiPadding);
          binRect.iH -= static_cast<int>(uiPadding);
          // We need to make a new image because we need to modify the data in
          // the old obsolete image.
          Memory aDst{
            static_cast<size_t>(binData.Width() * binData.Height() * 2) };
          // We need to fill it with transparent white pixels, since we can't
          // use memset, we'll use fill instead. !FIXME: Don't need to write to
          // 'all' pixels, just the new ones.
          aDst.Fill<uint16_t>(0x00FF);
          // Copy scanlines from the old image
          const size_t stBWidthx2 = uiBWidth * 2;
          for(size_t stY = 0,
                     stBinWidth = static_cast<size_t>(binData.Width());
                     stY < uiBHeight;
                   ++stY)
          { // Calculate source and destination position and copy the scanline
            const size_t stSrcPos = (uiBWidth * stY) * 2,
                         stDestPos = (stBinWidth * stY) * 2;
            aDst.Write(stDestPos, aSrc.Read(stSrcPos, stBWidthx2), stBWidthx2);
          } // This is the new image and the old one will be destroyed
          aSrc.SwapMemory(move(aDst));
          aDst.DeInit();
          // Calculate how much the image increased. This should really be 2
          // every time but we'll just make a calculation like this just
          // incase.
          const unsigned int uiDivisor = binData.Width() / uiBWidth;
          // Update the actual image dimensions
          uiBWidth = binData.Width();
          uiBHeight = binData.Height();
          // Image dimensions have changed too so modify them
          imData.imgData.uiWidth = imdSlot.uiWidth = uiBWidth;
          imData.imgData.uiHeight = imdSlot.uiHeight = uiBHeight;
          // Now we need to walk through the char datas and reduce the values
          // by the enlargement factor. A very simple and effective solution.
          // Note that using transform is ~100% slower than this.
          MYFOREACH(par_unseq, tcList.begin(), tcList.end(),
            [uiDivisor](CoordData &tcI)
              { for(size_t stTriId = 0; stTriId < TRISPERQUAD; ++stTriId)
                  for(size_t stFltId = 0; stFltId < FLOATSPERCOORD; ++stFltId)
                    tcI.fTC[stTriId][stFltId] /= uiDivisor; });
          // Copy the glyph to texture atlast
          GlyphToTexture(binRect, bData, tcItem, aSrc);
          // Re-upload the whole texture to VRAM.
          rtCmd = RT_FULL;
          // Say that we resized the image
          LW(LH_DEBUG, "Font enlarged '$' by a factor of $ to $x$.",
            IdentGet(), uiDivisor, uiBWidth, uiBHeight);
        } // Done with drawing glyph
      }
    } // Failed to grab pointer to glyph data
    else XC("Failed to get glyph pointer!",
            "Identifier", IdentGet(), "Glyph", stChar);
  }
  /* -- Initialise freetype char and set types ----------------------------- */
  template<class StrokerFuncGlyphNormal, class StrokerFuncGlyphOutline>
    void DoInitFTCharOutline(FT_GlyphSlot &ftgD, const size_t stP,
      const size_t stC, const GLfloat fA)
  { // Initialise main character
    DoInitFTChar<StrokerFuncGlyphNormal>(ftgD, stP, stC, fA);
    // Initialise outline character
    DoInitFTChar<StrokerFuncGlyphOutline>(ftgD, stP+1, stC, fA);
  }
  /* -- Load type class functors for InitFTChar() -------------------------- */
  class StrokerLoadTypeGlyphNoOutline {
    public: StrokerLoadTypeGlyphNoOutline(Font*const fP, FT_GlyphSlot &ftgD,
      const size_t stP, const size_t stC, const GLfloat fA)
    { fP->DoInitFTChar<StrokerFuncGlyphNoOutline>(ftgD, stP, stC, fA); } };
  /* -- Outline required --------------------------------------------------- */
  class StrokerLoadTypeGlyphPlusOutline2 {
    public: StrokerLoadTypeGlyphPlusOutline2(Font*const fP, FT_GlyphSlot &ftgD,
      const size_t stP, const size_t stC, const GLfloat fA)
    { fP->DoInitFTCharOutline
        <StrokerFuncGlyphPlusOutlineInside, StrokerFuncGlyphPlusOutline>
          (ftgD, stP, stC, fA); } };
  /* -- Outside outline required ------------------------------------------- */
  class StrokerLoadTypeGlyphPlusOutline1 {
    public: StrokerLoadTypeGlyphPlusOutline1(Font*const fP, FT_GlyphSlot &ftgD,
      const size_t stP, const size_t stC, const GLfloat fA)
    { fP->DoInitFTCharOutline
        <StrokerFuncGlyphNoOutline, StrokerFuncGlyphPlusOutline>
          (ftgD, stP, stC, fA); } };
  /* -- Function to select correct outline method -------------------------- */
  void DoSelectOutlineType(FT_GlyphSlot &ftgData, const size_t stPos,
      const size_t stChar, const GLfloat fAdvance)
  { // Stroker loaded?
    if(ftfData.LoadedStroker())
    { // Stroke inside and outside border?
      if(ffFTLoadFlags.FlagIsSet(FF_STROKETYPE2))
        StrokerLoadTypeGlyphPlusOutline2{ this,
          ftgData, stPos, stChar, fAdvance };
      // Stroke just the outside? (default
      else StrokerLoadTypeGlyphPlusOutline1{ this,
        ftgData, stPos, stChar, fAdvance };
    } // No outline, just load as normal
    else StrokerLoadTypeGlyphNoOutline{ this,
      ftgData, stPos, stChar, fAdvance };
  }
  /* -- Functor to automatically check for outline method ------------------ */
  template<class StrokerLoadTypeFunc>
    class StrokerCheckFuncAuto {
      public: StrokerCheckFuncAuto(Font*const fP, FT_GlyphSlot &ftgD,
        const size_t stP, const size_t stC, const GLfloat fA)
      { fP->DoSelectOutlineType(ftgD, stP, stC, fA); } };
  /* -- Functor to set outline method manually ----------------------------- */
  template<class StrokerLoadTypeFunc>
    class StrokerCheckFuncManual : private StrokerLoadTypeFunc {
      public: StrokerCheckFuncManual(Font*const fP,
        FT_GlyphSlot &ftgD, const size_t stP, const size_t stC,
        const GLfloat fA) : StrokerLoadTypeFunc{ fP, ftgD, stP, stC, fA } {} };
  /* -- Function to to automatically check for rounding method ------------- */
  template<class RoundFunc>class RoundCheckFuncAuto : public RoundFunc
  { /* -- Detect advance rounding method from flags ------------------------ */
    GLfloat Detect(const FontFlagsConst &ffFlags, const GLfloat fAdvance)
    { // Load the character with floor rounding?
      if(ffFlags.FlagIsSet(FF_FLOORADVANCE))
        return RoundFuncFloor<GLfloat>{ fAdvance }.Result();
      // Load the character with ceil rounding?
      else if(ffFlags.FlagIsSet(FF_CEILADVANCE))
        return RoundFuncCeil<GLfloat>{ fAdvance }.Result();
      // Load the character with round rounding?
      else if(ffFlags.FlagIsSet(FF_ROUNDADVANCE))
        return RoundFuncRound<GLfloat>{ fAdvance }.Result();
      // No rounding (allows subpixel drawing)
      return fAdvance;
    }
    /* -- Normal entry for auto-detection -------------------------- */ public:
    RoundCheckFuncAuto(const FontFlagsConst &ffFlags,
      const GLfloat fAdvance) : RoundFunc{ Detect(ffFlags, fAdvance) } {}
  };/* -- Manually specified rounding functor check ------------------------ */
  template<class RoundFunc>class RoundCheckFuncManual : public RoundFunc {
    public: RoundCheckFuncManual(const FontFlagsConst&,
      const GLfloat fAdvance) : RoundFunc{ fAdvance } {} };
  /* ----------------------------------------------------------------------- */
  template<class StrokerCheckFunc, class RoundCheckFunc>
    size_t DoHandleFTGlyph(const size_t stChar, const size_t stPos)
  { // If position is not allocated?
    if(stPos >= glData.size())
    { // Extend and initialise storage
      glData.resize(stPos + stMultiplier);
      tcList.resize(glData.size());
    } // Return the position if already loaded
    else if(glData[stPos].bLoaded) return stPos;
    // Translate character to glyph and if failed?
    const FT_UInt uiGl = ftfData.CharToGlyph(static_cast<FT_ULong>(stChar));
    if(uiGl == 0)
    { // Bail out if we tried to load the default char. We cannot try to load
      // it again.
      if(stChar == ulDefaultChar)
        XC("Default character not available!",
           "Identifier", IdentGet(), "Index", stChar, "Position", stPos);
      // Try to load the default character instead.
      return DoHandleFTGlyph<StrokerCheckFunc, RoundCheckFunc>(
        static_cast<size_t>(ulDefaultChar),
        static_cast<size_t>(ulDefaultChar) * stMultiplier);
    } // Load glyph and return glyph on success else throw exception
    FT(ftfData.LoadGlyph(uiGl), "Failed to load glyph!",
      "Identifier", IdentGet(), "Index", stChar);
    // Get glyph slot handle and get advance width.
    FT_GlyphSlot ftgData = ftfData.GetGlyphData();
    // Compare type of border required
    const GLfloat fAdvance = RoundCheckFunc(ffFTLoadFlags,
      static_cast<GLfloat>(ftgData->metrics.horiAdvance) / 64).Result();
    // Begin initialisation of char by checking stroker setting. This can
    // either be a pre-calculated or calculated right now.
    StrokerCheckFunc{ this, ftgData, stPos, stChar, fAdvance };
    // Return position
    return stPos;
  }
  /* -- Do handle a static glyph ------------------------------------------- */
  size_t DoHandleStaticGlyph(const size_t stChar, const size_t stPos)
  { // Static font. If character is in range and loaded
    if(stPos < glData.size() && glData[stPos].bLoaded) return stPos;
    // Try to find the default character and return position if valiid
    const size_t stBackupPos =
      static_cast<size_t>(ulDefaultChar) * stMultiplier;
    if(stBackupPos < glData.size()) return stBackupPos;
    // Impossible situation. Caller should at least have a question mark in
    // their font.
    XC("Specified string contains unprintable characters with no "
       "valid fall-back character!",
       "Identifier",      IdentGet(),       "Character",  stChar,
       "BackupCharacter", ulDefaultChar, "Position",   stPos,
       "BackupPosition",  stBackupPos,   "Mulitplier", stMultiplier,
       "Maximum",         glData.size());
  }
  /* -- Function to select correct outline method -------------------------- */
  template<class StrokerCheckFunc, class RoundCheckFunc>
    size_t DoSelectFontType(const size_t stChar, const size_t stPos)
  { // Stroker loaded?
    return ftfData.Loaded()
      ? DoHandleFTGlyph<StrokerCheckFunc, RoundCheckFunc>(stChar, stPos)
      : DoHandleStaticGlyph(stChar, stPos);
  }
  /* -- Auto detect font type functor -------------------------------------- */
  class FontTypeAuto
  { /* ------------------------------------------------------------- */ public:
    template<class StrokerCheckFunc, class RoundCheckFunc>
      size_t DoHandleGlyph(Font*const fP, const size_t stChar,
        const size_t stPos)
          { return fP->DoSelectFontType
              <StrokerCheckFunc, RoundCheckFunc>(stChar, stPos); }
    /* --------------------------------------------------------------------- */
    FontTypeAuto(void) { }
  };/* -- Freetype font selector functor ----------------------------------- */
  class FontTypeFreeType
  { /* ------------------------------------------------------------- */ public:
    template<class StrokerCheckFunc, class RoundCheckFunc>
      size_t DoHandleGlyph(Font*const fP, const size_t stChar,
        const size_t stPos)
          { return fP->DoHandleFTGlyph
              <StrokerCheckFunc, RoundCheckFunc>(stChar, stPos); }
    /* --------------------------------------------------------------------- */
    FontTypeFreeType(void) { }
  };/* -- Standard glyph font selector functor ----------------------------- */
  class FontTypeGlyph
  { /* ------------------------------------------------------------- */ public:
    template<class StrokerCheckFunc, class RoundCheckFunc>
      size_t DoHandleGlyph(Font*const fP, const size_t stChar,
        const size_t stPos)
          { return fP->DoHandleStaticGlyph(stChar, stPos); }
    /* --------------------------------------------------------------------- */
    FontTypeGlyph(void) { }
  };/* -- Check if a character needs initialising -------------------------- */
  template<class FontCheckFunc, class StrokerCheckFunc, class RoundCheckFunc>
    size_t DoCheckGlyph(const size_t stChar)
  { // Get character position and if freetype font is assigned?
    return FontCheckFunc().template DoHandleGlyph<StrokerCheckFunc,
      RoundCheckFunc>(this, stChar, stChar * stMultiplier);
  }
  /* -- Check if a character needs initialising ---------------------------- */
  size_t CheckGlyph(const size_t stChar)
  { // Apply requested outline?
    return DoCheckGlyph<FontTypeAuto,
                        StrokerCheckFuncAuto<StrokerLoadTypeGlyphNoOutline>,
                        RoundCheckFuncAuto<RoundFuncNone<GLfloat>>>(stChar);
  }
  /* -- Handle return on print --------------------------------------------- */
  void HandleReturn(Decoder &utfStr, GLfloat &fX, GLfloat &fY,
    const GLfloat fI)
  { // Go down own line and set indentation
    fX = fI;
    fY += fLineSpacingHeight;
    // Discard further spaces and return string minus one space
    utfStr.Ignore(' ');
  }
  /* -- Get modified character width at specified position------------------ */
  GLfloat GetCharWidth(const size_t stPos)
    { return (glData[stPos].fAdvance + fCharSpacing) * fScale; }
  /* -- Locate a supported character while checking if word can be printed - */
  bool PrintGetWord(Decoder &utfStr, size_t &stPos, float fX, float &fY,
    const float fW)
  { // Save position because we're not drawing anything
    const unsigned char*const ucpPtr = utfStr.GetCPtr();
    // Until null character. Which control token?
    while(const unsigned int uiChar = utfStr.Next()) switch(uiChar)
    { // Carriage return or space char? Restore position and return no wrap
      case '\n': case ' ': utfStr.SetCPtr(ucpPtr); return false;
      // Other control character? Handle print control characters
      case '\r': HandlePrintControl(fX, fY, utfStr, true); break;
      // Normal character
      default:
        // Get character
        stPos = CheckGlyph(uiChar);
        // Get character width
        const GLfloat fWidth = GetCharWidth(stPos);
        // Printing next character would exceed wrap width?
        if(fWidth + fX >= fW)
        { // Restore position
          utfStr.SetCPtr(ucpPtr);
          // Wrap to next position
          return true;
        } // Move X position forward
        fX += fWidth;
        // Done
        break;
    } // Restore position
    utfStr.SetCPtr(ucpPtr);
    // No wrap occured and caller should not Y adjust
    return false;
  }
  /* -- Handle actual printing of characters ------------------------------- */
  void PrintDraw(GLfloat &fX, const GLfloat fY, const size_t stPos)
  { // Blit outline character?
    if(ftfData.LoadedStroker())
    { // Get outline character info and print the outline glyph
      const Glyph &gOData = glData[stPos+1];
      BlitLTRBC(0, stPos+1, fX+gOData.fXMin * fScale, fY+gOData.fYMin * fScale,
                            fX+gOData.fXMax * fScale, fY+gOData.fYMax * fScale,
        rgbaOutline.GetCData());
    } // Get character info and print the opaque glyph
    const Glyph &gData = glData[stPos];
      BlitLTRB(0, stPos, fX+gData.fXMin * fScale, fY+gData.fYMin * fScale,
                         fX+gData.fXMax * fScale, fY+gData.fYMax * fScale);
    // Increase position
    fX += (gData.fAdvance + fCharSpacing) * fScale;
  }
  /* -- Handle print control character ------------------------------------- */
  void HandlePrintControl(GLfloat &fX, GLfloat &fY, Decoder &utfStr,
    const bool bSimulation, const bool bReverse=false, const bool bClip=false,
    const GLfloat fXO=0, const GLfloat fL=0, const GLfloat fR=0)
  { // Get next character
    switch(utfStr.Next())
    { // Colour selection
      case 'c':
      { // Scan for the hexadecimal value and if we found it? Set Tint if not
        // simulation and we read 8 bytes
        unsigned int uiCol;
        if(utfStr.ScanValue(uiCol) == 8 && !bSimulation) SetQuadRGBInt(uiCol);
        // Done
        break;
      } // Outline colour selection
      case 'o':
      { // Scan for the hexadecimal value and if we found it? Set Tint if not
        // simulation and we read 8 bytes
        unsigned int uiCol;
        if(utfStr.ScanValue(uiCol) == 8 && !bSimulation)
          rgbaOutline.SetQuadRGBInt(uiCol);
        // Done
        break;
      } // Reset colour
      case 'r': if(!bSimulation) PopQuadColour(); break;
      // Print glyph
      case 't':
      { // Scan for the hexadecimal value and if we found it and we have
        // a glyphs texture assigned and is a valid glyph value?
        unsigned int uiGlyph;
        if(utfStr.ScanValue(uiGlyph) == 8 && tGlyphs &&
          uiGlyph < tGlyphs->tcList.size())
        { // to wrap and scan the string again
          if(!bClip && fL != 0.0f && fX + fGSize >= fL)
          { // Go down own line and set indentation
            fX = fXO + fR;
            fY += fLineSpacingHeight;
            // Print the tile
            if(!bSimulation)
              tGlyphs->BlitLTRB(0, uiGlyph, fX, fY+fGPadScaled,
                fX + fGSize, fY+fGSize+fGPadScaled);
            // Discard further spaces
            utfStr.Ignore(' ');
            // Go forward (or backword). Ignore the width because we want to
            // make it a perfect square.
            fX = bReverse ? fX - fGSize - fCharSpacingScale :
                            fX + fGSize + fCharSpacingScale;
            // Done
            break;
          } // If not simluation?
          if(!bSimulation)
          { // Need clipping?
            if(bClip)
            { // Clipping left margin
              if(fX - fXO < fL)
              { // Really clipping left margin. Blit character.
                if(fX - fXO >= fL - fGSize)
                  tGlyphs->BlitLTRBS(0, uiGlyph, fX, fY+fGPadScaled,
                    fX+fGSize, fY+fGSize+fGPadScaled,
                    ((fX+fGSize)-(fL+fXO))/fGSize, 1);
              } // Clipping right margin?
              else if(fX + fGSize >= fXO + fR)
              { // Really clipping right margin. Blit character.
                if(fX + fGSize < fXO + fR + fGSize)
                  tGlyphs->BlitLTRBS(0, uiGlyph, fX, fY+fGPadScaled,
                    fX+fGSize, fY+fGSize+fGPadScaled,
                    1, 1-(((fX+fGSize)-(fR+fXO))/fGSize));
              } // Draw character normally
              else tGlyphs->BlitLTRB(0, uiGlyph, fX, fY+fGPadScaled,
                fX+fGSize, fY+fGSize+fGPadScaled);
            } // Draw glyph normally
            else tGlyphs->BlitLTRB(0, uiGlyph, fX, fY+fGPadScaled,
              fX+fGSize, fY+fGSize+fGPadScaled);
          }
          // Go forward (or backword). Ignore the width because we want to
          // make it a perfect square.
          fX = bReverse ? fX - fGSize - fCharSpacingScale :
                          fX + fGSize + fCharSpacingScale;
        } // Done
        break;
      } // Invalid control character.
      default: break;
    }
  }
  /* -- Print a utfstring of textures, wrap at width, return height -------- */
  GLfloat DoPrintW(GLfloat fX, GLfloat fY, const GLfloat fW, const GLfloat fI,
    Decoder &utfStr)
  { // Record original X and Y position and maximum X position
    const GLfloat fXO = fX + fI, fYO = fY;
    // Until null character, which character?
    while(const unsigned int uiChar = utfStr.Next()) switch(uiChar)
    { // Carriage return?
      case '\n': HandleReturn(utfStr, fX, fY, fXO); break;
      // Other control character? Handle print control characters
      case '\r': HandlePrintControl(fX, fY, utfStr, false, false, false,
                   fXO, fW, fI);
                 break;
      // Whitespace character?
      case ' ':
      { // Get first character as normal
        size_t stPos = CheckGlyph(uiChar);
        // Do the print to move the X position
        PrintDraw(fX, fY, stPos);
        // Ignore if the space character processed went over the limit OR
        // Check if the draw length of the next word would go off the limit
        // and if either did?
        if(fX + GetCharWidth(stPos) >= fW ||
          PrintGetWord(utfStr, stPos, fX, fY, fW))
            HandleReturn(utfStr, fX, fY, fXO);
        // Done
        break;
      } // Normal character
      default:
      { // Character storage
        const size_t stPos = CheckGlyph(uiChar);
        // Printing next character would exceed wrap width? Handle return
        if(fX + GetCharWidth(stPos) >= fW) HandleReturn(utfStr, fX, fY, fXO);
        // Print the character regardless of wrap
        PrintDraw(fX, fY, stPos);
        // Done
        break;
      }
    } // Return height
    return (fY - fYO) + fLineSpacingHeight;
  }
  /* -- Simluate printing a wrapped utfstring and return height ------------ */
  GLfloat DoPrintWS(const GLfloat fW, const GLfloat fI, Decoder &utfStr)
  { // Record original X and Y position and maximum X position
    GLfloat fX = 0, fY = fLineSpacingHeight;
    // Until null character, which character?
    while(const unsigned int uiChar = utfStr.Next()) switch(uiChar)
    { // Carriage return?
      case '\n': HandleReturn(utfStr, fX, fY, fI); break;
      // Other control character?
      case '\r':
      { // Handle print control characters
        HandlePrintControl(fX, fY, utfStr, true, false, false, 0, fW, fI);
        // Done
        break;
      } // Whitespace character?
      case ' ':
      { // Get first character as normal
        size_t stPos = CheckGlyph(uiChar);
        // Get character width
        const GLfloat fWidth = GetCharWidth(stPos);
        // Move X position forward
        fX += fWidth;
        // Ignore if the space character processed went over the limit OR
        // Check if the draw length of the next word would go off the limit
        // and if either did? Handle the return!
        if(fWidth + fX >= fW || PrintGetWord(utfStr, stPos, fX, fY, fW))
          HandleReturn(utfStr, fX, fY, fI);
        // Done
        break;
      } // Normal character
      default:
      { // Get width
        const GLfloat fWidth = GetCharWidth(CheckGlyph(uiChar));
        // Printing next character would exceed wrap width? Wrap and indent
        if(fWidth + fX >= fW) HandleReturn(utfStr, fX, fY, fI);
        // No exceed, move X position forward
        fX += fWidth;
        // Done
        break;
      }
    } // Return height
    return fY;
  }
  /* -- Print a utf string ------------------------------------------------- */
  void DoPrint(GLfloat fX, GLfloat fY, const GLfloat fXO, Decoder &utfStr)
  { // Until null character. Do printing of characters
    while(const unsigned int uiChar = utfStr.Next()) switch(uiChar)
    { // Carriage return?
      case '\n': HandleReturn(utfStr, fX, fY, fXO); break;
      // Other control character? Handle print control characters
      case '\r': HandlePrintControl(fX, fY, utfStr, false); break;
      // Normal character? Print character
      default: PrintDraw(fX, fY, CheckGlyph(uiChar)); break;
    }
  }
  /* -- Print a utfstring of textures with right align --------------------- */
  void DoPrintR(GLfloat fX, GLfloat fY, Decoder &utfStr)
  { // Record original X position
    const GLfloat fXO = fX;
    // Save position
    const unsigned char *cpSaved = utfStr.GetCPtr();
    // The first character is already assumed as valid
    for(unsigned int uiChar = utfStr.Next();;
                     uiChar = utfStr.Next()) switch(uiChar)
    { // Carriage return?
      case '\n':
      { // Create and prepare spliced string. *DO NOT* merge these lines
        // as the string will be freed after utfSliced() returns.
        const string strSpliced{ utfStr.Slice(cpSaved) };
        Decoder utfSliced{ strSpliced };
        // Now print the string. We need to duplicate the string here, because
        // we shouldn't modify the original string, even though it would be
        // more optimal to just nullify this character instead.
        DoPrint(fX, fY, fXO, utfSliced);
        // Set new string
        HandleReturn(utfStr, fX, fY, fXO);
        // Save current position
        cpSaved = utfStr.GetCPtr();
        // Done
        break;
      } // Other control character?
      case '\r':
      { // Handle print control characters
        HandlePrintControl(fX, fY, utfStr, true, true);
        // Done
        break;
      } // End of string or invalid unicode character
      case '\0':
      { // Create spliced string
        Decoder utfSliced{ cpSaved };
        // Now print the string. We need to duplicate the string here, because
        // we shouldn't modify the original string, even though it would be
        // more optimal to just nullify this character instead.
        DoPrint(fX, fY, fXO, utfSliced);
        // Handle the return character
        return HandleReturn(utfStr, fX, fY, fXO);
      } // Normal character? Get width and go backwards
      default: fX -= GetCharWidth(CheckGlyph(uiChar)); break;
    }
  }
  /* -- Print a utfstring with centre alignment ---------------------------- */
  void DoPrintC(GLfloat fX, GLfloat fY, Decoder &utfStr)
  { // Adjust X position with spacing so it doesnt affect the centre position
    if(fCharSpacing != 0.0f) fX += fCharSpacingScale / 2.0f;
    // Record original X position.
    const GLfloat fXO = fX;
    // Save position
    const unsigned char *cpSaved = utfStr.GetCPtr();
    // Until null character move the X axis
    for(unsigned int uiChar = utfStr.Next();;uiChar = utfStr.Next())
      switch(uiChar)
    { // Carriage return?
      case '\n':
      { // Create and prepare spliced string. *DO NOT* merge these lines
        // as the string will be freed after utfSliced() returns.
        const string strSliced{ utfStr.Slice(cpSaved) };
        Decoder utfSliced{ strSliced };
        // Now print the string. We need to duplicate the string here, because
        // we shouldn't modify the original string, even though it would be
        // more optimal to just nullify this character instead.
        DoPrint(fX, fY, fXO, utfSliced);
        // Set new string
        HandleReturn(utfStr, fX, fY, fXO);
        // Save current position
        cpSaved = utfStr.GetCPtr();
        // Done
        break;
      } // Other control character?
      case '\r':
      { // To store with of drawing if needed
        GLfloat fXW = 0;
        // Handle print control characters
        HandlePrintControl(fXW, fY, utfStr, true);
        // Go back if needed
        if(fXW != 0.0f) fX -= fXW / 2.0f;
        // Done
        break;
      } // End of string or invalid unicode character
      case '\0':
      { // Slice the utf8 string
        Decoder utfSliced{ cpSaved };
        // Now print the string. We need to duplicate the string here, because
        // we shouldn't modify the original string, even though it would be
        // more optimal to just nullify this character instead.
        DoPrint(fX, fY, fXO, utfSliced);
        // Set new string
        return HandleReturn(utfStr, fX, fY, fXO);
      } // Normal character? Get width and go backwards
      default: fX -= GetCharWidth(CheckGlyph(uiChar)) / 2.0f; break;
    }
  }
  /* -- Simulate printing a utfstring -------------------------------------- */
  GLfloat DoPrintS(Decoder &utfStr)
  { // Width and highest width
    GLfloat fW = 0, fWH = 0;
    // Increase width until end of string
    while(const unsigned int uiChar = utfStr.Next()) switch(uiChar)
    { // Carriage return? Set highest width and reset width
      case '\n': if(fW > fWH) fWH = fW; fW = 0; break;
      // Other control character? Handle the character
      case '\r': HandlePrintControl(fW, fW, utfStr, true); break;
      // Normal character? Go forth!
      default: fW += GetCharWidth(CheckGlyph(uiChar)); break;
    } // Return width
    return fW > fWH ? fW : fWH;
  }
  /* -- Simulate printing a utfstring and returning the height ------------- */
  GLfloat DoPrintSU(Decoder &utfStr)
  { // Width and highest width
    GLfloat fX = 0, fY;
    // We're going to print something
    fY = fLineSpacingHeight;
    // Increase width until end of string
    while(const unsigned int uiChar = utfStr.Next()) switch(uiChar)
    { // Carriage return? Handle it
      case '\n': HandleReturn(utfStr, fX, fY, 0); break;
      // Other control character? Handle it
      case '\r': HandlePrintControl(fX, fY, utfStr, true); break;
      // Normal character? Go forward
      default: fX += GetCharWidth(CheckGlyph(uiChar)); break;
    } // Return height
    return fY;
  }
  /* -- Draw a partial glyph ----------------------------------------------- */
  bool DrawPartialGlyph(const bool bMove, const size_t stPos,
    const GLfloat fXO, GLfloat &fX, const GLfloat fY, const GLfloat fL,
    const GLfloat fR, const QuadColData &faColPtr)
  { // Get glyph data
    const Glyph &gData = glData[stPos];
    // Make adjusted co-ordinates based on glyph data
    const GLfloat
      fXMin = fX + gData.fXMin * fScale,
      fYMin = fY + gData.fYMin * fScale,
      fXMax = fX + gData.fXMax * fScale,
      fYMax = fY + gData.fYMax * fScale,
      fW = gData.fW * fScale;
    // Glyph would clip left margin?
    if(fXMin - fXO < fL)
    { // Really clipping left margin? Draw partial glyph with left clipping
      if(fXMin - fXO >= fL - fW)
        BlitLTRBSC(0, stPos, fXMin, fYMin, fXMax, fYMax,
          (fXMax - (fL + fXO)) / fW, 1, faColPtr);
    } // Clipping right margin?
    else if(fXMax > fXO + fR)
    { // Really clipping left margin? Draw partial glyph with left clipping
      if(fXMax <= fXO + fR + fW)
        BlitLTRBSC(0, stPos, fXMin, fYMin, fXMax, fYMax,
          1, 1 - ((fXMax - (fR + fXO)) / fW), faColPtr);
      // Do not process anymore characters
      return true;
    } // Draw character normally
    else BlitLTRBC(0, stPos, fXMin, fYMin, fXMax, fYMax, faColPtr);
    // Increase position if we're to move the pen
    if(bMove) fX += (gData.fAdvance + fCharSpacing) * fScale;
    // Not finished printing characters
    return false;
  }
  /* -- Print a utfstring of textures -------------------------------------- */
  void DoPrintM(GLfloat fX, GLfloat fY, const GLfloat fL, const GLfloat fR,
    Decoder &utfStr)
  { // Parameters:-
    // > fX = The X position of where to draw the string.
    // > fY = The Y position of where to draw the string.
    // > fL = The amount to scroll leftwards by in pixels.
    // > fR = The width of the string to print.
    // Record original X position
    const GLfloat fXO = fX;
    // Until null character. Do printing of characters
    while(const unsigned int uiChar = utfStr.Next()) switch(uiChar)
    { // Carriage return? Set new co-ordinates.
      case '\n': HandleReturn(utfStr, fX, fY, fXO); break;
      // Other control character?
      case '\r':
      { // Handle print control characters
        HandlePrintControl(fX, fY, utfStr, false, false, true, fXO, fL, fR);
        // If we went past the end (glyph drawing might have done this)
        if(fX >= fXO + fR) return;
        // Done
        break;
      } // Normal character
      default:
      { // Get character
        const size_t stPos = CheckGlyph(uiChar);
        // Draw outline glyph first, don't care about return status
        if(ftfData.LoadedStroker())
          DrawPartialGlyph(false, stPos+1, fXO, fX, fY, fL, fR,
            rgbaOutline.GetCData());
        // Draw main glyph, and if we don't need to draw anymore? Do not
        // process any more characters
        if(DrawPartialGlyph(true, stPos, fXO, fX, fY, fL, fR, GetCData()))
          return;
        // Done
        break;
      }
    }
  }
  /* -- Pop colour and reset glyphs ---------------------------------------- */
  void PopQuadColourAndGlyphs(void)
  { // Reset glyphs
    tGlyphs = nullptr;
    // Pop tint
    PopQuadColour();
  }
  /* -- Push colour and glyphs --------------------------------------------- */
  void PushQuadColourAndGlyphs(Texture*const _tGlyphs)
  { // Push tint
    PushQuadColour();
    // Set glyphs texture and set alpha to our current alpha (dont set colour)
    tGlyphs = _tGlyphs;
    tGlyphs->SetQuadAlpha(GetCData(0)[3]);
  }
  /* -- Return text height plus line spacing ------------------------------- */
  void UpdateHeightPlusLineSpacing(void)
    { fLineSpacingHeight = fLineSpacing + fSHeight; }
  void UpdateCharSpacingTimesScale(void)
    { fCharSpacingScale = fCharSpacing * fScale; }
  void UpdateGlyphPaddingTimesScale(void)
    { fGPadScaled = fGPad * fScale; }
  /* ----------------------------------------------------------------------- */
  GLuint GetMaxTexSizeFromBounds(const GLuint uiWidth,
    const GLuint uiHeight, const GLuint uiXWidth, const GLuint uiXHeight,
    const GLuint uiMultiplier)
  { // Calculate the best possible texture size, by rounding up the
    // requested glpyh size plus the current canvas size up to the
    // nearest power of two, or double the current image size, whichever
    // value is largest.
    return Maximum(Maximum(NearestPow2<GLuint>(uiWidth + uiXWidth),
                           uiWidth * uiMultiplier),
                   Maximum(NearestPow2<GLuint>(uiHeight + uiXHeight),
                           uiHeight * uiMultiplier));
  }
  /* -- Do initialise all freetype characters in specified range ----------- */
  template<class FontTypeCheckFunc,
           class StrokerLoadTypeFunc,
           class RoundCheckFunc>
    void DoInitFTCharRange(const size_t stStart, const size_t stEnd)
      { for(size_t stIndex = stStart; stIndex < stEnd; ++stIndex)
          DoCheckGlyph<FontTypeCheckFunc,
                       StrokerLoadTypeFunc,
                       RoundCheckFunc>(stIndex); }
  /* -- Apply rounding functor before entering loop ------------------------ */
  template<class FontTypeCheckFunc, class StrokerLoadTypeFunc>
    void DoInitFTCharRangeApplyRound(const size_t stStart, const size_t stEnd)
  { // Load the character with floor rounding?
    if(ffFTLoadFlags.FlagIsSet(FF_FLOORADVANCE))
      DoInitFTCharRange<FontTypeCheckFunc, StrokerLoadTypeFunc,
        RoundCheckFuncManual<RoundFuncFloor<GLfloat>>>(stStart, stEnd);
    // Load the character with ceil rounding?
    else if(ffFTLoadFlags.FlagIsSet(FF_CEILADVANCE))
      DoInitFTCharRange<FontTypeCheckFunc, StrokerLoadTypeFunc,
        RoundCheckFuncManual<RoundFuncCeil<GLfloat>>>(stStart, stEnd);
    // Load the character with round rounding?
    else if(ffFTLoadFlags.FlagIsSet(FF_ROUNDADVANCE))
      DoInitFTCharRange<FontTypeCheckFunc, StrokerLoadTypeFunc,
        RoundCheckFuncManual<RoundFuncRound<GLfloat>>>(stStart, stEnd);
    // No rounding (allows subpixel drawing)
    DoInitFTCharRange<FontTypeCheckFunc, StrokerLoadTypeFunc,
      RoundCheckFuncManual<RoundFuncNone<GLfloat>>>(stStart, stEnd);
  }
  /* -- Apply stroker functor before entering loop ------------------------- */
  template<class FontTypeCheckFunc>
    void DoInitFTCharRangeApplyStroker(const size_t stStart,
      const size_t stEnd)
  { // Stroker requested
    if(ftfData.LoadedStroker())
    { // Load entire stroker?
      if(ffFTLoadFlags.FlagIsSet(FF_STROKETYPE2))
        DoInitFTCharRangeApplyRound<FontTypeCheckFunc,
          StrokerCheckFuncManual<StrokerLoadTypeGlyphPlusOutline2>>
            (stStart, stEnd);
      // Load outside stroker (default)
      else DoInitFTCharRangeApplyRound<FontTypeCheckFunc,
        StrokerCheckFuncManual<StrokerLoadTypeGlyphPlusOutline1>>
          (stStart, stEnd);
    }  // Load no stroker
    else DoInitFTCharRangeApplyRound<FontTypeCheckFunc,
      StrokerCheckFuncManual<StrokerLoadTypeGlyphNoOutline>>
        (stStart, stEnd);
  }
  /* -- Do initialise all freetype characters in specified string ---------- */
  template<class FontTypeCheckFunc,
           class StrokerLoadTypeFunc,
           class RoundCheckFunc>
  void DoInitFTCharString(const GLubyte*const ucpPtr)
  { // Build a new utfstring class with the string
    Decoder utfStr{ ucpPtr };
    // Enumerate trough the entire string
    while(const unsigned int uiChar = utfStr.Next())
      DoCheckGlyph<FontTypeCheckFunc,
                   StrokerLoadTypeFunc,
                   RoundCheckFunc>(uiChar);
  }
  /* -- Apply rounding functor before entering loop ------------------------ */
  template<class FontTypeCheckFunc, class StrokerLoadTypeFunc>
    void DoInitFTCharStringApplyRound(const GLubyte*const ucpPtr)
  { // Load the character with floor rounding?
    if(ffFTLoadFlags.FlagIsSet(FF_FLOORADVANCE))
      DoInitFTCharString<FontTypeCheckFunc, StrokerLoadTypeFunc,
        RoundCheckFuncManual<RoundFuncFloor<GLfloat>>>(ucpPtr);
    // Load the character with ceil rounding?
    else if(ffFTLoadFlags.FlagIsSet(FF_CEILADVANCE))
      DoInitFTCharString<FontTypeCheckFunc, StrokerLoadTypeFunc,
        RoundCheckFuncManual<RoundFuncCeil<GLfloat>>>(ucpPtr);
    // Load the character with round rounding?
    else if(ffFTLoadFlags.FlagIsSet(FF_ROUNDADVANCE))
      DoInitFTCharString<FontTypeCheckFunc, StrokerLoadTypeFunc,
        RoundCheckFuncManual<RoundFuncRound<GLfloat>>>(ucpPtr);
    // No rounding (allows subpixel drawing)
    DoInitFTCharString<FontTypeCheckFunc, StrokerLoadTypeFunc,
      RoundCheckFuncManual<RoundFuncNone<GLfloat>>>(ucpPtr);
  }
  /* -- Apply rounding functor before entering loop ------------------------ */
  template<class FontTypeCheckFunc>
    void DoInitFTCharStringApplyStroker(const GLubyte*const ucpPtr)
  { // If stroker is loaded?
    if(ftfData.LoadedStroker())
    { // Load entire stroker outline?
      if(ffFTLoadFlags.FlagIsSet(FF_STROKETYPE2))
        DoInitFTCharStringApplyRound<FontTypeCheckFunc,
          StrokerCheckFuncManual<StrokerLoadTypeGlyphPlusOutline2>>
            (ucpPtr);
      // Load outside of stroker (default)
      else DoInitFTCharStringApplyRound<FontTypeCheckFunc,
        StrokerCheckFuncManual<StrokerLoadTypeGlyphPlusOutline1>>
          (ucpPtr);
    } // Load no stroker
    else DoInitFTCharStringApplyRound<FontTypeCheckFunc,
      StrokerCheckFuncManual<StrokerLoadTypeGlyphNoOutline>>
        (ucpPtr);
  }
  /* -- Check to make sure texture was loaded as a font -------------------- */
  bool PrintSanityCheck(const Decoder &utfStr)
  { // No string? Bail out
    if(!utfStr.Valid())
      XC("Null pointer assignment!", "Identifier", IdentGet());
    // Return true if there are no characters to process
    return utfStr.Finished();
  }
  /* -- Check to make sure texture was loaded as a font + glyphs are valid - */
  bool PrintSanityCheck(const Decoder &utfStr, const Texture*const tGlyph)
  { // Can't print anything if no characters stored
    if(PrintSanityCheck(utfStr)) return false;
    // No glyph texture? Bail out
    if(!tGlyph) XC("Glyph handle to use is null!", "Identifier", IdentGet());
    // Return if there are characters to process
    return true;
  }
  /* -- Font information ------------------------------------------- */ public:
  void SetGlyphSize(const GLfloat fSize)
    { fGSize = fSize; UpdateGlyphPaddingTimesScale(); }
  size_t GetCharCount(void) const { return glData.size() / stMultiplier; }
  GLfloat GetCharScale(void) const { return fScale; }
  double GetTexOccupancy(void) const { return binData.Occupancy(); }
  /* -- Get below baseline height of specified character ------------------- */
  GLfloat GetBaselineBelow(const unsigned int uiChar)
  { // Now get character info and return data
    const Glyph &glItem = glData[CheckGlyph(uiChar)];
    return (glItem.fYMax - glItem.fH) * fScale;
  }
  /* -- Check if texture reload required ----------------------------------- */
  void CheckReloadTexture(void)
  { // Check reload command
    switch(rtCmd)
    { // No reload so just return
      case RT_NONE: return;
      // Full reload
      case RT_FULL:
      { // Reset reload command incase of error
        rtCmd = RT_NONE;
        // Full reload of texture
        ReloadTexture();
        // Log that we reuploaded the texture
        LW(LH_DEBUG, "Font '$' full texture reload (S:$,$).",
          IdentGet(), uiBWidth, uiBHeight);
        // Done
        return;
      } // Partial reload
      case RT_PARTIAL:
      { // Reset reload command incase of error
        rtCmd = RT_NONE;
        // Calculate position to read from in buffer
        const size_t stRTPos = CoordsToAbsolute(uiRTX1, uiRTY1, uiBWidth, 2);
        // Calculate position in buffer to read from
        const GLubyte*const ucpSrc =
          imData.GetFirstSlot().memData.Read<GLubyte>(stRTPos, uiBWidth);
        // Update partial texture
        UpdateEx(GetSubName(0),
          static_cast<GLint>(uiRTX1),
          static_cast<GLint>(uiRTY1),
          static_cast<GLsizei>(uiRTX2-uiRTX1),
          static_cast<GLsizei>(uiRTY2-uiRTY1),
          glPixelType, ucpSrc, static_cast<GLsizei>(uiBWidth));
        // Log that we partially reuploaded the texture
        LW(LH_DEBUG, "Font '$' partial re-upload (B:$,$,$,$;P:$).",
          IdentGet(), uiRTX1, uiRTY1, uiRTX2, uiRTY2, stRTPos);
        // Reset range parameters
        uiRTX1 = uiRTY1 = numeric_limits<GLuint>::max();
        uiRTX2 = uiRTY2 = 0;
        // Done
        return;
      }
    }
  }
  /* -- Print a string of textures, wrap at specified width, return height - */
  GLfloat PrintW(const GLfloat fX, const GLfloat fY, const GLfloat fW,
    const GLfloat fI, const GLubyte*const ucpStr)
  { // Build a new utfstring class with the string
    Decoder utfStr{ ucpStr };
    // Check that texture is a font and the string is valid
    if(PrintSanityCheck(utfStr)) return fLineSpacingHeight;
    // Push tint
    PushQuadColour();
    // Print with width
    const GLfloat fHeight = DoPrintW(fX, fY, fW, fI, utfStr);
    // Restore tint
    PopQuadColour();
    // Check if texture needs reloading
    CheckReloadTexture();
    // Return height of printed text
    return fHeight;
  }
  /* -- Simluate printing a wrapped string and return height --------------- */
  GLfloat PrintWS(const GLfloat fW, const GLfloat fI,
    const GLubyte*const ucpStr)
  { // Build a new utfstring class with the string
    Decoder utfStr{ ucpStr };
    // Check that texture is a font and the string is valid
    if(PrintSanityCheck(utfStr)) return fLineSpacingHeight;
    // Push tint
    PushQuadColour();
    // Print the utf string
    const GLfloat fHeight = DoPrintWS(fW, fI, utfStr);
    // Restore tint
    PopQuadColour();
    // Return height of printed text
    return fHeight;
  }
  /* -- Print a string of textures, wrap at specified width, return height - */
  GLfloat PrintWTS(const GLfloat fW, const GLfloat fI,
    const GLubyte*const ucpStr, Texture*const _tGlyphs)
  { // Build a new utfstring class with the string
    Decoder utfStr{ ucpStr };
    // Check that texture is a font and the string is valid
    if(PrintSanityCheck(utfStr)) return fLineSpacingHeight;
    // Save current colour and set glyph texture
    PushQuadColourAndGlyphs(_tGlyphs);
    // Print the string
    const GLfloat fHeight = DoPrintWS(fW, fI, utfStr);
    // Restore colour and reset glyph texture
    PopQuadColourAndGlyphs();
    // Return height of printed text
    return fHeight;
  }
  /* -- Print a string of textures, wrap at specified width, return height - */
  GLfloat PrintWU(const GLfloat fX, const GLfloat fY, const GLfloat fW,
    const GLfloat fI, const GLubyte*const ucpStr)
  { // Build a new utfstring class with the string
    Decoder utfStr{ ucpStr };
    // Check that texture is a font and the string is valid
    if(PrintSanityCheck(utfStr)) return fLineSpacingHeight;
    // Push tint
    PushQuadColour();
    // Simulate the height of the print
    const GLfloat fHeight = DoPrintWS(Distance(fX, fW), fI, utfStr);
    // Reset the iterator on the utf string.
    utfStr.Reset();
    // Print the string
    DoPrintW(fX, fY-fHeight, fW, fI, utfStr);
    // Restore tint
    PopQuadColour();
    // Check if texture needs reloading
    CheckReloadTexture();
    // Return height of printed text
    return fHeight;
  }
  /* -- Print a string of textures, return height -------------------------- */
  void PrintU(const GLfloat fX, const GLfloat fY, const GLubyte*const ucpStr)
  { // Build a new utfstring class with the string
    Decoder utfStr{ ucpStr };
    // Check that texture is a font and the string is valid
    if(PrintSanityCheck(utfStr)) return;
    // Push tint
    PushQuadColour();
    // Simulate the height of the print
    const GLfloat fHeight = DoPrintSU(utfStr);
    // Reset the iterator on the utf string
    utfStr.Reset();
    // Simulate the height of the print
    DoPrint(fX, fY-fHeight, fX, utfStr);
    // Check if texture needs reloading
    CheckReloadTexture();
    // Restore tint
    PopQuadColour();
  }
  /* -- Print a string of textures, wrap at specified width, return height - */
  GLfloat PrintWUT(const GLfloat fX, const GLfloat fY, const GLfloat fW,
    const GLfloat fI, const GLubyte*const ucpStr, Texture*const _tGlyphs)
  { // Build a new utfstring class with the string
    Decoder utfStr{ ucpStr };
    // Check that texture is a font and the string is valid
    if(PrintSanityCheck(utfStr)) return fLineSpacingHeight;
    // Save current colour and set glyph texture
    PushQuadColourAndGlyphs(_tGlyphs);
    // Simulate the height of the print
    const GLfloat fHeight = DoPrintWS(Distance(fX, fW), fI, utfStr);
    // Reset the iterator on the utf string
    utfStr.Reset();
    // Now actually print
    DoPrintW(fX, fY - fHeight, fW, fI, utfStr);
    // Restore colour and reset glyph texture
    PopQuadColourAndGlyphs();
    // Check if texture needs reloading
    CheckReloadTexture();
    // Return height
    return fHeight;
  }
  /* -- Print a string of textures, wrap at specified width, return height - */
  GLfloat PrintWT(const GLfloat fX, const GLfloat fY, const GLfloat fW,
    const GLfloat fI, const GLubyte*const ucpStr, Texture*const _tGlyphs)
  { // Build a new utfstring class with the string
    Decoder utfStr{ ucpStr };
    // Check that texture is a font and the string is valid
    if(PrintSanityCheck(utfStr)) return fLineSpacingHeight;
    // Save current colour and set glyph texture
    PushQuadColourAndGlyphs(_tGlyphs);
    // Print the string
    const GLfloat fHeight = DoPrintW(fX, fY, fW, fI, utfStr);
    // Restore colour and reset glyph texture
    PopQuadColourAndGlyphs();
    // Check if texture needs reloading
    CheckReloadTexture();
    // Return height
    return fHeight;
  }
  /* -- Print a string of textures ----------------------------------------- */
  void PrintT(const GLfloat fX, const GLfloat fY, const GLubyte*const ucpStr,
    Texture*const _tGlyphs)
  { // Build a new utfstring class with the string
    Decoder utfStr{ ucpStr };
    // Check that texture is a font and the string is valid
    if(PrintSanityCheck(utfStr)) return;
    // Save current colour and set glyph texture
    PushQuadColourAndGlyphs(_tGlyphs);
    // Print the string
    DoPrint(fX, fY, fX, utfStr);
    // Restore colour and reset glyph texture
    PopQuadColourAndGlyphs();
    // Check if texture needs reloading
    CheckReloadTexture();
  }
  /* -- Print a string of textures with right align ------------------------ */
  void PrintRT(const GLfloat fX, const GLfloat fY, const GLubyte*const ucpStr,
    Texture*const _tGlyphs)
  { // Build a new utfstring class with the string
    Decoder utfStr{ ucpStr };
    // Check that texture is a font and the string is valid
    if(PrintSanityCheck(utfStr)) return;
    // Save current colour and set glyph texture
    PushQuadColourAndGlyphs(_tGlyphs);
    // Print the string
    DoPrintR(fX, fY, utfStr);
    // Restore colour and reset glyph texture
    PopQuadColourAndGlyphs();
    // Check if texture needs reloading
    CheckReloadTexture();
  }
  /* -- Print a string ----------------------------------------------------- */
  void Print(const GLfloat fX, const GLfloat fY, const GLubyte*const ucpStr)
  { // Build a new utfstring class with the string
    Decoder utfStr{ ucpStr };
    // Check that texture is a font and the string is valid
    if(PrintSanityCheck(utfStr)) return;
    // Save tint
    PushQuadColour();
    // Print the utf string
    DoPrint(fX, fY, fX, utfStr);
    // Restore tint
    PopQuadColour();
    // Check if texture needs reloading
    CheckReloadTexture();
  }
  /* -- Print a string of textures with right align ------------------------ */
  void PrintR(const GLfloat fX, const GLfloat fY, const GLubyte*const ucpStr)
  { // Build a new utfstring class with the string
    Decoder utfStr{ ucpStr };
    // Check that texture is a font and the string is valid
    if(PrintSanityCheck(utfStr)) return;
    // Push tint
    PushQuadColour();
    // Print the string
    DoPrintR(fX, fY, utfStr);
    // Restore tint
    PopQuadColour();
    // Check if texture needs reloading
    CheckReloadTexture();
  }
  /* -- Print a string with centre alignment ------------------------------- */
  void PrintC(const GLfloat fX, const GLfloat fY, const GLubyte*const ucpStr)
  { // Build a new utfstring class with the string
    Decoder utfStr{ ucpStr };
    // Check that texture is a font and the string is valid
    if(PrintSanityCheck(utfStr)) return;
    // Save colour
    PushQuadColour();
    // Print the utfstring
    DoPrintC(fX, fY, utfStr);
    // Restore colour
    PopQuadColour();
    // Check if texture needs reloading
    CheckReloadTexture();
  }
  /* -- Print a string of textures ----------------------------------------- */
  void PrintCT(const GLfloat fX, const GLfloat fY, const GLubyte*const ucpStr,
    Texture*const _tGlyphs)
  { // Build a new utfstring class with the string
    Decoder utfStr{ ucpStr };
    // Check that texture is a font and the string is valid
    if(PrintSanityCheck(utfStr)) return;
    // Save current colour and set glyph texture
    PushQuadColourAndGlyphs(_tGlyphs);
    // Print the string
    DoPrintC(fX, fY, utfStr);
    // Restore colour and reset glyph texture
    PopQuadColourAndGlyphs();
    // Check if texture needs reloading
    CheckReloadTexture();
  }
  /* -- Print a string of textures with glyphs ----------------------------- */
  void PrintMT(const GLfloat fX, const GLfloat fY, const GLfloat fL,
    const GLfloat fR, const GLubyte*const ucpStr, Texture*const _tGlyphs)
  { // Build a new utfstring class with the string
    Decoder utfStr{ ucpStr };
    // Check that texture is a font and the string is valid
    if(PrintSanityCheck(utfStr)) return;
    // Save current colour and set glyph texture
    PushQuadColourAndGlyphs(_tGlyphs);
    // Print the string
    DoPrintM(fX, fY, fL, fR, utfStr);
    // Restore colour and reset glyph texture
    PopQuadColourAndGlyphs();
    // Check if texture needs reloading
    CheckReloadTexture();
  }
  /* -- Simulate printing a string and returning the height ---------------- */
  GLfloat PrintSU(const GLubyte*const ucpStr)
  { // Build a new utfstring class with the string
    Decoder utfStr{ ucpStr };
    // Check that texture is a font and the string is valid
    if(PrintSanityCheck(utfStr)) return fLineSpacingHeight;
    // Push tint
    PushQuadColour();
    // Simulate the height
    const GLfloat fHeight = DoPrintSU(utfStr);
    // Restore tint
    PopQuadColour();
    // Return highest height
    return fHeight;
  }
  /* -- Simulate printing a string ----------------------------------------- */
  GLfloat PrintS(const GLubyte*const ucpStr)
  { // Build a new utfstring class with the string
    Decoder utfStr{ ucpStr };
    // Check that texture is a font and the string is valid
    if(PrintSanityCheck(utfStr)) return 0;
    // Push tint
    PushQuadColour();
    // Print simulate of the utf string
    const GLfloat fWidth = DoPrintS(utfStr);
    // Restore tint
    PopQuadColour();
    // Return highest width or width
    return fWidth;
  }
  /* -- Simulate a string of textures with glyphs -------------------------- */
  GLfloat PrintTS(const GLubyte*const ucpStr, Texture*const _tGlyphs)
  { // Build a new utfstring class with the string
    Decoder utfStr{ ucpStr };
    // Check that texture is a font and the string is valid
    if(PrintSanityCheck(utfStr)) return 0;
    // Save current colour and set glyph texture
    PushQuadColourAndGlyphs(_tGlyphs);
    // Print the string
    const GLfloat fWidth = DoPrintS(utfStr);
    // Restore colour and reset glyph texture
    PopQuadColourAndGlyphs();
    // Return highest width or width
    return fWidth;
  }
  /* -- Print a string of textures ----------------------------------------- */
  void PrintM(const GLfloat fX, const GLfloat fY, const GLfloat fL,
    const GLfloat fR, const GLubyte*const ucpStr)
  { // Build a new utfstring class with the string
    Decoder utfStr{ ucpStr };
    // Check that texture is a font and the string is valid
    if(PrintSanityCheck(utfStr)) return;
    // Save tint
    PushQuadColour();
    // Do the print
    DoPrintM(fX, fY, fL, fR, utfStr);
    // Restore tint
    PopQuadColour();
    // Check if texture needs reloading
    CheckReloadTexture();
  }
  /* -- Set line spacing of the font --------------------------------------- */
  void SetLineSpacing(const GLfloat fNewLineSpacing)
  { // Set line spacing
    fLineSpacing = fNewLineSpacing;
    // Update line spacing plus height
    UpdateHeightPlusLineSpacing();
  }
  /* -- Set new glyph Y padding and the new scaling value ------------------ */
  void SetGlyphPadding(const GLfloat fNewGPad)
  { // Set new glyph padding
    fGPad = fNewGPad;
    // Update glyph padding times scale
    UpdateGlyphPaddingTimesScale();
  }
  /* -- Set character spacing of the font ---------------------------------- */
  void SetCharSpacing(const GLfloat fNewCharSpacing)
  { // Update character spacing value
    fCharSpacing = fNewCharSpacing;
    // Update new character spacing times scale value
    UpdateCharSpacingTimesScale();
  }
  /* -- Set size of the font ----------------------------------------------- */
  void SetSize(const GLfloat _fScale)
  { // Set scale
    fScale = Clamp(_fScale, 0, 1024);
    // Update scaled dimensions
    fSWidth = static_cast<GLfloat>(uiTWidth) * fScale;
    fSHeight = static_cast<GLfloat>(uiTHeight) * fScale;
    // Update glyph size
    SetGlyphSize(fSHeight);
    // Update frequently used values
    UpdateCharSpacingTimesScale();
    UpdateHeightPlusLineSpacing();
  }
  /* -- Render Glyph to Memory --------------------------------------------- */
  void GlyphToTexture(const Pack<GLint>::Rect &prDst,
    const FT_Bitmap &ftbGlyph, CoordData &tcItem, Memory &aDst)
  { // Get source and destination sizes and return if they're different
    const size_t
      stSrcWidth  = static_cast<size_t>(ftbGlyph.width),
      stSrcHeight = static_cast<size_t>(ftbGlyph.rows),
      stDstWidth  = static_cast<size_t>(prDst.iW),
      stDstHeight = static_cast<size_t>(prDst.iH);
    if(stSrcWidth != stDstWidth || stSrcHeight != stDstHeight) return;
    // Get destination X and Y position as size_t
    const size_t stDstX = static_cast<size_t>(prDst.iX),
                 stDstY = static_cast<size_t>(prDst.iY);
    // Put glyph data and size in a managed class
    const DataConst dcSrc{ stSrcWidth * stSrcHeight, ftbGlyph.buffer };
    // For each pixel row of glyph image
    for(size_t stPixPosY = 0; stPixPosY < stSrcHeight; ++stPixPosY)
    { // For each pixel column of glyph image
      for(size_t stPixPosX = 0; stPixPosX < stSrcWidth; ++stPixPosX)
      { // Calculate X and Y position co-ordinates in buffer...
        const size_t stPosX = stDstX + stPixPosX;
        const size_t stPosY = stDstY + stPixPosY;
        // Set the character to write...
        // * Step 1: 0xNN   (8-bit colour pixel value) converts to...
        // * Step 2: 0x00NN (16-bit integer) shift eight bits left to become...
        // * Step 3: 0xNN00 (16-bit integer) set all first eight bits...
        // * Step 4: 0xNNFF (16-bit colour alpha pixel value).
        // This will obviously need to be revised if compiling on big-endian.
        const size_t stSrcPos =
          CoordsToAbsolute(stPixPosX, stPixPosY, stSrcWidth);
        const uint16_t usPixel = static_cast<uint16_t>(
          static_cast<int>(dcSrc.ReadInt<uint8_t>(stSrcPos)) << 8 | 0xFF);
        // ...and the final offset position value
        const size_t stDstPos = CoordsToAbsolute(stPosX, stPosY, uiBWidth, 2);
        // If the paint position is in the tile bounds?
        // Copy pixels from source to destination.
        aDst.WriteInt<uint16_t>(stDstPos, usPixel);
      }
    } // Calculate texture bounds
    const GLfloat
      fMinX = static_cast<GLfloat>(stDstX),
      fMinY = static_cast<GLfloat>(stDstY),
      fMaxX = fMinX + static_cast<GLfloat>(stSrcWidth),
      fMaxY = fMinY + static_cast<GLfloat>(stSrcHeight),
      fBW   = static_cast<GLfloat>(uiBWidth),
      fBH   = static_cast<GLfloat>(uiBHeight);
    // Get reference to first and second triangles
    TriPosData &tdT1 = tcItem.fTC[0], &tdT2 = tcItem.fTC[1];
    // Push opengl tile coords (keep .fW/.fH to zero which is already zero)
    tdT1[0] = tdT1[4] = tdT2[2] = fMinX / fBW; // Left
    tdT1[1] = tdT1[3] = tdT2[5] = fMinY / fBH; // Top
    tdT1[2] = tdT2[0] = tdT2[4] = fMaxX / fBW; // Right
    tdT1[5] = tdT2[1] = tdT2[3] = fMaxY / fBH; // Bottom
  }
  /* -- Do initialise specified freetype character range ------------------- */
  void InitFTCharRange(const size_t stStart, const size_t stEnd)
  { // Ignore if not a freetype font.
    if(!ftfData.Loaded()) return;
    // Log pre-cache progress
    LW(LH_DEBUG, "Font '$' pre-caching character range $ to $...",
      IdentGet(), stStart, stEnd);
    // Load the specified character range
    DoInitFTCharRangeApplyStroker<FontTypeFreeType>(stStart, stEnd);
    // Check if any textures need reloading
    CheckReloadTexture();
    // Log success
    LW(LH_DEBUG, "Font '$' finished pre-caching character range $ to $.",
      IdentGet(), stStart, stEnd);
  }
  /* -- Do initialise all freetype characters in specified string ---------- */
  void InitFTCharString(const GLubyte*const ucpPtr)
  { // Ignore if string not valid or font not loaded
    if(!IsCStringValid(ucpPtr) || !ftfData.Loaded()) return;
    // Do load string characters
    DoInitFTCharStringApplyStroker<FontTypeFreeType>(ucpPtr);
    // Check if any textures need reloading
    CheckReloadTexture();
  }
  /* -- Do initialise freetype font ---------------------------------------- */
  void InitFTFont(Ftf &_ftfData, const GLuint uiISize, const GLuint _uiPadding,
    const GLuint _uiFilter, const FontFlagsConst &ffFlags)
  { // Make sure padding isn't negative. We use int because it is optimal for
    // use with the BinPack routines.
    if(IntWillOverflow<int>(uiPadding))
      XC("Invalid padding size!",
         "Identifier", IdentGet(), "Requested",  _uiPadding);
    // Show that we're loading the file
    LW(LH_DEBUG, "Font loading '$' (IS:$;P:$;F:$;FL:$$)...",
      _ftfData.IdentGet(), uiISize, _uiPadding, _uiFilter, hex,
      ffFlags.FlagGet());
    // If source and destination ftf class are not the same?
    if(&ftfData != &_ftfData)
    { // Assign freetype font data
      ftfData.SwapFtf(_ftfData);
      // The ftf passed in the arguments is usually still allocated by LUA
      // and will still be registered, so lets put a note in the image to show
      // that this function has nicked this font class.
      _ftfData.IdentSet(Format("!FON!$!", ftfData.IdentGet()));
    } // Pixel type is 16bit (Grayscale+Alpha)
    glPixelType = GL_RG;
    uiBitsPerPixel = 16;
    uiBytesPerPixel = 2;
    // Set initial font size. Since the font size is a float we should round
    // to next whole number so there is enough space.
    uiTWidth = static_cast<GLuint>(ceil(ftfData.GetWidth()));
    uiTHeight = static_cast<GLuint>(ceil(ftfData.GetHeight()));
    // Initialise other data
    fFWidth = ftfData.GetWidth();
    fFHeight = ftfData.GetHeight();
    uiPadding = _uiPadding;
    uiFilter = _uiFilter;
    ffFTLoadFlags.FlagReset(ffFlags);
    // Outline characters will be stored one after the opaque characters
    // thus doubling the size of the glyph data/texture coord lists
    stMultiplier = ftfData.GetOutline() > 0 ? 2 : 1;
    // Set default scaled font size and line spacing adjust
    SetSize(1);
    // Set initial size of image. The image size starts here and can
    // automatically grow by the power of 2 if more space is needed.
    if(uiISize == 0)
      uiBWidth = uiBHeight =
        GetMaxTexSizeFromBounds(0, 0, uiTWidth, uiTHeight, 1);
    // Texture size is valid
    else uiBWidth = uiBHeight = uiISize;
    // Check if texture size is valid
    if(uiBWidth > cOgl->MaxTexSize() || uiTWidth > cOgl->MaxTexSize())
      XC("Texture dimensions for font not supported by graphics processor!",
         "Identifier", IdentGet(),  "Requested",  uiISize,
         "Width",      uiBWidth, "Height",     uiBHeight,
         "TileWidth",  uiTWidth, "TileHeight", uiTHeight,
         "Maximum",    cOgl->MaxTexSize());
    // Estimate how many glyphs we're fitting in here to prevent unnecessary
    // alocations
    const size_t
      stGlyphsW = uiBWidth / uiTWidth,
      stGlyphsH = uiBHeight / uiTHeight,
      stGlyphsT = NearestPow2<size_t>(stGlyphsW * stGlyphsH);
    // Init bin packer so we can tightly pack glyphs together. We're trying to
    // guess the size of the rlFree and rlUsed structs are too.
    binData.Init(uiBWidth, uiBHeight, stGlyphsT, stGlyphsT);
    // Initialise texture image. Remember it needs to be cleared as the
    // parts that are padding will not be written to 'ever' and that would
    // cause display artifacts
    Memory aData{ uiBWidth * uiBHeight * uiBytesPerPixel };
    aData.Fill<uint16_t>(0x00FF);
    imData.InitRaw(ftfData.IdentGet(), move(aData), uiBWidth, uiBHeight,
      uiBitsPerPixel, GL_RG);
    // Texture class will NOT automatically generate a tileset because we will
    // manage this ourselves.
    bGenerateTileset = false;
    // Initialise image in opengl
    InitImage(imData, uiTWidth, uiTHeight, uiPadding, uiPadding, uiFilter);
    // Show that we've loaded the file
    LW(LH_INFO, "Font '$' loaded FT font (IS:$;P:$;F:$;FL:$$)...",
      ftfData.IdentGet(), uiISize, uiPadding, uiFilter, hex,
      ffFlags.FlagGet());
  }
  /* -- Init as a pre-rendered font ---------------------------------------- */
  void InitFont(Image &imSrc)
  { // Get filename from image and use it as an id for the font
    IdentSet(imSrc.IdentGet());
    // Show filename
    LW(LH_DEBUG, "Font loading '$' from bitmap.", IdentGet());
    // Strip path name and set descriptor file name
    const PathSplit pathData{ IdentGet() };
    const string strIndex{ Append(pathData.strLoc, pathData.strFile, ".txt") };
    // Get the file. It should at least be 3 bytes long
    const FileMap fC{ AssetExtract(strIndex) };
    if(fC.Size() < 3)
      XC("Index file is possibly corrupted!",
         "Identifier", IdentGet(), "Index", strIndex);
    // Convert whole file data to a string
    const string strBuffer{ fC.ToString() };
    // Get format of CR/LF in buffer. Throw error if not found
    const string strTokens{ GetTextFormat(strBuffer) };
    if(strTokens.empty())
      XC("Index file format not detected!",
         "Identifier",   IdentGet(), "Index", strIndex,
         "BufferLength", strBuffer.length());
    // Split variables and throw error if there are no vars
    const VarsConst vC{ strBuffer, strTokens, '=' };
    if(!vC.size())
      XC("No metadata in index file!",
         "Identifier", IdentGet(), "Index", strIndex);
    // Get number of characters and offset.
    const unsigned int uiCharCount = ToNumber<unsigned int>(vC["range"]);
    const unsigned int uiCharOffset = ToNumber<unsigned int>(vC["rangestart"]);
    if(!uiCharCount)
      XC("Invalid character count in metadata!",
         "Identifier", IdentGet(),      "Index", strIndex,
         "Offset",     uiCharOffset, "Count", uiCharCount);
    const unsigned int uiCharEnd = uiCharOffset + uiCharCount;
    // Set and check default character and if it's a number
    const string strDefaultChar{ vC["default"] };
    if(strDefaultChar.empty())
      XC("Empty default character index in metadata!",
         "Identifier", IdentGet());
    // Check if it's a literal number as prefixed by a hash sign? Convert as a
    // literal number else convert as an a literal character then verify it is
    // in the specified range.
    ulDefaultChar = (strDefaultChar.length() > 1 && strDefaultChar[0] == '#')
      ? ToNumber<unsigned int>(strDefaultChar.substr(1))
      : Decoder{ strDefaultChar }.Next();
    if(ulDefaultChar < uiCharOffset || ulDefaultChar >= uiCharEnd)
      XC("Default character index in metadata out of range!",
         "Identifier", IdentGet(),        "Index",   strIndex,
         "Value",      strDefaultChar, "Default", ulDefaultChar,
         "Minimum",    uiCharOffset,   "Maximum", uiCharEnd-1,
         "Count",      uiCharCount);
    // Get filter
    uiFilter = ToNumber<unsigned int>(vC["filter"]);
    if(uiFilter > 11)
      XC("Invalid filter index specified in font metadata!",
         "Identifier", IdentGet(), "Filter", uiFilter);
    // Look for widths and throw if there are none then report them in log
    const string strWidths{ vC["width"] };
    if(strWidths.empty())
      XC("No widths found in metadata!",
         "Identifier", IdentGet(), "Index", strIndex);
    LW(LH_DEBUG, "- Widths: $.", strWidths);
    // Break apart the widths and throw if there are none
    const StrVector svList{ move(Token{ strWidths, strSpace }) };
    if(svList.size() != uiCharCount)
      XC("Unexpected number of widths detected in metadata!",
         "Identifier", IdentGet(),  "Index",  strIndex,
         "Expect",     uiCharCount, "Actual", svList.size());
    // Reserve memory and create empty elements for unused glyph/tiles. We
    // don't know what the initial values for glData and tcList will be at
    // first until we have setup the default character.
    glData.reserve(uiCharEnd); glData.resize(uiCharOffset);
    tcList.reserve(uiCharEnd); tcList.resize(uiCharOffset);
    // Read size of tile. Texture init will clamp this if needed.
    const unsigned int
      uiTW = ToNumber<unsigned int>(vC["tilewidth"]),
      uiTH = ToNumber<unsigned int>(vC["tileheight"]);
    // Convert to float as we need a float version of this in the next loop
    const GLfloat fW = static_cast<GLfloat>(uiTW),
                  fH = static_cast<GLfloat>(uiTH);
    // Populate character data
    transform(svList.cbegin(), svList.cend(), back_inserter(glData),
      [fW, fH](const string &strWidth)->const Glyph
        { return { true, fW,fH, ToNumber<GLfloat>(strWidth), 0,0, fW,fH }; });
    // Initialise the uninitialised glyphs with the default character
    const unsigned int uiCharOffsetM1 = uiCharOffset-1;
    MYFILL(par_unseq, glData.begin(),
      glData.begin()+uiCharOffsetM1, glData[ulDefaultChar]);
    // Get extra tile padding dimensions. Also clamped by texture class
    const unsigned int
      uiPX = ToNumber<unsigned int>(vC["tilespacingwidth"]),
      uiPY = ToNumber<unsigned int>(vC["tilespacingheight"]);
    // Multiplier is always 1 for bitmap fonts. No outline support.
    stMultiplier = 1;
    // Init texture with custom parameters and generate tileset
    InitImage(imSrc, uiTW, uiTH, uiPX, uiPY, uiFilter);
    // Initialise the uninitialised texcoords with the default character that
    // was initialised using InitImage
    MYFILL(par_unseq, tcList.begin(),
      tcList.begin()+uiCharOffsetM1, tcList[ulDefaultChar]);
    // Initialise font scale
    SetSize(ToNumber<GLfloat>(vC["scale"]));
    // Show that we've loaded the file
    LW(LH_INFO, "Font '$' loaded from bitmap (T:$x$;F:$).",
      IdentGet(), uiTW, uiTH, uiFilter);
  }
  /* -- Constructor (Initialisation then registration) --------------------- */
  Font(void) :                         // No parameters
    /* -- Initialisation of members ---------------------------------------- */
    ICHelperFont{ *cFonts, this }      // Initially registered
    /* --------------------------------------------------------------------- */
    { }                                // Do nothing else
  /* -- Constructor (without registration) --------------------------------- */
  explicit Font(const bool) :          // Dummy parameter
    /* -- Initialisation of members ---------------------------------------- */
    ICHelperFont{ *cFonts }            // Initially unregistered
    /* --------------------------------------------------------------------- */
    { }                                // Do nothing else
  /* ----------------------------------------------------------------------- */
  DELETECOPYCTORS(Font);               // Omit copy constructor for safety
};/* -- Finish the collector ----------------------------------------------- */
END_COLLECTOR(Fonts);
/* -- DeInit Font Textures ------------------------------------------------- */
static void FontDeInitTextures(void)
{ // Ignore if no fonts
  if(cFonts->empty()) return;
  // De-init all the font textures and log pre/post deinit
  LW(LH_DEBUG, "Fonts de-initialising $ objects...", cFonts->size());
  for(Font*const fCptr : *cFonts) fCptr->DeInit();
  LW(LH_INFO, "Fonts de-initialising $ objects.", cFonts->size());
}
/* -- Reload Font Textures ------------------------------------------------- */
static void FontReInitTextures(void)
{ // Ignore if no fonts
  if(cFonts->empty()) return;
  // Re-init all the font textures and log pre/post init
  LW(LH_DEBUG, "Fonts re-initialising $ objects...", cFonts->size());
  for(Font*const fCptr : *cFonts) fCptr->ReloadTexture();
  LW(LH_INFO, "Fonts re-initialising $ objects.", cFonts->size());
}
/* -- End of module namespace ---------------------------------------------- */
};                                     // End of interface
/* == EoF =========================================================== EoF == */

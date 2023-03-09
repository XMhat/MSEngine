/* == MASK.HPP ============================================================= */
/* ######################################################################### */
/* ## MS-ENGINE              Copyright (c) MS-Design, All Rights Reserved ## */
/* ######################################################################### */
/* ## 1-bit bitmask collision system.                                     ## */
/* ######################################################################### */
/* ========================================================================= */
#pragma once                           // Only one incursion allowed
/* -- Module namespace ----------------------------------------------------- */
namespace IfMask {                     // Keep declarations neatly categorised
/* -- Includes ------------------------------------------------------------- */
using namespace IfImage;               // Using image interface
/* == Mask collector and member class ====================================== */
BEGIN_COLLECTORDUO(Masks, Mask, CLHelperUnsafe, ICHelperUnsafe),
  /* -- Base classes ------------------------------------------------------- */
  public MemoryVector,                 // Slots for each mask
  public Lockable,                     // Lua garbage collector instruction
  public Ident                         // Name of mask object
{ /* -- Variables -------------------------------------------------- */ public:
  int              iWidth,iHeight;     // Size of mask image
  size_t           stSize;             // Size of all mask bitmaps in array
  /* -- Two masks overlap? ------------------------------------------------- */
  bool IsCollide(const size_t stSourceId, const int iSrcX, const int iSrcY,
    const Mask &mCdest, const size_t stDestId, const int iDestX,
    const int iDestY) const
  { // Caculate distances and lengths between both bounds
    const int
      iXMax1 = iSrcX + iWidth,          iYMax1 = iSrcY + iHeight,
      iXMax2 = iDestX + mCdest.iWidth,  iYMax2 = iDestY + mCdest.iHeight,
      iXMin  = Maximum(iSrcX, iDestX),  iYMin  = Maximum(iSrcY, iDestY),
      iXMax  = Minimum(iXMax1, iXMax2), iYMax  = Minimum(iYMax1, iYMax2);
    // Bail if out of bounds
    if(iXMax <= iXMin || iYMax <= iYMin) return false;
    // Get bitmask surfaces for both masks
    const unsigned char*const cpS = at(stSourceId).Ptr<unsigned char>(),
                       *const cpD = mCdest[stDestId].Ptr<unsigned char>();
    // Walk through the pixels of the intersection and check the bits and
    // return if we found a match, else try next pixel
    for(int iY = iYMin; iY < iYMax; ++iY)
      for(int iX = iXMin; iX < iXMax; ++iX)
        if(Bit::Test(cpS, ((iY-iSrcY)*iWidth)+(iX-iSrcX)) &&
           Bit::Test(cpD, ((iY-iDestY)*mCdest.iWidth)+(iX-iDestX)))
          return true;
    // No collision
    return false;
  }
  /* -- Two masks overlap with raycasting? --------------------------------- */
  bool Raycast(const size_t stSourceId, const int iSrcX, const int iSrcY,
    const Mask &mCdest, const size_t stDestId, const int iFromX,
    const int iFromY, int &iToX, int &iToY) const
  { // Make calculations
    int         iDX = abs(iToX - iFromX), iDY = abs(iToY - iFromY),
                 iX = iFromX,              iY = iFromY,
                iLX = iFromX,             iLY = iFromY,
                 iN = 1 + iDX + iDY,   iError = iDX - iDY;
    const int iXinc = iToX > iFromX ? 1 : iToX < iFromX ? -1 : 0,
              iYinc = iToY > iFromY ? 1 : iToY < iFromY ? -1 : 0;
    // Make further calculations
    iDX *= 2;
    iDY *= 2;
    // Iterations
    for(;iN > 0; --iN)
    { // Check collision and return if found? Set last good position
      if(IsCollide(stSourceId, iSrcX, iSrcY, mCdest, stDestId, iX, iY))
        { iToX = iLX; iToY = iLY; return true; }
      // Check for errors
      else if(iError > 0) { iLX = iX; iX += iXinc; iError -= iDY; }
      else                { iLY = iY; iY += iYinc; iError += iDX; }
    }
    // No collision
    return false;
  }
  /* -- Copy specified mask into this one ---------------------------------- */
  void Copy(const size_t stDestId, const Mask &mCsrc,
    const size_t stSourceId, const int iDestX, const int iDestY)
  { // Caculate distances and lengths between both bounds
    const int
      iXMax2 = iDestX + mCsrc.iWidth,   iYMax2 = iDestY + mCsrc.iHeight,
      iXMin  = Maximum(iDestX, 0),      iYMin  = Maximum(iDestY, 0),
      iXMax  = Minimum(iWidth, iXMax2), iYMax  = Minimum(iHeight, iYMax2);
    // Bail if out of bounds
    if(iXMax <= iXMin || iYMax <= iYMin) return;
    // Get bitmask surfaces for both masks
    const unsigned char*const cpS = mCsrc[stSourceId].Ptr<unsigned char>();
          unsigned char*const cpD = at(stDestId).Ptr<unsigned char>();
    // Walk through the pixels of the intersection
    for(int iY = iYMin; iY < iYMax; ++iY)
      for(int iX = iXMin; iX < iXMax; ++iX)
      { // Clear the bits first
        Bit::Clear(cpD, (iY*iWidth)+iX);
        // Copy the bits
        Bit::Set2(cpD, (iY*iWidth)+iX,
                  cpS, ((iY-iDestY)*mCsrc.iWidth)+(iX-iDestX));
      }
  }
  /* -- Merge specified mask into this one --------------------------------- */
  void Merge(const Mask &mCsrc, const size_t stSourceId,
    const int iDestX, const int iDestY)
  { // Caculate distances and lengths between both bounds
    const int
      iXMax2 = iDestX + mCsrc.iWidth,   iYMax2 = iDestY + mCsrc.iHeight,
      iXMin  = Maximum(iDestX, 0),      iYMin  = Maximum(iDestY, 0),
      iXMax  = Minimum(iWidth, iXMax2), iYMax  = Minimum(iHeight, iYMax2);
    // Bail if out of bounds
    if(iXMax <= iXMin || iYMax <= iYMin) return;
    // Get bitmask surfaces for both masks
          unsigned char*const cpD = at(0).Ptr<unsigned char>();
    const unsigned char*const cpS = mCsrc[stSourceId].Ptr<unsigned char>();
    // Walk through the pixels of the intersection
    // Check the bits and return if we found a match, else try next pixel
    for(int iY = iYMin; iY < iYMax; ++iY)
      for(int iX = iXMin; iX < iXMax; ++iX)
        Bit::Set2(cpD, (iY*iWidth)+iX,
                  cpS, ((iY-iDestY)*mCsrc.iWidth)+(iX-iDestX));
  }
  /* -- Erase specified mask into this one --------------------------------- */
  void Erase(const size_t stDestId)
  { // Bail if out of bounds
    if(iWidth <= 0 || iHeight <= 0) return;
    // Get bitmask surfaces for both masks
    unsigned char*const cpD = at(stDestId).Ptr<unsigned char>();
    // Walk through the pixels of the intersection
    // Check the bits and return if we found a match, else try next pixel
    for(int iY = 0; iY < iHeight; ++iY)
      for(int iX = 0; iX < iWidth; ++iX)
        Bit::Clear(cpD, (iY*iWidth)+iX);
  }
  /* -- Fill specified mask ------------------------------------------------ */
  void Fill(const int iDX, const int iDY, const int iW, const int iH)
  { // Caculate distances and lengths between both bounds
    const int
      iXMax2 = iDX + iW,                iYMax2 = iDY + iH,
      iXMin  = Maximum(iDX, 0),         iYMin  = Maximum(iDY, 0),
      iXMax  = Minimum(iWidth, iXMax2), iYMax  = Minimum(iHeight, iYMax2);
    // Bail if out of bounds
    if(iXMax <= iXMin || iYMax <= iYMin) return;
    // Get bitmask surfaces for both masks
    unsigned char*const cpD = at(0).Ptr<unsigned char>();
    // Walk through the pixels of the intersection and set each bit
    for(int iY = iYMin; iY < iYMax; ++iY)
      for(int iX = iXMin; iX < iXMax; ++iX)
        Bit::Set(cpD, (iY*iWidth)+iX);
  }
  /* -- Clear specified mask ----------------------------------------------- */
  void Clear(const int iDX, const int iDY, const int iW, const int iH)
  { // Caculate distances and lengths between both bounds
    const int
      iXMax2 = iDX + iW,                iYMax2 = iDY + iH,
      iXMin  = Maximum(iDX, 0),         iYMin  = Maximum(iDY, 0),
      iXMax  = Minimum(iWidth, iXMax2), iYMax  = Maximum(iHeight, iYMax2);
    // Bail if out of bounds
    if(iXMax <= iXMin || iYMax <= iYMin) return;
    // Get bitmask surfaces for both masks
    unsigned char*const cpD = at(0).Ptr<unsigned char>();
    // Walk through the pixels of the intersection and set each bit
    for(int iY = iYMin; iY < iYMax; ++iY)
      for(int iX = iXMin; iX < iXMax; ++iX)
        Bit::Clear(cpD, (iY*iWidth)+iX);
  }
  /* -- Init --------------------------------------------------------------- */
  void Init(const unsigned int uiW, const unsigned int uiH)
  { // Check dimension parameters
    if(!uiW || !uiH || IntWillOverflow<int>(uiW) || IntWillOverflow<int>(uiH))
      XC("Mask dimensions are invalid!", "Width", uiW, "Height", uiH);
    // Calculate space required, push it into mask list and increment size
    const size_t stLen = (uiW * uiH) / 8;
    emplace_back(Memory{ stLen });
    stSize = stSize + stLen;
    // Set name of mask
    IdentSet(Format(":$*$", uiW, uiH));
    // Set width and height
    iWidth = static_cast<int>(uiW);
    iHeight = static_cast<int>(uiH);
  }
  /* -- Init cleared mask -------------------------------------------------- */
  void InitBlank(const unsigned int uiW, const unsigned int uiH)
  { // Initialise new mask memory
    Init(uiW, uiH);
    // Now clear it
    back().Fill();
  }
  /* -- Dump a tile to disk ------------------------------------------------ */
  void Dump(const size_t stId, const string &strFile) const
  { // Create file. It will be auto closed when we leave this scope
    if(FStream fOut{ strFile, FStream::FM_W_B }) try
    { // Memory to write as we need to reverse the bits
      const DataConst &dcSrc = at(stId);
      // Create targa header
      Memory mDst{ CodecTGA::HL_MAX + dcSrc.Size() };
      // Fill in file header data
      mDst.WriteInt<uint8_t>(CodecTGA::HL_U08LE_LEN_AFTER_HEADER, 0);
      mDst.WriteInt<uint8_t>(CodecTGA::HL_U08LE_COLOUR_MAP_TYPE, 0);
      mDst.WriteInt<uint8_t>(CodecTGA::HL_U08LE_DATA_TYPE_CODE, 3);
      mDst.WriteIntLE<uint16_t>(CodecTGA::HL_U16LE_COLOUR_MAP_ORIGIN, 0);
      mDst.WriteIntLE<uint16_t>(CodecTGA::HL_U16LE_COLOUR_MAP_LENGTH, 0);
      mDst.WriteInt<uint8_t>(CodecTGA::HL_U08LE_COLOUR_MAP_DEPTH, 0);
      mDst.WriteIntLE<uint16_t>(CodecTGA::HL_U16LE_X_ORIGIN, 0);
      mDst.WriteIntLE<uint16_t>(CodecTGA::HL_U16LE_Y_ORIGIN, 0);
      mDst.WriteIntLE<uint16_t>(CodecTGA::HL_U16LE_WIDTH,
        static_cast<uint16_t>(iWidth));
      mDst.WriteIntLE<uint16_t>(CodecTGA::HL_U16LE_HEIGHT,
        static_cast<uint16_t>(iHeight));
      mDst.WriteInt<uint8_t>(CodecTGA::HL_U08LE_BITS_PER_PIXEL, 1);
      mDst.WriteInt<uint8_t>(CodecTGA::HL_U08LE_IMAGE_DESCRIPTOR,
        CodecTGA::IDF_FLIPPED);
      // Read src, reverse bits and write to destination
      for(size_t stSrcPos = 0; stSrcPos < dcSrc.Size(); ++stSrcPos)
        mDst.WriteInt<uint8_t>(CodecTGA::HL_MAX + stSrcPos,
          ReverseByte(dcSrc.ReadInt<uint8_t>(stSrcPos)));
      // Write targa header
      const size_t stHdrWritten = fOut.FStreamWriteBlock(mDst);
      if(stHdrWritten != mDst.Size())
        XCL("Could not write targa data mask!",
            "File", strFile, "Expected", mDst.Size(), "Actual", stHdrWritten);
    } // exception occured?
    catch(const exception &)
    { // Close the file and delete it
      fOut.FStreamClose();
      DirFileUnlink(strFile);
      // Throw original error
      throw;
    } // Failed to open file
    else XCL("Could not open file for writing mask!", "File", strFile);
  }
  /* -- InitFromFile ------------------------------------------------------- */
  void InitFromImage(Image &iC, const unsigned int _uiTWidth,
    const unsigned int _uiTHeight)
  { // Set texture name
    IdentSet(iC.IdentGet());
    // Must have slots
    if(iC.NoSlots()) XC("No data in image object!", "Identifier", IdentGet());
    // Check dimensions. We're also working with ints for sizes so we have
    // to limit the size to signed int range so check for that too.
    if(!_uiTWidth || !_uiTHeight ||
      IntWillOverflow<int>(_uiTWidth) ||
      IntWillOverflow<int>(_uiTHeight))
        XC("Invalid tile dimensions!",
           "Identifier", IdentGet(), "Width", _uiTWidth, "Height", _uiTHeight);
    // Get first image slot and show error as we are not reversing this.
    ImageSlot &bData = iC.GetFirstSlot();
    // Check bit depth
    if(iC.GetBitsPP() != 1)
      XC("Image is not monochrome!",
         "Identifier", IdentGet(),        "Width",        bData.uiWidth,
         "Height",     bData.uiHeight, "BitsPerPixel", iC.GetBitsPP());
    // If the targa is reversed?
    if(iC.IsReverse())
    { // We need to reverse the pixels
      iC.ReversePixels();
      // Put warning in log as it's a performance warning
      LW(LH_WARNING, "Mask '$' was forced unreversal!", IdentGet());
    } // Check image dimensions too. Again we're dealing with ints!
    if(!bData.uiWidth || !bData.uiHeight ||
      IntWillOverflow<int>(bData.uiWidth) ||
      IntWillOverflow<int>(bData.uiHeight))
        XC("Invalid image dimensions!",
           "Identifier", IdentGet(), "Width", bData.uiWidth,
           "Height",     bData.uiHeight);
    // Image is divisible by 8?
    if(!IsDivisible(static_cast<double>(bData.uiWidth)/8) ||
       !IsDivisible(static_cast<double>(bData.uiHeight)/8))
      XC("Image dimensions are not divisible by eight!",
         "Identifier", IdentGet(), "Width", bData.uiWidth,
         "Height",     bData.uiHeight);
    // Get reference to the image memory and if no tiling needed?
    Memory &mDst = bData.memData;
    if(bData.uiWidth == _uiTWidth && bData.uiHeight == _uiTHeight)
    { // Use full size image and return
      emplace_back(move(mDst));
      return;
    } // Calculate how many tiles we can fit in the image
    const unsigned int uiTotalX = bData.uiWidth / _uiTWidth,
                       uiTotalY = bData.uiHeight / _uiTHeight,
    // Properly clamped tile count value
                       uiTotalXWhole = uiTotalX * _uiTWidth,
                       uiTotalYWhole = uiTotalY * _uiTHeight;
    // Calculate size of the bitmask required
    const size_t stBytes = _uiTWidth * _uiTHeight / CHAR_BIT;
    // Get source buffer
    const unsigned char*const ucpS = mDst.Ptr<unsigned char>();
    // Reserve memory for all the tiles
    reserve(uiTotalX * uiTotalY);
    // For each tile
    for(unsigned int uiY = 0; uiY < uiTotalYWhole; uiY += _uiTHeight)
      for(unsigned int uiX = 0; uiX < uiTotalXWhole; uiX += _uiTWidth)
      { // Create cleared mask buffer, insert it into list and get ptr to
        // the memory.
        unsigned char*const ucpD = emplace(end(),
          Memory{ stBytes, true })->Ptr<unsigned char>();
        // Copy source to buffer
        for(unsigned int uiTY = 0; uiTY < _uiTHeight; ++uiTY)
          for(unsigned int uiTX = 0; uiTX < _uiTWidth; ++uiTX)
            Bit::Set2(ucpD, (uiTY * _uiTWidth) + uiTX, ucpS,
              ((bData.uiHeight - 1 - (uiY + uiTY)) * bData.uiWidth) +
                (uiX + uiTX));
        // Increment size
        stSize = stSize + stBytes;
      }
    // The mask passed in the arguments is usually still allocated by LUA and
    // will still be registered, sp lets put a note in the mask to show that
    // this function has nicked the mask.
    iC.IdentSet(Format("!MAS!$!", iC.IdentGet()));
    // Tell log what we did
    LW(LH_INFO, "Mask created $ ($x$) tiles from a $x$ bitmask.",
      size(), _uiTWidth, _uiTHeight, bData.uiWidth, bData.uiHeight);
    // Set new size and tile size
    iWidth = static_cast<int>(_uiTWidth);
    iHeight = static_cast<int>(_uiTHeight);
  }
  /* -- Constructor -------------------------------------------------------- */
  Mask(void) :
    /* -- Initialisation of members ---------------------------------------- */
    ICHelperMask{ *cMasks, this },     // Register this object in collector
    iWidth(0),                         // Uninitialised width
    iHeight(0),                        // Uninitialised height
    stSize(0)                          // Uninitialised size
    /* -- No code ---------------------------------------------------------- */
    { }
  /* ----------------------------------------------------------------------- */
  DELETECOPYCTORS(Mask);               // Omit copy constructor for safety
};/* ----------------------------------------------------------------------- */
END_COLLECTOR(Masks);
/* -- End of module namespace ---------------------------------------------- */
};                                     // End of interface
/* == EoF =========================================================== EoF == */

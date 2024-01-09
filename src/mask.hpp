/* == MASK.HPP ============================================================= **
** ######################################################################### **
** ## MS-ENGINE              Copyright (c) MS-Design, All Rights Reserved ## **
** ######################################################################### **
** ## 1-bit bitmask collision system.                                     ## **
** ######################################################################### **
** ========================================================================= */
#pragma once                           // Only one incursion allowed
/* ------------------------------------------------------------------------- */
namespace IMask {                      // Start of private module namespace
/* -- Dependencies --------------------------------------------------------- */
using namespace ICollector::P;         using namespace IDim;
using namespace IDir::P;               using namespace IError::P;
using namespace IIdent::P;             using namespace IImage::P;
using namespace IImageDef::P;          using namespace ILog::P;
using namespace IMemory::P;            using namespace IStd::P;
using namespace ISysUtil::P;           using namespace IUtil::P;
/* ------------------------------------------------------------------------- */
namespace P {                          // Start of public module namespace
/* == Mask collector and member class ====================================== */
BEGIN_COLLECTORDUO(Masks, Mask, CLHelperUnsafe, ICHelperUnsafe),
  /* -- Base classes ------------------------------------------------------- */
  public MemoryVector,                 // Slots for each mask
  public Lockable,                     // Lua garbage collector instruction
  public Ident,                        // Name of mask object
  public Dimensions<int>               // Size of mask image
{ /* -- Variables ---------------------------------------------------------- */
  size_t           stAlloc;            // Size of all mask bitmaps in array
  /* -- Two masks overlap? ----------------------------------------- */ public:
  bool IsCollide(const size_t stSourceId, const int iSrcX, const int iSrcY,
    const Mask &mCdest, const size_t stDestId, const int iDestX,
    const int iDestY) const
  { // Caculate distances and lengths between both bounds
    const int
      iXMax1 = iSrcX + DimGetWidth(),
      iYMax1 = iSrcY + DimGetHeight(),
      iXMax2 = iDestX + mCdest.DimGetWidth(),
      iYMax2 = iDestY + mCdest.DimGetHeight(),
      iXMin  = UtilMaximum(iSrcX, iDestX),
      iYMin  = UtilMaximum(iSrcY, iDestY),
      iXMax  = UtilMinimum(iXMax1, iXMax2),
      iYMax  = UtilMinimum(iYMax1, iYMax2);
    // Bail if out of bounds
    if(iXMax <= iXMin || iYMax <= iYMin) return false;
    // Get bitmask surfaces for both masks
    const unsigned char*const cpS = at(stSourceId).Ptr<unsigned char>(),
                       *const cpD = mCdest[stDestId].Ptr<unsigned char>();
    // Walk through the pixels of the intersection and check the bits and
    // return if we found a match, else try next pixel
    for(int iY = iYMin; iY < iYMax; ++iY)
      for(int iX = iXMin; iX < iXMax; ++iX)
        if(UtilBitTest(cpS, ((iY-iSrcY)*DimGetWidth())+(iX-iSrcX)) &&
           UtilBitTest(cpD, ((iY-iDestY)*mCdest.DimGetWidth())+(iX-iDestX)))
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
      iXMax2 = iDestX + mCsrc.DimGetWidth(),
      iYMax2 = iDestY + mCsrc.DimGetHeight(),
      iXMin  = UtilMaximum(iDestX, 0),
      iYMin  = UtilMaximum(iDestY, 0),
      iXMax  = UtilMinimum(DimGetWidth(), iXMax2),
      iYMax  = UtilMinimum(DimGetHeight(), iYMax2);
    // Bail if out of bounds
    if(iXMax <= iXMin || iYMax <= iYMin) return;
    // Get bitmask surfaces for both masks
    const unsigned char*const cpS = mCsrc[stSourceId].Ptr<unsigned char>();
          unsigned char*const cpD = at(stDestId).Ptr<unsigned char>();
    // Walk through the pixels of the intersection
    for(int iY = iYMin; iY < iYMax; ++iY)
      for(int iX = iXMin; iX < iXMax; ++iX)
      { // Clear the bits first
        UtilBitClear(cpD, (iY*DimGetWidth())+iX);
        // Copy the bits
        UtilBitSet2(cpD, (iY*DimGetWidth())+iX,
                  cpS, ((iY-iDestY)*mCsrc.DimGetWidth())+(iX-iDestX));
      }
  }
  /* -- Merge specified mask into this one --------------------------------- */
  void Merge(const Mask &mCsrc, const size_t stSourceId,
    const int iDestX, const int iDestY)
  { // Caculate distances and lengths between both bounds
    const int
      iXMax2 = iDestX + mCsrc.DimGetWidth(),
      iYMax2 = iDestY + mCsrc.DimGetHeight(),
      iXMin  = UtilMaximum(iDestX, 0),
      iYMin  = UtilMaximum(iDestY, 0),
      iXMax  = UtilMinimum(DimGetWidth(), iXMax2),
      iYMax  = UtilMinimum(DimGetHeight(), iYMax2);
    // Bail if out of bounds
    if(iXMax <= iXMin || iYMax <= iYMin) return;
    // Get bitmask surfaces for both masks
          unsigned char*const cpD = at(0).Ptr<unsigned char>();
    const unsigned char*const cpS = mCsrc[stSourceId].Ptr<unsigned char>();
    // Walk through the pixels of the intersection
    // Check the bits and return if we found a match, else try next pixel
    for(int iY = iYMin; iY < iYMax; ++iY)
      for(int iX = iXMin; iX < iXMax; ++iX)
        UtilBitSet2(cpD, (iY*DimGetWidth())+iX,
                    cpS, ((iY-iDestY)*mCsrc.DimGetWidth())+(iX-iDestX));
  }
  /* -- Erase specified mask into this one --------------------------------- */
  void Erase(const size_t stDestId)
  { // Bail if out of bounds
    if(DimGetWidth() <= 0 || DimGetHeight() <= 0) return;
    // Get bitmask surfaces for both masks
    unsigned char*const cpD = at(stDestId).Ptr<unsigned char>();
    // Walk through the pixels of the intersection
    // Check the bits and return if we found a match, else try next pixel
    for(int iY = 0; iY < DimGetHeight(); ++iY)
      for(int iX = 0; iX < DimGetWidth(); ++iX)
        UtilBitClear(cpD, (iY*DimGetWidth())+iX);
  }
  /* -- Fill specified mask ------------------------------------------------ */
  void Fill(const int iDX, const int iDY, const int iW, const int iH)
  { // Caculate distances and lengths between both bounds
    const int
      iXMax2 = iDX + iW,                iYMax2 = iDY + iH,
      iXMin  = UtilMaximum(iDX, 0),     iYMin  = UtilMaximum(iDY, 0),
      iXMax  = UtilMinimum(DimGetWidth(), iXMax2),
      iYMax  = UtilMinimum(DimGetHeight(), iYMax2);
    // Bail if out of bounds
    if(iXMax <= iXMin || iYMax <= iYMin) return;
    // Get bitmask surfaces for both masks
    unsigned char*const cpD = at(0).Ptr<unsigned char>();
    // Walk through the pixels of the intersection and set each bit
    for(int iY = iYMin; iY < iYMax; ++iY)
      for(int iX = iXMin; iX < iXMax; ++iX)
        UtilBitSet(cpD, (iY*DimGetWidth())+iX);
  }
  /* -- Clear specified mask ----------------------------------------------- */
  void Clear(const int iDX, const int iDY, const int iW, const int iH)
  { // Caculate distances and lengths between both bounds
    const int
      iXMax2 = iDX + iW,               iYMax2 = iDY + iH,
      iXMin  = UtilMaximum(iDX, 0),    iYMin  = UtilMaximum(iDY, 0),
      iXMax  = UtilMinimum(DimGetWidth(), iXMax2),
      iYMax  = UtilMaximum(DimGetHeight(), iYMax2);
    // Bail if out of bounds
    if(iXMax <= iXMin || iYMax <= iYMin) return;
    // Get bitmask surfaces for both masks
    unsigned char*const cpD = at(0).Ptr<unsigned char>();
    // Walk through the pixels of the intersection and set each bit
    for(int iY = iYMin; iY < iYMax; ++iY)
      for(int iX = iXMin; iX < iXMax; ++iX)
        UtilBitClear(cpD, (iY*DimGetWidth())+iX);
  }
  /* -- Init --------------------------------------------------------------- */
  void Init(const unsigned int uiW, const unsigned int uiH)
  { // Check dimension parameters
    if(!uiW || !uiH ||
       UtilIntWillOverflow<int>(uiW) || UtilIntWillOverflow<int>(uiH))
      XC("Mask dimensions are invalid!", "Width", uiW, "Height", uiH);
    // Calculate space required, push it into mask list and increment size
    const size_t stLen = (uiW * uiH) / 8;
    emplace_back(Memory{ stLen });
    stAlloc += stLen;
    // Set name of mask
    IdentSet(":$*$", uiW, uiH);
    // Set width and height
    DimSet(static_cast<int>(uiW), static_cast<int>(uiH));
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
  { // Get source slot
    const DataConst &dcSrc = (*this)[stId];
    // Copy the slot because the image init moves it
    Memory mDst{ dcSrc.Size(), dcSrc.Ptr() };
    // Byte swap it
    mDst.ByteSwap8();
    // Setup raw image
    const Image imOut{ strFile, StdMove(mDst), DimGetWidth<unsigned int>(),
      DimGetHeight<unsigned int>(), BD_BINARY, GL_NONE };
    // Capture exceptions
    try
    { // Save bitmap to PNG
      imOut.SaveFile(imOut.IdentGet(), 0, 0);
    } // exception occured?
    catch(const exception &)
    { // Close the file and delete it
      DirFileUnlink(imOut.IdentGet());
      // Throw original error
      throw;
    }
  }
  /* -- InitFromFile ------------------------------------------------------- */
  void InitFromImage(Image &iC, const unsigned int _uiTWidth,
    const unsigned int _uiTHeight)
  { // Set texture name
    IdentSet(iC);
    // Must have slots
    if(iC.IsNoSlots())
      XC("No data in image object!", "Identifier", IdentGet());
    // Check dimensions. We're also working with ints for sizes so we have
    // to limit the size to signed int range so check for that too.
    if(!_uiTWidth || !_uiTHeight ||
      UtilIntWillOverflow<int>(_uiTWidth) ||
      UtilIntWillOverflow<int>(_uiTHeight))
        XC("Invalid tile dimensions!",
           "Identifier", IdentGet(), "Width", _uiTWidth, "Height", _uiTHeight);
    // Get first image slot and show error as we are not reversing this.
    ImageSlot &bData = iC.GetSlots().front();
    // Check bit depth
    if(iC.GetBitsPerPixel() != 1)
      XC("Image is not monochrome!",
         "Identifier",   IdentGet(),
         "Width",        bData.DimGetWidth(),
         "Height",       bData.DimGetHeight(),
         "BitsPerPixel", iC.GetBitsPerPixel());
    // Check image dimensions too. Again we're dealing with ints!
    if(!bData.DimIsSet() ||
      UtilIntWillOverflow<int>(bData.DimGetWidth()) ||
      UtilIntWillOverflow<int>(bData.DimGetHeight()))
        XC("Invalid image dimensions!",
           "Identifier", IdentGet(), "Width", bData.DimGetWidth(),
           "Height",     bData.DimGetHeight());
    // Image is divisible by 8?
    if(!UtilIsDivisible(bData.DimGetWidth<double>() / 8) ||
       !UtilIsDivisible(bData.DimGetHeight<double>() / 8))
      XC("Image dimensions are not divisible by eight!",
         "Identifier", IdentGet(), "Width", bData.DimGetWidth(),
         "Height",     bData.DimGetHeight());
    // Get reference to the image memory and if no tiling needed? We can just
    // add the full size texture.
    if(bData.DimGetWidth() == _uiTWidth && bData.DimGetHeight() == _uiTHeight)
      { emplace_back(StdMove(bData)); return; }
    // We're dealing with memory now so we need everything as size_t
    const size_t
      // Tile dimensions
      stTWidth = static_cast<size_t>(_uiTWidth),
      stTHeight = static_cast<size_t>(_uiTHeight),
      // Bitmap dimensions
      stWidth = bData.DimGetWidth<size_t>(),
      stHeight = bData.DimGetHeight<size_t>(),
      stHeightM1 = stHeight - 1,
      // Calculate how many tiles we can fit in the image
      stTotalX = stWidth / stTWidth,
      stTotalY = stHeight / stTHeight,
      // Properly clamped tile count value
      stTotalXWhole = stTotalX * stTWidth,
      stTotalYWhole = stTotalY * stTHeight,
      // Calculate size of the bitmask required
      stBytes = stTWidth * stTHeight / CHAR_BIT;
    // Reserve memory for all the tiles
    reserve(stTotalX * stTotalY);
    // Get source buffer
    const unsigned char*const ucpS = bData.Ptr<unsigned char>();
    // If bitmap is reversed?
    if(iC.IsReversed())
    { // Get height and width minus one.
      const size_t stTHeightM1 = stTHeight - 1;
      // Start iterating rows from the bottom
      for(size_t stY = stTotalYWhole - stTHeight;
                 stY < stTotalYWhole;
                 stY -= stTHeight)
      { // Iterate columns from the left
        for(size_t stX = 0; stX < stTotalXWhole; stX += stTWidth)
        { // Create cleared mask buffer, insert it into list and get ptr to
          // the memory.
          unsigned char*const ucpD = emplace(cend(),
            Memory{ stBytes, true })->Ptr<unsigned char>();
          // Copy source to buffer
          for(size_t stTY = stTHeightM1; stTY < stTHeight; --stTY)
            for(size_t stTX = 0; stTX < stTWidth; ++stTX)
              // Note that bits are reversed too.
              UtilBitSet2R(ucpD, ((stTHeightM1 - stTY) * stTWidth) + stTX,
                ucpS, ((stHeightM1 - (stY + stTY)) * stWidth) + (stX + stTX));
        }
      }
    } // Iterate each row from the top
    else for(size_t stY = 0; stY < stTotalYWhole; stY += stTHeight)
    { // Iterate each column from the left
      for(size_t stX = 0; stX < stTotalXWhole; stX += stTWidth)
      { // Create cleared mask buffer, insert it into list and get ptr to
        // the memory.
        unsigned char*const ucpD = emplace(cend(),
          Memory{ stBytes, true })->Ptr<unsigned char>();
        // Copy source to buffer
        for(size_t stTY = 0; stTY < stTHeight; ++stTY)
          for(size_t stTX = 0; stTX < stTWidth; ++stTX)
            // Note that bits are reversed too.
            UtilBitSet2R(ucpD, (stTY * stTWidth) + stTX, ucpS,
              ((stHeightM1 - (stY + stTY)) * stWidth) + (stX + stTX));
      }
    } // Set allocated size
    stAlloc = stWidth * stHeight / CHAR_BIT;
    // The mask passed in the arguments is usually still allocated by LUA and
    // will still be registered, sp lets put a note in the mask to show that
    // this function has nicked the mask.
    iC.IdentSet("!MAS!$!", iC.IdentGet());
    // Tell log what we did
    cLog->LogInfoExSafe("Mask created $ ($x$) tiles from a $x$ $ bitmask.",
      size(), stTWidth, stTHeight, stWidth, stHeight,
      iC.IsReversed() ? "reversed" : "non-reversed");
    // Set new size and tile size
    DimSet(static_cast<int>(stTWidth), static_cast<int>(stTHeight));
  }
  /* -- Get size of all masks ---------------------------------------------- */
  size_t GetAlloc(void) const { return stAlloc; }
  /* -- Constructor -------------------------------------------------------- */
  Mask(void) :
    /* -- Initialisers ----------------------------------------------------- */
    ICHelperMask{ *cMasks, this },     // Register this object in collector
    IdentCSlave{ cParent.CtrNext() },  // Initialise identification number
    stAlloc(0)                         // Uninitialised allocated size
    /* -- No code ---------------------------------------------------------- */
    { }
  /* ----------------------------------------------------------------------- */
  DELETECOPYCTORS(Mask)                // Omit copy constructor for safety
};/* ----------------------------------------------------------------------- */
END_COLLECTOR(Masks)
/* ------------------------------------------------------------------------- */
}                                      // End of public module namespace
/* ------------------------------------------------------------------------- */
}                                      // End of private module namespace
/* == EoF =========================================================== EoF == */

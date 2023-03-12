/* == BIN.HPP ============================================================== */
/* ######################################################################### */
/* ## MS-ENGINE              Copyright (c) MS-Design, All Rights Reserved ## */
/* ######################################################################### */
/* ## This module handles packing of 2D rectangles into a limited         ## */
/* ## rectangular space.                                                  ## */
/* ######################################################################### */
/* ========================================================================= */
#pragma once                           // Only one incursion allowed
/* ------------------------------------------------------------------------- */
namespace IfBin {                      // Start of module namespace
/* -- Includes ------------------------------------------------------------- */
using namespace IfCollector;           // Using collector namespace
using namespace IfDim;                 // Using dimensions namespace
/* ========================================================================= */
template<typename IntType=int,         // Unsigned type not allowed!
         typename Int=typename make_signed<IntType>::type,
         typename UInt=typename make_unsigned<IntType>::type,
         class DimClass=Dimensions<Int>>
class Pack :
  /* -- Initialisers ------------------------------------------------------- */
  public DimClass                      // Dimensions of bin in pixels
{ /* -- Checks ------------------------------------------------------------- */
  static_assert(is_same_v<IntType, Int>, "Invalid type!");
  /* --------------------------------------------------------------- */ public:
  typedef DimCoords<Int> Rect;         // Rectangle of signed ints
  typedef vector<Rect> RectList;       // list of rectangles
  /* -------------------------------------------------------------- */ private:
  RectList         rlUsed, rlFree;     // Used and free data
  /* -- Remove a rect (not that this can cause a realloc) ------------------ */
  void PruneFreeRect(const size_t stIndex)
    { rlFree.erase(next(rlFree.cbegin(), static_cast<ssize_t>(stIndex))); }
  /* -- Add a rect (note this can cause a realloc) ------------------------- */
  void AddFreeRect(const Int iX, const Int iY, const Int iW, const int iH)
    { rlFree.push_back({ iX, iY, iW, iH }); }
  /* ----------------------------------------------------------------------- */
  const Rect FindPositionForNewNodeBestShortSideFit(const Int iW,
    const Int iH, Int &iBestShortSideFit, Int &iBestLongSideFit) const
  { // The best node that will be returned
    iBestShortSideFit = iBestLongSideFit = numeric_limits<Int>::max();
    Rect rFound;
    for(auto &rNode : rlFree)
    { // Try to place the rectangle in upright (non-flipped) orientation.
      if(rNode.DimGetWidth() < iW || rNode.DimGetHeight() < iH) continue;
      const Int iLeftOverHoriz = abs(rNode.DimGetWidth() - iW),
                iLeftOverVert = abs(rNode.DimGetHeight() - iH),
                iShortSideFit = Minimum(iLeftOverHoriz, iLeftOverVert),
                iLongSideFit = Maximum(iLeftOverHoriz, iLeftOverVert);
      if(iShortSideFit < iBestShortSideFit ||
        (iShortSideFit == iBestShortSideFit &&
         iLongSideFit < iBestLongSideFit))
      {
        iBestShortSideFit = iShortSideFit;
        iBestLongSideFit = iLongSideFit;
        rFound.DimCoSet(rNode.CoordGetX(), rNode.CoordGetY(), iW, iH);
      }
    } // Return what we found if we did
    return rFound;
  }
  /* -- Do try to split a free node and ------------------------------------ */
  void SplitFreeNodeUnsafe(const Rect &rFree, const Rect &rUsed,
    const Int iFreeH, const Int iUsedH, const Int iFreeV, const Int iUsedV)
  { // Check horizontal bounds
    if(rUsed.CoordGetX() < iFreeH && iUsedH > rFree.CoordGetX())
    { // New node at the top side of the used node.
      if(rUsed.CoordGetY() > rFree.CoordGetY() && rUsed.CoordGetY() < iFreeV)
        AddFreeRect(rFree.CoordGetX(), rFree.CoordGetY(),
          rFree.DimGetWidth(), rUsed.CoordGetY() - rFree.CoordGetY());
      // New node at the bottom side of the used node.
      if(iUsedV < iFreeV)
        AddFreeRect(rFree.CoordGetX(), iUsedV,
          rFree.DimGetWidth(), iFreeV - iUsedV);
    } // Check vertical bounds
    if(rUsed.CoordGetY() < iFreeV && iUsedV > rFree.CoordGetY())
    { // New node at the left side of the used node.
      if(rUsed.CoordGetX() > rFree.CoordGetX() && rUsed.CoordGetX() < iFreeH)
        AddFreeRect(rFree.CoordGetX(), rFree.CoordGetY(),
          rUsed.CoordGetX() - rFree.CoordGetX(), rFree.DimGetHeight());
      // New node at the right side of the used node.
      if(iUsedH < iFreeH)
        AddFreeRect(iUsedH, rFree.CoordGetY(),
          iFreeH - iUsedH, rFree.DimGetHeight());
    }
  }
  /* -- Try to split a free node and return true if succeeded -------------- */
  bool SplitFreeNode(const size_t stIndex, const Rect &rUsed)
  { // Get reference to free node
    const Rect &rFree = rlFree[stIndex];
     // Calculate maximum bounds of the free and used rslRects.
    const Int iFreeH = rFree.CoordGetX() + rFree.DimGetWidth(),
              iUsedH = rUsed.CoordGetX() + rUsed.DimGetWidth(),
              iFreeV = rFree.CoordGetY() + rFree.DimGetHeight(),
              iUsedV = rUsed.CoordGetY() + rUsed.DimGetHeight();
    // Test with SAT if the rectangles even intersect.
    if(rUsed.CoordGetX() >= iFreeH || iUsedH <= rFree.CoordGetX() ||
       rUsed.CoordGetY() >= iFreeV || iUsedV <= rFree.CoordGetY())
      return false;
    // DANGER! There must be at least four reserved vector nodes or a
    // push_back() could trigger a realloc() being a std::vector and &rFree
    // would in succession point to freed memory and all havoc breaks lose in
    // the engine! So if there is not enough room for four more push_back()'s?
    if(rlFree.capacity() - rlFree.size() < 4)
    { // Not enough room so try to reserve up to four more items. Reserve()
      // will not grow exponentially so we should handle that instead.
      rlFree.reserve(NearestPow2<size_t>(rlFree.size() + 4));
      // Do the split with a NEW evaluation of rlFree[stIndex] as again, the
      // address of rlFree[stIndex] may have changed!
      SplitFreeNodeUnsafe(rlFree[stIndex], rUsed,
        iFreeH, iUsedH, iFreeV, iUsedV);
    } // Enough room so we can just continue with the existing reference.
    else SplitFreeNodeUnsafe(rFree, rUsed, iFreeH, iUsedH, iFreeV, iUsedV);
    // Success!
    return true;
  }
  /* ----------------------------------------------------------------------- */
  bool IsContainedIn(const Rect &rSrc, const Rect &rDest) const
  { // Return if the src rect bounds are contained in dest bounds
    return rSrc.CoordGetX() >= rDest.CoordGetX()
        && rSrc.CoordGetY() >= rDest.CoordGetY()
        && rSrc.CoordGetX() + rSrc.DimGetWidth()
        <= rDest.CoordGetX() + rDest.DimGetWidth()
        && rSrc.CoordGetY() + rSrc.DimGetHeight()
        <= rDest.CoordGetY() + rDest.DimGetHeight();
  }
  /* -- Goes through free rectangle list and removes any redundan ---------- */
  void PruneFreeList(void)
  { // Go through each free rectangle
    for(size_t stIndex = 0; stIndex < rlFree.size(); ++stIndex)
    { // Compare with all other rectangles
      for(size_t stSubIndex = stIndex + 1;
                 stSubIndex < rlFree.size();
               ++stSubIndex)
      { // If rectangle sub-rect is inside this rect
        if(IsContainedIn(rlFree[stIndex], rlFree[stSubIndex]))
        { // Remove it
          PruneFreeRect(stIndex);
          // Decrement index
          --stIndex;
          // We're done with the sub-rect
          break;
        } // Ignore if subrect is inside this rect
        if(!IsContainedIn(rlFree[stSubIndex], rlFree[stIndex])) continue;
        // Remove sub-rect
        PruneFreeRect(stSubIndex);
        // Decrement index
        --stSubIndex;
      }
    }
  }
  /* -- Swap --------------------------------------------------------------- */
  void BinSwap(Pack &pOther)
  { // Swap dimensions
    DimSwap(pOther);
    // Swap used and free lists
    rlUsed.swap(pOther.rlUsed);
    rlFree.swap(pOther.rlFree);
  }
  /* -- (Re)initialise bin to empty state -------------------------- */ public:
  void Init(const UInt iNWidth, const UInt iNHeight)
  { // Init iW and iH
    this->DimSet(static_cast<Int>(iNWidth), static_cast<Int>(iNHeight));
    // Clear current usage
    rlUsed.clear();
    rlFree.clear();
    // Full bounds initially available
    AddFreeRect(0, 0, this->DimGetWidth(), this->DimGetHeight());
  }
  /* -- Reserve memory ----------------------------------------------------- */
  void Reserve(const size_t stUsedReserve, const size_t stFreeReserve)
    { rlUsed.reserve(stUsedReserve); rlFree.reserve(stFreeReserve); }
  /* -- (Re)initialise bin to empty state ---------------------------------- */
  void Init(const UInt uiNWidth, const UInt uiNHeight,
     const size_t stUsedReserve, const size_t stFreeReserve)
  { // Initialise rect
    Init(uiNWidth, uiNHeight);
    // Reserve memory
    Reserve(stUsedReserve, stFreeReserve);
  }
  /* -- Englarge the bin --------------------------------------------------- */
  bool Resize(const UInt uiNWidth, const UInt uiNHeight)
  { // If there is nothing in the bin?
    if(rlUsed.empty())
    { // Just reinitialise it
      Init(uiNWidth, uiNHeight);
      // Success
      return true;
    } // Convert values to int
    const int iNHeight = static_cast<Int>(uiNHeight);
    const int iNWidth = static_cast<Int>(uiNWidth);
    // New width same as old?
    if(iNWidth <= this->DimGetWidth())
    { // Return if new height same as old
      if(iNHeight <= this->DimGetHeight()) return false;
      // New height specified so add new free rect and update it
      UpdateHeight:
      AddFreeRect(0, this->DimGetHeight(), iNWidth,
        iNHeight - this->DimGetHeight());
      this->DimSetHeight(iNHeight);
    } // New width specified?
    else
    { // New width specified so add new free rect and update it
      AddFreeRect(this->DimGetWidth(), 0,
        iNWidth - this->DimGetWidth(), this->DimGetHeight());
      this->DimSetWidth(iNWidth);
      // New height specified? Update it
      if(iNHeight > this->DimGetHeight()) goto UpdateHeight;
    } // Englarge complete
    return true;
  }
  /* ----------------------------------------------------------------------- */
  // Test insert a single rectangle into the bin, possibly rotated.
  const Rect Test(const UInt iW, const UInt iH)
  { Int iScore1 = numeric_limits<Int>::max(), iScore2 = iScore1;
    return FindPositionForNewNodeBestShortSideFit(static_cast<Int>(iW),
      static_cast<Int>(iH), iScore1, iScore2);
  }
  /* ----------------------------------------------------------------------- */
  // Inserts a single rectangle into the bin, possibly rotated.
  const Rect Insert(const UInt uiW, const UInt uiH)
  { // Unused in this function. We don't need to know the score after finding
    // the position.
    Int iScore1 = numeric_limits<Int>::max(), iScore2 = iScore1;
    const Rect rNew{ FindPositionForNewNodeBestShortSideFit(
      static_cast<Int>(uiW), static_cast<Int>(uiH), iScore1, iScore2) };
    if(rNew.DimGetHeight() == 0) return rNew;
    size_t stRectanglesToProcess = rlFree.size();
    for(size_t stIndex = 0; stIndex < stRectanglesToProcess; ++stIndex)
    { if(!SplitFreeNode(stIndex, rNew)) continue;
      PruneFreeRect(stIndex);
      --stIndex;
      --stRectanglesToProcess;
    }
    PruneFreeList();
    rlUsed.emplace_back(rNew);
    return rNew;
  }
  /* ----------------------------------------------------------------------- */
  double Occupancy(void) const
  { // Return the ratio of used surface area to the total bin area.
    return accumulate(rlUsed.cbegin(), rlUsed.cend(), 0.0,
      [](double dUsed, const Rect &rNode)
        { return dUsed += rNode.template DimGetWidth<double>() *
                          rNode.DimGetHeight(); }) /
      (this->template DimGetWidth<double>() * this->DimGetHeight());
  }
  /* ----------------------------------------------------------------------- */
  size_t Used(void) const { return rlUsed.size(); }
  size_t Free(void) const { return rlFree.size(); }
  size_t Total(void) const { return rlFree.size() + rlUsed.size(); }
  /* -- Default constructor that instantiates an empty bin of size 0x0 ----- */
  Pack(void)
    /* -- Do nothing ------------------------------------------------------- */
    { }
  /* -- Instantiates a bin of the given size with pre-reserved memory ------ */
  Pack(const UInt   uiNWidth,          // Set width of new pack
       const UInt   uiNHeight,         // Set height of new pack
       const size_t stUsedReserve,     // Used list initial size
       const size_t stFreeReserve) :   // Free list initial size
    /* -- Initialisation of members ---------------------------------------- */
    Pack{ uiNWidth, uiNHeight }        // Initialise pack
    /* -- Reserve list sizes ----------------------------------------------- */
    { Reserve(stUsedReserve, stFreeReserve); }
  /* -- Instantiates a bin of the given size ------------------------------- */
  Pack(const UInt uiNWidth,            // Width of new pack
       const UInt uiNHeight) :         // Height of new pack
    /* -- Initialisation of members ---------------------------------------- */
    DimClass{ static_cast<Int>(uiNWidth),    // Set width of new pack
              static_cast<Int>(uiNHeight) }, // Set height of new pack
    rlFree{{ 0, 0,                     // Initialise co-ordinates
      this->DimGetWidth(),             // Initialise specified width
      this->DimGetHeight() }}          // Initialise specified height
    /* -- Do nothing ------------------------------------------------------- */
    { }
};/* ----------------------------------------------------------------------- */
/* == Bin object collector and member class ================================ */
BEGIN_COLLECTORDUO(Bins, Bin, CLHelperUnsafe, ICHelperUnsafe),
  /* -- Base classes ------------------------------------------------------- */
  public Lockable,                     // Lua garbage collector instruction
  public Pack<GLint>                   // Base Pack class with GLint's
{ /* -- Default constructor ---------------------------------------- */ public:
  Bin(void) :
    /* -- Initialisation of members ---------------------------------------- */
    ICHelperBin{ *cBins, this },       // Register the object in collector
    IdentCSlave{ cParent.CtrNext() }   // Initialise identification number
    /* --------------------------------------------------------------------- */
    { }
};/* ----------------------------------------------------------------------- */
END_COLLECTOR(Bins);                   // End of bin objects collector
/* ------------------------------------------------------------------------- */
};                                     // End of module namespace
/* == EoF =========================================================== EoF == */

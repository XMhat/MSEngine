/* == DIM.HPP ============================================================== */
/* ######################################################################### */
/* ## MS-ENGINE              Copyright (c) MS-Design, All Rights Reserved ## */
/* ######################################################################### */
/* ## Sets up some basic dimension and coordinate classes to use          ## */
/* ## throughout the engine.                                              ## */
/* ######################################################################### */
/* ========================================================================= */
#pragma once                           // Only one incursion allowed
/* ------------------------------------------------------------------------- */
namespace IfDim {                      // Start of module namespace
/* ------------------------------------------------------------------------- */
template<typename Int = unsigned int,  // Integer type to use
         class Pair = pair<Int, Int>>  // Pair of specified types
class IntPair :                        // Members initially private
  /* -- Base classes ------------------------------------------------------- */
  private Pair                         // Pair of values
{ /* -- Private variables --------------------------------------- */ protected:
  constexpr static const Int i0 = static_cast<Int>(0), // 0 of specified type
  /* Some needed values */   i1 = static_cast<Int>(1); // 1 of specified type
  /* -- Get ---------------------------------------------------------------- */
  template<typename RInt=Int>RInt IPGetOne(void) const
    { return static_cast<RInt>(this->first); }
  template<typename RInt=Int>RInt IPGetTwo(void) const
    { return static_cast<RInt>(this->second); }
  template<typename RInt=Int>RInt IPDefGet(void) const
    { return static_cast<RInt>(i0); }
  /* -- Set ---------------------------------------------------------------- */
  void IPSetOne(const Int iV) { this->first = iV; }
  void IPSetTwo(const Int iV) { this->second = iV; }
  void IPSet(const Int iV1, const Int iV2) { IPSetOne(iV1); IPSetTwo(iV2); }
  void IPSet(const Int iV = i0) { IPSet(iV, iV); }
  void IPSet(const IntPair &ipO) { IPSet(ipO.IPGetOne(), ipO.IPGetTwo()); }
  void IPSwap(IntPair &ipO) { this->swap(ipO); }
  /* -- Increment ---------------------------------------------------------- */
  void IPIncOne(const Int iV = i1) { IPSetOne(IPGetOne() + iV); }
  void IPIncTwo(const Int iV = i1) { IPSetTwo(IPGetTwo() + iV); }
  /* -- Decrement ---------------------------------------------------------- */
  void IPDecOne(const Int iV = i1) { IPSetOne(IPGetOne() - iV); }
  void IPDecTwo(const Int iV = i1) { IPSetTwo(IPGetTwo() - iV); }
  /* -- Test --------------------------------------------------------------- */
  bool IPIsOneSet(void) const { return IPGetOne() != IPDefGet(); }
  bool IPIsNotOneSet(void) const { return !IPIsOneSet(); }
  bool IPIsTwoSet(void) const { return IPGetTwo() != IPDefGet(); }
  bool IPIsNotTwoSet(void) const { return !IPIsTwoSet(); }
  bool IPIsSet(void) const { return IPIsOneSet() && IPIsTwoSet(); }
  bool IPIsNotSet(void) const { return !IPIsSet(); }
  /* -- Copy assignment ---------------------------------------------------- */
  IntPair& operator=(const IntPair &ipO) { IPSet(ipO); return *this; }
  /* -- Copy constructor --------------------------------------------------- */
  IntPair(const IntPair &ipO) :        // Other class
    /* -- Initialisers ----------------------------------------------------- */
    Pair{ ipO }                        // Copy pair
    /* -- No code ---------------------------------------------------------- */
    { }
  /* -- Initialisation constructor ----------------------------------------- */
  IntPair(const Int iV1,               // First value
          const Int iV2) :             // Second value
    /* -- Initialisers ----------------------------------------------------- */
    Pair{ iV1, iV2 }                   // Initialise specified pair
    /* -- No code ---------------------------------------------------------- */
    { }
  /* -- Default constructor ------------------------------------------------ */
  IntPair(void) :                      // No parameters
    /* -- Initialisers ----------------------------------------------------- */
    IntPair{ i0, i0 }                  // Initialise default pair
    /* -- No code ---------------------------------------------------------- */
    { }
};/* ----------------------------------------------------------------------- */
template<typename Int = unsigned int,  // Integer type to use
         class Base = IntPair<Int>>    // Base class type to use
struct Dimensions :                    // Members initially public
  /* -- Base classes ------------------------------------------------------- */
  private Base                         // Base double value class
{ /* -- Get ---------------------------------------------------------------- */
  template<typename RInt=Int>RInt DimGetWidth(void) const
    { return this->template IPGetOne<RInt>(); }
  template<typename RInt=Int>RInt DimGetHeight(void) const
    { return this->template IPGetTwo<RInt>(); }
  template<typename RInt=Int>RInt DimDefGet(void) const
    { return this->template IPDefGet<RInt>(); }
  /* -- Set ---------------------------------------------------------------- */
  void DimSetWidth(const Int iV) { this->IPSetOne(iV); }
  void DimSetHeight(const Int iV) { this->IPSetTwo(iV); }
  void DimSet(const Int iW, const Int iH) { this->IPSet(iW, iH); }
  void DimSet(const Int iV = Base::i0) { this->IPSet(iV); }
  void DimSet(const Dimensions &dO) { this->IPSet(dO); }
  void DimSwap(Dimensions &dO) { this->IPSwap(dO); }
  /* -- Increment ---------------------------------------------------------- */
  void DimIncWidth(const Int iV = Base::i1) { this->IPIncOne(iV); }
  void DimIncHeight(const Int iV = Base::i1) { this->IPIncTwo(iV); }
  /* -- Decrement ---------------------------------------------------------- */
  void DimDecWidth(const Int iV = Base::i1) { this->IPDecOne(iV); }
  void DimDecHeight(const Int iV = Base::i1) { this->IPDecTwo(iV); }
  /* -- Test --------------------------------------------------------------- */
  bool DimIsWidthSet(void) const { return this->IPIsOneSet(); }
  bool DimIsNotWidthSet(void) const { return this->IPIsNotOneSet(); }
  bool DimIsHeightSet(void) const { return this->IPIsTwoSet(); }
  bool DimIsNotHeightSet(void)const { return this->IPIsNotTwoSet(); }
  bool DimIsSet(void) const { return this->IPIsSet(); }
  bool DimIsNotSet(void) const { return this->IPIsNotSet(); }
  /* -- Initialisation constructor ----------------------------------------- */
  Dimensions(const Int itW,            // Specified width to initialise to
             const Int itH) :          // Specified height to initialise to
    /* -- Initialisers ----------------------------------------------------- */
    Base{ itW, itH }                   // Initialise specified values
    /* -- No code ---------------------------------------------------------- */
    { }
  /* -- Default constructor that does not need to do anything -------------- */
  Dimensions(void)                     // No parameters
    /* -- No code ---------------------------------------------------------- */
    { }
};/* ----------------------------------------------------------------------- */
template<typename Int = unsigned int,  // Integer type to use
         class Base = IntPair<Int>>    // Base int pair class type to use
struct Coordinates :                   // Members initially public
  /* -- Base classes ------------------------------------------------------- */
  private Base                         // Base double value class
{ /* -- Get ---------------------------------------------------------------- */
  template<typename RInt=Int>RInt CoordGetX(void) const
    { return this->template IPGetOne<RInt>(); }
  template<typename RInt=Int>RInt CoordGetY(void) const
    { return this->template IPGetTwo<RInt>(); }
  template<typename RInt=Int>RInt CoordDefGet(void) const
    { return this->template IPDefGet<RInt>(); }
  /* -- Set ---------------------------------------------------------------- */
  void CoordSetX(const Int iV) { this->IPSetOne(iV); }
  void CoordSetY(const Int iV) { this->IPSetTwo(iV); }
  void CoordSet(const Int itX, const Int itY) { this->IPSet(itX,itY); }
  void CoordSet(const Int iV = Base::i0) { this->IPSet(iV); }
  void CoordSet(const Coordinates &cO) { this->IPSet(cO); }
  void CoordSwap(Coordinates &cO) { this->IPSwap(cO); }
  /* -- Increment ---------------------------------------------------------- */
  void CoordIncX(const Int iV = Base::i1) { this->IPIncOne(iV); }
  void CoordIncY(const Int iV = Base::i1) { this->IPIncTwo(iV); }
  /* -- Decrement ---------------------------------------------------------- */
  void CoordDecX(const Int iV = Base::i1) { this->IPDecOne(iV); }
  void CoordDecY(const Int iV = Base::i1) { this->IPDecTwo(iV); }
  /* -- Test --------------------------------------------------------------- */
  bool CoordIsXSet(void) const { return this->IPIsOneSet(); }
  bool CoordIsNotXSet(void) const { return this->IPIsNotOneSet(); }
  bool CoordIsYSet(void) const { return this->IPIsTwoSet(); }
  bool CoordIsNotYSet(void) const { return this->IPIsNotTwoSet(); }
  bool CoordIsSet(void) const { return this->IPIsSet(); }
  bool CoordIsNotSet(void) const { return this->IPIsNotSet(); }
  /* -- Test operator ------------------------------------------------------ */
  operator bool(void) const { return CoordIsSet(); }
  /* -- Initialisation constructor ----------------------------------------- */
  Coordinates(const Int itX, const Int itY) :
    /* -- Initialisers ----------------------------------------------------- */
    Base{ itX, itY }                   // Initialise specified values
    /* -- No code ---------------------------------------------------------- */
    { }
  /* -- Default constructor that does not need to do anything -------------- */
  Coordinates(void)                    // No parameters
    /* -- No code ---------------------------------------------------------- */
    { }
};/* ----------------------------------------------------------------------- */
template<typename DInt = unsigned int, // Dimensions integer type
         typename CInt = DInt,         // Coords class integer type
         class DClass  = Dimensions<DInt>,  // Dimensions class to use
         class CClass  = Coordinates<CInt>> // Coords class to use
struct DimCoords :                     // Members initially public
  /* -- Base classes ------------------------------------------------------- */
  public CClass,                       // Co-ordinates class
  public DClass                        // Dimensions class
{ /* -- Set co-ordinates and dimensions ------------------------------------ */
  void DimCoSet(const CInt ciX, const CInt ciY, const DInt diW, const DInt diH)
    { this->CoordSet(ciX, ciY); this->DimSet(diW, diH); }
  /* -- Constructor to fill all parameters --------------------------------- */
  DimCoords(const CInt ciX, const CInt ciY, const DInt diW, const DInt diH) :
    /* -- Initialisers ----------------------------------------------------- */
    CClass{ ciX, ciY },                // Initialise co-ordinates
    DClass{ diW, diH }                 // Initialise dimensions
    /* -- No code ---------------------------------------------------------- */
    { }
  /* -- Default constructor that zero initialises everything --------------- */
  DimCoords(void)                      // No parameters
    /* -- No code ---------------------------------------------------------- */
    { }
};/* ----------------------------------------------------------------------- */
template<typename Int = unsigned int,      // The integer type to use
         class Class = Coordinates<Int>,   // Coordinates class to use
         class CPair = pair<Class, Class>> // A pair of co-ordinate classes
class Rectangle :                      // Members initially private
  /* -- Base classes ------------------------------------------------------- */
  private CPair                        // Two co-ordinates make a rectangle
{ /* -- Set -------------------------------------------------------- */ public:
  void RectSetX1(const Int iV = CPair::i1) { this->first.CoordSetX(iV); }
  void RectSetY1(const Int iV = CPair::i1) { this->first.CoordSetY(iV); }
  void RectSetX2(const Int iV = CPair::i1) { this->second.CoordSetX(iV); }
  void RectSetY2(const Int iV = CPair::i1) { this->second.CoordSetY(iV); }
  /* -- Increment ---------------------------------------------------------- */
  void RectIncX1(const Int iV = CPair::i1) { this->first.CoordIncX(iV); }
  void RectIncY1(const Int iV = CPair::i1) { this->first.CoordIncY(iV); }
  void RectIncX2(const Int iV = CPair::i1) { this->second.CoordIncX(iV); }
  void RectIncY2(const Int iV = CPair::i1) { this->second.CoordIncY(iV); }
  /* -- Decrement ---------------------------------------------------------- */
  void RectDecX1(const Int iV = CPair::i1) { this->first.CoordDecX(iV); }
  void RectDecY1(const Int iV = CPair::i1) { this->first.CoordDecY(iV); }
  void RectDecX2(const Int iV = CPair::i1) { this->second.CoordDecX(iV); }
  void RectDecY2(const Int iV = CPair::i1) { this->second.CoordDecY(iV); }
  /* -- Set co-ordinates --------------------------------------------------- */
  void RectSetTopLeft(const Int itX, const Int itY)
    { this->first.CoordSet(itX, itY); }
  void RectSetTopLeft(const Int iV) { RectSetTopLeft(iV, iV); }
  void RectSetBottomRight(const Int itX, const Int itY)
    { this->second.CoordSet(itX, itY); }
  void RectSetBottomRight(const Int iV) { RectSetBottomRight(iV, iV); }
  /* -- Set more ----------------------------------------------------------- */
  void RectSet(const Int itX1, const Int itY1, const Int itX2, const Int itY2)
    { RectSetTopLeft(itX1, itY1); RectSetBottomRight(itX2, itY2); }
  /* -- Get ---------------------------------------------------------------- */
  template<typename RInt=Int>RInt RectGetX1(void) const
    { return this->first.template CoordGetX<RInt>(); }
  template<typename RInt=Int>RInt RectGetY1(void) const
    { return this->first.template CoordGetY<RInt>(); }
  template<typename RInt=Int>RInt RectGetX2(void) const
    { return this->second.template CoordGetX<RInt>(); }
  template<typename RInt=Int>RInt RectGetY2(void) const
    { return this->second.template CoordGetY<RInt>(); }
  /* -- Constructor to fill all parameters --------------------------------- */
  Rectangle(const Int itX1,            // First co-ordinate X position
            const Int itY1,            // First co-ordinate Y position
            const Int itX2,            // Second co-ordinate X position
            const Int itY2) :          // Second co-ordinate Y position
    /* -- Initialisers ----------------------------------------------------- */
    CPair{ { itX1, itY1 },             // Initialise first co-ordinates
           { itX2, itY2 } }            // Initialise second co-ordinates
    /* -- No code ---------------------------------------------------------- */
    { }
  /* -- Default constructor ------------------------------------------------ */
  Rectangle(void)                      // No parameters
    /* -- No code ---------------------------------------------------------- */
    { }
};/* ----------------------------------------------------------------------- */
};                                     // End of module namespace
/* == EoF =========================================================== EoF == */

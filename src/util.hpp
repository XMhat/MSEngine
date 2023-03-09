/* == UTIL.HPP ============================================================= */
/* ######################################################################### */
/* ## MS-ENGINE              Copyright (c) MS-Design, All Rights Reserved ## */
/* ######################################################################### */
/* ## Miscellaneous utility classes and functions too small or            ## */
/* ## insignificant (i.e. not a core engine sub-system) to be worth       ## */
/* ## putting into it's own file.                                         ## */
/* ######################################################################### */
/* ========================================================================= */
#pragma once                           // Only one incursion allowed
/* -- Module namespace ----------------------------------------------------- */
namespace IfUtil {                     // Keep declarations neatly categorised
/* -- Includes ------------------------------------------------------------- */
using namespace IfString;              // Using string interface
/* -- Check that uint32 and float are the same size ------------------------ */
static_assert(sizeof(float) == sizeof(uint32_t) &&
              sizeof(float) == sizeof(int32_t),
  "Size of 'float' and '(u)int32_t' are not equal!");
/* -- Check that uint64 and double are the same size ----------------------- */
static_assert(sizeof(double) == sizeof(uint64_t) &&
              sizeof(double) == sizeof(int64_t),
  "Size of 'double' and '(u)int64_t' are not equal!");
/* -- Manual memory allocation --------------------------------------------- */
template<typename AnyType,typename IntType>
  static AnyType *MemAlloc(const IntType itBytes)
    { return reinterpret_cast<AnyType*>
        (malloc(static_cast<size_t>(itBytes))); }
template<typename AnyType,typename IntType>
  static AnyType *MemReAlloc(AnyType*const atPtr, const IntType itBytes)
    { return reinterpret_cast<AnyType*>
        (realloc(reinterpret_cast<void*>(atPtr),
                 static_cast<size_t>(itBytes))); }
template<typename AnyType>
  static void MemFree(AnyType*const atPtr)
    { free(reinterpret_cast<void*>(atPtr)); }
/* == Number is divisible by specified number ============================== */
static bool IsDivisible(const double dNumber)
  { double dDummy; return modf(dNumber, &dDummy) == 0; }
// template<typename FloatType=double>
//  static bool IsNormal(const FloatType ftValue)
//    { return fpclassify(iitValue) == FP_NORMAL; }
/* ------------------------------------------------------------------------- */
template<typename IntType=double, typename FloatType=double>
  static IntType Round(const FloatType ftValue, const int iPrecision)
{ // Classify input file
  if(ftValue == 0.0) return IntType(0);
  // Calculate adjustment
  const FloatType ftAmount = pow(10.0, iPrecision);
  // Do rounding
  return static_cast<IntType>(floor(ftValue * ftAmount + 0.5) / ftAmount);
}
/* -- Swap and clear a list ------------------------------------------------ */
template<class ListType>
  static void SwapListsAndClear(ListType &lListOne, ListType &lListTwo)
    { lListOne.swap(lListTwo); lListOne.clear(); }
/* -- Expand dimensions to specified outer bounds keeping aspect ----------- */
static void StretchToOuterBounds(double &fdOW, double &fdOH, double &fdIW,
  double &fdIH)
{ // Get aspect ratio of inner and outer dimensions
  const double fdOuterAspect = fdOW / fdOH, fdInnerAspect = fdIW / fdIH;
  // If the aspect ratios are the same then the screen rectangle will do,
  // otherwise we need to calculate the new rectangle
  if(fdInnerAspect > fdOuterAspect)
  { // Calculate new width and centring factor
    const double fdNewWidth = fdOH / fdIH * fdIW,
                 fdCentringFactor = (fdOW - fdNewWidth) / 2;
    // Set new bounds. We'll use the original vars to set the values
    fdIW = fdNewWidth + fdCentringFactor;
    fdIH = fdOH;
    fdOW = fdCentringFactor;
    fdOH = 0;
  } // Otherwise
  else if(fdInnerAspect < fdOuterAspect)
  { // Calculate new width and centring factor
    const double fdNewHeight = fdOW / fdIW * fdIH,
                 fdCentringFactor = (fdOH - fdNewHeight) / 2;
    // Set new bounds. We'll use the original vars to set the values
    fdIH = fdNewHeight + fdCentringFactor;
    fdIW = fdOW;
    fdOW = 0;
    fdOH = fdCentringFactor;
  } // No change?
  else
  { // Keep original values
    fdIW = fdOW;
    fdIH = fdOH;
    fdOW = fdOH = 0;
  }
}
/* -- Expand dimensions to specified inner bounds keeping aspect ----------- */
static void StretchToInnerBounds(double &fdOW, double &fdOH, double &fdIW,
  double &fdIH)
{ // Get aspect ratio of inner and outer dimensions
  const double fdOuterAspect = fdOW / fdOH, fdInnerAspect = fdIW / fdIH;
  // If the aspect ratios are the same then the screen rectangle will do,
  // otherwise we need to calculate the new rectangle
  if(fdInnerAspect > fdOuterAspect)
  { // Calculate new width and centring factor
    const double fdNewHeight = fdOW / fdIW * fdIH,
                 fdCentringFactor = (fdOH - fdNewHeight) / 2;
    // Set new bounds. We'll use the original vars to set the values
    fdIH = fdNewHeight + fdCentringFactor;
    fdIW = fdOW;
    fdOW = 0;
    fdOH = fdCentringFactor;
  } // Otherwise
  else if(fdInnerAspect < fdOuterAspect)
  { // Calculate new width and centring factor
    const double fdNewWidth = fdOH / fdIH * fdIW,
                 fdCentringFactor = (fdOW - fdNewWidth) / 2;
    // Set new bounds. We'll use the original vars to set the values
    fdIW = fdNewWidth + fdCentringFactor;
    fdIH = 0;
    fdOW = fdCentringFactor;
  } // No change?
  else
  { // Keep original values
    fdIW = fdOW;
    fdIH = fdOH;
    fdOW = fdOH = 0;
  }
}
/* -- Try to reserve items in a list -------------------------------------- */
template<class List>static bool ReserveList(List &lList, const size_t stCount)
{ // Return if specified value is outrageous!
  if(stCount > lList.max_size()) return false;
  // Reserve room for this many flaots
  lList.reserve(stCount);
  // Success
  return true;
}
/* -- Reverse a byte ------------------------------------------------------- */
static uint8_t ReverseByte(const uint8_t ucByte)
{ // We shall use a lookup table for this as it is faster
  static const array<const uint8_t,16> ucaLookup{
    0x0, 0x8, 0x4, 0xc, 0x2, 0xa, 0x6, 0xe,   // Index 1 == 0b0001 => 0b1000
    0x1, 0x9, 0x5, 0xd, 0x3, 0xb, 0x7, 0xf }; // Index 7 == 0b0111 => 0b1110
  // Reverse the top and bottom nibble then swap them.
  return static_cast<uint8_t>((ucaLookup[ucByte & 0b1111] << 4) |
    ucaLookup[ucByte >> 4]);
}
/* -- Lock a mutex and copy/move a variable -------------------------------- */
template<typename ValDst,typename ValSrc=ValDst>
  static void CopyVarSafe(mutex &mMutex, ValDst &vdDst, const ValSrc &vsSrc)
    { const LockGuard lgSync{ mMutex }; vdDst = vsSrc; }
template<typename ValDst,typename ValSrc=ValDst>
  static void MoveVarSafe(mutex &mMutex, ValDst &vdDst, ValSrc &vsSrc)
    { const LockGuard lgSync{ mMutex }; vdDst = move(vsSrc); }
/* -- Helper functions to force integer byte ordering ---------------------- */
template<typename IntType>static IntType ToI16LE(const IntType itV)
  { static_assert(sizeof(IntType) == sizeof(uint16_t) &&
      is_integral_v<IntType>, "Not a 16-bit integer!");
    return static_cast<IntType>(STRICT_U16LE(itV)); }
template<typename IntType>static IntType ToI16BE(const IntType itV)
  { static_assert(sizeof(IntType) == sizeof(uint16_t) &&
      is_integral_v<IntType>, "Not a 16-bit integer!");
    return static_cast<IntType>(STRICT_U16BE(itV)); }
template<typename IntType>static IntType ToI32LE(const IntType itV)
  { static_assert(sizeof(IntType) == sizeof(uint32_t) &&
      is_integral_v<IntType>, "Not a 32-bit integer!");
    return static_cast<IntType>(STRICT_U32LE(itV)); }
template<typename IntType>static IntType ToI32BE(const IntType itV)
 { static_assert(sizeof(IntType) == sizeof(uint32_t) &&
     is_integral_v<IntType>, "Not a 32-bit integer!");
   return static_cast<IntType>(STRICT_U32BE(itV)); }
template<typename IntType>static IntType ToI64LE(const IntType itV)
  { static_assert(sizeof(IntType) == sizeof(uint64_t) &&
      is_integral_v<IntType>, "Not a 64-bit integer!");
    return static_cast<IntType>(STRICT_U64LE(itV)); }
template<typename IntType>static IntType ToI64BE(const IntType itV)
  { static_assert(sizeof(IntType) == sizeof(uint64_t) &&
      is_integral_v<IntType>, "Not a 64-bit integer!");
    return static_cast<IntType>(STRICT_U64BE(itV)); }
/* -- Convert bit count to bitmask ----------------------------------------- */
template<typename IntType>static IntType BitsToMask(size_t stCount)
{ // Initial value
  IntType itValue = 0;
  // Build mask for bit count
  while(--stCount != numeric_limits<size_t>::max()) itValue |= 1 << stCount;
  // Return generated value
  return itValue;
}
/* -- Swap class functors -------------------------------------------------- */
struct Swap32LEFunctor { uint32_t v; explicit Swap32LEFunctor(uint32_t dwV) :
  v(ToI32LE(dwV)) { } };
struct Swap32BEFunctor { uint32_t v; explicit Swap32BEFunctor(uint32_t dwV) :
  v(ToI32BE(dwV)) { } };
struct Swap64LEFunctor { uint64_t v; explicit Swap64LEFunctor(uint64_t qwV) :
  v(ToI64LE(qwV)) { } };
struct Swap64BEFunctor { uint64_t v; explicit Swap64BEFunctor(uint64_t qwV) :
  v(ToI64BE(qwV)) { } };
/* -- Convert const object to non-const ------------------------------------ */
template<typename Type>static Type &ToNonConst(const Type &tSrc)
  { return const_cast<Type&>(tSrc); }
/* ------------------------------------------------------------------------- */
template<typename TypeTo, typename TypeFrom>
  static TypeTo &BruteCast(TypeFrom &tfV)
    { return *reinterpret_cast<TypeTo*>(&tfV); }
/* ------------------------------------------------------------------------- */
static const uint32_t &CastFloatToInt32(const float &fV)
  { return BruteCast<const uint32_t>(fV); }
static const float &CastInt32ToFloat(const uint32_t &ulV)
  { return BruteCast<const float>(ulV); }
/* ------------------------------------------------------------------------- */
static const double &CastInt64ToDouble(const uint64_t &ullV)
  { return BruteCast<const double>(ullV); }
static const uint64_t &CastDoubleToInt64(const double &dV)
  { return BruteCast<const uint64_t>(dV); }
/* -- Helper functions to force float byte ordering ------------------------ */
/* -- THESE FUNCTIONS ARE OK! THEY ARE FOR BYTE ORDERING CONVERSIONS! ------ */
static float ToF32LE(const float fV)
  { return CastInt32ToFloat(ToI32LE(CastFloatToInt32(fV))); }
static float ToF32BE(const float fV)
  { return CastInt32ToFloat(ToI32BE(CastFloatToInt32(fV))); }
static double ToF64LE(const double dV)
  { return CastInt64ToDouble(ToI64LE(CastDoubleToInt64(dV))); }
static double ToF64BE(const double dV)
  { return CastInt64ToDouble(ToI64BE(CastDoubleToInt64(dV))); }
/* -- Check an enum's validity --------------------------------------------- */
template<typename EnumType>
  static EnumType TestEnumRange(const EnumType etValue,
    const EnumType etMax, const EnumType etMin=static_cast<EnumType>(0))
      { return etValue >= etMin && etValue < etMax ? etValue : etMax; }
/* -- More helper functions for byte ordering ------------------------------ */
static int16_t ToLittleEndian(const int16_t wV) { return ToI16LE(wV); }
static int16_t ToBigEndian(const int16_t wV) { return ToI16BE(wV); }
static uint16_t ToLittleEndian(const uint16_t wV) { return ToI16LE(wV); }
static uint16_t ToBigEndian(const uint16_t wV) { return ToI16BE(wV); }
static int32_t ToLittleEndian(const int32_t dwV) { return ToI32LE(dwV); }
static int32_t ToBigEndian(const int32_t dwV) { return ToI32BE(dwV); }
static uint32_t ToLittleEndian(const uint32_t dwV) { return ToI32LE(dwV); }
static uint32_t ToBigEndian(const uint32_t dwV) { return ToI32BE(dwV); }
static int64_t ToLittleEndian(const int64_t qwV) { return ToI64LE(qwV); }
static int64_t ToBigEndian(const int64_t qwV) { return ToI64BE(qwV); }
static uint64_t ToLittleEndian(const uint64_t qwV) { return ToI64LE(qwV); }
static uint64_t ToBigEndian(const uint64_t qwV) { return ToI64BE(qwV); }
/* -- Convert integer to float between 0 and 1 ----------------------------- */
template<typename RT,typename PT>static const RT ToFloat(const PT tC)
  { return static_cast<RT>(tC) / numeric_limits<PT>::max(); }
/* -- Grab part of an integer and normalise it between 0 and 1 ----------- */
template<typename FT=float,size_t SB=0,typename IT=uint8_t>
  static FT Normalise(const unsigned int uiV)
    { return ToFloat<FT,IT>(static_cast<IT>(uiV >> SB)); }
/* -- Returns if specified integer is negative ----------------------------- */
template<bool is_signed, typename IntType>class IsNegativeFunctor;
template<typename IntType>class IsNegativeFunctor<true, IntType>
  { public: bool operator()(const IntType itVal) { return itVal < 0; } };
template<typename IntType>class IsNegativeFunctor<false, IntType>
  { public: bool operator()(const IntType) { return false; } };
template<typename IntType>static bool IsNegative(const IntType itVal)
  { return IsNegativeFunctor<numeric_limits<IntType>::is_signed,
      IntType>()(itVal); }
/* -- Returns if specified integer would overflow specified type ----------- */
template<typename TestIntType, typename ParamIntType>
  static bool IntWillOverflow(const ParamIntType pitVal)
{ // Automatically false if both types are the same
  if constexpr(is_same_v<TestIntType, ParamIntType>) return false;
  // Is signed (can be negative?). Return if out of bounds
  if constexpr(numeric_limits<TestIntType>::is_signed)
  { // If input type is unsigned then return true if it overflows.
    if constexpr(!numeric_limits<ParamIntType>::is_signed)
      if(static_cast<uintmax_t>(pitVal) > static_cast<uintmax_t>(INTMAX_MAX))
        return true;
    // Return if overflows
    const intmax_t iVal = static_cast<intmax_t>(pitVal);
    return iVal < static_cast<intmax_t>(numeric_limits<TestIntType>::min()) ||
           iVal > static_cast<intmax_t>(numeric_limits<TestIntType>::max());
  } // Unsigned so not as much checking needed. Return if out of bounds
  return IsNegative(pitVal) ||
    static_cast<uintmax_t>(pitVal) >
      static_cast<uintmax_t>(numeric_limits<TestIntType>::max());
}
/* -- Join two integers to make a bigger integer -------------------------- */
template<typename IntTypeHigh,typename IntTypeLow>
  static uint16_t MakeWord(const IntTypeHigh ithV, const IntTypeLow itlV)
    { return static_cast<uint16_t>((static_cast<uint16_t>(ithV) <<  8) |
        (static_cast<uint16_t>(itlV) & 0xff)); }
template<typename IntTypeHigh,typename IntTypeLow>
  static uint32_t MakeDWord(const IntTypeHigh ithV, const IntTypeLow itlV)
    { return static_cast<uint32_t>((static_cast<uint32_t>(ithV) << 16) |
        (static_cast<uint32_t>(itlV) & 0xffff)); }
template<typename IntTypeHigh,typename IntTypeLow>
  static uint64_t MakeQWord(const IntTypeHigh ithV, const IntTypeLow itlV)
    { return static_cast<uint64_t>((static_cast<uint64_t>(ithV) << 32) |
        (static_cast<uint64_t>(itlV) & 0xffffffff)); }
/* -- Return lowest and highest 8-bits of integer ------------------------- */
template<typename IntType>static uint8_t LowByte(const IntType itVal)
  { return static_cast<uint8_t>(itVal & 0x00ff); }
template<typename IntType>static uint8_t HighByte(const IntType itVal)
  { return static_cast<uint8_t>((itVal & 0xff00) >> 8); }
/* -- Return lowest and highest 16-bits of integer ------------------------- */
template<typename IntType>static uint16_t LowWord(const IntType itVal)
  { return static_cast<uint16_t>(itVal & 0x0000ffff); }
template<typename IntType>static uint16_t HighWord(const IntType itVal)
  { return static_cast<uint16_t>((itVal & 0xffff0000) >> 16); }
/* -- Return lowest and highest 32-bits of integer ------------------------- */
template<typename IntType>static uint32_t LowDWord(const IntType itVal)
  { return static_cast<uint32_t>(itVal & 0x00000000ffffffff); }
template<typename IntType>static uint32_t HighDWord(const IntType itVal)
  { return static_cast<uint32_t>((itVal & 0xffffffff00000000) >> 32); }
/* -- Return lowest or highest number out of two --------------------------- */
template<typename IntType1,typename IntType2>
  static IntType1 Minimum(const IntType1 itOne, const IntType2 itTwo)
    { return (itOne < static_cast<IntType1>(itTwo)) ?
        itOne : static_cast<IntType1>(itTwo); }
template<typename IntType1,typename IntType2>
  static IntType1 Maximum(const IntType1 itOne, const IntType2 itTwo)
    { return (itOne > static_cast<IntType1>(itTwo)) ?
        itOne : static_cast<IntType1>(itTwo); }
/* -- Clamp a number between two values ------------------------------------ */
template<typename TVAL, typename TMIN, typename TMAX>
  static TVAL Clamp(const TVAL tVal, const TMIN tMin, const TMAX tMax)
    { return Maximum(static_cast<TVAL>(tMin),
             Minimum(static_cast<TVAL>(tMax), tVal)); }
/* -- Make a percentage ---------------------------------------------------- */
template<typename T1, typename T2, typename R=double>
  static R MakePercentage(const T1 tCurrent, const T2 tMaximum,
    const R rMulti=100)
      { return static_cast<R>(tCurrent) / tMaximum * rMulti; }
/* -- Calculate distance between two values -------------------------------- */
template<typename T>static T Distance(const T tX, const T tY)
  { return tX > tY ? tX - tY : tY - tX; }
/* -- Round to nearest value ----------------------------------------------- */
template<typename T>static T Nearest(const T tValue, const T tMultiple)
  { return (tValue + (tMultiple / 2)) / tMultiple * tMultiple; }
/* -- Swaps the lowest four bits with the highest four bits of a char ------ */
template<typename T>static T Swap4Bit(const T tValue)
  { return (((tValue & 0xff) >> 4) | ((tValue & 0xff) << 4)) & 0xff; }
/* -- Returns the nearest power of two to specified number ----------------- */
template<typename RetType,typename IntType>
  static const RetType NearestPow2(const IntType itValue)
    { return static_cast<RetType>(pow(2, ceil(log(itValue) / log(2)))); }
/* -- If variable would overflow another type then return its maximum ------ */
template<typename RetType, typename IntType>
  static RetType IntOrMax(const IntType itValue)
    { return IntWillOverflow<RetType, IntType>(itValue) ?
        numeric_limits<RetType>::max() : static_cast<RetType>(itValue); }
/* -- Configurable rounding functor helper class --------------------------- */
template<typename T=double>class RoundFuncNone // Default no-outline class
{ /* -------------------------------------------------------------- */ private:
  static_assert(is_floating_point_v<T>, "Type not floating point!");
  /* ----------------------------------------------------------------------- */
  const T          tValue;             // Calculated advance value
  /* --------------------------------------------------------------- */ public:
  T Result(void) const { return tValue; }
  /* ----------------------------------------------------------------------- */
  explicit RoundFuncNone(const T tV) : tValue{ tV } { }
};/* -- Full outline required ---------------------------------------------- */
template<typename T=double>class RoundFuncFloor : public RoundFuncNone<T> {
  public: explicit RoundFuncFloor(const T tV) :
    RoundFuncNone<T>{ floor(tV) } {}
};/* -- Outside outline required ------------------------------------------- */
template<typename T=double>class RoundFuncCeil : public RoundFuncNone<T> {
  public: explicit RoundFuncCeil(const T tV) :
    RoundFuncNone<T>{ ceil(tV) } {}
};/* ----------------------------------------------------------------------- */
template<typename T=double>class RoundFuncRound : public RoundFuncNone<T> {
  public: explicit RoundFuncRound(const T tV) :
    RoundFuncNone<T>{ round(tV) } {}
};
/* -- Conversions ---------------------------------------------------------- */
template<typename IntType>
  static double MillimetresToInches(const IntType itValue)
    { return static_cast<double>(itValue) * 0.0393700787; }
template<typename IntType>
  static double GetDiagLength(const IntType itWidth, const IntType itHeight)
    { return sqrt(pow(itWidth, 2) + pow(itHeight, 2)); }
/* ------------------------------------------------------------------------- */
namespace Bit {                        // Bit functions namespace
/* -- Bits handling functions ---------------------------------------------- */
template<typename IntType>
  static IntType Slot(const IntType itPos)
    { return itPos / CHAR_BIT; }
template<typename IntType>
  static IntType Bit(const IntType itPos)
    { return itPos * CHAR_BIT; }
template<typename RetType,typename IntType>
  static RetType Mask(const IntType itPos)
    { return static_cast<RetType>(1 << (itPos % CHAR_BIT)); }
template<typename PtrType,typename IntType>
  static void Set(PtrType*const ptDst, const IntType itPos)
    { ptDst[Slot(itPos)] |=
        Mask<typename remove_pointer<PtrType>::type,IntType>(itPos); }
template<typename PtrType,typename IntType>
  static bool Test(PtrType*const ptDst, const IntType itPos)
    { return !!(ptDst[Slot(itPos)] &
        Mask<typename remove_pointer<PtrType>::type,IntType>(itPos)); }
template<typename PtrType,typename IntType>
  static void Clear(PtrType*const ptDst, const IntType itPos)
    { ptDst[Slot(itPos)] &=
        ~Mask<typename remove_pointer<PtrType>::type,IntType>(itPos); }
template<typename PtrType,typename IntType>
  static void Flip(PtrType*const ptDst, const IntType itPos)
    { ptDst[Slot(itPos)] ^=
        Mask<typename remove_pointer<PtrType>::type,IntType>(itPos); }
/* -- Bits handling functions copying from another bit buffer -------------- */
template<typename PtrType,typename IntType>
  static void Set2(PtrType*const ptDst, const IntType itDstPos,
    const PtrType*const ptSrc, const IntType itSrcPos)
      { ptDst[Slot(itDstPos)] |= ptSrc[Slot(itSrcPos)]; }
template<typename PtrType,typename IntType>
  static bool Test2(PtrType*const ptDst, const IntType itDstPos,
    const PtrType*const ptSrc, const IntType itSrcPos)
      { return ptDst[Slot(itDstPos)] & ptSrc[Slot(itSrcPos)]; }
template<typename PtrType,typename IntType>
  static void Clear2(PtrType*const ptDst, const IntType itDstPos,
    const PtrType*const ptSrc, const IntType itSrcPos)
      { ptDst[Slot(itDstPos)] &= ~ptSrc[Slot(itSrcPos)]; }
template<typename PtrType,typename IntType>
  static void Flip2(PtrType*const ptDst, const IntType itDstPos,
    const PtrType*const ptSrc, const IntType itSrcPos)
      { ptDst[Slot(itDstPos)] ^= ptSrc[Slot(itSrcPos)]; }
/* ------------------------------------------------------------------------- */
};                                     // End of bit functions namespace
/* -- End of module namespace ---------------------------------------------- */
};                                     // End of interface
/* == EoF =========================================================== EoF == */

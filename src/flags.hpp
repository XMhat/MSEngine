/* == FLAGS.HPP ============================================================ **
** ######################################################################### **
** ## MS-ENGINE              Copyright (c) MS-Design, All Rights Reserved ## **
** ######################################################################### **
** ## This class helps with managing flags (a collection of booleans) or  ## **
** ## many bits in a byte. The compiler should hopefully optimise all of  ## **
** ## this complecated functory to single CPU instructions.               ## **
** ######################################################################### **
** ========================================================================= */
#pragma once                           // Only one incursion allowed
/* ------------------------------------------------------------------------- */
namespace IFlags {                     // Start of module namespace
/* == Storage for flags ==================================================== **
** ######################################################################### **
** ## Simple unprotected integer based flags.                             ## **
** ######################################################################### */
template<typename IntType>class FlagsStorageUnsafe
{ /* -- Values storage ------------------------------------------ */ protected:
  IntType          itV;                // The simple value
  /* -- Reset with specified value ----------------------------------------- */
  template<typename AnyType>void FlagSetInt(const AnyType atO)
    { itV = static_cast<IntType>(atO); }
  /* -- Swap values -------------------------------------------------------- */
  void FlagSwapStorage(FlagsStorageUnsafe &fO) { swap(itV, fO.itV); }
  /* -- Implicit init constructor (todo, make explicit but many errors!) --- */
  explicit FlagsStorageUnsafe(const IntType iV) : itV{ iV } {}
  /* -- Get values ------------------------------------------------- */ public:
  template<typename AnyType=IntType>AnyType FlagGet(void) const
    { return static_cast<AnyType>(itV); }
};/* ----------------------------------------------------------------------- */
/* == Atomic storage for flags ============================================= **
** ######################################################################### **
** ## Thread safe integer based flags.                                    ## **
** ######################################################################### */
template<typename IntType,
         typename SafeType = atomic<IntType>>
class FlagsStorageSafe :
  /* -- Base classes ------------------------------------------------------- */
  private SafeType
{ /* -- Set values ---------------------------------------------- */ protected:
  template<typename AnyType>void FlagSetInt(const AnyType atO)
    { this->store(static_cast<IntType>(atO)); }
  /* -- Swap values -------------------------------------------------------- */
  void FlagSwapStorage(FlagsStorageSafe &fO) { swap(fO); }
  /* -- Implicit init constructor (todo, make explicit but many errors!) --- */
  explicit FlagsStorageSafe(const IntType iV) : SafeType{ iV } {}
  /* -- Get values ------------------------------------------------- */ public:
  template<typename AnyType=IntType>AnyType FlagGet(void) const
    { return static_cast<AnyType>(this->load()); }
};/* ----------------------------------------------------------------------- */
/* == Read-only flags helper class ========================================= **
** ######################################################################### **
** ## If only read-only access is desired.                                ## **
** ######################################################################### */
template<typename IntType,
         class StorageType = FlagsStorageUnsafe<IntType>>
class FlagsConst :
  /* -- Base classes ------------------------------------------------------- */
  public StorageType
{ /* -- Is bits lesser than specified value? ----------------------- */ public:
  bool FlagIsLesser(const FlagsConst &fO) const
    { return this->FlagGet() < fO.FlagGet(); }
  /* -- Is bits lesser or equal to specified value? ------------------------ */
  bool FlagIsLesserEqual(const FlagsConst &fO) const
    { return this->FlagGet() <= fO.FlagGet(); }
  /* -- Is bits greater to specified value? -------------------------------- */
  bool FlagIsGreater(const FlagsConst &fO) const
    { return this->FlagGet() > fO.FlagGet(); }
  /* -- Is bits greater or equal to specified value? ----------------------- */
  bool FlagIsGreaterEqual(const FlagsConst &fO) const
    { return this->FlagGet() >= fO.FlagGet(); }
  /* -- Are there not any flags set? --------------------------------------- */
  bool FlagIsZero(void) const
    { return this->FlagGet() == static_cast<IntType>(0); }
  /* -- Are any flags actually set? ---------------------------------------- */
  bool FlagIsNonZero(void) const { return !FlagIsZero(); }
  /* -- Is flag set with specified value? ---------------------------------- */
  bool FlagIsSet(const FlagsConst &fO) const
    { return (this->FlagGet() & fO.FlagGet()) == fO.FlagGet(); }
  /* -- Is any flag set with specified value? ------------------------------ */
  bool FlagIsAnyOfSet(const FlagsConst &fO) const
    { return (this->FlagGet() & fO.FlagGet()) != 0; }
  /* -- Is bit clear of specified value? ----------------------------------- */
  bool FlagIsClear(const FlagsConst &fO) const
    { return (~this->FlagGet() & fO.FlagGet()) == fO.FlagGet(); }
  /* -- Is bit clear of specified value? ----------------------------------- */
  bool FlagIsAnyOfClear(const FlagsConst &fO) const
    { return (~this->FlagGet() & fO.FlagGet()) != 0; }
  /* -- Is flag set with specified value and clear with another? ----------- */
  bool FlagIsSetAndClear(const FlagsConst &fO1, const FlagsConst &fO2) const
    { return FlagIsSet(fO1) && FlagIsClear(fO2); }
  /* -- Flags are not masked in specified other flags? --------------------- */
  bool FlagIsNotInMask(const FlagsConst &fO) const
    { return this->FlagGet() & ~fO.FlagGet(); }
  /* -- Flags are masked in specified other flags? ------------------------- */
  bool FlagIsInMask(const FlagsConst &fO) const
    { return !FlagIsNotInMask(fO); }
  /* -- Is any of these flags set and cleared? ----------------------------- */
  bool FlagIsAnyOfSetAndClear(void) const { return false; }
  template<typename ...VarArgs>
    bool FlagIsAnyOfSetAndClear(const FlagsConst &fO1, const FlagsConst &fO2,
      const VarArgs &...vaVars) const
  { return FlagIsSetAndClear(fO1, fO2) ?
      true : FlagIsAnyOfSetAndClear(vaVars...); }
  /* -- Is bits set? ------------------------------------------------------- */
  bool FlagIsEqualToBool(const FlagsConst &fO, const bool bS) const
    { return FlagIsSet(fO) == bS; }
  /* -- Is bits not set? --------------------------------------------------- */
  bool FlagIsNotEqualToBool(const FlagsConst &fO, const bool bS)
    const { return FlagIsSet(fO) != bS; }
  /* -- Return one variable or another if set ------------------------------ */
  template<typename AnyType>
    const AnyType FlagIsSetTwo(const FlagsConst &fO,
      const AnyType tSet, const AnyType tClear) const
  { return FlagIsSet(fO) ? tSet : tClear; }
  /* -- Init constructors -------------------------------------------------- */
  template<typename AnyType>explicit FlagsConst(const AnyType atV) :
    StorageType{ static_cast<IntType>(atV) } { }
  /* -- Operators ---------------------------------------------------------- */
  const FlagsConst operator~(void) const
    { return FlagsConst{ ~this->template FlagGet<IntType>() }; }
  const FlagsConst operator|(const FlagsConst &a) const
    { return FlagsConst{ this->template FlagGet<IntType>() |
                             a.template FlagGet<IntType>() }; }
  const FlagsConst operator&(const FlagsConst &a) const
    { return FlagsConst{ this->template FlagGet<IntType>() &
                             a.template FlagGet<IntType>() }; }
  const FlagsConst operator^(const FlagsConst &a) const
    { return FlagsConst{ this->template FlagGet<IntType>() ^
                             a.template FlagGet<IntType>() }; }
  /* -- Direct access using class variable name which returns value -------- */
  operator IntType(void) const
    { return this->template FlagGet<IntType>(); }
};/* ----------------------------------------------------------------------- */
/* == Flags helper class =================================================== **
** ######################################################################### **
** ## Read-write acesss for specified type.                               ## **
** ######################################################################### */
template<typename IntType,
         class StorageType = FlagsStorageUnsafe<IntType>,
         class ConstType = FlagsConst<IntType, StorageType>>
struct Flags :
  /* -- Base classes ------------------------------------------------------- */
  public ConstType
{ /* -- Swap function ------------------------------------------------------ */
  void FlagSwap(Flags &fO) { this->FlagSwapStorage(fO); }
  /* -- Set bits ----------------------------------------------------------- */
  void FlagSet(const IntType &itO) { this->FlagSetInt(this->FlagGet() | itO); }
  void FlagSet(const ConstType &fO)
    { this->FlagSetInt(this->FlagGet() | fO.FlagGet()); }
  /* -- Set all bits ------------------------------------------------------- */
  void FlagReset(const IntType itO) { this->FlagSetInt(itO); }
  void FlagReset(const ConstType &fO) { this->FlagSetInt(fO.FlagGet()); }
  void FlagReset(void) { this->FlagSetInt(0); }
  /* -- Not specified bits ------------------------------------------------- */
  void FlagNot(const ConstType &fO)
    { this->FlagSetInt(this->FlagGet() & ~fO.FlagGet()); }
  /* -- Mask bits ---------------------------------------------------------- */
  void FlagMask(const ConstType &fO)
    { this->FlagSetInt(this->FlagGet() & fO.FlagGet()); }
  /* -- Toggle specified bits ---------------------------------------------- */
  void FlagToggle(const ConstType &fO)
    { this->FlagSetInt(this->FlagGet() ^ fO.FlagGet()); }
  /* -- Clear specified bits ----------------------------------------------- */
  void FlagClear(const ConstType &fO)
    { FlagMask(ConstType{ ~fO.template FlagGet<IntType>() }); }
  /* -- Add or clear bits -------------------------------------------------- */
  void FlagSetOrClear(const ConstType &fO, const bool bX)
    { if(bX) FlagSet(fO); else FlagClear(fO); }
  /* -- Add and clear bits from specified enum ----------------------------- */
  void FlagSetAndClear(const ConstType &fS, const ConstType &fC)
    { FlagSet(fS); FlagClear(fC); }
  /* -- Init constructors -------------------------------------------------- */
  explicit Flags(const IntType &itO) : ConstType{ itO } {}
  explicit Flags(const ConstType &ctO) : ConstType{ ctO } {}
  /* -- Default constructor ------------------------------------------------ */
  Flags(void) : ConstType{ 0 } {}
};/* ----------------------------------------------------------------------- */
/* == Safe flags helper class ============================================== **
** ######################################################################### **
** ## Uses atomic storage for safe access.                                ## **
** ######################################################################### */
template<typename IntType,
         class StorageType = FlagsStorageSafe<IntType>,
         class UStorageType = FlagsStorageUnsafe<IntType>,
         class UConstType = FlagsConst<IntType, UStorageType>,
         class FlagsType = Flags<IntType, StorageType, UConstType>,
         class ConstType = FlagsConst<IntType, StorageType>>
class SafeFlags :
  /* -- Base classes ------------------------------------------------------- */
  public FlagsType
{ /* -- Implicit init constructor (todo, make explicit!) ----------- */ public:
  explicit SafeFlags(const ConstType &fO) : FlagsType{ fO } {}
  explicit SafeFlags(const UConstType &fO) : FlagsType{ fO } {}
};/* ----------------------------------------------------------------------- */
/* == Flags helper macro =================================================== */
#define BUILD_FLAGS_EX(n, s, ...) \
  typedef uint64_t n ## FlagsType; \
  typedef s<n ## FlagsType> n ## Flags; \
  typedef FlagsConst<n ## FlagsType> n ## FlagsConst; \
  static const n ## FlagsConst __VA_ARGS__;
#define BUILD_FLAGS(n, ...) BUILD_FLAGS_EX(n, Flags, __VA_ARGS__)
#define BUILD_SECURE_FLAGS(n, ...) BUILD_FLAGS_EX(n, SafeFlags, __VA_ARGS__)
/* -- Pre-defined flags ---------------------------------------------------- */
constexpr const array<const uint64_t, 65> Flag
{ /* ----------------------------------------------------------------------- */
  0x0000000000000000,0x0000000000000001,0x0000000000000002,0x0000000000000004,
  0x0000000000000008,0x0000000000000010,0x0000000000000020,0x0000000000000040,
  0x0000000000000080,0x0000000000000100,0x0000000000000200,0x0000000000000400,
  0x0000000000000800,0x0000000000001000,0x0000000000002000,0x0000000000004000,
  0x0000000000008000,0x0000000000010000,0x0000000000020000,0x0000000000040000,
  0x0000000000080000,0x0000000000100000,0x0000000000200000,0x0000000000400000,
  0x0000000000800000,0x0000000001000000,0x0000000002000000,0x0000000004000000,
  0x0000000008000000,0x0000000010000000,0x0000000020000000,0x0000000040000000,
  0x0000000080000000,0x0000000100000000,0x0000000200000000,0x0000000400000000,
  0x0000000800000000,0x0000001000000000,0x0000002000000000,0x0000004000000000,
  0x0000008000000000,0x0000010000000000,0x0000020000000000,0x0000040000000000,
  0x0000080000000000,0x0000100000000000,0x0000200000000000,0x0000400000000000,
  0x0000800000000000,0x0001000000000000,0x0002000000000000,0x0004000000000000,
  0x0008000000000000,0x0010000000000000,0x0020000000000000,0x0040000000000000,
  0x0080000000000000,0x0100000000000000,0x0200000000000000,0x0400000000000000,
  0x0800000000000000,0x1000000000000000,0x2000000000000000,0x4000000000000000,
  0x8000000000000000
};/* ----------------------------------------------------------------------- */
}                                      // End of module namespace
/* == EoF =========================================================== EoF == */

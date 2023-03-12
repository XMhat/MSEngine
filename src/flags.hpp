/* == FLAGS.HPP ============================================================ */
/* ######################################################################### */
/* ## MS-ENGINE              Copyright (c) MS-Design, All Rights Reserved ## */
/* ######################################################################### */
/* ## This class helps with managing flags (a collection of booleans) or  ## */
/* ## many bits in a byte. The compiler should hopefully optimise all of  ## */
/* ## this complecated functory to single CPU instructions.               ## */
/* ######################################################################### */
/* ========================================================================= */
#pragma once                           // Only one incursion allowed
/* ------------------------------------------------------------------------- */
namespace IfFlags {                    // Start of module namespace
/* == Storage for flags ==================================================== */
/* ######################################################################### */
/* ## Simple unprotected integer based flags.                             ## */
/* ######################################################################### */
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
/* == Atomic storage for flags ============================================= */
/* ######################################################################### */
/* ## Thread safe integer based flags.                                    ## */
/* ######################################################################### */
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
/* == Read-only flags helper class ========================================= */
/* ######################################################################### */
/* ## If only read-only access is desired.                                ## */
/* ######################################################################### */
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
    { return this->FlagGet() & fO.FlagGet(); }
  /* -- Flags are not masked in specified other flags? --------------------- */
  bool FlagIsNotInMask(const FlagsConst &fO) const
    { return this->FlagGet() & ~fO.FlagGet(); }
  /* -- Flags are masked in specified other flags? ------------------------- */
  bool FlagIsInMask(const FlagsConst &fO) const
    { return !FlagIsNotInMask(fO); }
  /* -- Is bit clear of specified value? ----------------------------------- */
  bool FlagIsClear(const FlagsConst &fO) const { return !FlagIsSet(fO); }
  /* -- Is bits set? ------------------------------------------------------- */
  bool FlagIsEqualToBool(const FlagsConst &fO, const bool bS) const
   { return FlagIsSet(fO) == bS; }
  /* -- Is bits not set? --------------------------------------------------- */
  bool FlagIsNotEqualToBool(const FlagsConst &fO, const bool bS) const
   { return FlagIsSet(fO) != bS; }
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
  operator IntType(void) const { return this->template FlagGet<IntType>(); }
};/* ----------------------------------------------------------------------- */
/* == Flags helper class =================================================== */
/* ######################################################################### */
/* ## Read-write acesss for specified type.                               ## */
/* ######################################################################### */
template<typename IntType,
         class StorageType = FlagsStorageUnsafe<IntType>,
         class ConstType = FlagsConst<IntType, StorageType>>
struct Flags :
  /* -- Base classes ------------------------------------------------------- */
  public ConstType
{ /* -- Swap function ------------------------------------------------------ */
  void FlagSwap(Flags &fO) { this->FlagSwapStorage(fO); }
  /* -- Set bits ----------------------------------------------------------- */
  void FlagSet(const ConstType &fO)
    { this->FlagSetInt(this->FlagGet() | fO.FlagGet()); }
  /* -- Set all bits ------------------------------------------------------- */
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
  /* -- nit constructors -------------------------------------------------- */
  explicit Flags(const IntType &itO) : ConstType{ itO } {}
  explicit Flags(const ConstType &ctO) : ConstType{ ctO } {}
};/* ----------------------------------------------------------------------- */
/* == Safe flags helper class ============================================== */
/* ######################################################################### */
/* ## Uses atomic storage for safe access.                                ## */
/* ######################################################################### */
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
};                                     // End of module namespace
/* == Flags helper macro =================================================== */
#define BUILD_FLAGS_EX(n, s, ...) \
  typedef uint64_t n ## FlagsType; \
  typedef s<n ## FlagsType> n ## Flags; \
  typedef FlagsConst<n ## FlagsType> n ## FlagsConst; \
  static const n ## FlagsConst __VA_ARGS__;
#define BUILD_FLAGS(n, ...) BUILD_FLAGS_EX(n, Flags, __VA_ARGS__)
#define BUILD_SECURE_FLAGS(n, ...) BUILD_FLAGS_EX(n, SafeFlags, __VA_ARGS__)
/* == EoF =========================================================== EoF == */

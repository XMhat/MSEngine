/* == IDENT.HPP ============================================================ */
/* ######################################################################### */
/* ## MS-ENGINE              Copyright (c) MS-Design, All Rights Reserved ## */
/* ######################################################################### */
/* ## Simple class to hold a string identifier.                           ## */
/* ######################################################################### */
/* ========================================================================= */
#pragma once                           // Only one incursion allowed
/* -- Module namespace ----------------------------------------------------- */
namespace IfIdent {                    // Keep declarations neatly categorised
/* -- Read only identifier class ------------------------------------------- */
template<class StringType>class IdentBase
{ /* -- Private variables --------------------------------------- */ protected:
  StringType       strIdentifier;      // The identifier
  /* -- Identifier is set? ----------------------------------------- */ public:
  bool IdentIsSet(void) { return !strIdentifier.empty(); }
  bool IdentIsNotSet(void) { return !IdentIsSet(); }
  /* -- Get identifier ----------------------------------------------------- */
  const StringType &IdentGet(void) const { return strIdentifier; }
  /* -- Get identifier by address ------------------------------------------ */
  const char *IdentGetCStr(void) const { return IdentGet().c_str(); }
  /* -- Move constructor from another rvalue string ------------- */ protected:
  explicit IdentBase(StringType &&strId) :
    /* -- Initialisation of members ---------------------------------------- */
    strIdentifier{ move(strId) }       // Move supplied string
    /* -- No code ---------------------------------------------------------- */
    { }
  /* -- Move constructor from rvalue identifier ---------------------------- */
  explicit IdentBase(IdentBase &&idOther) :
    /* -- Initialisation of members ---------------------------------------- */
    strIdentifier{                     // Initialise string
      move(idOther.strIdentifier) }    // Move supplied string
    /* -- No code ---------------------------------------------------------- */
    { }
  /* -- Copy constructor from another lvalue string ------------------------ */
  explicit IdentBase(const StringType &strId) :
    /* -- Initialisation of members ---------------------------------------- */
    strIdentifier{ strId }             // Copy supplied name
    /* -- Noi code --------------------------------------------------------- */
    { }
  /* -- Standby constructor ------------------------------------------------ */
  IdentBase(void) { }
  /* -- Default suppressions ----------------------------------------------- */
  DELETECOPYCTORS(IdentBase);          // Remove default functions
};/* ----------------------------------------------------------------------- */
/* -- Identifier class ----------------------------------------------------- */
class Ident :
  /* -- Base classes ------------------------------------------------------- */
  public IdentBase<string>             // The read-only class
{ /* -- Set identifier by rvalue ----------------------------------- */ public:
  void IdentSet(string &&strId) { strIdentifier = move(strId); }
  /* -- Set identifier by lvalue ------------------------------------------- */
  void IdentSet(const string &strId) { strIdentifier = strId; }
  /* -- Clear identifier --------------------------------------------------- */
  void IdentClear(void) { strIdentifier.clear(); }
  /* -- Swap identifier ---------------------------------------------------- */
  void IdentSwap(Ident &idOther) { strIdentifier.swap(idOther.strIdentifier); }
  /* -- Move constructor from another rvalue string ------------------------ */
  explicit Ident(string &&strId) :
    /* -- Initialisation of members ---------------------------------------- */
    IdentBase{ move(strId) }           // Move supplied name
    /* -- No code ---------------------------------------------------------- */
    { }
  /* -- Move constructor from rvalue identifier ---------------------------- */
  explicit Ident(Ident &&idOther) :
    /* -- Initialisation of members ---------------------------------------- */
    IdentBase{                         // Initialise base
      move(idOther.strIdentifier) }    // Move other name
    /* -- No code ---------------------------------------------------------- */
    { }
  /* -- Copy constructor from another lvalue string ------------------------ */
  explicit Ident(const string &strId) :
    /* -- Initialisation of members ---------------------------------------- */
    IdentBase{ strId }                 // Copy name
    /* -- No code ---------------------------------------------------------- */
    { }
  /* -- Standby constructor ------------------------------------------------ */
  Ident(void)
    /* -- No code ---------------------------------------------------------- */
    { }
  /* -- Default suppressions ----------------------------------------------- */
  DELETECOPYCTORS(Ident);              // Remove default functions
};/* ----------------------------------------------------------------------- */
typedef IdentBase<const string> IdentConst;
/* == Id to string list helper class ======================================= */
template<size_t stMaximum,size_t stMinimum=0>class IdList :
  /* -- Dependents --------------------------------------------------------- */
  private IdentConst                   // Alternative if id is unknown
{ /* -- Typedefs ----------------------------------------------------------- */
  typedef array<const string, stMaximum> List;
  /* -- Variables ---------------------------------------------------------- */
  const List       lItems;             // The list of items
  /* -- Constructor (no initialisation) ---------------------------- */ public:
  public: explicit IdList(const List &lNI,
    const string &strNU=IfString::strBlank) :
    /* -- Initialisers ----------------------------------------------------- */
    IdentConst{ move(strNU) },         // Unknown item string
    lItems{ move(lNI) }                // Items
    /* -- No code ---------------------------------------------------------- */
    { }
  /* -- Get string --------------------------------------------------------- */
  template<typename IntType=size_t>const string &Get(const IntType itId) const
  { // Allow any input integer type, we don't need to convert if the same
    const size_t stId = static_cast<size_t>(itId);
    return stId >= stMinimum && stId < stMaximum ? lItems[stId] : IdentGet();
  }
};/* ----------------------------------------------------------------------- */
/* == Id to string list helper class ======================================= */
template<class KeyType=unsigned int, class ValueType=string>class IdMap :
  /* -- Dependents --------------------------------------------------------- */
  private IdentConst                   // Alternative if id is unknown
{ /* -- Macros ------------------------------------------------------------- */
  #define IDMAPSTR(i) { i, #i }        // Helper macro for constructors
  /* -- Typedefs ----------------------------------------------------------- */
  typedef map<const KeyType, const ValueType> Map;
  /* -- Variables ---------------------------------------------------------- */
  const Map        mItems;             // The list of items
  /* -- Constructor (no initialisation) ---------------------------- */ public:
  public: explicit IdMap(const Map &mNI,
    const string &strNU=IfString::strBlank) :
    /* -- Initialisers ----------------------------------------------------- */
    IdentConst{ move(strNU) },         // Unknown item string
    mItems{ move(mNI) }                // Items map
    /* -- No code ---------------------------------------------------------- */
    { }
  /* -- Get string --------------------------------------------------------- */
  const ValueType &Get(const KeyType ktId) const
  { // Find code and return custom error if not found else return string
    const auto aName{ mItems.find(ktId) };
    return aName != mItems.cend() ? aName->second : IdentGet();
  }
};/* ----------------------------------------------------------------------- */
/* -- End of module namespace ---------------------------------------------- */
};                                     // End of interface
/* == EoF =========================================================== EoF == */

/* == DYLIB.HPP ============================================================ */
/* ######################################################################### */
/* ## MS-ENGINE              Copyright (c) MS-Design, All Rights Reserved ## */
/* ######################################################################### */
/* ## This is a class that handles the ability to load DLL files and      ## */
/* ## execute functions from them.                                        ## */
/* ######################################################################### */
/* ========================================================================= */
#pragma once                           // Only one incursion allowed
/* -- Module namespace ----------------------------------------------------- */
namespace IfDyLib {                    // Keep declarations neatly categorised
/* -- Includes ------------------------------------------------------------- */
using namespace IfSystem;              // Using system interface
/* -- Typedefs ------------------------------------------------------------- */
typedef unique_ptr<void, function<decltype(System::LibFree)>> DllHandle;
/* == DyLib collector and member class ===================================== */
BEGIN_COLLECTORDUO(DyLibs, DyLib, CLHelperUnsafe, ICHelperUnsafe),
  /* -- Base classes ------------------------------------------------------- */
  public Ident,                        // Object name
  private DllHandle                    // Self freeing dll handle
{ /* -- Private variables -------------------------------------------------- */
  Ident            idFull;             // Full path to module
  /* -- Swap with another DyLib ------------------------------------ */ public:
  void DLSwap(DyLib &oOther)
  { // Swap module handle
    swap(oOther);
    // Swap filename
    IdentSwap(oOther);
    idFull.IdentSwap(oOther.idFull);
    // Swap registration
    CollectorSwapRegistration(oOther);
  }
  /* -- Get module address ------------------------------------------------- */
  void *DLGetHandle(void) const { return get(); }
  const string &DLGet(void) const { return idFull.IdentGet(); }
  /* -- Return if library is opened or closed ------------------------------ */
  bool DLIsOpened(void) const { return !!DLGetHandle(); }
  bool DLIsClosed(void) const { return !DLIsOpened(); }
  /* -- Direct access using class variable name which returns opened ------- */
  operator bool(void) const { return DLIsOpened(); }
  /* -- Get address of function -------------------------------------------- */
  template<typename FuncType>FuncType
    DLGetAddr(const char*const cpName) const
      { return System::LibGetAddr<FuncType>(DLGetHandle(), cpName); }
  /* -- Init constructor --------------------------------------------------- */
  explicit DyLib(const string &strN) : // Name of module to load
    /* -- Initialisation of members ---------------------------------------- */
    ICHelperDyLib{ *cDyLibs, this },   // Automatic (de)registration
    Ident{ strN },                     // Set filename
    DllHandle{                         // Load the specified module
      System::LibLoad(IdentGetCStr()),    // The specified filename
      System::LibFree },               // Automatically free on leaving scope
    idFull{                            // Set full name of file
      cSystem->LibGetName(             // System function request
        DLGetHandle(),                 // Get module handle
        IdentGetCStr()) }                 // Get requested module name
    /* -- Set the full name of the module ---------------------------------- */
    { }
  /* -- Init constructor --------------------------------------------------- */
  explicit DyLib(string &&strN) :      // Name of module to be moved load
    /* -- Initialisation of members ---------------------------------------- */
    ICHelperDyLib{ *cDyLibs, this },   // Automatic (de)registration
    Ident{ move(strN) },               // Set filename
    DllHandle{                         // Load the specified module
      System::LibLoad(IdentGetCStr()),    // The specified filename
      System::LibFree },               // Automatically free on leaving scope
    idFull{                            // Set full name of file
      cSystem->LibGetName(             // System function request
        DLGetHandle(),                 // Get system module handle
        IdentGetCStr()) }                 // Get requested module name
    /* -- Set the full name of the module ---------------------------------- */
    { }
  /* -- Standby constructor ------------------------------------------------ */
  DyLib(void) :                        // No parameters
    /* -- Initialisation of members ---------------------------------------- */
    ICHelperDyLib{ *cDyLibs }          // No automatic registration
    /* --------------------------------------------------------------------- */
    { }                                // Do nothing else
  /* -- Assign constructor on class creation ------------------------------- */
  DyLib(DyLib &&oOther) :              // Move constructor
    /* -- Initialisation of members ---------------------------------------- */
    DyLib()                            // Use stand-by initialisers
    /* -- Swap with other dylib -------------------------------------------- */
    { DLSwap(oOther); }
  /* ----------------------------------------------------------------------- */
  DELETECOPYCTORS(DyLib);              // Disable copy constructor and operator
};/* ----------------------------------------------------------------------- */
END_COLLECTOR(DyLibs);                 // End of collector class
/* -- End of module namespace ---------------------------------------------- */
};                                     // End of interface
/* == EoF =========================================================== EoF == */

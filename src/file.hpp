/* == FILE.HPP ============================================================= */
/* ######################################################################### */
/* ## MS-ENGINE              Copyright (c) MS-Design, All Rights Reserved ## */
/* ######################################################################### */
/* ## This module is a simple C++ wrapper for C file stream functions.    ## */
/* ######################################################################### */
/* ========================================================================= */
#pragma once                           // Only one incursion allowed
/* ------------------------------------------------------------------------- */
namespace IfFile {                     // Start of module namespace
/* -- Includes ------------------------------------------------------------- */
using namespace IfFStream;             // Using fstream namespace
using namespace IfCollector;           // Using collector namespace
/* -- File collector and member class -------------------------------------- */
BEGIN_COLLECTORDUO(Files, File, CLHelperUnsafe, ICHelperUnsafe),
  /* -- Base classes ------------------------------------------------------- */
  public FStream,                      // File stream class
  public Lockable                      // Lua garbage collector instruction
{ /* -- Basic constructor with no init ----------------------------- */ public:
  File(void) :
    /* -- Initialisers ----------------------------------------------------- */
    ICHelperFile{ *cFiles, this },     // Register the object in collector
    IdentCSlave{ cParent.CtrNext() }   // Initialise identification number
    /* --------------------------------------------------------------------- */
    { }
  /* ----------------------------------------------------------------------- */
  DELETECOPYCTORS(File)                // Disable copy constructor and operator
};/* ----------------------------------------------------------------------- */
END_COLLECTOR(Files)                   // Finish global Files collector
/* ------------------------------------------------------------------------- */
};                                     // End of module namespace
/* == EoF =========================================================== EoF == */

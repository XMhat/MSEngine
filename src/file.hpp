/* == FILE.HPP ============================================================= */
/* ######################################################################### */
/* ## MS-ENGINE              Copyright (c) MS-Design, All Rights Reserved ## */
/* ######################################################################### */
/* ## This module is a simple C++ wrapper for C file stream functions.    ## */
/* ######################################################################### */
/* ========================================================================= */
#pragma once                           // Only one incursion allowed
/* -- Module namespace ----------------------------------------------------- */
namespace IfFile {                     // Keep declarations neatly categorised
/* -- Includes ------------------------------------------------------------- */
using namespace IfFStream;             // Using fstream interface
/* -- File collector and member class -------------------------------------- */
BEGIN_COLLECTORDUO(Files, File, CLHelperUnsafe, ICHelperUnsafe),
  /* -- Base classes ------------------------------------------------------- */
  public FStream,                      // File stream class
  public Lockable                      // Lua garbage collector instruction
{ /* -- Basic constructor with no init ----------------------------- */ public:
  File(void) :
    /* -- Initialisation of members ---------------------------------------- */
    ICHelperFile{ *cFiles, this }      // Register the object in collector
    /* --------------------------------------------------------------------- */
    { }
  /* ----------------------------------------------------------------------- */
  DELETECOPYCTORS(File);               // Disable copy constructor and operator
};/* ----------------------------------------------------------------------- */
END_COLLECTOR(Files);                  // Finish global Files collector
/* -- End of module namespace ---------------------------------------------- */
};                                     // End of interface
/* == EoF =========================================================== EoF == */

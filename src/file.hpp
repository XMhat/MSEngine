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
/* -- Read string to file in one go ---------------------------------------- */
void FileReadString(lua_State*const lS)
{ // Open file in text mode with the specified mode and if successful?
  if(FStream fFile{ GetCppFileName(lS, 1, "File"), FStream::FM_R_T })
  { // Read file and store in string
    const string strData{ fFile.FStreamReadStringSafe() };
    // If no error occured then return the data read
    if(!fFile.FStreamFError()) return PushCppString(lS, strData);
    // Error occured so return error message
    PushBoolean(lS, false);
    PushCppString(lS, fFile.FStreamGetErrStr());
  } // Open failed?
  else
  { // Error occured so return error message
    PushBoolean(lS, false);
    PushCppString(lS, fFile.FStreamGetErrStr());
  }
}
/* -- Write data a file in one go ------------------------------------------ */
void FileWriteData(lua_State*const lS, const string &strFile,
  const FStream::Mode mMode, const void*const vpPtr, const size_t stBytes)
{ // Open file with the specified mode and if successful?
  if(FStream fFile{ strFile, mMode })
  { // Success if zero size
    if(!stBytes) return PushBoolean(lS, true);
    // Write the data and check if we wrote enough data?
    const size_t stWritten = fFile.FStreamWrite(vpPtr, stBytes);
    if(stWritten == stBytes) return PushInteger(lS, stWritten);
    // Error occured so return failure with error message
    PushBoolean(lS, false);
    PushCppString(lS, fFile.FStreamFError() ? fFile.FStreamGetErrStr() :
      Format("Only $ of $ bytes written", stWritten, stBytes));
  } // Failed so return reason
  else
  { // Error occured so return failure with error message
    PushBoolean(lS, false);
    PushCppString(lS, fFile.FStreamGetErrStr());
  }
}
/* -- Append string to file in one go -------------------------------------- */
void FileAppendString(lua_State*const lS)
{ // Get filename, string to write from Lua and send it to writer function
  const string strFile{ GetCppFileName(lS, 1, "File") },
               strWhat{ GetCppString(lS, 2, "String") };
  FileWriteData(lS,
    strFile, FStream::FM_A_B, strWhat.data(), strWhat.length());
}
/* -- Write string to file in one go --------------------------------------- */
void FileWriteString(lua_State*const lS)
{ // Get filename, string to write from Lua and send it to writer function
  const string strFile{ GetCppFileName(lS, 1, "File") },
               strWhat{ GetCppString(lS, 2, "String") };
  FileWriteData(lS,
    strFile, FStream::FM_W_B, strWhat.data(), strWhat.length());
}
/* -- Append data to file in one go ---------------------------------------- */
void FileAppendBlock(lua_State*const lS)
{ // Get filename, string to write from Lua and send it to writer function
  const string strFile{ GetCppFileName(lS, 1, "File") };
  const DataConst &dcWhat = *GetPtr<Asset>(lS, 2, "Asset");
  FileWriteData(lS, strFile, FStream::FM_A_B, dcWhat.Ptr(), dcWhat.Size());
}
/* -- Write data to file in one go ----------------------------------------- */
void FileWriteBlock(lua_State*const lS)
{ // Get filename, string to write from Lua and send it to writer function
  const string strFile{ GetCppFileName(lS, 1, "File") };
  const DataConst &dcWhat = *GetPtr<Asset>(lS, 2, "Asset");
  FileWriteData(lS, strFile, FStream::FM_W_B, dcWhat.Ptr(), dcWhat.Size());
}
/* ------------------------------------------------------------------------- */
};                                     // End of module namespace
/* == EoF =========================================================== EoF == */

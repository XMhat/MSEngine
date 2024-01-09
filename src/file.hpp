/* == FILE.HPP ============================================================= **
** ######################################################################### **
** ## MS-ENGINE              Copyright (c) MS-Design, All Rights Reserved ## **
** ######################################################################### **
** ## This module is a simple C++ wrapper for C file stream functions.    ## **
** ######################################################################### **
** ========================================================================= */
#pragma once                           // Only one incursion allowed
/* ------------------------------------------------------------------------- */
namespace IFile {                      // Start of private module namespace
/* -- Dependencies --------------------------------------------------------- */
using namespace IAsset::P;             using namespace ICollector::P;
using namespace IFStream::P;           using namespace IIdent::P;
using namespace ILuaUtil::P;           using namespace IMemory::P;
using namespace IStd::P;               using namespace IString::P;
using namespace ISysUtil::P;
/* ------------------------------------------------------------------------- */
namespace P {                          // Start of public module namespace
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
  if(FStream fFile{ LuaUtilGetCppFile(lS, 1, "File"), FStream::FM_R_T })
  { // Read file and store in string
    const string strData{ fFile.FStreamReadStringSafe() };
    // If no error occured then return the data read
    if(!fFile.FStreamFError()) return LuaUtilPushStr(lS, strData);
    // Error occured so return error message
    LuaUtilPushBool(lS, false);
    LuaUtilPushStr(lS, fFile.FStreamGetErrStr());
  } // Open failed?
  else
  { // Error occured so return error message
    LuaUtilPushBool(lS, false);
    LuaUtilPushStr(lS, fFile.FStreamGetErrStr());
  }
}
/* -- Write data a file in one go ------------------------------------------ */
void FileWriteData(lua_State*const lS, const string &strFile,
  const FStream::Mode mMode, const void*const vpPtr, const size_t stBytes)
{ // Open file with the specified mode and if successful?
  if(FStream fFile{ strFile, mMode })
  { // Success if zero size
    if(!stBytes) return LuaUtilPushBool(lS, true);
    // Write the data and check if we wrote enough data?
    const size_t stWritten = fFile.FStreamWrite(vpPtr, stBytes);
    if(stWritten == stBytes) return LuaUtilPushInt(lS, stWritten);
    // Error occured so return failure with error message
    LuaUtilPushBool(lS, false);
    LuaUtilPushStr(lS, fFile.FStreamFError() ? fFile.FStreamGetErrStr() :
      StrFormat("Only $ of $ bytes written", stWritten, stBytes));
  } // Failed so return reason
  else
  { // Error occured so return failure with error message
    LuaUtilPushBool(lS, false);
    LuaUtilPushStr(lS, fFile.FStreamGetErrStr());
  }
}
/* -- Append string to file in one go -------------------------------------- */
void FileAppendString(lua_State*const lS)
{ // Get filename, string to write from Lua and send it to writer function
  const string strFile{ LuaUtilGetCppFile(lS, 1, "File") },
               strWhat{ LuaUtilGetCppStr(lS, 2, "String") };
  FileWriteData(lS,
    strFile, FStream::FM_A_B, strWhat.data(), strWhat.length());
}
/* -- Write string to file in one go --------------------------------------- */
void FileWriteString(lua_State*const lS)
{ // Get filename, string to write from Lua and send it to writer function
  const string strFile{ LuaUtilGetCppFile(lS, 1, "File") },
               strWhat{ LuaUtilGetCppStr(lS, 2, "String") };
  FileWriteData(lS,
    strFile, FStream::FM_W_B, strWhat.data(), strWhat.length());
}
/* -- Append data to file in one go ---------------------------------------- */
void FileAppendBlock(lua_State*const lS)
{ // Get filename, string to write from Lua and send it to writer function
  const string strFile{ LuaUtilGetCppFile(lS, 1, "File") };
  const DataConst &dcWhat = *LuaUtilGetPtr<Asset>(lS, 2, "Asset");
  FileWriteData(lS, strFile, FStream::FM_A_B, dcWhat.Ptr(), dcWhat.Size());
}
/* -- Write data to file in one go ----------------------------------------- */
void FileWriteBlock(lua_State*const lS)
{ // Get filename, string to write from Lua and send it to writer function
  const string strFile{ LuaUtilGetCppFile(lS, 1, "File") };
  const DataConst &dcWhat = *LuaUtilGetPtr<Asset>(lS, 2, "Asset");
  FileWriteData(lS, strFile, FStream::FM_W_B, dcWhat.Ptr(), dcWhat.Size());
}
/* ------------------------------------------------------------------------- */
}                                      // End of public module namespace
/* ------------------------------------------------------------------------- */
}                                      // End of private module namespace
/* == EoF =========================================================== EoF == */

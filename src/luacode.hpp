/* == LUACODE.HPP ========================================================== */
/* ######################################################################### */
/* ## MS-ENGINE              Copyright (c) MS-Design, All Rights Reserved ## */
/* ######################################################################### */
/* ## This file defines the execution and caching system of lua code      ## */
/* ######################################################################### */
/* ========================================================================= */
#pragma once                           // Only one incursion allowed
/* ------------------------------------------------------------------------- */
namespace IfLuaCode {                  // Start of module namespace
/* -- Includes ------------------------------------------------------------- */
using namespace IfSql;                 // Using sql namespace
/* -- Consts --------------------------------------------------------------- */
static enum LuaCache { LCC_OFF, LCC_FULL, LCC_MINIMUM, LCC_MAX } lcSetting;
/* -- Set lua cache setting ------------------------------------------------ */
static CVarReturn LuaCodeSetCache(const unsigned int uiVal)
  { return CVarSimpleSetIntNGE(lcSetting,
      static_cast<LuaCache>(uiVal), LCC_MAX); }
/* -- Callback for lua_dump ------------------------------------------------ */
namespace LuaCodeDumpHelper
{ /* -- Memory blocks structure for dump function -------------------------- */
  struct MemData { MemoryList mlBlocks; size_t stTotal; };
  /* -- The callback function ---------------------------------------------- */
  static int PopulateMemoryListCallback(lua_State*const,
    const void*const vpAddr, const size_t stSize, void*const vpUser)
  { // Get memory data
    MemData &mdData = *reinterpret_cast<MemData*>(vpUser);
    // Make a new memory block
    mdData.mlBlocks.push_back({ stSize, vpAddr });
    // Add to size total
    mdData.stTotal += stSize;
    // Return success
    return 0;
  }
};/* -- Compile a function to binary --------------------------------------- */
static Memory LuaCodeCompileFunction(lua_State*const lS, const bool bDebug)
{ // Include utility namespace
  using namespace LuaCodeDumpHelper;
  // Memory blocks to write
  MemData mdData{ {}, 0 };
  // Dump the code to binary and if error occured?
  if(lua_dump(lS, PopulateMemoryListCallback, &mdData, bDebug ? 0 : 1))
    XC("Failure dumping function!");
  // Error if no blocks
  if(mdData.mlBlocks.empty() || !mdData.stTotal) XC("Empty function dump!");
  // If there is only one block?
  if(mdData.mlBlocks.size() == 1)
  { // Get first memory block and if position not reached? Thats not right!
    Memory &mbData = *mdData.mlBlocks.begin();
    if(mdData.stTotal != mbData.Size())
      XC("Not enough bytes written to binary!",
        "Written", mbData.Size(), "Needed", mdData.stTotal);
    // Return memory block
    return std::move(mbData);
  } // Make full memory block
  Memory mbData{ mdData.stTotal };
  // Position to write to
  size_t stPos = 0;
  // Until no blocks level or position reaches end
  while(!mdData.mlBlocks.empty() && stPos < mdData.stTotal)
  { // Get memory block and write it to our big final black
    Memory &mbBlock = *mdData.mlBlocks.begin();
    mbData.WriteBlock(stPos, mbBlock);
    // Incrememnt position and erase the block
    stPos += mbBlock.Size();
    mdData.mlBlocks.erase(mdData.mlBlocks.cbegin());
  } // Error if position not reached
  if(stPos != mdData.stTotal)
    XC("Not enough bytes written to binary!",
      "Written", stPos, "Needed", mdData.stTotal,
      "Remain", mdData.mlBlocks.size());
  // Return compiled memory
  return mbData;
}
/* -- Compile a buffer ----------------------------------------------------- */
static void LuaCodeDoCompileBuffer(lua_State*const lS, const char *cpBuf,
  size_t stSize, const string &strRef)
{ // Compile the specified script and capture result
  switch(const int iR = luaL_loadbuffer(lS, cpBuf, stSize, strRef.c_str()))
  { // No error? Execute functions and log success. We should always be in
    // the sandbox, so no pcall is needed.
    case 0: return;
    // Syntax error? Show error
    case LUA_ERRSYNTAX: XC(Append("Compile error! > ", GetAndPopString(lS)));
    // Not enough memory?
    case LUA_ERRMEM: XC("Not enough memory executing script!");
    // Unknown error (never get here, but only to stop compiler whining)
    default: XC("Unknown error executing script!", "Result", iR);
  }
}
/* -- Compile a buffer ----------------------------------------------------- */
static void LuaCodeCompileBuffer(lua_State*const lS, const char*const cpBuf,
  const size_t stSize, const string &strRef)
{ // If code is binary or lua cache is disable or has default reference?
  if((stSize >= sizeof(uint32_t) &&
     *reinterpret_cast<const uint32_t*>(cpBuf) == 0x61754C1B) ||
    lcSetting == LCC_OFF || strRef.empty())
  { // Do compile the buffer
    LuaCodeDoCompileBuffer(lS, cpBuf, stSize, strRef);
    // Do nothing else
    return;
  } // Get checksum of module
  const unsigned int uiCRC = CrcCalc(cpBuf, stSize);
  // Check if we have cached this in the sql database and if we have?
  if(cSql->ExecuteAndSuccess("SELECT D from L WHERE R=? AND C=?",
    { Sql::Cell(strRef), Sql::Cell(uiCRC) }))
  { // Get records and if we have results?
    const Sql::Result &vData = cSql->GetRecords();
    if(!vData.empty())
    { // If we should show the rows affected. This is sloppy but sqllite
      // doesn't support resetting sqlite3_changes result yet :(
      const Sql::Records &sslPairs = *vData.cbegin();
      const Sql::RecordsIt smmI(sslPairs.find("D"));
      if(smmI != sslPairs.cend())
      { // Get value and if its a blob? Set new buffer to load
        const Sql::DataListItem &mbO = smmI->second;
        if(mbO.iT == SQLITE_BLOB)
        { // Cache is valid
          cLog->LogDebugExSafe(
            "LuaCode will use cached version of '$'[$]($$)!",
              strRef, stSize, hex, uiCRC);
          // Do compile the buffer
          LuaCodeDoCompileBuffer(lS, mbO.Ptr<char>(), mbO.Size(), strRef);
          // Done
          return;
        } // Invalid type
        else cLog->LogWarningExSafe(
          "LuaCode will recompile '$'[$]($$$) as it has a bad type of $!",
            strRef, stSize, hex, uiCRC, dec, mbO.iT);
      } // Invalid keyname?
      else cLog->LogWarningExSafe(
        "LuaCode will recompile '$'[$]($$) as it has a bad keyname!",
        strRef, stSize, hex, uiCRC);
    } // No results
    else cLog->LogDebugExSafe(
      "LuaCode will compile '$'[$]($$) for the first time!",
        strRef, stSize, hex, uiCRC);
  } // Error reading database
  else
  { // Try to rebuild table
    cSql->LuaCacheDropTable();
    cSql->LuaCacheCreateTable();
  } // Do compile the buffer
  LuaCodeDoCompileBuffer(lS, cpBuf, stSize, strRef);
  // Compile the function
  Memory mbData{ LuaCodeCompileFunction(lS, lcSetting == LCC_FULL) };
  // Send to sql database and return if succeeded
  if(cSql->ExecuteAndSuccess(
       "INSERT or REPLACE into L(C,T,R,D) VALUES(?,?,?,?)",
    { Sql::Cell(uiCRC), Sql::Cell(cmSys.GetTimeNS<sqlite3_int64>()),
      Sql::Cell(strRef), Sql::Cell(mbData) })) return;
  // Show error
  cLog->LogWarningExSafe(
    "LuaCode failed to store cache for '$' because $ ($)!",
    strRef, cSql->GetErrorStr(), cSql->GetError());
  // Try to rebuild table
  cSql->LuaCacheDropTable();
  cSql->LuaCacheCreateTable();
}
/* -- Compile a memory block ----------------------------------------------- */
static void LuaCodeCompileBlock(lua_State*const lS, const DataConst &dcData,
  const string &strRef)
    { LuaCodeCompileBuffer(lS, dcData.Ptr<char>(), dcData.Size(), strRef); }
/* -- Execute specified block ---------------------------------------------- */
static void LuaCodeExecuteBlock(lua_State*const lS, const DataConst &dcData,
  const int iRet, const string &strRef)
    { LuaCodeCompileBlock(lS, dcData, strRef); CallFunc(lS, iRet); }
/* -- Compile a string ----------------------------------------------------- */
static void LuaCodeCompileString(lua_State*const lS, const string &strBuf,
  const string &strRef)
    { LuaCodeCompileBuffer(lS, strBuf.data(), strBuf.length(), strRef); }
/* -- Execute specified string in unprotected ------------------------------ */
static void LuaCodeExecuteString(lua_State*const lS, const string &strCode,
  const int iRet, const string &strRef)
    { LuaCodeCompileString(lS, strCode, strRef); CallFunc(lS, iRet); }
/* -- Compile contents of a file (returns function on lua stack) ----------- */
static void LuaCodeCompileFile(lua_State*const lS, const FileMap &fScript)
  { LuaCodeCompileBuffer(lS, fScript.Ptr<char>(), fScript.Size(),
      fScript.IdentGetCStr()); }
/* -- Execute specified file ----------------------------------------------- */
static void LuaCodeExecuteFile(lua_State*const lS, const FileMap &fScript,
  const int iRet=0)
    { LuaCodeCompileFile(lS, fScript); CallFunc(lS, iRet); }
/* -- Same as ExecString() but returns uiRet (for LuaLib) ------------------ */
static int LuaCodeExecStringAndRets(lua_State*const lS,
  const string &strCode, const int iRet, const string &strName)
    { LuaCodeExecuteString(lS, strCode, iRet, strName); return iRet; }
/* -- Same as ExecString() but returns iRet (for LuaLib) ------------------- */
static int LuaCodeExecBlockAndRets(lua_State*const lS,
  const DataConst &dcData, const int iRet, const string &strName)
    { LuaCodeExecuteBlock(lS, dcData, iRet, strName); return iRet; }
/* -- Load file and execute script that may be binary ---------------------- */
static void LuaCodeExecFile(lua_State*const lS, const string &strFilename,
  const int iRet=0)
    { LuaCodeExecuteFile(lS, AssetExtract(strFilename), iRet); }
/* -- Copmile file and execute script that may be binary ------------------- */
static void LuaCodeCompileFile(lua_State*const lS, const string &strFilename)
  { LuaCodeCompileFile(lS, AssetExtract(strFilename)); }
/* -- Copmile function to binary ------------------------------------------- */
static void LuaCodeCompileFunction(lua_State*const lS)
{ // Must have two parameters
  CheckParams(lS, 2);
  // Debug parameter
  const bool bDebug = GetBool(lS, 1, "Debug");
  // Second parameter must be function
  CheckFunction(lS, 2, "Function");
  // Compile the function
  Memory mbData{ LuaCodeCompileFunction(lS, bDebug) };
  // Return a newly created asset
  ClassCreate<Asset>(lS, "Asset")->SwapMemory(std::move(mbData));
}
/* -- Same as ExecFile() but returns iRet (for LuaLib) --------------------- */
static int LuaCodeExecFileAndRets(lua_State*const lS,
  const string &strFilename, const int iRet)
    { LuaCodeExecFile(lS, strFilename, iRet); return iRet; }
/* ------------------------------------------------------------------------- */
};                                     // End of module namespace
/* == EoF =========================================================== EoF == */

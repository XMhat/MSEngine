/* == ASSET.HPP ============================================================ **
** ######################################################################### **
** ## MS-ENGINE              Copyright (c) MS-Design, All Rights Reserved ## **
** ######################################################################### **
** ## This the file handles the .adb (7-zip) archives and the extraction  ## **
** ## and decompression of files.                                         ## **
** ######################################################################### **
** ========================================================================= */
#pragma once                           // Only one incursion allowed
/* ------------------------------------------------------------------------- */
namespace IAsset {                     // Start of private module namespace
/* -- Dependencies --------------------------------------------------------- */
using namespace IArchive::P;           using namespace IASync::P;
using namespace ICodec::P;             using namespace ICollector::P;
using namespace ICVarDef::P;           using namespace IDir::P;
using namespace IError::P;             using namespace IEvtMain::P;
using namespace IIdent::P;             using namespace IFlags;
using namespace ILog::P;               using namespace ILuaUtil::P;
using namespace IMemory::P;            using namespace IStd::P;
using namespace IString::P;            using namespace ISysUtil::P;
/* ------------------------------------------------------------------------- */
namespace P {                          // Start of public module namespace
/* -- Typedefs ------------------------------------------------------------- */
BUILD_FLAGS(Asset,                     // Asset loading flags
  /* -- Commands ----------------------------------------------------------- */
  // Leave the file loaded as is?      Decode the specified block?
  CD_NONE                {0x00000000}, CD_DECODE              {0x00000001},
  // Copy the specified block?         Encrypt the specified block?
  CD_ENCODE_RAW          {0x00000002}, CD_ENCODE_AES          {0x00000004},
  // Deflate the specified block?      Compress the specified block?
  CD_ENCODE_ZLIB         {0x00000008}, CD_ENCODE_LZMA         {0x00000010},
  // Deflate+encrypt specified block?  Compress+encrypt specified block?
  CD_ENCODE_ZLIBAES      {0x00000020}, CD_ENCODE_LZMAAES      {0x00000040},
  /* -- Options ------------------------------------------------------------ */
  // Fastest compression (less mem)?   Fast compression?
  CD_LEVEL_FASTEST       {0x08000000}, CD_LEVEL_FAST          {0x10000000},
  // Medium compression?               Good compression?
  CD_LEVEL_MODERATE      {0x20000000}, CD_LEVEL_SLOW          {0x40000000},
  // Maximum compression (more mem)?
  CD_LEVEL_SLOWEST       {0x80000000},
  /* -- All options -------------------------------------------------------- */
  CD_MASK{ CD_DECODE|CD_ENCODE_RAW|CD_ENCODE_AES|CD_ENCODE_ZLIB|CD_ENCODE_LZMA|
           CD_ENCODE_ZLIBAES|CD_ENCODE_LZMAAES|CD_LEVEL_FASTEST|CD_LEVEL_FAST|
           CD_LEVEL_MODERATE|CD_LEVEL_SLOW|CD_LEVEL_SLOWEST }
);/* == Asset collector class for collector and custom variables =========== */
BEGIN_ASYNCCOLLECTOREX(Assets, Asset, CLHelperUnsafe,
/* -- Asset collector variables -------------------------------------------- */
bool               bOverride;          /* Allow load of external files      */\
SafeSizeT          stPipeBufSize;      /* Pipe buffer size for execute      */\
); /* ---------------------------------------------------------------------- */
/* -- Function to load a file locally -------------------------------------- */
static FileMap AssetLoadFromDisk(const string &strFile)
{ // Open it and sending full-load flag and if succeeded?
  if(FileMap fmFile{ strFile })
  { // Put in the log that we loaded the file successfully
    cLog->LogDebugExSafe("Assets mapped resource '$'[$]!",
      strFile, fmFile.Size());
    // Return file class to caller
    return fmFile;
  } // Failed so throw exception
  XCL("Failed to open local resource!", "File",  strFile, "Path", DirGetCWD());
}
/* -- Function to search local directory then archives for a file ---------- */
static FileMap AssetExtract(const string &strFile)
{ // Do we have the permission to load external files?
  if(cAssets->bOverride)
  { // Does the file exist on disk?
    if(DirLocalFileExists(strFile)) return AssetLoadFromDisk(strFile);
    // No need to try to check archives if there are no archives
    if(cArchives->CollectorEmpty())
      XCL("Local resource not found!", "File",  strFile, "Path", DirGetCWD());
  } // Loading from archives only. No point continuing if there aren't any.
  else if(cArchives->CollectorEmpty())
    XC("No route to resource!", "File", strFile);
  // Try to extract file from archives and return it if succeeded
  if(FileMap fmFile{ ArchiveExtract(strFile) }) return fmFile;
  // Or show archives list
  XCL("Failed to open resource from archives!",
    "File",     strFile,
    "Count",    cArchives->CollectorCount(),
    "Archives", ArchiveGetNames());
}
/* == Asset object class =================================================== */
BEGIN_MEMBERCLASS(Assets, Asset, ICHelperUnsafe),
  /* -- Base classes ------------------------------------------------------- */
  public AsyncLoader<Asset>,           // For loading assets off main thread
  public Memory,                       // Memory storage for this asset
  public Lockable,                     // Lua garbage collector instruction
  public AssetFlags                    // Asset settings
{ /* -------------------------------------------------------------- */ private:
  void SwapAsset(Asset &aOther)
  { // Swap settings flags
    FlagSwap(aOther);
    SwapMemory(StdMove(aOther));
    LockSwap(aOther);
    IdentSwap(aOther);
    CollectorSwapRegistration(aOther);
  }
  /* -- Perform decoding --------------------------------------------------- */
  template<class Codec>void CodecExec(FileMap &fC, size_t stLevel=0)
    { SwapMemory(Block<Codec>{ fC, stLevel }); }
  /* -- Perform decoding converting flags to compression level ------------- */
  template<class Codec>void CodecExecEx(FileMap &fC)
    { CodecExec<Codec>(fC, FlagIsSet(CD_LEVEL_FASTEST)  ? 1 :
                          (FlagIsSet(CD_LEVEL_FAST)     ? 3 :
                          (FlagIsSet(CD_LEVEL_MODERATE) ? 5 :
                          (FlagIsSet(CD_LEVEL_SLOW)     ? 7 :
                          (FlagIsSet(CD_LEVEL_SLOWEST)  ? 9 : 1))))); }
  /* -- Load asset from memory ------------------------------------- */ public:
  void AsyncReady(FileMap &fC)
  { // Guest wants data put into a raw magic block (no user flags)
    if(FlagIsSet(CD_ENCODE_RAW)) CodecExec<RAWEncoder>(fC);
    // Guest wants data encrypted into a magic block (no user flags)
    else if(FlagIsSet(CD_ENCODE_AES)) CodecExec<AESEncoder>(fC);
    // Guest wants data deflated into a magic block
    else if(FlagIsSet(CD_ENCODE_ZLIB)) CodecExecEx<ZLIBEncoder>(fC);
    // Guest wants data encrypted and deflated into a magic block
    else if(FlagIsSet(CD_ENCODE_ZLIBAES)) CodecExecEx<AESZLIBEncoder>(fC);
    // Guest wants data compressed into a magic block
    else if(FlagIsSet(CD_ENCODE_LZMA)) CodecExecEx<LZMAEncoder>(fC);
    // Guest wants data encrypted and compressed into a magic block
    else if(FlagIsSet(CD_ENCODE_LZMAAES)) CodecExecEx<AESLZMAEncoder>(fC);
    // Guest wants data decoded from a magic block (no user flags)
    else if(FlagIsSet(CD_DECODE)) CodecExec<CoDecoder>(fC);
    // Guest wants data untouched but we need to copy it all from the map
    else SwapMemory(fC.FileMapDecouple());
  }
  /* -- Load data from string asynchronously ------------------------------- */
  void InitAsyncString(lua_State*const lS)
  { // Must have 5 parameters (including the class pointer)
    LuaUtilCheckParams(lS, 7);
    // Check and get parameters
    const string strName{ LuaUtilGetCppStrNE(lS, 1, "Identifier") };
    Memory mData{ LuaUtilGetMBfromLStr(lS, 2, "String") };
    FlagReset(LuaUtilGetFlags(lS, 3, CD_MASK, "Flags"));
    LuaUtilCheckFuncs(lS, 4, "ErrorFunc", 5, "ProgressFunc", 6, "SuccessFunc");
    // Init the specified string as an array asynchronously
    AsyncInitArray(lS, strName, "assetstring", StdMove(mData));
  }
  /* -- Load data from array asynchronously -------------------------------- */
  void InitAsyncArray(lua_State*const lS)
  { // Must have 5 parameters (including the class pointer)
    LuaUtilCheckParams(lS, 7);
    // Check and get parameters
    const string strName{ LuaUtilGetCppStrNE(lS, 1, "Identifier") };
    Asset &aData = *LuaUtilGetPtr<Asset>(lS, 2, "Asset");
    FlagReset(LuaUtilGetFlags(lS, 3, CD_MASK, "Flags"));
    LuaUtilCheckFuncs(lS, 4, "ErrorFunc", 5, "ProgressFunc", 6, "SuccessFunc");
    // Init the specified string as an array asynchronously
    AsyncInitArray(lS, strName, "assetdata", StdMove(aData));
  }
  /* -- Load asset from file asynchronously -------------------------------- */
  void InitAsyncFile(lua_State*const lS)
  { // Need 4 parameters (class pointer was already pushed onto the stack);
    LuaUtilCheckParams(lS, 6);
    // Get and check parameters
    const string strName{ LuaUtilGetCppFile(lS, 1, "File") };
    FlagReset(LuaUtilGetFlags(lS, 2, CD_MASK, "Flags"));
    LuaUtilCheckFuncs(lS, 3, "ErrorFunc", 4, "ProgressFunc", 5, "SuccessFunc");
    // Load asset from file asynchronously
    AsyncInitFile(lS, strName, "assetfile");
  }
  /* -- Load asset from command-line --------------------------------------- */
  void InitAsyncCmdLine(lua_State*const lS)
  { // Need 4 parameters (class pointer was already pushed onto the stack);
    LuaUtilCheckParams(lS, 6);
    // Get and check parameters
    const string strCmdLine{ LuaUtilGetCppStrNE(lS, 1, "CmdLine") };
    FlagReset(LuaUtilGetFlags(lS, 2, CD_MASK, "Flags"));
    LuaUtilCheckFuncs(lS, 3, "ErrorFunc", 4, "ProgressFunc", 5, "SuccessFunc");
    // Load asset from file asynchronously
    Memory mbBlank;
    AsyncInitCmdLine(lS, strCmdLine, "CL", mbBlank);
  }
  /* -- Load asset from command-line --------------------------------------- */
  void InitAsyncCmdLineEx(lua_State*const lS)
  { // Need 5 parameters (class pointer was already pushed onto the stack);
    LuaUtilCheckParams(lS, 7);
    // Get and check parameters
    const string strCmdLine{ LuaUtilGetCppStrNE(lS, 1, "CmdLine") };
    FlagReset(LuaUtilGetFlags(lS, 2, CD_MASK, "Flags"));
    Asset &aInput = *LuaUtilGetPtr<Asset>(lS, 3, "Asset");
    LuaUtilCheckFuncs(lS, 4, "ErrorFunc", 5, "ProgressFunc", 6, "SuccessFunc");
    // Load asset from file asynchronously
    AsyncInitCmdLine(lS, strCmdLine, StrAppend("CLP", aInput.Size()), aInput);
  }
  /* -- Init from file ----------------------------------------------------- */
  void InitFile(const string &strFilename, const AssetFlagsConst &lfS)
  { // Set load flags
    FlagReset(lfS);
    // Load file normally
    SyncInitFileSafe(strFilename);
  }
  /* -- Init from string --------------------------------------------------- */
  void InitString(const string &strName, const string &strStr,
    const AssetFlagsConst &lfS)
  { // Set load flags
    FlagReset(lfS);
    // Load file as array converted from string
    SyncInitArray(strName, Memory{ strStr });
  }
  /* -- Init from string --------------------------------------------------- */
  void InitArray(const string &strN, Asset &&aSrc, const AssetFlagsConst &lfS)
  { // Set load flags
    FlagReset(lfS);
    // Set filename and flags
    SyncInitArray(strN, StdMove(aSrc));
  }
  /* -- Move assignment ---------------------------------------------------- */
  Asset& operator=(Asset &&aOther) { SwapAsset(aOther); return *this; }
  /* -- Move constructor --------------------------------------------------- */
  Asset(Asset &&aOther) : Asset() { SwapAsset(aOther); }
  /* -- For loading via LUA ------------------------------------------------ */
  Asset(void) :
    /* -- Initialisers ----------------------------------------------------- */
    ICHelperAsset{ *cAssets },         // Initially unregistered
    IdentCSlave{ cParent.CtrNext() },  // Initialise identification number
    AsyncLoader<Asset>{ this,          // Initialise async class with this
      EMC_MP_ASSET },                  // ...and the event id for it.
    AssetFlags{ CD_NONE }              // Np asset load flags initially
    /* -- No code ---------------------------------------------------------- */
    { }
  /* -- Destructor --------------------------------------------------------- */
  ~Asset(void) { AsyncCancel(); }
  /* ----------------------------------------------------------------------- */
  DELETECOPYCTORS(Asset)               // Disable copy constructor and operator
};/* ======================================================================= */
/* -- End-of-collector ---------------------------------------------- End -- */
END_ASYNCCOLLECTOR(Assets, Asset, ASSET, bOverride(false));
/* -- Class to help enumerate files ---------------------------------------- */
struct AssetList :
  /* -- Dependents --------------------------------------------------------- */
  public StrSet                        // Sorted storage for filenames
{ /* -- Direct access using class variable name which returns if !empty ---- */
  operator bool(void) const { return !empty(); }
  /* -- Cast down to StrSet ------------------------------------------------ */
  StrSet &ToStrSet(void) { return *this; }
  /* -- Return files in directories ---------------------------------------- */
  AssetList(const string &strDir, const bool bOnlyDirs) :
    /* -- Initialisers ----------------------------------------------------- */
    StrSet{ bOnlyDirs ? StdMove(Dir{ strDir }.DirsToSet()) :
                        StdMove(Dir{ strDir }.FilesToSet()) }
    /* -- Add archive files to list ---------------------------------------- */
    { ArchiveEnumerate(strDir, cCommon->Blank(), bOnlyDirs, *this); }
  /* -- Return files in directories with extension matching ---------------- */
  AssetList(const string &strDir, const string &strExt, const bool bOnlyDirs) :
    /* -- Initialisers ----------------------------------------------------- */
    StrSet{ bOnlyDirs ? StdMove(Dir{ strDir, strExt }.DirsToSet()) :
                        StdMove(Dir{ strDir, strExt }.FilesToSet()) }
    /* -- Add archive files to list ---------------------------------------- */
    { ArchiveEnumerate(strDir, strExt, bOnlyDirs, *this); }
};/* ----------------------------------------------------------------------- */
/* -- Look if a file exists ------------------------------------------------ */
static bool AssetExists(const string &strFile)
  { return DirLocalFileExists(strFile) || ArchiveFileExists(strFile); }
/* -- Allow external file access ------------------------------------------- */
static CVarReturn AssetSetFSOverride(const bool bState)
  { return CVarSimpleSetInt(cAssets->bOverride, bState); }
/* -- Set pipe buffer size ------------------------------------------------- */
static CVarReturn AssetSetPipeBufferSize(const size_t stSize)
  { return CVarSimpleSetIntNLG(cAssets->stPipeBufSize, stSize,
      static_cast<size_t>(1), static_cast<size_t>(4096)); }
/* -- Set pipe buffer size ------------------------------------------------- */
static size_t AssetGetPipeBufferSize(void) { return cAssets->stPipeBufSize; }
/* ------------------------------------------------------------------------- */
}                                      // End of public module namespace
/* ------------------------------------------------------------------------- */
}                                      // End of private module namespace
/* == EoF =========================================================== EoF == */

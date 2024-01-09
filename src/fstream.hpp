/* == FSTREAM.HPP ========================================================== **
** ######################################################################### **
** ## MS-ENGINE              Copyright (c) MS-Design, All Rights Reserved ## **
** ######################################################################### **
** ## This module is a simple C++ wrapper for C file stream functions.    ## **
** ######################################################################### **
** ========================================================================= */
#pragma once                           // Only one incursion allowed
/* ------------------------------------------------------------------------- */
namespace IFStream {                   // Start of private module namespace
/* ------------------------------------------------------------------------- */
using namespace IError::P;             using namespace IIdent::P;
using namespace IStd::P;               using namespace IString::P;
using namespace IMemory::P;            using namespace IUtil::P;
/* ------------------------------------------------------------------------- */
namespace P {                          // Start of public module namespace
/* -- FStream Class -------------------------------------------------------- */
class FStream :
  /* -- Base classes ------------------------------------------------------- */
  public Ident                         // Contains filename string
{ /* -- Public typedefs -------------------------------------------- */ public:
  enum Mode                            // Open types allowed
  { /* --------------------------------------------------------------------- */
    FM_R_T,                            // [00] "rt"  Read|Exists|Text
    FM_W_T,                            // [01] "wt"  Write|New|Trunc|Text
    FM_A_T,                            // [02] "at"  Write|Append|New|Text
    FM_R_P_T,                          // [03] "r+t" Read|Write|Exists|Text
    FM_W_P_T,                          // [04] "w+t" Read|Write|New|Trunc|Text
    FM_A_P_T,                          // [05] "a+t" Read|Write|Append|New|Text
    FM_R_B,                            // [06] "rb"  Read|Exists|Binary
    FM_W_B,                            // [07] "wb"  Write|New|Trunc|Binary
    FM_A_B,                            // [08] "ab"  Write|Append|New|Binary
    FM_R_P_B,                          // [09] "r+b" Read|Write|Exists|Binary
    FM_W_P_B,                          // [10] "w+b" Read|Write|New|Trunc|Binar
    FM_A_P_B,                          // [11] "a+b" Read|Append|New|Binary
    /* --------------------------------------------------------------------- */
    FM_MAX                             // [12] Maximum number of modes
  };/* -- Private variables --------------------------------------- */ private:
  FILE            *fStream;            // Stream handle
  int              iErrNo;             // Stored error number
  /* -- Accept a file stream from DoOpen() --------------------------------- */
  bool FStreamDoAccept(const string &strFile, FILE*const fNew)
  { // Close original file if opened
    FStreamCloseSafe();
    // Set the new handle and file name
    FStreamSetHandle(fNew);
    IdentSet(strFile);
    // Success!
    return true;
  }
  /* -- Error number wrapper handle ---------------------------------------- */
  template<typename AnyType>AnyType FStreamErrNoWrapper(AnyType atVal)
    { iErrNo = errno; return atVal; }
  /* -- Open a file -------------------------------------------------------- */
  bool FStreamDoOpen(const string &strFile, const Mode mMode)
  { // Obviously Windows has to be different from everyone else!
#if defined(WINDOWS)                   // Using windows?
    // The mode supported (in unicode)
    static const array<const wchar_t*const,FM_MAX>cplModes{
      L"rt", L"wt", L"at", L"r+t", L"w+t", L"a+t",
      L"rb", L"wb", L"ab", L"r+b", L"w+b", L"a+b"
    };
    // Send it to the Windows file open call and accept it if successful
    if(FILE*const fNew =
      FStreamErrNoWrapper(_wfsopen(UTFtoS16(strFile).c_str(),
        cplModes[mMode], _SH_DENYWR)))
          return FStreamDoAccept(strFile, fNew);
#else                                  // Using linux or MacOS?
    // The mode supported (in ansi string)
    static const array<const char*const,FM_MAX>cplModes{
      "rt", "wt", "at", "r+t", "w+t", "a+t",
      "rb", "wb", "ab", "r+b", "w+b", "a+b"
    };
    // Send it to the posix file open call and accept it if successful
    if(FILE*const fNew =
      FStreamErrNoWrapper(fopen(strFile.c_str(), cplModes[mMode])))
        return FStreamDoAccept(strFile, fNew);
#endif                                 // Operating system check
    // Failed
    return false;
  }
  /* -- Retrun true if internal stream or stream closed successfully ------- */
  bool FStreamDoClose(void)
    { return !FStreamErrNoWrapper(fclose(FStreamGetCtx())); }
  /* -- Clear file handle -------------------------------------------------- */
  void FStreamClearHandle(void) { FStreamSetHandle(nullptr); }
  /* -- Set handle ---------------------------------------------- */ protected:
  void FStreamSetHandle(FILE*const fpS) { fStream = fpS; }
  /* -- Return file stream handle ---------------------------------- */ public:
  FILE *FStreamGetCtx(void) const { return fStream; }
  /* -- Returns true if handle is internal --------------------------------- */
  bool FStreamIsHandleStd(void) { return FStreamGetCtx() == stdin ||
                                         FStreamGetCtx() == stdout ||
                                         FStreamGetCtx() == stderr; }
  /* -- Get file stream descriptor ----------------------------------------- */
  int FStreamGetFd(void) const { return StdFileNo(FStreamGetCtx()); }
  int FStreamGetFdSafe(void) const
    { return FStreamOpened() ? FStreamGetFd() : EOF; }
  /* -- Swap stream handle and filename ------------------------------------ */
  void FStreamSwap(FStream &fsOther)
    { swap(fStream, fsOther.fStream); IdentSwap(fsOther); }
  /* -- File is opened or closed?  ----------------------------------------- */
  bool FStreamOpened(void) const { return !!FStreamGetCtx(); }
  bool FStreamClosed(void) const { return !FStreamOpened(); }
  /* -- Direct access using class variable name which returns opened ------- */
  operator bool(void) const { return FStreamOpened(); }
  /* -- Return last error nuumber ------------------------------------------ */
  int FStreamGetErrNo(void) const { return iErrNo; }
  const string FStreamGetErrStr(void) const { return StrFromErrNo(iErrNo); }
  /* -- Return handle to stream -------------------------------------------- */
  int FStreamGetID(void) const { return StdFileNo(FStreamGetCtx()); }
  int FStreamGetIDSafe(void) const
    { return FStreamOpened() ? FStreamGetID() : 0; }
  /* -- Set current file position ------------------------------------------ */
  bool FStreamSetPosition(const int64_t lPos, const int iMode)
    { return !FStreamErrNoWrapper(StdFSeek(FStreamGetCtx(), lPos, iMode)); }
  bool FStreamSetPositionSafe(const int64_t lPos, const int iMode)
    { return FStreamOpened() ? FStreamSetPosition(lPos, iMode) : EOF; }
  /* -- Return current file position --------------------------------------- */
  int64_t FStreamGetPosition(void)
    { return FStreamErrNoWrapper(StdFTell(FStreamGetCtx())); }
  int64_t FStreamGetPositionSafe(void)
    { return FStreamOpened() ? FStreamGetPosition() : EOF; }
  /* -- Flush buffered file data to disk ----------------------------------- */
  bool FStreamFlush(void) const { return !fflush(FStreamGetCtx()); }
  bool FStreamFlushSafe(void) const
    { return FStreamOpened() ? FStreamFlush() : false; }
  /* -- File stream is set to error status? -------------------------------- */
  bool FStreamFError(void) const { return !!ferror(FStreamGetCtx()); }
  bool FStreamFErrorSafe(void) const
    { return FStreamOpened() ? FStreamFError() : true; }
  /* -- File stream is ready to be read or written ------------------------- */
  bool FStreamIsReadyRead(void) const { return !FStreamFErrorSafe(); }
  bool FStreamIsNotReadyRead(void) const { return !FStreamIsReadyRead(); }
  bool FStreamIsReadyWrite(void) const
    { return FStreamIsReadyRead() && FStreamIsNotEOF(); }
  bool FStreamIsNotReadyWrite(void) const { return !FStreamIsReadyWrite(); }
  /* -- Is position at end of the file? ------------------------------------ */
  bool FStreamIsEOF(void) const { return feof(FStreamGetCtx()); }
  bool FStreamIsEOFSafe(void) const
    { return FStreamClosed() || FStreamIsEOF(); }
  bool FStreamIsNotEOF(void) const { return !FStreamIsEOF(); }
  bool FStreamIsNotEOFSafe(void) const { return !FStreamIsEOFSafe(); }
  /* -- Read file information ---------------------------------------------- */
  bool FStreamStat(StdFStatStruct &sData)
    { return !FStreamErrNoWrapper(StdHStat(StdFileNo(FStreamGetCtx()),
        &sData)); }
  bool FStreamStatSafe(StdFStatStruct &sData)
    { return FStreamOpened() ? FStreamStat(sData) : false; }
  /* -- Read data from file ------------------------------------------------ */
  size_t FStreamRead(void*const vpBuffer, const size_t stBytes)
    { return FStreamErrNoWrapper(fread(vpBuffer,
        1, stBytes, FStreamGetCtx())); }
  size_t FStreamReadSafe(void*const vpBuffer, const size_t stBytes)
    { return FStreamIsReadyRead() ? FStreamRead(vpBuffer, stBytes) : 0; }
  /* -- Read line string from file ----------------------------------------- */
  const string FStreamReadString(const size_t stBytes)
  { // Setup string for buffer and read into it
    string strRead; strRead.resize(stBytes);
    strRead.resize(FStreamRead(const_cast<char*>(strRead.data()),
      strRead.length()));
    // Return string
    return strRead;
  }
  const string FStreamReadStringSafe(const size_t stBytes)
    { return FStreamIsReadyRead() && stBytes ?
        FStreamReadString(stBytes) : cCommon->Blank(); }
  const string FStreamReadStringSafe(void)
  { // Return empty string if closed or end-of file
    if(!FStreamIsReadyRead()) return {};
    // Get string and return default if zero
    const size_t stBytes = FStreamSizeT();
    if(!stBytes) return {};
    // Return read string
    return FStreamReadString(stBytes);
  }
  /* -- Write data to file ------------------------------------------------- */
  size_t FStreamWrite(const void*const vpPtr, const size_t stBytes)
    { return FStreamErrNoWrapper(fwrite(vpPtr,
        1, stBytes, FStreamGetCtx())); }
  size_t FStreamWriteSafe(const void*const vpPtr, const size_t stBytes)
    { return FStreamIsReadyWrite() ? FStreamWrite(vpPtr, stBytes) : 0; }
  /* -- Write memory block to file ----------------------------------------- */
  size_t FStreamWriteBlock(const DataConst &dcSrc)
    { return FStreamWrite(dcSrc.Ptr(), dcSrc.Size()); }
  size_t FStreamWriteBlockSafe(const DataConst &dcSrc)
    { return FStreamIsReadyWrite() && !dcSrc.Empty() ?
        FStreamWrite(dcSrc.Ptr(), dcSrc.Size()) : 0; }
  /* -- Write a string to the file ----------------------------------------- */
  size_t FStreamWriteString(const string &strString)
    { return FStreamWrite(strString.data(), strString.length()); }
  size_t FStreamWriteStringSafe(const string &strString)
    { return FStreamIsReadyWrite() && !strString.empty() ?
        FStreamWrite(strString.data(), strString.length()) : 0; }
  /* -- Read entire file without knowing the size of the file -------------- */
  const string FStreamReadStringChunked(const size_t stBytes=4096)
  { // Stream to write strings to
    ostringstream osS;
    // Loop point
    ContinueReadingStrings:
    // Read the chunk and break if at end of file or error
    const string strChunk{ FStreamReadStringSafe(stBytes) };
    if(strChunk.empty()) return osS.str();
    // Accumulate the size
    osS << StdMove(strChunk);
    // Read another string
    goto ContinueReadingStrings;
  }
  /* -- Read entire file without knowing the size of the file -------------- */
  const string FStreamReadStringChunkedSafe(const size_t stBytes=4096)
    { return FStreamIsReadyRead() && stBytes ?
        FStreamReadStringChunked(stBytes) : cCommon->Blank(); }
  /* -- Read data and return memory block ---------------------------------- */
  Memory FStreamReadBlock(const size_t stBytes)
  { // Allocate initial memory of expected bytes to read
    Memory mOut{ stBytes };
    // Read file data into buffer and resize buffer to the amount acteally read
    mOut.Resize(FStreamRead(mOut.Ptr(), stBytes));
    // Return the new size of the buffer and the number of bytes read
    return mOut;
  }
  Memory FStreamReadBlockSafe(const size_t stBytes)
    { return FStreamIsReadyRead() && stBytes ?
        FStreamReadBlock(stBytes) : Memory{}; }
  Memory FStreamReadBlockSafe(void)
  { // bail if no stream or size is zero
    if(FStreamIsNotReadyRead()) return {};
    // Get string and return default if zero
    const size_t stBytes = FStreamSizeT();
    if(!stBytes) return {};
    // Read block of file size
    return FStreamReadBlock(stBytes);
  }
  /* -- Write an static value to file -------------------------------------- */
  template<typename T>size_t FSWriteInt(const T tVar)
    { return FStreamWrite(&tVar, sizeof(tVar)); }
  template<typename T>size_t FSWriteIntSafe(const T tVar)
    { return FStreamOpened() ? FStreamWrite<T>(tVar) : 0; }
  /* -- Return file position to the beginning ------------------------------ */
  bool FStreamRewind(void) { return FStreamSetPosition(0, SEEK_SET); }
  bool FStreamRewindSafe(void)
    { return FStreamOpened() ? FStreamRewind() : false; }
  /* -- Return size of file ------------------------------------------------ */
  int64_t FStreamSize(void)
  { // Store current position, return if failed and reset to start
    const int64_t qCurrent = FStreamGetPosition();
    if(qCurrent == EOF) return EOF;
    if(!FStreamSetPosition(0, SEEK_END)) return 0;
    // Store current position (size) and restore old position
    const int64_t qSize = FStreamGetPosition();
    FStreamSetPosition(qCurrent, SEEK_SET);
    // Return size
    return qSize;
  }
  /* -- Return size as size_t so it will be clamped ------------------------ */
  size_t FStreamSizeT(void) { return UtilIntOrMax<size_t>(FStreamSize()); }
  /* -- Return size of file ------------------------------------------------ */
  int64_t FStreamSizeSafe(void) { return FStreamClosed() ? 0 : FStreamSize(); }
  /* -- Open a file without filename validation ---------------------------- */
  int FStreamOpen(const string &strFile, const Mode mMode)
  { // Try to open the file on disk and if succeeded?
    if(FStreamDoOpen(strFile, mMode)) return 0;
    // Using cmdline namespace
    using namespace ICmdLine::P;
    if(cCmdLine->IsNoHome()) return StdGetError();
    // Save error number
    const int iError = StdGetError();
    // Return original error code if persist storage fails or success
    return FStreamDoOpen(cCmdLine->GetHome(strFile), mMode) ? iError : 0;
  }
  /* -- Close file --------------------------------------------------------- */
  bool FStreamClose(void)
  { // Return result
    bool bResult;
    // Success if is a std handle
    if(FStreamIsHandleStd()) bResult = true;
    // Not a std handle so now actually close and grab result
    else bResult = FStreamDoClose();
    // Stream is now invalid so clear handle
    FStreamClearHandle();
    // Returm result
    return bResult;
  }
  bool FStreamCloseSafe(void)
    { return FStreamOpened() ? FStreamClose() : true; }
  /* -- Basic constructor with no init ------------------------------------- */
  FStream(void) :                      // No parameters
    /* -- Initialisers ----------------------------------------------------- */
    fStream(nullptr),                  // File context not initialised yet
    iErrNo(0)                          // Error number not initialised yet
    /* -- No code ---------------------------------------------------------- */
    { }
  /* -- Constructor with optional checking --------------------------------- */
  FStream(const string &strF, const Mode mMode) :
    /* -- Initialisers ----------------------------------------------------- */
    FStream{}                          // Initialise defaults
    /* -- Open the file and throw error if failed -------------------------- */
    { if(FStreamOpen(strF, mMode))
        XCL("Failed to open file!", "File", strF, "Mode", mMode); }
  /* -- Constructor with rvalue name init, no open ------------------------- */
  explicit FStream(string &&strF) :    // Movable filename string
    /* -- Initialisers ----------------------------------------------------- */
    Ident{ StdMove(strF) },            // Move filename across
    fStream(nullptr),                  // File context not initialised yet
    iErrNo(0)                          // Error number not initialised yet
    /* -- No code ---------------------------------------------------------- */
    { }
  /* -- Constructor with lvalue name init, no open ------------------------- */
  explicit FStream(const string &strF) : // Filename to set
    /* -- Initialisers ----------------------------------------------------- */
    Ident{ strF },                     // Move filename across
    fStream(nullptr),                  // File context not initialised yet
    iErrNo(0)                          // Error number not initialised yet
    /* -- No code ---------------------------------------------------------- */
    { }
  /* -- MOVE assignment constructor ---------------------------------------- */
  FStream(FStream &&fsOther) :         // Other FStream object
    /* -- Initialisers ----------------------------------------------------- */
    Ident{ StdMove(fsOther) },         // Move filename across from other
    fStream(fsOther.FStreamGetCtx()),  // Copy stream context over
    iErrNo(fsOther.FStreamGetErrNo())  // Copy error number over
    /* -- Clear other handle ----------------------------------------------- */
    { fsOther.FStreamClearHandle(); }
  /* -- Destructor (Close the file if opened) ------------------------------ */
  ~FStream(void)
    { if(FStreamOpened() && !FStreamIsHandleStd()) FStreamDoClose(); }
  /* ----------------------------------------------------------------------- */
  DELETECOPYCTORS(FStream)             // Disable copy constructor and operator
};/* ----------------------------------------------------------------------- */
}                                      // End of public module namespace
/* ------------------------------------------------------------------------- */
}                                      // End of private module namespace
/* == EoF =========================================================== EoF == */

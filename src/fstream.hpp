/* == FSTREAM.HPP ========================================================== */
/* ######################################################################### */
/* ## MS-ENGINE              Copyright (c) MS-Design, All Rights Reserved ## */
/* ######################################################################### */
/* ## This module is a simple C++ wrapper for C file stream functions.    ## */
/* ######################################################################### */
/* ========================================================================= */
#pragma once                           // Only one incursion allowed
/* ------------------------------------------------------------------------- */
namespace IfFStream {                  // Start of module namespace
/* -- Includes ------------------------------------------------------------- */
using namespace IfCmdLine;             // Using cmdline namespace
using namespace IfMemory;              // Using memory namespace
using namespace IfIdent;               // Using ident namespace
/* -- FStream Class -------------------------------------------------------- */
class FStream :
  /* -- Base classes ------------------------------------------------------- */
  public Ident                         // Contains filename string
{ /* -- Public typedefs -------------------------------------------- */ public:
  enum Mode                            // Open types allowed
  { /* --------------------------------------------------------------------- */
    FM_R_T,                            // "rt"  Read|Exists|Text
    FM_W_T,                            // "wt"  Write|New|Trunc|Text
    FM_A_T,                            // "at"  Write|Append|New|Text
    FM_R_P_T,                          // "r+t" Read|Write|Exists|Text
    FM_W_P_T,                          // "w+t" Read|Write|New|Trunc|Text
    FM_A_P_T,                          // "a+t" Read|Write|Append|New|Text
    FM_R_B,                            // "rb"  Read|Exists|Binary
    FM_W_B,                            // "wb"  Write|New|Trunc|Binary
    FM_A_B,                            // "ab"  Write|Append|New|Binary
    FM_R_P_B,                          // "r+b" Read|Write|Exists|Binary
    FM_W_P_B,                          // "w+b" Read|Write|New|Trunc|Binary
    FM_A_P_B,                          // "a+b" Read|Append|New|Binary
    /* --------------------------------------------------------------------- */
    FM_MAX                             // Maximum number of modes supported
  };/* -- Private variables --------------------------------------- */ private:
  FILE            *fStream;            // Stream handle
  /* -- Set handle ---------------------------------------------- */ protected:
  void FStreamSetHandle(FILE*const fpS) { fStream = fpS; }
  /* -- Set errno and return it ----------------------------------- */ private:
  static int FStreamSetAndReturnErrNo(const int iErr)
    { SetErrNo(iErr); return iErr; }
  /* -- Clear file handle -------------------------------------------------- */
  void FStreamClearHandle(void) { FStreamSetHandle(nullptr); }
  /* -- Accept a file stream from DoOpen() --------------------------------- */
  bool FStreamDoAccept(const string &strFile, FILE*const fNew)
  { // Close original file if opened
    FStreamDoClose();
    // Set the new handle
    FStreamSetHandle(fNew);
    // Set the file name
    IdentSet(strFile);
    // Success!
    return true;
  }
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
      _wfsopen(UTFtoS16(strFile), cplModes[mMode], _SH_DENYWR))
        return FStreamDoAccept(strFile, fNew);
#else                                  // Using linux or MacOS?
    // The mode supported (in ansi string)
    static const array<const char*const,FM_MAX>cplModes{
      "rt", "wt", "at", "r+t", "w+t", "a+t",
      "rb", "wb", "ab", "r+b", "w+b", "a+b"
    };
    // Send it to the posix file open call and accept it if successful
    if(FILE*const fNew = fopen(strFile.c_str(), cplModes[mMode]))
      return FStreamDoAccept(strFile, fNew);
#endif                                 // Operating system check
    // Failed
    return false;
  }
  /* -- Retrun true if internal stream or stream closed successfully ------- */
  bool FStreamDoClose(void)
    { return FStreamOpened() && (FStreamIsHandleStd() ||
        !fclose(FStreamGetHandle())); }
  /* -- Return file stream handle ---------------------------------- */ public:
  FILE *FStreamGetHandle(void) const { return fStream; }
  /* -- Returns true if handle is internal --------------------------------- */
  bool FStreamIsHandleStd(void) { return FStreamGetHandle() == stdin ||
                                         FStreamGetHandle() == stdout ||
                                         FStreamGetHandle() == stderr; }
  /* -- Swap stream handle and filename ------------------------------------ */
  void FStreamSwap(FStream &fsOther)
    { swap(fStream, fsOther.fStream); IdentSwap(fsOther); }
  /* -- File is opened?  --------------------------------------------------- */
  bool FStreamOpened(void) const { return !!FStreamGetHandle(); }
  /* -- Direct access using class variable name which returns opened ------- */
  operator bool(void) const { return FStreamOpened(); }
  /* -- File is closed?  --------------------------------------------------- */
  bool FStreamClosed(void) const { return !FStreamGetHandle(); }
  /* -- Return handle to stream -------------------------------------------- */
  int FStreamGetID(void) const
    { return FStreamOpened() ? StdFileNo(FStreamGetHandle()) : 0; }
  /* -- Set current file position ------------------------------------------ */
  bool FStreamSetPosition(const int64_t lPos, const int iMode) const
    { return FStreamOpened() ?
        !StdFSeek(FStreamGetHandle(), lPos, iMode) : false; }
  /* -- Return current file position --------------------------------------- */
  int64_t FStreamGetPosition(void) const
    { return FStreamOpened() ? StdFTell(FStreamGetHandle()) : EOF; }
  /* -- Flush buffered file data to disk ----------------------------------- */
  bool FStreamFlush(void) const
    { return FStreamOpened() ? fflush(FStreamGetHandle()) != EOF : false; }
  /* -- File stream is set to error status? -------------------------------- */
  int FStreamFError(void) const
    { return FStreamOpened() ? ferror(FStreamGetHandle()) : EOF; }
  /* -- Is position at end of the file? ------------------------------------ */
  bool FStreamIsEOF(void) const
    { return FStreamClosed() || feof(FStreamGetHandle()); }
  /* -- Is position not at end of the file? -------------------------------- */
  bool FStreamIsNotEOF(void) const { return !FStreamIsEOF(); }
  /* -- Read file information ---------------------------------------------- */
  bool FStreamStat(StdFStatStruct &sData) const
    { return !StdHStat(StdFileNo(FStreamGetHandle()), &sData); }
  /* -- Read data from file ------------------------------------------------ */
  size_t FStreamRead(void*const vpBuffer, const size_t stBytes) const
    { return fread(vpBuffer, 1, stBytes, FStreamGetHandle()); }
  /* -- Read line string from file ----------------------------------------- */
  const string FStreamReadString(const size_t stBytes) const
  { // bail if nothing to read
    if(FStreamIsEOF()) return {};
    // Setup string for buffer and read into it
    string strRead; strRead.resize(stBytes);
    strRead.resize(FStreamRead(const_cast<char*>(strRead.data()),
      strRead.length()));
    // Return string
    return strRead;
  }
  /* -- Read entire file without knowing the size of the file -------------- */
  const string FStreamReadStringChunked(const size_t stBufSize=4096) const
  { // Ignore if invalid size
    if(!stBufSize || FStreamIsEOF() || FStreamFError()) return {};
    // Stream to write strings to
    ostringstream osS;
    // Loop point
    ContinueReadingStrings:
    // Read the chunk and break if at end of file or error
    const string strChunk{ FStreamReadString(stBufSize) };
    if(strChunk.empty()) return osS.str();
    // Accumulate the size
    osS << std::move(strChunk);
    // Read another string
    goto ContinueReadingStrings;
  }
  /* -- Read entire file to string ----------------------------------------- */
  const string FStreamReadString(void) const
    { return FStreamReadString(FStreamSizeT()); }
  /* -- Write data to file ------------------------------------------------- */
  size_t FStreamWrite(const void*const vpPtr, const size_t stBytes) const
    { return fwrite(vpPtr, 1, stBytes, FStreamGetHandle()); }
  /* -- Write an static value to file -------------------------------------- */
  template<typename T>size_t FSWriteInt(const T tVar) const
    { return FStreamWrite(&tVar, sizeof(tVar)); }
  /* -- Write a string to the file ----------------------------------------- */
  size_t FStreamWriteString(const string &strString) const
    { return FStreamWrite(strString.data(), strString.length()); }
  /* -- Return file position to the beginning ------------------------------ */
  bool FStreamRewind(void) const { return FStreamSetPosition(0, SEEK_SET); }
  /* -- Return size as size_t so it will be clamped ------------------------ */
  size_t FStreamSizeT(void) const
    { return IntOrMax<size_t>(FStreamSize()); }
  /* -- Return size of file ------------------------------------------------ */
  int64_t FStreamSize(void) const
  { // No size if not opened
    if(FStreamClosed()) return 0;
    // Store current position
    const int64_t qCurrent = FStreamGetPosition();
    // Set end psotion
    FStreamSetPosition(0, SEEK_END);
    // Store position
    const int64_t qSize = FStreamGetPosition();
    // Restore position
    FStreamSetPosition(qCurrent, SEEK_SET);
    // Return size
    return qSize;
  }
  /* -- Open a file without filename validation ---------------------------- */
  int FStreamOpen(const string &strFile, const Mode mMode)
  { // Try to open the file on disk and if succeeded?
    if(FStreamDoOpen(strFile, mMode)) return 0;
    // Return error if the error was FNF or there is no backup home directory.
    if(cCmdLine->IsNoHome()) return GetErrNo();
    // Save error number
    const int iError = GetErrNo();
    // Return original error code if persist storage fails or success
    return FStreamDoOpen(cCmdLine->GetHome(strFile), mMode) ? iError : 0;
  }
  /* -- Close file --------------------------------------------------------- */
  bool FStreamClose(void)
  { // Success if handle already closed
    if(FStreamClosed()) return true;
    // Return if the handle isn't a built-in handle and failed to close
    if(!FStreamDoClose()) return false;
    // Stream was closed successfully so clear handle
    FStreamClearHandle();
    // Successful close
    return true;
  }
  /* -- Write memory block to file ----------------------------------------- */
  size_t FStreamWriteBlock(const DataConst &dcSrc)
    { return FStreamWrite(dcSrc.Ptr(), dcSrc.Size()); }
  /* -- Read data and return memory block ---------------------------------- */
  Memory FStreamReadBlock(const size_t stSize) const
  { // bail if no stream
    if(FStreamClosed()) return {};
    // Allocate initial memory of expected bytes to read
    Memory mOut{ stSize };
    // Read file data into buffer and resize buffer to the amount acteally read
    mOut.Resize(FStreamRead(mOut.Ptr(), stSize));
    // Return the new size of the buffer and the number of bytes read
    return mOut;
  }
  /* -- Read memory block from file ---------------------------------------- */
  Memory FStreamReadBlock(void) const
    { return FStreamReadBlock(FStreamSizeT()); }
  /* -- Basic constructor with no init ------------------------------------- */
  FStream(void) :
    /* -- Initialisation of members ---------------------------------------- */
    fStream{ nullptr }
    /* -- No code ---------------------------------------------------------- */
    { }
  /* -- Constructor with optional checking --------------------------------- */
  FStream(const string &strF, const Mode mMode) :
    /* -- Initialisation of members ---------------------------------------- */
    FStream{}
    /* -- Open the file and throw error if failed -------------------------- */
    { if(FStreamOpen(strF, mMode))
        XCL("Failed to open file!", "File", strF, "Mode", mMode); }
  /* -- Constructor with rvalue name init, no open ------------------------- */
  explicit FStream(string &&strF) :
    /* -- Initialisation of members ---------------------------------------- */
    Ident{ std::move(strF) },
    fStream{ nullptr }
    /* -- No code ---------------------------------------------------------- */
    { }
  /* -- Constructor with lvalue name init, no open ------------------------- */
  explicit FStream(const string &strF) :
    /* -- Initialisation of members ---------------------------------------- */
    Ident{ strF },
    fStream{ nullptr }
    /* -- No code ---------------------------------------------------------- */
    { }
  /* -- MOVE assignment constructor ---------------------------------------- */
  FStream(FStream &&fsOther) :
    /* -- Initialisation of members ---------------------------------------- */
    Ident{ std::move(fsOther) },
    fStream{ fsOther.FStreamGetHandle() }
    /* -- Clear other handle ----------------------------------------------- */
    { fsOther.FStreamClearHandle(); }
  /* -- Destructor (Close the file) ---------------------------------------- */
  ~FStream(void) { FStreamDoClose(); }
  /* ----------------------------------------------------------------------- */
  DELETECOPYCTORS(FStream);            // Disable copy constructor and operator
};/* ----------------------------------------------------------------------- */
/* ------------------------------------------------------------------------- */
};                                     // End of module namespace
/* == EoF =========================================================== EoF == */

/* == ARCHIVE.HPP ========================================================== */
/* ######################################################################### */
/* ## MS-ENGINE              Copyright (c) MS-Design, All Rights Reserved ## */
/* ######################################################################### */
/* ## This the file handles the .adb (7-zip) archives and the extraction  ## */
/* ## and decompression of files.                                         ## */
/* ######################################################################### */
/* ========================================================================= */
#pragma once                           // Only one incursion allowed
/* ------------------------------------------------------------------------- */
namespace IfArchive {                  // Start of module namespace
/* -- Includes ------------------------------------------------------------- */
using namespace IfASync;               // Using async namespace
using namespace IfCodec;               // Using codec namespace
using namespace IfCrypt;               // Using cryptographic namespace
/* == Archive collector with extract buffer size =========================== */
BEGIN_ASYNCCOLLECTOREX(Archives, Archive, CLHelperSafe,
  /* ----------------------------------------------------------------------- */
  size_t           stExtractBufSize;   /* Extract buffer size               */\
  ISzAlloc         isaData;            /* Allocator functions               */\
  /* -- Alloc function for lzma -------------------------------------------- */
  static void *Alloc(ISzAllocPtr, size_t stBytes)\
    { return MemAlloc<void>(stBytes); }\
  /* -- Free function for lzma --------------------------------------------- */
  static void Free(ISzAllocPtr, void*const vpAddress)\
    { MemFree(vpAddress); }\
);/* ----------------------------------------------------------------------- */
BUILD_FLAGS(Archive,
  /* ----------------------------------------------------------------------- */
  // Archive on standby?             Archive file handle opened?
  AE_STANDBY           {0x00000000}, AE_FILEOPENED        {0x00000001},
  // Allocated look2read structs?    Allocated archive structs?
  AE_SETUPL2R          {0x00000002}, AE_ARCHIVEINIT       {0x00000004}
);/* == Archive object class =============================================== */
BEGIN_MEMBERCLASS(Archives, Archive, ICHelperUnsafe),
  /* -- Base classes ------------------------------------------------------- */
  public AsyncLoader<Archive>,         // Async manager for off-thread loading
  public Lockable,                     // Lua garbage collect instruction
  public ArchiveFlags                  // Archive initialisation flags
{ /* -- Private macros ----------------------------------------------------- */
#if !defined(WINDOWS)                  // Not using Windows?
# define LZMAOpen(s,f)                 InFile_Open(s, f)
# define LZMAGetHandle(s)              (s).file.fd
#else                                  // Using Windows?
# define LZMAOpen(s,f)                 InFile_OpenW(s, UTFtoS16(f))
# define LZMAGetHandle(s)              (s).file.handle
#endif                                 // Operating system check
  /* -- Private Variables -------------------------------------------------- */
  condition_variable cvExtract;        // Waiting for async ops to complete
  mutex            mExtract;           // mutex for condition variable
  SafeSizeT        stInUse;            // API in use reference count
  /* ----------------------------------------------------------------------- */
  StrUIntMap       lFiles, lDirs;      // Files and directories as a map means
                                       // quick access to assets via filenames
  /* ----------------------------------------------------------------------- */
  StrUIntMapConstItVector vFiles, vDirs;  // Indexed list of referenced string
                                       // Ref to all data in lFiles/lDirs
  /* ----------------------------------------------------------------------- */
  uint64_t         qwArchPos;          // Position of archive in file
  CFileInStream    cfisData;           // LZMA file stream data
  CLookToRead2     cltrData;           // LZMA lookup data
  CSzArEx          csaeData;           // LZMA archive Data
  /* -- Process extracted data --------------------------------------------- */
  FileMap Extract(const string &strFile, const unsigned int uiSrcId,
    CLookToRead2 &cltrD, CSzArEx &csaeD)
  { // Storage for buffer
    unsigned char *ucpData = nullptr;
    // Capture exceptions so we can clean up
    try
    { // If the file came from a solid block then stUncompressed is the size
      // of the entire block which may include other data from other files.
      // stCompressed is the actual size of the file. Offset is the byte offset
      // of the file data requested insize the solid block, always zero with
      // files in non-solid blocks.
      size_t stCompressed = 0, stOffset = 0, stUncompressed = 0;
      // Block index returned in extractor function
      unsigned int uiBlockIndex = static_cast<unsigned int>(-1);
      // Decompress the buffer using our base handles and throw error if it
      // failed
      if(const int iCode = SzArEx_Extract(&csaeD, &cltrD.vt,
        uiSrcId, &uiBlockIndex, &ucpData, &stUncompressed, &stOffset,
        &stCompressed, &cParent.isaData, &cParent.isaData))
          XC("Failed to extract file",
             "Archive", IdentGet(), "File",  strFile,
             "Index",   uiSrcId, "Code",  iCode,
             "Reason",  CodecGetLzmaErrString(iCode));
      // No data returned meaning a zero-byte file?
      if(!ucpData)
      { // Allocate a zero-byte array to a new class. Remember we need to send
        // a valid allocated pointer to the file map.
        FileMap fmFile{ strFile, Memory{ 0 },
          GetCreatedTime(uiSrcId),
          GetModifiedTime(uiSrcId) };
        // Log progress
        cLog->LogInfoExSafe("Archive extracted empty '$' from '$'.",
          strFile, IdentGet());
        // Return file
        return fmFile;
      } // If the file is NOT from a solid block?
      if(stUncompressed == stCompressed)
      { // Put this pointer in a memory block and inherit the pointer, which
        // means no allocation or copying of memory is needed. Do not manually
        // deallocate this pointer, then return newly added item
        FileMap fmFile{ strFile,
          { stUncompressed, reinterpret_cast<void*>(ucpData) },
          GetCreatedTime(uiSrcId), GetModifiedTime(uiSrcId) };
        // Log progress
        cLog->LogInfoExSafe("Archive extracted '$'[$]<$> from '$'.",
          strFile, uiBlockIndex, stUncompressed, IdentGet());
        // Return file
        return fmFile;
      }
      // In this case we need to allocate a new block and copy over the data. I
      // highly discourage use of solid blocks due to this performance issue,
      // but there is really no need to deny this kind of loading.
      Memory mData{ stCompressed,
        reinterpret_cast<void*>(ucpData + stOffset) };
      // Free the data that was allocated by LZMA as we had to copy it
      cParent.isaData.Free(nullptr, reinterpret_cast<void*>(ucpData));
      // Return newly added item
      FileMap fmFile{ strFile, std::move(mData), GetCreatedTime(uiSrcId),
        GetModifiedTime(uiSrcId) };
      // Log progress
      cLog->LogInfoExSafe("Archive extracted '$'[$]{$>$} from '$'.",
        strFile, uiBlockIndex, stUncompressed, stCompressed, IdentGet());
      // Return class
      return fmFile;
    } // Exception occured
    catch(...)
    { // Free the block if it was allocated
      if(ucpData)
        cParent.isaData.Free(nullptr, reinterpret_cast<void*>(ucpData));
      // Rethrow error
      throw;
    }
  }
  /* -- DeInitialise Look2Read structs ------------------------------------- */
  void CleanupLookToRead(CLookToRead2 &cltrIn)
  { // Free the decopmression buffer if it was created
    if(cltrData.buf) ISzAlloc_Free(&cParent.isaData, cltrIn.buf);
  }
  /* -- Initialise Look2Read structs --------------------------------------- */
  void SetupLookToRead(CFileInStream &cfisOut, CLookToRead2 &cltrOut)
  { // Setup vtables and stream pointers
    FileInStream_CreateVTable(&cfisOut);
    LookToRead2_CreateVTable(&cltrOut, False);
    cltrOut.realStream = &cfisOut.vt;
    // Need to allocate transfer buffer in later LZMA.
    cltrOut.buf = reinterpret_cast<Byte*>
      (ISzAlloc_Alloc(&cParent.isaData, cParent.stExtractBufSize));
    if(!cltrData.buf)
      XC("Error allocating buffer for archive!",
         "Archive", IdentGet(), "Bytes", cParent.stExtractBufSize);
    cltrOut.bufSize = cParent.stExtractBufSize;
    // Initialise look2read structs. On p7zip, the buffer allocation is already
    // done for us.
    LookToRead2_Init(&cltrOut);
  }
  /* -- Get archive file/dir as table ------------------------------ */ public:
  const StrUIntMap &GetFileList(void) const { return lFiles; }
  const StrUIntMap &GetDirList(void) const { return lDirs; }
  /* -- Returns modified time of specified file ---------------------------- */
  StdTimeT GetModifiedTime(const size_t stId) const
    { return static_cast<StdTimeT>(SzBitWithVals_Check(&csaeData.MTime, stId) ?
        BruteCast<uint64_t>(csaeData.MTime.Vals[stId]) / 100000000 :
        numeric_limits<StdTimeT>::max()); }
  /* -- Returns creation time of specified file ---------------------------- */
  StdTimeT GetCreatedTime(const size_t stId) const
    { return static_cast<StdTimeT>(SzBitWithVals_Check(&csaeData.CTime, stId) ?
        BruteCast<uint64_t>(csaeData.CTime.Vals[stId]) / 100000000 :
        numeric_limits<StdTimeT>::max()); }
  /* -- Returns uncompressed size of file by id ---------------------------- */
  uint64_t GetSize(const size_t stId) const
    { return static_cast<uint64_t>(SzArEx_GetFileSize(&csaeData, stId)); }
  /* -- Total files and directories in archive ----------------------------- */
  size_t GetTotal(void) const { return csaeData.NumFiles; }
  /* -- Get a file/dir and uid by zero-index ------------------------------- */
  const StrUIntMapConstIt &GetFile(const size_t stI) const
    { return vFiles[stI]; }
  const StrUIntMapConstIt &GetDir(const size_t stI) const
    { return vDirs[stI]; }
  /* -- Return number of extra archives opened ----------------------------- */
  size_t GetInUse(void) const { return stInUse; }
  /* -- Return if iterator is valid ---------------------------------------- */
  bool IsFileIteratorValid(const StrUIntMapConstIt &flItem) const
    { return flItem != lFiles.cend(); }
  /* -- Gets the iterator of a filename ------------------------------------ */
  const StrUIntMapConstIt GetFileIterator(const string &strFile) const
    { return lFiles.find(strFile); }
  /* -- Loads a file from archive by iterator ------------------------------ */
  FileMap Extract(const StrUIntMapConstIt &flItem)
  { // Lock mutex. We don't care if we can't lock it though because we will
    // open another archive if we cannot lock it.
    const UniqueLock uLock{ mExtract, try_to_lock };
    // Create class to notify destructor when leaving this scope
    const class Notify { public: Archive &aCref;
      explicit Notify(Archive &oCr) : aCref(oCr) { }
      ~Notify(void) { aCref.cvExtract.notify_one(); } } cNotify(*this);
    // Get filename and filename id from iterator
    const string &strFile = flItem->first;
    const unsigned int &uiSrcId = flItem->second;
    // If the base archive file is in use?
    if(!uLock.owns_lock())
    { // We need to adjust in-use counter when joining and leaving this scope
      // This tells the destructor to wait until it is zero.
      const class Counter { public: Archive &aCref;
        explicit Counter(Archive &oCr) : aCref(oCr) { ++aCref.stInUse; }
        ~Counter(void) { --aCref.stInUse; } } cCounter(*this);
      // Because the API doesn't support sharing file handles, we will need
      // to re-open the archive again with new data. We don't need to collect
      // any filename data again though thankfully so this should still be
      // quite a speedy process.
      CFileInStream cfisData2{};
      CSzArEx csaeData2{};
      CLookToRead2 cltrData2{};
      // Capture exceptions so we can clean up 7zip api
      try
      { // Open new archive and throw error if it failed
        if(const int iCode = LZMAOpen(&cfisData2.file, IdentGetCStr()))
          XC("Failed to open archive!",
            "Archive", IdentGet(),       "Index", flItem->second,
            "File",    flItem->first, "Code",  iCode,
            "Reason",  CodecGetLzmaErrString(iCode));
        // Custom start position specified
        if(qwArchPos > 0 &&
          cSystem->SeekFile(LZMAGetHandle(cfisData), qwArchPos) != qwArchPos)
            XC("Failed to seek in archive!",
              "Archive", IdentGet(),       "Index",    flItem->second,
              "File",    flItem->first, "Position", qwArchPos);
        // Load archive
        SetupLookToRead(cfisData2, cltrData2);
        // Init lzma data
        SzArEx_Init(&csaeData2);
        // Initialise archive database and if failed, just log it
        if(const int iCode = SzArEx_Open(&csaeData2,
          &cltrData2.vt, &cParent.isaData, &cParent.isaData))
            XC("Failed to load archive!",
               "Archive", IdentGet(),       "Index", flItem->second,
               "File",    flItem->first, "Code",  iCode,
               "Reason",  CodecGetLzmaErrString(iCode));
        // Decompress the buffer using our duplicated handles
        FileMap fData{ Extract(strFile, uiSrcId, cltrData2, csaeData2) };
        // Clean up look to read
        CleanupLookToRead(cltrData2);
        // Free memory
        SzArEx_Free(&csaeData2, &cParent.isaData);
        // Close archive
        if(File_Close(&cfisData2.file))
          cLog->LogWarningExSafe("Archive failed to close archive '$': $!",
            IdentGet(), SysError());
        // Done
        return fData;
      } // exception occured
      catch(const exception &)
      { // Clean up look to read
        CleanupLookToRead(cltrData2);
        // Free memory
        SzArEx_Free(&csaeData2, &cParent.isaData);
        // Close archive
        if(File_Close(&cfisData2.file))
          cLog->LogWarningExSafe("Archive failed to close archive '$': $!",
            IdentGet(), SysError());
        // Show new exception for plain error message
        throw;
      } // Extract block
    } // Extract and return decompressed file
    else return Extract(strFile, uiSrcId, cltrData, csaeData);
  }
  /* -- Loads a file from archive by filename ------------------------------ */
  FileMap Extract(const string &strFile)
  { // Get iterator from filename and return empty file if not found
    const StrUIntMapConstIt flItem{ GetFileIterator(strFile) };
    if(flItem == lFiles.cend()) return {};
    // Extract the file
    return Extract(flItem);
  }
  /* -- Checks if file is in archive --------------------------------------- */
  bool FileExists(const string &strFile) const
    { return GetFileIterator(strFile) != lFiles.cend(); }
  /* -- Loads the specified archive ---------------------------------------- */
  void AsyncReady(FileMap &)
  { // Open archive and throw and show errno if it failed
    if(const int iCode = LZMAOpen(&cfisData.file, IdentGetCStr()))
      XCS("Error opening archive!", "Archive", IdentGet(), "Code", iCode);
    FlagSet(AE_FILEOPENED);
    // Custom start position specified?
    if(qwArchPos > 0)
    { // Log position setting
      cLog->LogDebugExSafe("Archive loading '$' from position $...",
        IdentGet(), qwArchPos);
      // Seek to overlay in executable + 1 and if failed? Log the warning
      if(cSystem->SeekFile(LZMAGetHandle(cfisData), qwArchPos) != qwArchPos)
        cLog->LogWarningExSafe("Archive '$' seek error! [$].",
          IdentGet(), SysError());
    } // Load from beginning? Log that we're loading from beginning
    else cLog->LogDebugExSafe("Archive loading '$'...", IdentGet());
    // Setup look to read structs
    SetupLookToRead(cfisData, cltrData);
    FlagSet(AE_SETUPL2R);
    // Init lzma data
    SzArEx_Init(&csaeData);
    FlagSet(AE_ARCHIVEINIT);
    // Initialise archive database and if failed, just log it
    if(const int iCode = SzArEx_Open(&csaeData, &cltrData.vt,
      &cParent.isaData, &cParent.isaData))
    { // Log warning and return
      cLog->LogWarningExSafe("Archive '$' not opened with code $ ($)!",
        IdentGet(), iCode, CodecGetLzmaErrString(iCode));
      return;
    }
    // Alocate memory for quick access via index vector. 7-zip won't tell us
    // how many files and directories there are individually so we'll reserve
    // memory for the maximum amount of entries in the 7-zip file. We'll shrink
    // this after to reclaim the memory
    const size_t stFilesMaximum = vFiles.max_size();
    if(csaeData.NumFiles > stFilesMaximum)
      XC("Insufficient storage for file list!",
         "Current", csaeData.NumFiles, "Maximum", stFilesMaximum);
    vFiles.reserve(csaeData.NumFiles);
    // ...and same for the directories too.
    const size_t stDirsMaximum = vDirs.max_size();
    if(vFiles.size() > stDirsMaximum)
      XC("Insufficient storage for directory list!",
         "Current", vFiles.size(), "Maximum", stDirsMaximum);
    vDirs.reserve(vFiles.size());
    // Enumerate through each file
    for(unsigned int uiIndex = 0; uiIndex < csaeData.NumFiles; ++uiIndex)
    { // Get length of file name string. This includes the null terminator.
      const size_t stLen =
        SzArEx_GetFileNameUtf16(&csaeData, uiIndex, nullptr);
      if(stLen < sizeof(UInt16)) continue;
      // Create buffer for file name.
      vector<UInt16> vFilesWide(stLen, 0x0000);
      SzArEx_GetFileNameUtf16(&csaeData, uiIndex, vFilesWide.data());
      // If is a directory?
      if(SzArEx_IsDir(&csaeData, uiIndex))
        // Convert wide-string to utf-8 and insert it in the dirs to integer
        // list and store the iterator in the vector
        vDirs.push_back(lDirs.insert({
          FromWideStringPtr(vFilesWide.data()), uiIndex }).first);
      // Is a file?
      else
        // Convert wide-string to utf-8 and insert it in the files to integer
        // list and store the iterator in the vector
        vFiles.push_back(lFiles.insert({
          FromWideStringPtr(vFilesWide.data()), uiIndex }).first);
    } // We did not know how many files and directories there were
    // specifically so lets free the extra memory allocated for the lists
    vFiles.shrink_to_fit();
    vDirs.shrink_to_fit();
    // Log progress
    cLog->LogInfoExSafe("Archive loaded '$' (F:$;D:$).",
      IdentGet(), lFiles.size(), lDirs.size());
  }
  /* -- Loads archive asynchronously --------------------------------------- */
  void InitAsyncFile(lua_State*const lS)
  { // Need 4 parameters (class pointer was already pushed onto the stack);
    CheckParams(lS, 5);
    // Get and check parameters
    const string strFilename{ GetCppFileName(lS, 1, "File") };
    CheckFunction(lS, 2, "ErrorFunc");
    CheckFunction(lS, 3, "ProgressFunc");
    CheckFunction(lS, 4, "SuccessFunc");
    // Throw error if invalid name
    if(const ValidResult vrId = DirValidName(strFilename))
      XC("Filename is invalid!",
         "File",     strFilename, "Reason", DirValidNameResultToString(vrId),
         "ReasonId", vrId);
    // Load asynchronously, except we load the file, not async class
    AsyncInitNone(lS, strFilename, "archivefile");
  }
  /* -- Loads archive synchronously ---------------------------------------- */
  void InitFromFile(const string &strFilename, const uint64_t qwPosition)
  { // Store position
    qwArchPos = qwPosition;
    // Set filename without filename checking
    SyncInitFile(strFilename);
  }
  /* -- Loads archive synchronously ---------------------------------------- */
  void InitFromFileSafe(const string &strFilename, const uint64_t qwPosition=0)
  { // Store position
    qwArchPos = qwPosition;
    // Load the file with filename checking
    SyncInitFileSafe(strFilename);
  }
  /* -- For loading via lua ------------------------------------------------ */
  Archive(void) :
    /* -- Initialisers ----------------------------------------------------- */
    ICHelperArchive{ *cArchives },     // Initialise collector with this obj
    IdentCSlave{ cParent.CtrNext() },  // Initialise identification number
    AsyncLoader<Archive>{ this,        // Initialise async collector
      EMC_MP_ARCHIVE },                // " our archive async event
    ArchiveFlags{ AE_STANDBY },        // Set default archive flags
    stInUse(0),                        // Set threads in use
    qwArchPos(0),                      // Set archive initial position
    cfisData{},                        // Init file stream data
    cltrData{},                        // Init lookup stream data
    csaeData{}                         // Init archive stream data
    /* -- No code ---------------------------------------------------------- */
    { }
  /* -- Unloads the archive ------------------------------------------------ */
  ~Archive(void)
  { // Done if a filename is not set
    if(IdentIsNotSet()) return;
    // Wait for archive loading async operations to complete
    AsyncCancel();
    // Done if a archive file handle is not opened.
    if(FlagIsClear(AE_FILEOPENED)) return;
    // Unloading archive in log
    cLog->LogDebugExSafe("Archive unloading '$'...", IdentGet());
    // If decompression is being executed across threads?
    if(stInUse > 0)
    { // Then we need to wait until they finish.
      cLog->LogInfoExSafe("Archive '$' waiting for $ async ops to complete...",
        IdentGet(), static_cast<size_t>(stInUse));
      // Wait for base and spawned file operations to finish
      UniqueLock uLock{ mExtract };
      cvExtract.wait(uLock, [this]{ return !stInUse; });
    } // Free archive structs if allocated
    if(FlagIsSet(AE_ARCHIVEINIT)) SzArEx_Free(&csaeData, &cParent.isaData);
    // Memory allocated? Free memory allocated for buffer
    if(FlagIsSet(AE_SETUPL2R)) CleanupLookToRead(cltrData);
    // Close archive handle
    if(File_Close(&cfisData.file))
      cLog->LogWarningExSafe("Archive failed to close archive '$': $!",
        IdentGet(), SysError());
    // Log shutdown
    cLog->LogInfoExSafe("Archive unloaded '$' successfully.", IdentGet());
  }
  /* ----------------------------------------------------------------------- */
  DELETECOPYCTORS(Archive);            // Supress copy constructor for safety
  /* -- Done with these defines -------------------------------------------- */
#undef LZMAGetHandle                   // Done with this macro
#undef LZMAOpen                        // Done with this macro
#undef ISzAllocPtr                     // Done with this macro
};/* ----------------------------------------------------------------------- */
END_ASYNCCOLLECTOR(Archives, Archive, ARCHIVE,
  stExtractBufSize(0),
  isaData{ Alloc, Free });
/* == Look if a file exists in archives ==================================== */
static bool ArchiveFileExists(const string &strFile)
{ // Lock archive list so it cannot be modified and iterate through the list
  const LockGuard lgArchivesSync{ cArchives->CollectorGetMutex() };
  // For each archive. Return if the specified file exists in it.
  return any_of(cArchives->cbegin(), cArchives->cend(),
    [&strFile](const Archive*const aCptr)
      { return aCptr->FileExists(strFile); });
}
/* -- Create and check a dynamic archive ----------------------------------- */
static Archive *ArchiveInitNew(const string &strFile, const size_t stSize=0)
{ // Dynamically create the archive. The pointer is recorded in the parent
  // collector class.
  typedef unique_ptr<Archive> ArchivePtr;
  if(ArchivePtr oClass{ new Archive })
  { // Load the archive and return archive if there are entries inside it
    Archive &oRef = *oClass.get();
    oRef.InitFromFile(strFile, stSize);
    if(!oRef.GetFileList().empty() || !oRef.GetDirList().empty())
      return oClass.release();
  } // Return nothing
  return nullptr;
}
/* -- Set extraction buffer size ------------------------------------------- */
static CVarReturn ArchiveSetBufferSize(const size_t stSize)
  { return CVarSimpleSetIntNLG(cArchives->stExtractBufSize, stSize,
      static_cast<size_t>(262144), static_cast<size_t>(16777216)); }
/* -- Loads the archive from executable ------------------------------------ */
static CVarReturn ArchiveInitExe(const bool bCheck)
{ // If we're checking the executable for archive?
  if(bCheck)
  { // Temporarily switch to executable directory. It is mainly cosmetic, but
    // I guess it provides security that the executables directory can be read
    // and executed.
    const string &strDir = cSystem->ENGLoc();
    if(!DirSetCWD(strDir))
      XCL("Failed to switch to executable directory!", "Directory", strDir);
    // Try to read executable size and if succeeded? Try loading it from the
    // position that was detected from the executable.
    const string &strFile = cSystem->ENGFileExt();
    if(const size_t stSize = cSystem->GetExeSize(strFile))
      ArchiveInitNew(strFile, stSize);
  } // Status is acceptable regardless
  return ACCEPT;
}
/* -- Loads the specified archive ------------------------------------------ */
static CVarReturn ArchiveInit(const string &strFileMask, string&)
{ // Ignore if file mask not specified
  if(strFileMask.empty()) return ACCEPT;
  // Build directory listing and log how many files we found
  cLog->LogDebugExSafe("Archives scanning for '$' files...", strFileMask);
  const Dir dList{ strBlank, strFileMask };
  // Return if no files found (rare but not impossible)
  if(dList.dFiles.empty())
  { // Log no files and return success
    cLog->LogWarningExSafe("Archives matched no potential archive filenames!");
    return ACCEPT;
  } // Start processing filenames
  cLog->LogDebugExSafe("Archives loading $ files in working directory...",
    dList.dFiles.size());
  // Counters
  size_t stFound = 0, stFiles = 0, stDirs = 0;
  // For each archive file
  for(const auto &dI : dList.dFiles)
  { // Log archive info
    cLog->LogDebugExSafe("- #$: '$' (S:$;A:0x$$;C:0x$;M:0x$).",
      ++stFound, dI.first, ToBytesStr(dI.second.qSize), hex,
      dI.second.qFlags, dI.second.tCreate, dI.second.tWrite);
    // Dynamically create the archive. The pointer is recorded in the parent
    // and is referenced from there when loading other files. If succeeded?
    if(const Archive*const aCptr = ArchiveInitNew(dI.first))
    { // Add counters to grand totals
      stFiles += aCptr->GetFileList().size();
      stDirs += aCptr->GetDirList().size();
    }
  } // Log init
  cLog->LogInfoExSafe("Archives loaded $ of $ archives (F:$;D:$).",
    cArchives->CollectorCount(), dList.dFiles.size(), stFiles, stDirs);
  // Ok
  return ACCEPT;
}
/* -- Parallel enumeration ------------------------------------------------- */
static void ArchiveEnumFiles(const string &strDir, const StrUIntMap &suimList,
  StrSet &ssFiles, mutex &mLock)
{ // For each directory in archive. Try to use multi-threading.
  MYFOREACH(par_unseq, suimList.cbegin(), suimList.cend(),
    [&strDir, &ssFiles, &mLock](const auto &itItem)
  { // If folder name does not match ignore
    if(strDir != itItem.first.substr(0, strDir.length())) return;
    // Split file path
    const PathSplit psFile{ itItem.first };
    // Lock access to archives list
    const LockGuard lgLock{ mLock };
    // Split path parts, lock mutex, and move into list
    ssFiles.emplace(std::move(psFile.strFileExt));
  });
}
/* -- Return files in directories and archives with empty check ------------ */
static const StrSet &ArchiveEnumerate(const string &strDir,
  const string &strExt, const bool bOnlyDirs, StrSet &ssFiles)
{ // Lock archive list so it cannot be modified and if we have archives?
  const LockGuard lgArchivesSync{ cArchives->CollectorGetMutex() };
  if(!cArchives->empty())
  { // Lock for file list
    mutex mLock;
    // If only dirs requested? For each archive.
    if(bOnlyDirs)
      MYFOREACH(par, cArchives->cbegin(), cArchives->cend(),
        [&strDir, &ssFiles, &mLock](const Archive*const aCptr)
          { ArchiveEnumFiles(strDir, aCptr->GetDirList(), ssFiles, mLock); });
    // No extension specified? Show all files
    else if(strExt.empty())
      MYFOREACH(par, cArchives->cbegin(), cArchives->cend(),
        [&strDir, &ssFiles, &mLock](const Archive*const aCptr)
          { ArchiveEnumFiles(strDir, aCptr->GetFileList(), ssFiles, mLock); });
    // Files with extension requested. For each archive.
    else MYFOREACH(par, cArchives->cbegin(), cArchives->cend(),
      [&strDir, &ssFiles, &mLock, &strExt](const Archive*const aCptr)
    { // Get reference to file list
      const StrUIntMap &suimList = aCptr->GetFileList();
      // For each directory in archive...
      MYFOREACH(par_unseq, suimList.cbegin(), suimList.cend(),
        [&strDir, &ssFiles, &mLock, &strExt](const auto &itItem)
      { // If folder name does not match ignore
        if(strDir != itItem.first.substr(0, strDir.length())) return;
        // Split path parts, and ignore if extension does not match
        const PathSplit psParts{ itItem.first };
        if(psParts.strExt != strExt) return;
        // Lock the mutex and insert into list
        const LockGuard lgLock{ mLock };
        ssFiles.emplace(std::move(psParts.strFileExt));
      });
    });
  } // Return file list
  return ssFiles;
}
/* -- Extract -------------------------------------------------------------- */
static FileMap ArchiveExtract(const string &strFile)
{ // Lock archive list so it cannot be modified and iterate through the list
  UniqueLock arLock{ cArchives->CollectorGetMutex() };
  // Enumerate each archive from last to first because the latest-most loaded
  // archive should always have priority if multiple archives have the same
  // filename, just like game engines do.
  for(Archives::const_reverse_iterator cArchIt{ cArchives->crbegin() };
                                       cArchIt != cArchives->crend();
                                     ++cArchIt)
  { // Get reference to archive and find file, try next file if file not found
    Archive &aCref = **cArchIt;
    const StrUIntMapConstIt flItem{ aCref.GetFileIterator(strFile) };
    if(!aCref.IsFileIteratorValid(flItem)) continue;
    // Unlock the mutex because we don't need access to the list anymore
    arLock.unlock();
    // Try to extract file and return it if succeeded!
    FileMap fmFile{ aCref.Extract(flItem) };
    if(fmFile.FileMapOpened()) return fmFile;
    // Something bad happened so relock the mutex and keep trying other
    // archives anyway.
    arLock.lock();
  } // FileMap not found
  return {};
}
/* -- ArchiveGetNames -------------------------------------------------- */
static const string ArchiveGetNames(void)
{ // Set default archive name if no archives
  if(cArchives->empty()) return {};
  // Get first archive
  Archives::const_iterator aCptrIt{ cArchives->cbegin() };
  // Lets build a list of archives
  ostringstream osS;
  // Add first archive name to list
  osS << (*aCptrIt)->IdentGet();
  // Until end of archives add their names prefixed with a space
  while(++aCptrIt != cArchives->cend()) osS << ' ' << (*aCptrIt)->IdentGet();
  // Return string
  return osS.str();
}
/* ------------------------------------------------------------------------- */
};                                     // End of module namespace
/* == EoF =========================================================== EoF == */

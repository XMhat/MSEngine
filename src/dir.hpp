/* == PSPLIT.HPP =========================================================== */
/* ######################################################################### */
/* ## MS-ENGINE              Copyright (c) MS-Design, All Rights Reserved ## */
/* ######################################################################### */
/* ## Reads and stores all files in the specified directory.              ## */
/* ######################################################################### */
/* ========================================================================= */
#pragma once                           // Only one incursion allowed
/* -- Module namespace ----------------------------------------------------- */
namespace IfDir {                      // Keep declarations neatly categorised
/* -- Includes ------------------------------------------------------------- */
using namespace IfPSplit;              // Using psplit interface
using namespace IfError;               // Using error interface
using namespace IfToken;               // Using token interface
/* ------------------------------------------------------------------------- */
enum ValidResult                       // Return values for ValidName()
{ /* ----------------------------------------------------------------------- */
  VR_OK,                               // 00: The filename is valid!
  VR_EMPTY,                            // 01: The filename is empty!
  VR_TOOLONG,                          // 02: The filename is too long!
  VR_NOROOT,                           // 03: Root directory not allowed
  VR_NODRIVE,                          // 04: Drive letter not allowed
  VR_INVDRIVE,                         // 05: Invalid drive letter
  VR_INVALID,                          // 06: Invalid trust parameter
  VR_DPRS,                             // 07: Doubleslash or pre/suffix slash
  VR_INVCHAR,                          // 08: Invalid character in part
  VR_PARENT,                           // 09: Parent directory not allowed
  VR_NOTRAILWS,                        // 10: No trailing whitespace
  VR_NOLEADWS,                         // 11: No leading whitespace
  VR_RESERVED,                         // 12: No reserved names
  /* ----------------------------------------------------------------------- */
  VR_MAX,                              // 13: Maximum number of errors
};/* ----------------------------------------------------------------------- */
enum ValidType                         // Types for ValidName()
{ /* ----------------------------------------------------------------------- */
  VT_TRUSTED,                          // Trusted caller
  VT_UNTRUSTED,                        // Untrusted caller (chroot-like)
}; /* ====================================================================== */
/* -- Convert a valid result from ValidName to string ---------------------- */
static const string &DirValidNameResultToString(const ValidResult vrId)
{ // Lookup table of errors
  static const IfIdent::IdList<VR_MAX> ilErrors{{
    "Filename is valid",         /*00-01*/ "Empty filename",
    "Filename too long",         /*02-03*/ "Root directory not allowed",
    "Drive letter not allowed",  /*04-05*/ "Invalid drive letter",
    "Invalid trust parameter",   /*06-07*/ "Double-slash or pre/suffix slash",
    "Invalid character in part", /*08-09*/ "Parent directory not allowed",
    "No trailing whitespace",    /*10-11*/ "No leading whitespace",
    "No reserved names",         /*12   */
  }};
  // Return looked up string
  return ilErrors.Get(vrId);
}
/* -- Check that path part character is valid ------------------------------ */
static bool DirIsValidPathPartCharacter(const char cChar)
{ // Test character
  switch(cChar)
  { // Check for invalid characters. On unix systems these characters are
    // allowed except for ':' which is problematic on MacOS but let's ban
    // them on all systems to keep cross compatibility.
    case '<': case '>': case ':': case '\"': case '\\': case '|': case '?':
    case '*': return false;
    // No control characters allowed
    default: if(cChar < ' ') return false;
  } // Success
  return true;
}
/* -- Check that path part characters are valid ---------------------------- */
static bool DirIsValidPathPartCharactersCallback(const char cChar)
  { return !DirIsValidPathPartCharacter(cChar); }
static bool DirIsValidPathPartCharacters(const string &strPart,
  const size_t stPos)
    { return !any_of(next(strPart.cbegin(), static_cast<ssize_t>(stPos)),
        strPart.cend(), DirIsValidPathPartCharactersCallback); }
static bool DirIsValidPathPartCharacters(const string &strPart)
  { return !any_of(strPart.cbegin(), strPart.cend(),
      DirIsValidPathPartCharactersCallback); }
/* -- Check that filename doesn't leave the exe directory ------------------ */
static ValidResult DirValidName(const string &strName,
  const ValidType vtId=VT_UNTRUSTED)
{ // Failed if empty string
  if(strName.empty()) return VR_EMPTY;
  // Failed if the length is longer than the maximum allowed path.
  if(strName.length() > _MAX_PATH) return VR_TOOLONG;
  // If using windows
#if defined(WINDOWS)
  // Failed if the first or last character is a space
  if(strName.front() <= 32) return VR_NOLEADWS;
  if(strName.back() <= 32) return VR_NOTRAILWS;
  // Replace backslashes with forward slashes on Windows
  const string &strChosen = strName.find('\\') == string::npos ?
    strName : PSplitBackToForwardSlashes(strName);
#else
  const string &strChosen = strName;
#endif
  // Which type
  switch(vtId)
  { // Full sandbox. Do not leave .exe directory
    case VT_UNTRUSTED:
    { // Root directory not allowed
      if(strChosen[0] == '/') return VR_NOROOT;
      // Get parts from pathname and return if empty.
      const Token tParts{ strChosen, "/" };
      // Get first iterator and string.
      StrVectorConstIt svciPart{ tParts.cbegin() };
      const string &strFirst = tParts[0];
      // If we have a length of 2 or more?
      if(strFirst.length() > 1)
      { // No parent directory or drive letter allowed
        if(strFirst == "..") return VR_PARENT;
        if(strFirst[1] == ':') return VR_NODRIVE;
      } // Test all the characters in the first string
      if(!DirIsValidPathPartCharacters(strFirst)) return VR_INVCHAR;
      // This check will allow trailing forwardslashes
      const StrVectorConstIt svciEnd{ tParts.cend() -
        (tParts.size() >= 2 && tParts.rbegin()->empty() ? 1 : 0) };
      // Check the rest of them
      while(++svciPart != svciEnd)
      { // Get part
        const string &strPart = *svciPart;
        // Not allowed to be empty or parent directory
        if(strPart.empty()) return VR_DPRS;
        if(strPart == "..") return VR_PARENT;
        // Failed first character is an invalid character.
        if(!DirIsValidPathPartCharacters(strPart)) return VR_INVCHAR;
      } // Success
      return VR_OK;
    } // Trusted filename?
    case VT_TRUSTED:
    { // Get parts from pathname and if was just a root directory, it's fine
      const Token tParts{ strChosen, "/" };
      // Get first string item and iterator.
      StrVectorConstIt svciPart{ tParts.cbegin() };
      const string &strFirst = tParts[0];
      // Check drive letter is valid
      if(strFirst.length() > 1 && strFirst[1] == ':')
      { // Get first character and make sure the drive letter is valid
        const char cFirst = strFirst.front();
        if((cFirst < 'A' || cFirst > 'Z') &&
           (cFirst < 'a' || cFirst > 'z')) return VR_INVDRIVE;
        // Test rest of characters from the second character
        if(!DirIsValidPathPartCharacters(strFirst, 2)) return VR_INVCHAR;
      } // Test all of the characters
      else if(!DirIsValidPathPartCharacters(strFirst)) return VR_INVCHAR;
      // This check will allow trailing forwardslashes
      const StrVectorConstIt svciEnd{ tParts.cend() -
        (tParts.size() >= 2 && tParts.rbegin()->empty() ? 1 : 0) };
      // Check the rest of them
      while(++svciPart != svciEnd)
      { // Get part
        const string &strPart = *svciPart;
        // Not allowed to be empty or parent directory
        if(strPart.empty()) return VR_DPRS;
        // Failed first character is an invalid character.
        if(!DirIsValidPathPartCharacters(strPart)) return VR_INVCHAR;
      } // Success
      return VR_OK;
    } // Anything else invalid
    default: return VR_INVALID;
  }
}
/* -- DirFile class -------------------------------------------------------- */
class DirFile                          // Files container class
{ /* -- Public typedefs -------------------------------------------- */ public:
  struct Item                          // File information structure
  { /* --------------------------------------------------------------------- */
    STDTIMET       tCreate;            // File creation time
    STDTIMET       tAccess;            // File access time
    STDTIMET       tWrite;             // File modification time
    uint64_t       qSize;              // File size
    uint64_t       qFlags;             // Attributes (OS specific)
  };/* --------------------------------------------------------------------- */
  typedef map<const string, const Item> EntMap;   // map of file entries
  typedef EntMap::const_iterator        EntMapIt; // " iterator
  /* -- Public variables --------------------------------------------------- */
  EntMap           dDirs, dFiles;      // Directories and files list
  /* -- Export ------------------------------------------------------------- */
  const StrSet Export(const EntMap &dSrc) const
  { // Write entries into a single set list and return it
    StrSet ssFiles;
    for(const auto &dFile : dSrc) ssFiles.emplace(move(dFile.first));
    return ssFiles;
  }
  /* -- Convert to set ----------------------------------------------------- */
  const StrSet DirsToSet(void) const { return Export(dDirs); }
  const StrSet FilesToSet(void) const { return Export(dFiles); }
  /* -- Empty constructor -------------------------------------------------- */
  DirFile(void) { }
  /* -- Move constructor --------------------------------------------------- */
  DirFile(EntMap &&dD, EntMap &&dF) :
    /* -- Initialisation of members ---------------------------------------- */
    dDirs{ move(dD) },
    dFiles{ move(dF) }
    /* -- No code ---------------------------------------------------------- */
    { }
  /* -- Move constructor --------------------------------------------------- */
  DirFile(DirFile &&dflOther) :
    /* -- Initialisation of members ---------------------------------------- */
    dDirs{ move(dflOther.dDirs) },
    dFiles{ move(dflOther.dFiles) }
    /* -- No code ---------------------------------------------------------- */
    { }
  /* ----------------------------------------------------------------------- */
  DELETECOPYCTORS(DirFile);            // Disable copy constructor and operator
};/* -- DirCore class ------------------------------------------------------ */
class DirCore                          // System specific implementation
{ /* -- Variables -------------------------------------------------- */ public:
  string           strFile;            // Name of next file
  DirFile::Item    dItem;              // Current item information
  bool             bIsDir;             // Current item is a directory
  /* -- Setup implementation for WIN32 ------------------------------------- */
#if defined(WINDOWS)                   // WIN32 implementation
  /* -- Variables for WIN32 system -------------------------------- */ private:
  _wfinddata64_t   wfData;             // Data returned
  const intptr_t   iH;                 // Context handle
  bool             bMore;              // If we have more files
  /* -- Process current match ---------------------------------------------- */
  void ProcessItem(void)
  { // Set if this is a directory
    bIsDir = wfData.attrib & _A_SUBDIR;
    // Set filename
    strFile = S16toUTF(wfData.name);
    // Set file information
    dItem = { wfData.time_create, wfData.time_access,
              wfData.time_write, static_cast<uint64_t>(wfData.size),
              wfData.attrib };
    // Get next item
    bMore = _wfindnext64(iH, &wfData) != -1;
  }
  /* -- Return if directory was opened on WIN32 system ------------- */ public:
  bool IsOpened(void) const { return iH != -1; }
  /* -- Prepare next file for WIN32 system --------------------------------- */
  bool GetNextFile(void)
  { // If there are no more files then we are done
    if(!bMore) return false;
    // Process the current item
    ProcessItem();
    // Success
    return true;
  }
  /* -- Constructor for WIN32 system --------------------------------------- */
  explicit DirCore(const string &strDir) :
    /* -- Initialisation of members ---------------------------------------- */
    iH(_wfindfirst64(UTFtoS16(string{ strDir.empty() ? "*" :
      Append(Trim(strDir, '/'), "/*") }), &wfData)),
    bMore(iH != -1)
    /* -- Process file if there are more ----------------------------------- */
    { if(bMore) ProcessItem(); }
  /* -- Destructor for WIN32 system ---------------------------------------- */
  ~DirCore(void) { if(iH != -1) _findclose(iH); }
  /* ----------------------------------------------------------------------- */
#elif defined(MACOS)                   // Must use readdir_r on OSX
  /* -- Private typedefs ------------------------------------------ */ private:
  // This handle will be cleaned up by closedir() when it goes out of scope!
  typedef unique_ptr<DIR, function<decltype(closedir)>> DirUPtr;
  /* -- Private variables -------------------------------------------------- */
  DirUPtr         dData;               // Context for opendir()
  string          strPrefix;           // Prefix for filenames with stat()
  struct dirent   dPtr, *dPtrNext;     // Directory entry struct + next ptr
  /* -- Return if directory was opened on POSIX system ------------- */ public:
  bool IsOpened(void) const { return !!dData; }
  /* -- Prepare next file for POSIX system --------------------------------- */
  bool GetNextFile(void)
  { // Read the filename and if failed
    if(readdir_r(dData.get(), &dPtr, &dPtrNext) || !dPtrNext) return false;
    // Set filename
    strFile = dPtr.d_name;
    // Set next handle
    dPtrNext = &dPtr;
    // Data for stat
    struct stat sData;
    // Get information about the filename
    if(stat(Append(strPrefix, strFile).c_str(), &sData) == -1)
    { // Not a directory (unknown)
      bIsDir = false;
      // Set the file data as blank
      dItem = { 0, 0, 0, 0, 0 };
    } // Stat was successful?
    else
    { // Set if is directory
      bIsDir = S_ISDIR(sData.st_mode);
      // Set data
      dItem = { sData.st_ctime, sData.st_atime, sData.st_mtime,
        static_cast<uint64_t>(sData.st_size), sData.st_mode };
    } // Success
    return true;
  }
  /* -- Constructor for POSIX system --------------------------------------- */
  explicit DirCore(const string &strDir) :
    /* -- Initialisation of members ---------------------------------------- */
    dData{ opendir(Trim(strDir.empty() ? "." : strDir, '/').c_str()),
      closedir },
    dPtrNext{ &dPtr }
    /* -- Initialise directory handle -------------------------------------- */
    { // Return if we could not open the directory
      if(!dData) return;
      // Prepare prefix for filenames to state
      strPrefix = Append(strDir, '/');
      // Prepare the first filename and reset handle if failed
      if(!GetNextFile()) dData.reset();
    }
  /* ----------------------------------------------------------------------- */
#else                                  // POSIX implementation?
  /* -- Private variables ----------------------------------------- */ private:
  DIR            *dData;               // Context for opendir()
  string          strPrefix;           // Prefix for filenames with stat()
  /* -- Return if directory was opened on POSIX system ------------- */ public:
  bool IsOpened(void) const { return !!dData; }
  /* -- Prepare next file for POSIX system --------------------------------- */
  bool GetNextFile(void)
  { // Read the filename and if failed
    if(struct dirent*const dPtr = readdir(dData))
    { // Data for stat
      struct stat sData;
      // Set filename
      strFile = dPtr->d_name;
      // Get information about the filename
      if(stat(Append(strPrefix, strFile).c_str(), &sData) == -1)
      { // Not a directory (unknown)
        bIsDir = false;
        // Set the file data as blank
        dItem = { 0, 0, 0, 0, 0 };
      } // Stat was successful?
      else
      { // Set if is directory
        bIsDir = S_ISDIR(sData.st_mode);
        // Set data
        dItem = { sData.st_ctime, sData.st_atime, sData.st_mtime,
          static_cast<uint64_t>(sData.st_size), sData.st_mode };
      } // Success
      return true;
    } // Failed
    return false;
  }
  /* -- Constructor for POSIX system --------------------------------------- */
  explicit DirCore(const string &strDir) :
    /* -- Initialisation of members ---------------------------------------- */
    dData{ opendir(Trim(strDir.empty() ? "." : strDir, '/').c_str()) }
    /* -- Initialise directory handle -------------------------------------- */
    { // Return if we could not open the directory
      if(!dData) return;
      // Prepare prefix for filenames to state
      strPrefix = Append(strDir, '/');
      // Prepare the first filename and return if succeeded
      if(GetNextFile()) return;
      // Close the directory
      closedir(dData);
      // Failed for IsOpened
      dData = nullptr;
    }
  /* -- Destructor for POSIX system ---------------------------------------- */
  ~DirCore(void) { if(dData) closedir(dData); }
  /* ----------------------------------------------------------------------- */
#endif                                 // End of system implementation check
  /* ----------------------------------------------------------------------- */
  DELETECOPYCTORS(DirCore);            // Disable copy constructor and operator
};/* == Dir class ========================================================== */
class Dir :                            // Directory information class
  /* -- Base classes ------------------------------------------------------- */
  public DirFile                       // Files container class
{ /* -- Do scan --------------------------------------------------- */ private:
  static void RemoveEntry(DirFile::EntMap &dflList, const char*const cpEntry)
  { // Remove specified entry
    const DirFile::EntMapIt dliItem(dflList.find(cpEntry));
    if(dliItem != dflList.cend()) dflList.erase(dliItem);
  }
  /* -- Remove current and parent directory entries ------------------------ */
  static void RemoveParentAndCurrentDirectory(DirFile::EntMap &dflList)
  { // Remove "." and ".." current directory entries
    RemoveEntry(dflList, ".");
    RemoveEntry(dflList, "..");
  }
  /* -- Scan with no match checking ---------------------------------------- */
  static DirFile ScanDir(const string &strDir={})
  { // Directory and file list
    DirFile::EntMap dliDirs, dliFiles;
    // Load up the specification and return if failed
    DirCore dfcInterface(strDir);
    if(dfcInterface.IsOpened())
    { // Repeat...
      do
      { // Add directory if is a directory
        if(dfcInterface.bIsDir)
          dliDirs.insert({ move(dfcInterface.strFile),
                           move(dfcInterface.dItem) });
        // Insert into files list
        else dliFiles.insert({ move(dfcInterface.strFile),
                               move(dfcInterface.dItem) });
        // ...until no more entries
      } while(dfcInterface.GetNextFile());
      // Remove '.' and '..' entries
      RemoveParentAndCurrentDirectory(dliDirs);
    } // Return list of files and directories
    return { move(dliDirs), move(dliFiles) };
  }
  /* -- Scan with match checking ------------------------------------------- */
  static DirFile ScanDirExt(const string &strDir, const string &strExt)
  { // Directory and file list
    DirFile::EntMap dliDirs, dliFiles;
    // Load up the specification and return if failed
    DirCore dfcInterface(strDir);
    if(dfcInterface.IsOpened())
    { // Repeat...
      do
      { // Add directory if is a directory
        if(dfcInterface.bIsDir)
          dliDirs.insert({ move(dfcInterface.strFile),
                           move(dfcInterface.dItem) });
        // Is a file and extension doesn't match? Ignore it
        else if(PathSplit(dfcInterface.strFile).strExt != strExt) continue;
        // Insert into files list
        else dliFiles.insert({ move(dfcInterface.strFile),
                               move(dfcInterface.dItem) });
        // ...until no more entries
      } while(dfcInterface.GetNextFile());
      // Remove '.' and '..' entries
      RemoveParentAndCurrentDirectory(dliDirs);
    } // Return list of files and directories
    return { move(dliDirs), move(dliFiles) };
  }
  /* -- Constructor of current directory without safety --------- */ protected:
  explicit Dir(DirFile &&dfList) :
    /* -- Initialisation of members ---------------------------------------- */
    DirFile{ move(dfList) }
    /* -- No code ---------------------------------------------------------- */
    { }
  /* -- Constructor of current directory without safety ------------ */ public:
  Dir(void) :
    /* -- Initialisation of members ---------------------------------------- */
    DirFile{ ScanDir() }
    /* -- No code ---------------------------------------------------------- */
    { }
  /* -- Constructor of specified directory without safety ------------------ */
  explicit Dir(const string &strDir) :
    /* -- Initialisation of members ---------------------------------------- */
    DirFile{ ScanDir(strDir) }
    /* -- No code ---------------------------------------------------------- */
    { }
  /* -- Constructor of specified dir without safety and file matching ------ */
  Dir(const string &strDir, const string &strExt) :
    /* -- Initialisation of members ---------------------------------------- */
    DirFile{ ScanDirExt(strDir, strExt) }
    /* -- No code ---------------------------------------------------------- */
    { }
  /* ----------------------------------------------------------------------- */
  DELETECOPYCTORS(Dir);                // Disable copy constructor and operator
};/* ----------------------------------------------------------------------- */
/* == Class to break apart urls ============================================ */
class Url
{ /* -- Public typedefs -------------------------------------------- */ public:
  enum Result                          // Result codes
  {/* ---------------------------------------------------------------------- */
    R_GOOD,                            // Url is good
    R_EMPTYURL,                        // Empty URL specified
    R_NOPROTO,                         // No protocol delimiter ':'
    R_INVPROTO,                        // Invalid protocol (not ://)
    R_EMPROTO,                         // Empty protocol after processing
    R_EMHOST,                          // Empty hostname
    R_EMPORT,                          // Empty port number
    R_INVPORT,                         // Invalid port number (1-65535)
    R_UNKPROTOPORT,                    // Unknown protocol with portr specified
    R_EMHOSTPORT,                      // Empty hostname after port parsed
    R_UNKPROTO,                        // Unknown protocol without port
    R_MAX                              // Maximum number of codes
  };/* -- Private variables --------------------------------------- */ private:
  Result           rResult;            // Result
  string           strProtocol;        // 'http' or 'https'
  string           strHost;            // Hostname
  string           strResource;        // The request uri
  string           strBookmark;        // Bookmark
  unsigned int     uiPort;             // Port number (1-65535)
  bool             bSecure;            // Connection would require SSL?
  /* -- Return data ------------------------------------------------ */ public:
  Result GetResult(void) const { return rResult; }
  const string &GetProtocol(void) const { return strProtocol; }
  const string &GetHost(void) const { return strHost; }
  const string &GetResource(void) const { return strResource; }
  const string &GetBookmark(void) const { return strBookmark; }
  unsigned int GetPort(void) const { return uiPort; }
  bool GetSecure(void) const { return bSecure; }
  /* -- Constructor -------------------------------------------------------- */
  explicit Url(const string &strUrl)
  { // Error if url is empty
    if(strUrl.empty()) { rResult = R_EMPTYURL; return; }
    // Find protocol and throw if error
    const size_t stProtPos = strUrl.find(':');
    if(stProtPos == string::npos) { rResult = R_NOPROTO; return; }
    for(size_t stPos = stProtPos + 1, stPosEnd = stPos + 1;
               stPos < stPosEnd;
             ++stPos)
      if(strUrl[stPos] != '/') { rResult = R_INVPROTO; return; }
    // Copy the protocol part and throw error if empty string
    strProtocol = strUrl.substr(0, stProtPos);
    if(strProtocol.empty()) { rResult = R_EMPROTO; return; }
    // Find hostname and if we couldn't find it?
    const size_t stUrlStartPos = stProtPos + 3;
    const size_t stHostPos = strUrl.find('/', stUrlStartPos);
    if(stHostPos == string::npos)
    { // Set default resource to root
      strResource = '/';
      // Finalise hostname
      strHost = strUrl.substr(stUrlStartPos);
    } // Request not found
    else
    { // Copy request part of url
      strResource = strUrl.substr(stHostPos);
      // Copy host part of url
      strHost = strUrl.substr(stUrlStartPos, stHostPos-stUrlStartPos);
    } // Check hostname not empty
    if(strHost.empty()) { rResult = R_EMHOST; return; }
    // Find port in hostname
    const size_t stPortPos = strHost.find(':');
    if(stPortPos != string::npos)
    { // Get port part of string and throw error if empty string
      const string strPort{ strHost.substr(stPortPos + 1) };
      if(strPort.empty()) { rResult = R_EMPORT; return; }
      // Convert port to number and throw error if invalid
      uiPort = ToNumber<unsigned int>(strPort);
      if(!uiPort || uiPort > 65535) { rResult = R_INVPORT; return; }
      // Set insecure if http was requested
      if(strProtocol == "http") bSecure = false;
      // Set secure if https was requested
      else if(strProtocol == "https") bSecure = true;
      // Unknown protocol error
      else { rResult = R_UNKPROTOPORT; return; }
      // Copy host part without port
      strHost.resize(stPortPos);
      if(strHost.empty()) { rResult = R_EMHOSTPORT; return; }
    } // Port number not found?
    else
    { // We have to find the port so set insecure port 80 if scheme is http
      if(strProtocol == "http") { uiPort = 80; bSecure = false; }
      // Set 443 if protocol is secure https
      else if(strProtocol == "https") { uiPort = 443; bSecure = true; }
      // Unknown protocol error
      else { rResult = R_UNKPROTO; return; }
    } // Find bookmark and if there is one?
    const size_t stBookmarkPos = strResource.find('#');
    if(stBookmarkPos != string::npos)
    { // Copy in bookmark string
      strBookmark = strResource.substr(stBookmarkPos + 1);
      // Trim resource
      strResource.resize(stBookmarkPos);
    } // Good result
    rResult = R_GOOD;
  }
};/* -- End of module namespace -------------------------------------------- */
/* -- Get current directory ------------------------------------------------ */
static const string DirGetCWD(void)
{ // On windows, we need to use unicode
#if defined(WINDOWS)
  // Storage of filename and initialise it to maximum path length
  wstring wstrDir; wstrDir.resize(_MAX_PATH);
  // Get current directory and store it in string, throw exception if error
  if(!_wgetcwd(const_cast<wchar_t*>(wstrDir.data()),
    static_cast<int>(wstrDir.capacity())))
      throw runtime_error{ "getcwd() failed!" };
  // Resize
  wstrDir.resize(wcslen(wstrDir.c_str()));
  // Return directory replacing backslashes for forward slashes
  return PSplitBackToForwardSlashes(WS16toUTF(wstrDir));
#else
  // Storage of filename and initialise it to maximum path length
  string strDir; strDir.resize(_MAX_PATH);
  // Get current directory and store it in string, throw exception if error
  if(!getcwd(const_cast<char*>(strDir.data()), strDir.capacity()))
    throw runtime_error{ "getcwd() failed!" };
  // Resize
  strDir.resize(strlen(strDir.c_str()));
  // Return directory
  return strDir;
#endif
}
/* == Set current directory ================================================ */
static bool DirSetCWD(const string &strDirectory)
{ // Ignore if empty
  if(strDirectory.empty()) return false;
  // Process is different on win32 with having drive letters
#if defined(WINDOWS)
  // Get first character because it needs casting
  const unsigned char &ucD = strDirectory.front();
  // Set drive first if specified
  if(strDirectory.length() >= 3 && strDirectory[1] == ':' &&
     (strDirectory[2] == '\\' || strDirectory[2] != '/') &&
       _chdrive((toupper(ucD) - 'A') + 1) < 0) return false;
  // Set directory and drive, return false if there is a problem
  return !_wchdir(UTFtoS16(strDirectory));
#else
  // It's far more easier on a unix system!
  return !chdir(strDirectory.c_str());
#endif
}
/* -- Make a directory ----------------------------------------------------- */
static bool DirMkDir(const string &strD) { return !STDMKDIR(strD); }
/* -- Remove a directory --------------------------------------------------- */
static bool DirRmDir(const string &strD) { return !STDRMDIR(strD); }
/* -- Make a directory and all it's interim components --------------------- */
static bool DirMkDirEx(const string &strDir)
{ // Break apart directory parts
  const PathSplit psParts{ strDir };
  // Break apart so we can check the directories. Will always be non-empty.
  const Token tParts{ Append(psParts.strDir, psParts.strFileExt), "/" };
  // This will be the string that wile sent to mkdir multiple times gradually.
  // Do not try to construct the oss with the drive string because it won't
  // work and thats not how the constructor works it seems!
  ostringstream osS; osS << psParts.strDrive;
  // Get the first item and if it is not empty?
  const string &strFirst = tParts.front();
  if(!strFirst.empty())
  { // Make the directory if isn't the drive and return failure if the
    // directory doesn't already exist
    if(!DirMkDir(strFirst) && IsNotErrNo(EEXIST)) return false;
    // Move first item. It will be empty if directory started with a slash
    osS << move(strFirst);
  } // If there are more directories?
  if(tParts.size() >= 2)
  { // Create all the other directories
    for(StrVectorConstIt svI{ next(tParts.cbegin(), 1) };
                         svI != tParts.cend();
                       ++svI)
    { // Append next directory
      osS << '/' << move(*svI);
      // Make the directory and if failed and it doesn't exist return error
      if(!DirMkDir(osS.str()) && IsNotErrNo(EEXIST)) return false;
    }
  } // Success
  return true;
}
/* -- Remove a directory and all it's interim components ------------------- */
static bool DirRmDirEx(const string &strDir)
{ // Break apart directory parts
  const PathSplit psParts{ strDir };
  // Break apart so we can check the directories. Will always be non-empty.
  Token tParts{ Append(psParts.strDir, psParts.strFileExt), "/" };
  // Get the first item and if it is not empty?
  while(!tParts.empty())
  { // This will be the string that wile sent to mkdir multiple times
    // gradually. Do not try to construct the oss with the drive string because
    // it won't work and thats not how the constructor works it seems!
    ostringstream osS; osS << psParts.strDrive;
    // Get the first item and if it is not empty?
    const string &strFirst = tParts.front();
    if(!strFirst.empty()) osS << strFirst;
    // If there are more directories? Build directory structure
    if(tParts.size() >= 2)
      for(StrVectorConstIt svI{ next(tParts.begin(), 1) };
                           svI != tParts.end();
                         ++svI)
        osS << '/' << *svI;
    // Make the directory and if failed and it doesn't exist return error
    if(!DirRmDir(osS.str()) && IsNotErrNo(EEXIST)) return false;
    // Remove the last item
    tParts.pop_back();
  } // Success
  return true;
}
/* -- Delete a file -------------------------------------------------------- */
static bool DirFileUnlink(const string &strF) { return !STDUNLINK(strF); }
/* -- Get file size - ------------------------------------------------------ */
static int DirFileSize(const string &strF, STDFSTATSTRUCT &sData)
  { return STDFSTAT(strF, &sData) ? GetErrNo() : 0; }
/* -- True if specified file has the specified mode ------------------------ */
static bool DirFileHasMode(const string &strF, const int iMode,
  const int iNegate)
{ // Get file information and and if succeeded?
  STDFSTATSTRUCT sData;
  if(!DirFileSize(strF, sData))
  { // If file attributes have specified mode then success
    if((sData.st_mode ^ iNegate) & iMode) return true;
    // Set error number
    SetErrNo(ENOTDIR);
  } // Failed
  return false;
}
/* -- True if specified file is actually a directory ----------------------- */
static bool DirLocalDirExists(const string &strF)
  { return DirFileHasMode(strF, _S_IFDIR, 0); }
/* -- True if specified file is actually a file ---------------------------- */
static bool DirLocalFileExists(const string &strF)
  { return DirFileHasMode(strF, _S_IFDIR, -1); }
/* -- Readable or writable? ------- Check if file is readable or writable -- */
static bool DirCheckFileAccess(const string &strF, const int iFlag)
  { return !STDACCESS(strF, iFlag); }
/* -- True if specified file exists and is readable ------------------------ */
static bool DirIsFileReadable(const string &strF)
  { return DirCheckFileAccess(strF, R_OK); }
/* -- True if specified file exists and is readable and writable ----------- */
static bool DirIsFileReadWriteable(const string &strF)
  { return DirCheckFileAccess(strF, R_OK|W_OK); }
/* -- True if specified file exists and is writable ------------------------ */
static bool DirIsFileWritable(const string &strF)
  { return DirCheckFileAccess(strF, W_OK); }
/* -- True if specified file exists and is executable ---------------------- */
static bool DirIsFileExecutable(const string &strF)
  { return DirCheckFileAccess(strF, X_OK); }
/* -- True if specified file or directory exists --------------------------- */
static bool DirLocalResourceExists(const string &strF)
   { return DirCheckFileAccess(strF, F_OK); }
/* -- Rename file ---------------------------------------------------------- */
static bool DirFileRename(const string &strS, const string &strD)
  { return !STDRENAME(strS, strD); }
/* -- Check that filename is valid and throw on error ---------------------- */
static void DirVerifyFileNameIsValid(const string &strFilename)
{ // Throw error if invalid name
  if(const ValidResult vrId = DirValidName(strFilename))
    XC("Filename is invalid!",
       "File",     strFilename,
       "Reason",   DirValidNameResultToString(vrId),
       "ReasonId", vrId);
}
/* ------------------------------------------------------------------------- */
class DirSaver
{ /* ----------------------------------------------------------------------- */
  const string strCWD;                 // Saved current directory
  /* --------------------------------------------------------------- */ public:
  explicit DirSaver(const string &strNWD) :
    /* --------------------------------------------------------------------- */
    strCWD{ DirGetCWD() }              // Save current working directory
    /* --------------------------------------------------------------------- */
    { DirSetCWD(strNWD); }
  /* ----------------------------------------------------------------------- */
  DirSaver(void) :                     // Save current working directory
    /* --------------------------------------------------------------------- */
    strCWD{ DirGetCWD() }
    /* --------------------------------------------------------------------- */
    { }
  /* ----------------------------------------------------------------------- */
  ~DirSaver(void) noexcept(false)
    /* --------------------------------------------------------------------- */
    { DirSetCWD(strCWD); }
};/* ----------------------------------------------------------------------- */
/* ------------------------------------------------------------------------- */
};                                     // End of interface
/* == EoF =========================================================== EoF == */

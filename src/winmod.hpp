/* == WINVER.HPP =========================================================== */
/* ######################################################################### */
/* ## MS-ENGINE              Copyright (c) MS-Design, All Rights Reserved ## */
/* ######################################################################### */
/* ## This is a Windows specific module that parses executable files to   ## */
/* ## reveal information about it.                                        ## */
/* ######################################################################### */
/* ========================================================================= */
#pragma once                           // Only one incursion allowed
/* -- System module class -------------------------------------------------- */
class SysModule :                      // Members initially private
  /* -- Base classes ------------------------------------------------------- */
  public SysModuleData                 // System module data
{ /* -- Get and store verison numbers -------------------------------------- */
  struct VersionNumbers                // Members initially public
  { /* -- Storage for version numbers -------------------------------------- */
    unsigned int uiMajor, uiMinor, uiBuild, uiRevision;
    /* -- Constructor ------------------------------------------------------ */
    explicit VersionNumbers(const wstring &wstrValue)
    { // Length of version info part
      UINT uiLength = 0;
      // Get version info. Return cleared information if failed.
      VS_FIXEDFILEINFO *lpFfi = nullptr;
      if(!VerQueryValueW(wstrValue.c_str(), L"\\",
        reinterpret_cast<LPVOID*>(&lpFfi), &uiLength))
      { // Clear version numbers
        uiMajor = SysErrorCode();
        uiMinor = uiRevision = uiBuild = static_cast<unsigned int>(-1);
        // Done
        return;
      } // Return if we did not read enough data
      if(uiLength < sizeof(VS_FIXEDFILEINFO)) return;
      // Create reference to version information
      const VS_FIXEDFILEINFO &rFfi = *lpFfi;
      uiMajor = HighWord(rFfi.dwFileVersionMS);
      uiMinor = LowWord(rFfi.dwFileVersionMS);
      uiBuild = HighWord(rFfi.dwFileVersionLS);
      uiRevision = LowWord(rFfi.dwFileVersionLS);
    }
    /* -- VersionNumbers::End ---------------------------------------------- */
  };                                   // End of VersionNumbers class
  /* -- Get and store string data ------------------------------------------ */
  struct VersionStrings                // Members initially public
  { /* -- Storage for version strings--------------------------------------- */
    string strDescription, strVendor, strCopyright;
    /* -- Get string value ---------------------------------------- */ private:
    const string GetStringValue(const wstring &wstrBlock,
      const wstring &wstrValue)
    { // Bail if no data
      if(wstrValue.empty() || wstrBlock.empty()) return {};
      // Get size of string
      UINT uiStrSize = static_cast<UINT>(wstrBlock.length());
      // Create substring and length containers
      wchar_t *wcpTitle = nullptr;
      // Retrieve file description for language and code page "i".
      if(!VerQueryValueW(wstrValue.c_str(), wstrBlock.c_str(),
        reinterpret_cast<LPVOID*>(&wcpTitle), &uiStrSize))
          return "Unknown";
      // Set string
      return S16toUTF(wcpTitle);
    }
    /* -- Constructor ---------------------------------------------- */ public:
    explicit VersionStrings(const wstring &wstrValue)
    { // Create language struct
      struct LANGANDCODEPAGE { WORD wLanguage, wCodePage; } *lcpData;
      // Length of version info part
      UINT uiLength = 0;
      // Read translation information strings and if failed?
      const wchar_t*const wcpLang = L"\\VarFileInfo\\Translation";
      if(!VerQueryValueW(wstrValue.c_str(), wcpLang,
        reinterpret_cast<LPVOID*>(&lcpData), &uiLength))
      { // Put error into data
        strDescription = Append("!E#", SysErrorCode());
        strVendor = S16toUTF(wcpLang);
        strCopyright = SysError();
        // Done
        return;
      } // Bail if no data
      if(uiLength < sizeof(struct LANGANDCODEPAGE)) return;
      // Clamp the limit to the lowest zero for safety.
      const size_t stLimit = static_cast<size_t>
        (floor(static_cast<double>(uiLength) /
          sizeof(struct LANGANDCODEPAGE)));
      // Read the file description for each language and code page.
      for(size_t stIndex = 0; stIndex < stLimit; ++stIndex)
      { // Get reference to version data struct and make language id from it
        const LANGANDCODEPAGE &lacpData = lcpData[stIndex];
        const LONG lLng = MakeDWord(lacpData.wLanguage, lacpData.wCodePage);
        // To help with retreiving some values
#define GSV(v,n) v = GetStringValue(UTFtoWS16( \
          Format("\\StringFileInfo\\$$$$$\\" n, \
            right, hex, setw(8), setfill('0'), lLng)), \
              wstrValue);
        // Get version, vendor and comments strings from module
        GSV(strDescription, "FileDescription");
        GSV(strVendor, "CompanyName");
        GSV(strCopyright, "Comments");
        // Done with this define
#undef GSV
      }
    }
    /* -- VersionStrings::End ---------------------------------------------- */
  };                                   // End of VersionStrings class
  /* -- Get executable version information size ---------------------------- */
  DWORD ReadSize(const string &strModule)
  { // Get size of version info structure. Done if succeeded
    DWORD dwDummy = 0;
    if(const DWORD dwSize =
      GetFileVersionInfoSizeW(UTFtoS16(strModule), &dwDummy))
        return dwSize;
    // Ignore if module has no resource section. This can be triggered when
    // using Wine as their DLL's don't have resource data sections.
    if(GetLastError() == ERROR_RESOURCE_DATA_NOT_FOUND) return 0;
    // All other errors must be thrown
    XCS("Unable to query the length of the version string inside "
        "the specified module!", "File", strModule);
  }
  /* -- Get version information--------------------------------------------- */
  const wstring ReadInfo(const string &strModule, const DWORD dwSize)
  { // Allocate memory for string and read data. Return string if succeeded!
    wstring wstrVI(dwSize, 0);
    if(GetFileVersionInfoW(UTFtoS16(strModule), 0, dwSize,
      ToNonConstCast<LPVOID>(wstrVI.data())))
        return wstrVI;
    // Failed to throw error
    XCS("Unable to query version information from the specified module!",
        "File", strModule, "Size", static_cast<unsigned int>(dwSize));
  }
  /* -- Return version information ----------------------------------------- */
  SysModuleData Load(const string &strModule)
  { // Read the size of the executable version info return nothing if empty
    if(const DWORD dwSize = ReadSize(strModule))
    { // Query version numbers and strings data
      const wstring wstrVersionInfo{ ReadInfo(strModule, dwSize) };
      VersionNumbers vnData{ wstrVersionInfo };
      VersionStrings vsData{ wstrVersionInfo };
      // Version numbers together has string
      string strVersionNumbers{ Format("$.$.$.$",
        vnData.uiMajor, vnData.uiMinor, vnData.uiBuild, vnData.uiRevision) };
      // Return data
      return SysModuleData{ strModule, vnData.uiMajor, vnData.uiMinor,
        vnData.uiBuild, vnData.uiRevision, std::move(vsData.strVendor),
        std::move(vsData.strDescription), std::move(vsData.strCopyright),
        std::move(strVersionNumbers) };
    } // Failed
    return SysModuleData{ strModule };
  }
  /* -- Return version information ----------------------------------------- */
  SysModuleData Load(string &&strModule)
  { // Read the size of the executable version info return nothing if empty
    if(const DWORD dwSize = ReadSize(strModule))
    { // Query version numbers and strings data
      const wstring wstrVersionInfo{ ReadInfo(strModule, dwSize) };
      VersionNumbers vnData{ wstrVersionInfo };
      VersionStrings vsData{ wstrVersionInfo };
      // Version numbers together has string
      string strVersionNumbers{ Format("$.$.$.$",
        vnData.uiMajor, vnData.uiMinor, vnData.uiBuild, vnData.uiRevision) };
      // Return data
      return SysModuleData{ std::move(strModule), vnData.uiMajor,
        vnData.uiMinor, vnData.uiBuild, vnData.uiRevision,
        std::move(vsData.strVendor), std::move(vsData.strDescription),
        std::move(vsData.strCopyright), std::move(strVersionNumbers) };
    } // Failed
    return SysModuleData{ std::move(strModule) };
  }
  /* -- Return data (copy filename) -------------------------------- */ public:
  explicit SysModule(const string &strModule) :
    /* -- Initialisation of members ---------------------------------------- */
    SysModuleData{ Load(strModule) }
    /* -- No code ---------------------------------------------------------- */
    { }
  /* -- Return data (move filename) ---------------------------------------- */
  explicit SysModule(string &&strModule) :
    /* -- Initialisation of members ---------------------------------------- */
    SysModuleData{ Load(std::move(strModule)) }
    /* -- No code ---------------------------------------------------------- */
    { }
};/* -- End ---------------------------------------------------------------- */
/* == EoF =========================================================== EoF == */

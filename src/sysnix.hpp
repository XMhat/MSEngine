/* == SYSNIX.HPP =========================================================== */
/* ######################################################################### */
/* ## MS-ENGINE              Copyright (c) MS-Design, All Rights Reserved ## */
/* ######################################################################### */
/* ## This is a Linux specific module that allows the engine to talk      ## */
/* ## to, and manipulate operating system procedures and funtions.        ## */
/* ######################################################################### */
/* ========================================================================= */
#pragma once                           // Only one incursion allowed
/* ------------------------------------------------------------------------- */
namespace SysBase {                    // Start of module namespace
/* -- Includes ------------------------------------------------------------- */
using namespace Lib::OS;               // Need operating system functions
/* -- Classes -------------------------------------------------------------- */
#include "pixbase.hpp"                 // Base system class
#include "pixcon.hpp"                  // Console terminal window class
#include "pixmod.hpp"                  // Module information class
#include "pixmap.hpp"                  // File mapping class
#include "pixpip.hpp"                  // Process output piping class
/* -- Namespaces ----------------------------------------------------------- */
using namespace IfVars;                // Using vars namespace
using namespace IfFStream;             // Using fstream namespace
/* == System intialisation helper ========================================== */
/* ######################################################################### */
/* ## Because we want to try and statically init const data as much as    ## */
/* ## possible, we need this class to derive by the System class so we    ## */
/* ## can make sure these functions are initialised first. Also making    ## */
/* ## this private prevents us from accessing these functions because     ## */
/* ## again - they are only for initialisation.                           ## */
/* ######################################################################### */
/* ------------------------------------------------------------------------- */
class SysProcess                       // Need this before of System init order
{ /* -- Streams ------------------------------------------------- */ protected:
  FStream          fsProcStat;         // Handle to proc/stat (cpu)
  FStream          fsProcStatM;        // Handle to proc/statm (memory)
  /* -- Processor ---------------------------------------------------------- */
  clock_t          ctUser;             // Last user cpu time
  clock_t          ctLow;              // Last low cpu time
  clock_t          ctSystem;           // Last system cpu time
  clock_t          ctIdle;             // Last clock idle time
  clock_t          ctProc;             // Last process cpu time
  clock_t          ctProcUser;         // Last process user cpu time
  clock_t          ctProcSys;          // Last process system cpu time
  /* -- Process ------------------------------------------------------------ */
  const size_t     stPageSize;         // Memory page size
  /* -- Process --------------------------------------------------- */ private:
  const pid_t      ullProcessId;       // Process id
  const pthread_t  vpThreadId;         // Thread id
  /* -- Return process and thread id ---------------------------- */ protected:
  template<typename IntType=decltype(ullProcessId)>IntType GetPid(void) const
    { return static_cast<IntType>(ullProcessId); }
  template<typename IntType=decltype(vpThreadId)>IntType GetTid(void) const
    { return static_cast<IntType>(vpThreadId); }
  /* ----------------------------------------------------------------------- */
  SysProcess(void) :
    /* -- Initialisers ----------------------------------------------------- */
    fsProcStat{ "/proc/stat",          // Open proc cpu stats stream
                FStream::FM_R_B },     // - Read/Binary mode
    fsProcStatM{ "/proc/self/statm",   // Open proc memory stats stream
                 FStream::FM_R_B },    // - Read/Binary mode
    ctUser(0),                         // Init user cpu time
    ctLow(0),                          // Init low cpu time
    ctSystem(0),                       // Init system cpu time
    ctIdle(0),                         // Init idle cpu time
    ctProc(0),                         // Init process cpu time
    ctProcUser(0),                     // Init user process cpu time
    ctProcSys(0),                      // Init system process cpu time
    stPageSize(sysconf(_SC_PAGESIZE)), // Get memory page size
    ullProcessId(getpid()),            // Get native process id
    vpThreadId(pthread_self())         // Get native thread id
    /* -- No code ---------------------------------------------------------- */
    { }
  /* ----------------------------------------------------------------------- */
  DELETECOPYCTORS(SysProcess);         // Supress copy constructor for safety
};/* == Class ============================================================== */
class SysCore :
  /* -- Base classes ------------------------------------------------------- */
  public SysProcess,                   // System process object
  public SysCon,                       // Defined in 'pixcon.hpp"
  public SysCommon                     // Common system object
{ /* -- Variables ------------------------------------------------- */ private:
  bool             bWindowInitialised; // Is window initialised?
  /* --------------------------------------------------------------- */ public:
  void UpdateMemoryUsageData(void)
  { // If the stat file is opened
    if(fsProcStatM.FStreamOpened())
    { // Read string from stat
      string strStat{ fsProcStatM.FStreamReadStringChunked() };
      // Find line feed
      const size_t stLF = strStat.find('\n');
      if(stLF != string::npos)
      { // Truncate the end of string. We only care about the top line.
        strStat.resize(stLF);
        // Reset memory value
        memData.stMProcUse = 0;
        // Grab tokens and enumerate them
        TokenListNC tStats{ strStat, cCommon->Space(), 8 };
        if(tStats.size() >= 3)
          memData.stMProcUse += ToNumber<size_t>(tStats[1]);
        // These are total pages, now lets multiply by the page size
        memData.stMProcUse *= stPageSize;
        // Check for new process peak
        if(memData.stMProcUse > memData.stMProcPeak)
          memData.stMProcPeak = memData.stMProcUse;
      } // Go back to start for next read
      fsProcStatM.FStreamRewind();
    } // Failed
    else memData.stMProcUse = 0;
    // Grab system memory information and if successful?
    struct sysinfo siData;
    if(!sysinfo(&siData))
      memData.qMTotal = siData.totalram,
      memData.qMFree = siData.freeram + siData.bufferram,
      memData.qMUsed = memData.qMTotal - memData.qMFree,
      memData.stMFree = Minimum(memData.qMFree, 0xFFFFFFFF),
      memData.dMLoad = MakePercentage(memData.qMUsed, memData.qMTotal);
    // Failed
    else memData.qMTotal = memData.qMFree = memData.qMUsed = 0,
         memData.stMFree = 0,
         memData.dMLoad = 0;
  }
  /* -- Terminate a process ------------------------------------------------ */
  bool TerminatePid(const unsigned int uiPid) const
    { return !kill(uiPid, SIGTERM); }
  /* -- Check if specified process id is running --------------------------- */
  bool IsPidRunning(const unsigned int uiPid) const
    { return !kill(uiPid, 0); }
  /* -- GLFW handles the icons on this ------------------------------------- */
  void UpdateIcons(void) { }
  /* ----------------------------------------------------------------------- */
  static bool LibFree(void*const vpModule)
    { return vpModule && !dlclose(vpModule); }
  /* ----------------------------------------------------------------------- */
  template<typename T>static T LibGetAddr(void*const vpModule,
    const char *cpName) { return vpModule ?
      reinterpret_cast<T>(dlsym(vpModule, cpName)) : nullptr; }
  /* ----------------------------------------------------------------------- */
  static void *LibLoad(const char*const cpName)
    { return dlopen(cpName, RTLD_LAZY | RTLD_LOCAL); }
  /* ----------------------------------------------------------------------- */
  const string LibGetName(void*const vpModule, const char *cpAltName) const
  { // Return nothing if no module
    if(!vpModule) return {};
    // Get information about the shared object
    struct Lib::OS::link_map *lmData;
    if(dlinfo(vpModule, RTLD_DI_LINKMAP, &lmData) || !lmData)
      XCL("Failed to read info about shared object!", "File", cpAltName);
    // Get full pathname of file
    return StdMove(PathSplit{ lmData->l_name, true }.strFull);
  }
  /* ----------------------------------------------------------------------- */
  void UpdateCPUUsageData(void)
  { // If the stat file is opened
    if(fsProcStat.FStreamOpened())
    { // Read string from stat
      string strStat{ fsProcStat.FStreamReadStringChunked() };
      // Find line feed
      const size_t stLF = strStat.find('\n');
      if(stLF != string::npos)
      { // Truncate the end of string. We only care about the top line.
        strStat.resize(stLF);
        // First item must be cpu and second should be empty. We created the
        // string so this tokeniser class is allowed to modify it for
        // increased performance of processing it.
        TokenListNC tStats{ strStat, cCommon->Space(), 6 };
        if(tStats.size() >= 5)
        { // Get idle time
          const clock_t cUserNow = ToNumber<clock_t>(tStats[2]);
          const clock_t cLowNow = ToNumber<clock_t>(tStats[3]);
          const clock_t cSystemNow = ToNumber<clock_t>(tStats[4]);
          const clock_t cIdleNow = ToNumber<clock_t>(tStats[5]);
          // Check for valid time
          if(cUserNow < ctUser || cLowNow < ctLow ||
             cSystemNow < ctSystem || cIdleNow < ctIdle)
               cpuUData.fdSystem = -1;
          // Valid time
          else
          { // Grab cpu usage jiffies
            clock_t cTotal = ((cUserNow - ctUser) + (cLowNow - ctLow) +
              (cSystemNow - ctSystem));
            // Set the percent to this
            cpuUData.fdSystem = cTotal;
            // Add the idle jiffies to the total jiffies
            cTotal += (cIdleNow - ctIdle);
            // Now we can calculate the true percent
            cpuUData.fdSystem = MakePercentage(cpuUData.fdSystem, cTotal);
          } // Update new times;
          ctUser = cUserNow, ctLow = cLowNow, ctSystem = cSystemNow,
            ctIdle = cIdleNow;
        } // No header
        else cpuUData.fdSystem = -2;
      } // No line feed
      else cpuUData.fdSystem = -3;
      // Go back to start for next read
      fsProcStat.FStreamRewind();
    } // Stat file is not opened
    else cpuUData.fdSystem = -3;
    // Get cpu times
    struct tms tmsData;
    const clock_t cProcNow = times(&tmsData);
    // If times are not valid? Show percent as error
    if(cProcNow <= ctProc || tmsData.tms_stime < ctProcSys ||
                            tmsData.tms_utime < ctProcUser)
      cpuUData.fdProcess = -1;
    // Times are valid
    else
    { // Caclulate total time
      cpuUData.fdProcess = (tmsData.tms_stime - ctProcSys) +
                          (tmsData.tms_utime - ctProcUser);
      // Divide by total cpu time
      cpuUData.fdProcess /= (cProcNow - ctProc);
      cpuUData.fdProcess /= thread::hardware_concurrency();
      cpuUData.fdProcess *= 100;
      // Update times
      ctProc = cProcNow, ctProcSys = tmsData.tms_stime,
        ctProcUser = tmsData.tms_utime;
    }
  }
  /* -- Seek to position in specified handle ------------------------------- */
  template<typename IntType>
    static IntType SeekFile(int iFp, const IntType itP)
      { return static_cast<IntType>
          (lseek64(iFp, static_cast<off64_t>(itP), SEEK_SET)); }
  /* -- Get executable size from header (N/A on Linux) --------------------- */
  static size_t GetExeSize(const string &strFile)
  { // Machine byte order check
#if defined(LITTLE_ENDIAN)
# define ELFDATANATIVE ELFDATA2LSB
#elif defined(BIG_ENDIAN)
# define ELFDATANATIVE ELFDATA2MSB
#endif
    // Open exe file and return on error
    if(FStream fExe{ strFile, FStream::FM_R_B })
    { // Read in the header
      Elf64_Ehdr ehData;
      if(const size_t stRead = fExe.FStreamRead(&ehData, sizeof(ehData)))
      { // We read enough bytes?
        if(stRead == sizeof(ehData))
        { // Rewind back to start
          if(!fExe.FStreamRewind())
            XCL("Failed to rewind executable file!", "File", strFile);
          // Get ELF data order type and if correct byte order?
          const unsigned int uiType = ehData.e_ident[EI_DATA];
          if(uiType == ELFDATA2LSB || uiType == ELFDATA2MSB)
          { // Is a 32-bit executable?
            const unsigned int uiClass = ehData.e_ident[EI_CLASS];
            if(uiClass == ELFCLASS32)
            { // Read in 32-bit header
              Elf32_Ehdr ehData32;
              if(const size_t stRead2 =
                fExe.FStreamRead(&ehData32, sizeof(ehData32)))
              { // We read enough bytes?
                if(stRead2 == sizeof(ehData32))
                { // Reverse bytes if not native
                  if(uiType != ELFDATANATIVE)
                    ehData.e_shoff = SWAP_U32(ehData32.e_shoff),
                    ehData.e_shentsize = SWAP_U16(ehData32.e_shentsize),
                    ehData.e_shnum = SWAP_U16(ehData32.e_shnum);
                  else ehData.e_shoff = ehData32.e_shoff,
                       ehData.e_shentsize = ehData32.e_shentsize,
                       ehData.e_shnum = ehData32.e_shnum;
                } // Failed to read enough bytes for ELF32 header
                else XC("Failed to read enough bytes for ELF32 header!",
                        "Requested", sizeof(ehData32), "Actual", stRead,
                        "File", strFile);
              } // Failed to read enough bytes for ELF32 header
              else XCL("Failed to read ELF32 header!",
                       "Requested", sizeof(ehData32), "File", strFile);
            } // Or is a 64-bit executable?
            else if(uiClass == ELFCLASS64)
            { // Read in 64-bit header
              Elf64_Ehdr ehData64;
              if(const size_t stRead2 =
                   fExe.FStreamRead(&ehData64, sizeof(ehData64)))
              { // We read enough bytes?
                if(stRead2 == sizeof(ehData64))
                { // Reverse bytes if not native
                  if(uiType != ELFDATANATIVE)
                    ehData.e_shoff = SWAP_U64(ehData64.e_shoff),
                    ehData.e_shentsize = SWAP_U16(ehData64.e_shentsize),
                    ehData.e_shnum = SWAP_U16(ehData64.e_shnum);
                  else ehData.e_shoff = ehData64.e_shoff,
                       ehData.e_shentsize = ehData64.e_shentsize,
                       ehData.e_shnum = ehData64.e_shnum;
                } // Failed to read enough bytes for ELF64 header
                else XC("Failed to read enough bytes for ELF64 header!",
                        "Requested", sizeof(ehData64), "Actual", stRead,
                        "File", strFile);
              } // Failed to read enough bytes for 64-bit header
              else XCL("Failed to read ELF64 header!",
                       "Requested", sizeof(ehData64), "File", strFile);
            } // Unknown executable type
            else XC("Invalid ELF header architecture!",
                  "Requested", ELFCLASS32, "OrRequested", ELFCLASS64,
                  "Actual", uiClass, "File", strFile);
            // Now we can return result
            return ehData.e_shoff + (ehData.e_shentsize * ehData.e_shnum);
          } // Failed to detect executable type
          else XC("Invalid ELF executable type!",
                  "Requested", ELFDATA2LSB, "OrRequested", ELFDATA2MSB,
                  "Actual", uiType, "File", strFile);
        } // Failed to read enough bytes
        else XC("Failed to read enough bytes for ELF header!",
                "Requested", sizeof(ehData), "Actual", stRead,
                "File", strFile);
      } // Failed to read elf ident
      else XCL("Failed to read ELF header!",
               "Requested", sizeof(ehData), "File", strFile);
    } // Failed to open executable file to read
    else XCL("Failed to open executable!", "File", strFile);
    // Done with this macro
#undef ELFDATANATIVE
  }
  /* -- Get executable file name ------------------------------------------- */
  const string GetExeName(void)
  { // Storage for executable name
    string strName; strName.resize(PATH_MAX);
    strName.resize(readlink("/proc/self/exe",
      const_cast<char*>(strName.data()), strName.size()));
    return strName;
  }
  /* -- Enum modules ------------------------------------------------------- */
  SysModList EnumModules(void)
  { // Make verison string
    string strVersion{ Append(sizeof(void*)*8, "-bit version") };
    // Mod list
    SysModList mlData;
    mlData.emplace(make_pair(static_cast<size_t>(0),
      SysModule{ GetExeName(), VER_MAJOR, VER_MINOR, VER_BUILD, VER_REV,
        VER_AUTHOR, VER_NAME, StdMove(strVersion), VER_STR }));
    // Module list which includes the executable module
    return mlData;
  }
  /* ----------------------------------------------------------------------- */
  OSData GetOperatingSystememData(void)
  { // Get operating system name
    struct utsname utsnData;
    if(uname(&utsnData)) XCS("Failed to read operating system information!");
    // Tokenize version numbers
    const Token tVersion{ utsnData.release, "." };
    // Get LANGUAGE code and set default if not found
    string strCode{ cCmdLine->GetEnv("LANGUAGE") };
    if(strCode.size() != 5)
    { // Get LANG code and set default if not found
      strCode = cCmdLine->GetEnv("LANG");
      if(strCode.size() < 5) strCode = "en-GB";
      // Language code was found?
      else
      { // Find a period (e.g. "en_GB.UTF8") and remove suffix it if found
        const size_t stPeriod = strCode.find('.');
        if(stPeriod != string::npos) strCode = strCode.substr(0, stPeriod);
      }
    } // Replace underscore with dash to be consistent with Windows
    if(strCode[2] == '_') strCode[2] = '-';
    // Return operating system info
    return { utsnData.sysname, cCommon->Blank(),
      tVersion.empty()    ? 0 : ToNumber<unsigned int>(tVersion[0]),
      tVersion.size() < 2 ? 0 : ToNumber<unsigned int>(tVersion[1]),
      tVersion.size() < 3 ? 0 : ToNumber<unsigned int>(tVersion[2]),
      sizeof(void*)*8, StdMove(strCode), DetectElevation(), false, 0,
      false };
  }
  /* ----------------------------------------------------------------------- */
  ExeData GetExecutableData(void) { return { 0, 0, false, false }; }
  /* ----------------------------------------------------------------------- */
  CPUData GetProcessorData(void)
  {  // Open cpu information file
    if(FStream fsCpuInfo{ "/proc/cpuinfo", FStream::FM_R_B })
    { // Read file and if we got data?
      const string strFile{ fsCpuInfo.FStreamReadStringChunked() };
      if(!strFile.empty())
      { // Parse the variables and if we got some?
        VarsConst vVars(strFile, GetTextFormat(strFile), ':');
        if(!vVars.empty())
        { // Read variables
          string strCpuId{ StdMove(vVars["model name"]) },
                 strSpeed{ StdMove(vVars["cpu MHz"]) },
                 strVendor{ StdMove(vVars["vendor_id"]) },
                 strFamily{ StdMove(vVars["cpu family"]) },
                 strModel{ StdMove(vVars["model"]) },
                 strStepping{ StdMove(vVars["stepping"]) };
          // Return default data we could not read
          return { strVendor.empty() ? "Unknown" : StdMove(strVendor),
                   strCpuId.empty() ? "Unknown" : StdMove(strCpuId),
                   strFamily.empty() &&
                   strModel.empty() &&
                   strStepping.empty() ? "Unknown" :
                     StdMove(Format("$ Family $ Model $ Stepping $",
                       StdMove(strVendor), StdMove(strFamily),
                       StdMove(strModel), StdMove(strStepping))),
                   thread::hardware_concurrency(),
                   strSpeed.empty() ? 0 : ToNumber<unsigned int>(strSpeed),
                   0, 0 };
        } // Failed to parse cpu variables
        else cLog->LogWarningSafe("Could not parse cpu information file!");
      } // Failed to read cpu info failed
      else cLog->LogWarningExSafe("Could not read cpu information file: $!",
        LocalError());
    } // Failed to open cpu info file
    else cLog->LogWarningExSafe("Could not open cpu information file: $!",
      LocalError());
    // Return default data we could not read
    return { "Unknown", "Unknown", "Unknown",
      thread::hardware_concurrency(), 0, 0, 0 };
  }
  /* ----------------------------------------------------------------------- */
  bool DebuggerRunning(void) const { return false; }
  /* -- Get process affinity masks (N/A on OSX) ---------------------------- */
  uint64_t GetAffinity(const bool bS)
  { // Get the process affinity mask
    cpu_set_t cstMask;
    if(sched_getaffinity(GetPid(), sizeof(cstMask), &cstMask))
      XCS("Failed to acquire process affinity!");
    // Return value
    uint64_t qwAffinity = 0;
    // Fill in the mask
    for(size_t stIndex = 0,
               stMaximum = Minimum(64, CPU_COUNT(&cstMask));
               stIndex < stMaximum;
             ++stIndex)
      qwAffinity |= CPU_ISSET(stIndex, &cstMask) << stIndex;
    // Return affinity mask
    return qwAffinity;
  }
  /* ----------------------------------------------------------------------- */
  int GetPriority(void)
  { // Get priority value and throw if failed
    const int iNice = getpriority(PRIO_PROCESS, GetPid());
    if(iNice == -1)
      XCS("Failed to acquire process priority!");
    // Return priority
    return iNice;
   }
  /* -- Return if running as root ------------------------------------------ */
  bool DetectElevation(void) { return getuid() == 0; }
  /* -- Return data from /dev/urandom -------------------------------------- */
  Memory GetEntropy(void) const
    { return FStream{ "/dev/random", FStream::FM_R_B }.
        FStreamReadBlock(1024); }
  /* ----------------------------------------------------------------------- */
  void *GetWindowHandle(void) const { return nullptr; }
  /* -- A window was created ----------------------------------------------- */
  void WindowInitialised(GlFW::GLFWwindow*const gwWindow)
    { bWindowInitialised = !!gwWindow; }
  /* -- Window was destroyed, nullify handles ------------------------------ */
  void SetWindowDestroyed(void) { bWindowInitialised = false; }
  /* -- Help with debugging ------------------------------------------------ */
  const char *HeapCheck(void) const { return "Not implemented!"; }
  /* ----------------------------------------------------------------------- */
  int LastSocketOrSysError(void) const { return GetErrNo(); }
  /* -- Build user roaming directory ---------------------------- */ protected:
  const string BuildRoamingDir(void) const
    { return cCmdLine->MakeEnvPath("HOME", "/.local"); }
  /* -- Constructor -------------------------------------------------------- */
  SysCore(void) :
    /* -- Initialisers ----------------------------------------------------- */
    SysCon{ EnumModules(), 0 },
    SysCommon{ GetExecutableData(),
               GetOperatingSystememData(),
               GetProcessorData() },
    bWindowInitialised(false)
    /* -- No code ---------------------------------------------------------- */
    { }
  /* ----------------------------------------------------------------------- */
  DELETECOPYCTORS(SysCore);            // Supress copy constructor for safety
}; /* ---------------------------------------------------------------------- */
}                                      // End of module namespace
/* == EoF =========================================================== EoF == */

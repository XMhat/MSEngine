/* == SYSMAC.HPP =========================================================== */
/* ######################################################################### */
/* ## MS-ENGINE              Copyright (c) MS-Design, All Rights Reserved ## */
/* ######################################################################### */
/* ## This is a MacOS specific module that allows the engine to talk      ## */
/* ## to, and manipulate operating system procedures and funtions.        ## */
/* ######################################################################### */
/* ========================================================================= */
#pragma once                           // Only one incursion allowed
/* == OSX includes ========================================================= */
#include <sys/sysctl.h>                // Kernel info stuff
#include <mach/mach_init.h>            // For getting cpu and memory usage
#include <mach/mach_error.h>           // For getting cpu usage
#include <mach/mach_host.h>            // For getting cpu and memory usage
#include <mach/vm_map.h>               // For getting cpu usage
#include <mach/vm_statistics.h>        // For getting memory usage
#include <mach/mach_types.h>           // For getting memory usage
#include <mach/mach.h>                 // For getting process memory usage
#include <mach/kern_return.h>          // For getting process memory usage
#include <mach/task_info.h>            // For getting process memory usage
#include <mach/mach_time.h>            // For getting system uptime
#include <mach-o/loader.h>             // For getting mach-o header format
#include <mach-o/fat.h>                // For getting mach-o fat header format
/* ------------------------------------------------------------------------- */
#include <termios.h>                   // For changing terminal settings
#include <libproc.h>                   // For getting program executable
/* ------------------------------------------------------------------------- */
#define _XOPEN_SOURCE_EXTENDED         // Unlock extended ncurses functionality
typedef void (*__sighandler_t)(int);   // For signal() on OSX
/* == Dependiencies ======================================================== */
#include "pixbase.hpp"                 // Base system class
#include "pixcon.hpp"                  // Console emulation class
#include "pixmod.hpp"                  // Version information class
#include "pixmap.hpp"                  // FileMap memory mapping class
#include "pixpip.hpp"                  // Process pipe handling
/* -- Includes ------------------------------------------------------------- */
using namespace IfGlFW;                // Using glfw interface
using namespace IfVars;                // Using vars interface
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
{ /* -- Private variables ----------------------------------------- */ private:
  const pid_t      ullProcessId;       // Process id
  const pthread_t  vpThreadId;         // Thread id
  /* -- Protected variables ------------------------------------- */ protected:
  const uint64_t   qwPageSize;         // Memory page size
  const mach_port_t mptHost, mptTask;  // Current mach host and task id
  /* -- Return process and thread id --------------------------------------- */
  template<typename IntType=decltype(ullProcessId)>IntType GetPid(void) const
    { return static_cast<IntType>(ullProcessId); }
  template<typename IntType=decltype(vpThreadId)>IntType GetTid(void) const
    { return static_cast<IntType>(BruteCast<const size_t>(vpThreadId)); }
  /* ----------------------------------------------------------------------- */
  SysProcess(void) :
    /* -- Initialisers ----------------------------------------------------- */
    ullProcessId(getpid()),            // Initialise process id number
    vpThreadId(pthread_self()),        // Initialise main thread id number
    qwPageSize(static_cast<uint64_t>   // Initialise memory page size
      (sysconf(_SC_PAGESIZE))),        // Usually 16k on M1 or 4k on Intel
    mptHost(mach_host_self()),         // Initialise host task
    mptTask(mach_task_self())          // Initialise self task
    /* -- No code ---------------------------------------------------------- */
    { }
  /* ----------------------------------------------------------------------- */
  DELETECOPYCTORS(SysProcess);         // Supress copy constructor for safety
};/* == Class ============================================================== */
class SysCore :
  /* -- Dependency classes ------------------------------------------------- */
  public SysCon,                       // Defined in 'pixcon.hpp'
  public SysProcess,                   // Process information class
  public SysVersion,                   // Version information class
  public SysCommon                     // Common functions class
{ /* -- Variables ------------------------------------------------- */ private:
  bool             bWindowInitialised; // Is window initialised?
  /* ----------------------------------------------------------------------- */
  const string GetSysCTLInfoString(const char *cpS)
  { // Get the size and return error if failed
    size_t stSize = 0;
    if(sysctlbyname(cpS, nullptr, &stSize, nullptr, 0) < 0) return "#ERR1#";
    // Resize and fill string
    string strOut; strOut.resize(stSize - 1);
    if(sysctlbyname(cpS, ToNonConstCast<char*>(strOut.c_str()),
      &stSize, nullptr, 0) < 0)
        return strBlank;
    // Return the string
    return strOut;
  }
  /* ----------------------------------------------------------------------- */
  template<typename T>const T GetSysCTLInfoNum(const char *cpS)
  { // Resize
    T tOut;
    // Size
    size_t stOut = sizeof(tOut);
    // Return the number
    return sysctlbyname(cpS, &tOut, &stOut, nullptr, 0) < 0 ? 0 : tOut;
  }
  /* --------------------------------------------------------------- */ public:
  void UpdateMemoryUsageData(void)
  { // More containers
    vm_statistics64_data_t vmsData;
    mach_msg_type_number_t mmtnType = sizeof(vmsData) / sizeof(natural_t);
    // Get total physical memory and if succeeded?
    if(host_statistics64(mptHost, HOST_VM_INFO64,
       reinterpret_cast<host_info_t>(&vmsData), &mmtnType) == KERN_SUCCESS)
    { // Succeeded getting info, now set the counters
      memData.qMFree = static_cast<uint64_t>(vmsData.free_count)
                     * qwPageSize;
      memData.qMUsed = (static_cast<uint64_t>(vmsData.active_count)
                     +  static_cast<uint64_t>(vmsData.inactive_count)
                     +  static_cast<uint64_t>(vmsData.wire_count))
                     * qwPageSize;
      memData.qMTotal = memData.qMFree + memData.qMUsed;
    } // For getting process info
    task_vm_info_data_t tvidData;
    mach_msg_type_number_t mmtnCount = TASK_VM_INFO_COUNT;
    // Get process memory usage
    if(task_info(mptTask, TASK_VM_INFO,
      reinterpret_cast<task_info_t>(&tvidData), &mmtnCount) == KERN_SUCCESS)
        memData.stMProcUse = static_cast<uint64_t>(tvidData.phys_footprint);
    // Set peak if breached
    if(memData.stMProcUse > memData.stMProcPeak)
      memData.stMProcPeak = memData.stMProcUse;
    // Calculate usage
    memData.dMLoad =
      static_cast<double>(memData.qMUsed) / memData.qMTotal * 100;
  }
  /* -- Send signal -------------------------------------------------------- */
  static int SendSignal(const unsigned int uiPid, const int iSignal)
    { return kill(static_cast<pid_t>(uiPid), iSignal); }
  /* -- Terminate a process ------------------------------------------------ */
  bool TerminatePid(const unsigned int uiPid) const
    { return !SendSignal(uiPid, SIGTERM); }
  /* -- Check if specified process id is running --------------------------- */
  bool IsPidRunning(const unsigned int uiPid) const
    { return !SendSignal(uiPid, 0); }
  /* -- GLFW handles the icons on this ------------------------------------- */
  void UpdateIcons(void) { }
  /* ----------------------------------------------------------------------- */
  static bool LibFree(void*const vpModule)
    { return vpModule && !dlclose(vpModule); }
  /* ----------------------------------------------------------------------- */
  template<typename T>
    static T LibGetAddr(void*const vpModule, const char *cpName)
      { return vpModule ? reinterpret_cast<T>(dlsym(vpModule, cpName)) :
          nullptr; }
  /* ----------------------------------------------------------------------- */
  static void *LibLoad(const char*const cpName)
    { return dlopen(cpName, RTLD_LAZY | RTLD_LOCAL); }
  /* ----------------------------------------------------------------------- */
  const string LibGetName(void*const vpModule, const char *cpAltName) const
  { // Return nothing if no module
    if(!vpModule) return {};
    // Get information about the shared object
    Dl_info dlData;
    if(!dladdr(vpModule, &dlData))
      XCL("Failed to read info about shared object!", "File", cpAltName);
    // Get full pathname of file
    return move(PathSplit{ dlData.dli_fname, true }.strFull);
  }
  /* ----------------------------------------------------------------------- */
  void UpdateCPUUsageData(void)
  { // For cpu information directly from operating system
    static mach_msg_type_number_t mmtnType = HOST_CPU_LOAD_INFO_COUNT;
    static host_cpu_load_info_data_t hclidData;
    // Get the usage and if succeeded?
    if(host_statistics(mptHost, HOST_CPU_LOAD_INFO,
      reinterpret_cast<host_info_t>(&hclidData), &mmtnType) == KERN_SUCCESS)
    { // Tick counters
      static unsigned long long qTotalTicksL, qIdleTicksL;
      // Calculate the total ticks
      unsigned long long qTotalTicks = 0;
      for(unsigned int uiI = 0; uiI < CPU_STATE_MAX; ++uiI)
        qTotalTicks += hclidData.cpu_ticks[uiI];
      // Do calculations
      unsigned long long
        qIdleTicks = hclidData.cpu_ticks[CPU_STATE_IDLE],
        qTotalTicksSinceLastTime = qTotalTicks - qTotalTicksL,
        qIdleTicksSinceLastTime = qIdleTicks - qIdleTicksL;
      // Set previous values
      qTotalTicksL = qTotalTicks;
      qIdleTicksL = qIdleTicks;
      // Final calculation
      cpuUData.fdSystem = (1.0 - ((qTotalTicksSinceLastTime > 0) ?
        (static_cast<double>(qIdleTicksSinceLastTime)) /
        qTotalTicksSinceLastTime : 0)) * 100;
    } // For retrieving CPU information about the process
    task_info_data_t tinfo;
    mach_msg_type_number_t task_info_count = TASK_INFO_MAX;
    // Get basic information about the current process
    if(task_info(mptTask, TASK_BASIC_INFO,
      static_cast<task_info_t>(tinfo), &task_info_count) != KERN_SUCCESS)
        return;
    // We have to calculate process CPU usage from all the threads
    thread_array_t         taThreads;
    mach_msg_type_number_t numThreads;
    thread_info_data_t     tidThread;
    mach_msg_type_number_t numThread;
    thread_basic_info_t    tbiInfo;
    // Get process thread list and if succeeded?
    if(task_threads(mptTask, &taThreads, &numThreads) != KERN_SUCCESS)
      return;
    // Cpu counters
    cpuUData.fdProcess = 0;
    // For each thread
    for(int iIndex = 0, iMax = IntOrMax<int>(numThreads);
            iIndex < iMax;
          ++iIndex)
    { // Set size of structure
      numThread = THREAD_INFO_MAX;
      // Get thread information and enumerate next thread if failed
      if(thread_info(taThreads[iIndex], THREAD_BASIC_INFO,
        static_cast<thread_info_t>(tidThread), &numThread) != KERN_SUCCESS)
          continue;
      // Set thread information
      tbiInfo = reinterpret_cast<thread_basic_info_t>(tidThread);
      // Ignore if thread not idle
      if(tbiInfo->flags & TH_FLAGS_IDLE) continue;
      // Now we can calculate cpu usage
      cpuUData.fdProcess += static_cast<double>(
        tbiInfo->cpu_usage / static_cast<double>(TH_USAGE_SCALE) *
          100.0);
    } // Deallocate thread list
    vm_deallocate(mptTask, reinterpret_cast<vm_offset_t>(taThreads),
      numThreads * sizeof(thread_t));
    // Divide by CPU thread count to make the value read like Windows and Linux
    cpuUData.fdProcess /= numThreads;
  }
  /* -- Seek to position in specified handle ------------------------------- */
  template<typename IntType>
    static IntType SeekFile(int iFp, const IntType itP)
      { return static_cast<IntType>
          (lseek(iFp, static_cast<off_t>(itP), SEEK_SET)); }
  /* -- Read command and segment data as big-endian ------------------------ */
  template<typename HdrType, class Swap32Type, class Swap64Type>
    static size_t GetExeSize(const char*const cpType, FStream &fExe)
  { // Rewind executable to start of file
    fExe.FStreamRewind();
    // Load specified header
    HdrType mhData;
    if(const size_t stRead = fExe.FStreamRead(&mhData, sizeof(mhData)))
    { // We read enough bytes?
      if(stRead == sizeof(mhData))
      { // Make sure byte order is correct
        mhData.ncmds = Swap32Type(mhData.ncmds).v;
        mhData.sizeofcmds = Swap32Type(mhData.sizeofcmds).v;
        // Highest executable position
        size_t stExeSize = 0;
        // Walk through each command
        for(unsigned int uiIndex = 0;
                         uiIndex < mhData.ncmds && fExe.FStreamIsNotEOF();
                       ++uiIndex)
        { // Save file position
          const int64_t qwCmdPos = fExe.FStreamGetPosition();
          // Create memory to store segment data
          load_command lcData;
          if(const size_t stReadCmd =
            fExe.FStreamRead(&lcData, sizeof(lcData)))
          { // We read enough bytes?
            if(stReadCmd == sizeof(lcData))
            { // Format command data
              lcData.cmd = Swap32Type(lcData.cmd).v;
              lcData.cmdsize = Swap32Type(lcData.cmdsize).v;
              // We are only interested in segments
              switch(lcData.cmd)
              { // 64-bit segment?
                case LC_SEGMENT_64:
                { // Move back to original offset
                  fExe.FStreamSetPosition(qwCmdPos, SEEK_SET);
                  // Read segment data
                  segment_command_64 scItem;
                  if(const size_t stReadSeg = fExe.FStreamRead(&scItem,
                    sizeof(scItem)))
                  { // We read enough bytes?
                    if(stReadSeg == sizeof(scItem))
                    { // Format segment data
                      scItem.fileoff = Swap64Type(scItem.fileoff).v;
                      scItem.filesize = Swap64Type(scItem.filesize).v;
                      // Get highest point in exe
                      const size_t stEnd = scItem.fileoff + scItem.filesize;
                      if(stEnd > stExeSize) stExeSize = stEnd;
                    } // Failed to read enough bytes for segment
                    else XC("Failed to read enough bytes for MACH-O 64 "
                            "segment!",
                            "Requested", sizeof(scItem),
                            "Actual",    stReadCmd,
                            "Type",      cpType,
                            "File",      fExe.IdentGet());
                  } // Failed to read segment bytes
                  else XCL("Failed to read MACH-O 64 segment!",
                           "Requested", sizeof(scItem), "Type", cpType,
                           "File",      fExe.IdentGet());
                  // Done
                  break;
                } // 32-bit segment?
                case LC_SEGMENT:
                { // Move back to original offset
                  fExe.FStreamSetPosition(qwCmdPos, SEEK_SET);
                  // Read segment data
                  segment_command scItem;
                  if(const size_t stReadSeg = fExe.FStreamRead(&scItem,
                    sizeof(scItem)))
                  { // We read enough bytes?
                    if(stReadSeg == sizeof(scItem))
                    { // Format needed segment data
                      scItem.fileoff = Swap32Type(scItem.fileoff).v;
                      scItem.filesize = Swap32Type(scItem.filesize).v;
                      // Get highest point in exe
                      const size_t stEnd = scItem.fileoff + scItem.filesize;
                      if(stEnd > stExeSize) stExeSize = stEnd;
                    } // Failed to read enough bytes for segment
                    else XC("Failed to read enough bytes for MACH-O 32 "
                            "segment!",
                            "Requested", sizeof(scItem),
                            "Actual",    stReadCmd,
                            "Type",      cpType,
                            "File",      fExe.IdentGet());
                  } // Failed to read segment bytes
                  else XCL("Failed to read MACH-O 32 segment!",
                           "Requested", sizeof(scItem), "Type", cpType,
                           "File",      fExe.IdentGet());
                  // Done
                  break;
                } // We're not supporting this command
                default: break;
              } // Move forward to next command position
              fExe.FStreamSetPosition(qwCmdPos + lcData.cmdsize, SEEK_SET);
            } // Failed to read enough bytes
            else XC("Failed to read enough bytes for MACH-O command!",
                    "Requested", sizeof(lcData), "Actual", stReadCmd,
                    "Type",      cpType,         "File",   fExe.IdentGet());
          } // Failed to read command bytes
          else XCL("Failed to read MACH-O command!",
                 "Requested", sizeof(lcData), "Type", cpType,
                 "File",      fExe.IdentGet());
        } // Return executable size
        return stExeSize;
      } // Failed to read enough bytes
      else XC("Failed to read enough bytes for MACH-O header!",
              "Requested", sizeof(mhData), "Actual", stRead,
              "Type",      cpType,         "File",   fExe.IdentGet());
    } // Failed to read elf ident
    else XCL("Failed to read MACH-O header!",
             "Requested", sizeof(mhData), "Type", cpType,
             "File",      fExe.IdentGet());
  }
  /* -- Read command and segment data as big-endian ------------------------ */
  template<typename ArchHdr, class SwapType>
    static size_t GetFatExeSize(const char*const cpType, FStream &fExe)
  { // Rewind executable to start of file
    fExe.FStreamRewind();
    // Load specified header
    fat_header fhData;
    if(const size_t stReadFat = fExe.FStreamRead(&fhData, sizeof(fhData)))
    { // We read enough bytes?
      if(stReadFat == sizeof(fhData))
      { // Highest executable position
        size_t stExeSize = 0;
        // Walk through all the executables
        for(size_t stMax =
          static_cast<size_t>(SwapType(fhData.nfat_arch).v),
                   stIndex = 0;
                   stIndex < stMax; ++stIndex)
        { // Read archive header
          ArchHdr faData;
          if(const size_t stReadArch =
            fExe.FStreamRead(&faData, sizeof(faData)))
          { // We read enough bytes?
            if(stReadArch == sizeof(faData))
            { // Format needed segment data
              faData.offset = SwapType(faData.offset).v;
              faData.size = SwapType(faData.size).v;
              // Get highest point in exe
              const size_t stEnd = faData.offset + faData.size;
              if(stEnd > stExeSize) stExeSize = stEnd;
            } // Failed to read enough bytes
            else XC("Failed to read enough bytes for FAT archive header!",
                    "Requested", sizeof(faData), "Actual", stReadArch,
                    "Type",      cpType,         "File",   fExe.IdentGet());
          } // Failed to read elf ident
          else XCL("Failed to read FAT archive header!",
                   "Requested", sizeof(faData), "Type", cpType,
                   "File",      fExe.IdentGet());
        } // Return executable size
        return stExeSize;
      } // Failed to read enough bytes
      else XC("Failed to read enough bytes for FAT header!",
              "Requested", sizeof(fhData), "Actual", stReadFat,
              "Type",      cpType,         "File",   fExe.IdentGet());
    } // Failed to read elf ident
    else XCL("Failed to read FAT header!",
             "Requested", sizeof(fhData), "Type", cpType,
             "File",      fExe.IdentGet());
  }
  /* -- Get executable size from header (N/A on OSX) ----------------------- */
  static size_t GetExeSize(const string &strFile)
  { // Open exe file and return on error
    if(FStream fExe{ strFile, FStream::FM_R_B })
    { // Machine byte order check
#if defined(LITTLE_ENDIAN)
# define MACHO_LE32 MH_MAGIC
# define MACHO_LE64 MH_MAGIC_64
# define MACHO_BE32 MH_CIGAM
# define MACHO_BE64 MH_CIGAM_64
# define MACHO_FAT_LE32 FAT_MAGIC
# define MACHO_FAT_LE64 FAT_MAGIC_64
# define MACHO_FAT_BE32 FAT_CIGAM
# define MACHO_FAT_BE64 FAT_CIGAM_64
#elif defined(BIG_ENDIAN)
# define MACHO_LE32 MH_CIGAM
# define MACHO_LE64 MH_CIGAM_64
# define MACHO_BE32 MH_MAGIC
# define MACHO_BE64 MH_MAGIC_64
# define MACHO_FAT_LE32 FAT_CIGAM
# define MACHO_FAT_LE64 FAT_CIGAM_64
# define MACHO_FAT_BE32 FAT_MAGIC
# define MACHO_FAT_BE64 FAT_MAGIC_64
#else
# error Unknown Endianness!
#endif
      // Load magic directly into integer
      unsigned int uiMagic;
      switch(const size_t stMagicBytes =
        fExe.FStreamRead(&uiMagic, sizeof(uiMagic)))
      { // Read enough bytes? Compare magic value
        case sizeof(uiMagic): switch(uiMagic)
        { // Little-endian 32-bit format executable
          case MACHO_LE32: return GetExeSize<mach_header,
            Swap32LEFunctor, Swap64LEFunctor>("32LE", fExe);
          // Little-endian 64-bit format executable
          case MACHO_LE64: return GetExeSize<mach_header_64,
            Swap32LEFunctor, Swap64LEFunctor>("64LE", fExe);
          // Big-endian 32-bit format executable
          case MACHO_BE32: return GetExeSize<mach_header,
            Swap32BEFunctor, Swap64BEFunctor>("32BE", fExe);
          // Big-endian 64-bit format executable
          case MACHO_BE64: return GetExeSize<mach_header_64,
            Swap32BEFunctor, Swap64BEFunctor>("64BE", fExe);
          // Little-endian 32-bit format executable
          case MACHO_FAT_LE32: return GetFatExeSize<fat_arch,
            Swap32LEFunctor>("FAT32LE", fExe);
          // Little-endian 64-bit format executable
          case MACHO_FAT_LE64: return GetFatExeSize<fat_arch_64,
            Swap64LEFunctor>("FAT64LE", fExe);
          // Big-endian 32-bit format executable
          case MACHO_FAT_BE32: return GetFatExeSize<fat_arch,
            Swap32BEFunctor>("FAT32BE", fExe);
          // Big-endian 64-bit format executable
          case MACHO_FAT_BE64: return GetFatExeSize<fat_arch_64,
            Swap64BEFunctor>("FAT64BE", fExe);
          // Invalid magic
          default: XC("MACH-O magic is invalid!",
                      "Magic", uiMagic, "File", strFile);
        } // Read nothing? Throw error with reason
        case 0: XCL("Failed to read MACH-O magic!",
                    "Requested", sizeof(uiMagic), "File", fExe.IdentGet());
        // Failed to read enough bytes
        default: XC("Failed to read enough bytes for MACH-O executable magic!",
                    "Requested", sizeof(uiMagic), "Actual", stMagicBytes,
                    "File", strFile);
      } // Done with these defines
#undef MACHO_FAT_BE64
#undef MACHO_FAT_BE32
#undef MACHO_FAT_LE64
#undef MACHO_FAT_LE32
#undef MACHO_BE32
#undef MACHO_BE64
#undef MACHO_LE32
#undef MACHO_LE64
    } // Failed to open mach executable
    XCL("Failed to open MACH-O executable!", "File", strFile);
  }
  /* -- Get executable file name ------------------------------------------- */
  const string GetExeName(void)
  { // Setup executable pathname
    string strExe; strExe.resize(PROC_PIDPATHINFO_MAXSIZE);
    // Get path to executable
    if(proc_pidpath(GetPid(),
      const_cast<char*>(strExe.c_str()), PROC_PIDPATHINFO_MAXSIZE) <= 0)
        XCS("Failed to get path to executable!",
            "Pid", GetPid(), "Buffer", strExe.capacity());
    // Set real size and trim the memory usage
    strExe.resize(strlen(strExe.c_str()));
    strExe.shrink_to_fit();
    // Return executable name
    return strExe;
  }
  /* -- Enum modules ------------------------------------------------------- */
  SysModList EnumModules(void)
  { // Make verison string
    string strVersion{ Append(sizeof(void*)*8, "-bit version") };
    // Mod list
    SysModList mlData;
    mlData.emplace(make_pair(static_cast<size_t>(0),
      SysModule{ GetExeName(), VER_MAJOR, VER_MINOR, VER_BUILD, VER_REV,
        VER_AUTHOR, VER_NAME, move(strVersion), VER_STR }));
    // Module list which includes the executable module
    return mlData;
  }
  /* ----------------------------------------------------------------------- */
  OSData GetOperatingSystemData(void)
  { // Get operating system name
    const Token tVersion{ GetSysCTLInfoString("kern.osproductversion"), "." };
    unsigned int uiMajor =
        tVersion.empty() ? 0 : ToNumber<unsigned int>(tVersion[0]),
      uiMinor = tVersion.size() < 2 ? 0 : ToNumber<unsigned int>(tVersion[1]),
      uiBuild = tVersion.size() < 3 ? 0 : ToNumber<unsigned int>(tVersion[2]);
    // Set operating system version string
    ostringstream osS; osS << "MacOS ";
    // Version information table
    struct OSListItem
    { // Label to append if verified
      const char*const cpLabel;
      // Major, minor and service pack of OS which applies to this label
      const unsigned int uiHi, uiLo;
      // Expiry date
      const STDTIMET ttExp;
    };
    // Handy converter at https://www.unixtimestamp.com/ and OS list data at...
    // https://en.wikipedia.org/wiki/List_of_Microsoft_Windows_versions.
    static const array<const OSListItem,20>osList{ {
      //   cpLevel       uiHi  uiLo  ttExp           Note
      { cpBlank,         14,   0,    1798675200 }, // ~31/12/2026
      { "Ventura",       13,   0,    1767139200 }, // ~31/12/2025
      { "Monterey",      12,   0,    1735603200 }, // ~31/12/2024
      { "Big Sur",       11,   0,    1703980800 }, // ~31/12/2023
      { "Catalina",      10,   15,   1667084400 }, // 30/10/2022 (EoL)
      { "Mojave",        10,   14,   1635116400 }, // 25/10/2021 (EoL)
      { "High Sierra",   10,   13,   1606780800 }, // 01/12/2020 (EoL)
      { "Sierra",        10,   12,   1569884400 }, // 01/10/2019 (EoL)
      { "El Capitan",    10,   11,   1543622400 }, // 01/12/2018 (EoL)
      { "Yosemite",      10,   10,   1501542000 }, // 01/08/2017 (EoL)
      { "Mavericks",     10,   9,    1480550400 }, // 01/12/2016 (EoL)
      { "Mountain Lion", 10,   8,    1441062000 }, // 01/09/2015 (EoL)
      { "Lion",          10,   7,    1412118000 }, // 01/10/2014 (EoL)
      { "Snow Leopard",  10,   6,    1393286400 }, // 25/02/2014 (EoL)
      { "Leopard",       10,   5,    1308783600 }, // 23/06/2011 (EoL)
      { "Tiger",         10,   4,    1251759600 }, // 01/09/2009 (EoL)
      { "Panther",       10,   3,    1167609600 }, // 01/01/2007 (EoL)
      { "Jaguar",        10,   2,    1136073600 }, // 01/01/2006 (EoL)
      { "Puma",          10,   1,    1104537600 }, // 01/01/2005 (EoL)
      { "Cheetah",       10,   0,    1072915200 }  // 01/01/2004 (EoL)
      //   cpLevel       uiHi  uiLo  uiBl  uiSp  ttExp           Note
    } };
    // Operating system expiry time
    STDTIMET ttExpiry;
    // Iterate through the versions and try to find a match for the
    // versions above. 'Unknown' is caught if none are found.
    for(const OSListItem &osItem : osList)
    { // Ignore if this version item doesn't match
      if(uiMajor < osItem.uiHi || uiMinor < osItem.uiLo) continue;
      // Set operating system version
      osS << osItem.cpLabel;
      // Set expiry time
      ttExpiry = osItem.ttExp;
      // Skip adding version numbers
      goto SkipNumericalVersionNumber;
    } // No expiry time
    ttExpiry = 0;
    // Nothing was found so add version number detected
    osS << uiMajor << '.' << uiMinor;
    // Label for when we found the a matching version
    SkipNumericalVersionNumber:
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
        if(stPeriod != string::npos) strCode.resize(stPeriod);
      }
    } // Replace underscore with dash to be consistent with Windows
    if(strCode[2] == '_') strCode[2] = '-';
    // Get operating system kernel name
    string strExtra;
    struct utsname utsnData;
    if(!uname(&utsnData))
      strExtra = Append(utsnData.sysname, " v", utsnData.release);
    // Return operating system info
    return {
      osS.str(),                       // Version string
      move(strExtra),                  // Extra version string
      uiMajor,                         // Major OS version
      uiMinor,                         // Minor OS version
      uiBuild,                         // OS build version
      sizeof(void*)*8,                 // 32 or 64 OS arch
      move(strCode),                   // Get locale
      DetectElevation(),               // Elevated?
      false,                           // Wine or Old OS?
      ttExpiry,                        // OS expiry
      cmSys.GetTimeS() >= ttExpiry     // OS has expired?
    };
  }
  /* ----------------------------------------------------------------------- */
  ExeData GetExecutableData(void)
  { // Suffix to test against
    const string strMacSig{ ".app/Contents/MacOS/" };
    // If executable directory matches this
    bool bIsBundled = ENGLoc().length() > strMacSig.length() &&
      ENGLoc().substr(ENGLoc().length() - strMacSig.length()) == strMacSig;
    // Return result
    return { 0, 0, false, bIsBundled };
  }
  /* ----------------------------------------------------------------------- */
  CPUData GetProcessorData(void)
  { // Get processor information
    const size_t stCpuCount = thread::hardware_concurrency();
    // Using arm cpu?
#if defined(RISC)
    const unsigned int
      ulFeatureSet = GetSysCTLInfoNum<unsigned int>("hw.cpufamily"),
      ulPlatformId = GetSysCTLInfoNum<unsigned int>("hw.cputype");
    const string strIdentifier{
      Format("Arm64 Family $ Model $",
        GetSysCTLInfoNum<uint32_t>("hw.cpusubfamily"),
        GetSysCTLInfoNum<uint32_t>("hw.cpusubtype"))
    }, strProcessorName{ GetSysCTLInfoString("machdep.cpu.brand_string") },
       strVendorId{ "Apple" };
    // Processor speeds common speeds (lowest vs highest speed).
    typedef array<const unsigned int, 2> UIntDouble;
    const UIntDouble uidM1{ { 2064, 3228 } }, // Apple M1
                     uidM2{ { 2420, 3480 } }; // Apple M2
    // Processor table with speeds. This is because there is no API to get
    // the speed of Apple branded processors.
    const map<const string, const UIntDouble&> smList{
      { "Apple M1",       uidM1 }, { "Apple M1 Pro",   uidM1 },
      { "Apple M1 Max",   uidM1 }, { "Apple M1 Ultra", uidM1 },
      { "Apple M2",       uidM2 }, { "Apple M2 Pro",   uidM2 },
      { "Apple M2 Max",   uidM2 }, { "Apple M2 Ultra", uidM2 }
    };
    // Find processor name to speed table and if we found it? Then copy the
    // value from the table as the actual speed.
    const auto smListIt{ smList.find(strProcessorName) };
    const unsigned int uiSpeed = smListIt == smList.cend() ?
      static_cast<unsigned int>
        (GetSysCTLInfoNum<uint64_t>("hw.tbfrequency")/10000) :
      smListIt->second[1];
    // Using INTEL processor?
#elif defined(CISC)
    const unsigned int
      uiSpeed = GetSysCTLInfoNum<uint64_t>("hw.cpufrequency")/1000000,
      ulFeatureSet = GetSysCTLInfoNum<uint64_t>("machdep.cpu.feature_bits"),
      ulPlatformId = GetSysCTLInfoNum<uint32_t>("machdep.cpu.signature");
    const string strIdentifier{
      Format("Intel$ Family $ Model $ Stepping $",
        sizeof(void*) == 8 ? "64" : strBlank,
        GetSysCTLInfoNum<uint32_t>("machdep.cpu.family"),
        GetSysCTLInfoNum<uint32_t>("machdep.cpu.model"),
        GetSysCTLInfoNum<uint32_t>("machdep.cpu.stepping"))
    }, strProcessorName{ GetSysCTLInfoString("machdep.cpu.brand_string") },
       strVendorId{ GetSysCTLInfoString("machdep.cpu.vendor") };
#endif
    // Return default data we could not read
    return { move(strVendorId),        move(strProcessorName),
             move(strIdentifier),      stCpuCount,
             uiSpeed,                  ulFeatureSet,
             ulPlatformId };
  }
  /* ----------------------------------------------------------------------- */
  bool DebuggerRunning(void) const
  { // Kernel process info
    struct kinfo_proc kipInfo;
    kipInfo.kp_proc.p_flag = 0;
    // Setup request parameters
    array<int,4> iaParams{ CTL_KERN, KERN_PROC, KERN_PROC_PID, GetPid() };
    // Check see if we're running a debugger
    size_t stSize = sizeof(kipInfo);
    sysctl(iaParams.data(), iaParams.size(), &kipInfo, &stSize, nullptr, 0);
    // Return status
    return (kipInfo.kp_proc.p_flag & P_TRACED) != 0;
  }
  /* -- Get process affinity masks (N/A on OSX) ---------------------------- */
  uint64_t GetAffinity(const bool) const
    { return BitsToMask<uint64_t>(thread::hardware_concurrency()); }
  /* ----------------------------------------------------------------------- */
  int GetPriority(void) const
  { // Get priority value and throw if failed
    const int iNice = getpriority(PRIO_PROCESS, GetPid<unsigned int>());
    if(iNice == -1)
      XCS("Failed to acquire process priority!", "Pid", GetPid());
    // Return priority
    return iNice;
   }
  /* -- Return if running as root ----------------------------------------- */
  bool DetectElevation(void) { return getuid() == 0; }
  /* -- Return data from /dev/random -------------------------------------- */
  Memory GetEntropy(void) const
    { return FStream{ "/dev/random", FStream::FM_R_B }.
        FStreamReadBlock(1024); }
  /* ----------------------------------------------------------------------- */
  void *GetWindowHandle(void) const { return nullptr; }
  /* -- A window was created ----------------------------------------------- */
  void WindowInitialised(GLFWwindow*const gwWindow)
    { bWindowInitialised = !!gwWindow; }
  /* -- Window was destroyed, nullify handles ------------------------------ */
  void SetWindowDestroyed(void) { bWindowInitialised = false; }
  /* -- Help with debugging ------------------------------------------------ */
  const char *HeapCheck(void) const { return "Not implemented!"; }
  /* ----------------------------------------------------------------------- */
  int LastSocketOrSysError(void) const { return GetErrNo(); }
  /* -- Build user roaming directory ---------------------------- */ protected:
  const string BuildRoamingDir(void) const
    { return cCmdLine->MakeEnvPath("HOME", "/Library/Application Support"); }
  /* -- Constructor -------------------------------------------------------- */
  SysCore(void) :
    /* -- Initialisation of members ---------------------------------------- */
    SysVersion{ EnumModules(), 0 },
    SysCommon{ GetExecutableData(),
               GetOperatingSystemData(),
               GetProcessorData() },
    bWindowInitialised(false) { }
  /* ----------------------------------------------------------------------- */
  DELETECOPYCTORS(SysCore);            // Supress copy constructor for safety
}; /* ---------------------------------------------------------------------- */
/* == EoF =========================================================== EoF == */

/* == STD.HPP ============================================================== */
/* ######################################################################### */
/* ## MS-ENGINE              Copyright (c) MS-Design, All Rights Reserved ## */
/* ######################################################################### */
/* ## This module contains common functions to target specific functions. ## */
/* ######################################################################### */
/* ========================================================================= */
#pragma once                           // Only one incursion allowed
/* ------------------------------------------------------------------------- */
namespace IfStd {                      // Start of module namespace
/* ------------------------------------------------------------------------- */
#if defined(WINDOWS)                   // Using Windows?
/* -- Entry function and arguments ----------------------------------------- */
#define ENTRYFUNC    WINAPI _tWinMain(HINSTANCE, HINSTANCE, LPTSTR, int)
#define ARGTYPE      TCHAR             // Widestring for args
#define CONENTRYFUNC _tmain            // For project management utility
/* -- STL multithread functions -------------------------------------------- */
#define MYFILL(p,b,e,...)        std::fill(p, b, e, __VA_ARGS__)
#define MYTRANSFORM(p,b,e,o,...) std::transform(p, b, e, o, __VA_ARGS__)
#define MYFOREACH(p,b,e,...)     std::for_each(p, b, e, __VA_ARGS__)
#define MYCOPY(p,b,e,...)        std::copy(p, b, e, __VA_ARGS__)
/* -- These don't exist on Windows so we'll make it easier ----------------- */
#define R_OK                         4 // File is readable (StdAccess)
#define W_OK                         2 // File is writable (StdAccess)
#define X_OK                         1 // File is executable (StdAccess)
#define F_OK                         0 // File exists (StdAccess)
/* -- UTF8 handling -------------------------------------------------------- */
#define S16toUTF(m)  IfUtf::FromWideStringPtr(m) // Widestring to utf8 str
#define WS16toUTF(m) S16toUTF((m).c_str())       //    " to utf8 cstr
#define UTFtoWS16(m) IfUtf::Decoder{ m }.Wide()  // utf8 to widestring str
#define UTFtoS16(m)  UTFtoWS16(m).c_str()        //    " to widestring cstr
/* -- Typedefs for structure names that are different on Windows ----------- */
typedef struct __stat64 StdFStatStruct; // Different on Windows
typedef struct tm       StdTMStruct;    // Different on Windows
typedef __time64_t      StdTimeT;       // Different on Windows
/* -- Typedefs for types not in Windows ------------------------------------ */
typedef std::make_signed<size_t>::type ssize_t; // Not in MSVC
/* -- Wrapper for _waccess() ----------------------------------------------- */
static int StdAccess(const wchar_t*const wcpPath, const int iMode)
  { return _waccess(wcpPath, iMode); }
static int StdAccess(const string &strPath, const int iMode)
  { return StdAccess(UTFtoS16(strPath), iMode); }
/* -- Wrapper for mkdir() function ----------------------------------------- */
static int StdMkDir(const wchar_t*const wcpPath) { return _wmkdir(wcpPath); }
static int StdMkDir(const string &strPath)
  { return StdMkDir(UTFtoS16(strPath)); }
/* -- Wrapper for _wrename() function -------------------------------------- */
static int StdRename(const wchar_t*const wcpSrcPath,
  const wchar_t*const wcpDstPath)
    { return _wrename(wcpSrcPath, wcpDstPath); }
static int StdRename(const string &strSrcPath, const string &strDstPath)
  { return StdRename(UTFtoS16(strSrcPath), UTFtoS16(strDstPath)); }
/* -- Wrapper for _wrmdir() function --------------------------------------- */
static int StdRmDir(const wchar_t*const wcpPath) { return _wrmdir(wcpPath); }
static int StdRmDir(const string &strPath)
  { return StdRmDir(UTFtoS16(strPath)); }
/* -- Wrapper for _wunlink() function -------------------------------------- */
static int StdUnlink(const wchar_t*const wcpPath) { return _wunlink(wcpPath); }
static int StdUnlink(const string &strPath)
  { return StdUnlink(UTFtoS16(strPath)); }
/* -- Wrapper for _wexecve() stdlib function ------------------------------- */
static int StdExecVE(const wchar_t*const wcpaArg[],
  const wchar_t*const wcpaEnv[])
{ return static_cast<int>(_wexecve(*wcpaArg, wcpaArg, wcpaEnv)); }
/* -- Wrapper for _fileno() function --------------------------------------- */
static int StdFileNo(FILE*const fStream) { return _fileno(fStream); }
/* -- Wrapper for _wstat64() function -------------------------------------- */
static int StdFStat(const wchar_t*const wcpPath,
  StdFStatStruct*const sDestBuffer)
{ return _wstat64(wcpPath, sDestBuffer);  }
static int StdFStat(const string &strPath, StdFStatStruct*const sDestBuffer)
  { return StdFStat(UTFtoS16(strPath), sDestBuffer);  }
/* -- Wrapper for _fseeki64() function ------------------------------------- */
static int StdFSeek(FILE*const fStream, const int64_t qwOffset, int iWhence)
  { return _fseeki64(fStream, qwOffset, iWhence); }
/* -- Wrapper for _ftelli64() function ------------------------------------- */
static int64_t StdFTell(FILE*const fStream) { return _ftelli64(fStream); }
/* -- Wrapper for _gmtime64_s() function ----------------------------------- */
static void StdGMTime(StdTMStruct*const tmpResult, const StdTimeT*const tpTime)
  { _gmtime64_s(tmpResult, tpTime); }
/* -- Wrapper for _fstat64() function -------------------------------------- */
static int StdHStat(const int iFd, StdFStatStruct*const sDestBuffer)
  { return _fstat64(iFd, sDestBuffer); }
/* -- Wrapper for _localtime64_s() function -------------------------------- */
static void StdLocalTime(StdTMStruct*const tmpResult,
  const StdTimeT*const tpTime)
{ _localtime64_s(tmpResult, tpTime); }
/* -- Wrapper for mktime() function ---------------------------------------- */
static StdTimeT StdMkTime(StdTMStruct*const tmpResult)
  { return _mktime64(tmpResult); }
/* -- Wrapper for _wpopen() function --------------------------------------- */
static FILE *StdPOpen(const wchar_t*const wcpCommand,
  const wchar_t*const wcpType=L"rt") { return _wpopen(wcpCommand, wcpType); }
[[maybe_unused]] static FILE *StdPOpen(const string &strCommand,
  const wchar_t*const wcpType=L"rt")
{ return StdPOpen(UTFtoS16(strCommand), wcpType); }
/* -- Wrapper for _pclose() function --------------------------------------- */
[[maybe_unused]] static int StdPClose(FILE*const fStream)
  { return _pclose(fStream); }
/* -- Wrapper for srandom() function --------------------------------------- */
static void StdSRand(const unsigned int uiSeed) { srand(uiSeed); }
/* ------------------------------------------------------------------------- */
#else                                  // Posix compatible target?
/* -- Entry function and arguments for anything but Windows ---------------- */
# define ARGTYPE      char             // Main() arguments type
# define ENTRYFUNC    main(int __argc,ARGTYPE**__wargv,ARGTYPE**_wenviron)
# define CONENTRYFUNC main             // For project management utility
/* -- STL multithread functions -------------------------------------------- */
#if defined(MACOS)                     // Using MacOS? (unsupported)
# define MYFILL(p,b,e,...)        std::fill(b, e, __VA_ARGS__)
# define MYTRANSFORM(p,b,e,o,...) std::transform(b, e, o, __VA_ARGS__)
# define MYFOREACH(p,b,e,...)     std::for_each(b, e, __VA_ARGS__)
# define MYCOPY(p,b,e,...)        std::copy(b, e, __VA_ARGS__)
#else                                  // Using Linux? (supported)
# define MYFILL(p,b,e,...)        std::fill(p, b, e, __VA_ARGS__)
# define MYTRANSFORM(p,b,e,o,...) std::transform(p, b, e, o, __VA_ARGS__)
# define MYFOREACH(p,b,e,...)     std::for_each(p, b, e, __VA_ARGS__)
# define MYCOPY(p,b,e,...)        std::copy(p, b, e, __VA_ARGS__)
#endif                                 // End of POSIX OS check
/* -- Maximum path part lengths -------------------------------------------- */
#define _MAX_PATH          PATH_MAX    // Maximum size of all path parts
#define _MAX_DIR           PATH_MAX    // Maximum size of directory part
#define _MAX_FNAME         PATH_MAX    // Maximum size of filename part
#define _MAX_EXT           PATH_MAX    // Maximum size of extension part
/* -- Other cross compatibilities ------------------------------------------ */
#define _S_IFDIR           S_IFDIR     // Specifies a directory for stat()
/* -- Critical error types ------------------------------------------------- */
#define MB_SYSTEMMODAL     0           // No window was available for message
#define MB_ICONINFORMATION 1           // Informational message only
#define MB_ICONEXCLAMATION 2           // A warning message occured
#define MB_ICONSTOP        4           // A critical error message occured
/* -- UTF8 handling (not applicable on this target) ------------------------ */
#define UTFtoWS16(m)       m           // Filenames already treated as UTF-8
#define UTFtoS16(m)        m           // Filenames already treated as UTF-8
#define WS16toUTF(m)       m           // Filenames already treated as UTF-8
#define S16toUTF(m)        m           // Filenames already treated as UTF-8
/* -- Typedefs for structure names that are different on Windows ----------- */
typedef struct stat StdFStatStruct;    // Different on Windows
typedef struct tm   StdTMStruct;       // Different on Windows
typedef time_t      StdTimeT;          // Different on Windows
/* -- Wrapper for access() stdlib function --------------------------------- */
static int StdAccess(const char*const cpPath, const int iMode)
  { return access(cpPath, iMode); }
static int StdAccess(const string &strPath, const int iMode)
  { return StdAccess(strPath.c_str(), iMode); }
/* -- Wrapper for mkdir() function ----------------------------------------- */
static int StdMkDir(const char*const cpPath)
  { return mkdir(cpPath,
      S_IRUSR|S_IWUSR|S_IXUSR|S_IRGRP|S_IXGRP|S_IROTH|S_IXOTH); }
static int StdMkDir(const string &strPath)
  { return StdMkDir(strPath.c_str()); }
/* -- Wrapper for rename() function ---------------------------------------- */
static int StdRename(const char*const cpSrcPath, const char*const cpDstPath)
  { return rename(cpSrcPath, cpDstPath); }
static int StdRename(const string &strSrcPath, const string &strDstPath)
  { return StdRename(strSrcPath.c_str(), strDstPath.c_str()); }
/* -- Wrapper for rmdir() function ----------------------------------------- */
static int StdRmDir(const char*const cpPath) { return rmdir(cpPath); }
static int StdRmDir(const string &strPath)
  { return StdRmDir(strPath.c_str()); }
/* -- Wrapper for unlink() function ---------------------------------------- */
static int StdUnlink(const char*const cpPath) { return unlink(cpPath); }
static int StdUnlink(const string &strPath)
  { return StdUnlink(strPath.c_str()); }
/* -- Wrapper for execve() stdlib function --------------------------------- */
static int StdExecVE(const char*const cpaArg[], const char*const cpaEnv[])
  { return execve(*cpaArg, const_cast<char**>(cpaArg),
      const_cast<char**>(cpaEnv)); }
/* -- Wrapper for fileno() function ---------------------------------------- */
static int StdFileNo(FILE*const fStream) { return fileno(fStream); }
/* -- Wrapper for stat() function ------------------------------------------ */
static int StdFStat(const char*const cpPath, StdFStatStruct*const sDestBuffer)
  { return stat(cpPath, sDestBuffer);  }
static int StdFStat(const string &strPath, StdFStatStruct*const sDestBuffer)
  { return StdFStat(strPath.c_str(), sDestBuffer);  }
/* -- Wrapper for fseek() function ----------------------------------------- */
static int StdFSeek(FILE*const fStream, const off_t otOffset, int iWhence)
  { return fseeko(fStream, otOffset, iWhence); }
/* -- Wrapper for ftell() function ----------------------------------------- */
static off_t StdFTell(FILE*const fStream) { return ftello(fStream); }
/* -- Wrapper for gmtime_r() function -------------------------------------- */
static void StdGMTime(StdTMStruct*const tmpResult, const StdTimeT*const tpTime)
  { gmtime_r(tpTime, tmpResult); }
/* -- Wrapper for fstat() function ----------------------------------------- */
static int StdHStat(const int iFd, StdFStatStruct*const sDestBuffer)
  { return fstat(iFd, sDestBuffer); }
/* -- Wrapper for localtime_r() function ----------------------------------- */
static void StdLocalTime(StdTMStruct*const tmpResult,
  const StdTimeT*const tpTime)
{ localtime_r(tpTime, tmpResult); }
/* -- Wrapper for mktime() function ---------------------------------------- */
static StdTimeT StdMkTime(StdTMStruct*const tmpResult)
  { return mktime(tmpResult); }
/* -- Wrapper for popen() function ----------------------------------------- */
static FILE *StdPOpen(const char*const cpCommand,
  const char*const cpType="r") { return popen(cpCommand, cpType); }
[[maybe_unused]] static FILE *StdPOpen(const string &strCommand,
  const char*const cpType="r") { return StdPOpen(strCommand.c_str(), cpType); }
/* -- Wrapper for pclose() function ---------------------------------------- */
[[maybe_unused]] static int StdPClose(FILE*const fStream)
  { return pclose(fStream); }
/* -- Wrapper for srandom() function --------------------------------------- */
static void StdSRand(const unsigned int uiSeed) { srandom(uiSeed); }
/* ------------------------------------------------------------------------- */
#endif                                 // Operating system check
/* ------------------------------------------------------------------------- */
};                                     // End of module namespace
/* ------------------------------------------------------------------------- */
#if defined(ALPHA)                     // Z-Lib debug version requires this
  extern "C" { int z_verbose = 0, z_error = 0; }
#endif                                 // Compiling DEBUG/ALPHA version
/* == Omission macros ====================================================== */
/* ######################################################################### */
/* ## These allow us to delete the assignment operator and assignment     ## */
/* ## copy constructor in a class so we don't accidently perform copies   ## */
/* ## instead of std::move()'s.                                           ## */
/* ######################################################################### */
/* ------------------------------------------------------------------------- */
#define DELETECOPYCTORS(x) \
  x(const x&) = delete; const x &operator=(const x&) = delete;
/* == Static class try/catch helpers ======================================= */
/* ######################################################################### */
/* ## Remember that if you put try/catch block on the function level, the ## */
/* ## memory for the members is not cleaned up and we get memory leaks.   ## */
/* ## C++ (ISO/IEC JTC 1/SC 22 N 4411). The process of calling            ## */
/* ## destructors for automatic objects constructed on the path from a    ## */
/* ## try block to a throw- expression is called 'stack unwinding.'. If a ## */
/* ## destructor called during stack unwinding exits with an exception,   ## */
/* ## std::terminate is called (15.5.1). So destructors should generally  ## */
/* ## catch exceptions and not let them propagate out of the destructor.  ## */
/* ######################################################################### */
/* ------------------------------------------------------------------------- */
#define DTORHELPERBEGIN(c) c(void) noexcept(false) try {
#define DTORHELPEREND(c) } catch(const exception &E) \
  { SysMessage(STR(c) " Shutdown Exception", E.what(), MB_ICONSTOP); }
#define DTORHELPER(c,...) DTORHELPERBEGIN(c) __VA_ARGS__; DTORHELPEREND(c)
/* == Init helper ========================================================== */
/* ######################################################################### */
/* ## Very useful little helper to create a class in-scope to init and    ## */
/* ## a de-init function when leaving the scope.                          ## */
/* ######################################################################### */
/* ## n ## Name of the class. The variable is called this too prefix 'c'. ## */
/* ## i ## The function to execute straight away.                         ## */
/* ## d ## The function to execute when leaving the scope.                ## */
/* ######################################################################### */
#define INITHELPER(n,i,d) class n{public:n(void){i;}\
                               ~n(void) noexcept(false){d;}} c ## n
#define DEINITHELPER(n,d) INITHELPER(n,,d)
/* == EoF =========================================================== EoF == */

/* == SETUP.HPP ============================================================ */
/* ######################################################################### */
/* ## MS-ENGINE              Copyright (c) MS-Design, All Rights Reserved ## */
/* ######################################################################### */
/* ## This module sets up the environment for the game engine. For        ## */
/* ## example. Making the build process compatible with different         ## */
/* ## compilers.                                                          ## */
/* ######################################################################### */
/* ========================================================================= */
#pragma once                           // Only one incursion allowed
/* == Build type setup ===================================================== */
#if defined(RELEASE)                   // Final public release version?
# define BUILD_TYPE_LABEL              "Release"
#elif defined(BETA)                    // Beta private release version?
# define BUILD_TYPE_LABEL              "Beta"
#elif defined(ALPHA)                   // Alpha/Debug private release version?
# define BUILD_TYPE_LABEL              "Alpha"
#else                                  // Not specified? (error!)
# error Release type of either RELEASE, BETA or ALPHA was not defined.
#endif                                 // End of release type check
/* == Compiled type setup ================================================== */
#if !defined(__cplusplus)              // Must be compiling in C++ mode
# error Please use a C++ compiler such as GCC, CLANG or MSVC!
#elif defined(_WIN32)                  // Only applies to MS compiler
# if defined(ALPHA)                    // Debugging?
#  if !defined(_DEBUG)                 // Might not be defined on LLVM
#   define _DEBUG                      // Not defined on CLang
#  endif                               // _DEBUG define check
#  define _CRTDBG_MAP_ALLOC            // >> Required by MSDN
# elif defined(BETA)||defined(RELEASE) // Beta mode defined?
#  if defined(_DEBUG)                  // Debug compiler options not specified?
#   error BETA or RELEASE specified but DEBUG was also defined!
#  endif                               // Debug compiler options check
#  define NDEBUG                       // No assertions
#  define _NDEBUG                      // No assertions
# endif                                // Debugging check
# ifdef __clang__                      // Actually using CLang?
#  if __cplusplus < 202002L            // Must be using a C++20 compiler
#   error Must use a C++20 standards clang compiler!
#  endif                               // C++ standards check
#  define COMPILER_NAME                "CLang"
#  define COMPILER_VERSION             STR(__clang_major__) "." \
                                       STR(__clang_minor__) "." \
                                       STR(__clang_patchlevel__)
# elif defined(_MSC_VER)               // Actually using Microsoft compiler?
#  if _MSC_VER < 1911                  // Is correct version?
#   error Must use Microsoft C++ Compiler v19.11 or better!
#  endif                               // Microsoft version check
#  define COMPILER_NAME                "MSVisualC"
#  define COMPILER_VERSION             STR(_MSC_VER)
# else                                 // Unsupported compiler?
#  error This compiler is not supported. Please use clang or msvc!
# endif                                // Check actual compiler
# if defined(_M_AMD64)                 // Target is AMD architecture?
#  define X64                          // Using X64 architechture
#  if defined(__AVX2__)                // Target has AVX2 instructions?
#   define BUILD_TARGET                "Win-X64-AVX2"
#  elif defined(__AVX__)               // Target has AVX instructions?
#   define BUILD_TARGET                "Win-X64-AVX"
#  else                                // Generic AMD64?
#   define BUILD_TARGET                "Win-X64-SSE2"
#  endif                               // Floating point checks
# elif defined(_M_ARM64)               // Target is ARM architecture?
#  define X64                          // Using X64 architechture
#  define BUILD_TARGET                 "Win-ARM64"
# elif defined(_M_ARM)                 // Target is ARM architechture?
#  define X86                          // Using 32-bit architechture
#  define BUILD_TARGET                 "Win-ARM"
# elif defined(_M_IX86)                // Target is X86 architecture?
#  define X86                          // Using X86 architechture
#  if _M_IX86_FP == 1                  // Target FP is SSE?
#   define BUILD_TARGET                "Win-X86-SSE"
#  elif _M_IX86_FP == 2                // Target FP is SSE2?
#   if defined(__AVX2__)               // Target has AVX2 instructions?
#    define BUILD_TARGET               "Win-X86-AVX2"
#   elif defined(__AVX__)              // Target has AVX instructions?
#    define BUILD_TARGET               "Win-X86-AVX"
#   else                               // Target has SSE2 instructions?
#    define BUILD_TARGET               "Win-X86-SSE2"
#   endif                              // SSE2 type checks
#  else                                // Target FP is generic IA32?
#   define BUILD_TARGET                "Win-X86"
#  endif                               // Floating point checks
# else                                 // Target is unknown?
#  error "This Windows target being compiled with is not supported!"
# endif                                // Target architechture detection
# define WINVER                 0x0501 // Target Windows XP or higher
# define _WIN32_WINNT           WINVER // Target Windows XP or higher
# define WIN32_LEAN_AND_MEAN           // Faster compilation of headers
# define VC_EXTRALEAN                  // Extra lean Visual C headers
# define UNICODE                       // Using native Windows functions
# define _UNICODE                      //  prevents conversions & allocations
# define NOMINMAX                      // Do not define min/max please
# define DLLEXT                 ".dll" // Extension to use for DLL's
#elif defined(__APPLE__)               // Apple target?
# include <TargetConditionals.h>       // Include target conditionals header
# if defined(TARGET_OS_MAC)            // Targeting MacOS?
#  if defined(__x86_64__)              // 64-bit x86 architecture?
#   define X64                         // Using 64-bit architechture
#   define BUILD_TARGET                "MacOS-X64"
#  elif defined(__arm64__)             // 64-bit ARM architecture?
#   define X64                         // Using 64-bit architechture
#   define BUILD_TARGET                "MacOS-ARM64"
#  else                                // Unknown target?
#   error "This MacOS architechture being compiled with is not supported!"
#  endif                               // Done checking architechture
# else                                 // Not targeting MacOS?
#  error "This MacOS target being compiled with is not supported!"
# endif                                // Target check
# define COMPILER_NAME                 "LLVM"
# define COMPILER_VERSION              STR(__GNUC__) "." \
                                       STR(__GNUC_MINOR__) "." \
                                       STR(__GNUC_PATCHLEVEL__)
# define _DARWIN_USE_64_BIT_INODE      // For 64-bit size values
# define _LIBCPP_ENABLE_CXX17_REMOVED_UNEXPECTED_FUNCTIONS // On in MSVC & GCC
# define DLLEXT               ".dylib" // Extension to use for DLL's
#elif __cplusplus < 202002L            // Must be using a C++20 compiler
# error Must use a C++ 20 or better compiler!
#elif defined(__linux__)               // Linux detected?
# define COMPILER_NAME                 "GCC"
# define COMPILER_VERSION              STR(__GNUC__) "." \
                                       STR(__GNUC_MINOR__) "." \
                                       STR(__GNUC_PATCHLEVEL__)
# if defined(__x86_64__)               // 64-bit compilation?
#  define X64                          // Using 64-bit architechture
#  if defined(__AVX2__)                // Using AVX2 instructions
#   define BUILD_TARGET                "Linux-X64-AVX2"
#  elif defined(__AVX__)               // Using AVX instructions
#   define BUILD_TARGET                "Linux-X64-AVX"
#  elif defined(__SSE3__)              // Using SSE3 instructions
#   define BUILD_TARGET                "Linux-X64-SSE3"
#  elif defined(__SSE2__)              // Using SSE2 instructions
#   define BUILD_TARGET                "Linux-X64-SSE2"
#  elif defined(__SSE__)               // Using SSE instructions
#   define BUILD_TARGET                "Linux-X64-SSE"
#  else                                // Using generic instructions
#   define BUILD_TARGET                "Linux-X64"
#  endif                               // FP extensions check
# elif defined(__i386__)               // 32-bit compilation?
#  define X86                          // Using 32-bit architechture
#  if defined(__AVX2__)                // Using AVX2 instructions
#   define BUILD_TARGET                "Linux-X86-AVX2"
#  elif defined(__AVX__)               // Using AVX instructions
#   define BUILD_TARGET                "Linux-X86-AVX"
#  elif defined(__SSE3__)              // Using SSE3 instructions
#   define BUILD_TARGET                "Linux-X86-SSE3"
#  elif defined(__SSE2__)              // Using SSE2 instructions
#   define BUILD_TARGET                "Linux-X86-SSE2"
#  elif defined(__SSE__)               // Using SSE instructions
#   define BUILD_TARGET                "Linux-X86-SSE"
#  else                                // Using generic instructions
#   define BUILD_TARGET                "Linux-X86"
#  endif                               // FP extensions check
# else                                 // Invalid architecture?
#  error "This Linux target being compiled with is not supported!"
# endif                                // Linux arch check
# define DLLEXT                  ".so" // Extension to use for DLL's
#else                                  // Unsupported target environment
# error "This platform being compiled with is not supported!"
#endif                                 // Using Microsoft compiler
/* -- Base defines --------------------------------------------------------- */
#define DEFAULT_CONFIGURATION    "app" // Default app configuration filename
#define ARCHIVE_EXTENSION        "adb" // Default archive extension
#define LOG_EXTENSION            "log" // Default log file extension
#define UDB_EXTENSION            "udb" // Default database file extension
#define CFG_EXTENSION            "cfg" // Default config file extension
#define CER_EXTENSION            "cer" // Default certificate file extension
/* == Base STL includes ==================================================== */
#include <algorithm>                   // Needed for find
#include <array>                       // Memory class
#include <atomic>                      // atomic class
#include <cctype>                      // Character check stuff
#include <clocale>                     // Locale header
#include <cmath>                       // Math header
#include <condition_variable>          // Condition variable class (cpp11)
#include <csetjmp>                     // Set jumper header
#include <csignal>                     // Signal header
#include <cstdarg>                     // Variable argumen
#include <cstdlib>                     // Standard library header
#include <cstring>                     // String header
#include <deque>                       // DeQue class
#include <exception>                   // exception class
#include <functional>                  // Function class
#include <iomanip>                     // Stream manipulation
#include <iterator>                    // Iterator class
#include <limits>                      // Limits
#include <list>                        // ItemMap class
#include <map>                         // Map class
#include <memory>                      // Memory class
#include <mutex>                       // Mutex class
#include <numeric>                     // Accumulate class
#include <queue>                       // Queue class
#include <set>                         // Set class
#include <sstream>                     // Stringstream class
#include <stdexcept>                   // Runtime error class
#include <string>                      // String class
#include <thread>                      // Thread class
#include <vector>                      // Vector class
/* -- Useful macros -------------------------------------------------------- */
#define STR_HELPER(...)  #__VA_ARGS__            // Convert macro integer
#define STR(...)         STR_HELPER(__VA_ARGS__) // to string
/* -- Cross-platform specific declarations --------------------------------- */
#ifdef _WIN32                          // Windows 32-bit or 64-bit
/* -- Entry function and arguments ----------------------------------------- */
#define ENTRYFUNC    WINAPI _tWinMain(HINSTANCE, HINSTANCE, LPTSTR, int)
#define CONENTRYFUNC _tmain            // Entry function for console app
#define ARGTYPE      TCHAR             // Widestring for args
/* -- For access(). MS don't define, but have same meaning as unix --------- */
#define R_OK                         4 // File is readable
#define W_OK                         2 // File is writable
#define X_OK                         1 // File is executable
#define F_OK                         0 // File exists
/* -- Standard library cross-platform compatibility ------------------------ */
#define STDACCESS(f,x)     _waccess(UTFtoS16(f),x)
#define STDEXECVE(v,e)     static_cast<int>(_wexecve(*v,v,e))
#define STDFILENO(h)       _fileno(h)
#define STDFSTAT(f,s)      _wstat64(UTFtoS16(f),s)
#define STDFSTATSTRUCT     struct __stat64
#define STDFSEEK(h,o,m)    _fseeki64(h,o,m)
#define STDFTELL(h)        _ftelli64(h)
#define STDGMTIME(r,t)     _gmtime64_s(r,t)
#define STDHSTAT(h,s)      _fstat64(h,s)
#define STDLOCALTIME(r,t)  _localtime64_s(r,t)
#define STDMKDIR(d)        _wmkdir(UTFtoS16(d))
#define STDMKTIME(s)       _mktime64(s)
#define STDPOPEN(c,e)      _wpopen(UTFtoS16(c),e)
#define STDPOPENFLAGS      L"rt"
#define STDPCLOSE(h)       _pclose(h)
#define STDPUTENV(e,v)     _wputenv(UTFtoS16(Append(e,'=',v)))
#define STDRENAME(s,d)     _wrename(UTFtoS16(s),UTFtoS16(d))
#define STDRMDIR(d)        _wrmdir(UTFtoS16(d))
#define STDSRAND(x)        srand(x)
#define STDTIMET           __time64_t
#define STDUNLINK(d)       _wunlink(UTFtoS16(d))
/* -- UTF8 handling -------------------------------------------------------- */
#define S16toUTF(m)  FromWideStringPtr(m)  // Widestring to utf8 str
#define WS16toUTF(m) S16toUTF((m).c_str()) //    " to utf8 cstr
#define UTFtoWS16(m) Decoder(m).Wide()     // utf8 to widestring str
#define UTFtoS16(m)  UTFtoWS16(m).c_str()  //    " to widestring cstr
/* -- Debug ---------------------------------------------------------------- */
#ifdef _CRTDBG_MAP_ALLOC               // >> Required by MSDN
# include <stdlib.h>                   // >> Required by MSDN
# include <crtdbg.h>                   // >> Basic Leak detection SDK
//# define USEVLD                      // Set to use VLD (copy *.dll/manifest)
# ifdef USEVLD                         // Use VLD debugging library?
#  include <vld/vld.h>                 // >> If basic detection not working
#  if defined(_WIN64)                  // 64-bit windows?
#   pragma comment(lib, "res/lib/vld64d") // >> Use 64-bit VLD lib
#  else                                // 32-bit windows?
#   pragma comment(lib, "res/lib/vld32d") // >> Use 32-bit VLD lib
#  endif                               // Bits check
# endif                                // VLD inclusion requested
#endif                                 // Not using debug mode
/* -- Endianness ----------------------------------------------------------- */
#define SWAP_U16(v) _byteswap_ushort(static_cast<unsigned short>(v))
#define SWAP_U32(v) _byteswap_ulong(static_cast<unsigned long>(v))
#define SWAP_U64(v) _byteswap_uint64(static_cast<unsigned __int64>(v))
#if REG_DWORD == REG_DWORD_LITTLE_ENDIAN  // Using Intel/AMD? (LE)
# define STRICT_U16LE(v) (v)              // Use 16-bit integers as-is
# define STRICT_U16BE(v) SWAP_U16(v)      // Swap 16-bits
# define STRICT_U32LE(v) (v)              // Use 32-bit integers as-is
# define STRICT_U32BE(v) SWAP_U32(v)      // Swap 32-bits
# define STRICT_U64LE(v) (v)              // Use 64-bit integers as-is
# define STRICT_U64BE(v) SWAP_U64(v)      // Swap 64-bits
# define LITTLE_ENDIAN                    // Using little endian byte order
#elif REG_DWORD == REG_DWORD_BIG_ENDIAN   // Using ARM? (BE)
# define STRICT_U16LE(v) SWAP_U16(v)      // Swap 16-bits
# define STRICT_U16BE(v) (v)              // Use 16-bit integers as-is
# define STRICT_U32LE(v) SWAP_U32(v)      // Swap 32-bits
# define STRICT_U32BE(v) (v)              // Use 32-bit integers as-is
# define STRICT_U64LE(v) SWAP_U64(v)      // Swap 64-bits
# define STRICT_U64BE(v) (v)              // Use 64-bit integers as-is
# define BIG_ENDIAN                       // Using big endian byte order
#else                                     // Unknown endianness?
# error Unknown endianness!
#endif                                    // Endianess setup
/* -- VC Headers ----------------------------------------------------------- */
#include <direct.h>                    // Directory header
#include <intrin.h>                    // Required by jpeg stuff
#include <stdio.h>                     // Standard IO stuff
#include <io.h>                        // Io header
#include <process.h>                   // Process header
#include <sys/timeb.h>                 // Time block functions
/* -- Windows base includes ------------------------------------------------ */
#include <Windows.H>                   // Need windows headers file
#include <TChar.H>                     // Unicode support
#include <ImageHlp.H>                  // Executable image headers file
#include <TlHelp32.H>                  // Toolhelp header file
#include <ShlObj.H>                    // Shell header file
#include <RTCapi.H>                    // RTC headers file
#include <WinSock.H>                   // Windows sockets headers file
#include <AccCtrl.H>                   // Access control headers file
#include <AclApi.H>                    // Access control list headers file
/* ------------------------------------------------------------------------- */
#define PSAPI_VERSION                1 // Want this to work on XP so use V1
#include <PsApi.H>                     // Process API header file
#undef PSAPI_VERSION                   // Don't need this anymore
#undef GetObject                       // RapidJSon compatibility
/* -- Using anything but Windows? ------------------------------------------ */
#else                                  // Could be OSX or Linux
/* -- Entry function and arguments for anything but Windows ---------------- */
#define ARGTYPE            char        // Main() arguments type
#define ENTRYFUNC          main(int __argc,ARGTYPE**__wargv,ARGTYPE**_wenviron)
#define CONENTRYFUNC       main
/* -- Standard library cross-platform compatibility ------------------------ */
#define STDACCESS(f,x)     access(f.c_str(),x)
#define STDEXECVE(v,e)     execve(*v,v,e)
#define STDFILENO(h)       fileno(h)
#define STDFSTAT(f,s)      stat(f.c_str(),s)
#define STDFSTATSTRUCT     struct stat
#define STDFSEEK(h,o,m)    fseeko(h,o,m)
#define STDFTELL(h)        ftello(h)
#define STDGMTIME(r,t)     gmtime_r(t,r)
#define STDHSTAT(h,s)      fstat(h,s)
#define STDLOCALTIME(r,t)  localtime_r(t,r)
#define STDMKDIR(d)        mkdir(d.c_str(),S_IRUSR|S_IWUSR|S_IXUSR|S_IRGRP|\
                                           S_IXGRP|S_IROTH|S_IXOTH)
#define STDMKTIME(s)       mktime(s)
#define STDPOPEN(c,e)      popen(c.c_str(),e)
#define STDPOPENFLAGS      "r"
#define STDPCLOSE(h)       pclose(h)
#define STDPUTENV(e,v)     setenv(e.c_str(),v.c_str())
#define STDRENAME(s,d)     rename(s.c_str(),d.c_str())
#define STDRMDIR(d)        rmdir(d.c_str())
#define STDSRAND(x)        srandom(x)
#define STDTIMET           time_t
#define STDUNLINK(d)       unlink(d.c_str())
/* -- UTF8 handling (not applicable) --------------------------------------- */
#define UTFtoWS16(m)       m           // Filenames already treated as UTF-8
#define UTFtoS16(m)        m           // Filenames already treated as UTF-8
#define WS16toUTF(m)       m           // Filenames already treated as UTF-8
#define S16toUTF(m)        m           // Filenames already treated as UTF-8
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
/* -- Endianness ----------------------------------------------------------- */
#define SWAP_U16(v) __builtin_bswap16(static_cast<unsigned short>(v))
#define SWAP_U32(v) __builtin_bswap32(static_cast<unsigned int>(v))
#define SWAP_U64(v) __builtin_bswap64(static_cast<unsigned long long>(v))
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__ // Using Intel/AMD? (LE)
# define STRICT_U16LE(v) (v)           // Use 16-bit integers as-is
# define STRICT_U16BE(v) SWAP_U16(v)   // Swap 16-bits
# define STRICT_U32LE(v) (v)           // Use 32-bit integers as-is
# define STRICT_U32BE(v) SWAP_U32(v)   // Swap 32-bits
# define STRICT_U64LE(v) (v)           // Use 64-bit integers as-is
# define STRICT_U64BE(v) SWAP_U64(v)   // Swap 64-bits
# ifndef LITTLE_ENDIAN                 // This could already be defined
#  define LITTLE_ENDIAN                // Define it if not
# endif                                // Little endian check
#elif __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__ // Using ARM? (BE)
# define STRICT_U16LE(v) SWAP_U16(v)   // Swap 16-bits
# define STRICT_U16BE(v) (v)           // Use 16-bit integers as-is
# define STRICT_U32LE(v) SWAP_U32(v)   // Swap 32-bits
# define STRICT_U32BE(v) (v)           // Use 32-bit integers as-is
# define STRICT_U64LE(v) SWAP_U64(v)   // Swap 64-bits
# define STRICT_U64BE(v) (v)           // Use 64-bit integers as-is
# ifndef BIG_ENDIAN                    // This could already be defined
#  define BIG_ENDIAN                   // Define it if not
# endif                                // Big endian check
#else                                  // Unknown endianness?
# error Unknown endianness!
#endif                                 // Endianness setup
/* ------------------------------------------------------------------------- */
#include <cxxabi.h>                    // Debug name demangling stuff
#include <dirent.h>                    // Directory functions (*dir)
#include <dlfcn.h>                     // Dylib functions
#include <execinfo.h>                  // Stack trace functions
#include <libgen.h>                    // Unix core (*name)
#include <sys/stat.h>                  // Stat struct
#include <sys/utsname.h>               // OS information
#include <unistd.h>                    // Unix standard stuff
/* ------------------------------------------------------------------------- */
#endif                                 // Operating system
/* ------------------------------------------------------------------------- */
#ifdef __APPLE__                       // Parallel execution not on Apple yet!
# define MYFILL(p,b,e,...)         fill(b, e, __VA_ARGS__)
# define MYTRANSFORM(p,b,e,o,...)  transform(b, e, o, __VA_ARGS__)
# define MYFOREACH(p,b,e,...)      for_each(b, e, __VA_ARGS__)
# define MYCOPY(p,b,e,...)         copy(b, e, __VA_ARGS__)
# define GLFW_EXPOSE_NATIVE_COCOA      // Expose Cocoa specific funcs in Glfw
/* ------------------------------------------------------------------------- */
#else                                  // Linux and windows compilers support
# include <execution>                  // Parallel execution support
# define MYFILL(p,b,e,...)         fill(p, b, e, __VA_ARGS__)
# define MYTRANSFORM(p,b,e,o,...)  transform(p, b, e, o, __VA_ARGS__)
# define MYFOREACH(p,b,e,...)      for_each(p, b, e, __VA_ARGS__)
# define MYCOPY(p,b,e,...)         copy(p, b, e, __VA_ARGS__)
# if defined(_WIN32)                   // Using windows?
typedef std::make_signed<size_t>::type ssize_t; // Not in MSVC
#  define GLFW_EXPOSE_NATIVE_WIN32     // Expose Cocoa specific funcs in Glfw
# elif defined(__linux__)              // Using linux?
#  define GLFW_EXPOSE_NATIVE_X11       // Expose X11 specific funcs in Glfw
#  define GLFW_EXPOSE_NATIVE_WAYLAND   // Expose Wayland specific funcs in Glfw
# endif                                // Windows check
#endif                                 // Apple check
/* ------------------------------------------------------------------------- */
#define UNUSED_VARIABLE(x)             (void)(x)
/* == Typedefs ============================================================= */
#ifdef ALPHA                           // Z-Lib debug version requires this
  extern "C" { int z_verbose = 0, z_error = 0; }
#endif                                 // Compiling DEBUG/ALPHA version
/* == API includes ========================================================= */
/* ######################################################################### */
/* ## Include API functions                                               ## */
/* ######################################################################### */
/* ------------------------------------------------------------------------- */
#ifndef _WIN32                         // Not using windows?
# pragma GCC diagnostic push           // Save warnings
# ifdef __linux__                      // Using linux?
#  pragma GCC diagnostic ignored "-Wall"        // Disable ALL warnings
# else                                 // Using macos?
#  pragma GCC diagnostic ignored "-Weverything" // Disable ALL warnings
# endif                                // Linux or MacOS check
#endif                                 // Not using windows
/* -- Lua includes --------------------------------------------------------- */
/* Because Lua was compiled as C++ in the root namespace, Lua's includes     */
/* also need to be defined in the root namespace.                            */
/* ------------------------------------------------------------------------- */
#include <lua/lstate.h>                // Definition of Lua_State
#include <lua/lauxlib.h>               // Lua auxiliary library
#include <lua/lualib.h>                // Lua library
#undef getstr                          // Ambiguity with ncurses
/* ------------------------------------------------------------------------- */
namespace Library                      // LIBRARY OF EXTERNAL API FUNCTIONS
{ /* ----------------------------------------------------------------------- */
namespace ZLib                         // ZLIB API FUNCTIONS
{ /* ----------------------------------------------------------------------- */
#include <zlib/zlib.h>                 // Main header
};/* ----------------------------------------------------------------------- */
namespace GlFW                         // GLFW API FUNCTIONS
{ /* ----------------------------------------------------------------------- */
#define GLFW_INCLUDE_GLCOREARB         // Need proper opengl headers
#include <glfw/glfw3.h>                // Main header
#include <glfw/glfw3native.h>          // Operating system includes
#undef GLFW_INCLUDE_GLCOREARB          // Done with this define
#ifdef __linux__                       // Using linux?
# undef Bool                           // X11 defines this, we don't need it
#endif                                 // Linux check
};/* ----------------------------------------------------------------------- */
namespace NSGif                        // LIBNSGIF API FUNCTIONS
{ /* ----------------------------------------------------------------------- */
#include <gif/gif.h>                   // Main header
};/* ----------------------------------------------------------------------- */
namespace Png                          // LIBPNG API FUNCTIONS
{ /* ----------------------------------------------------------------------- */
#include <png/png.h>                   // Main header
};/* ----------------------------------------------------------------------- */
namespace JpegTurbo                    // LIBJPEGTURBO API FUNCTIONS
{ /* ----------------------------------------------------------------------- */
#ifdef __linux__                       // Using Linux?
# include <jpeglib.h>                  // Repository provided main header
# include <jerror.h>                   // Repository provided error handling
#else                                  // MacOS or Windows?
# include <jpeg/jpeglib.h>             // Our main header
# include <jpeg/jerror.h>              // Our error handling
# include <jpeg/jversion.h>            // Our version information
#endif                                 // Linux check
};/* ----------------------------------------------------------------------- */
namespace RapidJson                    // RAPIDJSON API FUNCTIONS
{ /* ----------------------------------------------------------------------- */
#define RAPIDJSON_NAMESPACE            Library::RapidJson
#define RAPIDJSON_HAS_STDSTRING      1 // Have Std::String
#define RAPIDJSON_NOEXCEPT             // We have noexcept
#define RAPIDJSON_NAMESPACE_BEGIN      // Keep blank or errors
#define RAPIDJSON_NAMESPACE_END        // Keep blank or errors
#define RAPIDJSON_ASSERT(x)            if(!(x)) throw std::runtime_error{(#x)};
#define RAPIDJSON_HAS_CXX11_NOEXCEPT 1 // Force to use noexcept
#include <rapidjson/document.h>        // Main header
#include <rapidjson/prettywriter.h>    // Pretty formatting
#include <rapidjson/error/en.h>        // Error handling
#undef RAPIDJSON_HAS_CXX11_NOEXCEPT    // Done with this define
#undef RAPIDJSON_ASSERT                // Done with this define
#undef RAPIDJSON_NAMESPACE_END         // Done with this define
#undef RAPIDJSON_NAMESPACE_BEGIN       // Done with this define
#undef RAPIDJSON_HAS_STDSTRING         // Done with this define
#undef RAPIDJSON_NAMESPACE             // Done with this define
};/* ----------------------------------------------------------------------- */
namespace OpenAL                       // OPENAL API FUNCTIONS
{ /* ----------------------------------------------------------------------- */
#define AL_ALEXT_PROTOTYPES            // So can get alcResetDevice
#include <al/alext.h>                  // Main header
#undef AL_ALEXT_PROTOTYPES             // Extra functions check
};/* ----------------------------------------------------------------------- */
namespace MiniMP3                      // MINIMP3 API FUNCTIONS
{ /* ----------------------------------------------------------------------- */
#include <mp3/minimp3.h>               // Main header
} /* -- Ogg Includes ------------------------------------------------------- */
namespace Ogg                          // OGGVORBIS API FUNCTIONS
{ /* ----------------------------------------------------------------------- */
#define OV_EXCLUDE_STATIC_CALLBACKS    // We don't need these callbacks
#include <ogg/vorbisfile.h>            // Vorbis/ogg API headers
#undef OV_EXCLUDE_STATIC_CALLBACKS     // The define is no longer needed
};/* ----------------------------------------------------------------------- */
namespace Sqlite                       // SQLITE API FUNCTIONS
{/* ------------------------------------------------------------------------ */
#include <sql/sqlite3.h>               // Main header
};/* ----------------------------------------------------------------------- */
namespace OpenSSL                      // OPENSSL API FUNCTIONS
{ /* ----------------------------------------------------------------------- */
#define OPENSSL_SUPPRESS_DEPRECATED    // Want deprecated functions
#include <openssl/conf.h>              // Conf header
#include <openssl/evp.h>               // EVP header
#include <openssl/engine.h>            // Engine header
#include <openssl/comp.h>              // Comp header
#include <openssl/rand.h>              // RNG header
#include <openssl/ssl.h>               // main header
#include <openssl/md5.h>               // MD5 hash (compatibility only)
#include <openssl/sha.h>               // SHA-1 hash (compatibility only)
#include <openssl/bio.h>               // Bio header
#include <openssl/err.h>               // Errors header
#include <openssl/ocsp.h>              // OCSP header
#include <openssl/x509v3.h>            // Certs header
#include <openssl/httperr.h>           // HTTP client errors (ToDo)
#include <openssl/http.h>              // HTTP client (ToDo)
};/* ----------------------------------------------------------------------- */
namespace SevenZip                     // 7-ZIP API FUNCTIONS
{ /* ----------------------------------------------------------------------- */
#include <7z/CpuArch.h>                // CPU configuration
#include <7z/7z.h>                     // Library core (old p7zip)
#include <7z/7zCrc.h>                  // CRC header
#include <7z/7zFile.h>                 // 7-zip file format
#include <7z/7zVersion.h>              // 7-zip version information
#include <7z/LzmaLib.h>                // For compression stuff
};/* ----------------------------------------------------------------------- */
namespace FreeType                     // FREETYPE API FUNCTIONS
{ /* ----------------------------------------------------------------------- */
#include <ft/ft2build.h>               // Build parameters header
#include <ft/freetype.h>               // Base header
#include <ft/ftstroke.h>               // Stroker header
#include <ft/ftglyph.h>                // Glyph header
#include <ft/ftmodapi.h>               // Modification header
};/* ----------------------------------------------------------------------- */
namespace Theora                       // THEORA API FUNCTIONS
{ /* ----------------------------------------------------------------------- */
  using namespace Ogg;                 // Require types from Ogg API
#include <theora/theora.h>             // Format codec (+ ogg codec)
#include <theora/theoradec.h>          // Decoder
};/* ----------------------------------------------------------------------- */
#ifndef _WIN32                         // Not using windows?
# pragma GCC diagnostic pop            // - Restore compiler warnings
#endif                                 // Not using windows
};/* ----------------------------------------------------------------------- */
/* == Omission macros ====================================================== */
/* ######################################################################### */
/* ## These allow us to delete the assignment operator and assignment     ## */
/* ## copy constructor in a class so we don't accidently perform copies   ## */
/* ## instead of move()'s.                                                ## */
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
                               ~n(void) noexcept(false){d;}} c ## n;
#define DEINITHELPER(n,d) INITHELPER(n,,d);
/* == EoF =========================================================== EoF == */

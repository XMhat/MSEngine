/* == CREDITS.HPP ========================================================== */
/* ######################################################################### */
/* ## MS-ENGINE              Copyright (c) MS-Design, All Rights Reserved ## */
/* ######################################################################### */
/* ## This file is a consolidation of libraries information used in the   ## */
/* ## engine which is logged at startup and can be read by the guest and  ## */
/* ## the end-user.                                                       ## */
/* ######################################################################### */
/* ========================================================================= */
#pragma once                           // Only one incursion allowed
/* ------------------------------------------------------------------------- */
namespace IfCredit {                   // Start of module namespace
/* -- Includes ------------------------------------------------------------- */
using namespace IfCVarDef;             // Using cvardef namespace
/* -- Credit library class ------------------------------------------------- */
class CreditLib :                      // Members initially private
  /* -- Base classes ------------------------------------------------------- */
  public DataConst                     // Licence data memory
{ /* ----------------------------------------------------------------------- */
  const string     strName;            // Name of library
  const string     strVersion;         // String version
  const string     strAuthor;          // Author of library
  const bool       bCopyright;         // Is copyrighted library
  /* --------------------------------------------------------------- */ public:
  const string &GetName(void) const { return strName; }
  const string &GetVersion(void) const { return strVersion; }
  const string &GetAuthor(void) const { return strAuthor; }
  bool IsCopyright(void) const { return bCopyright; }
  /* ----------------------------------------------------------------------- */
  CreditLib(const string &strN, const string &strV, const string &strA,
    const bool bC, const void*const vpData, const size_t stSize) :
    /* -- Initialisers ----------------------------------------------------- */
    DataConst{ stSize, vpData },       // Init credit licence data
    strName{ strN },                   // Init credit name
    strVersion{ strV },                // Init credit version
    strAuthor{ strA },                 // Init credit author
    bCopyright{ bC }                   // Init credit copyright status
    /* -- No code ---------------------------------------------------------- */
    { }
};/* ----------------------------------------------------------------------- */
/* -- Decompress a credit item --------------------------------------------- */
static const string CreditGetItemText(const CreditLib &libItem) try
{ // Decode the file and show the result
  return Block<CoDecoder>{ libItem }.ToString();
} // exception occured?
catch(const exception &e)
{ // Log failure and try to reset the initial var so this does not
  XC("Failed to decode licence text!",
     "Name", libItem.GetName(), "Reason", e, "Length", libItem.Size());
}
/* -- Credits list lookup table -------------------------------------------- */
enum CreditEnums                       // Credit ids
{ /* ----------------------------------------------------------------------- */
  CL_FIRST,                            // The first item (see llcredit.hpp)
  /* ----------------------------------------------------------------------- */
  CL_MSE=CL_FIRST,                     // Id for MS-Engine data (always first)
  CL_FT,                               // Id for FreeType credit data
  CL_GLFW,                             // Id for GLFW credit data
  CL_JPEG,                             // Id for LibJPEGTurbo credit data
  CL_GIF,                              // Id for LibNSGif credit data
  CL_PNG,                              // Id for LibPNG credit data
  CL_LUA,                              // Id for Lua credit data
  CL_LZMA,                             // Id for 7-Zip/LZMA credit data
  CL_MP3,                              // Id for MiniMP3 credit data
#if !defined(WINDOWS)                  // Not using Windows?
  CL_NCURSES,                          // Id for NCurses credit data
#endif                                 // Not using windows
  CL_OGG,                              // Id for LibOgg+LibVorbis credit data
  CL_AL,                               // Id for OpenALSoft credit data
  CL_SSL,                              // Id for OpenSSL credit data
  CL_JSON,                             // Id for RapidJson credit data
  CL_SQL,                              // Id for SqLite credit data
  CL_THEO,                             // Id for Theora credit data
  CL_ZLIB,                             // Id for ZLib credit data
  /* ----------------------------------------------------------------------- */
  CL_MAX                               // Item count. Don't remove
};/* ----------------------------------------------------------------------- */
typedef array<const CreditLib,CL_MAX> CreditLibList; // Library list typedef
static const CreditLibList &CreditGetList(void)
{ // Setup compressed licence data
#define BEGINLICENCE(n, s)             static array<const uint8_t, s> l ## n{
#define ENDLICENCE };                  // Helper functions for licences header
#include "licence.hpp"                 // Load up compressed licences
#undef ENDLICENCE                      // Done with this macro
#undef BEGINLICENCE                    // Done with this macro
  // Setup compressed licence data list structure
  static const CreditLibList libList{  // The library list
  { // t = Title of dependency         v = Version of dependency
    // n = licence variable name       c = is dependency copyrighted?
    // a = Author of dependency
#define LD(t,v,c,a,n) { t, v, c, a, l ## n.data(), l ## n.size() }
    // The credits data structure (Keep MS-Engine as the first)
    LD(cSystem->ENGName(), cSystem->ENGVersion(), cSystem->ENGAuthor(), true,
      MSENGINE),
    LD("FreeType", STR(FREETYPE_MAJOR) "." STR(FREETYPE_MINOR) "."
      STR(FREETYPE_PATCH), "The FreeType Project", true, FREETYPE),
    LD("GLFW", STR(GLFW_VERSION_MAJOR) "." STR(GLFW_VERSION_MINOR) "."
      STR(GLFW_VERSION_REVISION), "Marcus Geelnard & Camilla Löwy", true,
      GLFW),
    LD("JPEGTurbo", STR(LIBJPEG_TURBO_VERSION), "IJG/Contributing authors",
      true, LIBJPEGTURBO),
    LD("LibNSGif", "0.2.1", "Richard Wilson & Sean Fox", true, LIBNSGIF),
    LD("LibPNG", PNG_LIBPNG_VER_STRING, "Contributing authors", true, LIBPNG),
    LD("LUA", LUA_VERSION_MAJOR "." LUA_VERSION_MINOR "." LUA_VERSION_RELEASE,
      "Lua.org, PUC-Rio", true, LUA),
    LD("LZMA", MY_VERSION, "Igor Pavlov", false, 7ZIP),
    LD("MiniMP3", "1.0", "Martin Fiedler", false, MINIMP3),
#if !defined(WINDOWS)
    LD("NCurses", NCURSES_VERSION, "Free Software Foundation", true, NCURSES),
#endif
    LD("OggVorbis", IfAudio::vorbis_version_string()+19, "Xiph.Org", false,
      OGGVORBISTHEORA),
    LD("OpenALSoft", "1.23.0", "Chris Robinson", false, OPENALSOFT),
    LD("OpenSSL", STR(OPENSSL_VERSION_MAJOR) "." STR(OPENSSL_VERSION_MINOR) "."
      STR(OPENSSL_VERSION_PATCH), "OpenSSL Software Foundation", true,
      OPENSSL),
    LD("RapidJson", RAPIDJSON_VERSION_STRING,
      "THL A29 Ltd., Tencent co. & Milo Yip", true, RAPIDJSON),
    LD("SQLite", SQLITE_VERSION, "Contributing authors", false, SQLITE),
    LD("Theora",
      Format("$.$.$", IfVideo::theora_version_number() >> 16 & 0xFFFF,
                      IfVideo::theora_version_number() >>  8 & 0x00FF,
                      IfVideo::theora_version_number()       & 0x00FF),
      "Xiph.Org", false, OGGVORBISTHEORA),
    LD("Z-Lib", STR(ZLIB_VER_MAJOR) "." STR(ZLIB_VER_MINOR) "."
      STR(ZLIB_VER_REVISION) "." STR(ZLIB_VER_SUBREVISION),
      "Jean-loup Gailly & Mark Adler", true, ZLIB),
    // End of credits data structure
#undef LD                              // Done with this macro
  } };
  // Return list
  return libList;
}
/* -- Get credit information ----------------------------------------------- */
static size_t CreditGetItemCount(void) { return CreditGetList().size(); }
static const CreditLib &CreditGetItem(const CreditEnums ceIndex)
  { return CreditGetList()[ceIndex]; }
/* -- Decompress a credit -------------------------------------------------- */
static const string CreditGetItemText(const CreditEnums ceIndex)
  { return CreditGetItemText(CreditGetItem(ceIndex)); }
/* -- Dump credits to log -------------------------------------------------- */
static void CreditDumpList(void)
{ // Iterate through each entry and send library information to log
  cLog->LogNLCInfoExSafe("Credits enumerating $ external libraries...",
      CreditGetItemCount());
  for(const CreditLib &lD : CreditGetList())
    cLog->LogNLCInfoExSafe("- Using $ (v$) $$", lD.GetName(),
      lD.GetVersion(), lD.IsCopyright() ? "\xC2\xA9 " : strBlank,
      lD.GetAuthor());
}
static CVarReturn CreditDumpList(const bool bDoIt)
  { if(bDoIt) CreditDumpList(); return ACCEPT; }
/* ------------------------------------------------------------------------- */
};                                     // End of module namespace
/* == EoF =========================================================== EoF == */

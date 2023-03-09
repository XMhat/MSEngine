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
/* -- Module namespace ----------------------------------------------------- */
namespace IfCredit {                   // Keep declarations neatly categorised
/* -- Includes ------------------------------------------------------------- */
using namespace IfCVarDef;             // Using cvardef interface
/* ------------------------------------------------------------------------- */
struct CreditLib                       // Credit library structure
{ /* ----------------------------------------------------------------------- */
  const string     &strName;           // Name of library
  const string     &strVersion;        // String version
  const DataConst  dcData;             // Licence data
  const bool       bCopyright;         // Is copyrighted library
  const string     &strAuthor;         // Author of library
};/* ----------------------------------------------------------------------- */
/* -- Decompress a credit item --------------------------------------------- */
static const string CreditGetItemText(const CreditLib &libItem) try
{ // Decode the file and show the result
  return Block<CoDecoder>{ libItem.dcData }.ToString();
} // exception occured?
catch(const exception &e)
{ // Log failure and try to reset the initial var so this does not
  XC("Failed to decode licence text!",
     "Name", libItem.strName, "Reason", e, "Length", libItem.dcData.Size());
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
#ifndef _WIN32                         // Not using Windows?
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
  #define ENDLICENCE };                // Helper functions for licences header
  #define BEGINLICENCE(n, s)           static array<const uint8_t, s> l ## n{
  #include "licence.hpp"               // Load up compressed licences
  #undef BEGINLICENCE                  // Done with this macro
  #undef ENDLICENCE                    // Done with this macro
  // Setup compressed licence data list structure
  static const CreditLibList libList{  // The library list
  { // t = Title of dependency         v = Version of dependency
    // n = licence variable name       c = is dependency copyrighted?
    // a = Author of dependency
    #define LD(t,v,n,c,a) { t, v, { l ## n.size(), l ## n.data() }, c, a }
    // The credits data structure (Keep MS-Engine as the first)
    LD(cSystem->ENGName(), cSystem->ENGVersion(), MSENGINE, true,
      cSystem->ENGAuthor()),
    LD("FreeType", STR(FREETYPE_MAJOR) "." STR(FREETYPE_MINOR) "."
      STR(FREETYPE_PATCH), FREETYPE, true, "The FreeType Project"),
    LD("GLFW", STR(GLFW_VERSION_MAJOR) "." STR(GLFW_VERSION_MINOR) "."
      STR(GLFW_VERSION_REVISION), GLFW, true,
      "Marcus Geelnard & Camilla Löwy"),
    LD("JPEGTurbo", STR(LIBJPEG_TURBO_VERSION), LIBJPEGTURBO, true,
      "IJG/Contributing authors"),
    LD("LibNSGif", "0.2.1", LIBNSGIF, true, "Richard Wilson & Sean Fox"),
    LD("LibPNG", PNG_LIBPNG_VER_STRING, LIBPNG, true, "Contributing authors"),
    LD("LUA", LUA_VERSION_MAJOR "." LUA_VERSION_MINOR "." LUA_VERSION_RELEASE,
      LUA, true, "Lua.org, PUC-Rio"),
    LD("LZMA", MY_VERSION, 7ZIP, false, "Igor Pavlov"),
    LD("MiniMP3", "1.0", MINIMP3, false, "Martin Fiedler"),
#ifndef _WIN32
    LD("NCurses", NCURSES_VERSION, NCURSES, true, "Free Software Foundation"),
#endif
    LD("OggVorbis", string{ IfAudio::vorbis_version_string() }.
      substr(19, string::npos), OGGVORBISTHEORA, false, "Xiph.Org"),
    LD("OpenALSoft", "1.23.0", OPENALSOFT, false, "Chris Robinson"),
    LD("OpenSSL", STR(OPENSSL_VERSION_MAJOR) "." STR(OPENSSL_VERSION_MINOR) "."
      STR(OPENSSL_VERSION_PATCH), OPENSSL, true,
      "OpenSSL Software Foundation"),
    LD("RapidJson", RAPIDJSON_VERSION_STRING, RAPIDJSON, true,
      "THL A29 Ltd., Tencent co. & Milo Yip"),
    LD("SQLite", SQLITE_VERSION, SQLITE, false, "Contributing authors"),
    LD("Theora",
      Format("$.$.$", IfVideo::theora_version_number() >> 16 & 0xFFFF,
                      IfVideo::theora_version_number() >>  8 & 0x00FF,
                      IfVideo::theora_version_number()       & 0x00FF),
      OGGVORBISTHEORA, false, "Xiph.Org"),
    LD("Z-Lib", STR(ZLIB_VER_MAJOR) "." STR(ZLIB_VER_MINOR) "."
      STR(ZLIB_VER_REVISION) "." STR(ZLIB_VER_SUBREVISION), ZLIB, true,
      "Jean-loup Gailly & Mark Adler"),
    // End of credits data structure
    #undef LD                          // Done with this macro
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
  cLog->WriteStringSafe(LH_INFO,
    Format("Credits enumerating $ external libraries...",
      CreditGetItemCount()));
  for(const CreditLib &lD : CreditGetList())
    cLog->WriteStringSafe(LH_INFO, Format("- Using $ (v$) $$", lD.strName,
      lD.strVersion, lD.bCopyright ? "\xC2\xA9 " : strBlank, lD.strAuthor));
}
static CVarReturn CreditDumpList(const bool bDoIt)
  { if(bDoIt) CreditDumpList(); return ACCEPT; }
/* -- End of module namespace ---------------------------------------------- */
};                                     // End of interface
/* == EoF =========================================================== EoF == */

/* == CVARDEF.HPP ========================================================== */
/* ######################################################################### */
/* ## MS-ENGINE              Copyright (c) MS-Design, All Rights Reserved ## */
/* ######################################################################### */
/* ## This module defines cvar names, default values and flags. Theres no ## */
/* ## actual code, just definitions!                                      ## */
/* ######################################################################### */
/* ========================================================================= */
#pragma once                           // Only one incursion allowed
/* ------------------------------------------------------------------------- */
namespace IfCVarDef {                  // Start of module namespace
/* -- Includes ------------------------------------------------------------- */
using namespace IfString;              // Using string namespace
using namespace IfFlags;               // Using flags namespace
/* -- Constraints ---------------------------------------------------------- */
#define CVAR_MIN_LENGTH              5 // Minimum length of a cvar name
#define CVAR_MAX_LENGTH            255 // Maximum length of a cvar name
/* ------------------------------------------------------------------------- */
enum CVarReturn                        // Callback return values
{ /* ----------------------------------------------------------------------- */
  DENY,                                // The new cvar value is not acceptable
  ACCEPT,                              // The new var value is acceptable
  ACCEPT_HANDLED,                      // The CBfunc changes cvarData.strValue
  ACCEPT_HANDLED_FORCECOMMIT,          // Same as above and mark to commit
};/* == Convert bool result cvar accept or deny ============================ */
static CVarReturn BoolToCVarReturn(const bool bState)
  { return bState ? ACCEPT : DENY; }
static bool CVarToBoolReturn(const CVarReturn cvState)
  { return cvState != DENY; }
/* -- Helper function to just set a value and return accept ---------------- */
template<typename AnyToType, typename AnyFromType>
  static CVarReturn CVarSimpleSetInt(AnyToType &atTo,
    const AnyFromType atFrom)
      { atTo = atFrom; return ACCEPT; }
template<typename AnyToType, typename AnyFromType, typename AnyRangeType>
  static CVarReturn CVarSimpleSetIntNL(AnyToType &atTo,
    const AnyFromType atFrom, const AnyRangeType artNL)
      { return atFrom < artNL ? DENY : CVarSimpleSetInt(atTo, atFrom); }
template<typename AnyToType, typename AnyFromType, typename AnyRangeType>
  static CVarReturn CVarSimpleSetIntNG(AnyToType &atTo,
    const AnyFromType atFrom, const AnyRangeType artNG)
      { return atFrom > artNG ? DENY : CVarSimpleSetInt(atTo, atFrom); }
template<typename AnyToType, typename AnyFromType, typename AnyRangeType>
  static CVarReturn CVarSimpleSetIntNGE(AnyToType &atTo,
    const AnyFromType atFrom, const AnyRangeType artNGE)
      { return atFrom >= artNGE ? DENY : CVarSimpleSetInt(atTo, atFrom); }
template<typename AnyToType, typename AnyFromType, typename AnyRangeType>
  static CVarReturn CVarSimpleSetIntNLG(AnyToType &atTo,
    const AnyFromType atFrom, const AnyRangeType artNL,
    const AnyRangeType artNG)
      { return atFrom < artNL || atFrom > artNG ?
          DENY : CVarSimpleSetInt(atTo, atFrom); }
template<typename AnyToType, typename AnyFromType, typename AnyRangeType>
  static CVarReturn CVarSimpleSetIntNLGE(AnyToType &atTo,
    const AnyFromType atFrom, const AnyRangeType artNL,
    const AnyRangeType artNGE)
      { return atFrom < artNL || atFrom >= artNGE ?
          DENY : CVarSimpleSetInt(atTo, atFrom); }
/* ------------------------------------------------------------------------- */
enum GuiMode                           // Main loop modes supported
{ /* ----------------------------------------------------------------------- */
  GM_TEXT_NOAUDIO,                     // [0] Botmode with no audio
  GM_TEXT_AUDIO,                       // [1] Botmode with audio
  GM_GRAPHICS,                         // [2] Interactive video mode
  /* ----------------------------------------------------------------------- */
  GM_HIGHEST,                          // [3] Maximum GUI mode allowed
  GM_TEXT_MAX = GM_TEXT_AUDIO          // [1] Maximum TEXT mode
};/* ----------------------------------------------------------------------- */
/* ========================================================================= */
BUILD_FLAGS(CVar,
  /* -- Types (T) ---------------------------------------------------------- */
  // Variable is a string?             Variable is a integer?
  TSTRING                {0x00000001}, TINTEGER               {0x00000002},
  // Variable is a float?              Variable is a boolean
  TFLOAT                 {0x00000004}, TBOOLEAN               {0x00000008},
  /* -- Conditional (C) ---------------------------------------------------- */
  // Var string can contain letters?   Variable string can contain numerics?
  CALPHA                 {0x00000010}, CNUMERIC               {0x00000020},
  // Variable is written to database?  Variable is protected?
  CSAVEABLE              {0x00000040}, CPROTECTED             {0x00000080},
  // Variable is compressed?           Variable string cannot be empty?
  CDEFLATE               {0x00000100}, CNOTEMPTY              {0x00000200},
  // Variable must be unsigned?        Variable integer must be power of two?
  CUNSIGNED              {0x00000400}, CPOW2                  {0x00000800},
  // Must be Pow2 but zero is allowed? Variable must be a valid filename
  CPOW2Z                 {0x00001000}, CFILENAME              {0x00002000},
  /* -- Manipulation (M) --------------------------------------------------- */
  MTRIM                  {0x00004000}, // Variable string should be trimmed
  /* -- Other (O) ---------------------------------------------------------- */
  OSAVEFORCE             {0x00008000}, // Force variable to be saved
  /* -- Privates (Used internally) ----------------------------------------- */
  // No flags                          Private vars begin after here
  NONE                   {0x00000000}, PRIVATE                {0x00010000},
  // Variable was created by LUA?      Variable is a trusted filename
  TLUA                   {PRIVATE},    CTRUSTEDFN             {0x00020000},
  // Cannot unreg this in callback?    Variable should be committed to DB?
  LOCKED                 {0x00040000}, COMMIT                 {0x00080000},
  // Var should be purged from DB?     Variable not readable by lua?
  PURGE                  {0x00100000}, CONFIDENTIAL           {0x00200000},
  // Variable value was loaded from db
  LOADED                 {0x00400000},
  /* -- Sources (S) [Private] ---------------------------------------------- */
  // Set from engine internally?       Set from command-line?
  SENGINE                {0x00800000}, SCMDLINE               {0x01000000},
  // Set from appconfig?               Set from database?
  SAPPCFG                {0x02000000}, SUDB                   {0x04000000},
  /* -- Permissions (P) [Private] ------------------------------------------ */
  // Variable can be changed at boot?  Variable can be changed by system
  PBOOT                  {0x08000000}, PSYSTEM                {0x10000000},
  // Variable can be changed by user?
  PUSR                   {0x20000000},
  /* -- Collections -------------------------------------------------------- */
  // Shortcut to unsigned int type     Shortcut to 'unsigned float' type
  TUINTEGER{ TINTEGER|CUNSIGNED },     TUFLOAT{ TFLOAT|CUNSIGNED },
  // Shortcut to int + saveable        Shortcut to float + saveable
  TINTEGERSAVE{ TINTEGER|CSAVEABLE },  TFLOATSAVE{ TFLOAT|CSAVEABLE },
  // Shortcut to uint + saveable       Shortcut to unsigned float + saveable
  TUINTEGERSAVE{ TUINTEGER|CSAVEABLE },TUFLOATSAVE{ TUFLOAT|CSAVEABLE },
  // Shortcut to string + saveable     Shortcut to boolean + saveable
  TSTRINGSAVE{ TSTRING|CSAVEABLE },    TBOOLEANSAVE{ TBOOLEAN|CSAVEABLE },
  // All perms granted to modify       // Any source
  PANY{ PBOOT|PSYSTEM|PUSR },          SANY{ SENGINE|SCMDLINE|SAPPCFG|SUDB },
  // Registration mask bits            Only alphanumeric characeters
  CVREGMASK{ COMMIT|SANY },            CALPHANUMERIC{ CALPHA|CNUMERIC },
  /* -- Allowed bits ------------------------------------------------------- */
  MASK{ TSTRING|TINTEGER|TFLOAT|TBOOLEAN|CALPHA|CNUMERIC|CSAVEABLE|CPROTECTED|
        CDEFLATE|CNOTEMPTY|CUNSIGNED|CPOW2|CPOW2Z|CFILENAME|MTRIM|OSAVEFORCE };
);/* -- Prototypes --------------------------------------------------------- */
class Item;                            // (Prototype) Cvar callback data
typedef CVarReturn (*CbFunc)(Item&, const string&); // Callback return type
/* ------------------------------------------------------------------------- */
};                                     // End of module namespace
/* == CVar library namespace =============================================== */
namespace IfCVarLib {                  // Beginning of CVar library namespace
/* -- Includes ------------------------------------------------------------- */
using namespace IfCVarDef;             // Using cvardef namespace
/* ========================================================================= */
/* ######################################################################### */
/* ## BASE ENGINE CVARS LIST                                              ## */
/* ######################################################################### */
/* ## Please note that if you add more and/or modify this order then you  ## */
/* ## need to update the 'cvKeyValueStaticList' list below this scope and ## */
/* ## then the 'cvEngList' list in 'cvarlib.hpp'.                         ## */
/* ######################################################################### */
/* ------------------------------------------------------------------------- */
enum CVarEnums : size_t
{ /* -- Critical cvars ----------------------------------------------------- */
  LOG_LEVEL,        APP_CMDLINE,         AST_LZMABUFFER,      AST_PIPEBUFFER,
  AST_FSOVERRIDE,   AST_EXEBUNDLE,       APP_BASEDIR,         AST_BUNDLES,
  APP_CONFIG,       APP_HOMEDIR,         SQL_DB,              SQL_ERASEEMPTY,
  SQL_TEMPSTORE,    SQL_SYNCHRONOUS,     SQL_JOURNALMODE,     SQL_AUTOVACUUM,
  SQL_FOREIGNKEYS,  SQL_INCVACUUM,       SQL_DEFAULTS,        SQL_LOADCONFIG,
  APP_GUIMODE,      LOG_LINES,           LOG_FILE,            APP_LONGNAME,
  APP_CLEARMUTEX,   ERR_INSTANCE,
  /* -- Object cvars ------------------------------------------------------- */
  OBJ_CMDMAX,       OBJ_CVARMAX,         OBJ_CVARIMAX,        OBJ_ARCHIVEMAX,
  OBJ_ASSETMAX,     OBJ_BINMAX,          OBJ_CURSORMAX,       OBJ_FBOMAX,
  OBJ_FONTMAX,      OBJ_FILEMAX,         OBJ_FTFMAX,          OBJ_FUNCMAX,
  OBJ_IMGMAX,       OBJ_IMGFMTMAX,       OBJ_JSONMAX,         OBJ_MASKMAX,
  OBJ_PCMMAX,       OBJ_PCMFMTMAX,       OBJ_SAMPLEMAX,       OBJ_SHADERMAX,
  OBJ_SOCKETMAX,    OBJ_SOURCEMAX,       OBJ_STATMAX,         OBJ_STREAMMAX,
  OBJ_TEXTUREMAX,   OBJ_THREADMAX,       OBJ_VIDEOMAX,
  /* -- Base cvars --------------------------------------------------------- */
  APP_SHORTNAME,    APP_DESCRIPTION,     APP_VERSION,         APP_ICON,
  APP_AUTHOR,       APP_COPYRIGHT,       APP_WEBSITE,         APP_TICKRATE,
  APP_DELAY,        APP_TITLE,
  /* -- Error cvars -------------------------------------------------------- */
  ERR_ADMIN,        ERR_CHECKSUM,        ERR_DEBUGGER,        ERR_LUAMODE,
  ERR_LMRESETLIMIT, ERR_MINVRAM,         ERR_MINRAM,
  /* -- Lua cvars ---------------------------------------------------------- */
  LUA_TICKTIMEOUT,  LUA_TICKCHECK,       LUA_CACHE,           LUA_SIZESTACK,
  LUA_GCPAUSE,      LUA_GCSTEPMUL,       LUA_RANDOMSEED,      LUA_APIFLAGS,
  LUA_SCRIPT,
  /* -- Audio cvars -------------------------------------------------------- */
  AUD_DELAY,        AUD_VOL,             AUD_INTERFACE,       AUD_CHECK,
  AUD_NUMSOURCES,   AUD_SAMVOL,          AUD_STRBUFCOUNT,     AUD_STRBUFSIZ,
  AUD_STRVOL,       AUD_FMVVOL,          AUD_HRTF,
  /* -- Console cvars ------------------------------------------------------ */
  CON_KEYPRIMARY,   CON_KEYSECONDARY,    CON_AUTOCOMPLETE,    CON_AUTOSCROLL,
  CON_AUTOCOPYCVAR, CON_HEIGHT,          CON_BLOUTPUT,        CON_BLINPUT,
  CON_DISABLED,     CON_CVSHOWFLAGS,     CON_BGCOLOUR,        CON_BGTEXTURE,
  CON_FONT,         CON_FONTFLAGS,       CON_FONTCOLOUR,      CON_FONTHEIGHT,
  CON_FONTPADDING,  CON_FONTPCMIN,       CON_FONTPCMAX,       CON_FONTSCALE,
  CON_FONTSPACING,  CON_FONTLSPACING,    CON_FONTWIDTH,       CON_FONTTEXSIZE,
  CON_INPUTMAX,     CON_PAGELINES,       CON_TMCCOLS,         CON_TMCROWS,
  CON_TMCREFRESH,   CON_TMCNOCLOSE,      CON_TMCTFORMAT,
  /* -- Fmv cvars ---------------------------------------------------------- */
  FMV_IOBUFFER,
  /* -- Input cvars -------------------------------------------------------- */
  INP_JOYDEFFDZ,    INP_JOYDEFRDZ,       INP_JOYSTICK,        INP_FSTOGGLER,
  INP_RAWMOUSE,     INP_STICKYKEY,       INP_STICKYMOUSE,
  /* -- Network cvars ------------------------------------------------------ */
  NET_CBPFLAG1,     NET_CBPFLAG2,        NET_BUFFER,          NET_RTIMEOUT,
  NET_STIMEOUT,     NET_CIPHERTLSv1,     NET_CIPHERTLSv13,    NET_CASTORE,
  NET_OCSP,         NET_USERAGENT,
  /* -- Video cvars -------------------------------------------------------- */
  VID_ALPHA,        VID_CLEAR,           VID_CLEARCOLOUR,     VID_DEBUG,
  VID_FS,           VID_FSMODE,          VID_LOCK,            VID_MONITOR,
  VID_GAMMA,        VID_FSAA,            VID_BPP,             VID_HIDPI,
  VID_STEREO,       VID_NOERRORS,        VID_SRGB,            VID_AUXBUFFERS,
  VID_SIMPLEMATRIX, VID_TEXFILTER,       VID_VSYNC,           VID_GASWITCH,
  VID_WIREFRAME,    VID_ORWIDTH,         VID_ORHEIGHT,        VID_ORASPMIN,
  VID_ORASPMAX,     VID_RFBO,            VID_RFLOATS,         VID_RCMDS,
  VID_RDTEX,        VID_RDFBO,           VID_SSTYPE,          VID_SUBPIXROUND,
  VID_QSHADER,      VID_QLINE,           VID_QPOLYGON,        VID_QCOMPRESS,
  WIN_ASPECT,       WIN_BORDER,          WIN_CLOSEABLE,       WIN_FLOATING,
  WIN_FOCUSED,      WIN_HEIGHT,          WIN_HEIGHTMIN,       WIN_MAXIMISED,
  WIN_MINIMISEAUTO, WIN_POSX,            WIN_POSY,            WIN_SIZABLE,
  WIN_THREAD,       WIN_WIDTH,           WIN_WIDTHMIN,
  /* -- Logging cvars ------------------------------------------------------ */
  LOG_CREDITS,      LOG_DYLIBS,
  /* -- Other -------------------------------------------------------------- */
  MAX_CVAR                             // Maximum cvars. DO NOT (RE)MOVE!
};/* ----------------------------------------------------------------------- */
struct TwoCStrings { const string &strN, &strV; };
typedef array<const TwoCStrings, MAX_CVAR> CVarKeyValueStaticList;
/* ------------------------------------------------------------------------- */
struct ItemStatic                      // Start of CVar static struct
{ /* ----------------------------------------------------------------------- */
  const GuiMode         guimMin;       // Minimum gui mode for this cvar
  const GuiMode         guimMax;       // Maximum gui mode for this cvar
  const TwoCStrings    &tcsVar;        // Variable name and value
  const CbFunc          cbTrigger;     // Callback trigger event
  const CVarFlagsConst &cFlags;        // Variable flags
};/* ----------------------------------------------------------------------- */
typedef array<const ItemStatic, MAX_CVAR> ItemStaticList;
extern const ItemStaticList cvEngList; // Declared in "cvarlib.hpp"
/* ------------------------------------------------------------------------- */
#define CVARDAT(n)                     IfCVarLib::cvKeyValueStaticList[n]
#define CVARNAME(n)                    CVARDAT(n).strN
/* ========================================================================= */
/* ######################################################################### */
/* ## CVAR NAME AND DEFAULT VALUES LIST                                   ## */
/* ######################################################################### */
/* ## Please note that if you add, remove cvars or modify this order then ## */
/* ## you need to update the 'CVarEnums' list above and also the          ## */
/* ## 'cvEngList' list in 'cvarlib.hpp'.                                  ## */
/* ######################################################################### */
/* ========================================================================= */
/* ######################################################################### */
/* ## CRITICAL ENGINE CVARS                                               ## */
/* ######################################################################### */
static const CVarKeyValueStaticList cvKeyValueStaticList{ {
/* ------------------------------------------------------------------------- */
// ! LOG_LEVEL
// ? Specifies the logging level...
// ? 0 (LH_DISABLED) = Nothing. Lua log function can still use this.
// ? 1 (LH_ERROR)    = For reporting only errors (which halt execution).
// ? 2 (LH_WARNING)  = For reporting only warnings (recoverable errors).
// ? 3 (LH_INFO)     = For reporting useful important messages.
// ? 4 (LH_DEBUG)    = For reporitng other information (debugging).
#if defined(ALPHA)                     // Alpha (Debug) version?
{ "log_level", "4" },                  // Default of DEBUG for log level
#elif defined(BETA)                    // Beta (Developer) version?
{ "log_level", "3" },                  // Default of INFO for log level
#else                                  // Release (Public) version?
{ "log_level", "2" },                  // Default of WARNING for log level
#endif                                 // Release type check
/* ------------------------------------------------------------------------- */
// ! APP_CMDLINE
// ? Shows the commandline sent to the application it cannot be changed at all.
{ "app_cmdline", strBlank },
/* ------------------------------------------------------------------------- */
// ! AST_LZMABUFFER
// ? Specifies the decompression buffer size (in bytes) for the lzma api. The
// ? default value is 256 kilobytes.
{ "ast_lzmabuffer", "262144" },
/* ------------------------------------------------------------------------- */
// ! AST_PIPEBUFFER
// ? Specifies the size of the pipe buffer.
{ "ast_pipebuffer", "4096" },
/* ------------------------------------------------------------------------- */
// ! AST_FSOVERRIDE
// ? Specifies whether the engine is allowed to override internal files from
// ? archives with files. This is makes it easier to update and mod resource
// ? if true. It is by default disabled on release builds and enabled on
// ? all other builds.
#if defined(RELEASE)                   // Default disabled in release builds
{ "ast_fsoverride", "0" },
#else                                  // Default enabled in other builds
{ "ast_fsoverride", "1" },
#endif                                 // Build type check
/* ------------------------------------------------------------------------- */
// ! AST_EXEBUNDLE
// ? A boolean to specify if the executable file should be checked for a
// ? 7-zip archive and use that to load guest assets. This is only supported
// ? on Windows architectures right now and ignored on others.
{ "ast_exebundle", "1" },
/* ------------------------------------------------------------------------- */
// ! APP_BASEDIR
// ? Specifies the base directory of where the executable was started from. It
// ? is only readable by the end-user and the host, but not the guest.
{ "app_basedir", strBlank },
/* ------------------------------------------------------------------------- */
// ! AST_BUNDLES
// ? Specifies the [.ext]ension to use as 7-zip archives. These filenames will
// ? be checked for at startup and automatically loaded by the engine.
{ "ast_bundles", "." ARCHIVE_EXTENSION },
/* ------------------------------------------------------------------------- */
// ! APP_CONFIG
// ? Loads the configuration file with the specified extension (usually 'app').
// ? The suffix extension of '.cfg' is always assumed for protection so the
// ? guest need never specify it. This variable can only be set at the
// ? command-line.
{ "app_config", DEFAULT_CONFIGURATION },
/* ------------------------------------------------------------------------- */
// ! APP_HOMEDIR
// ? Species the users home directory where files are written to if they cannot
// ? be written to the working directory. It is only readable by the end-user
// ? and the host, but not the guest.
{ "app_homedir", strBlank },
/* ------------------------------------------------------------------------- */
// ! SQL_DB
// ? Not explained yet.
{ "sql_db", strBlank },
/* ------------------------------------------------------------------------- */
// ! SQL_ERASEEMPTY
// ? Not explained yet.
{ "sql_eraseempty", "1" },
/* ------------------------------------------------------------------------- */
// ! SQL_TEMPSTORE
// ? Not explained yet.
{ "sql_tempstore", "MEMORY" },
/* ------------------------------------------------------------------------- */
// ! SQL_SYNCHRONOUS
// ? Not explained yet.
{ "sql_synchronous", "0" },
/* ------------------------------------------------------------------------- */
// ! SQL_JOURNALMODE
// ? Not explained yet.
{ "sql_journalmode", "0" },
/* ------------------------------------------------------------------------- */
// ! SQL_AUTOVACUUM
// ? Not explained yet.
{ "sql_autovacuum", "1" },
/* ------------------------------------------------------------------------- */
// ! SQL_FOREIGNKEYS
// ? Not explained yet.
{ "sql_foreignkeys", "1" },
/* ------------------------------------------------------------------------- */
// ! SQL_INCVACUUM
// ? Not explained yet.
{ "sql_incvacuum", "0" },
/* ------------------------------------------------------------------------- */
// ! SQL_DEFAULTS
// ? Not explained yet.
{ "sql_defaults", "0" },
/* ------------------------------------------------------------------------- */
// ! SQL_LOADCONFIG
// ? Not explained yet.
{ "sql_loadconfig", "1" },
/* ------------------------------------------------------------------------- */
// ! APP_GUIMODE
// ? Specifies how the host wants to be run...
// ? 0 (GM_TEXT_NOAUDIO) = Botmode with no audio.
// ? 1 (GM_TEXT_AUDIO)   = Botmode with audio.
// ? 2 (GM_GRAPHICS)     = Interactive mode.
// ? Bot modes will show a terminal console window instead of an OpenGL window
// ? when the gpu is not needed by the guest. In interactive mode, you must
// ? use Fbo.Draw() in Lua to allow the engine draw the frame at the end of the
// ? frame which is useful for preserving cpu/gpu cycles when the screen does.
// ? not need to be redrawn. Certain commands, cvars, lua api functions are
// ? hidden and shown depending on this value. It can only be set in the guest
// ? app.cfg file.
{ "app_guimode", "0" },
/* ------------------------------------------------------------------------- */
// ! LOG_LINES
// ? Specifies the maximum number of lines to keep in the backlog. The log
// ? lines are always empty when logging to a file.
{ "log_lines", "10000" },
/* ------------------------------------------------------------------------- */
// ! LOG_FILE
// ? Specifies a file to log internal engine messages to. It is used to help
// ? debugging. Leave as blank for no log file. It can be set from command-line
// ? parameters, the app.cfg file and the user.
{ "log_file", strBlank },
/* ------------------------------------------------------------------------- */
// ! APP_LONGNAME
// ? Specifies a long title name for the guest application. It is uses as the
// ? window title and for display purposes only to the end-user. It can be
// ? requested by the guest and only set in the app.cfg file.
{ "app_longname", "Untitled" },
/* ------------------------------------------------------------------------- */
// ! APP_CLEARMUTEX
// ? Asks the system to delete the global mutex in Linux and MacOS so the
// ? engine can startup. You only need to do this if the engine crashes. On
// ? Windows the mutex is freed on process termination so there is never any
// ? need to use this on Windows.
#if !defined(WINDOWS) && defined(ALPHA)
{ "app_clearmutex", "1" },
#else
{ "app_clearmutex", "0" },
#endif
/* ------------------------------------------------------------------------- */
// ! ERR_INSTANCE
// ? Tries to activate an existing window of the same name if another instance
// ? is running. On Windows, mutexes are always cleaned up properly even in a
// ? crash but not on Linux and OSX. So just simply remove the app name from
// ? /dev/shm and the engine should run again.
{ "err_instance", "1" },
/* ========================================================================= */
/* ######################################################################### */
/* ## OBJECT CVARS                                                        ## */
/* ######################################################################### */
/* ------------------------------------------------------------------------- */
// ! OBJ_CMDMAX
// ? Specifies the maximum number of console commands allowed to be registered
// ? by the engine. This includes the internal default commands. An exception
// ? is generated if more cvars than this are allocated.
{ "obj_cmdmax", "1000" },
/* ------------------------------------------------------------------------- */
// ! OBJ_CVARMAX
// ? Specifies the maximum number of cvars allowed to be registered by the
// ? engine. This includes the internal default cvars. An exception is
// ? generated if more cvars than this are allocated.
{ "obj_cvarmax", "1000" },
/* ------------------------------------------------------------------------- */
// ! OBJ_CVARIMAX
// ? Specifies the maximum number of initial cvars allowed to be registered by
// ? the engine. Initial cvars are in a temporary state, they are used when
// ? the corresponding cvar is registered, and passed back to this temporary
// ? state when the cvar is unregistered. The temporary state of all initial
// ? cvars are then commited to database at exit and on-demand. An exception is
// ? generated if more initial cvars than this are allocated.
{ "obj_cvarimax", "1000" },
/* ------------------------------------------------------------------------- */
// ! OBJ_ARCHIVEMAX
// ? Specifies the maximum number of archive objects allowed to be registered
// ? by the engine. This includes the archives that are loaded at startup
// ? An exception is generated if more archives than this are allocated.
{ "obj_archivemax", "1000" },
/* ------------------------------------------------------------------------- */
// ! OBJ_ASSETMAX
// ? Specifies the maximum number of asset objects allowed to be registered by
// ? the engine. An exception is generated if more cvars than this are
// ? allocated.
{ "obj_assetmax", "1000" },
/* ------------------------------------------------------------------------- */
// ! OBJ_BINMAX
// ? Specifies the maximum number of bin objects allowed to be registered by
// ? the engine. An exception is generated if more cvars than this are
// ? allocated.
{ "obj_binmax", "1000" },
/* ------------------------------------------------------------------------- */
// ! OBJ_CURSORMAX
// ? Specifies the maximum number of cursor objects allowed to be registered by
// ? the engine. An exception is generated if more cvars than this are
// ? allocated.
{ "obj_cursormax", "1000" },
/* ------------------------------------------------------------------------- */
// ! OBJ_FBOMAX
// ? Specifies the maximum number of framebuffer objects allowed to be
// ? registered by the engine. An exception is generated if more cvars than
// ? this are allocated.
{ "obj_fbomax", "1000" },
/* ------------------------------------------------------------------------- */
// ! OBJ_FONTMAX
// ? Specifies the maximum number of font objects allowed to be registered by
// ? the engine. An exception is generated if more cvars than this are
// ? allocated.
{ "obj_fontmax", "1000" },
/* ------------------------------------------------------------------------- */
// ! OBJ_FILEMAX
// ? Specifies the maximum number of file objects allowed to be registered by
// ? the engine. An exception is generated if more cvars than this are
// ? allocated.
{ "obj_filemax", "1000" },
/* ------------------------------------------------------------------------- */
// ! OBJ_FTFMAX
// ? Specifies the maximum number of freetype objects allowed to be registered
// ? by the engine. An exception is generated if more cvars than this are
// ? allocated.
{ "obj_ftfmax", "1000" },
/* ------------------------------------------------------------------------- */
// ! OBJ_FUNCMAX
// ? Specifies the maximum number of lua referenced function objects allowed to
// ? be registered by the engine. An exception is generated if more cvars than
// ? this are allocated.
{ "obj_funcmax", "1000" },
/* ------------------------------------------------------------------------- */
// ! OBJ_IMGMAX
// ? Specifies the maximum number of image objects allowed to be registered by
// ? the engine. An exception is generated if more cvars than this are
// ? allocated.
{ "obj_imgmax", "1000" },
/* ------------------------------------------------------------------------- */
// ! OBJ_IMGFMTMAX
// ? Specifies the maximum number of image format objects allowed to be
// ? registered by the engine. An exception is generated if more cvars than
// ? this are allocated.
{ "obj_imgfmtmax", "1000" },
/* ------------------------------------------------------------------------- */
// ! OBJ_JSONMAX
// ? Specifies the maximum number of json objects allowed to be registered by
// ? the engine. An exception is generated if more cvars than this are
// ? allocated.
{ "obj_jsonmax", "1000" },
/* ------------------------------------------------------------------------- */
// ! OBJ_MASKMAX
// ? Specifies the maximum number of mask objects allowed to be registered by
// ? the engine. An exception is generated if more cvars than this are
// ? allocated.
{ "obj_maskmax", "1000" },
/* ------------------------------------------------------------------------- */
// ! OBJ_PCMMAX
// ? Specifies the maximum number of pcm objects allowed to be registered by
// ? the engine. An exception is generated if more objects than this are
// ? allocated.
{ "obj_pcmmax", "1000" },
/* ------------------------------------------------------------------------- */
// ! OBJ_PCMFMTMAX
// ? Specifies the maximum number of pcm format objects allowed to be
// ? registered by the engine. An exception is generated if more objects than
// ? this are allocated.
{ "obj_pcmfmtmax", "1000" },
/* ------------------------------------------------------------------------- */
// ! OBJ_SAMPLEMAX
// ? Specifies the maximum number of sample objects allowed to be
// ? registered by the engine. An exception is generated if more objects than
// ? this are allocated.
{ "obj_samplemax", "1000" },
/* ------------------------------------------------------------------------- */
// ! OBJ_SHADERMAX
// ? Specifies the maximum number of shader objects allowed to be
// ? registered by the engine. An exception is generated if more objects than
// ? this are allocated.
{ "obj_shadermax", "1000" },
/* ------------------------------------------------------------------------- */
// ! OBJ_SOCKETMAX
// ? Specifies the maximum number of socket objects allowed to be
// ? registered by the engine. An exception is generated if more objects than
// ? this are allocated.
{ "obj_socketmax", "1000" },
/* ------------------------------------------------------------------------- */
// ! OBJ_SOURCEMAX
// ? Specifies the maximum number of source objects allowed to be
// ? registered by the engine. An exception is generated if more objects than
// ? this are allocated.
{ "obj_sourcemax", "1000" },
/* ------------------------------------------------------------------------- */
// ! OBJ_STATMAX
// ? Specifies the maximum number of stat objects allowed to be
// ? registered by the engine. An exception is generated if more objects than
// ? this are allocated.
{ "obj_statmax", "1000" },
/* ------------------------------------------------------------------------- */
// ! OBJ_STREAMMAX
// ? Specifies the maximum number of stream objects allowed to be
// ? registered by the engine. An exception is generated if more objects than
// ? this are allocated.
{ "obj_streammax", "1000" },
/* ------------------------------------------------------------------------- */
// ! OBJ_TEXTUREMAX
// ? Specifies the maximum number of texture objects allowed to be
// ? registered by the engine. An exception is generated if more objects than
// ? this are allocated.
{ "obj_texturemax", "1000" },
/* ------------------------------------------------------------------------- */
// ! OBJ_THREADMAX
// ? Specifies the maximum number of thread objects allowed to be
// ? registered by the engine. An exception is generated if more objects than
// ? this are allocated.
{ "obj_threadmax", "1000" },
/* ------------------------------------------------------------------------- */
// ! OBJ_VIDEOMAX
// ? Specifies the maximum number of video objects allowed to be
// ? registered by the engine. An exception is generated if more objects than
// ? this are allocated.
/* ------------------------------------------------------------------------- */
{ "obj_videomax", "1000" },
/* ========================================================================= */
/* ######################################################################### */
/* ## BASE CVARS                                                          ## */
/* ######################################################################### */
/* ------------------------------------------------------------------------- */
// ! APP_SHORTNAME
// ? Specifies a short title name for the guest application. It is purely for
// ? display purposes only to the end-user. It can be requested by the guest
// ? and only set in the app.cfg file.
{ "app_shortname", "Untitled" },
/* ------------------------------------------------------------------------- */
// ! APP_DESCRIPTION
// ? Specifies a description for the guest application. It is purely for
// ? display purposes only to the end-user. It can be requested by the guest
// ? and only set in the app.cfg file.
{ "app_description", "Undescribed" },
/* ------------------------------------------------------------------------- */
// ! APP_VERSION
// ? Specifies a version for the guest application. It is purely for display
// ? purposes only to the end-user. It can be requested by the guest and only
// ? set in the app.cfg file.
{ "app_version", "0.0" },
/* ------------------------------------------------------------------------- */
// ! APP_ICON
// ? Specifies a large icon for use with the application and window. It can be
// ? any format that is supported with the Image.* loading functions. It can
// ? also be changed dynamically by Lua. Keep to empty for default icon.
{ "app_icon", strBlank },
/* ------------------------------------------------------------------------- */
// ! APP_AUTHOR
// ? Specifies the author of the guest application. This is used for display
// ? purposes only and can be requested by the guest and only set in the
// ? app.cfg file.
{ "app_author", "Anonymous" },
/* ------------------------------------------------------------------------- */
// ! APP_COPYRIGHT
// ? Specifies any copyright information for the guest application. It is
// ? purely for display purposes only to the end-user. It can be requested by
// ? the guest and only set in the app.cfg file.
{ "app_copyright", "Public Domain" },
/* ------------------------------------------------------------------------- */
// ! APP_WEBSITE
// ? Specifies a website for the guest application. It is purely for display
// ? purposes only to the end-user. It can be requested by the guest and only
// ? set in the app.cfg file.
{ "app_website", "about:blank" },
/* ------------------------------------------------------------------------- */
// ! APP_TICKRATE
// ? Specifies the base tick rate of the guest application in ticks when using
// ? the timer accumulator (GuImode=GM_INT_ACCU||GM_INT_MF_ACCU). The engine
// ? locks to this tick rate regardless of lag. It can also be changed
// ? dynamically with Lua.
{ "app_tickrate", "60" },
/* ------------------------------------------------------------------------- */
// ! APP_DELAY
// ? Specifies an artificial delay to force for bot mode in milliseconds. This
// ? is ignored on interactive mode because a one millisecond delay is forced
// ? for every frame under the target rate.
{ "app_delay", "1" },
/* ------------------------------------------------------------------------- */
// ! APP_TITLE
// ? Sets a custom title for the window. This can only be changed at the
// ? command-line or the application configuration file and not saved to the
// ? persistence database. It only applies to the Win32 terminal window (not
// ? MacOS nor Linux terminal windows) and to any OS desktop windows. If not
// ? specified, the default is used in the format of 'L V (T)' where 'L' is the
// ? value of 'app_longname', where 'V' is the value of 'app_version' and 'T'
// ? is the target executable architechture.
{ "app_title", strBlank },
/* ========================================================================= */
/* ######################################################################### */
/* ## ERROR CONDITION VARIABLES                                           ## */
/* ######################################################################### */
/* ------------------------------------------------------------------------- */
// ! ERR_ADMIN
// ? Throws an error if the user is running the engine with elevated
// ? privileges. This can be one of three values...
// ? 0 = The engine is allowed to be run with elevated privileges.
// ? 1 = The engine is NOT allowed to be run with elevated privileges.
// ? 2 = Same as 1 but ALLOWS when OS's has admin as default (i.e. XP!).
// ? The default value is 2.
{ "err_admin", "2" },
/* ------------------------------------------------------------------------- */
// ! ERR_CHECKSUM
// ? Throws an error if there is an executable checksum mismatch. This only
// ? applies on Windows executables only as Linux executable does not have a
// ? checksum and MacOS uses code signing externally. The default value is 1
// ? on release executable else 0 on beta executable.
#if defined(RELEASE)
{ "err_checksum", "1" },
#else
{ "err_checksum", "0" },
#endif
/* ------------------------------------------------------------------------- */
// ! ERR_DEBUGGER
// ? Throws an error if a debugger is running at start-up. The default value
// ? is 1 on release executable else 0 on beta executable.
#if defined(RELEASE)
{ "err_debugger", "1" },
#else
{ "err_debugger", "0" },
#endif
/* ------------------------------------------------------------------------- */
// ! ERR_LUAMODE
// ? Sets how to handle a LUA script error to one of these values...
// ? 0 (LEM_IGNORE)   = Ignore errors and try to continue.
// ? 1 (LEM_RESET)    = Automatically reset on error.
// ? 2 (LEM_SHOW)     = Open console and show error.
// ? 3 (LEM_CRITICAL) = Terminate engine with error.
// ? The default value is 3 for release executable and 2 for beta executable.
#if defined(RELEASE)
{ "err_luamode", "3" },
#else
{ "err_luamode", "2" },
#endif
/* ------------------------------------------------------------------------- */
// ! ERR_LMRESETLIMIT
// ? When ERR_LUAMODE is set to 1, this specifies the number of LUA script
// ? errors that are allowed before an error after this is treated as a
// ? critical error. The default value for release executable is 1000 and
// ? 10 for beta executable.
#if defined(RELEASE)
{ "err_lmresetlimit", "1000" },
#else
{ "err_lmresetlimit", "10" },
#endif
/* ------------------------------------------------------------------------- */
// ! ERR_MINVRAM
// ? The engine fails to run if the system does not have this amount of VRAM
// ? available.
{ "err_minvram", "0" },
/* ------------------------------------------------------------------------- */
// ! ERR_MINRAM
// ? The engine fails to run if the system does not have this amount of free
// ? memory available. If this value is zero then the check does not take
// ? place else the amount specified is allocated at startup, cleared, the
// ? speed of the clear is reported in the log then the memory is freed. The
// ? default value is zero.
{ "err_minram", "0" },
/* ========================================================================= */
/* ######################################################################### */
/* ## LUA CVARS                                                           ## */
/* ######################################################################### */
/* ------------------------------------------------------------------------- */
// ! LUA_TICKTIMEOUT
// ? Not explained yet.
{ "lua_ticktimeout", "10" },
/* ------------------------------------------------------------------------- */
// ! LUA_TICKCHECK
// ? Not explained yet.
{ "lua_tickcheck", "1000000" },
/* ------------------------------------------------------------------------- */
// ! LUA_CACHE
// ? Not explained yet.
{ "lua_cache", "1" },
/* ------------------------------------------------------------------------- */
// ! LUA_SIZESTACK
// ? Not explained yet.
{ "lua_sizestack", "1000" },
/* ------------------------------------------------------------------------- */
// ! LUA_GCPAUSE
// ? Not explained yet.
{ "lua_gcpause", "200" },
/* ------------------------------------------------------------------------- */
// ! LUA_GCSTEPMUL
// ? Not explained yet.
{ "lua_gcstepmul", "100" },
/* ------------------------------------------------------------------------- */
// ! LUA_RANDOMSEED
// ? Not explained yet.
{ "lua_randomseed", "0" },
/* ------------------------------------------------------------------------- */
// ! LUA_APIFLAGS
// ? Not explained yet.
{ "lua_apiflags", "3" },
/* ------------------------------------------------------------------------- */
// ! LUA_SCRIPT
// ? Not explained yet.
{ "lua_script", "main.lua" },
/* ========================================================================= */
/* ######################################################################### */
/* ## AUDIO CVARS                                                         ## */
/* ######################################################################### */
/* ------------------------------------------------------------------------- */
// ! AUD_DELAY
// ? Specifies an delay (in number of milliseconds) to suspend the audio
// ? thread each tick. Lower values use less CPU usage but may cause audio
// ? pops. The audio thread is in charge of managing Source classes and
// ? checking for discreprencies.
{ "aud_delay", "10" },
/* ------------------------------------------------------------------------- */
// ! AUD_VOL
// ? Specifies the global volume of all class types. 0.0 (mute) to
// ? 1.0 (maximum volume). Setting a value too high may cause artifacts on
// ? older OS audio API's.
{ "aud_vol", "0.75" },
/* ------------------------------------------------------------------------- */
// ! AUD_INTERFACE
// ? Specifies the audio device number to use. -1 makes sure to use the default
// ? device, otherwise it is the index number of a specific audio device to
// ? use.
{ "aud_interface", "-1" },
/* ------------------------------------------------------------------------- */
// ! AUD_CHECK
// ? Specifies an interval (in number of milliseconds) of checking for audio
// ? device changes as OpenAL does not have an event interface for this. This
// ? check is done in the audio manager thread. If a change in the audio
// ? device list is detected then the audio is reset automatically, as if the
// ? 'areset' console command was used.
{ "aud_check", "5000" },
/* ------------------------------------------------------------------------- */
// ! AUD_NUMSOURCES
// ? Specifies the maximum number of sources to preallocate. 0 means sources
// ? are (de)allocated dynamically. Setting this value to non-zero may improve
// ? CPU usage but increases the chances of errors if too many sources are
// ? requested.
{ "aud_numsources", "0" },
/* ------------------------------------------------------------------------- */
// ! AUD_SAMVOL
// ? Specifies the volume of Sample classes. 0.0 (mute) to
// ? 1.0 (maximum volume). Setting a value too high may cause artifacts on
// ? older OS audio API's.
{ "aud_samvol", "0.75" },
/* ------------------------------------------------------------------------- */
// ! AUD_STRBUFCOUNT
// ? Specifies the number of buffers to use for Stream classes. We need
// ? multiple buffers to make streamed audio playback consistent. Four should
// ? always be enough.
{ "aud_strbufcount", "4" },
/* ------------------------------------------------------------------------- */
// ! AUD_STRBUFSIZ
// ? Specifies the size (in bytes) of each stream buffer for Stream classes.
// ? We need sufficiently large enough buffers to make streamed audio playback
// ? consistent.
{ "aud_strbufsiz", "65536" },
/* ------------------------------------------------------------------------- */
// ! AUD_STRVOL
// ? Specifies the volume of Stream classes. 0.0 (mute) to
// ? 1.0 (maximum volume). Setting a value too high may cause artifacts on
// ? older OS audio API's.
{ "aud_strvol", "0.75" },
/* ------------------------------------------------------------------------- */
// ! AUD_FMVVOL
// ? Specifies the volume of Video classes. 0.0 (mute) to 1.0 (maximum volume).
// ? Setting a value too high may cause artifacts on older OS audio API's.
{ "aud_fmvvol", "0.75" },
/* ------------------------------------------------------------------------- */
// ! AUD_HRTF
// ? Specifies whether to use HRTF dynamics on audio output. This could cause
// ? strange audio stereo quality issues so it is recommended to disable.
{ "aud_hrtf", "0" },
/* ========================================================================= */
/* ######################################################################### */
/* ## CONSOLE CVARS                                                       ## */
/* ######################################################################### */
/* ------------------------------------------------------------------------- */
// ! CON_KEYPRIMARY
// ? The primary GLFW console key virtual key code to use to toggle console
// ? visibility. It is set to '`' as default on a UK keyboard.
{ "con_keyprimary", "96" },
/* ------------------------------------------------------------------------- */
// ! CON_KEYSECONDARY
// ? The secondary GLFW console key virtual key code to use to toggle console
// ? visibility. It is set to '+-' as default on a US keyboard.
{ "con_keysecondary", "161" },
/* ------------------------------------------------------------------------- */
// ! CON_AUTOCOMPLETE
// ? Flags that specify what to auto complete when TAB key is pressed in the
// ? console...
// ? AC_NONE     (0x0): Autocompletion is disabled
// ? AC_COMMANDS (0x1): Autocomplete command names
// ? AC_CVARS    (0x2): Autocomplete cvar names
{ "con_autocomplete", "3" },
/* ------------------------------------------------------------------------- */
// ! CON_AUTOSCROLL
// ? A boolean that specifies whether to autoscroll the console to the last
// ? newly printed message.
{ "con_autoscroll", "1" },
/* ------------------------------------------------------------------------- */
// ! CON_AUTOCOPYCVAR
// ? A boolean that specifies whether to automatically copy the variable
// ? name and value when a cvar name is typed in the console.
{ "con_autocopycvar", "1" },
/* ------------------------------------------------------------------------- */
// ! CON_HEIGHT
// ? Specifies the height of the console in OpenGL mode. 1 means the console
// ? is covering the whole screen, and 0 means the console is not showing.
{ "con_height", "0.5" },
/* ------------------------------------------------------------------------- */
// ! CON_BLOUTPUT
// ? Specifies the maximum number of lines to keep in the console. Excess
// ? messages are discarded automatically to not fill up memory.
{ "con_bloutput", "10000" },
/* ------------------------------------------------------------------------- */
// ! CON_BLINPUT
// ? Number of lines to store in the input backlog. A successful command input
// ? automatically adds the last command typed into this list.
{ "con_blinput", "1000" },
/* ------------------------------------------------------------------------- */
// ! CON_DISABLED
// ? Specifies wether the console is permanantly disabled or not.
#if defined(RELEASE)
{ "con_disabled", "1" },
#else
{ "con_disabled", "0" },
#endif
/* ------------------------------------------------------------------------- */
// ! CON_CVSHOWFLAGS
// ? Flags specifying how to show CVar values in the console to protect
// ? exposure of sensetive information...
// ? SF_NONE         (0): Do not show cvars marked as private or protected.
// ? SF_CONFIDENTIAL (1): Show cvars marked as private.
// ? SF_PROTECTED    (2): Show cvars marked as protected.
{ "con_cvshowflags", "0" },
/* ------------------------------------------------------------------------- */
// ! CON_BGCOLOUR
// ? Specifies the background colour of the console texture in the syntax of
// ? 0xAARRGGBB or an integral number.
{ "con_bgcolour", "2130706432" },
/* ------------------------------------------------------------------------- */
// ! CON_BGTEXTURE
// ? The texture file to load that will be used as the background for the
// ? console.
{ "con_bgtexture", strBlank },
/* ------------------------------------------------------------------------- */
// ! CON_FONT
// ? Specifies the filename of the FreeType compatible font to load as the
// ? console font file.
{ "con_font", "console.ttf" },
/* ------------------------------------------------------------------------- */
// ! CON_FONTFLAGS
// ? Specifies manipulation of the loaded font. See the 'Char.Flags' for
// ? possible values
{ "con_fontflags", "0" },
/* ------------------------------------------------------------------------- */
// ! CON_FONTCOLOUR
// ? Specifies the default colour of the console text. See the Console.Colours
// ? lookup table for possible values. There are only 16 pre-defined colours
// ? to keep compatibility with the system text console.
{ "con_fontcolour", "15" },
/* ------------------------------------------------------------------------- */
// ! CON_FONTHEIGHT
// ? Specifies the height of the loaded console FreeType font.
{ "con_fontheight", "20" },
/* ------------------------------------------------------------------------- */
// ! CON_FONTPADDING
// ? Specifies any extra padding to add to each FreeType font glyph to prevent
// ? pixels from other glyphs spilling into the render due to filtering.
{ "con_fontpadding", "0" },
/* ------------------------------------------------------------------------- */
// ! CON_FONTPCMIN
// ? Specifies the minimum character code to start precaching from
{ "con_fontpcmin", "32" },
/* ------------------------------------------------------------------------- */
// ! CON_FONTPCMAX
// ? Specifies the maximum character code to end precaching at
{ "con_fontpcmax", "256" },
/* ------------------------------------------------------------------------- */
// ! CON_FONTSCALE
// ? Specifies the scale adjust of the font. A value not equal to one will
// ? cause interpolation to occur so filtering is advised.
{ "con_fontscale", "1" },
/* ------------------------------------------------------------------------- */
// ! CON_FONTSPACING
// ? Specifies the amount of padding to add after each cahracter rendered.
{ "con_fontspacing", "0" },
/* ------------------------------------------------------------------------- */
// ! CON_FONTLSPACING
// ? Specifies the amount of padding to add below each line of text rendered.
{ "con_fontlspacing", "0" },
/* ------------------------------------------------------------------------- */
// ! CON_FONTWIDTH
// ? Specifies the width of the loaded console FreeType font.
{ "con_fontwidth", "20" },
/* ------------------------------------------------------------------------- */
// ! CON_FONTTEXSIZE
// ? Specifies the initial size of the texture canvas in pixels. Do not set
// ? this more than 1024 pixels unless you know for a fact the GPU will support
// ? this value. An exception is thrown if not. Zero means start with enough
// ? size for one character.
{ "con_fonttexsize", "0" },
/* ------------------------------------------------------------------------- */
// ! CON_INPUTMAX
// ? Specifies the maximum number of characters that are allowed to be typed
// ? into the console input buffer.
{ "con_inputmax", "1000" },
/* ------------------------------------------------------------------------- */
// ! CON_PAGELINES
// ? The number of lines that are scrolled when pressing CONTROL and PAGEUP or
// ? PAGEDOWN keys.
{ "con_pagelines", "10" },
/* ------------------------------------------------------------------------- */
// ! CON_TMCCOLS
// ? In bot mode, this is the maximum number of columns to draw. The larger
// ? this value is, the more text that can be displayed at once but will
// ? increase CPU usage from the operating system.
{ "con_tmccols", "80" },
/* ------------------------------------------------------------------------- */
// ! CON_TMCROWS
// ? In bot mode, this is the maximum number of rows to draw. The larger
// ? this value is, the more text that can be displayed at once but will
// ? increase CPU usage from the operating system.
{ "con_tmcrows", "32" },
/* ------------------------------------------------------------------------- */
// ! CON_TMCREFRESH
// ? The interval in milliseconds where the status bar and title bar are
// ? refreshed. The title bar contains basic operating statistics and the
// ? status bar is configurable by LUA and contains the input bar when text
// ? is being typed.
{ "con_tmcrefresh", "1000" },
/* ------------------------------------------------------------------------- */
// ! CON_TMCNOCLOSE
// ? Disables the X button and close in the application context menu. This does
// ? not prevent CONTROL+C/BREAK keys.
{ "con_tmcnoclose", "1" },
/* ------------------------------------------------------------------------- */
// ! CON_TMCTFORMAT
// ? The format of the time in the titlebar (see strftime() for details).
{ "con_tmctformat", "%y-%m-%d %T" },
/* ========================================================================= */
/* ######################################################################### */
/* ## VORBIS CVARS                                                        ## */
/* ######################################################################### */
/* ------------------------------------------------------------------------- */
// ! FMV_IOBUFFER
// ? Not explained yet.
{ "fmv_iobuffer", "65536" },
/* ========================================================================= */
/* ######################################################################### */
/* ## INPUT CVARS                                                         ## */
/* ######################################################################### */
/* ------------------------------------------------------------------------- */
// ! INP_JOYDEFFDZ
// ? Not explained yet.
{ "inp_joydeffdz", "0.25" },
/* ------------------------------------------------------------------------- */
// ! INP_JOYDEFRDZ
// ? Not explained yet.
{ "inp_joydefrdz", "0.25" },
/* ------------------------------------------------------------------------- */
// ! INP_JOYSTICK
// ? Not explained yet.
{ "inp_joystick", "-1" },
/* ------------------------------------------------------------------------- */
// ! INP_FSTOGGLER
// ? Not explained yet.
{ "inp_fstoggler", "1" },
/* ------------------------------------------------------------------------- */
// ! INP_RAWMOUSE
// ? Not explained yet.
{ "inp_rawmouse", "1" },
/* ------------------------------------------------------------------------- */
// ! INP_STICKYKEY
// ? Not explained yet.
{ "inp_stickykey", "1" },
/* ------------------------------------------------------------------------- */
// ! INP_STICKYMOUSE
// ? Not explained yet.
{ "inp_stickymouse", "1" },
/* ========================================================================= */
/* ######################################################################### */
/* ## NETWORK CVARS                                                       ## */
/* ######################################################################### */
/* ------------------------------------------------------------------------- */
// ! NET_CBPFLAG1
// ? Not explained yet.
{ "net_cbpflag1", "0" },
/* ------------------------------------------------------------------------- */
// ! NET_CBPFLAG2
// ? Not explained yet.
{ "net_cbpflag2", "0" },
/* ------------------------------------------------------------------------- */
// ! NET_BUFFER
// ? Not explained yet.
{ "net_buffer", "65536" },
/* ------------------------------------------------------------------------- */
// ! NET_RTIMEOUT
// ? Not explained yet.
{ "net_rtimeout", "120" },
/* ------------------------------------------------------------------------- */
// ! NET_STIMEOUT
// ? Not explained yet.
{ "net_stimeout", "30" },
/* ------------------------------------------------------------------------- */
// ! NET_CIPHERTLSv1
// ? Not explained yet.
{ "net_ciphertlsv1", "HIGH:!DSS:!aNULL@STRENGTH" },
/* ------------------------------------------------------------------------- */
// ! NET_CIPHERTLSv13
// ? Not explained yet.
{ "net_ciphertlsv13", strBlank },
/* ------------------------------------------------------------------------- */
// ! NET_CASTORE
// ? Not explained yet.
{ "net_castore", strBlank },
/* ------------------------------------------------------------------------- */
// ! NET_OCSP
// ? Not explained yet.
{ "net_ocsp", "1" },
/* ------------------------------------------------------------------------- */
// ! NET_USERAGENT
// ? Not explained yet.
{ "net_useragent", strBlank },
/* ========================================================================= */
/* ######################################################################### */
/* ## VIDEO CVARS                                                         ## */
/* ######################################################################### */
/* ------------------------------------------------------------------------- */
// ! VID_ALPHA
// ? Not explained yet.
{ "vid_alpha", "0" },
/* ------------------------------------------------------------------------- */
// ! VID_CLEAR
// ? Not explained yet.
{ "vid_clear", "1" },
/* ------------------------------------------------------------------------- */
// ! VID_CLEARCOLOUR
// ? Not explained yet.
{ "vid_clearcolour", "0" },
/* ------------------------------------------------------------------------- */
// ! VID_DEBUG
// ? Not explained yet.
{ "vid_debug", "0" },
/* ------------------------------------------------------------------------- */
// ! VID_FS
// ? Not explained yet.
{ "vid_fs", "0" },
/* ------------------------------------------------------------------------- */
// ! VID_FSMODE
// ? Specifies which full-screen mode to use. You can check the log or use
// ? 'vmlist' console command to see which resolutions are available on your
// ? system. Anything >= -1 means exclusive full-screen mode and -1 makes the
// ? engine use the current desktop resolution (default on Mac for now). Use -2
// ? to make the engine use borderless full-screen mode (default on Win/Linux).
#if defined(MACOS)
{ "vid_fsmode", "-1" },
#else
{ "vid_fsmode", "-2" },
#endif
/* ------------------------------------------------------------------------- */
// ! VID_LOCK
// ? Not explained yet.
{ "vid_lock", "0" },
/* ------------------------------------------------------------------------- */
// ! VID_MONITOR
// ? Not explained yet.
{ "vid_monitor", "-1" },
/* ------------------------------------------------------------------------- */
// ! VID_GAMMA
// ? Not explained yet.
{ "vid_gamma", "1" },
/* ------------------------------------------------------------------------- */
// ! VID_FSAA
// ? Not explained yet.
{ "vid_fsaa", "-1" },
/* ------------------------------------------------------------------------- */
// ! VID_BPP
// ? Not explained yet.
#if defined(WINDOWS)
{ "vid_bpp", "0" },                    // Win32 doesn't need forcing depth
#else
{ "vid_bpp", "16" },                   // For better bit-depth quality
#endif
/* ------------------------------------------------------------------------- */
// ! VID_HIDPI
// ? Not explained yet.
{ "vid_hidpi", "1" },
/* ------------------------------------------------------------------------- */
// ! VID_STEREO
// ? Not explained yet.
{ "vid_stereo", "0" },
/* ------------------------------------------------------------------------- */
// ! VID_NOERRORS
// ? Not explained yet.
{ "vid_noerrors", "0" },
/* ------------------------------------------------------------------------- */
// ! VID_SRGB
// ? Not explained yet.
{ "vid_srgb", "1" },
/* ------------------------------------------------------------------------- */
// ! VID_AUXBUFFERS
// ? Not explained yet.
{ "vid_auxbuffers", "-1" },
/* ------------------------------------------------------------------------- */
// ! VID_SIMPLEMATRIX
// ? Not explained yet.
{ "vid_simplematrix", "0" },
/* ------------------------------------------------------------------------- */
// ! VID_TEXFILTER
// ? Not explained yet.
{ "vid_texfilter", "3" },
/* ------------------------------------------------------------------------- */
// ! VID_VSYNC
// ? Not explained yet.
{ "vid_vsync", "1" },
/* ------------------------------------------------------------------------- */
// ! VID_GASWITCH
// ? Set to 0 (default) to disable MacOS graphics switching, or 1 to allow
// ? MacOS to switch between integral and dedicated graphics.
{ "vid_gaswitch", "0" },
/* ------------------------------------------------------------------------- */
// ! VID_WIREFRAME
// ? Not explained yet.
{ "vid_wireframe", "0" },
/* ------------------------------------------------------------------------- */
// ! VID_ORWIDTH
// ? Not explained yet.
{ "vid_orwidth", "640" },
/* ------------------------------------------------------------------------- */
// ! VID_ORHEIGHT
// ? Not explained yet.
{ "vid_orheight", "480" },
/* ------------------------------------------------------------------------- */
// ! VID_ORASPMIN
// ? Not explained yet.
{ "vid_oraspmin", "1.25" },
/* ------------------------------------------------------------------------- */
// ! VID_ORASPMAX
// ? Not explained yet.
{ "vid_oraspmax", "1.777778" },
/* ------------------------------------------------------------------------- */
// ! VID_RFBO
// ? Not explained yet.
{ "vid_rfbo", "2" },
/* ------------------------------------------------------------------------- */
// ! VID_RFLOATS
// ? Not explained yet.
{ "vid_rfloats", "10000" },
/* ------------------------------------------------------------------------- */
// ! VID_RCMDS
// ? Not explained yet.
{ "vid_rcmds", "1000" },
/* ------------------------------------------------------------------------- */
// ! VID_RDTEX
// ? Not explained yet.
{ "vid_rdtex", "10" },
/* ------------------------------------------------------------------------- */
// ! VID_RDFBO
// ? Not explained yet.
{ "vid_rdfbo", "10" },
/* ------------------------------------------------------------------------- */
// ! VID_SSTYPE
// ? Not explained yet.
{ "vid_sstype", "2" },
/* ------------------------------------------------------------------------- */
// ! VID_SUBPIXROUND
// ? Not explained yet.
{ "vid_subpixround", "0" },
/* ------------------------------------------------------------------------- */
// ! VID_QSHADER
// ? Not explained yet.
{ "vid_qshader", "3" },
/* ------------------------------------------------------------------------- */
// ! VID_QLINE
// ? Not explained yet.
{ "vid_qline", "0" },
/* ------------------------------------------------------------------------- */
// ! VID_QPOLYGON
// ? Not explained yet.
{ "vid_qpolygon", "0" },
/* ------------------------------------------------------------------------- */
// ! VID_QCOMPRESS
// ? Not explained yet.
{ "vid_qcompress", "3" },
/* ------------------------------------------------------------------------- */
// ! WIN_ASPECT
// ? Force the window to have an aspect ratio. Specify as a floating point
// ? number. i.e. 16.9 for 16:9 or 4.3 for 4:3, etc. or 0 for numeric and/or
// ? denominator for freedom for the end user to resize at will.
{ "win_aspect", "0" },
/* ------------------------------------------------------------------------- */
// ! WIN_BORDER
// ? Not explained yet.
{ "win_border", "1" },
/* ------------------------------------------------------------------------- */
// ! WIN_CLOSEABLE
// ? Not explained yet.
{ "win_closeable", "1" },
/* ------------------------------------------------------------------------- */
// ! WIN_FLOATING
// ? Not explained yet.
{ "win_floating", "0" },
/* ------------------------------------------------------------------------- */
// ! WIN_FOCUSED
// ? Not explained yet.
{ "win_focused", "1" },
/* ------------------------------------------------------------------------- */
// ! WIN_HEIGHT
// ? Not explained yet.
{ "win_height", "0" },
/* ------------------------------------------------------------------------- */
// ! WIN_HEIGHTMIN
// ? Not explained yet.
{ "win_heightmin", "0" },
/* ------------------------------------------------------------------------- */
// ! WIN_MAXIMISED
// ? Specify 1 to have the window automatically maximised on creation or 0
// ? to not. The default value is 0.
{ "win_maximised", "0" },
/* ------------------------------------------------------------------------- */
// ! WIN_MINIMISEAUTO
// ? Specify 1 to have the window automatically minimise when it is not active
// ? or 0 to not. The default value is 1.
{ "win_minimiseauto", "1" },
/* ------------------------------------------------------------------------- */
// ! WIN_POSX
// ? Specify the default X position of the window. -2 means the centre of the
// ? screen and -1 means the default position. Any other value is the actual
// ? physical position on the screen. The default value is -2.
{ "win_posx", "-2" },
/* ------------------------------------------------------------------------- */
// ! WIN_POSY
// ? Specify the default Y position of the window. -2 means the centre of the
// ? screen and -1 means the default position. Any other value is the actual
// ? physical position on the screen. The default value is -2.
{ "win_posy", "-2" },
/* ------------------------------------------------------------------------- */
// ! WIN_SIZABLE
// ? Specify 1 if the window is allowed to be resized or 0 if not. The default
// ? value is 1.
{ "win_sizable", "1" },
/* ------------------------------------------------------------------------- */
// ! WIN_THREAD
// ? Set to 1 to allow the window to be managed in it's own thread for optimum
// ? performance. Set to 0 to have it share with the engine main thread. This
// ? variable is defaulted to 0 on anything but Windows due to what appears to
// ? be upstream issues on both Wayland and MacOS. This can only be changed at
// ? the in the application configuration file and not saved to the persistence
// ? database.
#if defined(WINDOWS)
{ "win_thread", "1" },
#else
{ "win_thread", "0" },                 // Fix temporary bug in GLFW
#endif
/* ------------------------------------------------------------------------- */
// ! WIN_WIDTH
// ? Sets the initial width of the window. This value is saved to the
// ? persistence database when changed.
{ "win_width", "0" },
/* ------------------------------------------------------------------------- */
// ! WIN_WIDTHMIN
// ? Sets the minimum width of the window. This is only changable at the
// ? application configuration file and is not saved to persistence database.
{ "win_widthmin", "0" },
/* ========================================================================= */
/* ######################################################################### */
/* ## LOGGING CVARS                                                       ## */
/* ######################################################################### */
/* ------------------------------------------------------------------------- */
// ! LOG_CREDITS
// ? Specifies to include credits in the log at startup.
{ "log_credits", "0" },
/* ------------------------------------------------------------------------- */
// ! LOG_DYLIBS
// ? Specifies to include shared libraries in the log at startup.
{ "log_dylibs", "0" },
/* ------------------------------------------------------------------------- */
} };                                   // End of cvar defaults list
/* ========================================================================= */
};                                     // End of module namespace
/* == EoF =========================================================== EoF == */

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
/* -- Gui mode flags ------------------------------------------------------- */
BUILD_FLAGS(Core,
  /* ----------------------------------------------------------------------- */
  // No flags?                         Want text mode console?
  CF_NOTHING             {0x00000000}, CF_TERMINAL            {0x00000001},
  // Want audio sub-system?            Want opengl window?
  CF_AUDIO               {0x00000002}, CF_VIDEO               {0x00000004},
  /* ----------------------------------------------------------------------- */
  CF_AUDIOVIDEO{ CF_AUDIO|CF_VIDEO },
  CF_MASK{ CF_TERMINAL|CF_AUDIOVIDEO }
);/* -- CVar flags --------------------------------------------------------- */
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
  APP_CONFIG,       APP_AUTHOR,          APP_SHORTNAME,       APP_HOMEDIR,
  SQL_DB,           SQL_ERASEEMPTY,      SQL_TEMPSTORE,       SQL_SYNCHRONOUS,
  SQL_JOURNALMODE,  SQL_AUTOVACUUM,      SQL_FOREIGNKEYS,     SQL_INCVACUUM,
  SQL_DEFAULTS,     SQL_LOADCONFIG,      APP_CFLAGS,          LOG_LINES,
  LOG_FILE,         APP_LONGNAME,        APP_CLEARMUTEX,      ERR_INSTANCE,
  /* -- Object cvars ------------------------------------------------------- */
  OBJ_CLIPMAX,      OBJ_CMDMAX,          OBJ_CVARMAX,         OBJ_CVARIMAX,
  OBJ_ARCHIVEMAX,   OBJ_ASSETMAX,        OBJ_BINMAX,          OBJ_CURSORMAX,
  OBJ_FBOMAX,       OBJ_FONTMAX,         OBJ_FILEMAX,         OBJ_FTFMAX,
  OBJ_FUNCMAX,      OBJ_IMGMAX,          OBJ_IMGFMTMAX,       OBJ_JSONMAX,
  OBJ_MASKMAX,      OBJ_PCMMAX,          OBJ_PCMFMTMAX,       OBJ_SAMPLEMAX,
  OBJ_SHADERMAX,    OBJ_SOCKETMAX,       OBJ_SOURCEMAX,       OBJ_STATMAX,
  OBJ_STREAMMAX,    OBJ_TEXTUREMAX,      OBJ_THREADMAX,       OBJ_VIDEOMAX,
  /* -- Base cvars --------------------------------------------------------- */
  APP_DESCRIPTION,  APP_VERSION,         APP_ICON,            APP_COPYRIGHT,
  APP_WEBSITE,      APP_TICKRATE,        APP_DELAY,           APP_TITLE,
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
  FMV_IOBUFFER,     FMV_MAXDRIFT,
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
  WIN_FOCUSED,      WIN_HEIGHT,          WIN_HEIGHTMAX,       WIN_HEIGHTMIN,
  WIN_MAXIMISED,    WIN_MINIMISEAUTO,    WIN_POSX,            WIN_POSY,
  WIN_SIZABLE,      WIN_THREAD,          WIN_WIDTH,           WIN_WIDTHMAX,
  WIN_WIDTHMIN,
  /* -- Logging cvars ------------------------------------------------------ */
  LOG_CREDITS,      LOG_DYLIBS,
  /* -- Other -------------------------------------------------------------- */
  MAX_CVAR                             // Maximum cvars. DO NOT (RE)MOVE!
};/* ----------------------------------------------------------------------- */
struct ItemStatic                      // Start of CVar static struct
{ /* ----------------------------------------------------------------------- */
  const CoreFlagsConst cfRequired;     // Required core flags
  const string         &strVar;        // Variable name
  const string         &strValue;      // Variable default value
  const CbFunc          cbTrigger;     // Callback trigger event
  const CVarFlagsConst &cFlags;        // Variable flags
};/* ----------------------------------------------------------------------- */
typedef array<const ItemStatic, MAX_CVAR> ItemStaticList;
/* ========================================================================= */
};                                     // End of module namespace
/* == EoF =========================================================== EoF == */

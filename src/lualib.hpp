/* == LUALIB.HPP =========================================================== */
/* ######################################################################### */
/* ## MS-ENGINE              Copyright (c) MS-Design, All Rights Reserved ## */
/* ######################################################################### */
/* ## This file defines the complete LUA API that interfaces with the     ## */
/* ## game engine. The guest will be able to use this functions when      ## */
/* ## writing scripts for their software.                                 ## */
/* ##                                                                     ## */
/* ## To define a function, begin with use of the llfuncbegin/end macros  ## */
/* ## to use multiple lines of code and llfunc(ex) to define a function   ## */
/* ## in a single line. The 'ex' signifies returning number of params.    ## */
/* ######################################################################### */
/* ------------------------------------------------------------------------- */
#pragma once                           // Only one incursion allowed
/* -- Module namespace ----------------------------------------------------- */
namespace IfLuaLib {                   // Keep declarations neatly categorised
/* -- Includes ------------------------------------------------------------- */
using namespace IfLuaUtil;             // Using luautil interface
/* == Macros to simplify param checking ==================================== */
#define LCPUSHBOOL(a)                  PushBoolean(lS, a)
#define LCPUSHNUM(a)                   PushNumber(lS, a)
#define LCPUSHINT(a)                   PushInteger(lS, a)
#define LCPUSHSTR(a)                   PushString(lS, a)
#define LCPUSHLSTR(a,l)                PushLString(lS, a, l)
#define LCPUSHXSTR(a)                  PushCppString(lS, a)
/* ------------------------------------------------------------------------- */
#define LCTOTABLE(...)                 ToTable(lS, __VA_ARGS__)
#define LCGETFLAGS(t,a,l,ge,n)         GetFlags<t>(lS, a, l, ge, n)
#define LCGETINTLGE(t,a,l,ge,n)        GetIntLGE<t>(lS, a, l, ge, n)
#define LCGETINTLG(t,a,l,g,n)          GetIntLG<t>(lS, a, l, g, n)
#define LCGETINT(t,a,n)                GetInt<t>(lS, a, n)
#define LCGETINTNUM(t,a,n)             GetIntOrNum<t>(lS, a, n)
#define LCGETNUMLGE(t,a,l,ge,n)        GetNumLGE<t>(lS, a, l, ge, n)
#define LCGETNUMLG(t,a,l,g,n)          GetNumLG<t>(lS, a, l, g, n)
#define LCGETNUM(t,a,n)                GetNum<t>(lS, a, n)
#define LCGETBOOL(a,n)                 GetBool(lS, a, n)
#define LCGETSTRING(t,a,n)             GetString<t>(lS, a, n)
#define LCGETCPPSTRING(a,n)            GetCppString(lS, a, n)
#define LCGETCPPSTRINGNE(a,n)          GetCppStringNE(lS, a, n)
#define LCGETCPPFILE(a,n)              GetCppFileName(lS, a, n)
#define LCCHECKFUNC(a,n)               CheckFunction(lS, a, n)
#define LCCHECKPARAMS(c)               CheckParams(lS, c)
#define LCCHECKMAINSTATE()             cLua->StateAssert(lS)
#define LCSETEVENTCBEX(c,p)            cLua->SetLuaRef(lS, c, p)
#define LCSETEVENTCB(c)                LCSETEVENTCBEX(c, 1)
/* == Macros to simplify LUA class creation and destruction ================ */
#define LCCLASSCREATEPTR(t,f)          ClassCreatePtr<t>(lS, #t, f)
#define LCCLASSCREATE(t)               ClassCreate<t>(lS, #t)
#define LCCLASSDESTROY(p,t)            ClassDestroy<t>(lS, p, #t)
#define LCGETPTR(p,t)                  GetPtr<t>(lS, p, #t)
/* == Macros to simplify try/catch on each lualib function ================= */
#define LLFUNCBEGIN(n)                 static int Cb ## n(lua_State*const lS) {
#define LLFUNCENDEX(p)                 UNUSED_VARIABLE(lS); return p; }
#define LLFUNCBEGINEX(n,p)             LLFUNCBEGIN(n) LCCHECKPARAMS(p);
#define LLFUNCBEGINTEMPLATE(n)         template<typename T>LLFUNCBEGIN(n)
#define LLFUNCEND                      LLFUNCENDEX(0)
/* == One-line function declaration ======================================== */
#define LLNAMESPACEBEGIN(n)            namespace Ns ## n {
#define LLNAMESPACEEND                 };
#define LLFUNC(n,f)                    LLFUNCBEGIN(n) f; LLFUNCEND
#define LLFUNCEX(n,x,f)                LLFUNCBEGIN(n) f; LLFUNCENDEX(x)
#define LLFUNCTEMPLATE(n,f)            LLFUNCBEGINTEMPLATE(n) f; LLFUNCEND
#define LLFUNCTEMPLATEEX(n,x,f)        LLFUNCBEGINTEMPLATE(n) f; LLFUNCENDEX(x)
/* == Macros to simplify definition of lualib reg func structures ========== */
#define ARRAYLEN(a)             (sizeof((a)) / sizeof((a)[0]) - 1)
#define LLRSBEGIN               static const luaL_Reg llStatics[]{
#define LLRSMFBEGIN             static const luaL_Reg llMethods[]{
#define LLRSCONSTBEGIN          static const LuaTable llConsts[]{
#define LLRSCONST(n)            { #n, llConsts ## n, ARRAYLEN(llConsts ## n) }
#define LLRSCONSTEND            { nullptr, nullptr, 0 } };
#define LLRSKTBEGIN(n)          static const LuaKeyInt llConsts ## n[]{
#define LLRSKTITEMEX2(k,v)      { k, static_cast<lua_Integer>(v) }
#define LLRSKTITEMEX(p,n,u)     LLRSKTITEMEX2(#n, p ## n ## u)
#define LLRSKTITEM(p,n)         LLRSKTITEMEX2(#n, p ## n)
#define LLRSKTEND               { nullptr, 0 } };
#define LLRSFUNC(n)             { #n, Cb ## n }
#define LLRSFUNCEX(n,f)         { #n, Cb ## f }
#define LLRSEND                 { nullptr, nullptr } };
/* == Includes ============================================================= */
#include "llarchive.hpp"               // Archive namespace and methods
#include "llasset.hpp"                 // Asset namespace and methods
#include "llaudio.hpp"                 // Audio namespace and methods
#include "llbin.hpp"                   // Bin namespace and methods
#include "llconsole.hpp"               // Console namespace and methods
#include "llcore.hpp"                  // Core namespace and methods
#include "llcredit.hpp"                // Credit namespace and methods
#include "llcrypt.hpp"                 // Crypt namespace and methods
#include "llcvars.hpp"                 // Cvars namespace and methods
#include "llcursor.hpp"                // Cursor namespace and methods
#include "lldisplay.hpp"               // Display namespace and methods
#include "llfbo.hpp"                   // Fbo namespace and methods
#include "llfile.hpp"                  // File namespace and methods
#include "llfont.hpp"                  // Font namespace and methods
#include "llftf.hpp"                   // Ftf namespace and methods
#include "llimage.hpp"                 // Image namespace and methods
#include "llinfo.hpp"                  // Info namespace and methods
#include "llinput.hpp"                 // Input namespace and methods
#include "lljson.hpp"                  // Json namespace and methods
#include "llmask.hpp"                  // Mask namespace and methods
#include "llpcm.hpp"                   // Pcm namespace and methods
#include "llsource.hpp"                // Source namespace and methods
#include "llsample.hpp"                // Sample namespace and methods
#include "llsocket.hpp"                // Socket namespace and methods
#include "llsql.hpp"                   // Sql namespace and methods
#include "llstat.hpp"                  // Stat namespace and methods
#include "llstream.hpp"                // Stream namespace and methods
#include "lltexture.hpp"               // Texture namespace and methods
#include "llutil.hpp"                  // Util namespace and methods
#include "llvideo.hpp"                 // Video namespace and methods
/* ========================================================================= */
/* ######################################################################### */
/* ## CLEAN-UP                                                            ## */
/* ######################################################################### */
/* ------------------------------------------------------------------------- */
#undef LLRSEND
#undef LLRSFUNCEX
#undef LLRSFUNC
#undef LLRSKTBEGIN
#undef LLRSKTITEMEX2
#undef LLRSKTITEMEX
#undef LLRSKTITEM
#undef LLRSCONSTEND
#undef LLRSCONST
#undef LLRSCONSTBEGIN
#undef LLRSMFBEGIN
#undef LLRSBEGIN
/* ------------------------------------------------------------------------- */
#undef LLNAMESPACEEND
#undef LLNAMESPACEBEGIN
#undef LLFUNCTEMPLATE
#undef LLFUNCTEMPLATEEX
#undef LLFUNCNOTHROW
#undef LLFUNCEX
#undef LLFUNC
#undef LLFUNCEND
#undef LLFUNCENDEX
#undef LLFUNCENDNOTHROW
#undef LLFUNCENDNOTHROWEX
#undef LLFUNCBEGINNOTHROW
#undef LLFUNCBEGINNOTHROWEX
#undef LLFUNCBEGINTEMPLATE
#undef LLFUNCBEGIN
#undef LLFUNCBEGINEX
/* ------------------------------------------------------------------------- */
#undef LCTOTABLE
#undef LCPUSHBOOL
#undef LCPUSHNUM
#undef LCPUSHINT
#undef LCPUSHSTR
#undef LCGETPTR
#undef LCCLASSCREATEPTR
#undef LCCLASSCREATE
#undef LCCLASSDESTROY
#undef LCGETINTNUM
#undef LCGETINTLGE
#undef LCGETINTLG
#undef LCGETINT
#undef LCGETNUMLGE
#undef LCGETNUMLG
#undef LCGETNUM
#undef LCCHECKFUNC
#undef LCCHECKPARAMS
#undef LCCHECKMAINSTATE
#undef LCSETEVENTCB
#undef LCGETSTRING
#undef LCGETBOOL
#undef LCGETCPPFILE
#undef LCGETCPPSTRINGNE
#undef LCGETCPPSTRING
#undef LCSETEVENTCBEX
#undef LCPUSHCPPSTR
#undef LCPUSHLSTR
#undef LCPUSHXSTR
/* ========================================================================= */
/* ######################################################################### */
/* ## LUA GLOBAL CLASS STRUCTURE                                          ## */
/* ######################################################################### */
/* -- Key ------------------------------------------------------------------ */
/* v (variable) = The fully qualified name of the array to use.              */
/* n (name)     = The name of the namespace.                                 */
/* t (type)     = The variable type (Statics, Methods or Consts).            */
/* l (level)    = The minimum gui level. See cvardef.hpp->GuiMode.           */
/* m (methods)  = Method function list. LLMETHODS(name) or LLNOMETHODS.      */
/* c (consts)   = Const key/value list. LLCONSTS(name) or LLNOCONSTS.        */
/* -- Helps build the pointer to the list and the length of the list ------- */
#define LLPTRLENEX(v)   v, ARRAYLEN(v)
#define LLPTRLEN(n,t)   LLPTRLENEX(Ns ## n::ll ## t)
/* -- Helps define the list of static, methods and consts ------------------ */
#define LLSTATICS(n)    LLPTRLEN(n, Statics)
#define LLMETHODS(n)    LLPTRLEN(n, Methods), \
  static_cast<lua_CFunction>(Ns ## n::CbDestroy)
#define LLNOMETHODS()   nullptr, 0, nullptr
#define LLCONSTS(n)     LLPTRLEN(n, Consts)
#define LLNOCONSTS()    nullptr, 0
#define LLITEM(n,l,m,c) { #n, GM_ ## l, LLSTATICS(n), m, c }
/* -- Helps define a namespace (use below) --------------------------------- */
#define LLSXX(n,l) LLITEM(n, l, LLNOMETHODS(), LLNOCONSTS()) // Meth:N,Const:N
#define LLSMX(n,l) LLITEM(n, l, LLMETHODS(n),  LLNOCONSTS()) // Meth:Y,Const:N
#define LLSXC(n,l) LLITEM(n, l, LLNOMETHODS(), LLCONSTS(n))  // Meth:N,Const:Y
#define LLSMC(n,l) LLITEM(n, l, LLMETHODS(n),  LLCONSTS(n))  // Meth:Y,Const:Y
/* -- Define the ms-engine api list loaded at startup ---------------------- */
const LuaLibStatic luaLibList[] =
{ /* -- Use the above macros to define namespaces -------------------------- */
  LLSMX(Archive, TEXT_NOAUDIO),        LLSMC(Asset,   TEXT_NOAUDIO),
  LLSXX(Audio,   TEXT_AUDIO),          LLSMX(Bin,     TEXT_NOAUDIO),
  LLSXC(Core,    TEXT_NOAUDIO),        LLSXX(Credit,  TEXT_NOAUDIO),
  LLSXC(CVars,   TEXT_NOAUDIO),        LLSXC(Console, TEXT_NOAUDIO),
  LLSXX(Crypt,   TEXT_NOAUDIO),        LLSMC(Cursor,  GRAPHICS),
  LLSXX(Display, GRAPHICS),            LLSMC(Fbo,     GRAPHICS),
  LLSMC(File,    TEXT_NOAUDIO),        LLSMC(Font,    GRAPHICS),
  LLSMX(Ftf,     TEXT_NOAUDIO),        LLSMC(Image,   TEXT_NOAUDIO),
  LLSXX(Info,    TEXT_NOAUDIO),        LLSXC(Input,   GRAPHICS),
  LLSMX(Json,    TEXT_NOAUDIO),        LLSMX(Mask,    TEXT_NOAUDIO),
  LLSMC(Pcm,     TEXT_NOAUDIO),        LLSMX(Sample,  TEXT_AUDIO),
  LLSMX(Stat,    TEXT_NOAUDIO),        LLSMC(Socket,  TEXT_NOAUDIO),
  LLSMX(Source,  TEXT_AUDIO),          LLSXC(Sql,     TEXT_NOAUDIO),
  LLSMC(Stream,  TEXT_AUDIO),          LLSMX(Texture, GRAPHICS),
  LLSXX(Util,    TEXT_NOAUDIO),        LLSMC(Video,   GRAPHICS),
  /* -- Last item, do not delete ------------------------------------------- */
  { nullptr, GM_HIGHEST, LLNOCONSTS(), LLNOMETHODS(), LLNOCONSTS() }
};/* ----------------------------------------------------------------------- */
/* -- Done with these macros ----------------------------------------------- */
#undef LLSXC
#undef LLSMC
#undef LLSMX
#undef LLSXX
#undef LLITEM
#undef LLNOMETHODS
#undef LLNOCONSTS
#undef LLCONSTS
#undef LLMETHODS
#undef LLSTATICS
#undef LLPTRLEN
#undef LLPTRLENEX
#undef ARRAYLEN
/* ========================================================================= */
};                                     // End of interface
/* == EoF =========================================================== EoF == */

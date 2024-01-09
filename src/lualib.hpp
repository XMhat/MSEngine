/* == LUALIB.HPP =========================================================== **
** ######################################################################### **
** ## MS-ENGINE              Copyright (c) MS-Design, All Rights Reserved ## **
** ######################################################################### **
** ## This file defines the complete LUA API that interfaces with the     ## **
** ## game engine. The guest will be able to use this functions when      ## **
** ## writing scripts for their software.                                 ## **
** ##                                                                     ## **
** ## To define a function, begin with use of the llfuncbegin/end macros  ## **
** ## to use multiple lines of code and llfunc(ex) to define a function   ## **
** ## in a single line. The 'ex' signifies returning number of params.    ## **
** ######################################################################### **
** ------------------------------------------------------------------------- */
#pragma once                           // Only one incursion allowed
/* ------------------------------------------------------------------------- */
namespace ILuaLib {                    // Start of private module namespace
/* -- Dependencies --------------------------------------------------------- */
using namespace ICVarDef::P;           using namespace ILuaUtil::P;
/* ------------------------------------------------------------------------- */
namespace P {                          // Start of public module namespace
/* -- Macros to simplify param checking ------------------------------------ */
#define LCPUSHVAR(...)                 LuaUtilPushVar(lS, __VA_ARGS__)
#define LCPUSHLSTR(a,l)                LuaUtilPushLStr(lS, a, l)
/* ------------------------------------------------------------------------- */
#define LCTOTABLE(...)                 LuaUtilToTable(lS, __VA_ARGS__)
#define LCGETFLAGS(t,a,m,n)            LuaUtilGetFlags<t>(lS, a, m, n)
#define LCGETINTLEG(t,a,l,ge,n)        LuaUtilGetIntLEG<t>(lS, a, l, ge, n)
#define LCGETINTLGE(t,a,l,ge,n)        LuaUtilGetIntLGE<t>(lS, a, l, ge, n)
#define LCGETINTLG(t,a,l,g,n)          LuaUtilGetIntLG<t>(lS, a, l, g, n)
#define LCGETINT(t,a,n)                LuaUtilGetInt<t>(lS, a, n)
#define LCGETNUMLGE(t,a,l,ge,n)        LuaUtilGetNumLGE<t>(lS, a, l, ge, n)
#define LCGETNUMLG(t,a,l,g,n)          LuaUtilGetNumLG<t>(lS, a, l, g, n)
#define LCGETNUM(t,a,n)                LuaUtilGetNum<t>(lS, a, n)
#define LCGETBOOL(a,n)                 LuaUtilGetBool(lS, a, n)
#define LCGETSTRING(t,a,n)             LuaUtilGetStr<t>(lS, a, n)
#define LCGETCPPSTRING(a,n)            LuaUtilGetCppStr(lS, a, n)
#define LCGETCPPSTRINGNE(a,n)          LuaUtilGetCppStrNE(lS, a, n)
#define LCGETCPPFILE(a,n)              LuaUtilGetCppFile(lS, a, n)
#define LCCHECKFUNC(a,n)               LuaUtilCheckFunc(lS, a, n)
#define LCCHECKPARAMS(c)               LuaUtilCheckParams(lS, c)
#define LCCHECKMAINSTATE()             cLua->StateAssert(lS)
#define LCSETEVENTCBEX(c,p)            cLua->SetLuaRef(lS, c, p)
#define LCSETEVENTCB(c)                LCSETEVENTCBEX(c, 1)
/* -- Macros to simplify LUA class creation and destruction ---------------- */
#define LCCLASSCREATEPTR(t,f)          LuaUtilClassCreatePtr<t>(lS, #t, f)
#define LCCLASSCREATE(t)               LuaUtilClassCreate<t>(lS, #t)
#define LCCLASSDESTROY(p,t)            LuaUtilClassDestroy<t>(lS, p, #t)
#define LCGETPTR(p,t)                  LuaUtilGetPtr<t>(lS, p, #t)
#define LCGETULPTR(p,t)                LuaUtilGetUnlockedPtr<t>(lS, p, #t)
/* -- Macros to simplify try/catch on each lualib function ----------------- */
#define LLFUNCBEGIN(n)                 static int Cb ## n(lua_State*const lS) {
#define LLFUNCENDEX(p)                 UNUSED_VARIABLE(lS); return p; }
#define LLFUNCBEGINEX(n,p)             LLFUNCBEGIN(n) LCCHECKPARAMS(p);
#define LLFUNCBEGINTEMPLATE(n)         template<typename T>LLFUNCBEGIN(n)
#define LLFUNCEND                      LLFUNCENDEX(0)
/* -- One-line function declaration ---------------------------------------- */
#define LLFUNC(n,f)                    LLFUNCBEGIN(n) f; LLFUNCEND
#define LLFUNCEX(n,x,f)                LLFUNCBEGIN(n) f; LLFUNCENDEX(x)
#define LLFUNCTEMPLATE(n,f)            LLFUNCBEGINTEMPLATE(n) f; LLFUNCEND
#define LLFUNCTEMPLATEEX(n,x,f)        LLFUNCBEGINTEMPLATE(n) f; LLFUNCENDEX(x)
/* -- Get length of an array ignoring the final NULL entry ----------------- */
template<typename IntType=int, typename AnyType, IntType itSize>
  constexpr int ArrayLen(AnyType (&)[itSize]) { return itSize - 1; };
/* -- Macros to simplify definition of lualib reg func structures ---------- */
#define LLRSBEGIN           static const luaL_Reg llStatics[]{
#define LLRSMFBEGIN         static const luaL_Reg llMethods[]{
#define LLRSCONSTBEGIN      static const LuaTable llConsts[]{
#define LLRSCONST(n)        { #n, llConsts ## n, ArrayLen(llConsts ## n) }
#define LLRSCONSTEND        { nullptr, nullptr, 0 } };
#define LLRSKTBEGIN(n)      static const LuaKeyInt llConsts ## n[]{
#define LLRSKTITEMEX2(k,v)  { k, static_cast<lua_Integer>(v) }
#define LLRSKTITEMEX(p,n,u) LLRSKTITEMEX2(#n, p ## n ## u)
#define LLRSKTITEM(p,n)     LLRSKTITEMEX2(#n, p ## n)
#define LLRSKTEND           { nullptr, 0 } };
#define LLRSFUNC(n)         { #n, Cb ## n }
#define LLRSFUNCEX(n,f)     { #n, Cb ## f }
#define LLRSEND             { nullptr, nullptr } };
/* -- Dependencies --------------------------------------------------------- */
#include "llarchive.hpp"               // Archive namespace and methods
#include "llasset.hpp"                 // Asset namespace and methods
#include "llaudio.hpp"                 // Audio namespace and methods
#include "llbin.hpp"                   // Bin namespace and methods
#include "llclip.hpp"                  // Clipboard namespace and methods
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
#include "llpalette.hpp"               // Palette namespace and methods
#include "llutil.hpp"                  // Util namespace and methods
#include "llvideo.hpp"                 // Video namespace and methods
/* -- Done with these macros ----------------------------------------------- */
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
#undef LCGETULPTR
#undef LCGETPTR
#undef LCCLASSCREATEPTR
#undef LCCLASSCREATE
#undef LCCLASSDESTROY
#undef LCGETINTLEG
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
#undef LCPUSHVAR
#undef LCPUSHLSTR
/* ========================================================================= **
** ######################################################################### **
** ## LUA GLOBAL CLASS STRUCTURE                                          ## **
** ######################################################################### **
** -- Key ------------------------------------------------------------------ **
** v (variable) = The fully qualified name of the array to use.              **
** n (name)     = The name of the namespace.                                 **
** t (type)     = The variable type (Statics, Methods or Consts).            **
** l (level)    = The required core flags. See cvardef.hpp->CoreFlags.       **
** m (methods)  = Method function list. LLMETHODS(name) or LLNOMETHODS.      **
** c (consts)   = Const key/value list. LLCONSTS(name) or LLNOCONSTS.        **
** -- Helps build the pointer to the list and the length of the list ------- */
#define LLPTRLENEX(v)   v, ArrayLen(v)
#define LLPTRLEN(n,t)   LLPTRLENEX(LL ## n::ll ## t)
/* -- Helps define the list of static, methods and consts ------------------ */
#define LLSTATICS(n)    LLPTRLEN(n, Statics)
#define LLMETHODS(n)    LLPTRLEN(n, Methods), \
  static_cast<lua_CFunction>(LL ## n::CbDestroy)
#define LLNOMETHODS()   nullptr, 0, nullptr
#define LLCONSTS(n)     LLPTRLEN(n, Consts)
#define LLNOCONSTS()    nullptr, 0
#define LLITEM(n,l,m,c) { #n, l, LLSTATICS(n), m, c }
/* -- Helps define a namespace (use below) --------------------------------- */
#define LLSXX(n,l) LLITEM(n, l, LLNOMETHODS(), LLNOCONSTS()) // Meth:N,Const:N
#define LLSMX(n,l) LLITEM(n, l, LLMETHODS(n),  LLNOCONSTS()) // Meth:Y,Const:N
#define LLSXC(n,l) LLITEM(n, l, LLNOMETHODS(), LLCONSTS(n))  // Meth:N,Const:Y
#define LLSMC(n,l) LLITEM(n, l, LLMETHODS(n),  LLCONSTS(n))  // Meth:Y,Const:Y
/* -- Define the ms-engine api list loaded at startup ---------------------- */
const LuaLibStatic luaLibList[] =
{ /* -- Use the above macros to define namespaces -------------------------- */
  LLSMX(Archive, CF_NOTHING),          LLSMC(Asset,   CF_NOTHING),
  LLSXX(Audio,   CF_AUDIO),            LLSMX(Bin,     CF_NOTHING),
  LLSMX(Clip,    CF_VIDEO),            LLSXC(Core,    CF_NOTHING),
  LLSXX(Credit,  CF_NOTHING),          LLSXC(CVars,   CF_NOTHING),
  LLSXC(Console, CF_NOTHING),          LLSXX(Crypt,   CF_NOTHING),
  LLSMC(Cursor,  CF_VIDEO),            LLSXC(Display, CF_VIDEO),
  LLSMC(Fbo,     CF_VIDEO),            LLSMC(File,    CF_NOTHING),
  LLSMC(Font,    CF_VIDEO),            LLSMX(Ftf,     CF_NOTHING),
  LLSMC(Image,   CF_NOTHING),          LLSXX(Info,    CF_NOTHING),
  LLSXC(Input,   CF_VIDEO),            LLSMX(Json,    CF_NOTHING),
  LLSMX(Mask,    CF_NOTHING),          LLSMX(Palette, CF_VIDEO),
  LLSMC(Pcm,     CF_NOTHING),          LLSMX(Sample,  CF_AUDIO),
  LLSMX(Stat,    CF_NOTHING),          LLSMC(Socket,  CF_NOTHING),
  LLSMX(Source,  CF_AUDIO),            LLSXC(Sql,     CF_NOTHING),
  LLSMC(Stream,  CF_AUDIO),            LLSMX(Texture, CF_VIDEO),
  LLSXX(Util,    CF_NOTHING),          LLSMC(Video,   CF_AUDIOVIDEO),
  /* -- Last item, do not delete ------------------------------------------- */
  { nullptr, CF_NOTHING, LLNOCONSTS(), LLNOMETHODS(), LLNOCONSTS() }
};/* -- Done with these macros --------------------------------------------- */
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
/* ------------------------------------------------------------------------- */
}                                      // End of public module namespace
/* ------------------------------------------------------------------------- */
}                                      // End of private module namespace
/* == EoF =========================================================== EoF == */

/* == LLCREDIT.HPP ========================================================= **
** ######################################################################### **
** ## MS-ENGINE              Copyright (c) MS-Design, All Rights Reserved ## **
** ######################################################################### **
** ## Defines the 'Credit' namespace and methods for the guest to use in  ## **
** ## Lua. This file is invoked by 'lualib.hpp'.                          ## **
** ######################################################################### **
** ------------------------------------------------------------------------- */
#pragma once                           // Only one incursion allowed
/* ========================================================================= **
** ######################################################################### **
** ========================================================================= */
// % Credit
/* ------------------------------------------------------------------------- */
// ! Allows access to engine credits information.
/* ========================================================================= */
namespace LLCredit {                   // Credit namespace
/* -- Dependencies --------------------------------------------------------- */
using namespace ICredit::P;            using namespace Common;
/* ========================================================================= **
** ######################################################################### **
** ## Credit common helper classes                                        ## **
** ######################################################################### **
** -- Read a credit id ----------------------------------------------------- */
struct AgCreditEnum : public AgIntegerLGE<CreditEnums> {
  explicit AgCreditEnum(lua_State*const lS, const int iArg) :
    AgIntegerLGE{lS, iArg, CL_FIRST, CL_MAX}{} };
/* ========================================================================= **
** ######################################################################### **
** ## Credit.* namespace functions                                        ## **
** ######################################################################### **
** ========================================================================= */
// $ Credit.Credit
// > Id:integer=The index of the license.
// < Name:string=Name of the credit.
// < Version:string=Version of the api.
// < Id:integer=License index.
// < Website:string=Website of the api.
// < Copyright:string=Copyright of the api.
// < Author:string=Author of the api.
// ? Shows the full credits information of the specified api index.
/* ------------------------------------------------------------------------- */
LLFUNC(Item, 4,
  const AgCreditEnum aCredit{lS, 1};
  const CreditLib &clItem = cCredits->CreditGetItem(aCredit);
  LuaUtilPushVar(lS, clItem.GetName(),     clItem.GetVersion(),
            clItem.IsCopyright(), clItem.GetAuthor()))
/* ========================================================================= */
// $ Credit.License
// > Id:integer=The index of the license.
// < Name:string=Name of the license.
// < Text:string=Full text file of the license.
// ? Shows the full license information of the specified index.
/* ------------------------------------------------------------------------- */
LLFUNC(License, 1,
  LuaUtilPushVar(lS, cCredits->CreditGetItemText(AgCreditEnum{lS, 1})))
/* ========================================================================= **
** ######################################################################### **
** ## Credit.* namespace functions structure                              ## **
** ######################################################################### **
** ------------------------------------------------------------------------- */
LLRSBEGIN                              // Credit.* namespace functions begin
  LLRSFUNC(Item), LLRSFUNC(License),
LLRSEND                                // Credit.* namespace functions end
/* ========================================================================= **
** ######################################################################### **
** ## Credit.* namespace constants                                        ## **
** ######################################################################### **
** ========================================================================= */
// @ Credit.Libs
// < Ids:table=The table of all library ids
// ? A table containing the ids of all the libraries supported.
/* ------------------------------------------------------------------------- */
LLRSKTBEGIN(Libs)                      // Beginning of supported library ids
  LLRSKTITEM(CL_,MSE),  LLRSKTITEM(CL_,FT),   LLRSKTITEM(CL_,GLFW),
  LLRSKTITEM(CL_,JPEG), LLRSKTITEM(CL_,GIF),  LLRSKTITEM(CL_,PNG),
  LLRSKTITEM(CL_,LUA),  LLRSKTITEM(CL_,LZMA), LLRSKTITEM(CL_,MP3),
#if !defined(WINDOWS)                  // Not using Windows?
  LLRSKTITEM(CL_,NCURSES),             // Id for NCurses credit data
#endif                                 // Not using windows
  LLRSKTITEM(CL_,OGG),  LLRSKTITEM(CL_,AL),  LLRSKTITEM(CL_,SSL),
  LLRSKTITEM(CL_,JSON), LLRSKTITEM(CL_,SQL), LLRSKTITEM(CL_,THEO),
  LLRSKTITEM(CL_,ZLIB), LLRSKTITEM(CL_,MAX),
LLRSKTEND                              // End of supported library ids
/* ========================================================================= **
** ######################################################################### **
** ## Credit.* namespace constants structure                              ## **
** ######################################################################### **
** ========================================================================= */
LLRSCONSTBEGIN                         // Credit.* namespace consts begin
  LLRSCONST(Libs),
LLRSCONSTEND                           // Credit.* namespace consts end
/* ========================================================================= */
}                                      // End of Credit namespace
/* == EoF =========================================================== EoF == */

/* == LLCVARS.HPP ========================================================== **
** ######################################################################### **
** ## MS-ENGINE              Copyright (c) MS-Design, All Rights Reserved ## **
** ######################################################################### **
** ## Defines the 'CVars' namespace and methods for the guest to use in   ## **
** ## Lua. This file is invoked by 'lualib.hpp'.                          ## **
** ######################################################################### **
** ------------------------------------------------------------------------- */
#pragma once                           // Only one incursion allowed
/* ========================================================================= **
** ######################################################################### **
** ========================================================================= */
// % CVars
/* ------------------------------------------------------------------------- */
// ! The cvars class allows the programmer to create their own cvars and to
// ! retrieve and modify their own or built-in CVars.
/* ========================================================================= */
namespace LLCVars {                    // CVars namespace
/* -- Dependencies --------------------------------------------------------- */
using namespace ICVar::P;              using namespace ICVarDef::P;
/* ========================================================================= **
** ######################################################################### **
** ## CVars.* namespace functions                                         ## **
** ######################################################################### **
** ========================================================================= */
// $ CVars.Get
// > Name:string=The engine cvar name.
// < Value:mixed=The engine cvar value.
// ? Retrieves the value of the specified cvar name. An exception is raised if
// ? the specified cvar does not exist.
/* ------------------------------------------------------------------------- */
LLFUNCEX(Get, 1,
  LCPUSHVAR(cCVars->LuaGetStrSafe(LCGETCPPSTRINGNE(1, "Variable"))));
/* ========================================================================= */
// $ CVars.Empty
// > Name:string=The engine cvar name.
// < IsEmpty:boolean=The specified cvar is empty.
// ? Checks if the specified cvar is empty. Perhaps a little faster than using
// ? CVars.Get which builds a new string, especially if the string is long.
/* ------------------------------------------------------------------------- */
LLFUNCEX(Empty, 1,
  LCPUSHVAR(cCVars->IsVarStrEmptySafe(LCGETCPPSTRINGNE(1, "Variable"))));
/* ========================================================================= */
// $ CVars.Set
// > Name:string=The engine cvar name.
// > Value:string=The new engine cvar value.
// ? Sets the new value of the specified engine cvar name. An exception is
// ? raised if any error occurds. See CVars.Result to see the possible
// ? results.
/* ------------------------------------------------------------------------- */
LLFUNCEX(Set, 1, LCPUSHVAR(cCVars->SetSafe(LCGETCPPSTRINGNE(1, "Variable"),
  LCGETCPPSTRING(2, "Value"), PUSR)));
/* ========================================================================= */
// $ CVars.Reset
// > Name:string=The engine cvar name.
// ? Resets the value of the cvar to the engine's built-in value.
/* ------------------------------------------------------------------------- */
LLFUNC(Reset, cCVars->ResetSafe(LCGETCPPSTRINGNE(1, "Variable"), PUSR));
/* ========================================================================= */
// $ CVars.Unregister
// > Name:string=The engine cvar name.
// ? Unregisters the specified cvar name created with CVars.Register().
/* ------------------------------------------------------------------------- */
LLFUNC(Unregister,
  cCVars->UnregisterLuaVarSafe(LCGETCPPSTRINGNE(1, "Variable")));
/* ========================================================================= */
// $ CVars.Register
// > Name:string=The engine cvar name.
// > Default:string=The default cvar value.
// > Flags:integer=The cvar flags.
// > Callback:function=The callback global function name.
// ? Registers the specified cvar name with the specified default value. The
// ? default value is ignored if there is already a saved value in the Sql
// ? cvars database. If the saved value is a corrupted encrypted value then
// ? the default is used. If the cvar already exists then an exception is
// ? thrown so make sure you check for it first if do not expect this call
// ? to garauntee success. Normally it is unless there is a memory allocation
// ? issue. You can register an unlimited amount of variables.
// ? See CVars.Flags to see available flags.
/* ------------------------------------------------------------------------- */
LLFUNC(Register, cCVars->RegisterLuaVar(lS));
/* ========================================================================= */
// $ CVars.Exists
// > Name:string=The engine cvar name.
// < Exists:boolean=Does the cvar exist?
// ? Returns TRUE if the cvar is registered or FALSE if not. Any attempt to
// ? read or write from invalid cvars will raise an exception, this function
// ? allows you to make sure the cvar exists before you peform that operation.
/* ------------------------------------------------------------------------- */
LLFUNCEX(Exists, 1,
  LCPUSHVAR(cCVars->VarExistsSafe(LCGETCPPSTRINGNE(1, "Identifier"))));
/* ========================================================================= */
// $ CVars.Save
// < Count:number=Number of items saved
// ? Commits all modified variables to database. Returns number of items
// ? written.
/* ------------------------------------------------------------------------- */
LLFUNCEX(Save, 1, LCPUSHVAR(cCVars->Save()));
/* ========================================================================= **
** ######################################################################### **
** ## CVars.* namespace functions structure                               ## **
** ######################################################################### **
** ------------------------------------------------------------------------- */
LLRSBEGIN                              // CVars.* namespace functions begin
  LLRSFUNC(Get),   LLRSFUNC(Empty), LLRSFUNC(Exists), LLRSFUNC(Register),
  LLRSFUNC(Reset), LLRSFUNC(Save),  LLRSFUNC(Set),    LLRSFUNC(Unregister),
LLRSEND                                // CVars.* namespace functions end
/* ========================================================================= **
** ######################################################################### **
** ## CVars.* namespace constants                                         ## **
** ######################################################################### **
** ========================================================================= */
// @ CVars.Flags
// < Data:table=A table of const string/int key pairs
// ? Returns all the cvar flags supported by the engine in the form of a
// ? string/int key pairs which can be used with the CVars.Register() command
/* ------------------------------------------------------------------------- */
LLRSKTBEGIN(Flags)                     // Beginning of cvar register flags
  LLRSKTITEM(T,STRING),                LLRSKTITEM(T,INTEGER),
  LLRSKTITEM(T,FLOAT),                 LLRSKTITEM(T,BOOLEAN),
  LLRSKTITEM(T,UINTEGER),              LLRSKTITEM(T,UFLOAT),
  LLRSKTITEM(T,INTEGERSAVE),           LLRSKTITEM(T,FLOATSAVE),
  LLRSKTITEM(T,UINTEGERSAVE),          LLRSKTITEM(T,UFLOATSAVE),
  LLRSKTITEM(T,STRINGSAVE),            LLRSKTITEM(T,BOOLEANSAVE),
  LLRSKTITEM(C,SAVEABLE),              LLRSKTITEM(C,PROTECTED),
  LLRSKTITEM(C,NOTEMPTY),              LLRSKTITEM(C,UNSIGNED),
  LLRSKTITEM(C,POW2),                  LLRSKTITEM(C,POW2Z),
  LLRSKTITEM(C,DEFLATE),               LLRSKTITEM(C,ALPHA),
  LLRSKTITEM(C,FILENAME),              LLRSKTITEM(C,NUMERIC),
  LLRSKTITEM(M,TRIM),                  LLRSKTITEM(O,SAVEFORCE),
LLRSKTEND                              // End of cvar register status flags
/* ========================================================================= */
// @ CVars.Result
// < Data:table=A table of const string/int key pairs
// ? Returns the possible values that can be returned with the CVars.Set()
// ? function.
/* ------------------------------------------------------------------------- */
LLRSKTBEGIN(Result)                    // Beginning of cvar result flags
  LLRSKTITEM(CVS_,OK),                 LLRSKTITEM(CVS_,OKNOTCHANGED),
LLRSKTEND                              // End of cvar result flags
/* ========================================================================= **
** ######################################################################### **
** ## CVars.* namespace constants structure                               ## **
** ######################################################################### **
** ========================================================================= */
LLRSCONSTBEGIN                         // CVars.* namespace consts begin
  LLRSCONST(Flags), LLRSCONST(Result),
LLRSCONSTEND                           // CVars.* namespace consts end
/* ========================================================================= */
}                                      // End of CVars namespace
/* == EoF =========================================================== EoF == */

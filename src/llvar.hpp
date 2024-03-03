/* == LLVAR.HPP ============================================================ **
** ######################################################################### **
** ## MS-ENGINE              Copyright (c) MS-Design, All Rights Reserved ## **
** ######################################################################### **
** ## Defines the 'Variable' namespace and methods for the guest to use   ## **
** ## in registering their own cvars. This file is invoked by             ## **
** ## 'lualib.hpp'.                                                       ## **
** ######################################################################### **
** ------------------------------------------------------------------------- */
#pragma once                           // Only one incursion allowed
/* ========================================================================= **
** ######################################################################### **
** ========================================================================= */
// % Variable
/* ------------------------------------------------------------------------- */
// ! The Variable class allows creation and manipulation of console commands
/* ========================================================================= */
namespace LLVariable {                 // Console namespace
/* -- Dependencies --------------------------------------------------------- */
using namespace ICVar::P;              using namespace ICVarLib::P;
using namespace ILuaVariable::P;
/* ========================================================================= */
// $ Variable:Destroy
// ? Unregisters the specified console command.
/* ------------------------------------------------------------------------- */
LLFUNC(Destroy, LCCLASSDESTROY(1, Variable));
/* ========================================================================= */
// $ Variable:Empty
// < Empty:boolean=Is the value empty?
// ? Returns if the value is empty.
/* ------------------------------------------------------------------------- */
LLFUNCEX(Empty, 1, LCPUSHVAR(LCGETPTR(1, Variable)->Empty()));
/* ========================================================================= */
// $ Variable:Id
// < Id:integer=The id of the variable.
// ? Returns the unique id of the variable.
/* ------------------------------------------------------------------------- */
LLFUNCEX(Id, 1, LCPUSHVAR(LCGETPTR(1, Variable)->CtrGet()));
/* ========================================================================= */
// $ Variable:Name
// < Name:string=The name of the console command.
// ? Returns the name of the console command this object was registered with.
/* ------------------------------------------------------------------------- */
LLFUNCEX(Name, 1, LCPUSHVAR(LCGETPTR(1, Variable)->Name()));
/* ========================================================================= */
// $ Variable:Default
// < Value:string=The engine default cvar value.
// ? Retrieves the value of the specified cvar name. An exception is raised if
// ? the specified cvar does not exist.
/* ------------------------------------------------------------------------- */
LLFUNCEX(Default, 1, LCPUSHVAR(LCGETPTR(1, Variable)->Default()));
/* ========================================================================= */
// $ Variable:Get
// < Value:mixed=The engine cvar value.
// ? Retrieves the value of the specified cvar name. An exception is raised if
// ? the specified cvar does not exist.
/* ------------------------------------------------------------------------- */
LLFUNCEX(Get, 1, LCPUSHVAR(LCGETPTR(1, Variable)->Get()));
/* ========================================================================= */
// $ Variable:Reset
// ? Resets the cvar to the default value as registered.
/* ------------------------------------------------------------------------- */
LLFUNC(Reset, LCGETPTR(1, Variable)->Reset());
/* ========================================================================= */
// $ Variable:Set
// > Value:string=The new cvar value.
// ? Sets the new value of the specified cvar name. An exception is raised if
// ? any error occurs. See Variable.Result to see the possible results.
/* ------------------------------------------------------------------------- */
LLFUNCEX(Set, 1,
  LCPUSHVAR(LCGETPTR(1, Variable)->Set(LCGETCPPSTRING(2, "Value"))));
/* ========================================================================= **
** ######################################################################### **
** ## Variable:* member functions structure                               ## **
** ######################################################################### **
** ------------------------------------------------------------------------- */
LLRSMFBEGIN                            // Variable:* member functions begin
  LLRSFUNC(Default), LLRSFUNC(Destroy), LLRSFUNC(Get), LLRSFUNC(Id),
  LLRSFUNC(Name),    LLRSFUNC(Reset),   LLRSFUNC(Set),
LLRSEND                                // Variable:* member functions end
/* ========================================================================= **
** ######################################################################### **
** ## Variable.* namespace functions                                      ## **
** ######################################################################### **
** ========================================================================= */
// $ Variable.Register
// > Name:string=The engine cvar name.
// > Default:string=The default cvar value.
// > Flags:integer=The cvar flags.
// > Callback:function=The callback global function name.
// < Class:Variable=The Variable object created.
// ? Registers the specified cvar name with the specified default value. The
// ? default value is ignored if there is already a saved value in the Sql
// ? cvars database. If the saved value is a corrupted encrypted value then
// ? the default is used. If the cvar already exists then an exception is
// ? thrown so make sure you check for it first if do not expect this call
// ? to garauntee success. Normally it is unless there is a memory allocation
// ? issue. You can register an unlimited amount of variables.
// ? See Variable.Flags to see available flags.
/* ------------------------------------------------------------------------- */
LLFUNCEX(Register, 1, LCCLASSCREATE(Variable)->Init(lS));
/* ========================================================================= */
// $ Variable.Exists
// > Variable:string=The console command name to lookup
// < Registered:boolean=True if the command is registered
// ? Returns if the specified console command is registered which includes
// ? the built-in engine console commands too.
/* ------------------------------------------------------------------------- */
LLFUNCEX(Exists, 1,
  LCPUSHVAR(cCVars->VarExists(LCGETCPPSTRINGNE(1, "Variable"))));
/* ========================================================================= */
// $ Variable.GetInt
// > Id:integer=The engine cvar index.
// < Value:mixed=The engine internal id value.
// ? Retrieves the value of the specified cvar given at the specified id. The
// ? id's are populated as key/value pairs in the 'Variable.Internal' table.
/* ------------------------------------------------------------------------- */
LLFUNCEX(GetInt, 1, LCPUSHVAR(cCVars->GetStrInternal(
  LCGETINTLGE(CVarEnums, 1, CVAR_FIRST, CVAR_MAX, "Id"))));
/* ========================================================================= */
// $ Variable.SetInt
// > Id:integer=The engine cvar index.
// > Value:string=The new engine cvar value.
// ? Sets the new value of the specified engine cvar name. An exception is
// ? raised if any error occurds. See Variable.Result to see the possible
// ? results.
/* ------------------------------------------------------------------------- */
LLFUNCEX(SetInt, 1, LCPUSHVAR(cCVars->SetInternal(
  LCGETINTLGE(CVarEnums, 1, CVAR_FIRST, CVAR_MAX, "Id"),
  LCGETCPPSTRING(2, "Value"))));
/* ========================================================================= */
// $ Variable.ResetInt
// > Id:integer=The engine cvar index.
// ? Resets the specified cvar to its default variable.
/* ------------------------------------------------------------------------- */
LLFUNCEX(ResetInt, 1, LCPUSHVAR(cCVars->Reset(LCGETINTLGE(CVarEnums,
  1, CVAR_FIRST, CVAR_MAX, "Id"))));
/* ========================================================================= */
// $ Variable.Save
// < Count:number=Number of items saved
// ? Commits all modified variables to database. Returns number of items
// ? written.
/* ------------------------------------------------------------------------- */
LLFUNCEX(Save, 1, LCPUSHVAR(cCVars->Save()));
/* ========================================================================= **
** ######################################################################### **
** ## Variable.* namespace functions structure                            ## **
** ######################################################################### **
** ------------------------------------------------------------------------- */
LLRSBEGIN                              // Variable.* namespace functions begin
  LLRSFUNC(Empty),    LLRSFUNC(Exists), LLRSFUNC(GetInt), LLRSFUNC(Register),
  LLRSFUNC(ResetInt), LLRSFUNC(Save),   LLRSFUNC(SetInt),
LLRSEND                                // Variable.* namespace functions end
/* ========================================================================= **
** ######################################################################### **
** ## Variable.* namespace constants                                      ## **
** ######################################################################### **
** ========================================================================= */
// @ Variable.Flags
// < Data:table=A table of const string/int key pairs
// ? Returns all the cvar flags supported by the engine in the form of a
// ? string/int key pairs which can be used with the Variable.Register()
// ? command.
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
  LLRSKTITEM(C,POW2),                  LLRSKTITEM(C,DEFLATE),
  LLRSKTITEM(C,ALPHA),                 LLRSKTITEM(C,FILENAME),
  LLRSKTITEM(C,NUMERIC),               LLRSKTITEM(M,TRIM),
  LLRSKTITEM(O,SAVEFORCE),
LLRSKTEND                              // End of cvar register status flags
/* ========================================================================= */
// @ Variable.Result
// < Data:table=A table of const string/int key pairs
// ? Returns the possible values that can be returned with the Variable.Set()
// ? function. Any other errors are thrown as an exception.
/* ------------------------------------------------------------------------- */
LLRSKTBEGIN(Result)                    // Beginning of cvar result flags
  LLRSKTITEM(CVS_,OK),                 LLRSKTITEM(CVS_,OKNOTCHANGED),
LLRSKTEND                              // End of cvar result flags
/* ========================================================================= **
** ######################################################################### **
** ## Variable.* namespace constants structure                            ## **
** ######################################################################### **
** ========================================================================= */
LLRSCONSTBEGIN                         // Variable.* namespace consts begin
  LLRSCONST(Flags),       LLRSCONST(Result),
LLRSCONSTEND                           // Variable.* namespace consts end
/* ========================================================================= */
}                                      // End of Console namespace
/* == EoF =========================================================== EoF == */

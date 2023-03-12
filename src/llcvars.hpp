/* == LLCVARS.HPP ========================================================== */
/* ######################################################################### */
/* ## MS-ENGINE              Copyright (c) MS-Design, All Rights Reserved ## */
/* ######################################################################### */
/* ## Defines the 'CVars' namespace and methods for the guest to use in   ## */
/* ## Lua. This file is invoked by 'lualib.hpp'.                          ## */
/* ######################################################################### */
/* ------------------------------------------------------------------------- */
#pragma once                           // Only one incursion allowed
/* ========================================================================= */
/* ######################################################################### */
/* ========================================================================= */
// % CVars
/* ------------------------------------------------------------------------- */
// ! The cvars class allows the programmer to create their own cvars and to
// ! retrieve and modify their own or built-in cCVars->
/* ========================================================================= */
LLNAMESPACEBEGIN(CVars)                // CVars namespace
/* -- Includes ------------------------------------------------------------- */
using namespace IfCVar;                // Using cvar namespace
/* ========================================================================= */
/* ######################################################################### */
/* ========================================================================= */
// $ CVars.Get
// > Name:string=The engine cvar name.
// < Value:mixed=The engine cvar value.
// ? Retrieves the value of the specified cvar name. An exception is raised if
// ? the specified cvar does not exist.
/* ------------------------------------------------------------------------- */
LLFUNCEX(Get, 1,
  LCPUSHXSTR(cCVars->LuaGetStrSafe(LCGETCPPSTRINGNE(1, "Variable"))));
/* ========================================================================= */
// $ CVars.Empty
// > Name:string=The engine cvar name.
// < IsEmpty:boolean=The specified cvar is empty.
// ? Checks if the specified cvar is empty. Perhaps a little faster than using
// ? CVars.Get which builds a new string, especially if the string is long.
/* ------------------------------------------------------------------------- */
LLFUNCEX(Empty, 1,
  LCPUSHBOOL(cCVars->IsVarStrEmptySafe(LCGETCPPSTRINGNE(1, "Variable"))));
/* ========================================================================= */
// $ CVars.Set
// > Name:string=The engine cvar name.
// > Value:string=The new engine cvar value.
// ? Sets the new value of the specified engine cvar name. An exception is
// ? raised if the specified cvar does not exist or the value cannot be set
// ? due to type restraints specified by the cvars creation flags.
/* ------------------------------------------------------------------------- */
LLFUNC(Set, cCVars->SetSafe(LCGETCPPSTRINGNE(1, "Variable"),
  LCGETCPPSTRING(2, "Value"), PUSR));
/* ========================================================================= */
// $ CVars.Reset
// > Name:string=The engine cvar name.
// ? Resets the value of the cvar to the engine's built-in value.
/* ------------------------------------------------------------------------- */
LLFUNC(Reset, cCVars->ResetSafe(LCGETCPPSTRINGNE(1, "Variable"), PUSR));
/* ========================================================================= */
// $ CVars.Unregister
// > Name:string=The engine cvar name.
// ? Unregisters the specified cvar name created with cCVars->Register().
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
// ? See CVars.Flags() to see available flags.
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
  LCPUSHBOOL(cCVars->VarExistsSafe(LCGETCPPSTRINGNE(1, "Identifier"))));
/* ========================================================================= */
// $ CVars.Save
// < Count:number=Number of items saved
// ? Commits all modified variables to database. Returns number of items
// ? written.
/* ------------------------------------------------------------------------- */
LLFUNCEX(Save, 1, LCPUSHINT(cCVars->Save()));
/* ========================================================================= */
/* ######################################################################### */
/* ## CVars.* namespace functions structure                               ## */
/* ######################################################################### */
/* ------------------------------------------------------------------------- */
LLRSBEGIN                              // CVars.* namespace functions begin
  LLRSFUNC(Get),                       // Get value of specified CVar
  LLRSFUNC(Empty),                     // Value of CVar is empty?
  LLRSFUNC(Exists),                    // CVar exists?
  LLRSFUNC(Register),                  // Register CVar
  LLRSFUNC(Reset),                     // Reset CVar value
  LLRSFUNC(Save),                      // Commit modified CVars
  LLRSFUNC(Set),                       // Set CVar value
  LLRSFUNC(Unregister),                // Unregister CVar
LLRSEND                                // CVars.* namespace functions end
/* ========================================================================= */
/* ######################################################################### */
/* ## CVars.* namespace constants structure                               ## */
/* ######################################################################### */
/* ========================================================================= */
// @ CVars.Flags
// < Data:table=A table of const string/int key pairs
// ? Returns all the cvar flags supported by the engine in the form of a
// ? string/int key pairs which can be used with the cCVars->Register() command
// ? STRING    (0x00001): Store an unlimited amount of text.
// ? INTEGER   (0x00002): Store a 64-bit integer.
// ? FLOAT     (0x00004): Store a 64-bit IEEE floating number.
// ? BOOLEAN   (0x00008): Store a simple binary number (0 or 1).
// ? SAVEABLE  (0x00010): The variable is persistant.
// ? PROTECTED (0x00020): The variable is encrypted in persistant storage.
// ? NOTEMPTY  (0x00040): The variable cannot be empty.
// ? UNSIGNED  (0x00080): A negative value is not allowed.
// ? POW2      (0x00100): The value cannot be zero and not a power of two.
// ? POW2Z     (0x00200): The value can be zero but not a power of two.
// ? TRIM      (0x10000): Whitespaces are trimmed from prefix and suffix.
// ? SAVEFORCE (0x20000): The value is saved even if default wasn't changed.
/* ------------------------------------------------------------------------- */
LLRSKTBEGIN(Flags)                     // Beginning of socket status flags
LLRSKTITEM(T,STRING),                  LLRSKTITEM(T,INTEGER),
LLRSKTITEM(T,FLOAT),                   LLRSKTITEM(T,BOOLEAN),
LLRSKTITEM(T,UINTEGER),                LLRSKTITEM(T,UFLOAT),
LLRSKTITEM(T,INTEGERSAVE),             LLRSKTITEM(T,FLOATSAVE),
LLRSKTITEM(T,UINTEGERSAVE),            LLRSKTITEM(T,UFLOATSAVE),
LLRSKTITEM(T,STRINGSAVE),              LLRSKTITEM(T,BOOLEANSAVE),
LLRSKTITEM(C,SAVEABLE),                LLRSKTITEM(C,PROTECTED),
LLRSKTITEM(C,NOTEMPTY),                LLRSKTITEM(C,UNSIGNED),
LLRSKTITEM(C,POW2),                    LLRSKTITEM(C,POW2Z),
LLRSKTITEM(C,DEFLATE),                 LLRSKTITEM(C,ALPHA),
LLRSKTITEM(C,FILENAME),                LLRSKTITEM(C,NUMERIC),
LLRSKTITEM(M,TRIM),                    LLRSKTITEM(O,SAVEFORCE),
LLRSKTEND                              // End of socket status flags
/* ========================================================================= */
LLRSCONSTBEGIN                         // CVars.* namespace consts begin
LLRSCONST(Flags),                      // Socket status flags
LLRSCONSTEND                           // CVars.* namespace consts end
/* ========================================================================= */
LLNAMESPACEEND                         // End of CVars namespace
/* == EoF =========================================================== EoF == */

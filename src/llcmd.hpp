/* == LLCMD.HPP ============================================================ **
** ######################################################################### **
** ## MS-ENGINE              Copyright (c) MS-Design, All Rights Reserved ## **
** ######################################################################### **
** ## Defines the 'Command' namespace and methods for the guest to use in ## **
** ## registering their own console commands. This file is invoked by     ## **
** ## 'lualib.hpp'.                                                       ## **
** ######################################################################### **
** ------------------------------------------------------------------------- */
#pragma once                           // Only one incursion allowed
/* ========================================================================= **
** ######################################################################### **
** ========================================================================= */
// % Command
/* ------------------------------------------------------------------------- */
// ! The Command class allows creation and manipulation of console commands
/* ========================================================================= */
namespace LLCommand {                  // Console namespace
/* -- Dependencies --------------------------------------------------------- */
using namespace IConsole::P;           using namespace ILuaCommand::P;
/* ========================================================================= */
// $ Command:Destroy
// ? Unregisters the specified console command.
/* ------------------------------------------------------------------------- */
LLFUNC(Destroy, LCCLASSDESTROY(1, Command));
/* ========================================================================= */
// $ Command:Id
// < Id:integer=The id of the command.
// ? Returns the unique id of the command.
/* ------------------------------------------------------------------------- */
LLFUNCEX(Id, 1, LCPUSHVAR(LCGETPTR(1, Command)->CtrGet()));
/* ========================================================================= */
// $ Command:Name
// < Name:string=The name of the console command.
// ? Returns the name of the console command this object was registered with.
/* ------------------------------------------------------------------------- */
LLFUNCEX(Name, 1, LCGETPTR(1, Command)->Name());
/* ========================================================================= **
** ######################################################################### **
** ## Command:* member functions structure                                ## **
** ######################################################################### **
** ------------------------------------------------------------------------- */
LLRSMFBEGIN                            // Command:* member functions begin
  LLRSFUNC(Destroy), LLRSFUNC(Id), LLRSFUNC(Name),
LLRSEND                                // Command:* member functions end
/* ========================================================================= **
** ######################################################################### **
** ## Command.* namespace functions                                       ## **
** ######################################################################### **
** ========================================================================= */
// $ Command.Register
// > Name:string=The console command name.
// > Minimum:integer=The minimum number of parameters allowed.
// > Maximum:integer=The maximum number of parameters allowed.
// > Callback:function=Pointer to the function to callback.
// < Object:Command=The class object created.
// ? Registers the specified console command and returns a reference to the
// ? newly created object which unregisters the console command when garbage
// ? collected.
/* ------------------------------------------------------------------------- */
LLFUNCEX(Register, 1, LCCLASSCREATE(Command)->Init(lS));
/* ========================================================================= */
// $ Command.Exists
// > Command:string=The console command name to lookup
// < Registered:boolean=True if the command is registered
// ? Returns if the specified console command is registered which includes
// ? the built-in engine console commands too.
/* ------------------------------------------------------------------------- */
LLFUNCEX(Exists, 1,
  LCPUSHVAR(cConsole->CommandIsRegistered(LCGETCPPSTRINGNE(1, "Command"))));
/* ========================================================================= **
** ######################################################################### **
** ## Command.* namespace functions structure                             ## **
** ######################################################################### **
** ------------------------------------------------------------------------- */
LLRSBEGIN                              // Command.* namespace functions begin
  LLRSFUNC(Exists), LLRSFUNC(Register),
LLRSEND                                // Command.* namespace functions end
/* ========================================================================= */
}                                      // End of Console namespace
/* == EoF =========================================================== EoF == */

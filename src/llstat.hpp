/* == LLSTAT.HPP =========================================================== */
/* ######################################################################### */
/* ## MS-ENGINE              Copyright (c) MS-Design, All Rights Reserved ## */
/* ######################################################################### */
/* ## Defines the 'Stat' namespace and methods for the guest to use in    ## */
/* ## Lua. This file is invoked by 'lualib.hpp'.                          ## */
/* ######################################################################### */
/* ------------------------------------------------------------------------- */
#pragma once                           // Only one incursion allowed
/* ========================================================================= */
/* ######################################################################### */
/* ========================================================================= */
// % Stat
/* ------------------------------------------------------------------------- */
// ! Allows creating of tables with properly justified and proportioned data.
// ! Useful for the neat display of debug data to the console or logging to
// ! a file.
/* ========================================================================= */
LLNAMESPACEBEGIN(Stat)                 // Stat namespace
/* -- Includes ------------------------------------------------------------- */
using namespace IfStat;                // Using stat interface
/* ========================================================================= */
/* ######################################################################### */
/* ========================================================================= */
// $ Stat:Data
// > Value:String=Data string to add to the cell.
// ? Inserts the specified string into the next cell.
/* ------------------------------------------------------------------------- */
LLFUNC(Data, LCGETPTR(1, Stat)->Data(LCGETCPPSTRING(2, "Value")));
/* ========================================================================= */
// $ Stat:Header
// > Value:String=Header string to add to the cell.
// > Right:Boolean=Header is right justified?
// ? Inserts the specified string into the next header. This has no effect if
// ? data has been added.
/* ------------------------------------------------------------------------- */
LLFUNC(Header, LCGETPTR(1, Stat)->Header(LCGETCPPSTRING(2, "Value"),
                                         LCGETBOOL(3, "Right")));
/* ========================================================================= */
// $ Stat:HeaderDupe
// > Count:Integer=Number of times to duplicate the current headers.
// ? Duplicates the currently stored headers.
/* ------------------------------------------------------------------------- */
LLFUNC(HeaderDupe, LCGETPTR(1, Stat)->
  DupeHeader(LCGETINT(size_t, 2, "Count")));
/* ========================================================================= */
// $ Stat:DataN
// > Value:Number=Number to add to the next cell.
// > Precision:Integer=Amount of precision to use on the number.
// ? Inserts the specified number into the next cell.
/* ------------------------------------------------------------------------- */
LLFUNC(DataN, LCGETPTR(1, Stat)->DataN(LCGETNUM(lua_Number, 2, "Value"),
                                       LCGETNUM(int,        3, "Precision")));
/* ========================================================================= */
// $ Stat:DataI
// > Value:Integer=Integer to add to the next cell.
// ? Inserts the specified integer into the next cell.
/* ------------------------------------------------------------------------- */
LLFUNC(DataI, LCGETPTR(1, Stat)->DataN(LCGETNUM(lua_Integer, 2, "Value")));
/* ========================================================================= */
// $ Stat:Finish
// > OmitLF:Boolean=Omits the ending linefeed.
// < Output:String=The final formatted output.
// ? Builds the whole formatted output and returns a string. All data and
// ? headers are erased and the class can be reused. Sizes and thus memory
// ? are preserved as per C++ STL rules when being reused.
/* ------------------------------------------------------------------------- */
LLFUNCEX(Finish, 1, LCPUSHXSTR(LCGETPTR(1, Stat)->
  Finish(LCGETBOOL(2, "OmitLF"), LCGETINT(size_t, 3, "Gap"))));
/* ========================================================================= */
// $ Stat:Destroy
// ? Destroys the stat object and frees all the memory associated with it. The
// ? object will no longer be useable after this call and an error will be
// ? generated if accessed.
/* ------------------------------------------------------------------------- */
LLFUNC(Destroy, LCCLASSDESTROY(1, Stat));
/* ========================================================================= */
/* ######################################################################### */
/* ## Stat:* member functions structure                                   ## */
/* ######################################################################### */
/* ------------------------------------------------------------------------- */
LLRSMFBEGIN                            // stat:* member functions begin
  LLRSFUNC(Data),                      // Insert string
  LLRSFUNC(DataI),                     // Insert integer
  LLRSFUNC(DataN),                     // Insert number
  LLRSFUNC(Destroy),                   // Destroy internal stat object
  LLRSFUNC(Header),                    // Add header
  LLRSFUNC(HeaderDupe),                // Duplicate headers
  LLRSFUNC(Finish),                    // Build data
LLRSEND                                // Stat:* member functions end
/* ========================================================================= */
// $ Stat.Create
// > Name:string=Name of the class
// ? Creates an empty stat table.
/* ------------------------------------------------------------------------- */
LLFUNCEX(Create, 1,
  LCCLASSCREATE(Stat)->IdentSet(LCGETSTRING(char, 1, "Name")));
/* ========================================================================= */
/* ######################################################################### */
/* ## Stat:* namespace functions structure                                ## */
/* ######################################################################### */
/* ------------------------------------------------------------------------- */
LLRSBEGIN                              // Bin.* namespace functions begin
  LLRSFUNC(Create),                    // Create the bin
LLRSEND                                // Bin.* namespace functions end
/* ========================================================================= */
LLNAMESPACEEND                         // End of Bin namespace
/* == EoF =========================================================== EoF == */

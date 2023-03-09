/* == LLSTAT.HPP =========================================================== **
** ######################################################################### **
** ## MS-ENGINE              Copyright (c) MS-Design, All Rights Reserved ## **
** ######################################################################### **
** ## Defines the 'Stat' namespace and methods for the guest to use in    ## **
** ## Lua. This file is invoked by 'lualib.hpp'.                          ## **
** ######################################################################### **
** ------------------------------------------------------------------------- */
#pragma once                           // Only one incursion allowed
/* ========================================================================= **
** ######################################################################### **
** ========================================================================= */
// % Stat
/* ------------------------------------------------------------------------- */
// ! Allows creating of tables with properly justified and proportioned data.
// ! Useful for the neat display of debug data to the console or logging to
// ! a file.
/* ========================================================================= */
namespace LLStat {                     // Stat namespace
/* -- Dependencies --------------------------------------------------------- */
using namespace IStat::P;              using namespace IStd::P;
/* ========================================================================= **
** ######################################################################### **
** ## Stat:* member functions                                             ## **
** ######################################################################### **
** ========================================================================= */
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
// $ Stat:Headers
// < Count:Integer=Number of headers registered
// ? Returns the number of headers in the table
/* ------------------------------------------------------------------------- */
LLFUNCEX(Headers, 1, LCGETPTR(1, Stat)->Headers());
/* ========================================================================= */
// $ Stat:Reserve
// > Rows:Integer=Reserve memory for this many rows.
// ? Reserves memory for this many rows.
/* ------------------------------------------------------------------------- */
LLFUNC(Reserve, LCGETPTR(1, Stat)->Reserve(LCGETINT(size_t, 2, "Rows")));
/* ========================================================================= */
// $ Stat:Sort
// > Column:Integer=Sort from this column
// > Desc:Boolean=Sort the list in descending order
// ? Sorts the data by the specified column.
/* ------------------------------------------------------------------------- */
LLFUNCBEGIN(Sort)
  Statistic &sStat = *LCGETPTR(1, Stat);
  sStat.Sort(
    LCGETINTLGE(ssize_t, 2, 0, static_cast<ssize_t>(sStat.Headers()),
      "Column"),
    LCGETBOOL(3, "Descending"));
LLFUNCEND
/* ========================================================================= */
// $ Stat:SortTwo
// > Column1:Integer=Sort from this primary column
// > Column2:Integer=Sort from this secondary column if primary is the same
// > Desc:Boolean=Sort the list in descending order
// ? Sorts the data by the specified columns.
/* ------------------------------------------------------------------------- */
LLFUNCBEGIN(SortTwo)
  Statistic &sStat = *LCGETPTR(1, Stat);
  const ssize_t sstHeaders = static_cast<ssize_t>(sStat.Headers());
  sStat.SortTwo(
    LCGETINTLGE(ssize_t, 2, 0, sstHeaders, "Column1"),
    LCGETINTLGE(ssize_t, 3, 0, sstHeaders, "Column2"),
    LCGETBOOL(4, "Descending"));
LLFUNCEND
/* ========================================================================= */
// $ Stat:Finish
// > OmitLF:Boolean=Omits the ending linefeed.
// < Output:String=The final formatted output.
// ? Builds the whole formatted output and returns a string. All data and
// ? headers are erased and the class can be reused. Sizes and thus memory
// ? are preserved as per C++ STL rules when being reused.
/* ------------------------------------------------------------------------- */
LLFUNCEX(Finish, 1, LCPUSHVAR(LCGETPTR(1, Stat)->
  Finish(LCGETBOOL(2, "OmitLF"), LCGETINT(size_t, 3, "Gap"))));
/* ========================================================================= */
// $ Stat:Id
// < Id:integer=The id number of the Stat object.
// ? Returns the unique id of the Stat object.
/* ------------------------------------------------------------------------- */
LLFUNCEX(Id, 1, LCPUSHVAR(LCGETPTR(1, Stat)->CtrGet()));
/* ========================================================================= */
// $ Stat:Name
// < Id:string=The stat identifier
// ? Returns the identifier of the Stat object.
/* ------------------------------------------------------------------------- */
LLFUNCEX(Name, 1, LCPUSHVAR(LCGETPTR(1, Stat)->IdentGet()));
/* ========================================================================= */
// $ Stat:Destroy
// ? Destroys the stat object and frees all the memory associated with it. The
// ? object will no longer be useable after this call and an error will be
// ? generated if accessed.
/* ------------------------------------------------------------------------- */
LLFUNC(Destroy, LCCLASSDESTROY(1, Stat));
/* ========================================================================= **
** ######################################################################### **
** ## Stat:* member functions structure                                   ## **
** ######################################################################### **
** ------------------------------------------------------------------------- */
LLRSMFBEGIN                            // stat:* member functions begin
  LLRSFUNC(Data),       LLRSFUNC(DataI),   LLRSFUNC(DataN),
  LLRSFUNC(Destroy),    LLRSFUNC(Finish),  LLRSFUNC(Header),
  LLRSFUNC(HeaderDupe), LLRSFUNC(Headers), LLRSFUNC(Id),
  LLRSFUNC(Name),       LLRSFUNC(Reserve), LLRSFUNC(Sort),
  LLRSFUNC(SortTwo),
LLRSEND                                // Stat:* member functions end
/* ========================================================================= */
// $ Stat.Create
// > Name:string=Name of the class
// ? Creates an empty stat table.
/* ------------------------------------------------------------------------- */
LLFUNCEX(Create, 1,
  LCCLASSCREATE(Stat)->IdentSet(LCGETSTRING(char, 1, "Name")));
/* ========================================================================= **
** ######################################################################### **
** ## Stat:* namespace functions structure                                ## **
** ######################################################################### **
** ------------------------------------------------------------------------- */
LLRSBEGIN                              // Bin.* namespace functions begin
  LLRSFUNC(Create),
LLRSEND                                // Bin.* namespace functions end
/* ========================================================================= */
}                                      // End of Bin namespace
/* == EoF =========================================================== EoF == */

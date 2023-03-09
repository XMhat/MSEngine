/* == LLSQL.HPP ============================================================ */
/* ######################################################################### */
/* ## MS-ENGINE              Copyright (c) MS-Design, All Rights Reserved ## */
/* ######################################################################### */
/* ## Defines the 'Sql' namespace and methods for the guest to use in     ## */
/* ## Lua. This file is invoked by 'lualib.hpp'.                          ## */
/* ######################################################################### */
/* ------------------------------------------------------------------------- */
#pragma once                           // Only one incursion allowed
/* ========================================================================= */
/* ######################################################################### */
/* ========================================================================= */
// % Sql
/* ------------------------------------------------------------------------- */
// ! The sql class allows manipulation of the engine database using the sqlite
// ! library.
/* ========================================================================= */
LLNAMESPACEBEGIN(Sql)                  // Sql namespace
/* -- Includes ------------------------------------------------------------- */
using namespace IfSql;                 // Using sql interface
/* ========================================================================= */
/* ######################################################################### */
/* ========================================================================= */
// $ Sql.Error
// < Code:integer=Last error code.
// ? Returns the error code of the last SQLlite operation.
/* ------------------------------------------------------------------------- */
LLFUNCEX(Error, 1, LCPUSHINT(cSql->GetError()));
/* ========================================================================= */
// $ Sql.ErrorStr
// < Error:string=Last error reason string.
// ? Returns the string version of the error code.
/* ------------------------------------------------------------------------- */
LLFUNCEX(ErrorStr, 1, LCPUSHXSTR(cSql->GetErrorAsIdString()));
/* ========================================================================= */
// $ Sql.Reason
// < Error:string=Last error reason string.
// ? Returns the error string of the last SQLlite operation.
/* ------------------------------------------------------------------------- */
LLFUNCEX(Reason, 1, LCPUSHXSTR(cSql->GetErrorStr()));
/* ========================================================================= */
// $ Sql.Records
// < Records:table=Resulting records.
// ? Returns the resulting records of the last SQLlite operation. The format
// ? is an indexed table of string key/value tables. e.g.
// ? table = {
// ?   { ["Index"]=1, ["Name"]="John Doe", ["Age"]=37 },
// ?   { ["Index"]=2, ["Name"]="Bill Gates", ["Age"]=33 },
// ?   etc...
// ? }
/* ------------------------------------------------------------------------- */
LLFUNCEX(Records, 1, cSql->RecordsToLuaTable(lS));
/* ========================================================================= */
// $ Sql.Exec
// > Code:string=The SQLlite code to execute.
// > Data:Any=The arguments used in place of '?'
// < Result:boolean=If the statement was added or not
// ? Execute the specified SQLlite statement. See
// ? https://www.sqlite.org/docs.html for more information on the SQLlite API.
/* ------------------------------------------------------------------------- */
LLFUNCEX(Exec, 1,
  LCPUSHINT(cSql->ExecuteFromLua(lS, LCGETCPPSTRING(1, "Command"))));
/* ========================================================================= */
// $ Sql.Reset
// ? Cleans up the last result, error and response.
/* ------------------------------------------------------------------------- */
LLFUNC(Reset, cSql->Reset());
/* ========================================================================= */
// $ Sql.Affected
// ? Returns number of affected rows in the last result
/* ------------------------------------------------------------------------- */
LLFUNCEX(Affected, 1, LCPUSHINT(cSql->Affected()));
/* ========================================================================= */
// $ Sql.Time
// ? Returns time taken with the last executed SQL statement
/* ------------------------------------------------------------------------- */
LLFUNCEX(Time, 1, LCPUSHNUM(cSql->Time()));
/* ========================================================================= */
// $ Sql.Active
// ? Returns wether a transaction is in progress or not.
/* ------------------------------------------------------------------------- */
LLFUNCEX(Active, 1, LCPUSHBOOL(cSql->Active()));
/* ========================================================================= */
// $ Sql.Size
// ? Returns size of database.
/* ------------------------------------------------------------------------- */
LLFUNCEX(Size, 1, LCPUSHINT(cSql->Size()));
/* ========================================================================= */
// $ Sql.Begin
// < Result:integer=The result of the operation
// ? Begins a new transaction. Returns non-zero if the call fails.
/* ------------------------------------------------------------------------- */
LLFUNCEX(Begin, 1, LCPUSHINT(cSql->Begin()));
/* ========================================================================= */
// $ Sql.Commit
// < Result:integer=The result of the operation
// ? Ends an existing transaction and commits it. Returns non-zero if the call
// ? fails.
/* ------------------------------------------------------------------------- */
LLFUNCEX(End, 1, LCPUSHINT(cSql->End()));
/* ========================================================================= */
/* ######################################################################### */
/* ## Sql.* namespace functions structure                              ## */
/* ######################################################################### */
/* ------------------------------------------------------------------------- */
LLRSBEGIN                              // Sql.* namespace functions begin
  LLRSFUNC(Active),                    // Transaction is active?
  LLRSFUNC(Affected),                  // Return rows affected
  LLRSFUNC(Begin),                     // Begins a new transaction
  LLRSFUNC(End),                       // Ends and commits a new transaction
  LLRSFUNC(Error),                     // Return error code
  LLRSFUNC(ErrorStr),                  // Return error code as string
  LLRSFUNC(Exec),                      // Execute sql command
  LLRSFUNC(Reason),                    // Return error reason
  LLRSFUNC(Records),                   // Return result records
  LLRSFUNC(Reset),                     // Clean results
  LLRSFUNC(Size),                      // Return size of database
  LLRSFUNC(Time),                      // Return duration of last query
LLRSEND                                // Sql.* namespace functions end
/* ========================================================================= */
// @ Sql.Codes
// < Data:table=The entire list of errors SQL supports
// ? Returns a table of key/value pairs that identify a SQL error code.
/* ------------------------------------------------------------------------- */
LLRSKTBEGIN(Codes)                     // Beginning of error codes
LLRSKTITEM(SQLITE_,OK),                // Successful result
LLRSKTITEM(SQLITE_,ERROR),             // Generic error
LLRSKTITEM(SQLITE_,INTERNAL),          // Internal logic error in SQLite
LLRSKTITEM(SQLITE_,PERM),              // Access permission denied
LLRSKTITEM(SQLITE_,ABORT),             // Callback routine requested an abort
LLRSKTITEM(SQLITE_,BUSY),              // The database file is locked
LLRSKTITEM(SQLITE_,LOCKED),            // A table in the database is locked
LLRSKTITEM(SQLITE_,NOMEM),             // A malloc() failed
LLRSKTITEM(SQLITE_,READONLY),          // Attempt to write a readonly database
LLRSKTITEM(SQLITE_,INTERRUPT),         // Op terminated by sqlite3_interrupt()
LLRSKTITEM(SQLITE_,IOERR),             // Some kind of disk I/O error occurred
LLRSKTITEM(SQLITE_,CORRUPT),           // The database disk image is malformed
LLRSKTITEM(SQLITE_,NOTFOUND),          // Unknown op in sqlite3_file_control()
LLRSKTITEM(SQLITE_,FULL),              // Insertion failed because db is full
LLRSKTITEM(SQLITE_,CANTOPEN),          // Unable to open the database file
LLRSKTITEM(SQLITE_,PROTOCOL),          // Database lock protocol error
LLRSKTITEM(SQLITE_,EMPTY),             // Internal use only
LLRSKTITEM(SQLITE_,SCHEMA),            // The database schema changed
LLRSKTITEM(SQLITE_,TOOBIG),            // String or BLOB exceeds size limit
LLRSKTITEM(SQLITE_,CONSTRAINT),        // Abort due to constraint violation
LLRSKTITEM(SQLITE_,MISMATCH),          // Data type mismatch
LLRSKTITEM(SQLITE_,MISUSE),            // Library used incorrectly
LLRSKTITEM(SQLITE_,NOLFS),             // Uses OS features unsupported on host
LLRSKTITEM(SQLITE_,AUTH),              // Authorization denied
LLRSKTITEM(SQLITE_,FORMAT),            // Not used
LLRSKTITEM(SQLITE_,RANGE),             // 2nd arg to sqlite3_bind out of range
LLRSKTITEM(SQLITE_,NOTADB),            // File opened that is not a db file
LLRSKTITEM(SQLITE_,NOTICE),            // Notifications from sqlite3_log()
LLRSKTITEM(SQLITE_,WARNING),           // Warnings from sqlite3_log()
LLRSKTITEM(SQLITE_,ROW),               // sqlite3_step() has another row ready
LLRSKTITEM(SQLITE_,DONE),              // sqlite3_step() has finished executing
LLRSKTEND                              // End of error codes
/* ========================================================================= */
LLRSCONSTBEGIN                         // Sql.* namespace consts begin
LLRSCONST(Codes),                      // Sqlite codes list
LLRSCONSTEND                           // Sql.* namespace consts end
/* ========================================================================= */
LLNAMESPACEEND                         // End of Sql namespace
/* == EoF =========================================================== EoF == */

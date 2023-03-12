/* == SQL.HPP ============================================================== */
/* ######################################################################### */
/* ## MS-ENGINE              Copyright (c) MS-Design, All Rights Reserved ## */
/* ######################################################################### */
/* ## This module handles writing and reading of persistant data to a     ## */
/* ## database on disk.                                                   ## */
/* ######################################################################### */
/* ========================================================================= */
#pragma once                           // Only one incursion allowed
/* ------------------------------------------------------------------------- */
namespace IfSql {                      // Start of module namespace
/* -- Includes ------------------------------------------------------------- */
using namespace Library::Sqlite;       // Using sqlite library functions
using namespace IfAsset;               // Using asset namespace
using namespace IfTimer;               // Using timer namespace
/* -- Sql cvar data types -------------------------------------------------- */
BUILD_FLAGS(SqlCVarData,
  /* -- (Note: Don't ever change these around) ----------------------------- */
  // No flags for this cvar in db?     The data value was encrypted?
  LF_NONE                (0x00000000), LF_ENCRYPTED           (0x00000001)
);/* -- Sql flags ---------------------------------------------------------- */
BUILD_FLAGS(Sql,                       // Sql flags classes
  /* ----------------------------------------------------------------------- */
  // No settings?                      Is temporary database?
  SF_NONE                {0x00000000}, SF_ISTEMPDB            {0x00000001},
  // Delete empty databases?           Debug sql executions?
  SF_DELETEEMPTYDB       {0x00000002}
);/* -- Sql manager class -------------------------------------------------- */
static class Sql final :
  /* -- Base classes ------------------------------------------------------- */
  public Ident,                        // Sql database filename
  private SqlFlags                     // Sql flags
{ /* -- Typedefs --------------------------------------------------- */ public:
  enum ADResult                        // Results for CanDeleteDatabase()
  { /* --------------------------------------------------------------------- */
    ADR_OK_NO_TABLES,                  // No tables exist (delete ok)
    ADR_OK_NO_RECORDS,                 // No records exist (delete ok)
    /* --------------------------------------------------------------------- */
    ADR_ERR,                           // Minimum error value (delete denied)
    /* --------------------------------------------------------------------- */
    ADR_ERR_TEMP_DB = ADR_ERR,         // Is temporary database?
    ADR_ERR_DENY_OPTION,               // Delete not allowed by guest?
    ADR_ERR_LU_TABLE,                  // Error looking up table?
    ADR_ERR_TABLES_EXIST,              // Tables exist?
    ADR_ERR_LU_RECORD,                 // Error looking up records?
    ADR_ERR_RECORDS_EXIST,             // Records exist?
    /* --------------------------------------------------------------------- */
    ADR_MAX                            // Maximum number of result types
  };/* --------------------------------------------------------------------- */
  enum PurgeResult                     // Result to a purge request
  { /* --------------------------------------------------------------------- */
    PR_FAIL,                           // Sql call failed with error
    PR_OK,                             // Sql call succeeded with changes
    PR_OK_NC,                          // Sql call succeeded but no changes
  };/* --------------------------------------------------------------------- */
  enum CreateTableResult               // Create table result
  { /* --------------------------------------------------------------------- */
    CTR_FAIL,                          // Sql create table failed
    CTR_OK,                            // Sql call commited the variable
    CTR_OK_ALREADY,                    // Sql table already exists
  };/* --------------------------------------------------------------------- */
  /* -- Recording data ----------------------------------------------------- */
  struct Cell                          // Sql memblock input record struct
  { /* --------------------------------------------------------------------- */
    union                              // Record multi-type value
    { /* ------------------------------------------------------------------- */
      const sqlite3_int64 qV;          // If data type is an 'int'
      const double        fdV;         // If data type is a 'double'
      const char*const    cpD;         // If data type is something else
    }; /* ------------------------------------------------------------------ */
    const int iS;                      // Size of the block at 'cpD'
    const int iT;                      // Sqlite data type id
    /* -- Constructor that stores a 64-bit integer ------------------------- */
    explicit Cell(const sqlite_int64 qV) :
      /* -- Initialisers --------------------------------------------------- */
      qV(qV),                          // Initialise 64-bit integer
      iS(0),                           // No buffer size
      iT(SQLITE_INTEGER)               // Integer type
      /* -- No code -------------------------------------------------------- */
      { }
    /* -- Constructor that stores similar types of integer ----------------- */
    explicit Cell(const int iV) :
      /* -- Initialisers --------------------------------------------------- */
      Cell{ static_cast<sqlite_int64>(iV) }
      /* -- No code -------------------------------------------------------- */
      { }
    /* -- Constructor that stores similar types of integer ----------------- */
    explicit Cell(const unsigned int uiV) :
      /* -- Initialisers --------------------------------------------------- */
      Cell{ static_cast<sqlite_int64>(uiV) }
      /* -- No code -------------------------------------------------------- */
      { }
    /* -- Constructor that stores similar types of integer ----------------- */
    explicit Cell(const long lV) :
      /* -- Initialisers --------------------------------------------------- */
      Cell{ static_cast<sqlite_int64>(lV) }
      /* -- No code -------------------------------------------------------- */
      { }
    /* -- Constructor that stores similar types of integer ----------------- */
    explicit Cell(const short sV) :
      /* -- Initialisers --------------------------------------------------- */
      Cell{ static_cast<sqlite_int64>(sV) }
      /* -- No code -------------------------------------------------------- */
      { }
    /* -- Initialise as double of type SQLITE_FLOAT ------------------------ */
    explicit Cell(const double fdV) :
      /* -- Initialisers --------------------------------------------------- */
      fdV(fdV),                        // Set floating point number
      iS(0),                           // No buffer size
      iT(SQLITE_FLOAT)                 // Float type
      /* -- No code -------------------------------------------------------- */
      { }
    /* -- Initialise raw data helper --------------------------------------- */
    explicit Cell(const char*const cpD, const int iNS, const int iNT) :
      /* -- Initialisers --------------------------------------------------- */
      cpD(cpD),                        // Set pointer to buffer
      iS(iNS),                         // Set size of buffer
      iT(iNT)                          // Set type of buffer
      /* -- No code -------------------------------------------------------- */
      { }
    /* -- Initialise as a c-string with the specified size as text --------- */
    explicit Cell(const int iNS, const char*const cpS) :
      Cell{ cpS, iNS, SQLITE_TEXT } { }
    /* -- Initialise as a stand alone c-string ----------------------------- */
    explicit Cell(const char*const cpS) :
      Cell{ cpS, IntOrMax<int>(strlen(cpS)), SQLITE_TEXT } { }
    /* -- Initialise as a c++ string --------------------------------------- */
    explicit Cell(const string &strT) :
      Cell{ IntOrMax<int>(strT.length()), strT.data() } { }
    /* -- Initialise as a c-string with the specified size as a blob ------- */
    explicit Cell(const char*const cpD, const int iNS) :
      Cell{ cpD, iNS, SQLITE_BLOB } { }
    /* -- Initialise as a memory block ------------------------------------- */
    explicit Cell(const DataConst &dcM) :
      Cell{ dcM.Ptr<char>(), IntOrMax<int>(dcM.Size()) } { }
    /* -- Uninitialised ---------------------------------------------------- */
    Cell(void) :
      /* -- Initialisers --------------------------------------------------- */
      cpD(nullptr),                       // No data
      iS(0),                           // No buffer size
      iT(SQLITE_NULL)                  // Null type
      /* -- No code -------------------------------------------------------- */
      { }
  };/* --------------------------------------------------------------------- */
  typedef vector<Cell>             CellList;   // Memory block list
  typedef CellList::const_iterator CellListIt; // Memory block iterator
  /* -- Querying data ------------------------------------------------------ */
  class DataListItem :
    /* -- Base classes ----------------------------------------------------- */
    public Memory                              // Memory block and type
  { /* -- Sql type variable ---------------------------------------- */ public:
    int       iT;                              // Type of memory in block
    /* -- Move assignment operator ----------------------------------------- */
    DataListItem &operator=(DataListItem &&dlOther)
      { SwapMemory(std::move(dlOther)); iT = dlOther.iT; return *this; }
    /* -- Initialise with rvalue memory and type --------------------------- */
    DataListItem(Memory &&memInit, const int iType) :
      /* -- Initialisers --------------------------------------------------- */
      Memory{ std::move(memInit) },         // Move other memory block other
      iT(iType)                        // Copy other type over
      /* -- No code -------------------------------------------------------- */
      { }
    /* -- Move constructor ------------------------------------------------- */
    DataListItem(DataListItem &&dlOther) :
      /* -- Initialisers --------------------------------------------------- */
      DataListItem{ std::move(dlOther), dlOther.iT }
      /* -- No code -------------------------------------------------------- */
      { }
    /* --------------------------------------------------------------------- */
    DELETECOPYCTORS(DataListItem);     // Remove copy assign operator/ctor
  };/* -- Querying data ---------------------------------------------------- */
  typedef map<const string,DataListItem> Records; // map of key/data
  typedef Records::const_iterator RecordsIt;   // Iterator to data blocks
  typedef list<Records> Result;             // vector of key/raw data blocks
  /* -- Variables ------------------------------------------------- */ private:
  sqlite3         *sqlDB;              // Pointer to SQL context
  int              iError;             // Last error code
  Result           vKeys;              // Last Execute(Raw) result
  /* -- Tables ------------------------------------------------------------- */
  const string     strMemoryDBName;    // Memory only database (no disk)
  /* -- Query time --------------------------------------------------------- */
  ClkDuration      duQuery;            // Last query execution time
  /* -- Convert sql error id to string ------------------------------------- */
  const string &ResultToString(const int iCode)
  { // Reasons to delete or not delete the database
    static const IdList<SQLITE_NOTICE> ilResults{{
      "OK",       /*00-01*/ "ERROR",     "INTERNAL", /*02-03*/ "PERM",
      "ABORT",    /*04-05*/ "BUSY",      "LOCKED",   /*06-07*/ "NOMEM",
      "READONLY", /*08-09*/ "INTERRUPT", "IOERR",    /*10-11*/ "CORRUPT",
      "NOTFOUND", /*12-13*/ "FULL",      "CANTOPEN", /*14-15*/ "PROTOCOL",
      "EMPTY",    /*16-17*/ "SCHEMA",    "TOOBIG",   /*18-19*/ "CONSTRAINT",
      "MISMATCH", /*20-21*/ "MISUSE",    "NOLFS",    /*22-23*/ "AUTH",
      "FORMAT",   /*24-25*/ "RANGE",     "NOTADB"    /*26---*/
    }, "UNKNOWN"};
    // Return result
    return ilResults.Get(iCode);
  }
  /* -- Convert ADR id to string ------------------------------------------- */
  const string &ADResultToString(const ADResult adrResult)
  { // Reasons to delete or not delete the database
    static const IdList<ADR_MAX> ilADResults{{
      "no tables",             /* 0-1 */ "no records",
      "temporary database",    /* 2-3 */ "option denied",
      "error reading tables",  /* 4-5 */ "tables exist",
      "error reading records", /* 6-7 */ "records exist"
    }};
    // Return result
    return ilADResults.Get(adrResult);
  }
  /* -- Close the database ------------------------------------------------- */
  void DoClose(void)
  { // Number of retries needed to close the database
    unsigned int uiRetries;
    // Wait until the database can be closed
    for(uiRetries = 0; sqlite3_close(sqlDB) == SQLITE_BUSY; ++uiRetries)
    { // Keep trying to clean up if we can (this shouldn't really happen).
      Finalise();
      // Don't whore the CPU usage while waiting for async ops.
      TimerSuspend();
    } // Database handle no longer valid
    sqlDB = nullptr;
    // Say we closed the database
    cLog->LogExSafe(uiRetries > 0 ? LH_WARNING : LH_INFO,
      "Sql database '$' closed successfully ($ retries).",
        IdentGet(), uiRetries);
  }
  /* -- Build a new node into the a records list --------------------------- */
  void DoPair(Records &smbData, const int iType, const char*const cpKey,
    const void*const vpPtr, const size_t stSize)
  { // Generate the memory block with the specified data.
    Memory mData{ stSize, vpPtr };
    // Generate the pair value and move the memory into it.
    DataListItem dliItem{ std::move(mData), iType };
    // Insert a new pair moving key and value across.
    smbData.emplace(make_pair(cpKey, std::move(dliItem)));
  }
  /* -- Build a new node from an integral value ---------------------------- */
  template<typename T>void DoPair(Records &smbData, const int iType,
    const char*const cpKey, const T tVal)
      { DoPair(smbData, iType, cpKey, &tVal, sizeof(tVal)); }
  /* -- Set error code ----------------------------------------------------- */
  void SetError(const int iCode) { iError = iCode; }
  /* -- Compile the sql command and store output rows ---------------------- */
  void DoStep(sqlite3_stmt*const stmtData)
  { // Until we're done with all the data
    for(SetError(sqlite3_step(stmtData));
        IsErrorNotEqual(SQLITE_DONE);
        SetError(sqlite3_step(stmtData)))
    { // Return if an error in the row
      if(IsErrorNotEqual(SQLITE_ROW)) return;
      // Create key/memblock map and reserve entries
      Records smbData;
      // For each column, add to string/memblock map
      const int iColMax = sqlite3_column_count(stmtData);
      for(int iCol = 0; iCol < iColMax; ++iCol)
      { // What's the column type? Also save the value, we need it
        switch(const int iType = sqlite3_column_type(stmtData, iCol))
        { // 64-bit integer? Size the memory block and assign integer to it
          case SQLITE_INTEGER:
          { // Insert the int64 data into the vector
            DoPair(smbData, iType, sqlite3_column_name(stmtData, iCol),
              sqlite3_column_int64(stmtData, iCol));
            break;
          } // 64-bit IEEE float?
          case SQLITE_FLOAT:
          { // Insert the double into the vector
            DoPair(smbData, iType, sqlite3_column_name(stmtData, iCol),
              sqlite3_column_double(stmtData, iCol));
            break;
          } // Raw data?
          case SQLITE_BLOB:
          { // Copy blob to a memory block if there's something to copy
            DoPair(smbData, iType, sqlite3_column_name(stmtData, iCol),
              sqlite3_column_blob(stmtData, iCol),
              static_cast<size_t>(sqlite3_column_bytes(stmtData, iCol)));
            break;
          } // Text?
          case SQLITE_TEXT:
          { // Copy to a memory block if there is something to copy
            DoPair(smbData, iType, sqlite3_column_name(stmtData, iCol),
              sqlite3_column_text(stmtData, iCol),
              static_cast<size_t>(sqlite3_column_bytes(stmtData, iCol)));
            break;
          } // Unknown data type de-init and treat as nullptr
          default: continue;
        }
      } // Move key/values into records list if there were keys inserted
      if(!smbData.empty()) vKeys.emplace_back(std::move(smbData));
    } // Set error code to OK because it's set to SQLITE_DONE
    SetError(SQLITE_OK);
  }
  /* -- Can database be deleted, no point keeping if it's empty! ----------- */
  ADResult CanDatabaseBeDeleted(void)
  { // No if this is a temporary database as theres nothing to delete.
    if(FlagIsSet(SF_ISTEMPDB)) return ADR_ERR_TEMP_DB;
    // No if we are not allowed to delete the database
    if(FlagIsClear(SF_DELETEEMPTYDB)) return ADR_ERR_DENY_OPTION;
    // Compare how many tables there are in the database...
    switch(GetTableCount())
    { // Error occured? Don't delete the database!
      case string::npos: return ADR_ERR_LU_TABLE;
      // No tables? Delete the database!
      case 0: return ADR_OK_NO_TABLES;
      // Only one or two tables (key and/or cvars table)?
      case 1: case 2:
        // Get number of records in the cvars table
        switch(GetRecordCount("C"))
        { // Error occured? Don't delete the database!
          case string::npos: return ADR_ERR_LU_RECORD;
          // Zero records? Delete the database! There is always the
          // mandatory private key inside the cvars database so there is no
          // point keeping that if there is nothing else in the cvars database.
          case 0: return ADR_OK_NO_RECORDS;
          // More than one record? Do not delete the database
          default: return ADR_ERR_RECORDS_EXIST;
        } // Call should not continue after this switch statement
      // More than one table? Do not delete
      default: return ADR_ERR_TABLES_EXIST;
    } // Call should not continue after this switch statement
  }
  /* -- Send command to sql in raw format ---------------------------------- */
  void DoExecute(const string &strC, const CellList &vIn={})
  { // Reset previous results
    Reset();
    // Set query start time
    const ClockInterval<> tpStart;
    // Statement preparation
    sqlite3_stmt *stmtData = nullptr;
    SetError(sqlite3_prepare_v2(sqlDB, strC.c_str(),
      IntOrMax<int>(strC.length()), &stmtData, nullptr));
    // If succeeded then start parsing the input and ouput
    if(IsNoError())
    { // Free the statement context incase of exception
      typedef unique_ptr<sqlite3_stmt,
        function<decltype(sqlite3_finalize)>> SqliteStatementPtr;
      const SqliteStatementPtr sspPtr{ stmtData, sqlite3_finalize };
      // How many bind parameters per record?
      const int iMax = sqlite3_bind_parameter_count(stmtData);
      // Column id
      int iCol = 1;
      // Commit the statement
      bool bCommit = false;
      // For each memblock
      for(const Cell &mbIn : vIn)
      { // Commit the last statement? We can't put this at the end of the loop
        // because we don't want to call sqlite3_step twice in a row.
        if(bCommit)
        { // Compile the record
          DoStep(stmtData);
          if(IsError()) return;
          // Reset bindings so we can insert another
          SetError(sqlite3_reset(stmtData));
          if(IsError()) return;
          // Done commit
          bCommit = false;
        } // What is the data type
        switch(mbIn.iT)
        { // 64-bit integer?
          case SQLITE_INTEGER: SetError(sqlite3_bind_int64(stmtData,
            iCol, mbIn.qV)); break;
          // 64-bit IEEE float?
          case SQLITE_FLOAT: SetError(sqlite3_bind_double(stmtData,
            iCol, mbIn.fdV)); break;
          // Raw data?
          case SQLITE_BLOB: SetError(sqlite3_bind_blob(stmtData,
            iCol, mbIn.cpD, mbIn.iS, SQLITE_TRANSIENT)); break;
          // Text?
          case SQLITE_TEXT: SetError(sqlite3_bind_text(stmtData,
            iCol, mbIn.cpD, mbIn.iS, SQLITE_TRANSIENT)); break;
          // SQLITE_NULL or unknown data type? Treat as null
          default: SetError(sqlite3_bind_null(stmtData, iCol)); break;
        } // Return if bind failed
        if(IsError()) return;
        // If we still have columns to bind reloop and bind the next parameter
        if(iCol != iMax) ++iCol;
        // Commit the record on the next iteration, we don't do it now because
        // this might be the last parameter the user supplied and we don't want
        // to insert the same record twice.
        else { bCommit = true; iCol = 1; }
      } // Make sure we process the last record
      DoStep(stmtData);
    } // Get end query time to get total execution duration
    duQuery = tpStart.CIDelta();
  }
  /* -- Is sqlite database opened? --------------------------------- */ public:
  bool IsOpened(void) { return !!sqlDB; }
  /* -- Malloc functions for Lua (if wanted) ------------------------------- */
  static void MemFree(void*const vpPtr)
    { return sqlite3_free(vpPtr); }
  static void *MemReAlloc(void*const vpPtr, const size_t stSize)
    { return sqlite3_realloc64(vpPtr, stSize); }
  /* -- Heap used ---------------------------------------------------------- */
  size_t HeapUsed(void) const
    { return static_cast<size_t>(sqlite3_memory_used()); }
  /* -- Read all the parameters in the LUA stack into a vector ============= */
  const CellList ReadSQLParamsFromLua(lua_State*const lS)
  { // Input parameters
    CellList vIn;
    // For each subsequent parameter
    for(int iI = 2; !lua_isnone(lS, iI); ++iI)
    { // Get lua variable type and compare its type
      switch(const int iType = lua_type(lS, iI))
      { // Variable is a number?
        case LUA_TNUMBER:
        { // Variable is actually an integer?
          if(lua_isinteger(lS, iI)) vIn.emplace_back
            (Cell{ static_cast<sqlite_int64>(lua_tointeger(lS, iI)) });
          else vIn.emplace_back
            (Cell{ static_cast<double>(lua_tonumber(lS, iI)) });
          break;
        } // Variable is a string
        case LUA_TSTRING:
        { // Get string and push SQLITE_TEXT type
          size_t stS; const char*const cpS = lua_tolstring(lS, iI, &stS);
          vIn.emplace_back(Cell{ static_cast<int>(stS), cpS });
          break;
        } // Variable is a boolean
        case LUA_TBOOLEAN:
        { // Push as integer
          vIn.emplace_back(
            Cell{ static_cast<sqlite_int64>(lua_toboolean(lS, iI)) });
          break;
        } // Variable doesn't exist
        case LUA_TNIL: vIn.emplace_back(Cell{}); break;
        // Variable is userdata
        case LUA_TUSERDATA:
        { // Get reference to memory block and push data to list
          const DataConst &dcCref = *GetPtr<Asset>(lS, iI, "Asset");
          vIn.emplace_back(
            Cell{ dcCref.Ptr<char>(), static_cast<int>(dcCref.Size())} );
          break;
        } // Other variable (ignore)
        default: XC("Unsupported parameter type!",
          "Param", iI, "LuaType", iType, "Typename", lua_typename(lS, iType));
      }
    } // Return formatted array
    return vIn;
  }
  /* -- Execute a command from Lua ----------------------------------------- */
  int ExecuteFromLua(lua_State*const lS, const string &strC)
    { return Execute(strC, ReadSQLParamsFromLua(lS)); }
  /* -- Convert records to lua table --------------------------------------- */
  void RecordsToLuaTable(lua_State*const lS)
  { // Create the table, we're creating a indexed/value array
    PushTable(lS, vKeys.size());
    // Memory id
    lua_Integer liId = 1;
    // For each table item
    for(const Records &smData : vKeys)
    { // Table index
      PushInteger(lS, liId++);
      // Create the table, we're creating non-indexed key/value pairs
      PushTable(lS, 0, smData.size());
      // For each column data
      for(const auto &colData : smData)
      { // Get data list item
        const DataListItem &dliCData = colData.second;
        // Get type
        switch(dliCData.iT)
        { // Text?
          case SQLITE_TEXT:
            PushDataBlock(lS, dliCData);
            break;
          // 64-bit integer?
          case SQLITE_INTEGER:
            PushInteger(lS, dliCData.ReadInt<lua_Integer>());
            break;
          // 64-bit IEEE float?
          case SQLITE_FLOAT:
            PushNumber(lS, dliCData.ReadInt<lua_Number>());
            break;
          // Raw data? Save as array
          case SQLITE_BLOB:
          { // Create memory block array class
            Asset &aCref = *ClassCreate<Asset>(lS, "Asset");
            // Initialise the memory block depending on if we have data
            if(dliCData.NotEmpty())
              aCref.InitData(dliCData.Size(), dliCData.Ptr<void*>());
            else aCref.InitBlank();
            // Done
            break;
          } // No data or unknown? Push nothing
          case SQLITE_NULL: default: PushNil(lS); break;
        } // Push key name
        lua_setfield(lS, -2, colData.first.c_str());
      } // Push key pair as integer table
      lua_rawset(lS, -3);
    }
  }
  /* -- Modify delete empty database permission ---------------------------- */
  CVarReturn DeleteEmptyDBModified(const bool bState)
    { FlagSetOrClear(SF_DELETEEMPTYDB, bState); return ACCEPT; }
  /* -- sql_temp_store cvar was modified ----------------------------------- */
  CVarReturn TempStoreModified(const string &strF, string&)
    { return BoolToCVarReturn(strF.find(' ') == string::npos &&
      strF.find(';') == string::npos && Pragma("temp_store", strF)); }
  /* -- sql_synchronous cvar was modified ---------------------------------- */
  CVarReturn SynchronousModified(const bool bState)
    { return BoolToCVarReturn(PragmaOnOff("synchronous", bState) ||
        IsReadOnlyError()); }
  /* -- sql_journal_mode cvar was modified --------------------------------- */
  CVarReturn JournalModeModified(const bool bState)
    { return BoolToCVarReturn(PragmaOnOff("journal_mode", bState) ||
        IsReadOnlyError()); }
  /* -- sql_auto_vacuum cvar was modified ---------------------------------- */
  CVarReturn AutoVacuumModified(const bool bState)
    { return BoolToCVarReturn(PragmaOnOff("auto_vacuum", bState) ||
        IsReadOnlyError()); }
  /* -- sql_auto_vacuum cvar was modified ---------------------------------- */
  CVarReturn ForeignKeysModified(const bool bState)
    { return BoolToCVarReturn(PragmaOnOff("foreign_keys", bState) ||
        IsReadOnlyError()); }
  /* -- sql_inc_vacuum cvar was modified ----------------------------------- */
  CVarReturn IncVacuumModified(const uint64_t qwVal)
    { return BoolToCVarReturn(Pragma(Format("incremental_vacuum($)", qwVal)) ||
        IsReadOnlyError()); }
  /* -- sql_db cvar was modified ------------------------------------------- */
  CVarReturn UdbFileModified(const string &strF, string &strV)
  { // Save original working directory and restore it when leaving scope
    const DirSaver dsSaver;
    // If the user did not specify anything?
    if(strF.empty())
    { // Switch to executable directory
      if(!DirSetCWD(cSystem->ENGLoc())) return DENY;
      // Set database file with executable
      strV = Append(cSystem->ENGFile(), "." UDB_EXTENSION);
    } // If a memory database was requested? We allow it!
    else if(strF == strMemoryDBName) strV = strF;
    // If the user specified something?
    else
    { // Switch to original startup directory
      if(!DirSetCWD(cCmdLine->GetStartupCWD())) return DENY;
      // Use theirs, but force UDB extension
      strV = Append(strF, "." UDB_EXTENSION);
    } // Initialise the db and if succeeded?
    if(Init(strV))
    { // Set full path name of the database
      SQLINITOK: strV = std::move(PathSplit{ strV, true }.strFull);
      // Success
      return ACCEPT_HANDLED;
    } // If we have a persistant directory?
    if(cCmdLine->IsHome())
    { // Set a new filename in the users home directory
      strV = cCmdLine->GetHome(PathSplit{ strV }.strFileExt);
      // Try opening that and if succeeded then return success
      if(Init(strV)) goto SQLINITOK;
    } // Use memory database instead
    strV = strMemoryDBName;
    // Now open the memory database which should ALWAYS succeed.
    return Init(strV) ? ACCEPT_HANDLED : DENY;
  }
  /* -- Reset last sql result ---------------------------------------------- */
  void Reset(void)
  { // Clear error
    SetError(SQLITE_OK);
    // Clear last result data
    vKeys.clear();
    // Reset query time
    duQuery = seconds(0);
  }
  /* -- Dispatch stored transaction with logging --------------------------- */
  int Execute(const string &strC, const CellList &vIn={})
  { // Ignore if nothing to dispatch
    if(strC.empty()) return SQLITE_ERROR;
    // Do execution
    DoExecute(strC, vIn);
    // If log is in debug mode?
    if(cLog->HasLevel(LH_DEBUG))
    { // Write string
      cLog->LogNLCDebugExSafe(
        "Sql executed '$'<$>.\n"
        "- Args: $; Code: $<$>; RTT: $ sec.",
          strC, strC.length(), vIn.size(), ResultToString(GetError()),
          GetError(), TimeStr());
      // Write parameters
      for(size_t stIndex = 0; stIndex < vIn.size(); ++stIndex)
      { // Get cell data
        const Cell &cItem = vIn[stIndex];
        // Get type
        switch(cItem.iT)
        { // Type is an integer?
          case SQLITE_INTEGER:
            cLog->LogNLCDebugExSafe("- Arg #$<Int> = $ <$0x$>.",
              stIndex, cItem.qV, hex, cItem.qV); break;
          // Type is a float?
          case SQLITE_FLOAT:
            cLog->LogNLCDebugExSafe("- Arg #$<Float> = $.",
              stIndex, cItem.fdV); break;
          // Type is text?
          case SQLITE_TEXT:
            cLog->LogNLCDebugExSafe("- Arg #$<Text> = \"$\" ($ bytes).",
              stIndex, cItem.cpD, cItem.iS); break;
          // Type is data?
          case SQLITE_BLOB:
            cLog->LogNLCDebugExSafe("- Arg #$<Blob> = $ bytes.",
              stIndex, cItem.iS); break;
          // Unknown type?
          default:
            cLog->LogNLCDebugExSafe("- Param $<Unknown:$> = $ bytes.",
              stIndex, cItem.iT, cItem.iS); break;
        }
      }
    } // Return error status
    return iError;
  }
  /* -- Dispatch stored transaction with logging but return success bool -- */
  bool ExecuteAndSuccess(const string &strC, const CellList &vIn={})
    { return Execute(strC, vIn) == SQLITE_OK; }
  /* -- Check integrity ---------------------------------------------------- */
  bool CheckIntegrity(void)
  { // Do check (We need a result so dont use Pragma())
    if(Execute("PRAGMA integrity_check(1)"))
    { // Log and return failure
      cLog->LogErrorExSafe(
        "Sql failed integrity check failed because $ ($<$>)!",
        GetErrorStr(), GetErrorAsIdString(), GetError());
      return false;
    } // This should never be true but just incase
    if(vKeys.empty())
    { // Log and return failure
      cLog->LogErrorSafe("Sql integrity check failed to return result row.");
      return false;
    } // Get reference to the map of memory blocks. It shouldn't be empty
    const Records &mbMap = *vKeys.cbegin();
    if(mbMap.empty())
    { // Log and return failure
      cLog->LogErrorSafe(
        "Sql integrity check failed to return result columns.");
      return false;
    } // Get result string. It should say 'ok' if everything went ok
    const string strResult{ mbMap.cbegin()->second.ToString() };
    if(strResult != "ok")
    { // Log and return failure
      cLog->LogErrorExSafe("Sql database corrupted: $", strResult);
      return false;
    } // Passed so return success
    cLog->LogInfoSafe("Sql integrity check passed.");
    return true;
  }
  /* -- Set a pragma ------------------------------------------------------- */
  bool Pragma(const string &strVar)
  { // Execute without value
    if(Execute(Append("PRAGMA ", strVar)))
    { // Log and return failure
      cLog->LogErrorExSafe("Sql pragma '$' failed because $ ($<$>)!",
        strVar, GetErrorStr(), GetErrorAsIdString(), GetError());
      return false;
    } // Log and return success
    cLog->LogDebugExSafe("Sql pragma '$' succeeded.", strVar);
    return true;
  }
  /* -- Set a pragma on or off --------------------------------------------- */
  bool PragmaOnOff(const string &strVar, const bool bState)
    { return Pragma(strVar, bState ? "ON" : "OFF"); }
  /* -- Set a pragma ------------------------------------------------------- */
  bool Pragma(const string &strVar, const string &strVal)
  { // No variable? Return failure
    if(strVar.empty()) return false;
    // No value? Set as single line pragma
    if(strVal.empty()) return Pragma(strVar);
    // Execute with value
    if(Execute(Format("PRAGMA $=$", strVar, strVal)))
    { // Log and return failure
      cLog->LogErrorExSafe(
        "Sql set pragma '$' to '$' failed because $ ($<$>)!",
        strVar, strVal, GetErrorStr(), GetErrorAsIdString(), GetError());
      return false;
    } // Log and return success
    cLog->LogDebugExSafe("Sql set pragma '$' to '$' succeeded.",
      strVar, strVal);
    return true;
  }
  /* -- Get size of database ----------------------------------------------- */
  uint64_t Size(void)
  { // Get the database page size
    if(Execute("pragma page_size") || vKeys.empty())
      return numeric_limits<uint64_t>::max();
    // Get reference to keys list
    const Records &smmPageSizePairs = *vKeys.cbegin();
    if(smmPageSizePairs.size() != 1) return numeric_limits<uint64_t>::max();
    // Get first item
    const RecordsIt smmPageSizeItem{ smmPageSizePairs.cbegin() };
    if(smmPageSizeItem->first != "page_size")
      return numeric_limits<uint64_t>::max();
    // Return number of tables
    const uint64_t qPageSize =
      smmPageSizeItem->second.ReadInt<uint64_t>();
    // Get the database page count
    if(Execute("pragma page_count") || vKeys.empty())
      return numeric_limits<uint64_t>::max();
    // Get reference to keys list
    const Records &smmPageCountPairs = *vKeys.cbegin();
    if(smmPageCountPairs.size() != 1) return numeric_limits<uint64_t>::max();
    // Get first item
    const RecordsIt smmPageCountItem{ smmPageCountPairs.cbegin() };
    if(smmPageCountItem->first != "page_count")
      return numeric_limits<uint64_t>::max();
    // Return number of tables
    const uint64_t qPageCount =
      smmPageCountItem->second.ReadInt<uint64_t>();
    // Return result
    return qPageSize * qPageCount;
  }
  /* -- Return error string ------------------------------------------------ */
  const char *GetErrorStr(void) const { return sqlite3_errmsg(sqlDB); }
  /* -- Return error code -------------------------------------------------- */
  int GetError(void) const { return iError; }
  bool IsErrorEqual(const int iWhat) const { return GetError() == iWhat; };
  bool IsErrorNotEqual(const int iWhat) const { return !IsErrorEqual(iWhat); };
  bool IsError(void) const { return IsErrorNotEqual(SQLITE_OK); }
  bool IsNoError(void) const { return !IsError(); }
  bool IsReadOnlyError(void) const { return IsErrorEqual(SQLITE_READONLY); }
  const string &GetErrorAsIdString(void) { return ResultToString(iError); }
  /* -- Return duration of last query -------------------------------------- */
  double Time(void) const { return ClockDurationToDouble(duQuery); }
  /* -- Return formatted query time ---------------------------------------- */
  const string TimeStr(void) const { return ToShortDuration(Time()); }
  /* -- Returns if sql is in a transaction --------------------------------- */
  bool Active(void) const { return !sqlite3_get_autocommit(sqlDB); }
  /* -- Return string map of records --------------------------------------- */
  Result &GetRecords(void) { return vKeys; }
  /* -- Useful aliases ----------------------------------------------------- */
  int Begin(void)
    { return Execute("BEGIN TRANSACTION"); }
  int End(void)
    { return Execute("END TRANSACTION"); }
  int DropTable(const string &strT)
    { return Execute(Append("DROP table ", strT)); }
  int FlushTable(const string &strT)
    { return Execute(Append("DELETE from ", strT)); }
  int Optimise(void)
    { return Execute("VACUUM"); }
  int Affected(void)
    { return sqlite3_changes(sqlDB); }
  /* -- Process a count(*) requested --------------------------------------- */
  size_t GetRecordCount(const string &strWhat)
  { // Do a table count lookup. If succeeded and have records?
    if(Execute(Append("SELECT count(*) FROM ", strWhat)) == SQLITE_OK &&
      !vKeys.empty())
    { // Get reference to keys list and if we have one result?
      const Records &smmPairs = *vKeys.cbegin();
      if(smmPairs.size() == 1)
      { // Get first item and if it's the count?
        const RecordsIt smmItem{ smmPairs.cbegin() };
        if(smmItem->first == "count(*)")
        { // Get result because we're going to clean up after
          const size_t stCount = smmItem->second.ReadInt<size_t>();
          // Clean up downloaded records
          Reset();
          // Return number of tables
          return stCount;
        } // Count(*) not found
      } // Result does not have one record
    } // Failed to execute query or no records, return failure
    return string::npos;
  }
  /* -- Get number of tables in database ----------------------------------- */
  size_t GetTableCount(void)
    { return GetRecordCount("sqlite_master AS tables WHERE type='table'"); }
  /* -- Flush all orphan statements ---------------------------------------- */
  size_t Finalise(void) const
  { // Number of orphan transactions
    size_t stOrphan = 0;
    // Finalise all remaining statements until there are no more left
    while(sqlite3_stmt*const stmtData = sqlite3_next_stmt(sqlDB, nullptr))
      { sqlite3_finalize(stmtData); ++stOrphan; }
    // Return number of orphans
    return stOrphan;
  }
  /* ----------------------------------------------------------------------- */
  bool CVarReadKeys(void)
  { // Read all variable names and if failed?
    if(Execute("SELECT K from C"))
    { // Put in log and return nothing loaded
      cLog->LogWarningExSafe(
        "CVars failed to fetch cvar key names because $ ($)!",
        GetErrorStr(), GetError());
      return false;
    } // Write number of records read and return success
    cLog->LogDebugExSafe("Sql read $ key names from CVars table.",
      GetRecords().size());
    return true;
  }
  /* ----------------------------------------------------------------------- */
  bool CVarReadAll(void)
  { // Read all variables and if failed?
    if(Execute("SELECT K,F,V from C"))
    { // Put in log and return nothing loaded
      cLog->LogWarningExSafe("Sql failed to read CVars table because $ ($)!",
        GetErrorStr(), GetError());
      return false;
    } // Write number of records read and return success
    cLog->LogDebugExSafe("Sql read $ records from CVars table.",
      GetRecords().size());
    return true;
  }
  /* ----------------------------------------------------------------------- */
  CreateTableResult CVarDropTable(void)
  { // If table exists return that it already exists
    if(GetRecordCount("C") == string::npos) return CTR_OK_ALREADY;
    // Drop the SQL table and if failed?
    if(DropTable("C"))
    { // Write error in console and return failure
      cLog->LogWarningExSafe(
        "Sql failed to destroy CVars table because $ ($)!",
        GetErrorStr(), GetError());
      return CTR_FAIL;
    } // Write success in console and return success
    cLog->LogDebugExSafe("Sql destroyed CVars table successfully.");
    return CTR_OK;
  }
  /* ----------------------------------------------------------------------- */
  CreateTableResult CVarCreateTable(void)
  { // If table already exists then return success already
    if(GetRecordCount("C") != string::npos) return CTR_OK_ALREADY;
    // Create the SQL table for our settings if it does not exist
    if(Execute("CREATE table C"        // Table name
      "(K TEXT UNIQUE NOT NULL,"       // Variable name
       "F INTEGER DEFAULT 0,"          // Value flags (crypt,comp,etc.)
       "V TEXT)"))                     // Value (any type allowed)
    { // Write error in console and return failure
      cLog->LogWarningExSafe("Sql failed to create CVars table because $ ($)!",
        GetErrorStr(), GetError());
      return CTR_FAIL;
    } // Write success in console and return success
    cLog->LogDebugExSafe("Sql created CVars table successfully.");
    return CTR_OK;
  }
  /* ----------------------------------------------------------------------- */
  CreateTableResult LuaCacheDropTable(void)
  { // If table exists return that it already exists
    if(GetRecordCount("L") == string::npos) return CTR_OK_ALREADY;
    // Drop the SQL table and if failed?
    if(DropTable("L"))
    { // Write error in console and return failure
      cLog->LogWarningExSafe(
        "Sql failed to destroy cache table because $ ($)!",
        GetErrorStr(), GetError());
      return CTR_FAIL;
    } // Write success in console and return success
    cLog->LogDebugExSafe("Sql destroyed cache table successfully.");
    return CTR_OK;
  }
  /* ----------------------------------------------------------------------- */
  CreateTableResult LuaCacheCreateTable(void)
  { // If table already exists then return success already
    if(GetRecordCount("L") != string::npos) return CTR_OK_ALREADY;
    // Create the SQL table for our settings if it does not exist
    if(Execute("CREATE table L"        // Table name
      "(C INTEGER UNIQUE NOT NULL,"    // Code CRC value
       "T INTEGER NOT NULL,"           // Code update timestamp
       "R TEXT UNIQUE NOT NULL,"       // Code eference
       "D TEXT NOT NULL)"))            // Code binary
    { // Write error in console and return failure
      cLog->LogWarningExSafe("Sql failed to create cache table because $ ($)!",
        GetErrorStr(), GetError());
      return CTR_FAIL;
    } // Write success in console and return success
    cLog->LogDebugExSafe("Sql created cache table successfully.");
    return CTR_OK;
  }
  /* ----------------------------------------------------------------------- */
  bool CVarCommitData(const string &strVar,
    const SqlCVarDataFlagsConst &lfType, const int iType,
    const char*const cpData, const size_t stLength)
  { // Try to write the specified cvar and if failed?
    if(Execute("INSERT or REPLACE into C(K,F,V) VALUES(?,?,?)", {
      Cell{ strVar }, Cell{ lfType.FlagGet<int>() },
      Cell{ cpData, IntOrMax<int>(stLength), iType }
    }))
    { // Log the warning and return failure
      cLog->LogWarningExSafe("Sql failed to commit CVar '$' "
                     "(T:$;ST:$;B:$) because $ ($)!",
        strVar, lfType.FlagGet(), iType, stLength, GetErrorStr(), GetError());
      return false;
    } // Report and return success
    cLog->LogDebugExSafe("Sql commited CVar '$' (T:$;ST:$;B:$).",
      strVar, lfType.FlagGet(), iType, stLength);
    return true;
  }
  /* ----------------------------------------------------------------------- */
  bool CVarCommitString(const string &strVar, const string &strVal)
    { return CVarCommitData(strVar, LF_NONE,
        SQLITE_TEXT, strVal.data(), strVal.length()); }
  /* ----------------------------------------------------------------------- */
  bool CVarCommitBlob(const string &strVar, const DataConst &dcVal)
    { return CVarCommitData(strVar, LF_ENCRYPTED,
        SQLITE_BLOB, dcVal.Ptr<char>(), dcVal.Size()); }
  /* ----------------------------------------------------------------------- */
  PurgeResult CVarPurgeData(const char*const cpKey, const size_t stKey)
  { // Try to purge the cvar from the database and if it failed?
    if(Execute("DELETE from C WHERE K=?",
      { Cell{ IntOrMax<int>(stKey), cpKey } }))
    { // Log the warning and return failure
      cLog->LogWarningExSafe("Sql failed to purge CVar '$' because $ ($)!",
        cpKey, GetErrorStr(), GetError());
      return PR_FAIL;
    } // Just return if no records were affected
    if(!Affected()) return PR_OK_NC;
    // Report and return success
    cLog->LogDebugExSafe("Sql purged CVar '$' from database.", cpKey);
    return PR_OK;
  }
  /* ----------------------------------------------------------------------- */
  PurgeResult CVarPurge(const string &strVar)
    { return CVarPurgeData(strVar.c_str(), strVar.length()); }
  /* -- DeInit ------------------------------------------------------------- */
  void DeInit(void)
  { // Ignore if no handle to deinit
    if(!sqlDB) return;
    // Log deinitialisation
    cLog->LogDebugExSafe("Sql database '$' is closing...", IdentGet());
    // Finalise statements and if we found orphans
    if(const size_t stOrphans = Finalise())
    { // Report how many statements unfinished
      cLog->LogWarningExSafe("Sql finalised $ orphan statements.", stOrphans);
    } // Check if database should be deleted, if it should'nt?
    switch(const ADResult adrResult = CanDatabaseBeDeleted())
    { // Deletion is ok?
      case ADR_OK_NO_TABLES:
      case ADR_OK_NO_RECORDS:
        // Print the result
        cLog->LogDebugExSafe("Sql will delete empty database because $ ($).",
          ADResultToString(adrResult), adrResult);
        // Close the database
        DoClose();
        // Try to delete it and if we could not delete it?
        if(!DirFileUnlink(IdentGet()))
        { // Log the reason as a warning
          cLog->LogWarningExSafe(
            "Sql empty database file '$' could not be deleted! $",
            IdentGet(), LocalError());
        } // We deleted it all right?
        else
        { // Log that we deleted the file
          cLog->LogDebugExSafe("Sql empty database file '$' deleted.",
            IdentGet());
        } // Done
        break;
      // Deletion is not ok?
      default:
        // Print the result
        cLog->LogDebugExSafe("Sql won't delete database because $ ($).",
          ADResultToString(adrResult), adrResult);
        // Sqlite docs recommend us to optimise before closing.
        Pragma("optimize");
        // Close the database
        DoClose();
        // Done
        break;
    } // Reset results
    Reset();
    // Reset database name
    IdentClear();
    // Temporary database boolean
    FlagClear(SF_ISTEMPDB);
    // Clear private key
    cCrypt->SetDefaultPrivateKey();
  }
  /* -- Create new private key --------------------------------------------- */
  bool CreatePrivateKey(void)
  { // Generate a new private key
    cCrypt->ResetPrivateKey();
    // Try to drop the original private key table
    if(DropTable("K"))
      cLog->LogWarningExSafe("Sql failed to drop key table because $ ($<$>)!",
        GetErrorStr(), GetErrorAsIdString(), GetError());
    // Now try to create the table that holds the private key and if failed?
    if(Execute("CREATE table K(I INTEGER NOT NULL,"
                              "K INTEGER NOT NULL)"))
    { // Log failure and return
      cLog->LogErrorExSafe("Sql failed to create key table because $ ($<$>)!",
        GetErrorStr(), GetErrorAsIdString(), GetError());
      return false;
    } // Prepare statement to insert values into sql
    const string strInsert{ "INSERT into K(I,K) VALUES(?,?)" };
    // For each key part to write
    for(size_t stIndex = 0; stIndex < PK_TOTAL_COUNT; ++stIndex)
    { // Get private key value and if failed?
      const sqlite3_int64 qVal =
        static_cast<sqlite3_int64>(cCrypt->ReadPrivateKey(stIndex));
      if(Execute(strInsert,
          { Cell{ static_cast<sqlite3_int64>(stIndex) }, Cell{ qVal } }))
      { // Log failure and return failure
        cLog->LogErrorExSafe(
          "Sql failed to write key table at $ because $ ($<$>)!",
          stIndex, GetErrorStr(), GetErrorAsIdString(), GetError());
        return false;
      }
    } // Log result and return success
    cLog->LogDebugExSafe(
      "Sql wrote a new key table into database successfully!");
    return true;
  }
  /* -- Load private key --------------------------------------------------- */
  void LoadPrivateKey(void)
  { // Check table count
    switch(const size_t stCount = GetRecordCount("K"))
    { // Error reading table?
      case string::npos:
      { // Log failure and create a new private key
        cLog->LogWarningExSafe(
          "Sql failed to read key table because $ ($<$>)!",
          GetErrorStr(), GetErrorAsIdString(), GetError());
        goto NewKey;
      } // Any other value is not allowed
      default:
      { // Log failure and create a new private key
        cLog->LogWarningExSafe("Sql key table corrupt ($ != $)!",
          stCount, PK_TOTAL_COUNT);
        goto NewKey;
      } // Read enough entries?
      case PK_TOTAL_COUNT:
      { // Read keys and if failed?
        if(Execute("SELECT K from K ORDER BY I ASC"))
        { // Log failure and create a new private key
          cLog->LogWarningExSafe(
            "Sql failed to read key table because $ ($<$>)!",
            GetErrorStr(), GetErrorAsIdString(), GetError());
          goto NewKey;
        } // If not enough results?
        if(vKeys.size() != PK_TOTAL_COUNT)
        { // Log failure and create a new private key
          cLog->LogWarningExSafe(
            "Sql read only $ of $ key table records!",
            vKeys.size(), PK_TOTAL_COUNT);
          goto NewKey;
        } // Record index number
        size_t stIndex = 0;
        // Update the private key parts
        for(const Records &rItem : vKeys)
        { // Get reference to data item and if it's not the right type?
          const DataListItem &dliItem = rItem.cbegin()->second;
          if(dliItem.iT != SQLITE_INTEGER)
          { // Log failure and create new private key table
            cLog->LogErrorExSafe(
              "Sql key table at column $ not correct type ($)!",
              stIndex, dliItem.iT);
            goto NewKey;
          } // Read record data. It must be 8 bytes. If it isnt?
          if(dliItem.Size() != sizeof(sqlite3_int64))
          { // Log failure and create new private key table
            cLog->LogErrorExSafe(
              "Sql key table at column $ expected $ not $ bytes!",
              stIndex, sizeof(sqlite3_int64), dliItem.Size());
            goto NewKey;
          } // Read in the value to the private key table
          cCrypt->WritePrivateKey(stIndex++,
            static_cast<uint64_t>(dliItem.ReadInt<sqlite3_int64>()));
        } // Log result and return
        cLog->LogDebugExSafe(
          "Sql loaded key table from database successfully!");
        return;
      }
    } // Could not read new private key so setup new private key
    NewKey: CreatePrivateKey();
  }
  /* -- Init --------------------------------------------------------------- */
  bool Init(const string &_strName)
  { // If named database is already opened
    if(sqlDB && _strName == IdentGet())
    { // Put in console and return failure
      cLog->LogWarningExSafe("Sql skipped re-init of '$'.", _strName);
      return false;
    } // Log initialisation. Set filename using memory db name if empty
    const string &strNTmp = _strName.empty() ? strMemoryDBName : _strName;
    cLog->LogDebugExSafe("Sql initialising database '$'...", strNTmp);
    // Open database with a temporary sqlite handle so if the open fails,
    // the old one stays intact
    sqlite3 *sqlDBtemp = nullptr;
    if(const int iCode = sqlite3_open_v2(strNTmp.c_str(), &sqlDBtemp,
      SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE | SQLITE_OPEN_FULLMUTEX |
      SQLITE_OPEN_SHAREDCACHE, nullptr))
    { // If error log result otherwise integrity check failed
      cLog->LogErrorExSafe("Sql could not open '$' because $ ($)!", strNTmp,
        GetErrorStr(), iCode);
      // Failure
      return false;
    } // Set to this database and set name
    sqlDB = sqlDBtemp;
    IdentSet(std::move(strNTmp));
    // Load private key
    LoadPrivateKey();
    // Initialised OK!
    cLog->LogInfoExSafe("Sql database '$' initialised.", IdentGet());
    // Success
    return true;
  }
  /* -- Constructor -------------------------------------------------------- */
  Sql(void) :                          // No parameters
    /* -- Initialisation of members ---------------------------------------- */
    SqlFlags(SF_NONE),                 // No sql flags (loaded externally)
    sqlDB(nullptr),                    // No sql database handle yet
    iError(sqlite3_initialize()),      // Initialise sqlite and store error
    strMemoryDBName{ ":memory:" }      // Create a memory database by default
    /* -- Code ------------------------------------------------------------- */
    { // Throw error if sqlite startup failed
      if(IsError()) XC("Failed to initialise SQLite!",
                       "Error", GetError(), "Reason", GetErrorAsIdString());
    }
  /* -- Destructor --------------------------------------------------------- */
  DTORHELPERBEGIN(~Sql) DeInit(); sqlite3_shutdown(); DTORHELPEREND(Sql);
  /* ----------------------------------------------------------------------- */
  DELETECOPYCTORS(Sql);                // Do not need defaults
  /* -- End ---------------------------------------------------------------- */
} *cSql = nullptr;                     // Pointer to static class
/* ------------------------------------------------------------------------- */
};                                     // End of module namespace
/* == EoF =========================================================== EoF == */

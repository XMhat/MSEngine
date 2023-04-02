/* == JSON.HPP ============================================================= */
/* ######################################################################### */
/* ## MS-ENGINE              Copyright (c) MS-Design, All Rights Reserved ## */
/* ######################################################################### */
/* ## This module defines a class that allows JSON encoding or decoding   ## */
/* ## to performed.                                                       ## */
/* ######################################################################### */
/* ========================================================================= */
#pragma once                           // Only one incursion allowed
/* ------------------------------------------------------------------------- */
namespace IfJson {                     // Start of module namespace
/* -- Includes ------------------------------------------------------------- */
using namespace Library::RapidJson;    // Using rapidjson library functions
using namespace IfAsset;               // Using asset namespace
/* == Json object collector and member class =============================== */
BEGIN_ASYNCCOLLECTORDUO(Jsons, Json, CLHelperUnsafe, ICHelperUnsafe),
  /* -- Base classes ------------------------------------------------------- */
  public AsyncLoader<Json>,            // Asynchronous loading of Json object
  public Lockable,                     // Lua garbage collector instruction
  private Document                     // RapidJson document class
{ /* -- Build a json string from lua string ----------------------- */ private:
  Value ToStr(lua_State*const lS, const int iId)
  { // Get string and length from LUA
    size_t stStr; const char*const cpStr = lua_tolstring(lS, iId, &stStr);
    // Return as a json string. Unfortunately, ALL strings from LUA are
    // volatile so we need to copy the string.
    return { cpStr, static_cast<SizeType>(stStr), GetAllocator() };
  }
  /* -- Handle type value -------------------------------------------------- */
  template<typename Value>
    static void ProcessValueType(lua_State*const lS, const Value &vValue)
  { // What type is the value?
    switch(vValue.GetType())
    { // Json entry is a number type?
      case kNumberType:
        // Actually an integer or a number type?
        if(vValue.IsInt()) PushInteger(lS, vValue.GetInt());
        else PushNumber(lS, vValue.GetDouble());
        break;
      // Json entry is a string type?
      case kStringType:
        PushLString(lS, vValue.GetString(), vValue.GetStringLength());
        break;
      // Json entry is a boolean type?
      case kTrueType:
        PushBoolean(lS, true);
        break;
      case kFalseType:
        PushBoolean(lS, false);
        break;
      // Json entry is an array[] type?
      case kArrayType:
        if(GetStackCount(lS)) ToTableArray(lS, vValue);
        break;
      // Json entry is an object{} type?
      case kObjectType:
        ToTableObject(lS, vValue);
        break;
      // Json entry is a null type?
      case kNullType:
      // Unknown type?
      default:
        PushNil(lS);
        break;
    }
  }
  /* -- Convert LUA table to rapidjson::Value ---------------------- */ public:
  Value ParseTable(lua_State*const lS, const int iId, const int iObjId)
  { // Check table
    CheckTable(lS, iId, "Data");
    // Test: lexec Console.Write(Json.Table({ }):ToString());
    // Get size of table and if we have length then we need to create an array
    if(const lua_Integer liLen =
      IntOrMax<lua_Integer>(LuaUtilGetSize(lS, iId)))
    {  // Set this value is array
      Value rjvRoot{ kArrayType };
      // We need one more free item on the stack, leave empty if not
      if(!StackHasCapacity(lS, IntOrMax<int>(liLen))) return rjvRoot;
      // Until end of table
      for(lua_Integer lI = 1; lI <= liLen; ++lI)
      { // Get first item
        GetReferenceEx(lS, iId, lI);
        // Append value if a string. Lua will convert any valid numbered
        // string to a number if this is not checked before integral checks.
        // Test with: lexec Console.Write(Json.Table({1,2,'3'}):ToHRString());
        switch(lua_type(lS, -1))
        { // Variable is a number
          case LUA_TNUMBER:
            if(lua_isinteger(lS, -1))
              rjvRoot.PushBack(Value().SetInt64(lua_tointeger(lS, -1)),
                GetAllocator());
            else rjvRoot.PushBack(lua_tonumber(lS, -1), GetAllocator());
            break;
          // Variable is a boolean
          case LUA_TBOOLEAN: rjvRoot.PushBack(lua_toboolean(lS, -1),
            GetAllocator()); break;
          // Variable is a table
          case LUA_TTABLE: rjvRoot.PushBack(ParseTable(lS, -1, -2),
            GetAllocator()); break;
          // Unknown or a string, just add a string
          case LUA_TSTRING:
          default: rjvRoot.PushBack(ToStr(lS, -1), GetAllocator()); break;
        } // Remove the last item
        RemoveStack(lS);
      } // Return new object
      return rjvRoot;
    } // Set this value as object
    Value rjvRoot{ kObjectType };
    // Save stack position so it can be restored on completion or exception
    const LuaStackSaver lSS{ lS };
    // We need two more free item on the stack, leave empty if not
    if(!StackHasCapacity(lS, 2)) return rjvRoot;
    // Push nil value
    // Walk through all the object members
    for(PushNil(lS); lua_next(lS, iObjId); RemoveStack(lS))
    { // Get keyname
      Value vKey{ ToStr(lS, -2) };
      // Set the key->value for the LUA variable
      switch(lua_type(lS, -1))
      { // Variable is a number
        case LUA_TNUMBER:
          if(lua_isinteger(lS, -1))
            rjvRoot.AddMember(vKey, Value().SetInt64(lua_tointeger(lS, -1)),
              GetAllocator());
          else
            rjvRoot.AddMember(vKey, lua_tonumber(lS, -1), GetAllocator());
          break;
        // Variable is a boolean
        case LUA_TBOOLEAN:
          rjvRoot.AddMember(vKey, lua_toboolean(lS, -1), GetAllocator());
          break;
        // Variable is a table
        case LUA_TTABLE: rjvRoot.AddMember(vKey,
          ParseTable(lS, -1, -2), GetAllocator()); break;
        // Unknown or a string, just add as string
        case LUA_TSTRING:
        default: rjvRoot.AddMember(vKey, ToStr(lS, -1), GetAllocator());
          break;
      }
    } // Return new object
    return rjvRoot;
  }
  /* -- Convert json value to lua object table and put it on stack --------- */
  static void ToTableObject(lua_State*const lS, const Value &rjvVal)
  { // Create the table, we're creating non-indexed key/value pairs
    PushTable(lS, 0, rjvVal.MemberCount());
    // We need one more free item on the stack, leave empty if not
    if(!StackHasCapacity(lS, 1)) return;
    // For each table item
    for(const auto &rjvRef : rjvVal.GetObject())
    { // What type is the value?
      ProcessValueType(lS, rjvRef.value);
      // Push key name
      lua_setfield(lS, -2, rjvRef.name.GetString());
    }
  }
  /* -- Convert json value to lua array table and put it on stack ---------- */
  static void ToTableArray(lua_State*const lS, const Value &rjvVal)
  { // Create the table, we're creating a indexed/value array
    PushTable(lS, rjvVal.Size());
    // We need two more free items on the stack, leave empty if not
    if(rjvVal.Empty() || !StackHasCapacity(lS, 2)) return;
    // Index id
    lua_Integer liId = 0;
    // For each table item
    for(const auto &rjvRef : rjvVal.GetArray())
    { // Table index
      PushInteger(lS, ++liId);
      // What type is the value?
      ProcessValueType(lS, rjvRef);
      // Push key pair as integer table
      lua_rawset(lS, -3);
    }
  }
  /* -- Convert json value to lua table and put it on stack ---------------- */
  void ToLuaTable(lua_State*const lS)
  { // Get value
    const Value &rjvVal =
      reinterpret_cast<const Value&>(static_cast<const Document&>(*this));
     // What type is the value?
    switch(rjvVal.GetType())
    { // Indexed array
      case kArrayType: ToTableArray(lS, rjvVal); break;
      // Key/value object
      case kObjectType: ToTableObject(lS, rjvVal); break;
      // Unacceptable
      default: XC("Json value not array or object!",
        "Type", rjvVal.GetType());
    }
  }
  /* ----------------------------------------------------------------------- */
  typedef Writer<StringBuffer, UTF8<>, UTF8<>> RJCompactWriter;
  typedef PrettyWriter<StringBuffer, UTF8<>, UTF8<>> RJPrettyWriter;
  /* ----------------------------------------------------------------------- */
  template<typename WriterType>const string ToString(void) const
  { // Output buffer
    StringBuffer rsbOut;
    WriterType rwWriter{ rsbOut };
    Accept(rwWriter);
    return { rsbOut.GetString(), rsbOut.GetSize() };
  }
  /* ----------------------------------------------------------------------- */
  template<typename T>int ToFile(const string &strFile) const
  { // Create output file and if succeeded?
    if(FStream fsOut{ strFile, FStream::FM_W_T })
    { // Write the output file
      fsOut.FStreamWriteString(ToString<T>());
      // Success
      return 0;
    } // Create output file failed so return reason for failure
    else return fsOut.FStreamFError();
  }
  /* ----------------------------------------------------------------------- */
  void AsyncReady(FileMap &fC)
  { // Parse the string and return if succeeded
    const ParseResult prData{ Parse<0>(fC.Ptr<char>(), fC.Size()) };
    if(!prData) XC(GetParseError_En(prData.Code()),
      "Identifier", fC.IdentGet(), "Size", fC.Size(),
      "JsonSize",   prData.Offset());
  }
  /* -- Load json from file asynchronously --------------------------------- */
  void InitAsyncFile(lua_State*const lS)
  { // Must have 4 parameters (including the class pointer)
    CheckParams(lS, 5);
    // Check and get parameters
    const string strName{ GetCppFileName(lS, 1, "File") };
    CheckFunction(lS, 2, "ErrorFunc");
    CheckFunction(lS, 3, "ProgressFunc");
    CheckFunction(lS, 4, "SuccessFunc");
    // Init the specified string as a file asynchronously
    AsyncInitFile(lS, strName, "jsonfile");
  }
  /* -- Load json from string asynchronously ------------------------------- */
  void InitAsyncString(lua_State*const lS)
  { // Must have 5 parameters (including the class pointer)
    CheckParams(lS, 6);
    // Check and get parameters
    const string strN{ GetCppStringNE(lS, 1, "Identifier") };
    Memory mData{ GetMBfromLString(lS, 2, "Code") };
    CheckFunction(lS, 3, "ErrorFunc");
    CheckFunction(lS, 4, "ProgressFunc");
    CheckFunction(lS, 5, "SuccessFunc");
    // Init the specified string as an array asynchronously
    AsyncInitArray(lS, strN, "jsonstring", StdMove(mData));
  }
  /* -- Init from LUA string ----------------------------------------------- */
  void InitString(lua_State*const lS)
  { // Need three parameters...
    //   1: identifier
    //   2: string to convert
    //   3: the userdata object that contains a pointer to this class
    CheckParams(lS, 3);
    // Get name and code to parse
    IdentSet(GetCppString(lS, 1, "Identifier"));
    // Init file as array
    SyncInitArray(IdentGet(), GetMBfromLString(lS, 2, "String"));
  }
  /* -- Parse the table into a value --------------------------------------- */
  void InitFromTable(lua_State*const lS) { ParseTable(lS, 1, 1).Swap(*this); }
  /* ----------------------------------------------------------------------- */
  ~Json(void) { AsyncCancel(); }
  /* ----------------------------------------------------------------------- */
  Json(void) :
    /* -- Initialisers ----------------------------------------------------- */
    ICHelperJson(*cJsons),
    IdentCSlave{ cParent.CtrNext() },  // Initialise identification number
    AsyncLoader<Json>{ this, EMC_MP_JSON }
    /* -- No code ---------------------------------------------------------- */
    { }
  /* ----------------------------------------------------------------------- */
  DELETECOPYCTORS(Json)                // Disable copy constructor and operator
};/* -- End ---------------------------------------------------------------- */
END_ASYNCCOLLECTOR(Jsons, Json, JSON)
/* ------------------------------------------------------------------------- */
};                                     // End of module namespace
/* == EoF =========================================================== EoF == */

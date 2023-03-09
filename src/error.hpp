/* == ERROR.HPP ============================================================ */
/* ######################################################################### */
/* ## MS-ENGINE              Copyright (c) MS-Design, All Rights Reserved ## */
/* ######################################################################### */
/* ## This is a very useful class to handle errors and display useful     ## */
/* ## data to help debug the error. It uses the C++11 variadic templates  ## */
/* ## Use the XC() macro as a helper to build an error message with your  ## */
/* ## chosen parameters to debug...                                       ## */
/* ######################################################################### */
/* ## Usage: XC(Reason, [VarName, VarValue[, ...]])                       ## */
/* ######################################################################### */
/* ## []       ## Denotes an optional argument.                           ## */
/* ## Reason   ## The reason for the error.                               ## */
/* ## VarName  ## A user-defined idenifier to the value.                  ## */
/* ## VarValue ## The variable to translate into the resulting message.   ## */
/* ## ...      ## An infinite amount more of variables can be specified.  ## */
/* ######################################################################### */
/* ========================================================================= */
#pragma once                           // Only one incursion allowed
/* -- Module namespace ----------------------------------------------------- */
namespace IfError {                    // Keep declarations neatly categorised
/* -- Includes ------------------------------------------------------------- */
using namespace IfString;              // Using string interface
/* ------------------------------------------------------------------------- */
struct ErrorPluginGeneric
  { explicit ErrorPluginGeneric(ostringstream&) { } };
/* ------------------------------------------------------------------------- */
template<class Plugin=ErrorPluginGeneric>class Error :
  /* -- Derivced classes --------------------------------------------------- */
  public exception,                    // So we can capture as exception
  public string                        // String to store generated string
{ /* -- exception class helper macro ------------------------------ */ private:
  #define XC(r,...) throw Error<>{ r, ## __VA_ARGS__ }
  /* -- Private variables -------------------------------------------------- */
  ostringstream osS;                   // Error message builder
  /* -- Write left part of var --------------------------------------------- */
  void Init(const char*const cpName, const char*const cpType)
    { osS << "\n+ " << cpName << '<' << cpType << "> = "; }
  /* -- Init a STL string -------------------------------------------------- */
  void Init(const string &strErr)
    { osS << (strErr.empty() ? "Unknown error!" : strErr); }
  /* -- Init a c-string ---------------------------------------------------- */
  void Init(const char*const cpErr)
    { osS << (cpErr ? (*cpErr ? cpErr : "Unknown error!") : "Bad error!"); }
  /* -- Last parameter processed ------------------------------------------- */
  void Param(void) { const Plugin pPlugin(osS); assign(osS.str()); }
  /* -- Show integer ------------------------------------------------------- */
  template<typename Type,typename UnsignedType=Type>
    void Int(const char*const cpName, const char*const cpType, const Type tVal)
  { // Write integer to stream
    Init(cpName, cpType);
    // Write value
    osS << dec << tVal << " (0x" << hex
        << static_cast<UnsignedType>(tVal) << ").";
  }
#ifdef _WIN32
  /* -- Process long integer ----------------------------------------------- */
  template<typename... V>void Param(const char*const cpName,
    const long lVal, const V... vVars)
      { Int<long,unsigned long>(cpName, "Long", lVal); Param(vVars...); }
  /* -- Process unsigned long integer -------------------------------------- */
  template<typename... V>void Param(const char*const cpName,
    const unsigned long lVal, const V... vVars)
      { Int<unsigned long>(cpName, "ULong", lVal); Param(vVars...); }
#endif
  /* -- Process 16-bit signed integer -------------------------------------- */
  template<typename... V>
    void Param(const char*const cpName, const int16_t sVal, const V... vVars)
      { Int<int16_t,uint16_t>(cpName, "Int16", sVal); Param(vVars...); }
  /* -- Process 16-bit unsigned integer ------------------------------------ */
  template<typename... V>
    void Param(const char*const cpName, const uint16_t usVal, const V... vVars)
      { Int<uint16_t>(cpName, "UInt16", usVal); Param(vVars...); }
  /* -- Process 32-bit signed integer -------------------------------------- */
  template<typename... V>
    void Param(const char*const cpName, const int32_t lVal, const V... vVars)
      { Int<int32_t,uint32_t>(cpName, "Int32", lVal); Param(vVars...); }
  /* -- Process 32-bit unsigned integer ------------------------------------ */
  template<typename... V>
    void Param(const char*const cpName, const uint32_t ulVal, const V... vVars)
      { Int<uint32_t>(cpName, "UInt32", ulVal); Param(vVars...); }
  /* -- Process 64-bit signed integer -------------------------------------- */
  template<typename... V>
    void Param(const char*const cpName, const int64_t qVal, const V... vVars)
      { Int<int64_t,uint64_t>(cpName, "Int64", qVal); Param(vVars...); }
  /* -- Process 64-bit unsigned integer ------------------------------------ */
  template<typename... V>
    void Param(const char*const cpName, const uint64_t uqVal, const V... vVars)
      { Int<uint64_t>(cpName, "UInt64", uqVal); Param(vVars...); }
  /* -- Process 8-bit unsigned integer ------------------------------------- */
  template<typename... V>void Param(const char*const cpName,
    const unsigned char ucByte, const V... vVars)
  { // First show as integer
    Int<unsigned int>(cpName, "UInt8", static_cast<unsigned int>(ucByte));
    // Display only if valid
    if(ucByte > 32) osS << " '" << static_cast<char>(ucByte) << "'.";
    // Process more parameters
    Param(vVars...);
  }
  /* -- Process 8-bit signed integer --------------------------------------- */
  template<typename... V>
    void Param(const char*const cpName, const char cByte, const V... vVars)
  { // First show as integer
    Int<int,unsigned int>(cpName, "Int8", static_cast<int>(cByte));
    // Display only if valid
    if(cByte > 32) osS << " '" << cByte << "'.";
    // Process more parameters
    Param(vVars...);
  }
  /* -- Apple needs this for some crazy reason ----------------------------- */
#ifdef __APPLE__
  /* -- Process signed long ------------------------------------------------ */
  template<typename... V>
    void Param(const char*const cpName, const long lVal, const V... vVars)
      { Int<long>(cpName, "Long", lVal); Param(vVars...); }
  /* -- Process unsigned long ---------------------------------------------- */
  template<typename... V>
    void Param(const char*const cpName, const unsigned long ulVal,
      const V... vVars)
        { Int<unsigned long>(cpName, "ULong", ulVal); Param(vVars...); }
  /* ----------------------------------------------------------------------- */
#endif
  /* -- Show float --------------------------------------------------------- */
  template<typename FloatType>void Float(const char*const cpName,
    const char*const cpType, const FloatType tVal)
      { Init(cpName, cpType); osS << fixed << tVal << '.'; }
  /* -- Process 64-bit double ---------------------------------------------- */
  template<typename... V>
    void Param(const char*const cpName, const double dVal, const V... vVars)
      { Float(cpName, "Float64", dVal); Param(vVars...); }
  /* -- Process 32-bit float ----------------------------------------------- */
  template<typename... V>
    void Param(const char*const cpName, const float fVal, const V... vVars)
      { Float(cpName, "Float32", fVal); Param(vVars...); }
  /* -- Process boolean ---------------------------------------------------- */
  template<typename... V>
    void Param(const char*const cpName, const bool bFlag, const V... vVars)
  { // Prepare parameter
    Init(cpName, "Bool");
    osS << TrueOrFalse(bFlag) << '.';
    // Process more parameters
    Param(vVars...);
  } /* -- Process pointer to address --------------------------------------- */
  template<typename... V>void Param(const char*const cpName,
    const void*const vpPtr, const V... vVars)
  { // Get StringStream
    Init(cpName, "Ptr");
     // Get variable as a C-string
    if(!vpPtr) osS << "<Null>";
    // Valid? Display and translation if neccesary
#ifdef _WIN32
    else osS << "0x" << vpPtr;         // Windows doesn't put in 0x for us
#else
    else osS << vpPtr;                 // Already puts in 0x for us
#endif
    // Add full stop
    osS << '.';
    // Process more parameters
    Param(vVars...);
  }
  /* -- Process C-String ------------------------------------------------- */
  template<typename... V>void Param(const char*const cpName,
    const char*const cpStr, const V... vVars)
  { // Initialise start of string
    Init(cpName, "CStr");
    // Get variable as a C-string
    if(!cpStr) osS << "<Null>.";
    // Empty?
    else if(!*cpStr) osS << "<Empty>.";
    // Displayable?
    else if(*cpStr < 32) osS << "<Invalid>.";
    // Valid? Display and translation if neccesary
    else osS << '\"' << cpStr << "\".";
    // Process more parameters
    Param(vVars...);
  }
  /* -- Process exception object ------------------------------------------- */
  template<typename... V>void Param(const char*const cpName,
    const exception &e, const V... vVars)
  { // Initialise start of string
    Init(cpName, "Ex");
    // Valid? Display and translation if neccesary
    osS << e.what() << '.';
    // Process more parameters
    Param(vVars...);
  }
  /* -- Show STL string ---------------------------------------------------- */
  template<class StringType>void Str(const char*const cpName,
    const char*const cpType, const StringType &tString)
  { // Initialise start of string
    Init(cpName, cpType);
    // String is empty?
    if(tString.empty()) osS << "<Empty>.";
    // String is not displayable?
    else if(tString[0] < 32) osS << "<Invalid>.";
    // Valid? Display string
    else osS << '\"' << tString << '\"' << dec
             << " [" << tString.length() << '/'
             << tString.capacity() << "].";
  }
  /* -- Process std::string lvalue ----------------------------------------- */
  template<typename... V>
    void Param(const char*const cpName, const string &strV, const V... vVars)
      { Str(cpName, "Str", strV); Param(vVars...); }
  /* -- Process std::wstring lvalue ---------------------------------------- */
  template<typename... V>
    void Param(const char*const cpName, const wstring &wstrV, const V... vVars)
      { Str(cpName, "WStr", wstrV); Param(vVars...); }
  /* -- Get message ------------------------------------------------ */ public:
  virtual const char *what(void) const noexcept override { return c_str(); }
  /* -- Prepare error message constructor with C-string--------------------- */
  template<typename... V>Error(const char*const cpErr, const V... vVars)
    { osS << cpErr; Param(vVars...); }
  /* -- Prepare error message constructor with STL string ------------------ */
  template<typename... V>Error(const string &strErr, const V... vVars)
    { osS << strErr; Param(vVars...); }
};/* ----------------------------------------------------------------------- */
/* -- End of module namespace ---------------------------------------------- */
};                                     // End of interface
/* == EoF =========================================================== EoF == */

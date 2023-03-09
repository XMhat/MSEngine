/* == SYSUTIL.HPP ========================================================== */
/* ######################################################################### */
/* ## MS-ENGINE              Copyright (c) MS-Design, All Rights Reserved ## */
/* ######################################################################### */
/* ## This is the header to define operating system level critical        ## */
/* ## utility functions.                                                  ## */
/* ######################################################################### */
/* ========================================================================= */
#pragma once                           // Only one incursion allowed
/* -- Module namespace ----------------------------------------------------- */
namespace IfSystem {                   // Keep declarations neatly categorised
/* ------------------------------------------------------------------------- */
using namespace Library::GlFW;         // Using GlFW library functions
using namespace IfUtf;                 // Using utf interface
using namespace IfString;              // Using string interface
/* -- Includes ------------------------------------------------------------- */
#if defined(_WIN32)                    // Using windows?
/* -- System error formatter with specified error code --------------------- */
static const string SysError(const int iError)
{ // Convert int to DWORD as we use the same function type across platforms
  const DWORD dwError = static_cast<DWORD>(iError);
  // Pointer to string
  LPWSTR lpszError = nullptr;
  // Capture exceptions
  try
  { // Format the system error code and catch result
    switch(const DWORD dwLen = FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM|
      FORMAT_MESSAGE_ALLOCATE_BUFFER|FORMAT_MESSAGE_IGNORE_INSERTS, nullptr,
      dwError, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        reinterpret_cast<LPWSTR>(&lpszError), 0, nullptr))
    { // Failed and no allocation was made so just store error code
      case 0: return Append("SE#", dwError);
      // We can remove carriage return and free the string (fall through)
      default: lpszError[dwLen - 3] = '\0';
      // Just incase we don't have enough characters
      case 1: case 2: const string strOut(S16toUTF(lpszError));
                      LocalFree(lpszError);
                      return strOut;
    }
  } // exception occured
  catch(const exception &E)
  { // Free the string if allocated
    if(lpszError) LocalFree(lpszError);
    // Assign the exception message as the error
    return Format("SE#$($)", dwError, E.what());
  }
}
/* -- System error code ---------------------------------------------------- */
template<typename IntType=int>static IntType SysErrorCode(void)
  { return static_cast<IntType>(GetLastError()); }
/* -- System error formatter with current error code ----------------------- */
static const string SysError(void) { return SysError(SysErrorCode()); }
/* -- Actual interface to MessageBoxExW ------------------------------------ */
static unsigned int SysMessage(void*const vpHandle, const string &strTitle,
  const string &strMessage, const unsigned int uiFlags)
    { return static_cast<unsigned int>(
        MessageBoxExW(reinterpret_cast<HWND>(vpHandle), UTFtoS16(strMessage),
        UTFtoS16(strTitle), static_cast<DWORD>(uiFlags), 0)); }
/* ------------------------------------------------------------------------- */
static void SysSetThreadName(const char*const cpName)
{ // Keep bytes aligned
#pragma pack(push, 8)
  struct WinDBGThreadData {
    DWORD  dwType;                     // Must be 0x1000.
    LPCSTR szName;                     // Pointer to name (in user addr space).
    DWORD  dwThreadID;                 // Thread ID (-1=caller thread).
    DWORD  dwFlags;                    // Reserved for future use, keep zero.
  } // Initialiser
  tInfo { 0x1000, cpName, static_cast<DWORD>(-1), 0 }; // Was ~DWORD{0}
#pragma pack(pop)
  // Send message to debugger to name the thread
  __try { RaiseException(0x406D1388, 0,
            sizeof(tInfo)/sizeof(ULONG_PTR), (ULONG_PTR*)&tInfo); }
  __except(EXCEPTION_CONTINUE_EXECUTION) { }
}
/* ------------------------------------------------------------------------- */
#elif defined(__APPLE__)               // Using mac?
/* -- System error code ---------------------------------------------------- */
template<typename IntType=int>static IntType SysErrorCode(void)
  { return static_cast<IntType>(GetErrNo()); }
/* -- System error formatter with specified error code --------------------- */
static const string SysError(const int iError) { return LocalError(iError); }
/* -- System error formatter with current error code ----------------------- */
static const string SysError(void) { return LocalError(SysErrorCode()); }
/* -- Actual interface to show a message box ----------------------------- */
static unsigned int SysMessage(void*const, const string &strTitle,
  const string &strMessage, const unsigned int)
{ // Make an autorelease ptr for Apple strings. Not sure if Apple provides a
  // non-pointer based CStringRef so we'll just remove it instead!
  typedef unique_ptr<const void, function<decltype(CFRelease)>> CFAutoRelPtr;
  // Setup dialogue title string with autorelease and if succeeded?
  if(const CFAutoRelPtr csrTitle{
    CFStringCreateWithCString(kCFAllocatorDefault, strTitle.c_str(),
      kCFStringEncodingUTF8), CFRelease })
    // Setup dialogue message string with autorelease and if succeeded?
    if(const CFAutoRelPtr csrMessage{
      CFStringCreateWithCString(kCFAllocatorDefault, strMessage.c_str(),
        kCFStringEncodingUTF8), CFRelease })
    { // Setup keys for dictionary
      array<const void*,2>
        vpKeys{ kCFUserNotificationAlertHeaderKey,
                kCFUserNotificationAlertMessageKey },
        // Setup values for dictionary
        vpVals{ csrTitle.get(),
                csrMessage.get() };
      // Create dictionary
      if(const CFAutoRelPtr cfdrDict{
        CFDictionaryCreate(nullptr, vpKeys.data(), vpVals.data(),
          vpKeys.size(), &kCFTypeDictionaryKeyCallBacks,
          &kCFTypeDictionaryValueCallBacks),
          CFRelease })
      { // Holds the result
        SInt32 nRes = 0;
        // Dispatch the message box and return result if successful
        if(const CFAutoRelPtr pDlg{
          CFUserNotificationCreate(kCFAllocatorDefault, 0,
            kCFUserNotificationPlainAlertLevel, &nRes,
            reinterpret_cast<CFDictionaryRef>(cfdrDict.get())),
            CFRelease })
          return static_cast<unsigned int>(nRes);
      }
    }
  // Didn't work so put in stdout
  fwprintf(stderr, L"%ls: %ls\n", Decoder{ strTitle }.Wide().c_str(),
                                  Decoder{ strMessage }.Wide().c_str());
  // If exited successfully? Return success
  return 0;
}
/* ------------------------------------------------------------------------- */
static void SysSetThreadName(const char*const) { }
/* ------------------------------------------------------------------------- */
#else                                  // Using linux?
/* -- Compatibility with X11 ----------------------------------------------- */
#ifdef Bool                            // Undefine 'Bool' set by X11
# undef Bool                           // To prevent problems with other apis
#endif                                 // Done checking for 'Bool'
/* -- System error code ---------------------------------------------------- */
template<typename IntType=int>static IntType SysErrorCode(void)
  { return static_cast<IntType>(GetErrNo()); }
/* -- System error formatter with specified error code --------------------- */
static const string SysError(const int iError) { return LocalError(iError); }
/* -- System error formatter with current error code ----------------------- */
static const string SysError(void) { return LocalError(SysErrorCode()); }
/* -- Actual interface to show a message box ----------------------------- */
static unsigned int SysMessage(void*const, const string &strTitle,
  const string &strMessage, const unsigned int uiFlags)
{ // Print the error first
  fwprintf(stderr, L"%ls: %ls\n", Decoder{ strTitle }.Wide().c_str(),
                                  Decoder{ strMessage }.Wide().c_str());
  // Return status code
  return 0;
}
/* ------------------------------------------------------------------------- */
static void SysSetThreadName(const char*const) { }
/* ------------------------------------------------------------------------- */
#endif                                 // Done checking OS
/* ------------------------------------------------------------------------- */
class SysErrorPlugin
{ /* -- Exception class helper macro for system errors --------------------- */
  #define XCS(r,...) throw Error<SysErrorPlugin>(r, ## __VA_ARGS__)
  /* -- Constructor to add system error code ----------------------- */ public:
  explicit SysErrorPlugin(ostringstream &osS)
  { // Get system error code and add system formatted parameter
    const int iCode = SysErrorCode();
    osS << "\n+ Reason<" << iCode << "> = \"" << SysError(iCode) << "\".";
  }
};/* ----------------------------------------------------------------------- */
static bool SysIsErrorCode(const int iCode=0)
  { return SysErrorCode() == iCode; }
static bool SysIsNotErrorCode(const int iCode=0)
  { return !SysIsErrorCode(iCode); }
/* -- System message without a handle -------------------------------------- */
static unsigned int SysMessage(const string &strTitle,
  const string &strMessage, const unsigned int uiFlags)
    { return SysMessage(nullptr, strTitle, strMessage,
        MB_SYSTEMMODAL|uiFlags); }
/* -- End of module namespace ---------------------------------------------- */
};                                     // End of interface
/* == EoF =========================================================== EoF == */

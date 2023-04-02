/* == SYSUTIL.HPP ========================================================== */
/* ######################################################################### */
/* ## MS-ENGINE              Copyright (c) MS-Design, All Rights Reserved ## */
/* ######################################################################### */
/* ## This is the header to define operating system level critical        ## */
/* ## utility functions.                                                  ## */
/* ######################################################################### */
/* ========================================================================= */
#pragma once                           // Only one incursion allowed
/* ------------------------------------------------------------------------- */
namespace IfSysUtil {                  // Start of module namespace
/* ------------------------------------------------------------------------- */
using namespace Library::GlFW;         // Using GlFW library functions
using namespace IfUtf;                 // Using utf namespace
using namespace IfString;              // Using string namespace
/* ------------------------------------------------------------------------- */
enum class SysThread                   // Thread priority types
{ /* ----------------------------------------------------------------------- */
  Main,                                // Reserved for main thread
  Engine,                              // Reserved for engine thread
  Audio,                               // Reserved for audio thread
  High,                                // Aux thread high priority
  Low                                  // Aux thread low priority
};/* -- Includes ----------------------------------------------------------- */
#if defined(WINDOWS)                   // Using windows?
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
        MessageBoxExW(reinterpret_cast<HWND>(vpHandle),
          UTFtoS16(strMessage).c_str(), UTFtoS16(strTitle).c_str(),
          static_cast<DWORD>(uiFlags), 0)); }
/* ------------------------------------------------------------------------- */
static bool SysInitThread(const char*const cpName, const SysThread stLevel)
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
  // Priority to set
  int iPriority;
  // What level was requested?
  switch(stLevel)
  { // Reserved for main thread
    case SysThread::Main: iPriority = THREAD_PRIORITY_ABOVE_NORMAL; break;
    // Reserved for engine thread
    case SysThread::Engine: iPriority = THREAD_PRIORITY_HIGHEST; break;
    // Reserved for audio thread
    case SysThread::Audio: iPriority = THREAD_PRIORITY_BELOW_NORMAL; break;
    // Aux thread high priority
    case SysThread::High: iPriority = THREAD_PRIORITY_NORMAL; break;
    // Aux thread low priority
    case SysThread::Low: iPriority = THREAD_PRIORITY_LOWEST; break;
    // Anything else
    default: return false;
  } // Set thread priorty and return result
  return !!SetThreadPriority(GetCurrentThread(), iPriority);
}
/* ------------------------------------------------------------------------- */
#elif defined(MACOS)                   // Using mac?
/* -- Actual interface to show a message box ----------------------------- */
static unsigned int SysMessage(void*const, const string &strTitle,
  const string &strMessage, const unsigned int uiFlags)
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
      // Setup button text string with autorelease and if succeeded?
      if(const CFAutoRelPtr csrButton{
        CFStringCreateWithCString(kCFAllocatorDefault, "Quit Application",
          kCFStringEncodingUTF8), CFRelease })
      { // Setup keys for dictionary
        array<const void*,3>
          vpKeys{ kCFUserNotificationAlertHeaderKey,
                  kCFUserNotificationAlertMessageKey,
                  kCFUserNotificationDefaultButtonTitleKey },
          // Setup values for dictionary
          vpVals{ csrTitle.get(),
                  csrMessage.get(),
                  csrButton.get() };
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
              (uiFlags & MB_ICONSTOP) ?
                kCFUserNotificationStopAlertLevel
            :((uiFlags & MB_ICONEXCLAMATION) ?
                kCFUserNotificationCautionAlertLevel
            :((uiFlags & MB_ICONINFORMATION) ?
                kCFUserNotificationPlainAlertLevel
            : kCFUserNotificationNoteAlertLevel)), &nRes,
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
#else                                  // Using linux?
/* -- Compatibility with X11 ----------------------------------------------- */
# if defined(Bool)                     // Undefine 'Bool' set by X11
#  undef Bool                          // To prevent problems with other apis
# endif                                // Done checking for 'Bool'
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
#endif                                 // Done checking OS
/* ------------------------------------------------------------------------- */
#if !defined(WINDOWS)                  // Not using Windows target? (POSIX)
/* -- System error code ---------------------------------------------------- */
template<typename IntType=int>static IntType SysErrorCode(void)
  { return static_cast<IntType>(GetErrNo()); }
/* -- System error formatter with specified error code --------------------- */
static const string SysError(const int iError) { return LocalError(iError); }
/* -- System error formatter with current error code ----------------------- */
static const string SysError(void) { return LocalError(SysErrorCode()); }
/* ------------------------------------------------------------------------- */
static bool SysInitThread(const char*const cpName, const SysThread stLevel)
{ // Get this thread handle
  pthread_t ptHandle = pthread_self();
  // Set thread name
#if defined(MACOS)
  pthread_setname_np(cpName);
#else
  pthread_setname_np(ptHandle, cpName);
#endif
  // Container for current scheduler parameters
  struct sched_param spParam;
  // Container for current policy level
  int iPolicy;
  // Get current thread settings
  if(pthread_getschedparam(ptHandle, &iPolicy, &spParam)) return false;
  // Get bounds
  const int iMinPriority = sched_get_priority_min(iPolicy),
            iMaxPriority = sched_get_priority_max(iPolicy);
  // What level was requested?
  switch(stLevel)
  { // Reserved for main thread
    case SysThread::Main:
      // Use high priority parameters
      spParam.sched_priority = iMinPriority+1;
      iPolicy = SCHED_RR;
      // Done
      break;
    // Reserved for engine thread
    case SysThread::Engine:
      // Use high priority parameters
      spParam.sched_priority = iMinPriority;
      iPolicy = SCHED_RR;
      // Done
      break;
    // Reserved for audio thread
    case SysThread::Audio:
      // Use high priority parameters
      spParam.sched_priority = iMinPriority+2;
      iPolicy = SCHED_RR;
      // Done
      break;
    // Aux thread high priority
    case SysThread::High:
      // Use high priority parameters
      spParam.sched_priority = iMinPriority+3;
      iPolicy = SCHED_RR;
      // Done
      break;
    // Aux thread low priority
    case SysThread::Low:
      // Use high priority parameters
      spParam.sched_priority = iMaxPriority;
      iPolicy = SCHED_OTHER;
      // Done
      break;
  } // Clamp the value
  spParam.sched_priority =
    IfUtil::Clamp(spParam.sched_priority, iMinPriority, iMaxPriority);
  // Set the new parameters and return true if succeeded
  return !pthread_setschedparam(ptHandle, iPolicy, &spParam);
}
/* ------------------------------------------------------------------------- */
#endif                                 // Not using Windows target
/* ------------------------------------------------------------------------- */
struct SysErrorPlugin final
{ /* -- Exception class helper macro for system errors --------------------- */
#define XCS(r,...) throw IfError::Error<SysErrorPlugin>(r, ## __VA_ARGS__)
  /* -- Constructor to add system error code ------------------------------- */
  explicit SysErrorPlugin(ostringstream &osS)
  { // Get system error code and add system formatted parameter
    const int iCode = SysErrorCode();
    osS << "\n+ Reason<" << iCode << "> = \"" << SysError(iCode) << "\".";
  }
};/* ----------------------------------------------------------------------- */
static bool SysIsErrorCode(const int iCode=0)
  { return SysErrorCode() == iCode; }
static bool SysIsNotErrorCode[[maybe_unused]](const int iCode=0)
  { return !SysIsErrorCode(iCode); }
/* -- System message without a handle -------------------------------------- */
static unsigned int SysMessage(const string &strTitle,
  const string &strMessage, const unsigned int uiFlags)
    { return SysMessage(nullptr, strTitle, strMessage,
        MB_SYSTEMMODAL|uiFlags); }
/* ------------------------------------------------------------------------- */
};                                     // End of module namespace
/* == EoF =========================================================== EoF == */

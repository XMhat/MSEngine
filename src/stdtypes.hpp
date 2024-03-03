/* == STDTYPES.HPP ========================================================= **
** ######################################################################### **
** ## MS-ENGINE              Copyright (c) MS-Design, All Rights Reserved ## **
** ######################################################################### **
** ## This module sets up all the types we use from the STL. All these    ## **
** ## types should sit in the Engine namespace so only the engine has     ## **
** ## access to them and not any other included API.                      ## **
** ######################################################################### **
** ========================================================================= */
#pragma once                           // Only one incursion allowed
/* -- Storage spaces ------------------------------------------------------- */
using ::std::array;                    using ::std::deque;
using ::std::list;                     using ::std::map;
using ::std::queue;                    using ::std::pair;
using ::std::set;                      using ::std::string;
using ::std::string_view;              using ::std::vector;
using ::std::wstring;
/* -- Exceptions ----------------------------------------------------------- */
using ::std::exception;                using ::std::runtime_error;
/* -- Other ---------------------------------------------------------------- */
using ::std::addressof;                using ::std::bind;
using ::std::function;                 using ::std::locale;
using ::std::make_pair;                using ::std::make_signed;
using ::std::make_unsigned;            using ::std::nothrow;
using ::std::numeric_limits;           using ::std::remove_const;
using ::std::remove_pointer;           using ::std::swap;
/* -- Iteratations --------------------------------------------------------- */
using ::std::accumulate;               using ::std::any_of;
using ::std::back_inserter;            using ::std::next;
using ::std::prev;
/* -- String streams ------------------------------------------------------- */
using ::std::dec;                      using ::std::fixed;
using ::std::fpclassify;               using ::std::get_time;
using ::std::hex;                      using ::std::ios_base;
using ::std::istringstream;            using ::std::left;
using ::std::noskipws;                 using ::std::oct;
using ::std::ostream_iterator;         using ::std::ostringstream;
using ::std::put_time;                 using ::std::right;
using ::std::setfill;                  using ::std::setprecision;
using ::std::setw;                     using ::std::showpos;
/* -- Constexpr functions -------------------------------------------------- */
using ::std::is_floating_point_v;      using ::std::is_integral_v;
using ::std::is_pointer_v;             using ::std::is_signed_v;
using ::std::is_enum_v;                using ::std::is_same_v;
using ::std::underlying_type_t;
/* -- Namespaces ----------------------------------------------------------- */
using ::std::placeholders::_1;
/* -- Times ---------------------------------------------------------------- */
using ::std::chrono::duration_cast;    using ::std::chrono::duration;
using ::std::chrono::microseconds;     using ::std::chrono::milliseconds;
using ::std::chrono::nanoseconds;      using ::std::chrono::seconds;
using ::std::chrono::system_clock;
/* -- Operating system specific -------------------------------------------- */
using ::std::terminate_handler;        using ::std::set_terminate;
#if !defined(MACOS)                    // Not using apple compiler?
using ::std::execution::par_unseq;     using ::std::execution::par;
using ::std::execution::seq;
#endif                                 // Apple check
/* -- Asynchronisation ----------------------------------------------------- */
using ::std::atomic;                   using ::std::condition_variable;
using ::std::lock_guard;               using ::std::mutex;
using ::std::thread;                   using ::std::try_to_lock;
using ::std::unique_lock;              using ::std::unique_ptr;
typedef atomic<bool>       SafeBool;   // Thread safe boolean
typedef atomic<double>     SafeDouble; // Thread safe double
typedef atomic<int>        SafeInt;    // Thread safe integer
typedef atomic<long>       SafeLong;   // Thread safe long
typedef atomic<size_t>     SafeSizeT;  // Thread safe size_t
typedef atomic<uint64_t>   SafeUInt64; // Thread safe 64-bit integer
typedef atomic<unsigned int> SafeUInt; // Thread safe unsigned integer
typedef lock_guard<mutex>  LockGuard;  // Shortcut to a mutex lock guard
typedef unique_lock<mutex> UniqueLock; // SHortcut to a mutex unique lock
/* -- Clocks --------------------------------------------------------------- */
using CoreClock = ::std::chrono::high_resolution_clock; // Using HRC
typedef CoreClock::time_point ClkTimePoint;             // Holds a time
typedef CoreClock::duration   ClkDuration;              // Holds a duration
typedef atomic<ClkDuration>   SafeClkDuration;          // Thread safe duration
/* -- Indexed vector list types -------------------------------------------- */
typedef vector<const char*>               CStrVector;
typedef CStrVector::const_iterator        CStrVectorConstIt;
typedef vector<float>                     FloatVector;
typedef FloatVector::const_iterator       FloatVectorConstIt;
typedef vector<int>                       IntVector;
typedef IntVector::const_iterator         IntVectorConstIt;
typedef vector<string>                    StrVector;
typedef StrVector::iterator               StrVectorIt;
typedef StrVector::const_iterator         StrVectorConstIt;
typedef StrVector::const_reverse_iterator StrVectorConstRevIt;
typedef vector<unsigned int>              UIntVector;
typedef UIntVector::const_iterator        UIntVectorConstIt;
/* -- Standard linked list types ------------------------------------------- */
typedef list<string>                    StrList;
typedef StrList::const_iterator         StrListConstIt;
typedef StrList::const_reverse_iterator StrListConstRevIt;
typedef StrList::iterator               StrListIt;
/* -- List of string pairs ------------------------------------------------- */
typedef pair<const string, const string> StrPair;
typedef list<StrPair>                    StrPairList;
/* -- Set of strings ------------------------------------------------------- */
typedef set<string>            StrSet;
typedef StrSet::const_iterator StrSetConstIt;
/* -- map of strings and unsigned ints ------------------------------------- */
typedef map<const string, const unsigned int> StrUIntMap;
typedef StrUIntMap::const_iterator            StrUIntMapConstIt;
typedef vector<StrUIntMapConstIt>             StrUIntMapConstItVector;
/* -- map of strings and signed ints --------------------------------------- */
typedef map<const string, const int> StrIntMap;
typedef StrIntMap::const_iterator    StrIntMapConstIt;
/* -- map of key and value strings ----------------------------------------- */
typedef map<const string, const string> StrStrMap;
typedef StrStrMap::iterator             StrStrMapIt;
typedef StrStrMap::const_iterator       StrStrMapConstIt;
/* -- map of key and value string_views ------------------------------------ */
typedef map<const string_view, const string_view> StrVStrVMap;
typedef StrVStrVMap::iterator                     StrVStrVMapIt;
typedef StrVStrVMap::const_iterator               StrVStrVMapConstIt;
/* -- map of key and modifyable value strings ------------------------------ */
typedef map<const string, string>   StrNCStrMap;
typedef StrNCStrMap::iterator       StrNCStrMapIt;
typedef StrNCStrMap::const_iterator StrNCStrMapConstIt;
/* == EoF =========================================================== EoF == */

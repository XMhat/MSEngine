/* == TYPES.HPP ============================================================ */
/* ######################################################################### */
/* ## MS-ENGINE              Copyright (c) MS-Design, All Rights Reserved ## */
/* ######################################################################### */
/* ## This module sets up all the types we use from the STL. All these    ## */
/* ## types should sit in the Engine namespace so only the engine has     ## */
/* ## access to them and not any other included API.                      ## */
/* ######################################################################### */
/* ========================================================================= */
#pragma once                           // Only one incursion allowed
/* -- Storage spaces ------------------------------------------------------- */
using ::std::array;                    // Static arrays
using ::std::deque;                    // Chunked indexed arrays
using ::std::list;                     // Linked lists
using ::std::map;                      // Sorted key lists
using ::std::pair;                     // Key value pairs
using ::std::set;                      // Unsorted string list
using ::std::string;                   // Narrow strings
using ::std::vector;                   // Indexed lists
using ::std::wstring;                  // Wide strings
/* -- Exceptions ----------------------------------------------------------- */
using ::std::exception;                // Exceptions
using ::std::runtime_error;            // Runtime errors (exceptions)
/* -- Other ---------------------------------------------------------------- */
using ::std::addressof;                // Get underlying address of a pointer
using ::std::bind;                     // Store function and calling convention
using ::std::function;                 // Store any function
using ::std::locale;                   // For applying localisation to values
using ::std::make_pair;                // Make a pair when 'insert' doesnt work
using ::std::nothrow;                  // Prevent throwing on 'new'
using ::std::numeric_limits;           // Query integral value limits
using ::std::remove_const;             // For removing const from type
using ::std::remove_pointer;           // For removing a pointer from type
using ::std::make_signed;              // Make a type signed
using ::std::make_unsigned;            // Make a type unsigned
using ::std::swap;                     // Swap data with another variable.
/* -- Iteratations --------------------------------------------------------- */
using ::std::accumulate;               // Loop and accumulate a value
using ::std::any_of;                   // Any of these iterator
using ::std::back_inserter;            // Helper when enumerating values
using ::std::next;                     // Traverse iterator forward
using ::std::prev;                     // Traverse iterator backwards
/* -- String streams ------------------------------------------------------- */
using ::std::dec;                      // Display integral value as decimal
using ::std::rotate;                   // Wrap data across boundaries
using ::std::fixed;                    // Show real numbers as non-scientific
using ::std::get_time;                 // Parse a time string
using ::std::hex;                      // Display integral value as hex
using ::std::ios_base;                 // Allows storing of alignment
using ::std::istringstream;            // Input string streams
using ::std::left;                     // Left aligned output (text)
using ::std::noskipws;                 // Don't skip whitespaces on parse
using ::std::oct;                      // Display integral value as octlet
using ::std::ostream_iterator;         // For copying data to ostringstream
using ::std::ostringstream;            // Output string streams
using ::std::put_time;                 // Push a formatted time string
using ::std::right;                    // Right aligned output (values)
using ::std::setfill;                  // Set padding characters
using ::std::setprecision;             // Set precision of real numbers
using ::std::setw;                     // Add padding to next value
using ::std::showpos;                  // Show '+' on positive values
using ::std::fpclassify;               // Classify floating point number
/* -- Static assert functions ---------------------------------------------- */
using ::std::is_floating_point_v;      // If type is floating point?
using ::std::is_integral_v;            // If type is integral?
using ::std::is_pointer_v;             // If type has a pointer?
using ::std::is_signed_v;              // If type is signed?
using ::std::is_same_v;                // If type is equal?
/* -- Namespaces ----------------------------------------------------------- */
using ::std::placeholders::_1;         // For ::std::bind placeholder parameter
/* -- Times ---------------------------------------------------------------- */
using ::std::chrono::duration_cast;    // Cast a duration to another type
using ::std::chrono::duration;         // Return duration of a specified time
using ::std::chrono::microseconds;     // Microseconds unit for duration
using ::std::chrono::milliseconds;     // Milliseconds unit for duration
using ::std::chrono::nanoseconds;      // Nanoseconds unit for duration
using ::std::chrono::seconds;          // Seconds unit for duration
using ::std::chrono::system_clock;     // Need system clock for time
/* -- Operating system specific -------------------------------------------- */
#if !defined(WINDOWS)                  // Not using windows?
using ::std::unexpected_handler;       // Unexpected exception handler
using ::std::set_unexpected;           // Set unexpected exception handler
#endif                                 // Not windows check
using ::std::terminate_handler;        // Terminate handler
using ::std::set_terminate;            // Set terminate handler
#if !defined(MACOS)                    // Using apple compiler?
using ::std::execution::par_unseq;     // Parallel unserialised operation
using ::std::execution::par;           // Parallel serialised operation
using ::std::execution::seq;           // Serialised operation
#endif                                 // Apple check
/* -- Asynchronisation ----------------------------------------------------- */
using ::std::atomic;                   // atomic variables (synchronising)
using ::std::condition_variable;       // Condition variables (synchronising)
using ::std::lock_guard;               // For exclusive lock in 'unique_lock'
using ::std::mutex;                    // Mutexes (synchronising)
using ::std::thread;                   // Threading support
using ::std::try_to_lock;              // For exclusive lock in 'unique_lock'
using ::std::unique_lock;              // For managing class pointers
using ::std::unique_ptr;               // For managing class pointers
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
typedef map<const string, const int>  StrIntMap;
typedef StrIntMap::const_iterator     StrIntMapConstIt;
/* -- map of key and value strings ----------------------------------------- */
typedef map<const string, const string> StrStrMap;
typedef StrStrMap::iterator             StrStrMapIt;
typedef StrStrMap::const_iterator       StrStrMapConstIt;
/* -- map of key and referenced value strings ------------------------------ */
typedef map<const string, const string&> StrStrRefMap;
typedef StrStrMap::iterator              StrStrRefMapIt;
typedef StrStrMap::const_iterator        StrStrRefMapConstIt;
/* -- map of key and modifyable value strings ------------------------------ */
typedef map<const string, string>   StrNCStrMap;
typedef StrNCStrMap::iterator       StrNCStrMapIt;
typedef StrNCStrMap::const_iterator StrNCStrMapConstIt;
/* == EoF =========================================================== EoF == */

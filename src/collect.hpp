/* == COLLECT.HPP ========================================================== */
/* ######################################################################### */
/* ## MS-ENGINE              Copyright (c) MS-Design, All Rights Reserved ## */
/* ######################################################################### */
/* ## This module contains routines in helping keeping a record of each   ## */
/* ## allocated member pointers in a collector list and keeping a limit   ## */
/* ## on the size of the list. It also features optional synchronisation. ## */
/* ######################################################################### */
/* ========================================================================= */
#pragma once                           // Only one incursion allowed
/* ------------------------------------------------------------------------- */
namespace IfCollector {                // Start of module namespace
/* -- Includes ------------------------------------------------------------- */
using namespace IfCVarDef;             // Using cvardef namespace
using namespace IfSysUtil;             // Using system utility namespace
using namespace IfLog;                 // Using log namespace
/* -- Collector header ----------------------------------------------------- */
#define COLLECTHDR(PCR,                /* The collector class type          */\
                   SCR,                /* The member class type             */\
                   CLH,                /* CLHelper or CLHelperAsync         */\
                   PCI,                /* CLHelperSafe or CLHelperUnsafe    */\
                   PCV,                /* Extra class body arguments (no ,) */\
                   ...)                /* Extra derivced classes            */\
  class SCR;                           /* Member class prototype            */\
  typedef CLH<SCR,PCI<SCR>>            /* Make an alias to the locktype     */\
    PCR ## CLHelper;                   /*   for the collector helper        */\
  static struct PCR final :            /* Begin collector object class      */\
    public PCR ## CLHelper             /* Derive by collector helper class  */\
    __VA_ARGS__                        /* Any other custom class derives    */\
  { DELETECOPYCTORS(PCR)               /* Remove default functions          */\
    PCR(void);                         /* Constructor prototype             */\
    ~PCR(void) noexcept(false);        /* Destructor prototype              */\
    PCV                                /* Any extra variables? (no comma!)  */
/* -- Collector footer ----------------------------------------------------- */
#define COLLECTFTR(PCR) } *c ## PCR = nullptr; /* Pointer to static class    */
/* -- Build a collector class body (no comma) with custom derived classes -- */
#define BEGIN_COLLECTOREX(PCR,         /* The collector type                */\
                          SCR,         /* The member type                   */\
                          PCI,         /* CLHelperSafe or CLHelperUnsafe    */\
                          PCV,         /* Extra body (no commas allowed)    */\
                          ...)         /* User specified collector body     */\
  COLLECTHDR(PCR,SCR,CLHelper,         /* Insert standard header            */\
    PCI,PCV,## __VA_ARGS__)            /*   with body and custom derives    */\
  COLLECTFTR(PCR)                      /* Insert standard footer             */
/* -- Build a collector class body with a fully custom body ---------------- */
#define BEGIN_COLLECTOREX2(PCR,        /* The collector type                */\
                           SCR,        /* The member type                   */\
                           PCI,        /* CLHelperSafe or CLHelperUnsafe    */\
                           ...)        /* User specified collector body     */\
  COLLECTHDR(PCR,SCR,CLHelper,PCI,)    /* Begin standard header             */\
  __VA_ARGS__                          /* Insert user collector body        */\
  COLLECTFTR(PCR)                      /* Insert standard footer             */
/* -- Build a collector class body with no special parameters -------------- */
#define BEGIN_COLLECTOR(PCR,SCR,PCI) BEGIN_COLLECTOREX(PCR,SCR,PCI,,)
/* -- Thread safe collector with user-defined variables or classes --------- */
#define BEGIN_ASYNCCOLLECTOREX(PCR,SCR,CLH,PCV,...) \
  COLLECTHDR(PCR,SCR,CLHelperAsync,CLH,PCV,## __VA_ARGS__) \
  COLLECTFTR(PCR)
/* -- Thread safe collector with no user-defined variables or classes ------ */
#define BEGIN_ASYNCCOLLECTOR(PCR,SCR,CLH) BEGIN_ASYNCCOLLECTOREX(PCR,SCR,CLH,,)
/* -- Tailing collector class macro with init and deinit calls ------------- */
#define END_COLLECTOREX2(PCR,CFI,CFD,...) \
  PCR::PCR(void) : __VA_ARGS__ { IHInitialise(); CFI; } \
  DTORHELPER(PCR::~PCR, if(IHNotDeInitialise()) return; CFD)
#define END_COLLECTOREX(PCR,CFI,CFD,...) \
  END_COLLECTOREX2(PCR,CFI,CFD,CLHelper{ STR(PCR) } __VA_ARGS__)
/* -- Tailing collector class macro with init and deinit calls ------------- */
#define END_ASYNCCOLLECTOREX2(PCR,PCE,CFI,CFD,...) \
  END_COLLECTOREX2(PCR,CFI,CFD,\
    CLHelperAsync{ STR(PCR), EMC_MP_ ## PCE } __VA_ARGS__);
#define END_ASYNCCOLLECTOREX(PCR,SCR,PCE,CFI,CFD,...) \
  END_ASYNCCOLLECTOREX2(PCR,PCE,CFI,CFD,,## __VA_ARGS__)
#define END_ASYNCCOLLECTOR(PCR,SCR,PCE,...) \
  END_ASYNCCOLLECTOREX(PCR,SCR,PCE,,,## __VA_ARGS__)
/* -- Tailing collector class macro with no init and deinit calls ---------- */
#define END_COLLECTOR(PCR) END_COLLECTOREX(PCR,,,)
/* -- Start building a member class for a collector ------------------------ */
#define BEGIN_MEMBERCLASSEX(SCR,       /* The collector type                */\
                            PCR,       /* The member type                   */\
                            ICH,       /* ICHelperSafe or ICHelperUnsafe    */\
                            ...)       /* Extra arguments                   */\
  typedef ICHelper<SCR,PCR,            /* Make an alias for the locktype    */\
    ICH<SCR,PCR>> ICHelper ## PCR;     /*   which will be derived           */\
  class PCR : public ICHelper ## PCR   /* Begin the member class            */\
    __VA_ARGS__                        /* Add extra arguments if needed     */
/* -- Start building a member class for a collector ------------------------ */
#define BEGIN_MEMBERCLASS(SCR,         /* The collector type                */\
                          PCR,         /* The member type                   */\
                          ICH)         /* ICHelperSafe or ICHelperUnsafe    */\
  BEGIN_MEMBERCLASSEX(SCR,PCR,ICH,,    /* Use expanded macro                */\
    public IdentCSlave<>)              /* Counter id slave class            */
/* -- All in one collector and member builder ------------------------------ */
#define BEGIN_COLLECTORDUO(SCR,        /* The collector type                */\
                           PCR,        /* The member type                   */\
                           CLH,        /* CLHelperSafe or CLHelperUnsafe    */\
                           ICH)        /* ICHelperSafe or ICHelperUnsafe    */\
  BEGIN_COLLECTOR(SCR,PCR,CLH)         /* Start building collector class    */\
  BEGIN_MEMBERCLASS(SCR,PCR,ICH)       /* Start building member class        */
/* -- All in one async collector and async member builder ------------------ */
#define BEGIN_ASYNCCOLLECTORDUO(SCR,   /* The collector type                */\
                                PCR,   /* The member type                   */\
                                CLH,   /* CLHelperSafe or CLHelperUnsafe    */\
                                ICH)   /* ICHelperSafe or ICHelperUnsafe    */\
  BEGIN_ASYNCCOLLECTOR(SCR,PCR,CLH)    /* Start building collector class    */\
  BEGIN_MEMBERCLASS(SCR,PCR,ICH)       /* Start building member class   */
/* == Init Helper Class ==================================================== */
/* ######################################################################### */
/* ## This class holds the name of a class and if it has been             ## */
/* ## initialised. It stops classes being accidentally double             ## */
/* ## initialised due to programming errors.                              ## */
/* ######################################################################### */
/* ------------------------------------------------------------------------- */
class IHelper :                        // The Init Helper class
  /* -- Base classes ------------------------------------------------------- */
  public IfIdent::IdentConst           // Holds the identifier
{ /* -- Private variables -------------------------------------------------- */
  ClkTimePoint     ctInitialised,      // Time class was initialised
                   ctDeinitialised;    // Time class was deinitialised
  /* ----------------------------------------------------------------------- */
  void IHSetInitialised(void) { ctInitialised = cmHiRes.GetTime(); }
  void IHSetDeInitialised(void) { ctDeinitialised = cmHiRes.GetTime(); }
  /* --------------------------------------------------------------- */ public:
  bool IHIsInitialised(void) const { return ctInitialised > ctDeinitialised; }
  bool IHIsNotInitialised(void) const { return !IHIsInitialised(); }
  /* ----------------------------------------------------------------------- */
  void IHInitialise(void)
  { // Raise exception if object already initialised
    if(IHIsInitialised())
      XC("Object already initialised!",
         "Identifier", IdentGet(),
         "Age", ToShortDuration(cmHiRes.
                  TimePointToClampedDouble(ctInitialised)));
    // Class now initialised
    IHSetInitialised();
  }
  /* ----------------------------------------------------------------------- */
  bool IHDeInitialise(void)
  { // Return if class already initialised
    if(IHIsNotInitialised()) return false;
    // Class now de-initialised
    IHSetDeInitialised();
    // Success
    return true;
  }
  /* ----------------------------------------------------------------------- */
  bool IHNotDeInitialise(void) { return !IHDeInitialise(); }
  /* -- Constructors -------------------------------------------- */ protected:
  explicit IHelper(const string &strIdent) :
    /* -- Initialisers ----------------------------------------------------- */
    IdentConst(strIdent),              // Initialise name
    ctInitialised{seconds{0}},         // Clear initialised time
    ctDeinitialised{cmHiRes.GetTime()} // Set deinitialised time
    /* -- No code ---------------------------------------------------------- */
    { }
  /* -- Default suppressions ----------------------------------------------- */
  DELETECOPYCTORS(IHelper)             // Remove copy ctor/assign oper
};/* ----------------------------------------------------------------------- */
/* == Collector class helper =============================================== */
/* ######################################################################### */
/* ## Derive this class into the main class to allow collection of child  ## */
/* ## classes.                                                            ## */
/* ######################################################################### */
/* ------------------------------------------------------------------------- */
template<class MemberType,             // Member type (Archive, Asset, etc.)
         class ListType,               // List type (std::list)
         class IteratorType>           // Iterator type (std::list::iterator)
class CLHelperBase :
  /* -- Base classes ------------------------------------------------------- */
  public IdentCMaster<>,               // Counter master class
  public ListType,                     // The list of members
  public IHelper                       // Initialisation helper
{ /* -- Private variables -------------------------------------------------- */
  size_t           stMaximum;          // Maximum children allowed
  /* -- Return last item in the list ------------------------------- */ public:
  IteratorType CLBaseGetLastItemUnsafe(void) { return this->end(); }
  /* -- Return if collector is empty --------------------------------------- */
  bool CLBaseIsEmptyUnsafe(void) {  return this->empty(); }
  /* -- Destroy all objects (clear() doesn't do this!) --------------------- */
  void CLBaseDestroyUnsafe(void)
    { while(!CLBaseIsEmptyUnsafe()) delete this->back(); }
  /* -- Return number of registered items ---------------------------------- */
  size_t CLBaseCountUnsafe(void) { return this->size(); }
  /* -- Reference counter increment ---------------------------------------- */
  void CLBaseCheckUnsafe(void)
  { // Throw error if at limit. This check is only performed during
    // construction of an object. Construction should never occur off-thread or
    // we will need to serialise this access.
    if(CLBaseCountUnsafe() >= stMaximum)
      XC("Collector child object threshold exceeded!",
         "Type", IdentGet(), "Current", CLBaseCountUnsafe(),
         "Maximum", stMaximum);
  }
  /* -- Set limit ---------------------------------------------------------- */
  CVarReturn CLBaseSetLimitUnsafe(const size_t stLimit)
    { return CVarSimpleSetInt(stMaximum, stLimit); }
  /* -- Add/remove object to the list -------------------------------------- */
  IteratorType CLBaseAddUnsafe(MemberType*const mtObj)
    { return this->emplace(CLBaseGetLastItemUnsafe(), mtObj); }
  IteratorType CLBaseEraseUnsafe(IteratorType &itObj)
    { this->erase(itObj); return CLBaseGetLastItemUnsafe(); }
  /* -- Check and destroy -------------------------------------------------- */
  void CLBaseCheckAndDestroyUnsafe(void)
  { // Return if class was initialised and there are no children left
    if(CLBaseIsEmptyUnsafe()) return;
    // Show message box to say we have items remaining
    cLog->LogWarningExSafe("$ collector unloading $ remaining objects...",
      IdentGet(), CLBaseCountUnsafe());
    CLBaseDestroyUnsafe();
    cLog->LogWarningExSafe("$ collector unloaded the remaining objects!",
      IdentGet());
  }
  /* -- Constructor -------------------------------------------------------- */
  explicit CLHelperBase(const string &strT) :
    /* -- Initialisers ----------------------------------------------------- */
    IHelper{ strT },                   // Set initialisation helper name
    stMaximum(                         // Initialise maximum objects
      numeric_limits<size_t>::max())   // " with maximum default objects
    /* -- No code ---------------------------------------------------------- */
    { }
  /* ----------------------------------------------------------------------- */
  DELETECOPYCTORS(CLHelperBase)        // Don't need the default constructor
};/* ----------------------------------------------------- Collector::End -- */
template<class MemberType,
         class ListType = list<MemberType*>,
         class IteratorType = typename ListType::iterator,
         class BaseType = CLHelperBase<MemberType, ListType, IteratorType>>
class CLHelperSafe :
  /* -- Base classes ------------------------------------------------------- */
  public BaseType                      // The collector base type
{ /* -- Private variables -------------------------------------------------- */
  mutex            mMutex;             // Multi-thread locking protection
  /* -- Protected functions ------------------------------------- */ protected:
  void CLLock(void) { CollectorGetMutex().lock(); }
  void CLUnlock(void) { CollectorGetMutex().unlock(); }
  /* -- Lock the mutex and return if empty --------------------------------- */
  bool CLEmpty(void)
    { const LockGuard lgGuardEmpty{ CollectorGetMutex() };
      return this->CLBaseIsEmptyUnsafe(); }
  /* -- Lock the mutex and check size -------------------------------------- */
  void CLCheck(void)
    { const LockGuard lgGuardCheck{ CollectorGetMutex() };
      this->CLBaseCheckUnsafe(); }
  /* -- Lock the mutex and return size ------------------------------------- */
  size_t CLCount(void)
    { const LockGuard lgGuardCount{ CollectorGetMutex() };
      return this->CLBaseCountUnsafe(); }
  /* -- Lock the mutex and return newly added object ----------------------- */
  IteratorType CLAdd(MemberType*const mtObj)
    { const LockGuard lgGuardAdd{ CollectorGetMutex() };
      return this->CLBaseAddUnsafe(mtObj); }
  /* -- Lock the mutex and return the removed iterator --------------------- */
  IteratorType CLErase(IteratorType &itObj)
    { const LockGuard lgGuardErase{ CollectorGetMutex() };
      return this->CLBaseEraseUnsafe(itObj); }
  /* -- Lock the mutex and return the removed iterator --------------------- */
  CVarReturn CLSetLimit(const size_t stLimit)
    { const LockGuard lgGuardSetLimit{ CollectorGetMutex() };
      return this->CLBaseSetLimitUnsafe(stLimit); }
  /* -- Constructor -------------------------------------------------------- */
  explicit CLHelperSafe(const char*const cpT) :
    /* -- Initialisers ----------------------------------------------------- */
    BaseType{ cpT }                    // Initialise base type with name
    /* -- No code ---------------------------------------------------------- */
    { }
  /* -- Destructor --------------------------------------------------------- */
  ~CLHelperSafe(void) { }
  /* ----------------------------------------------------------------------- */
  DELETECOPYCTORS(CLHelperSafe)        // Don't need the default constructor
  /* -- Return the mutex ------------------------------------------- */ public:
  mutex &CollectorGetMutex(void) { return mMutex; }
};/* ----------------------------------------------------------------------- */
template<class MemberType,
         class ListType = list<MemberType*>,
         class IteratorType = typename ListType::iterator,
         class BaseType = CLHelperBase<MemberType, ListType, IteratorType>>
class CLHelperUnsafe :                 // Members initially private
  /* -- Base classes ------------------------------------------------------- */
  public BaseType                      // The collector base type
{ /* -- (Un)Lock the mutex (nope) ------------------------------- */ protected:
  void CLLock(void) { }
  void CLUnlock(void) { }
  /* -- Return if collector is empty --------------------------------------- */
  bool CLEmpty(void) { return this->CLBaseIsEmptyUnsafe(); }
  /* -- Check for overflow ------------------------------------------------- */
  void CLCheck(void) { this->CLBaseCheckUnsafe(); }
  /* -- Return number of registered items ---------------------------------- */
  size_t CLCount(void) { return this->CLBaseCountUnsafe(); }
  /* -- Add/remove object to the list -------------------------------------- */
  IteratorType CLAdd(MemberType*const mtObj)
    { return this->CLBaseAddUnsafe(mtObj); }
  IteratorType CLErase(IteratorType &itObj)
    { return this->CLBaseEraseUnsafe(itObj); }
  /* -- Set maximum objects ------------------------------------------------ */
  CVarReturn CLSetLimit(const size_t stL)
    { return this->CLBaseSetLimitUnsafe(stL); }
  /* -- Constructor -------------------------------------------------------- */
  explicit CLHelperUnsafe(const char*const cpT) :
    /* -- Initialisers ----------------------------------------------------- */
    BaseType{ cpT }                    // Initialise base type with name
    /* -- No code ---------------------------------------------------------- */
    { }
  /* -- Destructor --------------------------------------------------------- */
  ~CLHelperUnsafe(void) { }
  /* ----------------------------------------------------------------------- */
  DELETECOPYCTORS(CLHelperUnsafe)      // Don't need the default constructor
};/* ----------------------------------------------------------------------- */
template<class MemberType,
         class LockType,
         class ListType = list<MemberType*>,
         class IteratorType = typename ListType::iterator>
struct CLHelper :                      // Members initially public
  /* -- Base classes ------------------------------------------------------- */
  public LockType                      // CLHelperSafe or CLHelperUnsafe
{ /* -- (Un)Lock the mutex (nope) ------------------------------------------ */
  void CollectorLock(void) { this->CLLock(); }
  void CollectorUnlock(void) { this->CLUnlock(); }
  /* -- Return last item in list ------------------------------------------- */
  IteratorType CollectorGetLastItemUnsafe(void)
    { return this->CLBaseGetLastItemUnsafe(); }
  /* -- Destory all members ------------------------------------------------ */
  void CollectorDestroyUnsafe(void) { this->CLBaseDestroyUnsafe(); }
  /* -- Return if collector is empty --------------------------------------- */
  bool CollectorEmpty(void) { return this->CLEmpty(); }
  /* -- Check for overflow ------------------------------------------------- */
  void CollectorCheck(void) { this->CLCheck(); }
  void CollectorCheckUnsafe(void) { this->CLBaseCheckUnsafe(); }
  /* -- Return number of registered items ---------------------------------- */
  size_t CollectorCount(void) { return this->CLCount(); }
  size_t CollectorCountUnsafe(void) { return this->CLBaseCountUnsafe(); }
  /* -- Add/remove object to the list (ICHelper needs these) --------------- */
  IteratorType CollectorAddUnsafe(MemberType*const mtObj)
    { return this->CLBaseAddUnsafe(mtObj); }
  IteratorType CollectorEraseUnsafe(IteratorType &itObj)
    { return this->CLBaseEraseUnsafe(itObj); }
  /* -- Add/remove object to the list (Use these everywhere else) ---------- */
  IteratorType CollectorAdd(MemberType*const mtObj)
    { return this->CLAdd(mtObj); }
  IteratorType CollectorErase(IteratorType &itObj)
    { return this->CLErase(itObj); }
  /* -- Set maximum objects ------------------------------------------------ */
  CVarReturn CollectorSetLimit(const size_t stLimit)
    { return this->CLSetLimit(stLimit); }
   /* -- Constructor ------------------------------------------------------- */
  explicit CLHelper(const char*const cpT) :
    /* -- Initialisers ----------------------------------------------------- */
    LockType{ cpT }                    // Initialise lock type with name
    /* -- No code ---------------------------------------------------------- */
    { }
  /* -- Destructor --------------------------------------------------------- */
  ~CLHelper(void) { this->CLBaseCheckAndDestroyUnsafe(); }
  /* ----------------------------------------------------------------------- */
  DELETECOPYCTORS(CLHelper)            // Don't need the default constructor
}; /* ---------------------------------------------------------------------- */
/* == MEMBER HELPERS ======================================================= */
/* ######################################################################### */
/* ## This is the best class for ICHelper. It allows member classes to    ## */
/* ## register and unregister themselves in the collector and other       ## */
/* ## neat things.                                                        ## */
/* ######################################################################### */
/* == Parent class helper ================================================== */
template<class CollectorType,
         class MemberType,
         class IteratorType = typename CollectorType::iterator>
struct ICHelperBase                    // Members initially public
{ /* -- Swap registration with another class ------------------------------- */
  CollectorType   &cParent;            // Parent class of this object
  /* -- Protected variables ------------------------------------- */ protected:
  IteratorType     cIterator;          // Iterator to this object in parent
  /* -- Add from collector with set parent --------------------------------- */
  void ICHelperBaseRegister(void)
  { // Return if already registered else check count and add to collector
    if(cIterator != cParent.CollectorGetLastItemUnsafe()) return;
    cParent.CollectorCheckUnsafe();
    cIterator = cParent.CollectorAddUnsafe(static_cast<MemberType*>(this));
  }
  /* -- Erase from collector with set parent ------------------------------- */
  void ICHelperBaseUnregister(void)
  { // Return if not registered otherwise unregister from collector
    if(cIterator == cParent.CollectorGetLastItemUnsafe()) return;
    cIterator = cParent.CollectorEraseUnsafe(cIterator);
  }
  /* -- Initialisation helpers on construction ----------------------------- */
  static IteratorType
    ICHelperBaseInit(CollectorType &ctRef, MemberType*const mtPtr)
      { return ctRef.CollectorAddUnsafe(mtPtr); }
  static IteratorType ICHelperBaseInit(CollectorType &ctRef)
    { return ctRef.CollectorGetLastItemUnsafe(); }
   /* -- Swap registration ------------------------------------------------- */
  // We ideally want to just replace the pointers in the iterator without
  // actually disturbing any actual iterators because it will be much faster
  // than just erasing the iterator and readding it! We unfortunately cannot
  // use iter_swap() as it asserts() when it tries to swap an end iterator.
  void ICHelperBaseSwapRegistration(const MemberType &mtObj)
  { // Convert to non-const
    MemberType &mtObjRef = const_cast<MemberType&>(mtObj);
    // If this class is registered?
    if(cIterator != this->cParent.CollectorGetLastItemUnsafe())
    { // If other is registered? Nothing needs swapping because both iterators
      // are already in the collector list
      if(mtObjRef.cIterator != this->cParent.CollectorGetLastItemUnsafe())
      { // Just swap iterators and pointers to the members
        swap(cIterator, mtObjRef.cIterator);
        swap(*addressof(*cIterator), *addressof(*mtObjRef.cIterator));
        // Done
        return;
      } // Other not registered?
      // Copy this iterator over the other iterator because we are using the
      // same physical position in the collector list...
      mtObjRef.cIterator = cIterator;
      // ...and we just have to replace the pointer to the other class of which
      // is now registered, and not this.
      *addressof(*mtObjRef.cIterator) = &mtObjRef;
      // We are now unregistered
      cIterator = this->cParent.CollectorGetLastItemUnsafe();
      // Done
      return;
    } // This isn't registered and if other isn't too, just return
    else if(mtObjRef.cIterator ==
      this->cParent.CollectorGetLastItemUnsafe()) return;
    // Copy other iterator over this iterator because we are using the same
    // physical position in the collector list...
    cIterator = mtObjRef.cIterator;
    // ...and we just have to replace the pointer to the other class with the
    // pointer to this class.
    *addressof(*cIterator) = static_cast<MemberType*>(this);
    // Other member no longer registered. Any use of the iterator in the other
    // class is now undefined behaviour.
    mtObjRef.cIterator = this->cParent.CollectorGetLastItemUnsafe();
  }
  /* ----------------------------------------------------------------------- */
  explicit ICHelperBase(CollectorType &ctObj, IteratorType &&itObj) :
    /* -- Initialisers ----------------------------------------------------- */
    cParent{ ctObj },
    cIterator{ StdMove(itObj) }
    /* -- No code ---------------------------------------------------------- */
    { }
  /* ----------------------------------------------------------------------- */
  ~ICHelperBase(void) { }
  /* ----------------------------------------------------------------------- */
  DELETECOPYCTORS(ICHelperBase)        // Remove default functions
};/* ----------------------------------------------------------------------- */
/* == Collector class helper WITH locks ==================================== */
/* ######################################################################### */
/* ## Derive this class into the IcHelper (main collector) class to make  ## */
/* ## use the collector with concurrency locking. Only the 'Sources'      ## */
/* ## objects need this because the list can be accessed and manipulated  ## */
/* ## in the audio processing thread.                                     ## */
/* ######################################################################### */
/* ------------------------------------------------------------------------- */
template<class CollectorType,
         class MemberType,
         class IteratorType = typename CollectorType::iterator,
         class BaseType = ICHelperBase<CollectorType, MemberType>>
class ICHelperSafe :                   // Members initially private
  /* -- Base classes ------------------------------------------------------- */
  public BaseType                      // ICHelper base class
{ /* -- Initialise (un)registered entry with synchronisation --------------- */
  IteratorType ICHelperInit(CollectorType &ctRef, MemberType*const mtPtr) const
    { const LockGuard lgInitRegistered{ ctRef.CollectorGetMutex() };
      return this->ICHelperBaseInit(ctRef, mtPtr); }
  IteratorType ICHelperInit(CollectorType &ctRef) const
    { const LockGuard lgInitUnregistered{ ctRef.CollectorGetMutex() };
      return this->ICHelperBaseInit(ctRef); }
  /* -- Insert/Remove from collector list with synchronisation -- */ protected:
  void ICHelperPush(void)
    { const LockGuard lgPush{ this->cParent.CollectorGetMutex() };
      this->ICHelperBaseRegister(); }
  void ICHelperErase(void)
    { const LockGuard lgErase{ this->cParent.CollectorGetMutex() };
      this->ICHelperBaseUnregister(); }
  /* -- Swap to objects ---------------------------------------------------- */
  void ICHelperSwap(const MemberType &mtObj)
    { const LockGuard lgSwap{ this->cParent.CollectorGetMutex() };
      this->ICHelperBaseSwapRegistration(mtObj); }
  /* -- Constructors ------------------------------------------------------- */
  explicit ICHelperSafe(CollectorType &ctRef) :
    BaseType(ctRef, StdMove(ICHelperInit(ctRef))) { }
  explicit ICHelperSafe(CollectorType &ctRef, MemberType*const mtPtr) :
    BaseType(ctRef, StdMove(ICHelperInit(ctRef, mtPtr))) { }
  /* ----------------------------------------------------------------------- */
  DELETECOPYCTORS(ICHelperSafe)        // Remove default functions
};/* ----------------------------------------------------------------------- */
/* == Collector class helper without locks ================================= */
/* ######################################################################### */
/* ## Derive this class into the IcHelper (main collector) class to make  ## */
/* ## use the collector without concurrency locking. Use this only if you ## */
/* ## know the objects are not going to modify or access the list in a    ## */
/* ## thread other than the engine thread.                                ## */
/* ######################################################################### */
/* ------------------------------------------------------------------------- */
template<class CollectorType,
         class MemberType,
         class IteratorType = typename CollectorType::iterator,
         class BaseType = ICHelperBase<CollectorType,MemberType>>
class ICHelperUnsafe :                 // Members initially private
  /* -- Base classes ------------------------------------------------------- */
  public BaseType                      // ICHelper base class
{ /* -- Initialise (un)registered entry without synchronisation ------------ */
  IteratorType ICHelperInit(CollectorType &ctRef, MemberType*const mtPtr) const
    { return this->ICHelperBaseInit(ctRef, mtPtr); }
  IteratorType ICHelperInit(CollectorType &ctRef) const
    { return this->ICHelperBaseInit(ctRef); }
  /* -- Push/Remove object into collector list ------------------ */ protected:
  void ICHelperPush(void) { this->ICHelperBaseRegister(); }
  void ICHelperErase(void) { this->ICHelperBaseUnregister(); }
  /* -- Remove object from collector list ---------------------------------- */
  void ICHelperSwap(const MemberType &mtObj)
    { this->ICHelperBaseSwapRegistration(mtObj); }
  /* -- Constructors without registration ---------------------------------- */
  explicit ICHelperUnsafe(CollectorType &ctRef) :
    /* -- Initialisers ----------------------------------------------------- */
    BaseType{ ctRef, StdMove(ICHelperInit(ctRef)) }
    /* -- No code ---------------------------------------------------------- */
    { }
  /* -- Constructor with registration -------------------------------------- */
  explicit ICHelperUnsafe(CollectorType &ctRef, MemberType*const mtPtr) :
    /* -- Initialisers ----------------------------------------------------- */
    BaseType{ ctRef, StdMove(ICHelperInit(ctRef, mtPtr)) }
    /* -- No code ---------------------------------------------------------- */
    { }
  /* ----------------------------------------------------------------------- */
  DELETECOPYCTORS(ICHelperUnsafe)      // Remove default functions
};/* ----------------------------------------------------------------------- */
/* == Collector class ====================================================== */
/* ######################################################################### */
/* ## This collector helper class can manually or automate class          ## */
/* ## registration of members in the collector class. It will store a     ## */
/* ## list::iterator of itself so it can be removed from the collector    ## */
/* ## as fast as possible. Also members can freely use 'cParent' to       ## */
/* ## access the collector.                                               ## */
/* ######################################################################### */
/* ------------------------------------------------------------------------- */
template<class CollectorType,
         class MemberType,
         class LockType,
         class IteratorType = typename CollectorType::iterator>
struct ICHelper :                      // Members initially public
  /* -- Base classes ------------------------------------------------------- */
  public LockType                      // ICHelperSafe or ICHelperUnSafe
{ /* -- Swap registration with another class ------------------------------- */
  void CollectorSwapRegistration(const MemberType &mtObj)
    { this->ICHelperSwap(mtObj); }
  /* -- Register to list --------------------------------------------------- */
  void CollectorRegister(void) { this->ICHelperPush(); }
  void CollectorUnregister(void) { this->ICHelperErase(); }
  /* -- Destructor (unregister if registered) ------------------------------ */
  ~ICHelper(void) { CollectorUnregister(); }
  /* -- Constructor (move) ------------------------------------------------- */
  explicit ICHelper(ICHelper &&icOther) :
    /* -- Initialisers ----------------------------------------------------- */
    LockType{ StdMove(icOther) }
    /* -- No code ---------------------------------------------------------- */
    { }
  /* -- Constructor (manual registration) ---------------------------------- */
  explicit ICHelper(CollectorType &ctObj) :
    /* -- Initialisers ----------------------------------------------------- */
    LockType{ ctObj }
    /* -- No code ---------------------------------------------------------- */
    { }
  /* -- Constructor with automatic registration ---------------------------- */
  explicit ICHelper(CollectorType &ctObj, MemberType*const mtPtr) :
    /* -- Initialisers ----------------------------------------------------- */
    LockType{ ctObj, mtPtr }
    /* -- No code ---------------------------------------------------------- */
    { }
  /* ----------------------------------------------------------------------- */
  DELETECOPYCTORS(ICHelper)            // Remove default functions
};/* ----------------------------------------------------------------------- */
/* == Lua userdata helper ================================================== */
/* ######################################################################### */
/* ## This class stores pointers to classes which are used by LUA and can ## */
/* ## locked by the engine so LUA cannot automatically delete them during ## */
/* ## garbage collection.                                                 ## */
/* ######################################################################### */
/* ------------------------------------------------------------------------- */
class Lockable                         // Members initially private
{ /* -- Private variables -------------------------------------------------- */
  bool             bLocked;            // Class is locked from being dealloced
  /* -- Set locked status -------------------------------------------------- */
  void LockSet(const bool bState) { bLocked = bState; }
  /* -- Return true if lock is set --------------------------------- */ public:
  bool LockIsSet(void) const { return bLocked; }
  /* -- Return true if lock is not set ------------------------------------- */
  bool LockIsNotSet(void) const { return !LockIsSet(); }
  /* -- Set locked status -------------------------------------------------- */
  void LockSet(void) { LockSet(true); }
  /* -- Clear locked status ------------------------------------------------ */
  void LockClear(void) { LockSet(false); }
  /* -- Swap lock status with another class -------------------------------- */
  void LockSwap(Lockable &lOther) { swap(bLocked, lOther.bLocked); }
  /* -- Constuctor --------------------------------------------------------- */
  explicit Lockable(                   // Initialise with lock (def: false)
    /* -- Parameters ------------------------------------------------------- */
    const bool bState=false            // The user requested lock state
    ): /* -- Initialisers -------------------------------------------------- */
    bLocked(bState)                    // Set the initial lock state
    /* -- No code ---------------------------------------------------------- */
    { }
  /* ----------------------------------------------------------------------- */
  DELETECOPYCTORS(Lockable)            // Remove default functions
};/* -- End ---------------------------------------------------------------- */
};                                     // End of module namespace
/* == EoF =========================================================== EoF == */

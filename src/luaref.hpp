/* == LUAREF.HPP =========================================================== **
** ######################################################################### **
** ## MS-ENGINE              Copyright (c) MS-Design, All Rights Reserved ## **
** ######################################################################### **
** ## This class manages a user specified number of Lua references        ## **
** ######################################################################### **
** ========================================================================= */
#pragma once                           // Only one incursion allowed
/* ------------------------------------------------------------------------- */
namespace ILuaRef {                    // Start of private module namespace
/* ------------------------------------------------------------------------- */
using namespace ILuaUtil::P;
/* ------------------------------------------------------------------------- */
namespace P {                          // Start of public module namespace
/* ------------------------------------------------------------------------- */
template<size_t Refs=1>class LuaRef    // Lua easy reference class
{ /* -- Private variables --------------------------------------- */ protected:
  lua_State       *lsState;            // State that owns the reference.
  array<int, Refs> iReferences;        // Reference that the state refers to.
  /* -- Initialise a specific reference --------------------------- */ private:
  bool LuaRefDoInitEx(const size_t stId)
  { // Initialise the new reference and return failure if failed
    const int iReference = LuaUtilRefInitUnsafe(LuaRefGetState());
    if(iReference == LUA_REFNIL) return false;
    // Success to assign
    iReferences[stId] = iReference;
    // Success
    return true;
  }
  /* -- DeInitialise a specific reference ---------------------------------- */
  bool LuaRefDoDeInitEx(const size_t stId)
  { // Get reference to reference ;) and ignore if reference not set
    int &iReference = iReferences[stId];
    if(iReference == LUA_REFNIL) return false;
    // Clear the reference
    LuaUtilRmRefUnsafe(LuaRefGetState(), iReference);
    // Reference no longer valid
    iReference = LUA_REFNIL;
    // Success
    return true;
  }
  /* -- Set new state ------------------------------------------------------ */
  void LuaRefSetState(lua_State*const lS=nullptr) { lsState = lS; }
  /* -- Get the state ---------------------------------------------- */ public:
  lua_State *LuaRefGetState(void) const { return lsState; }
  /* -- Returns the reference at the specified index ----------------------- */
  int LuaRefGetId(const size_t stId=0) const { return iReferences[stId]; }
  /* -- Returns the function at the specified index ------------------------ */
  bool LuaRefGetFunc(const size_t stId=0) const
    { return LuaUtilGetRefFunc(LuaRefGetState(), LuaRefGetId(stId)); }
  /* -- Returns the userdata at the specified index ------------------------ */
  bool LuaRefGetUData(const size_t stId=0) const
    { return LuaUtilGetRefUsrData(LuaRefGetState(), LuaRefGetId(stId)); }
  /* -- Returns the reference at the specified index ----------------------- */
  void LuaRefGet(const size_t stId=0) const
    { return LuaUtilGetRef(LuaRefGetState(), LuaRefGetId(stId)); }
  /* -- Returns if the state is equal to the specified state --------------- */
  bool LuaRefStateIsEqual(const lua_State*const lS) const
    { return LuaRefGetState() == lS; }
  /* -- Returns if the state is NOT equal to the specified state ----------- */
  bool LuaRefStateIsNotEqual(const lua_State*const lS) const
    { return !LuaRefStateIsEqual(lS); }
  /* -- Returns if the state is set ---------------------------------------- */
  bool LuaRefStateIsSet(void) const { return LuaRefStateIsNotEqual(nullptr); }
  /* -- Returns if the state is NOT set ------------------------------------ */
  bool LuaRefStateIsNotSet(void) const { return !LuaRefStateIsSet(); }
  /* -- Returns if the specified reference is set -------------------------- */
  bool LuaRefIsSet(const size_t stId=0) const
    { return LuaRefStateIsSet() && LuaRefGetId(stId) != LUA_REFNIL; }
  /* -- De-initialise the reference ---------------------------------------- */
  bool LuaRefDeInit(void)
  { // Return if theres a state?
    if(LuaRefStateIsNotSet()) return false;
    // Unload and clear references from back to front
    for(auto itRef{iReferences.rbegin()}; itRef != iReferences.rend(); ++itRef)
    { // Get reference to reference in iterator and goto next if not set
      int &iReference = *itRef;
      if(iReference == LUA_REFNIL) continue;
      // Clear the reference
      LuaUtilRmRefUnsafe(LuaRefGetState(), iReference);
      // Reference no longer valid
      iReference = LUA_REFNIL;
    } // Clear the state
    LuaRefSetState();
    // Success
    return true;
  }
  /* -- Initialise the reference ------------------------------------------- */
  bool LuaRefInitEx(const size_t stId)
  { // Failed if no state
    if(LuaRefStateIsNotSet()) return false;
    // Deinit existing reference
    LuaRefDoDeInitEx(stId);
    // Reference the specified state
    return LuaRefDoInitEx(stId);
  }
  /* -- Initialise the reference ------------------------------------------- */
  bool LuaRefInit(lua_State*const lS, const size_t stMax = Refs)
  { // De-init any existing reference first
    LuaRefDeInit();
    // Failed if no state
    if(!lS) return false;
    // Set the state
    LuaRefSetState(lS);
    // Initialise references
    for(size_t stIndex = 0; stIndex < stMax; ++stIndex)
      LuaRefDoInitEx(stIndex);
    // Return success
    return true;
  }
  /* -- Constructor that does nothing but pre-initialise references -------- */
  LuaRef(void) :
    /* -- Initialisers ----------------------------------------------------- */
    lsState(nullptr)                   // State not initialised yet
    /* -- Uninitialised lua reverences ------------------------------------- */
    { iReferences.fill(LUA_REFNIL); }
  /* -- Destructor, delete the reference if set----------------------------- */
  ~LuaRef(void)
  { // Done if no state
    if(LuaRefStateIsNotSet()) return;
    // Delete references back to front
    for(auto itRef{ iReferences.rbegin() };
             itRef != iReferences.rend();
           ++itRef)
    { // Get reference and ignore if reference not set
      const int iReference = *itRef;
      if(iReference == LUA_REFNIL) continue;
      // Clear the reference
      LuaUtilRmRefUnsafe(LuaRefGetState(), iReference);
    }
  }
  /* ----------------------------------------------------------------------- */
  DELETECOPYCTORS(LuaRef)              // Omit copy constructor for safety
};/* ----------------------------------------------------------------------- */
}                                      // End of public module namespace
/* ------------------------------------------------------------------------- */
}                                      // End of private module namespace
/* == EoF =========================================================== EoF == */

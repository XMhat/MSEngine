/* == FTF.HPP ============================================================== */
/* ######################################################################### */
/* ## MS-ENGINE              Copyright (c) MS-Design, All Rights Reserved ## */
/* ######################################################################### */
/* ## This module defines a class that can load true-type fonts using     ## */
/* ## freetype. As of writing, Freetype supports the following fonts.     ## */
/* ##   OTF, CFF, OTC  OpenType and variants                              ## */
/* ##   TTF, TTC       TrueType and variants                              ## */
/* ##   WOFF           Open web font format                               ## */
/* ##   PFA, PFB       Type 1                                             ## */
/* ##   CID            Keyed Type 1                                       ## */
/* ##   SFNT           Bitmap fonts including colour emoji                ## */
/* ##   PCF            X11 Unix                                           ## */
/* ##   FNT            Windows bitmap font                                ## */
/* ##   BDF            Including anti-aliased                             ## */
/* ##   PFR            BitStream TrueDoc                                  ## */
/* ##   N/A            Type 42 PostScript fonts (limited support)         ## */
/* ######################################################################### */
/* ========================================================================= */
#pragma once                           // Only one incursion allowed
/* ------------------------------------------------------------------------- */
namespace IfFtf {                      // Start of module namespace
/* -- Includes ------------------------------------------------------------- */
using namespace Library::FreeType;     // Using freetype library functions
using namespace IfAsset;               // Using asset namespace
using namespace IfDim;                 // Using dimensions namespace
/* -- Freetype core class -------------------------------------------------- */
static class FreeType final            // Members initially private
{ /* -- Private variables -------------------------------------------------- */
  FT_Library    ftLibrary;             // Freetype instance
  FT_MemoryRec_ ftMemory;              // Freetype custom allocator
  mutex         mMutex;                // Instance protection
  /* ----------------------------------------------------------------------- */
  bool DoDeInit(void)
  { // Return failed if library not available
    if(!IsLibraryAvailable()) return false;
    // Unload freetype library
    cLog->LogDebugSafe("FreeType subsystem deinitialising...");
    FT_Done_Library(ftLibrary);
    cLog->LogInfoSafe("FreeType subsystem deinitialised.");
    // Success
    return true;
  }
  /* --------------------------------------------------------------- */ public:
  static FT_Error ApplyStrokerFull(FT_Glyph &gData, FT_Stroker ftStroker)
    { return FT_Glyph_Stroke(&gData, ftStroker, true); }
  /* ----------------------------------------------------------------------- */
  static FT_Error ApplyStrokerPartial(FT_Glyph &gData, FT_Stroker ftStroker,
    const bool bInside)
      { return FT_Glyph_StrokeBorder(&gData, ftStroker, bInside, true); }
  /* ----------------------------------------------------------------------- */
  static FT_Error ApplyStrokerOutside(FT_Glyph &gData, FT_Stroker ftStroker)
    { return ApplyStrokerPartial(gData, ftStroker, false); }
  /* ----------------------------------------------------------------------- */
  static FT_Error ApplyStrokerInside(FT_Glyph &gData, FT_Stroker ftStroker)
    { return ApplyStrokerPartial(gData, ftStroker, true); }
  /* ----------------------------------------------------------------------- */
  FT_Error NewStroker(FT_Stroker &ftsDst) const
    { return FT_Stroker_New(ftLibrary, &ftsDst); }
  /* ----------------------------------------------------------------------- */
  bool IsLibraryAvailable(void) { return ftLibrary != nullptr; }
  /* ----------------------------------------------------------------------- */
  FT_Error NewFont(const DataConst &dcSrc, FT_Face &ftfDst)
  { // Lock a mutex to protect FT_Library.
    // > freetype.org/freetype2/docs/reference/ft2-base_interface.html
    const LockGuard lgFreeTypeSync{ mMutex };
    // Create the font, throw exception on error
    return FT_New_Memory_Face(ftLibrary, dcSrc.Ptr<FT_Byte>(),
      dcSrc.Size<FT_Long>(), 0, &ftfDst);
  }
  /* ----------------------------------------------------------------------- */
  void DestroyFont(FT_Face ftFace)
  { // Lock a mutex to protect FT_Library.
    // > freetype.org/freetype2/docs/reference/ft2-base_interface.html
    const LockGuard lgFreeTypeSync{ mMutex };
    // Destroy the font
    FT_Done_Face(ftFace);
  }
  /* -- Error checker with custom error details ---------------------------- */
  template<typename ...VarArgs>
    static void CheckError(const FT_Error ftErr,
      const char*cpMessage, const VarArgs... vaArgs)
  { if(ftErr) XC(cpMessage, "Code", ftErr, "Reason", FT_Error_String(ftErr),
                vaArgs...); }
  /* ----------------------------------------------------------------------- */
  void Init(void)
  { // Class initialised
    if(IsLibraryAvailable()) XC("Freetype already initialised!");
    // Log initialisation and do the init
    cLog->LogDebugSafe("FreeType subsystem initialising...");
    // Create the memory
    CheckError(FT_New_Library(&ftMemory, &ftLibrary),
      "Failed to initialise FreeType!");
    // Documentation says we must run this function (fails if not)
    // https://www.freetype.org/freetype2/docs/design/design-4.html
    FT_Add_Default_Modules(ftLibrary);
    // Log successful initialisation
    cLog->LogInfoSafe("FreeType subsystem initialised.");
  }
  /* ----------------------------------------------------------------------- */
  void DeInit(void) { if(DoDeInit()) ftLibrary = nullptr; }
  /* ----------------------------------------------------------------------- */
  DTORHELPER(~FreeType, DoDeInit());
  /* ----------------------------------------------------------------------- */
  FreeType(void) : ftLibrary(nullptr), ftMemory{ this,
    [](FT_Memory, long lBytes)->void*
      { return MemAlloc<void>(lBytes); },
    [](FT_Memory, void*const vpAddress)
      { MemFree(vpAddress); },
    [](FT_Memory, long, long lBytes, void*const vpAddress)->void*
      { return MemReAlloc(vpAddress, lBytes); }
  } { }
  /* ----------------------------------------------------------------------- */
  DELETECOPYCTORS(FreeType);           // No copying of class allowed
  /* ----------------------------------------------------------------------- */
} *cFreeType = nullptr;                   // Pointer to static class
/* == Ftf collector class for collector data and custom variables ========== */
BEGIN_ASYNCCOLLECTORDUO(Ftfs, Ftf, CLHelperUnsafe, ICHelperUnsafe),
  /* -- Base classes ------------------------------------------------------- */
  public AsyncLoader<Ftf>,             // Asyncronous loading of data
  public Lockable,                     // Lua garbage collector instruction
  public Dimensions<GLfloat>           // Requested font width/height
{ /* -- Private variables -------------------------------------------------- */
  GLfloat          fOutline;           // FT outline size
  FT_Face          ftFace;             // FT Char handle
  FT_Stroker       ftStroker;          // FT Outline handle
  FileMap          fFTData;            // FT ttf file data (persistant)
  /* --------------------------------------------------------------- */ public:
  Dimensions<>     dDPI;               // FT DPI width and height
  /* -------------------------------------------------------------- */ private:
  void DoDeInit(void)
  { // Clear freetype handles if created
    if(LoadedStroker()) FT_Stroker_Done(ftStroker);
    if(Loaded()) cFreeType->DestroyFont(ftFace);
  }
  /* -- Returns if face is loaded----------------------------------- */ public:
  bool Loaded(void) const { return !!ftFace; }
  bool LoadedStroker(void) const { return !!ftStroker; }
  FT_Stroker GetStroker(void) const { return ftStroker; }
  unsigned int GetDPIWidth(void) const { return dDPI.DimGetWidth(); }
  unsigned int GetDPIHeight(void) const { return dDPI.DimGetHeight(); }
  GLfloat GetOutline(void) const { return fOutline; }
  bool IsOutline(void) const { return GetOutline() > 0.0f; }
  FT_GlyphSlot GetGlyphData(void) const { return ftFace->glyph; }
  const FT_String *GetFamily(void) const { return ftFace->family_name; }
  const FT_String *GetStyle(void) const { return ftFace->style_name; }
  FT_Long GetGlyphCount(void) const { return ftFace->num_glyphs; }
  /* -- Set ftf size ------------------------------------------------------- */
  void UpdateSize(void)
  { // For some twisted reason, FreeType measures char size in terms f 1/64ths
    // of pixels. Thus, to make a char 'h' pixels high, we need to request a
    // size of 'h*64'.
    cFreeType->CheckError(FT_Set_Char_Size(ftFace,
      static_cast<FT_F26Dot6>(DimGetWidth() * 64),
      static_cast<FT_F26Dot6>(DimGetHeight() * 64),
      static_cast<FT_UInt>(dDPI.DimGetWidth()),
      static_cast<FT_UInt>(dDPI.DimGetHeight())),
      "Failed to set character size!",
      "Identifier", IdentGet(),         "Width",    DimGetWidth(),
      "Height",     DimGetHeight(),     "DPIWidth", dDPI.DimGetWidth(),
      "DPIHeight",  dDPI.DimGetHeight());
  }
  /* -- Convert character to glyph index --------------------------- */ public:
  FT_UInt CharToGlyph(const FT_ULong dwChar)
    { return FT_Get_Char_Index(ftFace, dwChar); }
  /* -- Load a glyph ------------------------------------------------------- */
  FT_Error LoadGlyph(const FT_UInt uiIndex)
    { return FT_Load_Glyph(ftFace, uiIndex,
        FT_LOAD_CROP_BITMAP|FT_LOAD_NO_AUTOHINT|FT_LOAD_NO_HINTING); }
  /* -- Load ftf from memory ----------------------------------------------- */
  void AsyncReady(FileMap &fC)
  { // Take ownership of the file data
    fFTData.FileMapSwap(fC);
    // Load font
    cFreeType->CheckError(cFreeType->NewFont(fFTData, ftFace),
      "Failed to create font!",
      "Identifier", IdentGet(),
      "Context",    cFreeType->IsLibraryAvailable(),
      "Buffer",     fFTData.IsPtrSet(),
      "Size",       fFTData.Size());
    // Update size
    UpdateSize();
    // Outline requested?
    if(IsOutline())
    { // Create stroker handle
      cFreeType->CheckError(cFreeType->NewStroker(ftStroker),
        "Failed to create stroker!",
        "Identifier", IdentGet(), "Context", cFreeType->IsLibraryAvailable());
      // Set properties of stroker handle
      FT_Stroker_Set(ftStroker, static_cast<FT_Fixed>(GetOutline() * 64),
       FT_STROKER_LINECAP_ROUND, FT_STROKER_LINEJOIN_ROUND, 0);
    } // Report loaded font
    cLog->LogInfoExSafe("Ftf loaded '$' (FF:$;FS:$;S:$$$x$;D:$x$;B:$).",
      IdentGet(), GetFamily(), GetStyle(), setprecision(0), fixed,
      DimGetWidth(), DimGetHeight(), GetDPIWidth(),
      GetDPIHeight(), GetOutline());
  }
  /* -- Check and initialise supplied variables ---------------------------- */
  void InitVars(const GLfloat fW, const GLfloat fH, const unsigned int uiDW,
    const unsigned int uiDH, const GLfloat fO)
  { // Set width and height
    DimSet(fW, fH);
    // Set DPI width and height
    dDPI.DimSet(uiDW, uiDH);
    // Set outline
    fOutline = fO;
  }
  /* -- Load pcm from memory asynchronously -------------------------------- */
  void InitAsyncArray(lua_State*const lS)
  { // We need eleven parameters
    CheckParams(lS, 11);
    // Get name and init parameters
    const string strName{ GetCppStringNE(lS, 1, "Identifier") };
    Asset &aData = *GetPtr<Asset>(lS, 2, "Asset");
    const GLfloat fW = GetNumLG<GLfloat>(lS, 3, 1, 4096, "Width"),
                  fH = GetNumLG<GLfloat>(lS, 4, 1, 4096, "Height");
    const unsigned int
      uiDW = GetIntLG<unsigned int>(lS, 5, 1, 1024, "DPIWidth"),
      uiDH = GetIntLG<unsigned int>(lS, 6, 1, 1024, "DPIHeight");
    const GLfloat fO = GetNumLG<GLfloat>(lS, 7, 0, 1024, "OutLine");
    // Check callbacks
    CheckFunction(lS, 8, "ErrorFunc");
    CheckFunction(lS, 9, "ProgressFunc");
    CheckFunction(lS, 10, "SuccessFunc");
    // Set other members
    InitVars(fW, fH, uiDW, uiDH, fO);
    // Prepare asynchronous loading from array
    AsyncInitArray(lS, strName, "ftfarray", std::move(aData));
  }
  /* -- Load pcm from file asynchronously ---------------------------------- */
  void InitAsyncFile(lua_State*const lS)
  { // We need nine parameters
    CheckParams(lS, 10);
    // Get name and init parameters
    const string strName{ GetCppFileName(lS, 1, "File") };
    const GLfloat fW = GetNumLG<GLfloat>(lS, 2, 1, 4096, "Width"),
                  fH = GetNumLG<GLfloat>(lS, 3, 1, 4096, "Height");
    const unsigned int
      uiDW = GetIntLG<unsigned int>(lS, 4, 1, 1024, "DPIWidth"),
      uiDH = GetIntLG<unsigned int>(lS, 5, 1, 1024, "DPIHeight");
    const GLfloat fO = GetNumLG<GLfloat>(lS, 6, 0, 1024, "OutLine");
    // Check callbacks
    CheckFunction(lS, 7, "ErrorFunc");
    CheckFunction(lS, 8, "ProgressFunc");
    CheckFunction(lS, 9, "SuccessFunc");
    // Set other members
    InitVars(fW, fH, uiDW, uiDH, fO);
    // Prepare asynchronous loading from array
    AsyncInitFile(lS, strName, "ftffile");
  }
  /* -- Init from file ----------------------------------------------------- */
  void InitFile(const string &strFilename, const GLfloat fW, const GLfloat fH,
    const unsigned int uiDW, const unsigned int uiDH, const GLfloat fO)
  { // Set other members
    InitVars(fW, fH, uiDW, uiDH, fO);
    // Load file normally
    SyncInitFileSafe(strFilename);
  }
  /* -- Init from array ---------------------------------------------------- */
  void InitArray(const string &strName, Memory &&mbD, const GLfloat fW,
    const GLfloat fH, const unsigned int uiDW, const unsigned int uiDH,
    const GLfloat fO)
  { // Set other members
    InitVars(fW, fH, uiDW, uiDH, fO);
    // Load file as array
    SyncInitArray(strName, std::move(mbD));
  }
  /* -- De-init ftf font --------------------------------------------------- */
  void DeInit(void) { DoDeInit(); ftStroker = nullptr; ftFace = nullptr; }
  /* ----------------------------------------------------------------------- */
  void SwapFtf(Ftf &oCref)
  { // Copy variables over from source class
    DimSwap(oCref);
    dDPI.DimSwap(oCref.dDPI);
    std::swap(fOutline, oCref.fOutline);
    std::swap(ftFace, oCref.ftFace);
    std::swap(ftStroker, oCref.ftStroker);
    // Swap file class
    fFTData.FileMapSwap(oCref.fFTData);
    // Swap async, lua lock data and registration
    IdentSwap(oCref);
    LockSwap(oCref);
    CollectorSwapRegistration(oCref);
  }
  /* -- MOVE assignment (Ftf=Ftf) just do a swap --------------------------- */
  Ftf& operator=(Ftf &&oCref) { SwapFtf(oCref); return *this; }
  /* -- Default constructor ------------------------------------------------ */
  Ftf(void) :                          // No parameters
    /* -- Initialisation of members ---------------------------------------- */
    ICHelperFtf{ *cFtfs },             // Initially unregistered
    IdentCSlave{ cParent.CtrNext() },  // Initialise identification number
    AsyncLoader<Ftf>{ this,            // Initialise async loader with class
      EMC_MP_FONT },                   // " and the event id
    fOutline(0.0f),                    // No outline size yet
    ftFace(nullptr),                   // No FreeType handle yet
    ftStroker(nullptr)                 // No FreeType stroker handle yet
    /* --------------------------------------------------------------------- */
    { }                                // Do nothing else
  /* -- MOVE constructor --------------------------------------------------- */
  Ftf(Ftf &&oCref) :                   // The other Ftf class to swap with
    /* -- Initialisation of members ---------------------------------------- */
    Ftf()                              // Use default initialisers
    /* --------------------------------------------------------------------- */
    { SwapFtf(oCref); }                // Do the swap
  /* -- Destructor --------------------------------------------------------- */
  ~Ftf(void) { AsyncCancel(); DoDeInit(); }
  /* ----------------------------------------------------------------------- */
  DELETECOPYCTORS(Ftf);                // Disable copy constructor and operator
};/* -- End ---------------------------------------------------------------- */
END_ASYNCCOLLECTOR(Ftfs, Ftf, FONT);   // End of ftf collector
/* ------------------------------------------------------------------------- */
};                                     // End of module namespace
/* == EoF =========================================================== EoF == */

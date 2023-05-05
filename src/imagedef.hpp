/* == IMAGEDEF.HPP ========================================================= */
/* ######################################################################### */
/* ## MS-ENGINE              Copyright (c) MS-Design, All Rights Reserved ## */
/* ######################################################################### */
/* ## Sets up the image data storage memory and metadata.                 ## */
/* ######################################################################### */
/* ========================================================================= */
#pragma once                           // Only one incursion allowed
/* ------------------------------------------------------------------------- */
namespace IfImageDef {                 // Start of module namespace
/* -- Includes ------------------------------------------------------------- */
using namespace Lib::OS::GlFW;         // Access to OpenGL headers
using namespace IfFlags;               // Using flags namespace
using namespace IfMemory;              // Using memory namespace
using namespace IfDim;                 // Using dimensions namespace
/* -- Shared image flags --------------------------------------------------- */
BUILD_FLAGS(Image,
  /* -- Note --------------------------------------------------------------- */
  /* The 'ImageData' class contains a 'Flags' class which is shared between  */
  /* four different classes, 'Image', 'ImageData', 'Font' and 'Texture' so   */
  /* it's important we don't duplicate values here.                          */
  /* -- Post processing (Only used in 'Image' class) ----------------------- */
  // No flags?                         Image will be loadable in OpenGL?
  IL_NONE        {0x0000000000000000}, IL_TOGPU        {0x0000000000000001},
  // Convert loaded image to 24bpp?    Convert loaded image to 32bpp?
  IL_TO24BPP     {0x0000000000000002}, IL_TO32BPP      {0x0000000000000004},
  // Convert loaded image to BGR(A)?   Convert loaded image to RGB(A)?
  IL_TOBGR       {0x0000000000000008}, IL_TORGB        {0x0000000000000010},
  // Convert loaded image to BINARY?   Force reverse the image?
  IL_TOBINARY    {0x0000000000000020}, IL_REVERSE      {0x0000000000000040},
  // Convert to atlas?
  IL_ATLAS       {0x0000000000000080},
  /* -- Force load formats (Only used in 'Image' class) -------------------- */
  // Force load as PNG?                Force load as JPEG?
  IL_FCE_PNG     {0x0000000000000100}, IL_FCE_JPG     {0x0000000000000200},
  // Force load as GIF?                // Force load as DDS?
  IL_FCE_GIF     {0x0000000000000400}, IL_FCE_DDS     {0x0000000000000800},
  /* -- Image loader public mask bits -------------------------------------- */
  IL_MASK{ IL_TOGPU|IL_TO24BPP|IL_TO32BPP|IL_TOBGR|IL_TORGB|IL_TOBINARY|
    IL_REVERSE|IL_ATLAS|IL_FCE_JPG|IL_FCE_PNG|IL_FCE_GIF|IL_FCE_DDS },
  /* -- Font loading flags (Only used in 'Font' class) --------------------- */
  // Use image glyph size for advance? True stroke but more buggy?
  FF_USEGLYPHSIZE{0x0000000000001000}, FF_STROKETYPE2  {0x0000000000002000},
  // Do floor() on advance width?      Do ceil() on advance width?
  FF_FLOORADVANCE{0x0000000000004000}, FF_CEILADVANCE  {0x0000000000008000},
  // Do round() on advance width?
  FF_ROUNDADVANCE{0x0000000000010000},
  /* -- Font loader public mask bits --------------------------------------- */
  FF_MASK{ FF_USEGLYPHSIZE|FF_STROKETYPE2|FF_FLOORADVANCE|FF_CEILADVANCE|
           FF_ROUNDADVANCE },
  /* -- Active flags (Only used in 'Image' class) ----------------------- */
  // Image will be loadable in GL?     Convert loaded image to 24bpp?
  IA_TOGPU       {0x0000000000020000}, IA_TO24BPP      {0x0000000000040000},
  // Convert loaded image to 32bpp?    Convert loaded image to BGR(A)?
  IA_TO32BPP     {0x0000000000080000}, IA_TOBGR        {0x0000000000100000},
  // Convert loaded image to RGB(A)?   Convert loaded image to BINARY?
  IA_TORGB       {0x0000000000200000}, IA_TOBINARY     {0x0000000000400000},
  // Force reverse the image?          Convert to atlas?
  IA_REVERSE     {0x0000000000800000}, IA_ATLAS        {0x0000000001000000},
  /* -- Image loaded flags (Only used in 'ImageData' class) ---------------- */
  // Bitmap has mipmaps?               Bitmap has reversed pixels?
  IF_MIPMAPS     {0x0000000002000000}, IF_REVERSED     {0x0000000004000000},
  // Bitmap is compressed?             Bitmap is dynamically created?
  IF_COMPRESSED  {0x0000000008000000}, IF_DYNAMIC      {0x0000000010000000},
  // A palette is included?            Move loaded data back
  IF_PALETTE     {0x0000000020000000},
  /* -- Texture loaded flags (Only used in 'Image' class) ------------------ */
  // Generate tiles for texture?       Marked for deletion
  TF_DELETE      {0x0000000040000000},
  /* -- Image purpose (help with debugging) -------------------------------- */
  // Image is stand-alone              Image is part of a Texture class
  IP_IMAGE       {0x0000000080000000}, IP_TEXTURE      {0x0000000100000000},
  // Image is part of a Font class
  IP_FONT        {0x0000000200000000}
);/* ----------------------------------------------------------------------- */
struct ImageSlot :                     // Members initially public
  /* -- Initialisers ------------------------------------------------------- */
  public Memory,                       // Memory data
  public Dimensions<>                  // Dimensions of data
{ /* -- Init constructor --------------------------------------------------- */
  ImageSlot(Memory &&mData, const unsigned int uiW, const unsigned int uiH) :
    /* -- Initialisers ----------------------------------------------------- */
    Memory{ StdMove(mData) },             // Move memory in place
    Dimensions<>{ uiW, uiH }           // Set dimensions
    /* -- No code ---------------------------------------------------------- */
    { }
};/* ----------------------------------------------------------------------- */
typedef vector<ImageSlot> SlotList;    // list of bitmaps
/* ------------------------------------------------------------------------- */
enum BitDepth : unsigned int           // Human readable bit-depths
{ /* ----------------------------------------------------------------------- */
  BD_NONE       =  0,                  // Not initialised yet
  BD_BINARY     =  1,                  // Binary format (8 pixels per byte)
  BD_GRAY       =  8,                  // Gray channel format
  BD_GRAYALPHA  = 16,                  // Gray + alpha channel format
  BD_RGB        = 24,                  // 1 pixel per 3 bytes (R+G+B)
  BD_RGBA       = 32                   // 1 pixel per 4 bytes (R+G+B+Alpha)
};/* ----------------------------------------------------------------------- */
enum ByteDepth : unsigned int          // Human readable byte-depths
{ /* ----------------------------------------------------------------------- */
  BY_NONE,                             // [0] Not initialised yet
  BY_GRAY,                             // [1] Gray channel format
  BY_GRAYALPHA,                        // [2] Gray + alpha channel format
  BY_RGB,                              // [3] 1 pixel per 3 bytes (R+G+B)
  BY_RGBA,                             // [4] 1 pixel per 4 bytes (R+G+B+Alpha)
};/* ----------------------------------------------------------------------- */
class ImageData :                      // Members initially private
  /* ----------------------------------------------------------------------- */
  public ImageFlags,                   // Shared with 'Image' class if needed
  public Dimensions<>                  // Image dimensions
{ /* ----------------------------------------------------------------------- */
  BitDepth         bdDepth;            // Image bits per pixel
  ByteDepth        byDepth;            // Image bytes per pixel
  GLenum           glPixelType;        // Image colour-byte bits
  size_t           stAlloc;            // Image data allocated in slots
  /* ------------------------------------------------------------ */ protected:
  SlotList         slSlots;            // Image data
  /* --------------------------------------------------------------- */ public:
  Dimensions<>     duTileOR;           // Override tile size if desired
  size_t           stTiles;            // Override number of tiles
  /* ----------------------------------------------------------------------- */
  void ImageDataSwap(ImageData &imdRef)
  { // Swap image lib data
    FlagSwap(imdRef);
    DimSwap(imdRef);
    slSlots.swap(imdRef.slSlots);
    // Swap values
    swap(bdDepth, imdRef.bdDepth);
    swap(byDepth, imdRef.byDepth);
    swap(glPixelType, imdRef.glPixelType);
    swap(stAlloc, imdRef.stAlloc);
    // Swap tiles and dimensions
    duTileOR.DimSwap(imdRef.duTileOR);
    swap(stTiles, imdRef.stTiles);
  }
  /* -- Set width and height and return if they are valid ------------------ */
  bool SetDimSafe(const unsigned int uiNWidth, const unsigned int uiNHeight)
    { DimSet(uiNWidth, uiNHeight); return uiNWidth && uiNHeight; }
  /* -- Set bits per pixel ------------------------------------------------- */
  void SetBitsPerPixel(const BitDepth bdNBPP) { bdDepth = bdNBPP; }
  template<typename IntType>void SetBitsPerPixelCast(const IntType uiNBPP)
    { SetBitsPerPixel(static_cast<BitDepth>(uiNBPP)); }
  /* -- Set bytes per pixel ------------------------------------------------ */
  void SetBytesPerPixel(const ByteDepth byNBPP) { byDepth = byNBPP; }
  template<typename IntType>void SetBytesPerPixelCast(const IntType uiNBPP)
    { SetBytesPerPixel(static_cast<ByteDepth>(uiNBPP)); }
  /* -- Set bits per pixel and auto update bytes per pixel ----------------- */
  void SetBitsAndBytesPerPixel(const BitDepth bdNBPP)
    { SetBitsPerPixel(bdNBPP); SetBytesPerPixelCast(bdNBPP / 8); }
  template<typename IntType>
    void SetBitsAndBytesPerPixelCast(const IntType uiNBPP)
      { SetBitsAndBytesPerPixel(static_cast<BitDepth>(uiNBPP)); }
  /* -- Set bytes per pixel and auto update bits per pixel ----------------- */
  void SetBytesAndBitsPerPixel(const ByteDepth byNBPP)
    { SetBytesPerPixel(byNBPP); SetBitsPerPixelCast(byNBPP * 8); }
  template<typename IntType>
    void SetBytesAndBitsPerPixelCast(const IntType uiNBPP)
      { SetBytesAndBitsPerPixel(static_cast<ByteDepth>(uiNBPP)); }
  /* ----------------------------------------------------------------------- */
  size_t GetAlloc(void) const { return stAlloc; }
  /* ----------------------------------------------------------------------- */
#define FH(n, f) \
  bool Is ## n(void) const { return FlagIsSet(f); } \
  bool IsNot ## n(void) const { return !Is ## n(); } \
  void Set ## n(bool bState=true) { FlagSetOrClear(f, bState); } \
  void Clear ## n(void) { Set ## n(false); }
  /* ----------------------------------------------------------------------- */
  FH(Mipmaps,          IF_MIPMAPS)     // Is/IsNot/Set/ClearMipmaps
  FH(Reversed,         IF_REVERSED)    // Is/IsNot/Set/ClearReversed
  FH(Compressed,       IF_COMPRESSED)  // Is/IsNot/Set/ClearCompressed
  FH(Dynamic,          IF_DYNAMIC)     // Is/IsNot/Set/ClearDynamic
  FH(Palette,          IF_PALETTE)     // Is/IsNot/Set/ClearPalette
  FH(LoadAsDDS,        IL_FCE_DDS)     // Is/IsNot/Set/ClearLoadAsDDS
  FH(LoadAsGIF,        IL_FCE_GIF)     // Is/IsNot/Set/ClearLoadAsGIF
  FH(LoadAsJPG,        IL_FCE_JPG)     // Is/IsNot/Set/ClearLoadAsJPG
  FH(LoadAsPNG,        IL_FCE_PNG)     // Is/IsNot/Set/ClearLoadAsPNG
  FH(ConvertAtlas,     IL_ATLAS)       // Is/IsNot/Set/ClearConvertAtlas
  FH(ConvertReverse,   IL_REVERSE)     // Is/IsNot/Set/ClearConvertReverse
  FH(ConvertRGB,       IL_TO24BPP)     // Is/IsNot/Set/ClearConvertRGB
  FH(ConvertRGBA,      IL_TO32BPP)     // Is/IsNot/Set/ClearConvertRGBA
  FH(ConvertBGROrder,  IL_TOBGR)       // Is/IsNot/Set/ClearConvertBGROrder
  FH(ConvertBinary,    IL_TOBINARY)    // Is/IsNot/Set/ClearConvertBinary
  FH(ConvertGPUCompat, IL_TOGPU)       // Is/IsNot/Set/ClearConvertGPUCompat
  FH(ConvertRGBOrder,  IL_TORGB)       // Is/IsNot/Set/ClearConvertRGBOrder
  FH(ActiveAtlas,      IA_ATLAS)       // Is/IsNot/Set/ClearActiveAtlas
  FH(ActiveReverse,    IA_REVERSE)     // Is/IsNot/Set/ClearActiveReverse
  FH(ActiveRGB,        IA_TO24BPP)     // Is/IsNot/Set/ClearActiveRGB
  FH(ActiveRGBA,       IA_TO32BPP)     // Is/IsNot/Set/ClearActiveRGBA
  FH(ActiveBGROrder,   IA_TOBGR)       // Is/IsNot/Set/ClearActiveBGROrder
  FH(ActiveBinary,     IA_TOBINARY)    // Is/IsNot/Set/ClearActiveBinary
  FH(ActiveGPUCompat,  IA_TOGPU)       // Is/IsNot/Set/ClearActiveGPUCompat
  FH(ActiveRGBOrder,   IA_TORGB)       // Is/IsNot/Set/ClearActiveRGBOrder
  FH(PurposeFont,      IP_FONT)        // Is/IsNot/Set/ClearPurposeFont
  FH(PurposeImage,     IP_IMAGE)       // Is/IsNot/Set/ClearPurposeImage
  FH(PurposeTexture,   IP_TEXTURE)     // Is/IsNot/Set/ClearPurposeTexture
  /* ----------------------------------------------------------------------- */
#undef FH                              // Done with this macro
  /* ----------------------------------------------------------------------- */
  void SetPixelType(const GLenum eMode) { glPixelType = eMode; }
  /* ----------------------------------------------------------------------- */
  GLenum GetPixelType(void) const { return glPixelType; }
  /* ----------------------------------------------------------------------- */
  size_t TotalPixels(void) const
    { return DimGetWidth<size_t>() * DimGetHeight<size_t>(); }
  /* ----------------------------------------------------------------------- */
  template<typename IntType=decltype(bdDepth)>
    IntType GetBitsPerPixel(void) const
      { return static_cast<IntType>(bdDepth); }
  /* ----------------------------------------------------------------------- */
  template<typename IntType=decltype(byDepth)>
    IntType GetBytesPerPixel(void) const
      { return static_cast<IntType>(byDepth); }
  /* -- Get slots ---------------------------------------------------------- */
  SlotList &GetSlots(void) { return slSlots; }
  /* -- Add a new slot ----------------------------------------------------- */
  void AddSlot(Memory &mData, const unsigned int uiSWidth,
    const unsigned int uiSHeight)
  { // Add the slot moving the memory over
    GetSlots().push_back({ StdMove(mData), uiSWidth, uiSHeight });
    // Add to memory bytes allocated counter
    stAlloc += GetSlots().back().Size();
  }
  /* -- Add a new slot using our image size -------------------------------- */
  void AddSlot(Memory &mData)
    { AddSlot(mData, DimGetWidth(), DimGetHeight()); }
  /* -- Get read-only slots ------------------------------------------------ */
  const SlotList &GetSlotsConst(void) const { return slSlots; }
  /* -- Is no slots? ------------------------------------------------------- */
  bool IsNoSlots(void) const { return GetSlotsConst().empty(); }
  /* -- Clear slots -------------------------------------------------------- */
  void ClearSlots(void) { GetSlots().clear(); }
  /* -- Recover slot memory ------------------------------------------------ */
  void CompactSlots(void) { GetSlots().shrink_to_fit(); }
  /* -- Get slots count ---------------------------------------------------- */
  size_t GetSlotCount(void) const { return GetSlotsConst().size(); }
  /* -- Set allocated data size -------------------------------------------- */
  void SetAlloc(const size_t stNAlloc) { stAlloc = stNAlloc; }
  /* -- Reserve allocated slots -------------------------------------------- */
  void ReserveSlots(const size_t stCount) { GetSlots().reserve(stCount); }
  /* -- Increase allocated data size --------------------------------------- */
  void IncreaseAlloc(const size_t stNAlloc) { stAlloc += stNAlloc; }
  /* -- Decrease allocated data size --------------------------------------- */
  void DecreaseAlloc(const size_t stNAlloc) { stAlloc -= stNAlloc; }
  /* -- Adjust allocation from old and new value --------------------------- */
  void AdjustAlloc(const size_t stOldAlloc, const size_t stNewAlloc)
  { // If new alloc is greater then the total value increased
    if(stNewAlloc > stOldAlloc) IncreaseAlloc(stNewAlloc - stOldAlloc);
    // If the new alloc is lesser then the total value decreased
    else if(stNewAlloc < stOldAlloc) DecreaseAlloc(stOldAlloc - stNewAlloc);
  }
  /* -- Clear slots and allocation size ------------------------------------ */
  void Clear(void) { SetAlloc(0); ClearSlots(); }
  /* -- Reset all data ----------------------------------------------------- */
  void ResetAllData(void)
  { // Reset dimensions and pixel data
    DimSet();
    SetBitsPerPixel(BD_NONE);
    SetBytesPerPixel(BY_NONE);
    SetPixelType(GL_NONE);
    // Remove image property flags
    ClearMipmaps();
    ClearReversed();
    ClearCompressed();
    ClearPalette();
    // Clear slots list and set allocation to zero
    Clear();
  }
  /* -- Default constructor ------------------------------------------------ */
  ImageData(void) :                    // No parameters
    /* -- Initialisers ----------------------------------------------------- */
    ImageFlags{ IP_IMAGE },            // Set initial flags
    bdDepth(BD_NONE),                  // Bit depth not initialised yet
    byDepth(BY_NONE),                  // Bytes per pixel not initialised yet
    glPixelType(GL_NONE),              // Pixel type not initialised yet
    stAlloc(0),                        // Allocated memory not initialised yet
    stTiles(0)                         // No number of tiles
    /* -- Code ------------------------------------------------------------- */
    { }                                // Nothing else to do
};/* ----------------------------------------------------------------------- */
};                                     // End of module namespace
/* == EoF =========================================================== EoF == */

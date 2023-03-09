/* == LLIMAGE.HPP ========================================================== */
/* ######################################################################### */
/* ## MS-ENGINE              Copyright (c) MS-Design, All Rights Reserved ## */
/* ######################################################################### */
/* ## Defines the 'Image' namespace and methods for the guest to use in   ## */
/* ## Lua. This file is invoked by 'lualib.hpp'.                          ## */
/* ######################################################################### */
/* ------------------------------------------------------------------------- */
#pragma once                           // Only one incursion allowed
/* ========================================================================= */
/* ######################################################################### */
/* ========================================================================= */
// % Image
/* ------------------------------------------------------------------------- */
// ! This allows the programmer to manipulate images asynchronously and send
// ! the objects to the OpenGL if needed.
// !
// ! The list and order of supported formats can is subject to change without
// ! notice. Type 'imgfmts' in the console to see the most up to date list of
// ! formats supported.
/* ========================================================================= */
LLNAMESPACEBEGIN(Image)                // Image namespace
/* -- Includes ------------------------------------------------------------- */
using namespace IfImage;               // Using image interface
/* ========================================================================= */
/* ######################################################################### */
/* ========================================================================= */
// $ Image:Width
// < Width:integer=The width of the image in pixels
// ? Returns the width if the image.
/* ------------------------------------------------------------------------- */
LLFUNCEX(Width, 1, LCPUSHINT(LCGETPTR(1, Image)->GetWidth()));
/* ========================================================================= */
// $ Image:Height
// < Height:integer=The height of the image in pixels
// ? Returns the height if the image.
/* ------------------------------------------------------------------------- */
LLFUNCEX(Height, 1, LCPUSHINT(LCGETPTR(1, Image)->GetHeight()));
/* ========================================================================= */
// $ Image:Depth
// < Height:integer=The bit-depth of the image in bits
// ? Returns the bit-depth of the image
/* ------------------------------------------------------------------------- */
LLFUNCEX(Depth, 1, LCPUSHINT(LCGETPTR(1, Image)->GetBitsPP()));
/* ========================================================================= */
// $ Image:Name
// < Name:string=Name of the image.
// ? If this font was loaded by a filename or it was set with a custom id.
// ? This function returns that name which was assigned to it.
/* ------------------------------------------------------------------------- */
LLFUNCEX(Name, 1, LCPUSHXSTR(LCGETPTR(1, Image)->IdentGet()));
/* ========================================================================= */
// $ Image:Destroy
// ? Destroys the image and frees all the memory associated with it. The
// ? object will no longer be useable after this call and an error will be
// ? generated if accessed.
/* ------------------------------------------------------------------------- */
LLFUNC(Destroy, LCCLASSDESTROY(1, Image));
/* ========================================================================= */
/* ######################################################################### */
/* ## Image:* member functions structure                                  ## */
/* ######################################################################### */
/* ------------------------------------------------------------------------- */
LLRSMFBEGIN                            // Image:* member functions begin
  LLRSFUNC(Depth),                     // Get image depth
  LLRSFUNC(Destroy),                   // Destroy internal object
  LLRSFUNC(Name),                      // Get image identifier
  LLRSFUNC(Width),                     // Get image width
  LLRSFUNC(Height),                    // Get image height
LLRSEND                                // Image:* member functions end
/* ========================================================================= */
// $ Image.FileAsync
// > Filename:string=The filename of the image to load
// > Flags:Integer=Load flags
// > ErrorFunc:function=The function to call when there is an error
// > SuccessFunc:function=The function to call when the image is laoded
// ? Loads a image off the main thread. The callback functions send an
// ? argument to the image object that was created.
/* ------------------------------------------------------------------------- */
LLFUNC(FileAsync, LCCLASSCREATE(Image)->InitAsyncFile(lS));
/* ======================================================================= */
// $ Image.File
// > Filename:string=The filename of the image to load
// > Flags:Integer=Load flags
// < Handle:Image=The image object
// ? Loads a image on the main thread from the specified file on disk. Returns
// ? the image object.
/* ------------------------------------------------------------------------- */
LLFUNCEX(File, 1, LCCLASSCREATE(Image)->InitFile(LCGETCPPFILE(1, "File"),
  LCGETFLAGS(ImageFlagsConst, 2, IL_NONE, IL_MASK, "Flags")));
/* ======================================================================= */
// $ Image.CreateArray
// > Name:string=A user-defined id of the image
// > Data:array=The image data array
// > Flags:Integer=Load flags
// > ErrorFunc:function=The function to call when there is an error
// > SuccessFunc:function=The function to call when the image is laoded
// ? Loads a image off the main thread from the specified array object. The
// ? callback functions send an argument to the image object that was created.
/* ------------------------------------------------------------------------- */
LLFUNC(ArrayAsync, LCCLASSCREATE(Image)->InitAsyncArray(lS));
/* ======================================================================= */
// $ Image.Asset
// > Name:String=The user-defined id of the image
// > Data:Asset=The data of the image to load
// > Flags:Integer=Load flags
// < Handle:Image=The image object
// ? Loads a image on the main thread from the specified array object.
// ? Returns the image object.
/* ------------------------------------------------------------------------- */
LLFUNCEX(Asset, 1, LCCLASSCREATE(Image)->InitArray(
  LCGETCPPSTRINGNE(1, "Identifier"), move(*LCGETPTR(2, Asset)),
  LCGETFLAGS(ImageFlagsConst, 3, IL_NONE, IL_MASK, "Flags")));
/* ======================================================================= */
// $ Image.Raw
// > Data:Asset=The data of the image to load
// < Handle:Image=The image object
// ? Loads a image on the main thread from the specified array object.
// ? Returns the image object.
/* ------------------------------------------------------------------------- */
LLFUNCEX(Raw, 1, LCCLASSCREATE(Image)->
  InitRaw(LCGETCPPSTRINGNE(1, "Identifier"),
    move(*LCGETPTR(2, Asset)),
    LCGETINTLG(unsigned int, 3, 1, 65535, "Width"),
    LCGETINTLG(unsigned int, 4, 1, 65535, "Height"),
    LCGETINTLGE(unsigned int, 5, 1, 32, "Depth"),
    LCGETINTLG(GLenum, 6, 1, 65535, "PixelType")));
/* ======================================================================= */
// $ Image.Colour
// > Colour:integer=The colour of the pixel (Syntax: 0xAABBGGRR)
// < Handle:Image=The image object
// ? Loads a 1x1 pixel for use as a solid.
/* ------------------------------------------------------------------------- */
LLFUNCEX(Colour, 1,
  LCCLASSCREATE(Image)->InitColour(LCGETINT(uint32_t, 1, "Colour")));
/* ======================================================================= */
// $ Image.Blank
// > Name:string=The name of the texture
// > Width:integer=The width of the texture
// > Height:integer=The height of the texture
// > Filter:integer=The texture filtering mode
// < Handle:Texture=A handle to the texture(s) created.
// ? Creates an empty blank texture for manipulation.
/* ------------------------------------------------------------------------- */
LLFUNCEX(Blank, 1, LCCLASSCREATE(Image)->InitBlank(
  LCGETCPPSTRINGNE(        1,              "Identifier"),
  LCGETINTLG(unsigned int, 2, 0, UINT_MAX, "Width"),
  LCGETINTLG(unsigned int, 3, 0, UINT_MAX, "Height"),
  LCGETBOOL (              4,              "Alpha"),
  LCGETBOOL (              5,              "Clear")));
/* ======================================================================= */
// $ Image.WaitAsync
// ? Halts main-thread execution until all async image events have completed
/* ------------------------------------------------------------------------- */
LLFUNC(WaitAsync, cImages->WaitAsync());
/* ========================================================================= */
/* ######################################################################### */
/* ## Image.* namespace functions structure                               ## */
/* ######################################################################### */
/* ------------------------------------------------------------------------- */
LLRSBEGIN                              // Image.* namespace functions begin
  LLRSFUNC(Asset),                     // Load from array asyncronously
  LLRSFUNC(ArrayAsync),                //   "  asyncronously
  LLRSFUNC(Blank),                     // Create a blank image
  LLRSFUNC(Colour),                    // Create a single pixel solid
  LLRSFUNC(File),                      // Load from specified file
  LLRSFUNC(FileAsync),                 //   "  asyncronously
  LLRSFUNC(Raw),                       // Load from raw data
  LLRSFUNC(WaitAsync),                 // Wait for async events to complete
LLRSEND                                // Image.* namespace functions end
/* ========================================================================= */
/* ######################################################################### */
/* ## Image.* namespace constants structure                               ## */
/* ######################################################################### */
/* ======================================================================= */
// @ Image.Flags
// < Codes:table=The table of key/value pairs of available flags
// ? Returns the image plugins available. Returned as key/value pairs. The
// ? value is a unique identifier to the flag.
/* ------------------------------------------------------------------------- */
LLRSKTBEGIN(Flags)                     // Beginning of cursor codes
LLRSKTITEM(IL_,NONE),                  // No image loading flags
LLRSKTITEM(IL_,TOGPU),                 // Force to be loadable by GPU
LLRSKTITEM(IL_,TO24BPP),               // Force to 24-bits per pixel
LLRSKTITEM(IL_,TO32BPP),               // Force to 32-bits per pixel
LLRSKTITEM(IL_,TOBGR),                 // Force order to Blue/Green/Red
LLRSKTITEM(IL_,TORGB),                 // Force order to Red/Green/Blue
LLRSKTITEM(IL_,REVERSE),               // Reverse bitmap
LLRSKTITEM(IL_,TOBINARY),              // Force to 1-bit per pixel
LLRSKTITEM(IL_,FCE_DDS),               // Source is a DDS bitmap
LLRSKTITEM(IL_,FCE_GIF),               // Source is a GIF bitmap
LLRSKTITEM(IL_,FCE_JPG),               // Source is a JPEG bitmap
LLRSKTITEM(IL_,FCE_PNG),               // Source is a PNG bitmap
LLRSKTITEM(IL_,FCE_TGA),               // Source is a TGA bitmap
LLRSKTEND                              // End of cursor codes
/* ========================================================================= */
LLRSCONSTBEGIN                         // Image.* namespace consts begin
LLRSCONST(Flags),                      // Image loading flags
LLRSCONSTEND                           // Image.* namespace consts end
/* ========================================================================= */
LLNAMESPACEEND                         // End of Image namespace
/* == EoF =========================================================== EoF == */

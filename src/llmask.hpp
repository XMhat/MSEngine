/* == LLMASK.HPP =========================================================== */
/* ######################################################################### */
/* ## MS-ENGINE              Copyright (c) MS-Design, All Rights Reserved ## */
/* ######################################################################### */
/* ## Defines the 'Mask' namespace and methods for the guest to use in    ## */
/* ## Lua. This file is invoked by 'lualib.hpp'.                          ## */
/* ######################################################################### */
/* ------------------------------------------------------------------------- */
#pragma once                           // Only one incursion allowed
/* ========================================================================= */
/* ######################################################################### */
/* ========================================================================= */
// % Mask
/* ------------------------------------------------------------------------- */
// ! The mask class is a pixel-perfect 1-bit bitmask processing API which
// ! allows the programmer to create destructable terrain and collision testing
// ! whether the test maybe between two sprites or actual terrain background.
// ! You can create bitmasks as big as the system will allow.
/* ========================================================================= */
LLNAMESPACEBEGIN(Mask)                 // Mask namespace
/* -- Includes ------------------------------------------------------------- */
using namespace IfMask;                // Using mask namespace
/* ========================================================================= */
/* ######################################################################### */
/* ========================================================================= */
// $ Mask:IsCollide
// > SourceId:integer=The sub-mask id to use
// > SourceX:integer=The X position of the source mask
// > SourceY:integer=The Y position of the source mask
// > Dest:Mask=The destination mask to test upon
// > DestId:integer=The destination sub-mask id
// > DestX:integer=The X position to overlap the source mask
// > DestY:integer=The Y position to overlap the source mask
// < Result:boolean=True if the two bitmasks overlap. False if not
// ? Performs a bitmask comparison between two masks
/* ------------------------------------------------------------------------- */
LLFUNCBEGIN(IsCollideEx)
  const Mask &mSrc = *LCGETPTR(1, Mask), &mDst = *LCGETPTR(5, Mask);
  LCPUSHBOOL(mSrc.IsCollide(
    LCGETINTLGE(size_t, 2, 0, mSrc.size(), "SrcID"),
    LCGETINT   (int,    3,                 "SrcXPos"),
    LCGETINT   (int,    4,                 "SrcYPos"), mDst,
    LCGETINTLGE(size_t, 6, 0, mDst.size(), "DestID"),
    LCGETINT   (int,    7,                 "DestXPos"),
    LCGETINT   (int,    8,                 "DestYPos")));
LLFUNCENDEX(1)
/* ========================================================================= */
// $ Mask:IsCollide
// > Dest:Mask=The destination mask to test upon
// > DestId:integer=The destination sub-mask id
// > DestX:integer=The X position to overlap the source mask
// > DestY:integer=The Y position to overlap the source mask
// < Result:boolean=True if the two bitmasks overlap. False if not
// ? Returns true if the second mask collides with the first mask at x0 y0.
/* ------------------------------------------------------------------------- */
LLFUNCBEGIN(IsCollide)
  const Mask &mSrc = *LCGETPTR(1, Mask), &mDst = *LCGETPTR(2, Mask);
  LCPUSHBOOL(mSrc.IsCollide(0, 0, 0, mDst,
    LCGETINTLGE(size_t, 3, 0, mDst.size(), "DestID"),
    LCGETINT   (int,    4,                 "DestXPos"),
    LCGETINT   (int,    5,                 "DestYPos")));
LLFUNCENDEX(1)
/* ========================================================================= */
// $ Mask:RayCast
// > SrcId:integer=The source mask id to test
// > SrcX:integer=The X position of the source mask
// > SrcY:integer=The Y position of the source mask
// > Dest:Mask=The destination mask handle to test upon
// > DestId:integer=The destination mask id to test upon
// > DestFX:integer=The starting X position of the destination mask
// > DestFY:integer=The starting Y position of the destination mask
// > DestTX:integer=The ending X position of the destination mask
// > DestTY:integer=The ending Y position of the destination mask
// < DoesCollide:boolean=True if the raycast collided with the other mask
// < CollideX:integer=If collision, the X co-ordinate of it
// < CollideY:integer=If collision, the Y co-ordinate of it
// ? Performs a raycast collision
/* ------------------------------------------------------------------------- */
LLFUNCBEGIN(Raycast)
  const Mask &mSrc = *LCGETPTR(1, Mask), &mDst = *LCGETPTR(5, Mask);
  int iX = LCGETINT(int,  9, "DestXPos");
  int iY = LCGETINT(int, 10, "DestYPos");
  LCPUSHBOOL(mSrc.Raycast(
    LCGETINTLGE(size_t, 2, 0, mSrc.size(), "SrcID"),
    LCGETINT   (int,    3,                 "SrcXPos"),
    LCGETINT   (int,    4,                 "SrcYPos"), mDst,
    LCGETINTLGE(size_t, 6, 0, mDst.size(), "DestID"),
    LCGETINT   (int,    7,                 "DestXPos"),
    LCGETINT   (int,    8,                 "DestYPos"), *&iX, *&iY));
  LCPUSHINT(iX);
  LCPUSHINT(iY);
LLFUNCENDEX(3)
/* ========================================================================= */
// $ Mask:Save
// > Id:integer=The id number of the mask to dump.
// > Filename:string=The targa file to save to.
// ? Dumps the specified mask to disk for debugging purposes.
/* ------------------------------------------------------------------------- */
LLFUNCBEGIN(Save)
  const Mask &mSrc = *LCGETPTR(1, Mask);
  mSrc.Dump(LCGETINTLGE(size_t, 2, 0, mSrc.size(), "Index"),
            LCGETCPPFILE(       3,                 "File"));
LLFUNCEND
/* ========================================================================= */
// $ Mask:Merge
// > SrcHandle:Mask=Source mask to merge from.
// > Id:integer=The id number of the source mask.
// > DestMaskX:integer=Destination X position.
// > DestMaskY:integer=Destination Y position.
// ? Merges the source mask id into destination mask id zero at the specified
// ? position.
/* ------------------------------------------------------------------------- */
LLFUNCBEGIN(Merge)
  Mask &mDst = *LCGETPTR(1, Mask), &mSrc = *LCGETPTR(2, Mask);
  mDst.Merge(mSrc,
    LCGETINTLGE(size_t, 3, 0, mDst.size(),         "Index"),
    LCGETINTLG (int,    4, 0, mDst.DimGetWidth(),  "X"),
    LCGETINTLG (int,    5, 0, mDst.DimGetHeight(), "Y"));
LLFUNCEND
/* ========================================================================= */
// $ Mask:Copy
// > SrcHandle:Mask=Source mask to merge from.
// > Id:integer=The id number of the source mask.
// > DestMaskX:integer=Destination X position.
// > DestMaskY:integer=Destination Y position.
// ? Copies the source mask id into destination mask id zero at the specified
// ? position.
/* ------------------------------------------------------------------------- */
LLFUNCBEGIN(Copy)
  Mask &mDst = *LCGETPTR(1, Mask), &mSrc = *LCGETPTR(2, Mask);
  mDst.Copy(0, mSrc,
    LCGETINTLGE(size_t, 3, 0, mSrc.size(),         "Index"),
    LCGETINTLG (int,    4, 0, mDst.DimGetWidth(),  "X"),
    LCGETINTLG (int,    5, 0, mDst.DimGetHeight(), "Y"));
LLFUNCEND
/* ========================================================================= */
// $ Mask:Erase
// > DestHandle:Mask=Destination mask to erase.
// > Id:integer=The id number of the source mask.
// ? Fills the destination mask with zeros.
/* ------------------------------------------------------------------------- */
LLFUNCBEGIN(Erase)
  Mask &mDst = *LCGETPTR(1, Mask);
  mDst.Erase(LCGETINTLGE(size_t, 2, 0, mDst.size(), "Index"));
LLFUNCEND
/* ========================================================================= */
// $ Mask:Fill
// > DestX:integer=Destination X position to start filling at.
// > DestY:integer=Destination Y position to start filling at.
// > DestWidth:integer=Horizontal pixel count to fill.
// > DestHeight:integer=Vertical pixel count to fill.
// ? Sets all the specified bits in the mask.
/* ------------------------------------------------------------------------- */
LLFUNCBEGIN(Fill)
  Mask &mDst = *LCGETPTR(1, Mask);
  mDst.Fill(
    LCGETINTLG(int, 2, 0, mDst.DimGetWidth(),  "X"),
    LCGETINTLG(int, 3, 0, mDst.DimGetHeight(), "Y"),
    LCGETINTLG(int, 4, 0, mDst.DimGetWidth(),  "Width"),
    LCGETINTLG(int, 5, 0, mDst.DimGetHeight(), "Height"));
LLFUNCEND
/* ========================================================================= */
// $ Mask:Clear
// > DestX:integer=Destination X position to start filling at.
// > DestY:integer=Destination Y position to start filling at.
// > DestWidth:integer=Horizontal pixel count to clear.
// > DestHeight:integer=Vertical pixel count to clear.
// ? Clears all the specified bits in the mask.
/* ------------------------------------------------------------------------- */
LLFUNCBEGIN(Clear)
  Mask &mDst = *LCGETPTR(1, Mask);
  mDst.Fill(
    LCGETINTLG(int, 2, 0, mDst.DimGetWidth(),  "X"),
    LCGETINTLG(int, 3, 0, mDst.DimGetHeight(), "Y"),
    LCGETINTLG(int, 4, 0, mDst.DimGetWidth(),  "Width"),
    LCGETINTLG(int, 5, 0, mDst.DimGetHeight(), "Height"));
LLFUNCEND
/* ========================================================================= */
// $ Mask:Name
// < Name:string=Name of the mask.
// ? If this mask was loaded by a filename or it was set with a custom id.
// ? This function returns that name which was assigned to it.
/* ------------------------------------------------------------------------- */
LLFUNCEX(Name, 1, LCPUSHXSTR(LCGETPTR(1, Mask)->IdentGet()));
/* ========================================================================= */
// $ Mask:Count
// < Count:integer=Number of sub-masks in the mask.
// ? Returns the number of sub-masks in the mask.
/* ------------------------------------------------------------------------- */
LLFUNCEX(Count, 1, LCPUSHINT(LCGETPTR(1, Mask)->size()));
/* ========================================================================= */
// $ Mask:Width
// < Width:integer=Width of each tile in sub-masks.
// ? Returns the width of each sub-mask in the mask.
/* ------------------------------------------------------------------------- */
LLFUNCEX(Width, 1, LCPUSHINT(LCGETPTR(1, Mask)->DimGetWidth()));
/* ========================================================================= */
// $ Mask:Height
// < Height:integer=Height of each tile in sub-masks.
// ? Returns the height of each sub-mask in the mask.
/* ------------------------------------------------------------------------- */
LLFUNCEX(Height, 1, LCPUSHINT(LCGETPTR(1, Mask)->DimGetHeight()));
/* ========================================================================= */
// $ Mask:Destroy
// ? destroys the mask and frees all the memory associated with it. The object
// ? will no longer be useable after this call and an error will be generated
// ? if accessed.
/* ------------------------------------------------------------------------- */
LLFUNCBEGIN(Destroy) LCCLASSDESTROY(1, Mask); LLFUNCEND
/* ========================================================================= */
/* ######################################################################### */
/* ## Mask:* member functions structure                                   ## */
/* ######################################################################### */
/* ------------------------------------------------------------------------- */
LLRSMFBEGIN                            // Mask:* member functions begin
  LLRSFUNC(Clear),                     // Set area of mask with 0's
  LLRSFUNC(Copy),                      // Copy with specified mask bits
  LLRSFUNC(Count),                     // Number of sub-masks in mask
  LLRSFUNC(Destroy),                   // Destroy the internal mask object
  LLRSFUNC(Erase),                     // Erase with specified mask bits
  LLRSFUNC(Fill),                      // Set area of mask with 1's
  LLRSFUNC(Height),                    // Return tile height of mask
  LLRSFUNC(IsCollide),                 // Other mask collides?
  LLRSFUNC(IsCollideEx),               // Two masks collide?
  LLRSFUNC(Merge),                     // Merge with specified mask bits
  LLRSFUNC(Name),                      // Name of the mask
  LLRSFUNC(Raycast),                   // Collision testing by raycasting
  LLRSFUNC(Save),                      // Save mask to disk
  LLRSFUNC(Width),                     // Return tile width of mask
LLRSEND                                // Mask:* member functions end
/* ========================================================================= */
// $ Mask.CreateNew
// > Width:integer=The width of the new mask.
// > Height:integer=The height of the new mask.
// < Handle:Mask=The handle of the new mask created.
// ? Creates a new collision mask of the specified dimensions. Useful
// ? for storing and modifying the entire bitmask of a level. Use of this
// ? function does not clear the memory so the storage of the masks are
// ? unspecified. Use this only if you're garaunteed to initialise the storage
// ? yourself.
/* ------------------------------------------------------------------------- */
LLFUNCEX(CreateNew, 1, LCCLASSCREATE(Mask)->Init(
  LCGETINTLG(unsigned int, 1, 0, UINT_MAX, "Width"),
  LCGETINTLG(unsigned int, 2, 0, UINT_MAX, "Height")));
/* ========================================================================= */
// $ Mask.CreateBlank
// > Width:integer=The width of the new mask.
// > Height:integer=The height of the new mask.
// < Handle:Mask=The handle of the new mask created.
// ? Creates a new blank collision mask of the specified dimensions. Useful
// ? for storing and modifying the entire bitmask of a level.
/* ------------------------------------------------------------------------- */
LLFUNCEX(CreateBlank, 1, LCCLASSCREATE(Mask)->InitBlank(
  LCGETINTLG(unsigned int, 1, 0, UINT_MAX, "Width"),
  LCGETINTLG(unsigned int, 2, 0, UINT_MAX, "Height")));
/* ========================================================================= */
// $ Mask.Create
// > Image:image=The monochrome image or targa to open.
// > Count:integer=The number of tiles to grab from image.
// > Width:integer=The width of each tile.
// > Height:integer=The height of each tile.
// < Handle:Mask=The handle of the new mask created.
// ? Loads pre-defined masks from the specified raster image file.
/* ------------------------------------------------------------------------- */
LLFUNCEX(Create, 1, LCCLASSCREATE(Mask)->InitFromImage(
 *LCGETPTR (              1,              Image),
  LCGETINTLG(unsigned int, 2, 0, UINT_MAX, "Width"),
  LCGETINTLG(unsigned int, 3, 0, UINT_MAX, "Height")));
/* ========================================================================= */
/* ######################################################################### */
/* ## Mask.* namespace functions structure                                ## */
/* ######################################################################### */
/* ------------------------------------------------------------------------- */
LLRSBEGIN                              // Mask.* namespace functions begin
  LLRSFUNC(Create),                    // Create a mask
  LLRSFUNC(CreateNew),                 // Create uninitialised mask
  LLRSFUNC(CreateBlank),               // Create a blank mask
LLRSEND                                // Mask.* namespace functions end
/* ========================================================================= */
LLNAMESPACEEND                         // End of Mask namespace
/* == EoF =========================================================== EoF == */

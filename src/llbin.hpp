/* == LLBIN.HPP ============================================================ **
** ######################################################################### **
** ## MS-ENGINE              Copyright (c) MS-Design, All Rights Reserved ## **
** ######################################################################### **
** ## Defines the 'Bin' namespace and methods for the guest to use in     ## **
** ## Lua. This file is invoked by 'lualib.hpp'.                          ## **
** ######################################################################### **
** ------------------------------------------------------------------------- */
#pragma once                           // Only one incursion allowed
/* ========================================================================= **
** ######################################################################### **
** ========================================================================= */
// % Bin
/* ------------------------------------------------------------------------- */
// ! This gives the guest to have a virtual bin of the specified size and
// ! rectangles of a given size are placed inside until filled up.
/* ========================================================================= */
namespace LLBin {                      // Bin namespace
/* -- Dependencies --------------------------------------------------------- */
using namespace IBin::P;
/* ========================================================================= **
** ######################################################################### **
** ## Bin:* member functions                                              ## **
** ######################################################################### **
** ========================================================================= */
// $ Bin:Insert
// > Width:integer=The width of the item to insert in units
// > Height:integer=The height of the item to insert in units
// < X1:integer=The X1 position of the item inserted
// < Y1:integer=The Y1 position of the item inserted
// < X2:integer=The X2 position of the item inserted
// < Y2:integer=The Y2 position of the item inserted
// ? Inserts the specified rectangle in the bin. All values returned are zero
// ? on failure.
/* ------------------------------------------------------------------------- */
LLFUNCBEGIN(Insert)
  const Pack<int>::Rect &rData = LCGETPTR(1, Bin)->Insert(
    LCGETINTLGE(unsigned int, 2, 0, numeric_limits<int>::max(), "Width"),
    LCGETINTLGE(unsigned int, 3, 0, numeric_limits<int>::max(), "Height"));
  LCPUSHVAR(rData.CoordGetX(),   rData.CoordGetY(),
            rData.DimGetWidth(), rData.DimGetHeight());
LLFUNCENDEX(4)
/* ========================================================================= */
// $ Bin:Test
// > Width:integer=The width of the item to insert in units
// > Height:integer=The height of the item to insert in units
// < X1:integer=The X1 position of the item inserted
// < Y1:integer=The Y1 position of the item inserted
// < X2:integer=The X2 position of the item inserted
// < Y2:integer=The Y2 position of the item inserted
// ? Test inserts the specified rectangle in the bin. All values returned are
// ? zero on failure.
/* ------------------------------------------------------------------------- */
LLFUNCBEGIN(Test)
  const Pack<int>::Rect &rData = LCGETPTR(1, Bin)->Test(
    LCGETINTLGE(unsigned int, 2, 0, numeric_limits<int>::max(), "Width"),
    LCGETINTLGE(unsigned int, 3, 0, numeric_limits<int>::max(), "Height"));
  LCPUSHVAR(rData.CoordGetX(),   rData.CoordGetY(),
            rData.DimGetWidth(), rData.DimGetHeight());
LLFUNCENDEX(4)
/* ========================================================================= */
// $ Bin:Enlarge
// > Width:integer=The new width of the bin in units
// > Height:integer=The new height of the bin in units
// < Success:boolean=The enlargement was a success
// ? Makes the specified bin bigger for you to put more rects in. Note this is
// ? a dirty operation. None of the original bounds are modified, only new
// ? new empty rects are added, i.e. sub-bins to explain it simply.
/* ------------------------------------------------------------------------- */
LLFUNCEX(Enlarge, 1, LCPUSHVAR(LCGETPTR(1, Bin)->Resize(
  LCGETINTLGE(unsigned int, 2, 0, numeric_limits<int>::max(), "Width"),
  LCGETINTLGE(unsigned int, 3, 0, numeric_limits<int>::max(), "Height"))));
/* ========================================================================= */
// $ Bin:Width
// < Width:integer=The width of the bin in units
// ? Returns the width of the bin in units.
/* ------------------------------------------------------------------------- */
LLFUNCEX(Width, 1, LCPUSHVAR(LCGETPTR(1, Bin)->DimGetWidth()));
/* ========================================================================= */
// $ Bin:Height
// < Height:integer=The height of the bin in units
// ? Returns the height of the bin in units.
/* ------------------------------------------------------------------------- */
LLFUNCEX(Height, 1, LCPUSHVAR(LCGETPTR(1, Bin)->DimGetHeight()));
/* ========================================================================= */
// $ Bin:Occupancy
// < Height:integer=The height of the bin in units
// ? Returns the height of the bin in units.
/* ------------------------------------------------------------------------- */
LLFUNCEX(Occupancy, 1, LCPUSHVAR(LCGETPTR(1, Bin)->Occupancy()));
/* ========================================================================= */
// $ Bin:Destroy
// ? Destroys the bin and frees all the memory associated with it. The object
// ? will no longer be useable after this call and an error will be generated
// ? if accessed.
/* ------------------------------------------------------------------------- */
LLFUNC(Destroy, LCCLASSDESTROY(1, Bin));
/* ========================================================================= **
** ######################################################################### **
** ## Bin:* member functions structure                                    ## **
** ######################################################################### **
** ------------------------------------------------------------------------- */
LLRSMFBEGIN                            // Bin:* member functions begin
  LLRSFUNC(Destroy),   LLRSFUNC(Insert), LLRSFUNC(Enlarge), LLRSFUNC(Height),
  LLRSFUNC(Occupancy), LLRSFUNC(Test),   LLRSFUNC(Width),
LLRSEND                                // Bin:* member functions end
/* ========================================================================= */
// $ Bin.Create
// > Width:integer=The width of the bin in units
// > Height:integer=The height of the bin in units
// < Handle:Bin=Handle to the Bin class
// ? Creates an empty bin of the specified size.
/* ------------------------------------------------------------------------- */
LLFUNCEX(Create, 1, LCCLASSCREATE(Bin)->Init(
  LCGETINTLGE(unsigned int, 1, 0, numeric_limits<int>::max(), "Width"),
  LCGETINTLGE(unsigned int, 2, 0, numeric_limits<int>::max(), "Height")));
/* ========================================================================= **
** ######################################################################### **
** ## Bin:* namespace functions structure                                 ## **
** ######################################################################### **
** ------------------------------------------------------------------------- */
LLRSBEGIN                              // Bin.* namespace functions begin
  LLRSFUNC(Create),
LLRSEND                                // Bin.* namespace functions end
/* ========================================================================= */
}                                      // End of Bin namespace
/* == EoF =========================================================== EoF == */

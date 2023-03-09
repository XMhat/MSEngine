/* == LLPALETTE.HPP ======================================================== */
/* ######################################################################### */
/* ## MS-ENGINE              Copyright (c) MS-Design, All Rights Reserved ## */
/* ######################################################################### */
/* ## Defines the 'Palette' namespace and methods for the guest to use in ## */
/* ## Lua. This file is invoked by 'lualib.hpp'.                          ## */
/* ######################################################################### */
/* ------------------------------------------------------------------------- */
#pragma once                           // Only one incursion allowed
/* ========================================================================= */
/* ######################################################################### */
/* ========================================================================= */
// % Palette
/* ------------------------------------------------------------------------- */
// ! The palette class allows creating and modifying palettes and uploading
// ! them to the GPU for use with 8-bit paletted textures. Only one palette can
// ! be active at once and textures marked with LF_PALETTE are affected by the
// ! palette.
/* ========================================================================= */
namespace NsPalette {                  // Palette namespace
/* -- Includes ------------------------------------------------------------- */
using namespace IfPalette;             // Using palette namespace
using namespace IfTexture;             // Using texture namespace
/* ========================================================================= */
/* ######################################################################### */
/* ========================================================================= */
// $ Palette:Commit
// ? Use this function to commit the new palette to the shader.
/* ------------------------------------------------------------------------- */
LLFUNC(Commit, LCGETPTR(1, Palette)->Commit());
/* ========================================================================= */
// $ Palette:Fill
// > Id:integer=Palette entry to modify from (0-255).
// > Count:integer=The number of indexes to fill
// < Red:number=The red intensity number (0.0-1.0).
// < Green:number=The green intensity number (0.0-1.0).
// < Blue:number=The blue intensity number (0.0-1.0).
// < Alpha:number=The alpha intensity number (0.0-1.0).
// ? Use this function to fill the palette with the specified values.
/* ------------------------------------------------------------------------- */
LLFUNCBEGIN(Fill)
  Palette &pDstRef = *LCGETPTR(1, Palette);
  const size_t stIndex =
    LCGETINTLGE(size_t, 2, 0, cPalettes->palDefault.size(), "Index");
  pDstRef.Fill(stIndex,
    LCGETINTLG(size_t, 3, 0, cPalettes->palDefault.size()-stIndex, "Count"),
    LCGETNUMLG(GLfloat, 4, 0.0f, 1.0f, "Red"),
    LCGETNUMLG(GLfloat, 5, 0.0f, 1.0f, "Green"),
    LCGETNUMLG(GLfloat, 6, 0.0f, 1.0f, "Blue"),
    LCGETNUMLG(GLfloat, 7, 0.0f, 1.0f, "Alpha"));
LLFUNCEND
/* ========================================================================= */
// $ Palette:Copy
// > Count:integer=The number of values to copy.
// > DstId:integer=Index offset to copy the values to.
// > Src:Palette=The source palette to copy.
// > SrcId:integer=Index offset to copy the values from.
// ? Use this function to copy palette data from another palette.
/* ------------------------------------------------------------------------- */
LLFUNCBEGIN(Copy)
  Palette &pDstRef = *LCGETPTR(1, Palette);
  const size_t stCount =
    LCGETINTLG(size_t, 2, 0, cPalettes->palDefault.size(), "Count");
  const size_t stMaxEnd = cPalettes->palDefault.size()-stCount;
  const size_t stDstIndex = LCGETINTLG(size_t, 3, 0, stMaxEnd, "DestIndex");
  const Palette &pSrcRef = *LCGETPTR(4, Palette);
  const size_t stSrcIndex = LCGETINTLG(size_t, 5, 0, stMaxEnd, "SrcIndex");
  pDstRef.Copy(stDstIndex, pSrcRef, stSrcIndex, stCount);
LLFUNCEND
/* ========================================================================= */
// $ Palette:Destroy
// ? Destroys the palette and frees the memory associated with it. This does
// ? not affect the active palette as this is stored on the GPU.
/* ------------------------------------------------------------------------- */
LLFUNC(Destroy, LCCLASSDESTROY(1, Palette));
/* ========================================================================= */
// $ Palette:GetA
// > Id:integer=Palette entry to modify (0-255).
// < Red:number=The red intensity number (0.0-1.0).
// ? Use this function to get the alpha palette entry.
/* ------------------------------------------------------------------------- */
LLFUNCEX(GetA, 1, LCPUSHNUM(LCGETPTR(1, Palette)->
  GetAlpha(LCGETINTLGE(size_t, 2, 0, cPalettes->palDefault.size(), "Id"))));
/* ========================================================================= */
// $ Palette:GetB
// > Id:integer=Palette entry to modify (0-255).
// < Red:number=The red intensity number (0.0-1.0).
// ? Use this function to get the blue palette entry.
/* ------------------------------------------------------------------------- */
LLFUNCEX(GetB, 1, LCPUSHNUM(LCGETPTR(1, Palette)->
  GetBlue(LCGETINTLGE(size_t, 2, 0, cPalettes->palDefault.size(), "Id"))));
/* ========================================================================= */
// $ Palette:GetG
// > Id:integer=Palette entry to modify (0-255).
// < Red:number=The red intensity number (0.0-1.0).
// ? Use this function to get the green palette entry.
/* ------------------------------------------------------------------------- */
LLFUNCEX(GetG, 1, LCPUSHNUM(LCGETPTR(1, Palette)->
  GetGreen(LCGETINTLGE(size_t, 2, 0, cPalettes->palDefault.size(), "Id"))));
/* ========================================================================= */
// $ Palette:GetAI
// > Id:integer=Palette entry to modify (0-255).
// < Red:integer=The alpha intensity integer (0-255).
// ? Use this function to get the alpha palette entry. This function is mainly
// ? for convenience only and has slightly more overhead than with :GetA() as
// ? the palette values have to be stored as GLfloats in the GPU and thus extra
// ? calculations are required as a result.
/* ------------------------------------------------------------------------- */
LLFUNCEX(GetAI, 1, LCPUSHINT(LCGETPTR(1, Palette)->
  GetAlphaInt(LCGETINTLGE(size_t, 2, 0, cPalettes->palDefault.size(), "Id"))));
/* ========================================================================= */
// $ Palette:GetBI
// > Id:integer=Palette entry to modify (0-255).
// < Blue:integer=The blue intensity integer (0-255).
// ? Use this function to get the blue palette entry. This function is mainly
// ? for convenience only and has slightly more overhead than with :GetB() as
// ? the palette values have to be stored as GLfloats in the GPU and thus extra
// ? calculations are required as a result.
/* ------------------------------------------------------------------------- */
LLFUNCEX(GetBI, 1, LCPUSHINT(LCGETPTR(1, Palette)->
  GetBlueInt(LCGETINTLGE(size_t, 2, 0, cPalettes->palDefault.size(), "Id"))));
/* ========================================================================= */
// $ Palette:GetGI
// > Id:integer=Palette entry to modify (0-255).
// < Green:integer=The green intensity integer (0-255).
// ? Use this function to get the green palette entry. This function is mainly
// ? for convenience only and has slightly more overhead than with :GetG() as
// ? the palette values have to be stored as GLfloats in the GPU and thus extra
// ? calculations are required as a result.
/* ------------------------------------------------------------------------- */
LLFUNCEX(GetGI, 1, LCPUSHINT(LCGETPTR(1, Palette)->
  GetGreenInt(LCGETINTLGE(size_t, 2, 0, cPalettes->palDefault.size(), "Id"))));
/* ========================================================================= */
// $ Palette:GetRI
// > Id:integer=Palette entry to modify (0-255).
// < Red:integer=The red intensity integer (0-255).
// ? Use this function to get the red palette entry. This function is mainly
// ? for convenience only and has slightly more overhead than with :GetR() as
// ? the palette values have to be stored as GLfloats in the GPU and thus extra
// ? calculations are required as a result.
/* ------------------------------------------------------------------------- */
LLFUNCEX(GetRI, 1, LCPUSHINT(LCGETPTR(1, Palette)->
  GetRedInt(LCGETINTLGE(size_t, 2, 0, cPalettes->palDefault.size(), "Id"))));
/* ========================================================================= */
// $ Palette:GetRGBAI
// > Id:integer=Palette entry to modify (0-255).
// < Red:integer=The red intensity integer (0-255).
// < Green:integer=The green intensity integer (0-255).
// < Blue:integer=The blue intensity integer (0-255).
// < Alpha:integer=The alpha intensity integer (0-255).
// ? Use this function to get the palette entry.
/* ------------------------------------------------------------------------- */
LLFUNCBEGIN(GetRGBAI)
  const auto &fbocData = LCGETPTR(1, Palette)->GetSlotConst(
    LCGETINTLGE(size_t, 2, 0, cPalettes->palDefault.size(), "Id")).
    Cast<lua_Integer>();
  LCPUSHINT(fbocData[0]);  LCPUSHINT(fbocData[1]);
  LCPUSHINT(fbocData[2]); LCPUSHINT(fbocData[3]);
LLFUNCENDEX(4)
/* ========================================================================= */
// $ Palette:GetR
// > Id:integer=Palette entry to modify (0-255).
// < Red:number=The red intensity number (0.0-1.0).
// ? Use this function to get the red palette entry.
/* ------------------------------------------------------------------------- */
LLFUNCEX(GetR, 1, LCPUSHNUM(LCGETPTR(1, Palette)->
  GetRed(LCGETINTLGE(size_t, 2, 0, cPalettes->palDefault.size(), "Id"))));
/* ========================================================================= */
// $ Palette:GetRGBA
// > Id:integer=Palette entry to modify (0-255).
// < Red:number=The red intensity number (0.0-1.0).
// < Green:number=The green intensity number (0.0-1.0).
// < Blue:number=The blue intensity number (0.0-1.0).
// < Alpha:number=The alpha intensity number (0.0-1.0).
// ? Use this function to get the palette entry.
/* ------------------------------------------------------------------------- */
LLFUNCBEGIN(GetRGBA)
  const FboColour &fbocData = LCGETPTR(1, Palette)->GetSlotConst(
    LCGETINTLGE(size_t, 2, 0, cPalettes->palDefault.size(), "Id"));
  LCPUSHNUM(fbocData.GetColourRed());  LCPUSHNUM(fbocData.GetColourGreen());
  LCPUSHNUM(fbocData.GetColourBlue()); LCPUSHNUM(fbocData.GetColourAlpha());
LLFUNCENDEX(4)
/* ========================================================================= */
// $ Palette:GetName
// < Name:string=The identifier of the palette.
// ? Returns the palette identifier
/* ------------------------------------------------------------------------- */
LLFUNCEX(GetName, 1, LCPUSHXSTR(LCGETPTR(1, Palette)->IdentGet()));
/* ========================================================================= */
// $ Palette:SetA
// > Id:integer=Palette entry to modify (0-255).
// > Alpha:number=The alpha intensity number (0.0-1.0).
// ? Use this function to set the alpha palette entry.
/* ------------------------------------------------------------------------- */
LLFUNC(SetA, LCGETPTR(1, Palette)->SetAlpha(
  LCGETINTLGE(size_t,  2,    0, cPalettes->palDefault.size(), "Id"),
  LCGETNUMLG (GLfloat, 3, 0.0f, 1.0f, "Alpha")));
/* ========================================================================= */
// $ Palette:SetB
// > Id:integer=Palette entry to modify (0-255).
// > Blue:number=The blue intensity number (0.0-1.0).
// ? Use this function to set the blue palette entry.
/* ------------------------------------------------------------------------- */
LLFUNC(SetB, LCGETPTR(1, Palette)->SetBlue(
  LCGETINTLGE(size_t,  2,    0, cPalettes->palDefault.size(), "Id"),
  LCGETNUMLG (GLfloat, 3, 0.0f, 1.0f, "Blue")));
/* ========================================================================= */
// $ Palette:SetG
// > Id:integer=Palette entry to modify (0-255).
// > Green:number=The green intensity number (0.0-1.0).
// ? Use this function to set the green palette entry.
/* ------------------------------------------------------------------------- */
LLFUNC(SetG, LCGETPTR(1, Palette)->SetGreen(
  LCGETINTLGE(size_t,  2,    0, cPalettes->palDefault.size(), "Id"),
  LCGETNUMLG (GLfloat, 3, 0.0f, 1.0f, "Green")));
/* ========================================================================= */
// $ Palette:SetAI
// > Id:integer=Palette entry to modify (0-255).
// > Alpha:integer=The alpha intensity integer (0-255).
// ? Use this function to set the alpha palette entry.
/* ------------------------------------------------------------------------- */
LLFUNC(SetAI, LCGETPTR(1, Palette)->SetAlphaInt(
  LCGETINTLGE(size_t,       2, 0, cPalettes->palDefault.size(), "Id"),
  LCGETINTLG (unsigned int, 3, 0, 255, "Alpha")));
/* ========================================================================= */
// $ Palette:SetBI
// > Id:integer=Palette entry to modify (0-255).
// > Blue:integer=The blue intensity integer (0-255).
// ? Use this function to set the blue palette entry.
/* ------------------------------------------------------------------------- */
LLFUNC(SetBI, LCGETPTR(1, Palette)->SetBlueInt(
  LCGETINTLGE(size_t,       2, 0, cPalettes->palDefault.size(), "Id"),
  LCGETINTLG (unsigned int, 3, 0, 255, "Blue")));
/* ========================================================================= */
// $ Palette:SetGI
// > Id:integer=Palette entry to modify (0-255).
// > Green:integer=The green intensity integer (0-255).
// ? Use this function to set the green palette entry.
/* ------------------------------------------------------------------------- */
LLFUNC(SetGI, LCGETPTR(1, Palette)->SetGreenInt(
  LCGETINTLGE(size_t,       2, 0, cPalettes->palDefault.size(), "Id"),
  LCGETINTLG (unsigned int, 3, 0, 255, "Green")));
/* ========================================================================= */
// $ Palette:SetRI
// > Id:integer=Palette entry to modify (0-255).
// > Red:integer=The red intensity integer (0-255).
// ? Use this function to set the red palette entry.
/* ------------------------------------------------------------------------- */
LLFUNC(SetRI, LCGETPTR(1, Palette)->SetRedInt(
  LCGETINTLGE(size_t,       2, 0, cPalettes->palDefault.size(), "Id"),
  LCGETINTLG (unsigned int, 3, 0, 255, "Red")));
/* ========================================================================= */
// $ Palette:SetRGBAI
// > Id:integer=Palette entry to modify (0-255).
// > Red:integer=The red intensity integer (0-255).
// > Green:integer=The green intensity integer (0-255).
// > Blue:integer=The blue intensity integer (0-255).
// > Alpha:integer=The alpha intensity integer (0-255).
// ? Use this function to set the palette entry using integers. This function
// ? is mainly for convenience only and has slightly more overhead than with
// ? :Set() as the palette values have to be stored as GLfloats in the GPU and
// ? thus extra calculations are required as a result.
/* ------------------------------------------------------------------------- */
LLFUNC(SetRGBAI, LCGETPTR(1, Palette)->SetRGBAInt(
  LCGETINTLGE(size_t,       2, 0, cPalettes->palDefault.size(), "Id"),
  LCGETINTLG (unsigned int, 3, 0, 255, "Red"),
  LCGETINTLG (unsigned int, 4, 0, 255, "Green"),
  LCGETINTLG (unsigned int, 5, 0, 255, "Blue"),
  LCGETINTLG (unsigned int, 6, 0, 255, "Alpha")));
/* ========================================================================= */
// $ Palette:SetR
// > Id:integer=Palette entry to modify (0-255).
// > Red:number=The red intensity number (0.0-1.0).
// ? Use this function to set the red palette entry.
/* ------------------------------------------------------------------------- */
LLFUNC(SetR, LCGETPTR(1, Palette)->SetRed(
  LCGETINTLGE(size_t,  2,    0, cPalettes->palDefault.size(), "Id"),
  LCGETNUMLG (GLfloat, 3, 0.0f, 1.0f, "Red")));
/* ========================================================================= */
// $ Palette:SetRGBA
// > Id:integer=Palette entry to modify (0-255).
// > Red:number=The red intensity number (0.0-1.0).
// > Green:number=The green intensity number (0.0-1.0).
// > Blue:number=The blue intensity number (0.0-1.0).
// > Alpha:number=The alpha intensity number (0.0-1.0).
// ? Use this function to set the palette entry.
/* ------------------------------------------------------------------------- */
LLFUNC(SetRGBA, LCGETPTR(1, Palette)->SetRGBA(
  LCGETINTLGE(size_t,  2,    0, cPalettes->palDefault.size(), "Id"),
  LCGETNUMLG (GLfloat, 3, 0.0f, 1.0f, "Red"),
  LCGETNUMLG (GLfloat, 4, 0.0f, 1.0f, "Green"),
  LCGETNUMLG (GLfloat, 5, 0.0f, 1.0f, "Blue"),
  LCGETNUMLG (GLfloat, 6, 0.0f, 1.0f, "Alpha")));
/* ========================================================================= */
// $ Palette:Shift
// > Begin:integer=The starting palette index to shift up to.
// > End:integer=The ending palette index to shift up to.
// > Amount:integer=Amount to rotate by.
// ? Shifts all palette entries backwards or forwards this amount from the
// ? specified palette index and limited to the specified number of indexes.
/* ------------------------------------------------------------------------- */
LLFUNCBEGIN(Shift)
  Palette &pRef = *LCGETPTR(1, Palette);
  const ssize_t stBegin = LCGETINTLG(ssize_t, 2, 0, pRef.Size(), "Begin");
  pRef.Shift(stBegin, LCGETINTLGE(ssize_t, 3, 0, pRef.Size() - stBegin, "End"),
    LCGETINTLEG(ssize_t, 4, pRef.SizeN(), pRef.Size(), "Amount"));
LLFUNCEND
/* ========================================================================= */
// $ Palette:ShiftB
// > Begin:integer=The starting palette index to shift up to.
// > End:integer=The ending palette index to shift up to.
// > Amount:integer=Amount to rotate backwards by.
// ? Shifts all palette entries backwards this amount from the specified
// ? palette index and limited to the specified number of indexes.
/* ------------------------------------------------------------------------- */
LLFUNCBEGIN(ShiftB)
  Palette &pRef = *LCGETPTR(1, Palette);
  const ssize_t stBegin = LCGETINTLGE(ssize_t, 2, 0, pRef.Size(), "Begin");
  pRef.ShiftBck(stBegin, LCGETINTLGE(ssize_t, 3, stBegin, pRef.Size(), "End"),
    LCGETINTLG(ssize_t, 4, 0, pRef.Size() - stBegin, "Amount"));
LLFUNCEND
/* ========================================================================= */
// $ Palette:ShiftF
// > Begin:integer=The starting palette index to shift up to.
// > End:integer=The ending palette index to shift up to.
// > Amount:integer=Amount to rotate forwards by.
// ? Shifts all palette entries forwards this amount from the specified
// ? palette index and limited to the specified number of indexes.
/* ------------------------------------------------------------------------- */
LLFUNCBEGIN(ShiftF)
  Palette &pRef = *LCGETPTR(1, Palette);
  const ssize_t stBegin = LCGETINTLGE(ssize_t, 2, 0, pRef.Size(), "Start");
  pRef.ShiftFwd(stBegin, LCGETINTLGE(ssize_t, 3,stBegin, pRef.Size(), "Limit"),
    LCGETINTLG(ssize_t, 4, 0, pRef.Size() - stBegin, "Amount"));
LLFUNCEND
/* ========================================================================= */
/* ######################################################################### */
/* ## Palette:* member functions structure                                ## */
/* ######################################################################### */
/* ------------------------------------------------------------------------- */
LLRSMFBEGIN                            // Palette:* member functions begin
  LLRSFUNC(Commit),  LLRSFUNC(Copy),     LLRSFUNC(Destroy), LLRSFUNC(Fill),
  LLRSFUNC(GetA),    LLRSFUNC(GetAI),    LLRSFUNC(GetB),    LLRSFUNC(GetBI),
  LLRSFUNC(GetG),    LLRSFUNC(GetGI),    LLRSFUNC(GetName), LLRSFUNC(GetR),
  LLRSFUNC(GetRGBA), LLRSFUNC(GetRGBAI), LLRSFUNC(GetRI),   LLRSFUNC(SetA),
  LLRSFUNC(SetAI),   LLRSFUNC(SetB),     LLRSFUNC(SetBI),   LLRSFUNC(SetG),
  LLRSFUNC(SetGI),   LLRSFUNC(SetR),     LLRSFUNC(SetRGBA), LLRSFUNC(SetRGBAI),
  LLRSFUNC(SetRI),   LLRSFUNC(Shift),    LLRSFUNC(ShiftB),  LLRSFUNC(ShiftF),
LLRSEND                                // Palette:* member functions end
/* ========================================================================= */
// $ Palette.Create
// > Identifier:string=Reference only user-defined identifier.
// < Handle:Palette=A handle to the newly created palette object.
// ? Creates a new empty palette with the specified name
/* ------------------------------------------------------------------------- */
LLFUNCEX(Create, 1,
  LCCLASSCREATE(Palette)->Init(LCGETCPPSTRINGNE(1, "Identifier")));
/* ========================================================================= */
// $ Palette.Image
// > Identifier:string=Reference only user-defined identifier.
// > Handle:Image=Handle to image to grab palette from.
// ? Creates a new palette from the specified image.
/* ------------------------------------------------------------------------- */
LLFUNCEX(Image, 1, LCCLASSCREATE(Palette)->Init(
  LCGETCPPSTRINGNE(1, "Identifier"), *LCGETPTR(2, Image)))
/* ========================================================================= */
// $ Palette.Texture
// > Identifier:string=Reference only user-defined identifier.
// > Handle:Texture=Handle to texture to grab palette from.
// ? Creates a new palette from the specified texture.
/* ------------------------------------------------------------------------- */
LLFUNCEX(Texture, 1, LCCLASSCREATE(Palette)->Init(
  LCGETCPPSTRINGNE(1, "Identifier"), *LCGETPTR(2, Texture)))
/* ========================================================================= */
// $ Palette.Default
// > Identifier:string=Reference only user-defined identifier.
// < Handle:Palette=A handle to the newly created palette object.
// ? Creates a new palette with the default VGA palette.
/* ------------------------------------------------------------------------- */
LLFUNCEX(Default, 1, LCCLASSCREATE(Palette)->Init(
  LCGETCPPSTRINGNE(1, "Identifier"), cPalettes->palDefault));
/* ========================================================================= */
// $ Palette.Palette
// > Identifier:string=Reference only user-defined identifier.
// < Handle:Palette=A handle to the newly created palette object.
// ? Creates a new palette from another essentially copying it.
/* ------------------------------------------------------------------------- */
LLFUNCEX(Palette, 1, LCCLASSCREATE(Palette)->Init(
  LCGETCPPSTRINGNE(1, "Identifier"), *LCGETPTR(2, Palette)));
/* ========================================================================= */
/* ######################################################################### */
/* ## Palette.* namespace functions structure                             ## */
/* ######################################################################### */
/* ------------------------------------------------------------------------- */
LLRSBEGIN                              // Palette.* namespace functions begin
  LLRSFUNC(Create),  LLRSFUNC(Default), LLRSFUNC(Image), LLRSFUNC(Palette),
  LLRSFUNC(Texture),
LLRSEND                                // Palette.* namespace functions end
/* ========================================================================= */
}                                      // End of Palette namespace
/* == EoF =========================================================== EoF == */

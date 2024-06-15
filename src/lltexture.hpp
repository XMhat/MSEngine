/* == LLTEXTURE.HPP ======================================================== **
** ######################################################################### **
** ## MS-ENGINE              Copyright (c) MS-Design, All Rights Reserved ## **
** ######################################################################### **
** ## Defines the 'Texture' namespace and methods for the guest to use in ## **
** ## Lua. This file is invoked by 'lualib.hpp'.                          ## **
** ######################################################################### **
** ------------------------------------------------------------------------- */
#pragma once                           // Only one incursion allowed
/* ========================================================================= **
** ######################################################################### **
** ========================================================================= */
// % Texture
/* ------------------------------------------------------------------------- */
// ! The texture class allows the programmer to draw textures on the screen
// ! which also includes font management.
/* ========================================================================= */
namespace LLTexture {                  // Texture namespace
/* -- Dependencies --------------------------------------------------------- */
using namespace IConsole::P;           using namespace IFboDef::P;
using namespace IImage::P;             using namespace IOgl::P;
using namespace ITexture::P;           using namespace Lib::OS::GlFW;
/* ========================================================================= **
** ######################################################################### **
** ## Texture:* member functions                                          ## **
** ######################################################################### **
** ========================================================================= */
// $ Texture:BlitLTRB
// > Left:number=The starting column to draw from.
// > Top:number=The starting row to draw from.
// > Right:number=The ending column pixel to draw to.
// > Bottom:number=The ending row pixel to draw to.
// ? Blits tile 0 of texture 0 at the specified bounds.
/* ------------------------------------------------------------------------- */
LLFUNC(BlitLTRB, LCGETPTR(1, Texture)->BlitLTRB(0, 0,
  LCGETNUM(GLfloat, 2, "Left"), LCGETNUM(GLfloat, 3, "Top"),
  LCGETNUM(GLfloat, 4, "Right"), LCGETNUM(GLfloat, 5, "Bottom")));
/* ========================================================================= */
// $ Texture:BlitLTWH
// > Left:number=The starting column to draw from.
// > Top:number=The starting row to draw from.
// > Width:number=The width of the quad to draw.
// > Height:number=The height of the quad to draw
// ? Blits tile 0 of texture 0 at the specified screen co-ordinates and with
// ? the specified dimensions.
/* ------------------------------------------------------------------------- */
LLFUNC(BlitLTWH, LCGETPTR(1, Texture)->BlitLTWH(0, 0,
  LCGETNUM(GLfloat, 2, "Left"), LCGETNUM(GLfloat, 3, "Top"),
  LCGETNUM(GLfloat, 4, "Width"), LCGETNUM(GLfloat, 5, "Height")));
/* ========================================================================= */
// $ Texture:BlitLTRBA
// > Left:number=The starting column to draw from.
// > Top:number=The starting row to draw from.
// > Right:number=The ending column pixel to draw to.
// > Bottom:number=The ending row pixel to draw to.
// > Angle:number=The angle in which to rotate from the centre point (-1 to 1).
// ? Blits tile 0 of texture 0 at the specified screen bounds and angle from
// ? the centre of the quad.
/* ------------------------------------------------------------------------- */
LLFUNC(BlitLTRBA, LCGETPTR(1, Texture)->BlitLTRBA(0, 0,
  LCGETNUM(GLfloat, 2, "Left"), LCGETNUM(GLfloat, 3, "Top"),
  LCGETNUM(GLfloat, 4, "Right"), LCGETNUM(GLfloat, 5, "Bottom"),
  LCGETNORM(GLfloat, 6, "Angle")));
/* ========================================================================= */
// $ Texture:BlitLTWHA
// > Left:number=The starting column to draw from.
// > Top:number=The starting row to draw from.
// > Width:number=The width of the quad to draw.
// > Height:number=The height of the quad to draw
// > Angle:number=The angle in which to rotate from the centre point (-1 to 1).
// ? Blits tile 0 of texture 0 at the specified screen co-ordinates and with
// ? the specified dimensions at the specified angle from the centre of the
// ? quad.
/* ------------------------------------------------------------------------- */
LLFUNC(BlitLTWHA, LCGETPTR(1, Texture)->BlitLTWHA(0, 0,
  LCGETNUM(GLfloat, 2, "Left"), LCGETNUM(GLfloat, 3, "Top"),
  LCGETNUM(GLfloat, 4, "Width"), LCGETNUM(GLfloat, 5, "Height"),
  LCGETNORM(GLfloat, 6, "Angle")));
/* ========================================================================= */
// $ Texture:SetVLTRB
// > Left:number=The starting column to draw from.
// > Top:number=The starting row to draw from.
// > Right:number=The ending column pixel to draw to.
// > Bottom:number=The ending row pixel to draw to.
// ? Presets these screen bounds for the next use of Texture:BlitP*.
/* ------------------------------------------------------------------------- */
LLFUNC(SetVLTRB, LCGETPTR(1, Texture)->FboItemSetVertex(
  LCGETNUM(GLfloat, 2, "Left"),  LCGETNUM(GLfloat, 3, "Top"),
  LCGETNUM(GLfloat, 4, "Right"), LCGETNUM(GLfloat, 5, "Bottom")));
/* ========================================================================= */
// $ Texture:SetVLTWH
// > Left:number=The starting column to draw from.
// > Top:number=The starting row to draw from.
// > Width:number=The width of the quad to draw.
// > Height:number=The height of the quad to draw
// ? Presets the specified screen co-ordinates and dimensions for the next use
// ? of Texture:BlitP*.
/* ------------------------------------------------------------------------- */
LLFUNC(SetVLTWH, LCGETPTR(1, Texture)->FboItemSetVertexWH(
  LCGETNUM(GLfloat, 2, "Left"),  LCGETNUM(GLfloat, 3, "Top"),
  LCGETNUM(GLfloat, 4, "Width"), LCGETNUM(GLfloat, 5, "Height")));
/* ========================================================================= */
// $ Texture:SetVLTRBA
// > Left:number=The starting column to draw from.
// > Top:number=The starting row to draw from.
// > Right:number=The ending column pixel to draw to.
// > Bottom:number=The ending row pixel to draw to.
// > Angle:number=The angle in which to rotate from the centre point (-1 to 1).
// ? Presets the specified screen bounds and angle for the next use of
// ? Texture:BlitP*.
/* ------------------------------------------------------------------------- */
LLFUNC(SetVLTRBA, LCGETPTR(1, Texture)->FboItemSetVertex(
  LCGETNUM(GLfloat, 2, "Left"), LCGETNUM(GLfloat, 3, "Top"),
  LCGETNUM(GLfloat, 4, "Right"), LCGETNUM(GLfloat, 5, "Bottom"),
  LCGETNORM(GLfloat, 6, "Angle")));
/* ========================================================================= */
// $ Texture:SetVLTWHA
// > Left:number=The starting column to draw from.
// > Top:number=The starting row to draw from.
// > Width:number=The width of the quad to draw.
// > Height:number=The height of the quad to draw
// > Angle:number=The angle in which to rotate from the centre point (-1 to 1).
// ? Presets the specified screen co-ordinates and dimensions with angle for
// ? the next use of Texture:BlitP*.
/* ------------------------------------------------------------------------- */
LLFUNC(SetVLTWHA, LCGETPTR(1, Texture)->FboItemSetVertexWH(
  LCGETNUM(GLfloat, 2, "Left"), LCGETNUM(GLfloat, 3, "Top"),
  LCGETNUM(GLfloat, 4, "Width"), LCGETNUM(GLfloat, 5, "Height"),
  LCGETNORM(GLfloat, 6, "Angle")));
/* ========================================================================= */
// $ Texture:BlitSLTRB
// > TileIndex:integer=The tile index of the texture to blit.
// > Left:number=The starting column to draw from.
// > Top:number=The starting row to draw from.
// > Right:number=The ending column pixel to draw to.
// > Bottom:number=The ending row pixel to draw to.
// ? Blits tile 'TileIndex' of texture 0 at the specified bounds.
/* ------------------------------------------------------------------------- */
LLFUNCBEGIN(BlitSLTRB)
  Texture &tCref = *LCGETPTR(1, Texture);
  tCref.BlitLTRB(0,
    LCGETINTLGE(size_t,  2, 0, tCref.GetTileCount(), "TileIndex"),
    LCGETNUM(GLfloat, 3, "Left"), LCGETNUM(GLfloat, 4, "Top"),
    LCGETNUM(GLfloat, 5, "Right"), LCGETNUM(GLfloat, 6, "Bottom"));
LLFUNCEND
/* ========================================================================= */
// $ Texture:BlitSLTWH
// > TileIndex:integer=The tile index of the texture to blit.
// > Left:number=The starting column to draw from.
// > Top:number=The starting row to draw from.
// > Width:number=The width of the quad to draw.
// > Height:number=The height of the quad to draw
// ? Blits tile 'TileIndex' of texture 0 at the specified screen co-ordinates
// ? and with the specified dimensions.
/* ------------------------------------------------------------------------- */
LLFUNCBEGIN(BlitSLTWH)
  Texture &tCref = *LCGETPTR(1, Texture);
  tCref.BlitLTWH(0,
    LCGETINTLGE(size_t, 2, 0, tCref.GetTileCount(), "TileIndex"),
    LCGETNUM(GLfloat, 3, "Left"), LCGETNUM(GLfloat, 4, "Top"),
    LCGETNUM(GLfloat, 5, "Width"), LCGETNUM(GLfloat, 6, "Height"));
LLFUNCEND
/* ========================================================================= */
// $ Texture:Blit
// ? Blits texture 0 with the stored preset texcoord (SetTC*), vertex (SetV*)
// ? and colour (SetC*). Using the Blit* functions will overwrite this stored
// ? preset data.
/* ------------------------------------------------------------------------- */
LLFUNC(Blit, LCGETPTR(1, Texture)->Blit(0));
/* ========================================================================= */
// $ Texture:BlitSLTRBA
// > TileIndex:integer=The tile index of the texture to blit.
// > Left:number=The starting column to draw from.
// > Top:number=The starting row to draw from.
// > Right:number=The ending column pixel to draw to.
// > Bottom:number=The ending row pixel to draw to.
// > Angle:number=The angle in which to rotate from the centre point (-1 to 1).
// ? Blits tile 'TileIndex' of texture 0 at the specified screenbounds and
// ? angle.
/* ------------------------------------------------------------------------- */
LLFUNCBEGIN(BlitSLTRBA)
  Texture &tCref = *LCGETPTR(1, Texture);
  tCref.BlitLTRBA(0,
    LCGETINTLGE(size_t, 2, 0, tCref.GetTileCount(), "TileIndex"),
    LCGETNUM(GLfloat, 3, "Left"), LCGETNUM(GLfloat, 4, "Top"),
    LCGETNUM(GLfloat, 5, "Right"), LCGETNUM(GLfloat, 6, "Bottom"),
    LCGETNORM(GLfloat, 7, "Angle"));
LLFUNCEND
/* ========================================================================= */
// $ Texture:BlitSLTWHA
// > TileIndex:integer=The tile index of the texture to blit.
// > Left:number=The starting column to draw from.
// > Top:number=The starting row to draw from.
// > Width:number=The width of the quad to draw.
// > Height:number=The height of the quad to draw
// > Angle:number=The angle in which to rotate from the centre point (-1 to 1).
// ? Blits tile 'TileIndex' of texture 0 at the specified screen co-ordinates
// ? and with the specified dimensions and angle.
/* ------------------------------------------------------------------------- */
LLFUNCBEGIN(BlitSLTWHA)
  Texture &tCref = *LCGETPTR(1, Texture);
  tCref.BlitLTWHA(0,
    LCGETINTLGE(size_t, 2, 0, tCref.GetTileCount(), "TileIndex"),
    LCGETNUM(GLfloat, 3, "Left"), LCGETNUM(GLfloat, 4, "Top"),
    LCGETNUM(GLfloat, 5, "Width"), LCGETNUM(GLfloat, 6, "Height"),
    LCGETNORM(GLfloat, 7, "Angle"));
LLFUNCEND
/* ========================================================================= */
// $ Texture:BlitILTRB
// > TexIndex:integer=The texture index to use.
// > Left:number=The starting column to draw from.
// > Top:number=The starting row to draw from.
// > Right:number=The ending column pixel to draw to.
// > Bottom:number=The ending row pixel to draw to.
// ? Blits tile 0 of texture 'TexIndex' at the specified bounds.
/* ------------------------------------------------------------------------- */
LLFUNCBEGIN(BlitILTRB)
  Texture &tCref = *LCGETPTR(1, Texture);
  tCref.BlitLTRB(
    LCGETINTLGE(size_t, 2, 0, tCref.GetSubCount(), "TexIndex"), 0,
    LCGETNUM(GLfloat, 3, "Left"), LCGETNUM(GLfloat, 4, "Top"),
    LCGETNUM(GLfloat, 5, "Right"), LCGETNUM(GLfloat, 6, "Bottom"));
LLFUNCEND
/* ========================================================================= */
// $ Texture:BlitILTWH
// > TexIndex:integer=The texture index to use.
// > Left:number=The starting column to draw from.
// > Top:number=The starting row to draw from.
// > Width:number=The width of the quad to draw.
// > Height:number=The height of the quad to draw
// ? Blits tile 0 of texture 'TexIndex' at the specified screen co-ordinates
// ? and with the specified dimensions.
/* ------------------------------------------------------------------------- */
LLFUNCBEGIN(BlitILTWH)
  Texture &tCref = *LCGETPTR(1, Texture);
  tCref.BlitLTWH(
    LCGETINTLGE(size_t,  2, 0, tCref.GetSubCount(), "TexIndex"), 0,
    LCGETNUM(GLfloat, 3, "Left"), LCGETNUM(GLfloat, 4, "Top"),
    LCGETNUM(GLfloat, 5, "Width"), LCGETNUM(GLfloat, 6, "Height"));
LLFUNCEND
/* ========================================================================= */
// $ Texture:BlitILTRBA
// > TexIndex:integer=The texture index to use.
// > Left:number=The starting column to draw from.
// > Top:number=The starting row to draw from.
// > Right:number=The ending column pixel to draw to.
// > Bottom:number=The ending row pixel to draw to.
// > Angle:number=The angle in which to rotate from the centre point (-1 to 1).
// ? Blits tile 0 of texture 'TexIndex' at the specified bounds.
/* ------------------------------------------------------------------------- */
LLFUNCBEGIN(BlitILTRBA)
  Texture &tCref = *LCGETPTR(1, Texture);
  tCref.BlitLTRBA(
    LCGETINTLGE(size_t, 2, 0, tCref.GetSubCount(), "TexIndex"), 0,
    LCGETNUM(GLfloat, 3, "Left"), LCGETNUM(GLfloat, 4, "Top"),
    LCGETNUM(GLfloat, 5, "Right"), LCGETNUM(GLfloat, 6, "Bottom"),
    LCGETNORM(GLfloat, 7, "Angle"));
LLFUNCEND
/* ========================================================================= */
// $ Texture:BlitILTWHA
// > TexIndex:integer=The texture index to use.
// > Left:number=The starting column to draw from.
// > Top:number=The starting row to draw from.
// > Width:number=The width of the quad to draw.
// > Height:number=The height of the quad to draw
// > Angle:number=The angle in which to rotate from the centre point (-1 to 1).
// ? Blits tile 0 of texture 'TexIndex' at the specified screen co-ordinates
// ? and with the specified dimensions at the specified angle.
/* ------------------------------------------------------------------------- */
LLFUNCBEGIN(BlitILTWHA)
  Texture &tCref = *LCGETPTR(1, Texture);
  tCref.BlitLTWHA(
    LCGETINTLGE(size_t, 2, 0, tCref.GetSubCount(), "TexIndex"), 0,
    LCGETNUM(GLfloat, 3, "Left"), LCGETNUM(GLfloat, 4, "Top"),
    LCGETNUM(GLfloat, 5, "Width"), LCGETNUM(GLfloat, 6, "Height"),
    LCGETNORM(GLfloat, 7, "Angle"));
LLFUNCEND
/* ========================================================================= */
// $ Texture:BlitISLTRB
// > TexIndex:integer=The texture index to use.
// > TileIndex:integer=The tile index to blit.
// > Left:number=The starting column to draw from.
// > Top:number=The starting row to draw from.
// > Right:number=The ending column pixel to draw to.
// > Bottom:number=The ending row pixel to draw to.
// ? Blits tile 'TileIndex' of texture 'TexIndex' at the specified screen
// ? bounds.
/* ------------------------------------------------------------------------- */
LLFUNCBEGIN(BlitISLTRB)
  Texture &tCref = *LCGETPTR(1, Texture);
  tCref.BlitLTRB(
    LCGETINTLGE(size_t,  2, 0, tCref.GetSubCount(),  "TexIndex"),
    LCGETINTLGE(size_t,  3, 0, tCref.GetTileCount(), "TileIndex"),
    LCGETNUM(GLfloat, 4, "Left"), LCGETNUM(GLfloat, 5, "Top"),
    LCGETNUM(GLfloat, 6, "Right"), LCGETNUM(GLfloat, 7, "Bottom"));
LLFUNCEND
/* ========================================================================= */
// $ Texture:BlitISLTWH
// > TexIndex:integer=The texture index to use.
// > TileIndex:integer=The tile index to blit.
// > Left:number=The starting column to draw from.
// > Top:number=The starting row to draw from.
// > Width:number=The width of the quad to draw.
// > Height:number=The height of the quad to draw
// ? Blits tile 'TileIndex' of texture 'TexIndex' at the specified screen
// ? co-ordinates and with the specified dimensions.
/* ------------------------------------------------------------------------- */
LLFUNCBEGIN(BlitISLTWH)
  Texture &tCref = *LCGETPTR(1, Texture);
  tCref.BlitLTWH(
    LCGETINTLGE(size_t,  2, 0, tCref.GetSubCount(),  "TexIndex"),
    LCGETINTLGE(size_t,  3, 0, tCref.GetTileCount(), "TileIndex"),
    LCGETNUM(GLfloat, 4, "Left"), LCGETNUM(GLfloat, 5, "Top"),
    LCGETNUM(GLfloat, 6, "Width"), LCGETNUM(GLfloat, 7, "Height"));
LLFUNCEND
/* ========================================================================= */
// $ Texture:BlitISLTRBA
// > TexIndex:integer=The texture index to use.
// > TileIndex:integer=The tile index to blit.
// > Left:number=The starting column to draw from.
// > Top:number=The starting row to draw from.
// > Right:number=The ending column pixel to draw to.
// > Bottom:number=The ending row pixel to draw to.
// > Angle:number=The angle in which to rotate from the centre point (-1 to 1).
// ? Blits tile 'TileIndex' of texture 'TexIndex' at the specified screen
// ? bounds.
/* ------------------------------------------------------------------------- */
LLFUNCBEGIN(BlitISLTRBA)
  Texture &tCref = *LCGETPTR(1, Texture);
  tCref.BlitLTRBA(
    LCGETINTLGE(size_t, 2, 0, tCref.GetSubCount(),  "Texindex"),
    LCGETINTLGE(size_t, 3, 0, tCref.GetTileCount(), "Tileindex"),
    LCGETNUM(GLfloat, 4, "Left"), LCGETNUM(GLfloat, 5, "Top"),
    LCGETNUM(GLfloat, 6, "Right"), LCGETNUM(GLfloat, 7, "Bottom"),
    LCGETNORM(GLfloat, 8, "Angle"));
LLFUNCEND
/* ========================================================================= */
// $ Texture:BlitISLTWHA
// > TexIndex:integer=The texture index to use.
// > TileIndex:integer=The tile index to blit.
// > Left:number=The starting column to draw from.
// > Top:number=The starting row to draw from.
// > Width:number=The width of the quad to draw.
// > Height:number=The height of the quad to draw
// > Angle:number=The angle in which to rotate from the centre point (-1 to 1).
// ? Blits tile 'TileIndex' of texture 'TexIndex' at the specified screen
// ? co-ordinates and with the specified dimensions.
/* ------------------------------------------------------------------------- */
LLFUNCBEGIN(BlitISLTWHA)
  Texture &tCref = *LCGETPTR(1, Texture);
  tCref.BlitLTWHA(
    LCGETINTLGE(size_t, 2, 0, tCref.GetSubCount(),  "Texindex"),
    LCGETINTLGE(size_t, 3, 0, tCref.GetTileCount(), "Tileindex"),
    LCGETNUM(GLfloat, 4, "Left"), LCGETNUM(GLfloat, 5, "Top"),
    LCGETNUM(GLfloat, 6, "Width"), LCGETNUM(GLfloat, 7, "Height"),
    LCGETNORM(GLfloat, 8, "Angle"));
LLFUNCEND
/* ========================================================================= */
// $ Texture:BlitLT
// > Left:number=The starting column to draw from.
// > Top:number=The starting row to draw from.
// ? Blits tile 0 of texture 0 at the specified screen co-ordinates with the
// ? internal tile width and height calculated when created.
/* ------------------------------------------------------------------------- */
LLFUNC(BlitLT, LCGETPTR(1, Texture)->BlitLT(0, 0,
  LCGETNUM(GLfloat, 2, "Left"), LCGETNUM(GLfloat, 3, "Top")));
/* ========================================================================= */
// $ Texture:BlitLTA
// > Left:number=The starting column to draw from.
// > Top:number=The starting row to draw from.
// > Angle:number=The angle in which to rotate from the centre point (-1 to 1).
// ? Blits tile 0 of texture 0 at the specified screen co-ordinates with the
// ? internal tile width and height calculated when created and with the
// ? specified angle.
/* ------------------------------------------------------------------------- */
LLFUNC(BlitLTA, LCGETPTR(1, Texture)->BlitLTA(0, 0,
  LCGETNUM(GLfloat, 2, "Left"), LCGETNUM(GLfloat, 3, "Top"),
  LCGETNORM(GLfloat, 4, "Angle")));
/* ========================================================================= */
// $ Texture:BlitSLT
// > TileIndex:integer=The tile index of the texture to blit.
// > Left:number=The starting column to draw from.
// > Top:number=The starting row to draw from.
// ? Blits tile 'TileIndex' of texture 0 at the specified screen co-ordinates
// ? with the internal tile width and height calculated when created.
/* ------------------------------------------------------------------------- */
LLFUNCBEGIN(BlitSLT)
  Texture &tCref = *LCGETPTR(1, Texture);
  tCref.BlitLT(0,
    LCGETINTLGE(size_t, 2, 0, tCref.GetTileCount(), "Tileindex"),
    LCGETNUM(GLfloat, 3, "Left"), LCGETNUM(GLfloat, 4, "Top"));
LLFUNCEND
/* ========================================================================= */
// $ Texture:BlitSLTA
// > TileIndex:integer=The tile index of the texture to blit.
// > Left:number=The starting column to draw from.
// > Top:number=The starting row to draw from.
// > Angle:number=The angle in which to rotate from the centre point (-1 to 1).
// ? Blits tile 'TileIndex' of texture 0 at the specified screen co-ordinates
// ? with the internal tile width and height calculated when created and with
// ? the specified angle.
/* ------------------------------------------------------------------------- */
LLFUNCBEGIN(BlitSLTA)
  Texture &tCref = *LCGETPTR(1, Texture);
  tCref.BlitLTA(0,
    LCGETINTLGE(size_t,  2, 0, tCref.GetTileCount(), "TileIndex"),
    LCGETNUM(GLfloat, 3, "Left"), LCGETNUM(GLfloat, 4, "Top"),
    LCGETNORM(GLfloat, 5, "Angle"));
LLFUNCEND
/* ========================================================================= */
// $ Texture:BlitILT
// > TexIndex:integer=The texture index to use.
// > Left:number=The starting column to draw from.
// > Top:number=The starting row to draw from.
// ? Blits tile 'TileIndex' of texture 0 at the specified screen co-ordinates
// ? with the internal tile width and height calculated when created.
/* ------------------------------------------------------------------------- */
LLFUNCBEGIN(BlitILT)
  Texture &tCref = *LCGETPTR(1, Texture);
  tCref.BlitLT(
    LCGETINTLGE(size_t, 2, 0, tCref.GetSubCount(), "Texindex"), 0,
    LCGETNUM(GLfloat, 3, "Left"), LCGETNUM(GLfloat, 4, "Top"));
LLFUNCEND
/* ========================================================================= */
// $ Texture:BlitILTA
// > TexIndex:integer=The texture index to use.
// > Left:number=The starting column to draw from.
// > Top:number=The starting row to draw from.
// > Angle:number=The angle in which to rotate from the centre point (-1 to 1).
// ? Blits tile 'TileIndex' of texture 0 at the specified screen co-ordinates
// ? with the internal tile width and height calculated when created and with
// ? the specified angle.
/* ------------------------------------------------------------------------- */
LLFUNCBEGIN(BlitILTA)
  Texture &tCref = *LCGETPTR(1, Texture);
  tCref.BlitLTA(
    LCGETINTLGE(size_t, 2, 0, tCref.GetSubCount(), "TexIndex"), 0,
    LCGETNUM(GLfloat, 3, "Left"), LCGETNUM(GLfloat, 4, "Top"),
    LCGETNORM(GLfloat, 5, "Angle"));
LLFUNCEND
/* ========================================================================= */
// $ Texture:BlitISLT
// > TexIndex:integer=The texture index to use.
// > TileIndex:integer=The tile index of the texture to blit.
// > Left:number=The starting column to draw from.
// > Top:number=The starting row to draw from.
// ? Blits tile 'TileIndex' of texture 'TexIndex' at the specified screen
// ? co-ordinates with the internal tile width and height calculated when
// ? created.
/* ------------------------------------------------------------------------- */
LLFUNCBEGIN(BlitISLT)
  Texture &tCref = *LCGETPTR(1, Texture);
  tCref.BlitLT(
    LCGETINTLGE(size_t, 2, 0, tCref.GetSubCount(),  "TexIndex"),
    LCGETINTLGE(size_t, 3, 0, tCref.GetTileCount(), "TileIndex"),
    LCGETNUM(GLfloat, 4, "Left"), LCGETNUM(GLfloat, 5, "Top"));
LLFUNCEND
/* ========================================================================= */
// $ Texture:BlitISLTA
// > TexIndex:integer=The texture index to use.
// > TileIndex:integer=The tile index of the texture to blit.
// > Left:number=The starting column to draw from.
// > Top:number=The starting row to draw from.
// > Angle:number=The angle in which to rotate from the centre point (-1 to 1).
// ? Blits tile 'TileIndex' of texture 'TexIndex' at the specified screen
// ? co-ordinates with the internal tile width and height calculated when
// ? created, and with the specified angle.
/* ------------------------------------------------------------------------- */
LLFUNCBEGIN(BlitISLTA)
  Texture &tCref = *LCGETPTR(1, Texture);
  tCref.BlitLTA(
    LCGETINTLGE(size_t, 2, 0, tCref.GetSubCount(),  "TexIndex"),
    LCGETINTLGE(size_t, 3, 0, tCref.GetTileCount(), "TileIndex"),
    LCGETNUM(GLfloat, 4, "Left"), LCGETNUM(GLfloat, 5, "Top"),
    LCGETNORM(GLfloat, 6, "Angle"));
LLFUNCEND
/* ========================================================================= */
// $ Texture:SetCRGB
// > Red:number=The colour intensity of the texture's red component (0-1).
// > Green:number=The colour intensity of the texture's green component (0-1).
// > Blue:number=The colour intensity of the texture's blue omponent (0-1).
// ? Sets the colour intensity of the texture for each component. The change
// ? affects all subsequent calls to all blitting functions.
/* ------------------------------------------------------------------------- */
LLFUNC(SetCRGB, LCGETPTR(1, Texture)->FboItemSetQuadRGB(
  LCGETNUM(GLfloat, 2, "Red"), LCGETNUM(GLfloat, 3, "Green"),
  LCGETNUM(GLfloat, 4, "Blue")));
/* ========================================================================= */
// $ Texture:SetCRGBA
// > Red:number=The colour intensity of the texture's red component (0-1).
// > Green:number=The colour intensity of the texture's green component (0-1).
// > Blue:number=The colour intensity of the texture's blue omponent (0-1).
// > Alpha:number=The transparency of the texture (0-1).
// ? Sets the colour intensity of the texture for each component. The change
// ? affects all subsequent calls to all blitting functions.
/* ------------------------------------------------------------------------- */
LLFUNC(SetCRGBA, LCGETPTR(1, Texture)->FboItemSetQuadRGBA(
  LCGETNUM(GLfloat, 2, "Red"),  LCGETNUM(GLfloat, 3, "Green"),
  LCGETNUM(GLfloat, 4, "Blue"), LCGETNUM(GLfloat, 5, "Alpha")));
/* ========================================================================= */
// $ Texture:SetCRGBAI
// > Colour:integer=The entire colour to set as an integer (0xAARRGGBB)
// ? Sets the colour intensity of the texture for each component using a 32
// ? bit integer.
/* ------------------------------------------------------------------------- */
LLFUNC(SetCRGBAI, LCGETPTR(1, Texture)->
  FboItemSetQuadRGBAInt(LCGETINT(uint32_t, 2, "Colour")));
/* ========================================================================= */
// $ Texture:SetCR
// > Red:number=The colour intensity of the texture's red component (0-1).
// ? Sets the colour intensity of the texture for the red component. The change
// ? affects all subsequent calls to all blitting functions.
/* ------------------------------------------------------------------------- */
LLFUNC(SetCR, LCGETPTR(1, Texture)->
  FboItemSetQuadRed(LCGETNUM(GLfloat, 2, "Red")));
/* ========================================================================= */
// $ Texture:SetCG
// > Green:number=The colour intensity of the texture's green component (0-1).
// ? Sets the colour intensity of the texture for the green component. The
// ? change affects all subsequent calls to all blitting functions.
/* ------------------------------------------------------------------------- */
LLFUNC(SetCG, LCGETPTR(1, Texture)->
  FboItemSetQuadGreen(LCGETNUM(GLfloat, 2, "Green")));
/* ========================================================================= */
// $ Texture:SetCB
// > Blue:number=The colour intensity of the texture's blue component (0-1).
// ? Sets the colour intensity of the texture for the blue component. The
// ? change affects all subsequent calls to all blitting functions.
/* ------------------------------------------------------------------------- */
LLFUNC(SetCB, LCGETPTR(1, Texture)->
  FboItemSetQuadBlue(LCGETNUM(GLfloat, 2, "Blue")));
/* ========================================================================= */
// $ Texture:SetCA
// > Alpha:number=The transparency of the texture (0-1).
// ? Sets the colour transparency of the texture. The change affects all
// ? subsequent calls to all blitting functions.
/* ------------------------------------------------------------------------- */
LLFUNC(SetCA, LCGETPTR(1, Texture)->
  FboItemSetQuadAlpha(LCGETNUM(GLfloat, 2, "Alpha")));
/* ========================================================================= */
// $ Texture:BlitM
// > Columns:integer=The number of horizonal textures to blit.
// > Left:number=The left position of the blit.
// > Top:number=The top position of the blit.
// > Right:number=The right position of the blit.
// > Bottom:number=The bottom position of the blit.
// ? Blits multiple textures as one giant texture which is a workaround for
// ? texture size limits on OpenGL.
/* ------------------------------------------------------------------------- */
LLFUNC(BlitM, LCGETPTR(1, Texture)->BlitMulti(
  LCGETINT(GLuint,  2, "Columns"), LCGETNUM(GLfloat, 3, "Left"),
  LCGETNUM(GLfloat, 4, "Top"), LCGETNUM(GLfloat, 5, "Right"),
  LCGETNUM(GLfloat, 6, "Bottom")));
/* ========================================================================= */
// $ Texture:TileSTC
// > Count:integer=The new size of the tile list.
// ? Modifies the size of the tile list. Useful if you want to manage
// ? your tile texcoord list dynamically. If the new size is below the current
// ? size then existing entries will be deleted, otherwise if the new size
// ? is larger, then new blank entries are created and are set to zero. You
// ? will need to use SetTileData to change them.
/* ------------------------------------------------------------------------- */
LLFUNC(TileSTC, LCGETPTR(1, Texture)->
  SetTileCount(LCGETINTLG(size_t, 2, 0, UINT_MAX, "Count")));
/* ========================================================================= */
// $ Texture:TileSSTC
// > Id:integer=The id of the sub-texture to set the size of.
// > Count:integer=The new size of the tile list.
// ? Modifies the size of the tile list. Useful if you want to manage
// ? your tile texcoord list dynamically. If the new size is below the current
// ? size then existing entries will be deleted, otherwise if the new size
// ? is larger, then new blank entries are created and are set to zero. You
// ? will need to use SetTileData to change them.
/* ------------------------------------------------------------------------- */
LLFUNCBEGIN(TileSSTC)
  Texture &tCref = *LCGETPTR(1, Texture);
  tCref.SetTileCount(LCGETINTLG(size_t, 3, 0, UINT_MAX, "Count"),
    LCGETINTLGE(size_t, 2, 0, tCref.GetSubCount(), "TexId"));
LLFUNCEND
/* ========================================================================= */
// $ Texture:TileAS
// > Id:integer=The id of the sub-texture to add to
// > Left:integer=The left pixel position on the texture.
// > Top:integer=The top pixel position on the texture.
// > Right:integer=The right pixel position on the texture
// > Bottom:integer=The bottom pixel position on the texture
// < Id:integer=The id number for the cached texcoords.
// ? Caches the specified texture coordinates which you can use as a parameter
// ? to 'tileid' on blitting functions of the specified sub-texture.
/* ------------------------------------------------------------------------- */
LLFUNCBEGIN(TileAS)
  Texture &tCref = *LCGETPTR(1, Texture);
  const size_t stId = LCGETINTLGE(size_t, 2, 0, tCref.GetSubCount(), "TexId");
  tCref.AddTileDOR(stId,
    LCGETNUMLG(GLfloat, 3, 0.0f, tCref.GetFWidth(),  "Left"),
    LCGETNUMLG(GLfloat, 4, 0.0f, tCref.GetFHeight(), "Top"),
    LCGETNUMLG(GLfloat, 5, 0.0f, tCref.GetFWidth(),  "Right"),
    LCGETNUMLG(GLfloat, 6, 0.0f, tCref.GetFHeight(), "Bottom"));
  LCPUSHVAR(tCref.GetTileCount(stId)-1);
LLFUNCENDEX(1)
/* ========================================================================= */
// $ Texture:TileASD
// > Id:integer=The id of the sub-texture to add to.
// > Left:integer=The left pixel position on the texture..
// > Top:integer=The top pixel position on the texture..
// > Width:integer=The width of the texture..
// > Height:integer=The height of the texture..
// < Id:integer=The id number for the cached texcoords..
// ? Caches the specified texture coordinates which you can use as a parameter
// ? to 'tileid' on blitting functions of the specified sub-texture.
/* ------------------------------------------------------------------------- */
LLFUNCBEGIN(TileASD)
  Texture &tCref = *LCGETPTR(1, Texture);
  const size_t stId = LCGETINTLGE(size_t, 2, 0, tCref.GetSubCount(), "TexId");
  const GLfloat
    fX = LCGETNUMLG(GLfloat, 3, 0.0f, tCref.GetFWidth(), "Left"),
    fY = LCGETNUMLG(GLfloat, 4, 0.0f, tCref.GetFHeight(), "Top");
  tCref.AddTileDORWH(stId, fX, fY,
    LCGETNUMLG(GLfloat, 5, 0.0f, tCref.GetFWidth()-fX, "Width"),
    LCGETNUMLG(GLfloat, 6, 0.0f, tCref.GetFHeight()-fY, "Height"));
  LCPUSHVAR(tCref.GetTileCount(stId)-1);
LLFUNCENDEX(1)
/* ========================================================================= */
// $ Texture:TileA
// > Left:integer=The left pixel position on the texture..
// > Top:integer=The top pixel position on the texture..
// > Right:integer=The right pixel position on the texture.
// > Bottom:integer=The bottom pixel position on the texture.
// < Id:integer=The id number for the cached texcoords..
// ? Caches the specified texture coordinates which you can use as a parameter
// ? to 'tileid' on blitting functions.
/* ------------------------------------------------------------------------- */
LLFUNCBEGIN(TileA)
  Texture &tCref = *LCGETPTR(1, Texture);
  tCref.AddTileDOR(0,
    LCGETNUMLG(GLfloat, 2, 0.0f, tCref.GetFWidth(), "Left"),
    LCGETNUMLG(GLfloat, 3, 0.0f, tCref.GetFHeight(), "Top"),
    LCGETNUMLG(GLfloat, 4, 0.0f, tCref.GetFWidth(), "Right"),
    LCGETNUMLG(GLfloat, 5, 0.0f, tCref.GetFHeight(), "Bottom"));
  LCPUSHVAR(tCref.GetTileCount()-1);
LLFUNCENDEX(1)
/* ========================================================================= */
// $ Texture:TileAD
// > Left:integer=The left pixel position on the texture.
// > Top:integer=The top pixel position on the texture.
// > Width:integer=The width of the texture.
// > Height:integer=The height of the texture.
// < Id:integer=The id number for the cached texcoords.
// ? Caches the specified texture coordinates which you can use as a parameter
// ? to 'tileid' on blitting functions.
/* ------------------------------------------------------------------------- */
LLFUNCBEGIN(TileAD)
  Texture &tCref = *LCGETPTR(1, Texture);
  const GLfloat
    fX = LCGETNUMLG(GLfloat, 2, 0.0f, tCref.GetFWidth(), "Left"),
    fY = LCGETNUMLG(GLfloat, 3, 0.0f, tCref.GetFHeight(), "Top");
  tCref.AddTileDORWH(0, fX, fY,
    LCGETNUMLG(GLfloat, 4, 0.0f, tCref.GetFWidth()-fX, "Width"),
    LCGETNUMLG(GLfloat, 5, 0.0f, tCref.GetFHeight()-fY, "Height"));
  LCPUSHVAR(tCref.GetTileCount()-1);
LLFUNCENDEX(1)
/* ========================================================================= */
// $ Texture:TileSS
// > TexId:integer=The id of the sub texture to change.
// > TileId:integer=The id of the tile id to change.
// > Left:integer=The left pixel position on the texture.
// > Top:integer=The top pixel position on the texture.
// > Right:integer=The right pixel position on the texture
// > Bottom:integer=The bottom pixel position on the texture
// ? Changes the specified texture coordinates which you can use as a parameter
// ? to 'tileid' on blitting functions.
/* ------------------------------------------------------------------------- */
LLFUNCBEGIN(TileSS)
  Texture &tCref = *LCGETPTR(1, Texture);
  const size_t stTexId =
    LCGETINTLGE(size_t,  2, 0,    tCref.GetSubCount(),         "TexId");
  tCref.SetTileDOR(stTexId,
    LCGETINTLGE(size_t,  3, 0,    tCref.GetTileCount(stTexId), "TileId"),
    LCGETNUMLG (GLfloat, 4, 0.0f, tCref.GetFWidth(),           "Left"),
    LCGETNUMLG (GLfloat, 5, 0.0f, tCref.GetFHeight(),          "Top"),
    LCGETNUMLG (GLfloat, 6, 0.0f, tCref.GetFWidth(),           "Right"),
    LCGETNUMLG (GLfloat, 7, 0.0f, tCref.GetFHeight(),          "Bottom"));
LLFUNCEND
/* ========================================================================= */
// $ Texture:TileSSD
// > TexId:integer=The id of the sub texture to change.
// > TileId:integer=The id of the tile id to change.
// > Left:integer=The left pixel position on the texture.
// > Top:integer=The top pixel position on the texture.
// > Width:integer=The width of the texture.
// > Height:integer=The height of the texture.
// ? Changes the specified texture coordinates which you can use as a parameter
// ? to 'tileid' on blitting functions.
/* ------------------------------------------------------------------------- */
LLFUNCBEGIN(TileSSD)
  Texture &tCref = *LCGETPTR(1, Texture);
  const size_t
    stTexId = LCGETINTLGE(size_t, 2, 0, tCref.GetSubCount(), "TexId"),
    stTileId = LCGETINTLGE(size_t, 3, 0, tCref.GetTileCount(stTexId),
      "TileId");
  const GLfloat
    fX = LCGETNUMLG(GLfloat, 4, 0.0f, tCref.GetFWidth(), "Left"),
    fY = LCGETNUMLG(GLfloat, 5, 0.0f, tCref.GetFHeight(), "Top");
  tCref.SetTileDORWH(stTexId, stTileId, fX, fY,
    LCGETNUMLG(GLfloat, 6, 0.0f, tCref.GetFWidth()-fX, "Width"),
    LCGETNUMLG(GLfloat, 7, 0.0f, tCref.GetFHeight()-fY, "Height"));
LLFUNCEND
/* ========================================================================= */
// $ Texture:TileS
// > Id:integer=The id of the tile to change
// > Left:integer=The left pixel position on the texture.
// > Top:integer=The top pixel position on the texture.
// > Right:integer=The right pixel position on the texture
// > Bottom:integer=The bottom pixel position on the texture
// ? Changes the specified texture coordinates which you can use as a parameter
// ? to 'tileid' on blitting functions.
/* ------------------------------------------------------------------------- */
LLFUNCBEGIN(TileS)
  Texture &tCref = *LCGETPTR(1, Texture);
  tCref.SetTileDOR(0,
    LCGETINTLGE(size_t, 2, 0, tCref.GetTileCount(0), "Index"),
    LCGETNUMLG(GLfloat, 3, 0.0f, tCref.GetFWidth(), "Left"),
    LCGETNUMLG(GLfloat, 4, 0.0f, tCref.GetFHeight(), "Top"),
    LCGETNUMLG(GLfloat, 5, 0.0f, tCref.GetFWidth(), "Right"),
    LCGETNUMLG(GLfloat, 6, 0.0f, tCref.GetFHeight(), "Bottom"));
LLFUNCEND
/* ========================================================================= */
// $ Texture:TileSD
// > Id:integer=The id of the tile to change
// > Left:integer=The left pixel position on the texture.
// > Top:integer=The top pixel position on the texture.
// > Width:integer=The width of the texture.
// > Height:integer=The height of the texture.
// ? Changes the specified texture coordinates which you can use as a parameter
// ? to 'tileid' on blitting functions.
/* ------------------------------------------------------------------------- */
LLFUNCBEGIN(TileSD)
  Texture &tCref = *LCGETPTR(1, Texture);
  const size_t stId =
    LCGETINTLGE(size_t, 2, 0, tCref.GetTileCount(0), "Index");
  const GLfloat
    fX = LCGETNUMLG(GLfloat, 3, 0.0f, tCref.GetFWidth(), "Left"),
    fY = LCGETNUMLG(GLfloat, 4, 0.0f, tCref.GetFHeight(), "Top");
  tCref.SetTileDORWH(0, stId, fX, fY,
    LCGETNUMLG(GLfloat, 5, 0.0f, tCref.GetFWidth()-fX, "Width"),
    LCGETNUMLG(GLfloat, 6, 0.0f, tCref.GetFHeight()-fY, "Height"));
LLFUNCEND
/* ========================================================================= */
// $ Texture:TileGTC
// < Count:integer=The handle of the texture created.
// ? Returns number of tiles found in the loaded texture file.
/* ------------------------------------------------------------------------- */
LLFUNCEX(TileGTC, 1, LCPUSHVAR(LCGETPTR(1, Texture)->GetTileCount()));
/* ========================================================================= */
// $ Texture:TileGSTC
// > Id:integer=The id of the sub-texture to get the count for.
// < Count:integer=The handle of the texture created.
// ? Returns number of tiles found in the loaded texture file with the
// ? specified sub-texture id.
/* ------------------------------------------------------------------------- */
LLFUNCBEGIN(TileGSTC)
  const Texture &tCref = *LCGETPTR(1, Texture);
  LCPUSHVAR(tCref.GetTileCount(
    LCGETINTLGE(size_t, 2, 0, tCref.GetSubCount(), "TexId")));
LLFUNCENDEX(1)
/* ========================================================================= */
// $ Texture:GetSubCount
// < Count:integer=The number of sub-textures in this texture
// ? Returns number of sub-textures found in the loaded image file.
/* ------------------------------------------------------------------------- */
LLFUNCEX(GetSubCount, 1, LCPUSHVAR(LCGETPTR(1, Texture)->GetSubCount()));
/* ========================================================================= */
// $ Texture:GetHeight
// < Height:integer=The handle of the new mask created.
// ? Returns height of texture.
/* ------------------------------------------------------------------------- */
LLFUNCEX(GetHeight, 1, LCPUSHVAR(LCGETPTR(1, Texture)->DimGetHeight()));
/* ========================================================================= */
// $ Texture:GetId
// < Id:integer=The id number of the Texture object.
// ? Returns the unique id of the Texture object.
/* ------------------------------------------------------------------------- */
LLFUNCEX(GetId, 1, LCPUSHVAR(LCGETPTR(1, Texture)->CtrGet()));
/* ========================================================================= */
// $ Texture:GetName
// < Name:string=Name of the texture.
// ? If this texture was loaded by a filename or it was set with a custom id.
// ? This function returns that name which was assigned to it.
/* ------------------------------------------------------------------------- */
LLFUNCEX(GetName, 1, LCPUSHVAR(LCGETPTR(1, Texture)->IdentGet()));
/* ========================================================================= */
// $ Texture:GetWidth
// < Width:integer=The handle of the new mask created.
// ? Returns width of texture.
/* ------------------------------------------------------------------------- */
LLFUNCEX(GetWidth, 1, LCPUSHVAR(LCGETPTR(1, Texture)->DimGetWidth()));
/* ========================================================================= */
// $ Texture:Dump
// ? Dumps the texture to the specified file in the specified format.
/* ------------------------------------------------------------------------- */
LLFUNCBEGIN(Dump)
  const Texture &tCref = *LCGETPTR(1, Texture);
  tCref.Dump(LCGETINTLGE(size_t, 2, 0, tCref.GetSubCount(), "TexId"),
    LCGETCPPFILE(3, "File"));
LLFUNCEND
/* ========================================================================= */
// $ Texture:Download
// > TexId:Integer=The sub-id of the texture to download
// < Handle:Image=The image class
// ? Dumps the texture from VRAM to the specified array. The format of which
// ? is how the image was originally uploaded
/* ------------------------------------------------------------------------- */
LLFUNCBEGIN(Download)
  const Texture &tCref = *LCGETPTR(1, Texture);
  Image imNew{ tCref.Download(LCGETINTLGE(size_t, 2, 0, tCref.GetSubCount(),
    "TexId")) };
  LCCLASSCREATE(Image)->SwapImage(imNew);
LLFUNCENDEX(1)
/* ========================================================================= */
// $ Texture:Upload
// > Data:Image=The pixel data to update the texture with
// ? Updates the whole texture in VRAM with the texture specified in the array.
// ? The array data must be the same width, the same height and the same pixel
// ? type of when the texture was first originally updated, thus the size of
// ? the memory must match the actual texture memory. The source and
// ? destination colour format must be the same, or use UploadEx
/* ------------------------------------------------------------------------- */
LLFUNC(Upload, LCGETPTR(1, Texture)->Update(*LCGETPTR(2, Image)));
/* ========================================================================= */
// $ Texture:UploadEx
// > TexId:Integer=The sub-id of the texture to download
// > Data:Image=The image to upload to the texture
// > OffsetX:Integer=The X position on the destination texture where to place
// > OffsetY:Integer=The Y position on the destination texture where to place
// > Width:Integer=The width of the image stored in the 'Data' array
// > Height:Integer=The height of the image stored in the 'Data' array
// > Alpha:boolean=The source image is RGBA 32bpp? Else it is RGB 24bpp.
// ? Updates a partial area of the supplied texture in VRAM with the texture
// ? data specified in the array. Only sources and destinations of 24 and 32
// ? bits per pixel are supported.
/* ------------------------------------------------------------------------- */
LLFUNCBEGIN(UploadEx)
  Texture &tCref = *LCGETPTR(1, Texture);
  tCref.UpdateEx(LCGETINTLGE(size_t, 2, 0, tCref.GetSubCount(), "TexId"),
    *LCGETPTR(3, Image), LCGETINT(GLint, 4, "OffsetX"),
    LCGETINT(GLint, 5, "OffsetY"));
LLFUNCEND
/* ========================================================================= */
// $ Texture:SetVX
// > TriIndex:integer=Triangle index. Triangle #1 (zero) or triangle #2 (one).
// > V1Left:number=GLfloat for X coord of vertex #1 of the specified triangle.
// > V1Top:number=GLfloat for Y coord of vertex #1 of the specified triangle.
// > V2Left:number=GLfloat for X coord of vertex #2 of the specified triangle.
// > V2Top:number=GLfloat for Y coord of vertex #2 of the specified triangle.
// > V3Left:number=GLfloat for X coord of vertex #3 of the specified triangle.
// > V3Top:number=GLfloat for Y coord of vertex #3 of the specified triangle.
// ? Presets the specified co-ordinates on each vertex for the specified
// ? triangle for the Texture:BlitP* function.
/* ------------------------------------------------------------------------- */
LLFUNC(SetVX, LCGETPTR(1, Texture)->
  FboItemSetVertexEx(LCGETINTLGE(size_t, 2, 0, stTrisPerQuad, "TriIndex"), {
    LCGETNUM(GLfloat, 3, "V1Left"), LCGETNUM(GLfloat, 4, "V1Top"),
    LCGETNUM(GLfloat, 5, "V2Left"), LCGETNUM(GLfloat, 6, "V2Top"),
    LCGETNUM(GLfloat, 7, "V3Left"), LCGETNUM(GLfloat, 8, "v3Top")
  }));
/* ========================================================================= */
// $ Texture:SetCX
// > TriIndex:integer=Triangle index. Triangle #1 (zero) or triangle #2 (one).
// > V1Red:number=Red component of vertex #1 of the specified triangle.
// > V1Green:number=Green component of vertex #1 of the specified triangle.
// > V1Blue:number=Blue component of vertex #1 of the specified triangle.
// > V1Alpha:number=Alpha component of vertex #1 of the specified triangle.
// > V2Red:number=Red component of vertex #2 of the specified triangle.
// > V2Green:number=Green component of vertex #2 of the specified triangle.
// > V2Blue:number=Blue component of vertex #2 of the specified triangle.
// > V2Alpha:number=Alpha component of vertex #2 of the specified triangle.
// > V3Red:number=Red component of vertex #3 of the specified triangle.
// > V3Green:number=Green component of vertex #3 of the specified triangle.
// > V3Blue:number=Blue component of vertex #3 of the specified triangle.
// > V3Alpha:number=Alpha component of vertex #3 of the specified triangle.
// ? Presets the specified colour intensities on each vertex for the
// ? Texture:BlitP* function.
/* ------------------------------------------------------------------------- */
LLFUNC(SetCX, LCGETPTR(1, Texture)->
  FboItemSetColourEx(LCGETINTLGE(size_t, 2, 0, stTrisPerQuad, "TriIndex"), {
    LCGETNUM(GLfloat, 3, "V1Red"), LCGETNUM(GLfloat, 4, "V1Green"),
    LCGETNUM(GLfloat, 5, "V1Blue"), LCGETNUM(GLfloat, 6, "V1Alpha"),
    LCGETNUM(GLfloat, 7, "V2Red"), LCGETNUM(GLfloat, 8, "V2Green"),
    LCGETNUM(GLfloat, 9, "V2Blue"), LCGETNUM(GLfloat, 10, "V2Alpha"),
    LCGETNUM(GLfloat, 11, "V3Red"), LCGETNUM(GLfloat, 12, "V3Green"),
    LCGETNUM(GLfloat, 13, "V3Blue"), LCGETNUM(GLfloat, 14, "V3Alpha")}));
/* ========================================================================= */
// $ Texture:SetTCLTRB
// > Left:number=The left co-ordinate.
// > Top:number=The top co-ordinate.
// > Right:number=The right co-ordinate.
// > Bottom:number=The bottom co-ordinate.
// ? Preset the specified texture bounds for the Texture:BlitP* function.
/* ------------------------------------------------------------------------- */
LLFUNC(SetTCLTRB, LCGETPTR(1, Texture)->FboItemSetTexCoord(
  LCGETNUM(GLfloat, 2, "Left"), LCGETNUM(GLfloat, 3, "Top"),
  LCGETNUM(GLfloat, 4, "Right"), LCGETNUM(GLfloat, 5, "Bottom")));
/* ========================================================================= */
// $ Texture:SetTCLTWH
// > Left:number=The left co-ordinate.
// > Top:number=The top co-ordinate.
// > Width:number=The width of the tile.
// > Height:number=The height of the tile.
// ? Preset the specified texture co-ordinates and dimensions for the
// ? Texture:BlitP* function.
/* ------------------------------------------------------------------------- */
LLFUNC(SetTCLTWH, LCGETPTR(1, Texture)->FboItemSetTexCoordWH(
  LCGETNUM(GLfloat, 2, "Left"), LCGETNUM(GLfloat, 3, "Top"),
  LCGETNUM(GLfloat, 4, "Width"), LCGETNUM(GLfloat, 5, "Height")));
/* ========================================================================= */
// $ Texture:SetTCX
// > TriIndex:integer=Triangle index. Triangle #1 (zero) or triangle #2 (one).
// > TC1Left:number=GLfloat for X coord of vertex #1 of the specified triangle.
// > TC1Top:number=GLfloat for Y coord of vertex #2 of the specified triangle.
// > TC2Left:number=GLfloat for X coord of vertex #3 of the specified triangle.
// > TC2Top:number=GLfloat for Y coord of vertex #1 of the specified triangle.
// > TC3Left:number=GLfloat for X coord of vertex #2 of the specified triangle.
// > TC3Top:number=GLfloat for Y coord of vertex #3 of the specified triangle.
// ? Presets the specified texture co-ordinates on the specified triangle for
// ? the Texture:BlitP* function.
/* ------------------------------------------------------------------------- */
LLFUNC(SetTCX, LCGETPTR(1, Texture)->
  FboItemSetTexCoordEx(LCGETINTLGE(size_t, 2, 0, stTrisPerQuad, "TriIndex"), {
    LCGETNUM(GLfloat, 3, "TC1Left"), LCGETNUM(GLfloat, 4, "TC1Top"),
    LCGETNUM(GLfloat, 5, "TC2Left"), LCGETNUM(GLfloat, 6, "TC2Top"),
    LCGETNUM(GLfloat, 7, "TC3Left"), LCGETNUM(GLfloat, 8, "TC3Top")
  }));
/* ========================================================================= */
// $ Texture:PushColour
// ? Saves the currently set colour by SetCRGBA. Use PopColour to restore it.
/* ------------------------------------------------------------------------- */
LLFUNC(PushColour, LCGETPTR(1, Texture)->FboItemPushQuadColour());
/* ========================================================================= */
// $ Texture:PopColour
// ? Restores the currently saved colour by PushColour.
/* ------------------------------------------------------------------------- */
LLFUNC(PopColour, LCGETPTR(1, Texture)->FboItemPopQuadColour());
/* ========================================================================= */
// $ Texture:Destroy
// ? Destroys the texture and frees all the memory associated with it. The
// ? OpenGL handles and VRAM allocated by it are freed after the main FBO has
// ? has been rendered. The object will no longer be useable after this call
// ? and an error will be generated if accessed.
/* ------------------------------------------------------------------------- */
LLFUNC(Destroy, LCCLASSDESTROY(1, Texture));
/* ========================================================================= **
** ######################################################################### **
** ## Texture:* member functions structure                                ## **
** ######################################################################### **
** ------------------------------------------------------------------------- */
LLRSMFBEGIN                            // Texture:* member functions begin
  LLRSFUNC(Blit),        LLRSFUNC(BlitILT),     LLRSFUNC(BlitILTA),
  LLRSFUNC(BlitILTRB),   LLRSFUNC(BlitILTRBA),  LLRSFUNC(BlitILTWH),
  LLRSFUNC(BlitILTWHA),  LLRSFUNC(BlitISLT),    LLRSFUNC(BlitISLTA),
  LLRSFUNC(BlitISLTRB),  LLRSFUNC(BlitISLTRBA), LLRSFUNC(BlitISLTWH),
  LLRSFUNC(BlitISLTWHA), LLRSFUNC(BlitLT),      LLRSFUNC(BlitLTA),
  LLRSFUNC(BlitLTRB),    LLRSFUNC(BlitLTRBA),   LLRSFUNC(BlitLTWH),
  LLRSFUNC(BlitLTWHA),   LLRSFUNC(BlitM),       LLRSFUNC(BlitSLT),
  LLRSFUNC(BlitSLTA),    LLRSFUNC(BlitSLTRB),   LLRSFUNC(BlitSLTRBA),
  LLRSFUNC(BlitSLTWH),   LLRSFUNC(BlitSLTWHA),  LLRSFUNC(Destroy),
  LLRSFUNC(Download),    LLRSFUNC(Dump),        LLRSFUNC(GetHeight),
  LLRSFUNC(GetId),       LLRSFUNC(GetName),     LLRSFUNC(GetSubCount),
  LLRSFUNC(GetWidth),    LLRSFUNC(PopColour),   LLRSFUNC(PushColour),
  LLRSFUNC(SetCA),       LLRSFUNC(SetCB),       LLRSFUNC(SetCG),
  LLRSFUNC(SetCR),       LLRSFUNC(SetCRGB),     LLRSFUNC(SetCRGBA),
  LLRSFUNC(SetCRGBAI),   LLRSFUNC(SetCX),       LLRSFUNC(SetTCLTRB),
  LLRSFUNC(SetTCLTWH),   LLRSFUNC(SetTCX),      LLRSFUNC(SetVLTRB),
  LLRSFUNC(SetVLTRBA),   LLRSFUNC(SetVLTWH),    LLRSFUNC(SetVLTWHA),
  LLRSFUNC(SetVX),       LLRSFUNC(TileA),       LLRSFUNC(TileAD),
  LLRSFUNC(TileAS),      LLRSFUNC(TileASD),     LLRSFUNC(TileGSTC),
  LLRSFUNC(TileGTC),     LLRSFUNC(TileS),       LLRSFUNC(TileSD),
  LLRSFUNC(TileSS),      LLRSFUNC(TileSSD),     LLRSFUNC(TileSSTC),
  LLRSFUNC(TileSTC),     LLRSFUNC(Upload),      LLRSFUNC(UploadEx),
LLRSEND                                // Texture:* member functions end
/* ========================================================================= */
// $ Texture.Create
// > Source:image=The image class to load from.
// > Filter:integer=The default filter to use
// < Handle:Texture=A handle to the texture(s) created.
// ? Creates the specified texture from the specified image class.
/* ========================================================================= */
LLFUNCEX(Create, 1, LCCLASSCREATE(Texture)->InitImage(*LCGETPTR(1, Image),
  0, 0, 0, 0, LCGETINTLGE(OglFilterEnum, 2, OF_N_N, OF_MAX, "Filter")));
/* ========================================================================= */
// $ Texture.CreateTS
// > Source:image=The image class to load from.
// > TWidth:integer=The width of each tile in the image.
// > THeight:integer=The height of each tile in the image.
// > PWidth:integer=The width padding between each tile in the image.
// > PHeight:integer=The height padding between each tile in the image.
// > Filter:integer=The filtering setting to use on texture
// < Handle:Texture=A handle to the texture(s) created.
// ? Loads a texture as a tileset from the specified image class.
/* ------------------------------------------------------------------------- */
LLFUNCEX(CreateTS, 1, LCCLASSCREATE(Texture)->InitImage(*LCGETPTR(1, Image),
  LCGETINTLG(GLuint, 2, 0, UINT_MAX, "TWidth"),
  LCGETINTLG(GLuint, 3, 0, UINT_MAX, "THeight"),
  LCGETINTLG(GLuint, 4, 0, UINT_MAX, "PWidth"),
  LCGETINTLG(GLuint, 5, 0, UINT_MAX, "PHeight"),
  LCGETINTLGE(OglFilterEnum, 6, OF_N_N, OF_MAX, "Filter")));
/* ========================================================================= */
// $ Texture.Console
// < Handle:Texture=Texture handle to console texture
// ? Returns the handle to the console texture. Useful if you want to reuse the
// ? texture in your application. Careful not to mess around with it's
// ? properties!
/* ------------------------------------------------------------------------- */
LLFUNCEX(Console, 1, LCCLASSCREATEPTR(Texture, cConGraphics->GetTexture()));
/* ========================================================================= **
** ######################################################################### **
** ## Texture.* namespace functions structure                             ## **
** ######################################################################### **
** ------------------------------------------------------------------------- */
LLRSBEGIN                              // Texture.* namespace functions begin
  LLRSFUNC(Create), LLRSFUNC(CreateTS), LLRSFUNC(Console),
LLRSEND                                // Texture.* namespace functions end
/* ========================================================================= */
}                                      // End of Texture namespace
/* == EoF =========================================================== EoF == */

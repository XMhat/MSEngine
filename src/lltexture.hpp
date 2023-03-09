/* == LLTEXTURE.HPP ======================================================== */
/* ######################################################################### */
/* ## MS-ENGINE              Copyright (c) MS-Design, All Rights Reserved ## */
/* ######################################################################### */
/* ## Defines the 'Texture' namespace and methods for the guest to use in ## */
/* ## Lua. This file is invoked by 'lualib.hpp'.                          ## */
/* ######################################################################### */
/* ------------------------------------------------------------------------- */
#pragma once                           // Only one incursion allowed
/* ========================================================================= */
/* ######################################################################### */
/* ========================================================================= */
// % Texture
/* ------------------------------------------------------------------------- */
// ! The texture class allows the programmer to draw textures on the screen
// ! which also includes font management.
/* ========================================================================= */
namespace NsTexture {                  // Texture namespace
/* -- Includes ------------------------------------------------------------- */
using namespace IfTexture;             // Using texture namespace
using namespace IfConsole;             // Using console namespace
/* ========================================================================= */
/* ######################################################################### */
/* ========================================================================= */
// $ Texture:BlitLTRB
// > X:number=The X screen position of the string.
// > Y:number=The Y screen position of the string.
// > Width:number=A value between 0-1 specifing the red component intensity.
// > Height:number=A value between 0-1 specifing the green component intensity.
// ? Blits tile 0 of texture 0 at the specified screen co-ordinates and
// ? with the specified dimensions.
/* ------------------------------------------------------------------------- */
LLFUNC(BlitLTRB, LCGETPTR(1, Texture)->BlitLTRB(0, 0,
  LCGETNUM(GLfloat, 2, "X"),     LCGETNUM(GLfloat, 3, "Y"),
  LCGETNUM(GLfloat, 4, "Right"), LCGETNUM(GLfloat, 5, "Bottom")));
/* ========================================================================= */
// $ Texture:SetLTRB
// > X:number=The X screen position of the string.
// > Y:number=The Y screen position of the string.
// > Width:number=A value between 0-1 specifing the red component intensity.
// > Height:number=A value between 0-1 specifing the green component intensity.
// ? Presets the position for the next blit.
/* ------------------------------------------------------------------------- */
LLFUNC(SetLTRB, LCGETPTR(1, Texture)->SetVertex(
  LCGETNUM(GLfloat, 2, "X"),     LCGETNUM(GLfloat, 3, "Y"),
  LCGETNUM(GLfloat, 4, "Right"), LCGETNUM(GLfloat, 5, "Bottom")));
/* ========================================================================= */
// $ Texture:BlitLTRBA
// > X:number=The X screen position of the string.
// > Y:number=The Y screen position of the string.
// > Width:number=A value between 0-1 specifing the red component intensity.
// > Height:number=A value between 0-1 specifing the green component intensity.
// > Angle:number=The angle in which to rotate the texture (1.0=full rotation).
// ? Blits tile 0 of texture 0 at the specified screen co-ordinates and
// ? with the specified dimensions and angle. (FIXME) The angle isn't in any
// ? known measurement at the moment.
/* ------------------------------------------------------------------------- */
LLFUNC(BlitLTRBA, LCGETPTR(1, Texture)->BlitLTRBA(0, 0,
  LCGETNUM(GLfloat, 2, "X"),     LCGETNUM(GLfloat, 3, "Y"),
  LCGETNUM(GLfloat, 4, "Right"), LCGETNUM(GLfloat, 5, "Bottom"),
  LCGETNUM(GLfloat, 6, "Angle")));
/* ========================================================================= */
// $ Texture:SetLTRBA
// > X:number=The X screen position of the string.
// > Y:number=The Y screen position of the string.
// > Width:number=A value between 0-1 specifing the red component intensity.
// > Height:number=A value between 0-1 specifing the green component intensity.
// > Angle:number=The angle in which to rotate the texture (1.0=full rotation).
// ? Presets the position for the next blit with angle.
/* ------------------------------------------------------------------------- */
LLFUNC(SetLTRBA, LCGETPTR(1, Texture)->SetVertex(
  LCGETNUM(GLfloat, 2, "X"),     LCGETNUM(GLfloat, 3, "Y"),
  LCGETNUM(GLfloat, 4, "Right"), LCGETNUM(GLfloat, 5, "Bottom"),
  LCGETNUM(GLfloat, 6, "Angle")));
/* ========================================================================= */
// $ Texture:BlitSLTRB
// > TileIndex:integer=The tile index of the texture to blit.
// > X:number=The X screen position of the string.
// > Y:number=The Y screen position of the string.
// > Width:number=A value between 0-1 specifing the red component intensity.
// > Height:number=A value between 0-1 specifing the green component intensity.
// ? Blits tile 'TileIndex' of texture 0 at the specified screen co-ordinates
// ? and with the specified dimensions.
/* ------------------------------------------------------------------------- */
LLFUNCBEGIN(BlitSLTRB)
  Texture &tCref = *LCGETPTR(1, Texture);
  tCref.BlitLTRB(0,
    LCGETINTLGE(size_t,  2, 0, tCref.GetTileCount(), "TileIndex"),
    LCGETNUM   (GLfloat, 3,                          "Left"),
    LCGETNUM   (GLfloat, 4,                          "Top"),
    LCGETNUM   (GLfloat, 5,                          "Right"),
    LCGETNUM   (GLfloat, 6,                          "Bottom"));
LLFUNCEND
/* ========================================================================= */
// $ Texture:Blit
// ? Blits tile 0 of texture 0 with the current stored position.
/* ------------------------------------------------------------------------- */
LLFUNC(Blit, LCGETPTR(1, Texture)->Blit(0, 0));
/* ========================================================================= */
// $ Texture:BlitX
// ? Blits with the currently stored texture at position 0 with the stored
// ? texcoord (SetTC*), vertex (SetV*) and colour (SetC*). Note
// ? that the Blit* functions will overwrite the stored values.
/* ------------------------------------------------------------------------- */
LLFUNC(BlitX, LCGETPTR(1, Texture)->BlitStored(0));
/* ========================================================================= */
// $ Texture:BlitXI
// > TexIndex:integer=The texture id number
// ? Blits with the currently stored texture at position 'TexIndex' with the
// ? stored texcoord (SetTC*), vertex (SetV*) and colour (SetC*).
// ? Note that the Blit* functions will overwrite the stored values.
/* ------------------------------------------------------------------------- */
LLFUNCBEGIN(BlitXI)
  Texture &tCref = *LCGETPTR(1, Texture);
  tCref.BlitStored(LCGETINTLGE(size_t, 2, 0, tCref.GetSubCount(), "TexIndex"));
LLFUNCEND
/* ========================================================================= */
// $ Texture:BlitT
// > TriIndex:integer=Triangle index. Triangle #1 (zero) or triangle #2 (one).
// ? Blits the specified triangle of tile 0 of texture 0 with the current
// ? stored position.
/* ------------------------------------------------------------------------- */
LLFUNC(BlitT, LCGETPTR(1, Texture)->
  BlitT(LCGETINTLGE(size_t, 2, 0, stTrisPerQuad, "TriIndex"), 0, 0));
/* ========================================================================= */
// $ Texture:BlitS
// > TileIndex:integer=The tile index of the texture to blit.
// ? Blits tile 'TileIndex' of texture 0 with the current stored position.
/* ------------------------------------------------------------------------- */
LLFUNCBEGIN(BlitS)
  Texture &tCref = *LCGETPTR(1, Texture);
  tCref.Blit(0, LCGETINTLGE(size_t, 2, 0, tCref.GetTileCount(), "TileIndex"));
LLFUNCEND
/* ========================================================================= */
// $ Texture:BlitTS
// > TriIndex:integer=Triangle index. Triangle #1 (zero) or triangle #2 (one).
// > TileIndex:integer=The tile index of the texture to blit.
// ? Blits the specified triangle 'TId' of the tile 'TileIndex' of texture 0
// ? with the current stored position.
/* ------------------------------------------------------------------------- */
LLFUNCBEGIN(BlitTS)
  Texture &tCref = *LCGETPTR(1, Texture);
  tCref.BlitT(LCGETINTLGE(size_t, 2, 0, stTrisPerQuad,          "TriIndex"), 0,
              LCGETINTLGE(size_t, 3, 0, tCref.GetTileCount(), "TileIndex"));
LLFUNCEND
/* ========================================================================= */
// $ Texture:BlitI
// > TexIndex:integer=The texture id number
// ? Blits tile 0 of texture 'TexIndex' with the current stored position.
/* ------------------------------------------------------------------------- */
LLFUNCBEGIN(BlitI)
  Texture &tCref = *LCGETPTR(1, Texture);
  tCref.Blit(LCGETINTLGE(size_t, 2, 0, tCref.GetSubCount(), "TexIndex"), 0);
LLFUNCEND
/* ========================================================================= */
// $ Texture:BlitTI
// > TriIndex:integer=Triangle index. Triangle #1 (zero) or triangle #2 (one).
// > TexIndex:integer=The texture id number.
// ? Blits triangle 'TId' of tile 0 of texture 'TexIndex' with the current
// ? stored position.
/* ------------------------------------------------------------------------- */
LLFUNCBEGIN(BlitTI)
  Texture &tCref = *LCGETPTR(1, Texture);
  tCref.BlitT(LCGETINTLGE(size_t, 2, 0, stTrisPerQuad,         "TriIndex"),
              LCGETINTLGE(size_t, 3, 0, tCref.GetSubCount(), "TexIndex"), 0);
LLFUNCEND
/* ========================================================================= */
// $ Texture:BlitIS
// > TexIndex:integer=The texture id number.
// > TileIndex:integer=The tile index of the texture to blit.
// ? Blits tile 0 of texture 'TexIndex' with the current stored position.
/* ------------------------------------------------------------------------- */
LLFUNCBEGIN(BlitIS)
  Texture &tCref = *LCGETPTR(1, Texture);
  tCref.Blit(
    LCGETINTLGE(size_t, 2, 0, tCref.GetSubCount(),  "TexIndex"),
    LCGETINTLGE(size_t, 3, 0, tCref.GetTileCount(), "TileIndex"));
LLFUNCEND
/* ========================================================================= */
// $ Texture:BlitTIS
// > TriIndex:integer=Triangle index. Triangle #1 (zero) or triangle #2 (one).
// > TexIndex:integer=The texture id number.
// > TileIndex:integer=The tile index of the texture to blit.
// ? Blits triangle 'TId' of texture id 'TexIndex' with the specified tile id
// ? 'TildIndex' to the current stored position.
/* ------------------------------------------------------------------------- */
LLFUNCBEGIN(BlitTIS)
  Texture &tCref = *LCGETPTR(1, Texture);
  tCref.BlitT(
    LCGETINTLGE(size_t, 2, 0, stTrisPerQuad,          "TriIndex"),
    LCGETINTLGE(size_t, 3, 0, tCref.GetSubCount(),  "TexIndex"),
    LCGETINTLGE(size_t, 4, 0, tCref.GetTileCount(), "TileIndex"));
LLFUNCEND
/* ========================================================================= */
// $ Texture:BlitSLTRBA
// > TileIndex:integer=The tile index of the texture to blit.
// > X:number=The X screen position of the string.
// > Y:number=The Y screen position of the string.
// > Width:number=A value between 0-1 specifing the red component intensity.
// > Height:number=A value between 0-1 specifing the green component intensity.
// > Angle:number=The angle of the blit the texture. (1.0=full rotation).
// ? Blits tile 'TileIndex' of texture 0 at the specified screen co-ordinates
// ? and with the specified dimensions and angle.
/* ------------------------------------------------------------------------- */
LLFUNCBEGIN(BlitSLTRBA)
  Texture &tCref = *LCGETPTR(1, Texture);
  tCref.BlitLTRBA(0,
    LCGETINTLGE(size_t,  2, 0, tCref.GetTileCount(), "TileIndex"),
    LCGETNUM   (GLfloat, 3,                       "Left"),
    LCGETNUM   (GLfloat, 4,                       "Top"),
    LCGETNUM   (GLfloat, 5,                       "Right"),
    LCGETNUM   (GLfloat, 6,                       "Bottom"),
    LCGETNUM   (GLfloat, 7,                       "Rotation"));
LLFUNCEND
/* ========================================================================= */
// $ Texture:BlitILTRB
// > TexIndex:integer=The texture index to blit.
// > X:number=The X screen position of the string.
// > Y:number=The Y screen position of the string.
// > Width:number=A value between 0-1 specifing the red component intensity.
// > Height:number=A value between 0-1 specifing the green component intensity.
// ? Blits tile 0 of texture 'TexIndex' at the specified screen co-ordinates
// ? and with the specified dimensions.
/* ------------------------------------------------------------------------- */
LLFUNCBEGIN(BlitILTRB)
  Texture &tCref = *LCGETPTR(1, Texture);
  tCref.BlitLTRB(
    LCGETINTLGE(size_t,  2, 0, tCref.GetSubCount(), "TexIndex"), 0,
    LCGETNUM   (GLfloat, 3,                      "Left"),
    LCGETNUM   (GLfloat, 4,                      "Top"),
    LCGETNUM   (GLfloat, 5,                      "Right"),
    LCGETNUM   (GLfloat, 6,                      "Bottom"));
LLFUNCEND
/* ========================================================================= */
// $ Texture:BlitILTRBA
// > TexIndex:integer=The texture index to blit.
// > X:number=The X screen position of the string.
// > Y:number=The Y screen position of the string.
// > Width:number=A value between 0-1 specifing the red component intensity.
// > Height:number=A value between 0-1 specifing the green component intensity.
// > Angle:number=The angle to blit the texture at (1.0=full rotation).
// ? Blits tile 0 of texture 'TexIndex' at the specified screen co-ordinates
// ? and with the specified dimensions with the specified angle.
/* ------------------------------------------------------------------------- */
LLFUNCBEGIN(BlitILTRBA)
  Texture &tCref = *LCGETPTR(1, Texture);
  tCref.BlitLTRBA(
    LCGETINTLGE(size_t,  2, 0, tCref.GetSubCount(), "TexIndex"), 0,
    LCGETNUM   (GLfloat, 3,                      "Left"),
    LCGETNUM   (GLfloat, 4,                      "Top"),
    LCGETNUM   (GLfloat, 5,                      "Right"),
    LCGETNUM   (GLfloat, 6,                      "Bottom"),
    LCGETNUM   (GLfloat, 7,                      "Rotation"));
LLFUNCEND
/* ========================================================================= */
// $ Texture:BlitISLTRB
// > TexIndex:integer=The texture index to blit.
// > TileIndex:integer=The tile index to blit.
// > X:number=The X screen position of the string.
// > Y:number=The Y screen position of the string.
// > Width:number=A value between 0-1 specifing the red component intensity.
// > Height:number=A value between 0-1 specifing the green component intensity.
// ? Blits tile 'TileIndex' of texture 'TexIndex' at the specified screen
// ? co-ordinates and with the specified dimensions.
/* ------------------------------------------------------------------------- */
LLFUNCBEGIN(BlitISLTRB)
  Texture &tCref = *LCGETPTR(1, Texture);
  tCref.BlitLTRB(
    LCGETINTLGE(size_t,  2, 0, tCref.GetSubCount(),  "TexIndex"),
    LCGETINTLGE(size_t,  3, 0, tCref.GetTileCount(), "TileIndex"),
    LCGETNUM   (GLfloat, 4,                          "Left"),
    LCGETNUM   (GLfloat, 5,                          "Top"),
    LCGETNUM   (GLfloat, 6,                          "Right"),
    LCGETNUM   (GLfloat, 7,                          "Bottom"));
LLFUNCEND
/* ========================================================================= */
// $ Texture:BlitISLTRBA
// > TexIndex:integer=The texture index to blit.
// > TileIndex:integer=The tile index to blit.
// > X:number=The X screen position of the string.
// > Y:number=The Y screen position of the string.
// > Width:number=A value between 0-1 specifing the red component intensity.
// > Height:number=A value between 0-1 specifing the green component intensity.
// > Angle:number=The angle to blit the texture at (1.0=full rotation).
// ? Blits tile 'TileIndex' of texture 'TexIndex' at the specified screen
// ? co-ordinates and with the specified dimensions and the specified angle.
/* ------------------------------------------------------------------------- */
LLFUNCBEGIN(BlitISLTRBA)
  Texture &tCref = *LCGETPTR(1, Texture);
  tCref.BlitLTRBA(
    LCGETINTLGE(size_t,  2, 0, tCref.GetSubCount(),  "Texindex"),
    LCGETINTLGE(size_t,  3, 0, tCref.GetTileCount(), "Tileindex"),
    LCGETNUM   (GLfloat, 4,                       "Left"),
    LCGETNUM   (GLfloat, 5,                       "Top"),
    LCGETNUM   (GLfloat, 6,                       "Right"),
    LCGETNUM   (GLfloat, 7,                       "Bottom"),
    LCGETNUM   (GLfloat, 8,                       "Rotation"));
LLFUNCEND
/* ========================================================================= */
// $ Texture:BlitLT
// > X:number=The X screen position of the string.
// > Y:number=The Y screen position of the string.
// ? Blits tile 0 of texture 0 at the specified screen co-ordinates with the
// ? internal tile width and height calculated when created.
/* ------------------------------------------------------------------------- */
LLFUNC(BlitLT, LCGETPTR(1, Texture)->BlitLT(0, 0,
  LCGETNUM(GLfloat, 2, "Left"), LCGETNUM(GLfloat, 3, "Top")));
/* ========================================================================= */
// $ Texture:BlitLTA
// > X:number=The X screen position of the string.
// > Y:number=The Y screen position of the string.
// > Angle:number=The angle of the blit in degrees.
// ? Blits tile 0 of texture 0 at the specified screen co-ordinates with the
// ? internal tile width and height calculated when created and with the
// ? specified angle.
/* ------------------------------------------------------------------------- */
LLFUNC(BlitLTA, LCGETPTR(1, Texture)->BlitLTA(0, 0,
  LCGETNUM(GLfloat, 2, "Left"), LCGETNUM(GLfloat, 3, "Top"),
  LCGETNUM(GLfloat, 4, "Rotation")));
/* ========================================================================= */
// $ Texture:BlitSLT
// > TileIndex:integer=The tile index of the texture to blit.
// > X:number=The X screen position of the string.
// > Y:number=The Y screen position of the string.
// ? Blits tile 'TileIndex' of texture 0 at the specified screen co-ordinates
// ? with the internal tile width and height calculated when created.
/* ------------------------------------------------------------------------- */
LLFUNCBEGIN(BlitSLT)
  Texture &tCref = *LCGETPTR(1, Texture);
  tCref.BlitLT(0,
    LCGETINTLGE(size_t,  2, 0, tCref.GetTileCount(), "Tileindex"),
    LCGETNUM   (GLfloat, 3,                       "Left"),
    LCGETNUM   (GLfloat, 4,                       "Top"));
LLFUNCEND
/* ========================================================================= */
// $ Texture:BlitSLTA
// > TileIndex:integer=The tile index of the texture to blit.
// > X:number=The X screen position of the string.
// > Y:number=The Y screen position of the string.
// > Angle:number=The angle of the blit in degrees.
// ? Blits tile 'TileIndex' of texture 0 at the specified screen co-ordinates
// ? with the internal tile width and height calculated when created and with
// ? the specified angle.
/* ------------------------------------------------------------------------- */
LLFUNCBEGIN(BlitSLTA)
  Texture &tCref = *LCGETPTR(1, Texture);
  tCref.BlitLTA(0,
    LCGETINTLGE(size_t,  2, 0, tCref.GetTileCount(), "TileIndex"),
    LCGETNUM   (GLfloat, 3,                       "Left"),
    LCGETNUM   (GLfloat, 4,                       "Top"),
    LCGETNUM   (GLfloat, 5,                       "Rotation"));
LLFUNCEND
/* ========================================================================= */
// $ Texture:BlitILT
// > TexIndex:integer=The texture index to blit.
// > X:number=The X screen position of the string.
// > Y:number=The Y screen position of the string.
// ? Blits tile 'TileIndex' of texture 0 at the specified screen co-ordinates
// ? with the internal tile width and height calculated when created.
/* ------------------------------------------------------------------------- */
LLFUNCBEGIN(BlitILT)
  Texture &tCref = *LCGETPTR(1, Texture);
  tCref.BlitLT(
    LCGETINTLGE(size_t,  2, 0, tCref.GetSubCount(), "Texindex"), 0,
    LCGETNUM   (GLfloat, 3,                      "Left"),
    LCGETNUM   (GLfloat, 4,                      "Top"));
LLFUNCEND
/* ========================================================================= */
// $ Texture:BlitILTA
// > TexIndex:integer=The texture index to blit.
// > X:number=The X screen position of the string.
// > Y:number=The Y screen position of the string.
// > Angle:number=The angle of the blit in degrees.
// ? Blits tile 'TileIndex' of texture 0 at the specified screen co-ordinates
// ? with the internal tile width and height calculated when created and with
// ? the specified angle.
/* ------------------------------------------------------------------------- */
LLFUNCBEGIN(BlitILTA)
  Texture &tCref = *LCGETPTR(1, Texture);
  tCref.BlitLTA(
    LCGETINTLGE(size_t,  2, 0, tCref.GetSubCount(), "TexIndex"), 0,
    LCGETNUM   (GLfloat, 3,                      "Left"),
    LCGETNUM   (GLfloat, 4,                      "Top"),
    LCGETNUM   (GLfloat, 5,                      "Angle"));
LLFUNCEND
/* ========================================================================= */
// $ Texture:BlitISLT
// > TexIndex:integer=The texture index to blit.
// > TileIndex:integer=The tile index of the texture to blit.
// > X:number=The X screen position of the string.
// > Y:number=The Y screen position of the string.
// ? Blits tile 'TileIndex' of texture 'TexIndex' at the specified screen
// ? co-ordinates with the internal tile width and height calculated when
// ? created.
/* ------------------------------------------------------------------------- */
LLFUNCBEGIN(BlitISLT)
  Texture &tCref = *LCGETPTR(1, Texture);
  tCref.BlitLT(
    LCGETINTLGE(size_t,  2, 0, tCref.GetSubCount(),  "TexIndex"),
    LCGETINTLGE(size_t,  3, 0, tCref.GetTileCount(), "TileIndex"),
    LCGETNUM   (GLfloat, 4,                       "Left"),
    LCGETNUM   (GLfloat, 5,                       "Top"));
LLFUNCEND
/* ========================================================================= */
// $ Texture:BlitISLTA
// > TexIndex:integer=The texture index to blit.
// > TileIndex:integer=The tile index of the texture to blit.
// > X:number=The X screen position of the string.
// > Y:number=The Y screen position of the string.
// > Angle:number=The angle of the blit in degrees.
// ? Blits tile 'TileIndex' of texture 'TexIndex' at the specified screen
// ? co-ordinates with the internal tile width and height calculated when
// ? created, and with the specified angle.
/* ------------------------------------------------------------------------- */
LLFUNCBEGIN(BlitISLTA)
  Texture &tCref = *LCGETPTR(1, Texture);
  tCref.BlitLTA(
    LCGETINTLGE(size_t,  2, 0, tCref.GetSubCount(),  "TexIndex"),
    LCGETINTLGE(size_t,  3, 0, tCref.GetTileCount(), "TileIndex"),
    LCGETNUM   (GLfloat, 4,                          "Left"),
    LCGETNUM   (GLfloat, 5,                          "Top"),
    LCGETNUM   (GLfloat, 6,                          "Angle"));
LLFUNCEND
/* ========================================================================= */
// $ Texture:SetCRGB
// > Red:number=The colour intensity of the texture's red component (0-1).
// > Green:number=The colour intensity of the texture's green component (0-1).
// > Blue:number=The colour intensity of the texture's blue omponent (0-1).
// ? Sets the colour intensity of the texture for each component. The change
// ? affects all subsequent calls to all blitting functions.
/* ------------------------------------------------------------------------- */
LLFUNC(SetCRGB, LCGETPTR(1, Texture)->SetQuadRGB(
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
LLFUNC(SetCRGBA, LCGETPTR(1, Texture)->SetQuadRGBA(
  LCGETNUM(GLfloat, 2, "Red"),  LCGETNUM(GLfloat, 3, "Green"),
  LCGETNUM(GLfloat, 4, "Blue"), LCGETNUM(GLfloat, 5, "Alpha")));
/* ========================================================================= */
// $ Texture:SetCRGBAI
// > Colour:integer=The entire colour to set as an integer (0xAARRGGBB)
// ? Sets the colour intensity of the texture for each component using a 32
// ? bit integer.
/* ------------------------------------------------------------------------- */
LLFUNC(SetCRGBAI,
  LCGETPTR(1, Texture)->SetQuadRGBAInt(LCGETINT(uint32_t, 2, "Colour")));
/* ========================================================================= */
// $ Texture:SetCR
// > Red:number=The colour intensity of the texture's red component (0-1).
// ? Sets the colour intensity of the texture for the red component. The change
// ? affects all subsequent calls to all blitting functions.
/* ------------------------------------------------------------------------- */
LLFUNC(SetCR,
  LCGETPTR(1, Texture)->SetQuadRed(LCGETNUM(GLfloat, 2, "Red")));
/* ========================================================================= */
// $ Texture:SetCG
// > Green:number=The colour intensity of the texture's green component (0-1).
// ? Sets the colour intensity of the texture for the green component. The
// ? change affects all subsequent calls to all blitting functions.
/* ------------------------------------------------------------------------- */
LLFUNC(SetCG,
  LCGETPTR(1, Texture)->SetQuadGreen(LCGETNUM(GLfloat, 2, "Green")));
/* ========================================================================= */
// $ Texture:SetCB
// > Blue:number=The colour intensity of the texture's blue component (0-1).
// ? Sets the colour intensity of the texture for the blue component. The
// ? change affects all subsequent calls to all blitting functions.
/* ------------------------------------------------------------------------- */
LLFUNC(SetCB,
  LCGETPTR(1, Texture)->SetQuadBlue(LCGETNUM(GLfloat, 2, "Blue")));
/* ========================================================================= */
// $ Texture:SetCA
// > Alpha:number=The transparency of the texture (0-1).
// ? Sets the colour transparency of the texture. The change affects all
// ? subsequent calls to all blitting functions.
/* ------------------------------------------------------------------------- */
LLFUNC(SetCA,
  LCGETPTR(1, Texture)->SetQuadAlpha(LCGETNUM(GLfloat, 2, "Alpha")));
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
  LCGETNUM(GLfloat, 4, "Top"),     LCGETNUM(GLfloat, 5, "Right"),
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
  tCref.SetTileCount(LCGETINTLG (size_t, 3, 0, UINT_MAX,            "Count"),
                     LCGETINTLGE(size_t, 2, 0, tCref.GetSubCount(), "TexId"));
LLFUNCEND
/* ========================================================================= */
// $ Texture:TileAS
// > Id:integer=The id of the sub-texture to add to
// > Left:integer=The left pixel position on the texture
// > Top:integer=The top pixel position on the texture
// > Right:integer=The right pixel position on the texture
// > Bottom:integer=The bottom pixel position on the texture
// < Id:integer=The id number for the cached texcoords
// ? Caches the specified texture coordinates which you can use as a parameter
// ? to 'tileid' on blitting functions of the specified sub-texture.
/* ------------------------------------------------------------------------- */
LLFUNCBEGIN(TileAS)
  Texture &tCref = *LCGETPTR(1, Texture);
  const size_t stId = LCGETINTLGE(size_t, 2, 0, tCref.GetSubCount(), "TexId");
  tCref.AddTileDOR(stId,
    LCGETNUMLG(GLfloat, 3, 0, tCref.GetFWidth(),  "Left"),
    LCGETNUMLG(GLfloat, 4, 0, tCref.GetFHeight(), "Top"),
    LCGETNUMLG(GLfloat, 5, 0, tCref.GetFWidth(),  "Right"),
    LCGETNUMLG(GLfloat, 6, 0, tCref.GetFHeight(), "Bottom"));
  LCPUSHINT(tCref.GetTileCount(stId)-1);
LLFUNCENDEX(1)
/* ========================================================================= */
// $ Texture:TileASD
// > Id:integer=The id of the sub-texture to add to
// > Left:integer=The left pixel position on the texture
// > Top:integer=The top pixel position on the texture
// > Width:integer=The width of the texture
// > Height:integer=The height of the texture
// < Id:integer=The id number for the cached texcoords
// ? Caches the specified texture coordinates which you can use as a parameter
// ? to 'tileid' on blitting functions of the specified sub-texture.
/* ------------------------------------------------------------------------- */
LLFUNCBEGIN(TileASD)
  Texture &tCref = *LCGETPTR(1, Texture);
  const size_t stId = LCGETINTLGE(size_t, 2, 0, tCref.GetSubCount(), "TexId");
  const GLfloat
    fX = LCGETNUMLG(GLfloat, 3, 0, tCref.GetFWidth(),     "Left"),
    fY = LCGETNUMLG(GLfloat, 4, 0, tCref.GetFHeight(),    "Top"),
    fW = LCGETNUMLG(GLfloat, 5, 0, tCref.GetFWidth()-fX,  "Width"),
    fH = LCGETNUMLG(GLfloat, 6, 0, tCref.GetFHeight()-fY, "Height");
  tCref.AddTileDORWH(stId, fX, fY, fW, fH);
  LCPUSHINT(tCref.GetTileCount(stId)-1);
LLFUNCENDEX(1)
/* ========================================================================= */
// $ Texture:TileA
// > Left:integer=The left pixel position on the texture
// > Top:integer=The top pixel position on the texture
// > Right:integer=The right pixel position on the texture
// > Bottom:integer=The bottom pixel position on the texture
// < Id:integer=The id number for the cached texcoords
// ? Caches the specified texture coordinates which you can use as a parameter
// ? to 'tileid' on blitting functions.
/* ------------------------------------------------------------------------- */
LLFUNCBEGIN(TileA)
  Texture &tCref = *LCGETPTR(1, Texture);
  tCref.AddTileDOR(0,
    LCGETNUMLG(GLfloat, 2, 0, tCref.GetFWidth(),  "Left"),
    LCGETNUMLG(GLfloat, 3, 0, tCref.GetFHeight(), "Top"),
    LCGETNUMLG(GLfloat, 4, 0, tCref.GetFWidth(),  "Right"),
    LCGETNUMLG(GLfloat, 5, 0, tCref.GetFHeight(), "Bottom"));
  LCPUSHINT(tCref.GetTileCount()-1);
LLFUNCENDEX(1)
/* ========================================================================= */
// $ Texture:TileAD
// > Left:integer=The left pixel position on the texture
// > Top:integer=The top pixel position on the texture
// > Width:integer=The width of the texture
// > Height:integer=The height of the texture
// < Id:integer=The id number for the cached texcoords
// ? Caches the specified texture coordinates which you can use as a parameter
// ? to 'tileid' on blitting functions.
/* ------------------------------------------------------------------------- */
LLFUNCBEGIN(TileAD)
  Texture &tCref = *LCGETPTR(1, Texture);
  const GLfloat
    fX = LCGETNUMLG(GLfloat, 2, 0, tCref.GetFWidth(),     "Left"),
    fY = LCGETNUMLG(GLfloat, 3, 0, tCref.GetFHeight(),    "Top"),
    fW = LCGETNUMLG(GLfloat, 4, 0, tCref.GetFWidth()-fX,  "Width"),
    fH = LCGETNUMLG(GLfloat, 5, 0, tCref.GetFHeight()-fY, "Height");
  tCref.AddTileDORWH(0, fX, fY, fW, fH);
  LCPUSHINT(tCref.GetTileCount()-1);
LLFUNCENDEX(1)
/* ========================================================================= */
// $ Texture:TileSS
// > TexId:integer=The id of the sub texture to change
// > TileId:integer=The id of the tile id to change
// > Left:integer=The left pixel position on the texture
// > Top:integer=The top pixel position on the texture
// > Right:integer=The right pixel position on the texture
// > Bottom:integer=The bottom pixel position on the texture
// ? Changes the specified texture coordinates which you can use as a parameter
// ? to 'tileid' on blitting functions.
/* ------------------------------------------------------------------------- */
LLFUNCBEGIN(TileSS)
  Texture &tCref = *LCGETPTR(1, Texture);
  const size_t stTexId =
    LCGETINTLGE(size_t,  2, 0, tCref.GetSubCount(),         "TexId");
  tCref.SetTileDOR(stTexId,
    LCGETINTLGE(size_t,  3, 0, tCref.GetTileCount(stTexId), "TileId"),
    LCGETNUMLG (GLfloat, 4, 0, tCref.GetFWidth(),           "Left"),
    LCGETNUMLG (GLfloat, 5, 0, tCref.GetFHeight(),          "Top"),
    LCGETNUMLG (GLfloat, 6, 0, tCref.GetFWidth(),           "Right"),
    LCGETNUMLG (GLfloat, 7, 0, tCref.GetFHeight(),          "Bottom"));
LLFUNCEND
/* ========================================================================= */
// $ Texture:TileSSD
// > TexId:integer=The id of the sub texture to change
// > TileId:integer=The id of the tile id to change
// > Left:integer=The left pixel position on the texture
// > Top:integer=The top pixel position on the texture
// > Width:integer=The width of the texture
// > Height:integer=The height of the texture
// ? Changes the specified texture coordinates which you can use as a parameter
// ? to 'tileid' on blitting functions.
/* ------------------------------------------------------------------------- */
LLFUNCBEGIN(TileSSD)
  Texture &tCref = *LCGETPTR(1, Texture);
  const size_t stTexId =
    LCGETINTLGE(size_t,  2, 0, tCref.GetSubCount(),         "TexId");
  const size_t stTileId =
    LCGETINTLGE(size_t,  3, 0, tCref.GetTileCount(stTexId), "TileId");
  const GLfloat
    fX = LCGETNUMLG(GLfloat, 4, 0, tCref.GetFWidth(),       "Left"),
    fY = LCGETNUMLG(GLfloat, 5, 0, tCref.GetFHeight(),      "Top"),
    fW = LCGETNUMLG(GLfloat, 6, 0, tCref.GetFWidth()-fX,    "Width"),
    fH = LCGETNUMLG(GLfloat, 7, 0, tCref.GetFHeight()-fY,   "Height");
  tCref.SetTileDORWH(stTexId, stTileId, fX, fY, fW, fH);
LLFUNCEND
/* ========================================================================= */
// $ Texture:TileS
// > Id:integer=The id of the tile to change
// > Left:integer=The left pixel position on the texture
// > Top:integer=The top pixel position on the texture
// > Right:integer=The right pixel position on the texture
// > Bottom:integer=The bottom pixel position on the texture
// ? Changes the specified texture coordinates which you can use as a parameter
// ? to 'tileid' on blitting functions.
/* ------------------------------------------------------------------------- */
LLFUNCBEGIN(TileS)
  Texture &tCref = *LCGETPTR(1, Texture);
  tCref.SetTileDOR(0,
    LCGETINTLGE(size_t,  2, 0, tCref.GetTileCount(0), "Index"),
    LCGETNUMLG (GLfloat, 3, 0, tCref.GetFWidth(),     "Left"),
    LCGETNUMLG (GLfloat, 4, 0, tCref.GetFHeight(),    "Top"),
    LCGETNUMLG (GLfloat, 5, 0, tCref.GetFWidth(),     "Right"),
    LCGETNUMLG (GLfloat, 6, 0, tCref.GetFHeight(),    "Bottom"));
LLFUNCEND
/* ========================================================================= */
// $ Texture:TileSD
// > Id:integer=The id of the tile to change
// > Left:integer=The left pixel position on the texture
// > Top:integer=The top pixel position on the texture
// > Width:integer=The width of the texture
// > Height:integer=The height of the texture
// ? Changes the specified texture coordinates which you can use as a parameter
// ? to 'tileid' on blitting functions.
/* ------------------------------------------------------------------------- */
LLFUNCBEGIN(TileSD)
  Texture &tCref = *LCGETPTR(1, Texture);
  const size_t stId =
    LCGETINTLGE(size_t, 2, 0, tCref.GetTileCount(0), "Index");
  const GLfloat
    fX = LCGETNUMLG(GLfloat, 3, 0, tCref.GetFWidth(),     "Left"),
    fY = LCGETNUMLG(GLfloat, 4, 0, tCref.GetFHeight(),    "Top"),
    fW = LCGETNUMLG(GLfloat, 5, 0, tCref.GetFWidth()-fX,  "Width"),
    fH = LCGETNUMLG(GLfloat, 6, 0, tCref.GetFHeight()-fY, "Height");
  tCref.SetTileDORWH(0, stId, fX, fY, fW, fH);
LLFUNCEND
/* ========================================================================= */
// $ Texture:TileGTC
// < Count:integer=The handle of the texture created.
// ? Returns number of tiles found in the loaded texture file.
/* ------------------------------------------------------------------------- */
LLFUNCEX(TileGTC, 1, LCPUSHINT(LCGETPTR(1, Texture)->GetTileCount()));
/* ========================================================================= */
// $ Texture:TileGSTC
// > Id:integer=The id of the sub-texture to get the count for.
// < Count:integer=The handle of the texture created.
// ? Returns number of tiles found in the loaded texture file with the
// ? specified sub-texture id.
/* ------------------------------------------------------------------------- */
LLFUNCBEGIN(TileGSTC)
  const Texture &tCref = *LCGETPTR(1, Texture);
  LCPUSHINT(tCref.GetTileCount(
    LCGETINTLGE(size_t, 2, 0, tCref.GetSubCount(), "TexId")));
LLFUNCENDEX(1)
/* ========================================================================= */
// $ Texture:GetSubCount
// < Count:integer=The number of sub-textures in this texture
// ? Returns number of sub-textures found in the loaded image file.
/* ------------------------------------------------------------------------- */
LLFUNCEX(GetSubCount, 1, LCPUSHINT(LCGETPTR(1, Texture)->GetSubCount()));
/* ========================================================================= */
// $ Texture:GetHeight
// < Height:integer=The handle of the new mask created.
// ? Returns height of texture.
/* ------------------------------------------------------------------------- */
LLFUNCEX(GetHeight, 1, LCPUSHINT(LCGETPTR(1, Texture)->DimGetHeight()));
/* ========================================================================= */
// $ Texture:GetName
// < Name:string=Name of the texture.
// ? If this texture was loaded by a filename or it was set with a custom id.
// ? This function returns that name which was assigned to it.
/* ------------------------------------------------------------------------- */
LLFUNCEX(GetName, 1, LCPUSHXSTR(LCGETPTR(1, Texture)->IdentGet()));
/* ========================================================================= */
// $ Texture:GetWidth
// < Width:integer=The handle of the new mask created.
// ? Returns width of texture.
/* ------------------------------------------------------------------------- */
LLFUNCEX(GetWidth, 1, LCPUSHINT(LCGETPTR(1, Texture)->DimGetWidth()));
/* ========================================================================= */
// $ Texture:Dump
// ? Dumps the texture to the specified file in the specified format.
/* ------------------------------------------------------------------------- */
LLFUNCBEGIN(Dump)
  const Texture &tCref = *LCGETPTR(1, Texture);
  tCref.Dump(LCGETINTLGE (size_t, 2, 0, tCref.GetSubCount(), "TexId"),
             LCGETCPPFILE(        3,                         "File"));
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
  Image imNew(tCref.Download(LCGETINTLGE(size_t,
    2, 0, tCref.GetSubCount(), "TexId")));
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
  tCref.UpdateEx
    (LCGETINTLGE(size_t, 2, 0, tCref.GetSubCount(), "TexId"),
    *LCGETPTR   (        3, Image),
     LCGETINT   (GLint,  4,                         "OffsetX"),
     LCGETINT   (GLint,  5,                         "OffsetY"));
LLFUNCEND
/* ========================================================================= */
// $ Texture:SetV
// > Left:number=The left co-ordinate.
// > Top:number=The top co-ordinate.
// > Right:number=The right co-ordinate.
// > Bottom:number=The bottom co-ordinate.
// ? Allows you to set basic vertex co-ordinates when blitting the fbo. For
// ? a more advanced version of this function, see Fbo:SetVX().
/* ------------------------------------------------------------------------- */
LLFUNC(SetV, LCGETPTR(1, Texture)->SetVertex(
  LCGETNUM(GLfloat, 2, "Left"),  LCGETNUM(GLfloat, 3, "Top"),
  LCGETNUM(GLfloat, 4, "Right"), LCGETNUM(GLfloat, 5, "Bottom")));
/* ========================================================================= */
// $ Texture:SetVX
// > TriIndex:integer=Triangle index. Triangle #1 (zero) or triangle #2 (one).
// > C1V1:number=GLfloat for coord #1 of vertex #1 of the specified triangle.
// > C2V2:number=GLfloat for coord #2 of vertex #2 of the specified triangle.
// > C1V3:number=GLfloat for coord #1 of vertex #3 of the specified triangle.
// > C2V1:number=GLfloat for coord #2 of vertex #1 of the specified triangle.
// > C1V2:number=GLfloat for coord #1 of vertex #2 of the specified triangle.
// > C2V3:number=GLfloat for coord #2 of vertex #3 of the specified triangle.
// ? Allows you to full control over the vertex co-ordinates apart from the Z
// ? co-ordinate which is not used in this 2D only engine.
/* ------------------------------------------------------------------------- */
LLFUNC(SetVX, LCGETPTR(1, Texture)->
  SetVertexEx(LCGETINTLGE(size_t, 2, 0, stTrisPerQuad, "TriIndex"), {
    LCGETNUM(GLfloat, 3, "C1V1"), LCGETNUM(GLfloat, 4, "C2V1"),
    LCGETNUM(GLfloat, 5, "C1V2"), LCGETNUM(GLfloat, 6, "C2V2"),
    LCGETNUM(GLfloat, 7, "C1V3"), LCGETNUM(GLfloat, 8, "C2V3") }));
/* ========================================================================= */
// $ Texture:SetVTC
// > TriIndex:integer=Triangle index. Triangle #1 (zero) or triangle #2 (one).
// > C1V1:number=GLfloat for coord #1 of vertex #1 of the specified triangle.
// > C2V2:number=GLfloat for coord #2 of vertex #2 of the specified triangle.
// > C1V3:number=GLfloat for coord #1 of vertex #3 of the specified triangle.
// > C2V1:number=GLfloat for coord #2 of vertex #1 of the specified triangle.
// > C1V2:number=GLfloat for coord #1 of vertex #2 of the specified triangle.
// > C2V3:number=GLfloat for coord #2 of vertex #3 of the specified triangle.
// ? Allows you to full control over the texture co-ordinates.
/* ------------------------------------------------------------------------- */
LLFUNC(SetVTC, LCGETPTR(1, Texture)->
  SetTexCoordEx(LCGETINTLGE(size_t, 2, 0, stTrisPerQuad, "TriIndex"), {
    LCGETNUM(GLfloat, 3, "C1V1"), LCGETNUM(GLfloat, 4, "C2V1"),
    LCGETNUM(GLfloat, 5, "C1V2"), LCGETNUM(GLfloat, 6, "C2V2"),
    LCGETNUM(GLfloat, 7, "C1V3"), LCGETNUM(GLfloat, 8, "C2V3") }));
/* ========================================================================= */
// $ Texture:SetVCRGBA
// > TriIndex:integer=Triangle index. Triangle #1 (zero) or triangle #2 (one).
// > RV1:number=Red component of vertex #1 of the specified triangle.
// > GV1:number=Green component of vertex #1 of the specified triangle.
// > BV1:number=Blue component of vertex #1 of the specified triangle.
// > AV1:number=Alpha component of vertex #1 of the specified triangle.
// > RV2:number=Red component of vertex #2 of the specified triangle.
// > GV2:number=Green component of vertex #2 of the specified triangle.
// > BV2:number=Blue component of vertex #2 of the specified triangle.
// > AV2:number=Alpha component of vertex #2 of the specified triangle.
// > RV3:number=Red component of vertex #3 of the specified triangle.
// > GV3:number=Green component of vertex #3 of the specified triangle.
// > BV3:number=Blue component of vertex #3 of the specified triangle.
// > AV3:number=Alpha component of vertex #3 of the specified triangle.
// ? Allows you to full control over the colour of the texture.
/* ------------------------------------------------------------------------- */
LLFUNC(SetVCRGBA, LCGETPTR(1, Texture)->
  SetColourEx(LCGETINTLGE(size_t, 2, 0, stTrisPerQuad, "TriIndex"), {
    LCGETNUM(GLfloat,  3, "RV1"), LCGETNUM(GLfloat,  4, "GV1"),
    LCGETNUM(GLfloat,  5, "BV1"), LCGETNUM(GLfloat,  6, "AV1"),
    LCGETNUM(GLfloat,  7, "RV2"), LCGETNUM(GLfloat,  8, "GV2"),
    LCGETNUM(GLfloat,  9, "BV2"), LCGETNUM(GLfloat, 10, "AV2"),
    LCGETNUM(GLfloat, 11, "RV3"), LCGETNUM(GLfloat, 12, "GV3"),
    LCGETNUM(GLfloat, 13, "BV3"), LCGETNUM(GLfloat, 14, "AV3") }));
/* ========================================================================= */
// $ Texture:SetTC
// > Left:number=The left co-ordinate.
// > Top:number=The top co-ordinate.
// > Right:number=The right co-ordinate.
// > Bottom:number=The bottom co-ordinate.
// ? Allows you to set basic texture co-ordinates when blitting the fbo. For
// ? a more advanced version of this function, see Fbo:SetTexCoordEx().
/* ------------------------------------------------------------------------- */
LLFUNC(SetTC, LCGETPTR(1, Texture)->SetTexCoord(
  LCGETNUM(GLfloat, 2, "Left"),  LCGETNUM(GLfloat, 3, "Top"),
  LCGETNUM(GLfloat, 4, "Right"), LCGETNUM(GLfloat, 5, "Bottom")));
/* ========================================================================= */
// $ Texture:SetVA
// > Left:number=The left co-ordinate.
// > Top:number=The top co-ordinate.
// > Right:number=The right co-ordinate.
// > Bottom:number=The bottom co-ordinate.
// > Angle:number=The angle of the vertex
// ? Allows you to set basic vertex co-ordinates when blitting the fbo with
// ? angle calculations.
/* ------------------------------------------------------------------------- */
LLFUNC(SetVA, LCGETPTR(1, Texture)->SetVertex(
  LCGETNUM(GLfloat, 2, "Left"),  LCGETNUM(GLfloat, 3, "Top"),
  LCGETNUM(GLfloat, 4, "Right"), LCGETNUM(GLfloat, 5, "Bottom"),
  LCGETNUM(GLfloat, 6, "Angle")));
/* ========================================================================= */
// $ Texture:PushColour
// ? Saves the currently set colour by SetCRGBA. Use PopColour to restore it.
/* ------------------------------------------------------------------------- */
LLFUNC(PushColour, LCGETPTR(1, Texture)->PushQuadColour());
/* ========================================================================= */
// $ Texture:PopColour
// ? Restores the currently saved colour by PushColour.
/* ------------------------------------------------------------------------- */
LLFUNC(PopColour, LCGETPTR(1, Texture)->PopQuadColour());
/* ========================================================================= */
// $ Texture:Destroy
// ? Destroys the texture and frees all the memory associated with it. The
// ? OpenGL handles and VRAM allocated by it are freed after the main FBO has
// ? has been rendered. The object will no longer be useable after this call
// ? and an error will be generated if accessed.
/* ------------------------------------------------------------------------- */
LLFUNC(Destroy, LCCLASSDESTROY(1, Texture));
/* ========================================================================= */
/* ######################################################################### */
/* ## Texture:* member functions structure                                ## */
/* ######################################################################### */
/* ------------------------------------------------------------------------- */
LLRSMFBEGIN                            // Texture:* member functions begin
  LLRSFUNC(Blit),       LLRSFUNC(BlitI),       LLRSFUNC(BlitILT),
  LLRSFUNC(BlitILTA),   LLRSFUNC(BlitILTRB),   LLRSFUNC(BlitILTRBA),
  LLRSFUNC(BlitIS),     LLRSFUNC(BlitISLT),    LLRSFUNC(BlitISLTA),
  LLRSFUNC(BlitISLTRB), LLRSFUNC(BlitISLTRBA), LLRSFUNC(BlitLT),
  LLRSFUNC(BlitLTA),    LLRSFUNC(BlitLTRB),    LLRSFUNC(BlitLTRBA),
  LLRSFUNC(BlitM),      LLRSFUNC(BlitS),       LLRSFUNC(BlitSLT),
  LLRSFUNC(BlitSLTA),   LLRSFUNC(BlitSLTRB),   LLRSFUNC(BlitSLTRBA),
  LLRSFUNC(BlitT),      LLRSFUNC(BlitTI),      LLRSFUNC(BlitTIS),
  LLRSFUNC(BlitTS),     LLRSFUNC(BlitX),       LLRSFUNC(BlitXI),
  LLRSFUNC(Destroy),    LLRSFUNC(Download),    LLRSFUNC(Dump),
  LLRSFUNC(GetHeight),  LLRSFUNC(GetName),     LLRSFUNC(GetSubCount),
  LLRSFUNC(GetWidth),   LLRSFUNC(PopColour),   LLRSFUNC(PushColour),
  LLRSFUNC(SetCA),      LLRSFUNC(SetCB),       LLRSFUNC(SetCG),
  LLRSFUNC(SetCR),      LLRSFUNC(SetCRGB),     LLRSFUNC(SetCRGBA),
  LLRSFUNC(SetCRGBAI),  LLRSFUNC(SetLTRB),     LLRSFUNC(SetLTRBA),
  LLRSFUNC(SetTC),      LLRSFUNC(SetV),        LLRSFUNC(SetVA),
  LLRSFUNC(SetVCRGBA),  LLRSFUNC(SetVTC),      LLRSFUNC(SetVX),
  LLRSFUNC(TileA),      LLRSFUNC(TileAD),      LLRSFUNC(TileAS),
  LLRSFUNC(TileASD),    LLRSFUNC(TileGSTC),    LLRSFUNC(TileGTC),
  LLRSFUNC(TileS),      LLRSFUNC(TileSD),      LLRSFUNC(TileSS),
  LLRSFUNC(TileSSD),    LLRSFUNC(TileSSTC),    LLRSFUNC(TileSTC),
  LLRSFUNC(Upload),     LLRSFUNC(UploadEx),
LLRSEND                                // Texture:* member functions end
/* ========================================================================= */
// $ Texture.Create
// > Source:image=The image class to load from
// > Filter:integer=The default filter to use
// < Handle:Texture=A handle to the texture(s) created.
// ? Creates the specified texture from the specified image class.
/* ========================================================================= */
LLFUNCEX(Create, 1, LCCLASSCREATE(Texture)->InitImage(*LCGETPTR(1, Image),
  0, 0, 0, 0, LCGETINTLGE(size_t, 2, 0, OF_MAX, "Filter")));
/* ========================================================================= */
// $ Texture.CreateTS
// > Source:image=The image class to load from
// > TWidth:integer=The width of each tile in the image.
// > THeight:integer=The height of each tile in the image.
// > PWidth:integer=The width padding between each tile in the image.
// > PHeight:integer=The height padding between each tile in the image.
// > Filter:integer=The filtering setting to use on texture
// < Handle:Texture=A handle to the texture(s) created.
// ? Loads a texture as a tileset from the specified image class.
/* ------------------------------------------------------------------------- */
LLFUNCEX(CreateTS, 1, LCCLASSCREATE(Texture)->InitImage(
 *LCGETPTR(1, Image),
  LCGETINTLG (GLuint, 2, 0, UINT_MAX, "TWidth"),
  LCGETINTLG (GLuint, 3, 0, UINT_MAX, "THeight"),
  LCGETINTLG (GLuint, 4, 0, UINT_MAX, "PWidth"),
  LCGETINTLG (GLuint, 5, 0, UINT_MAX, "PHeight"),
  LCGETINTLGE(size_t, 6, 0,   OF_MAX, "Filter")));
/* ========================================================================= */
// $ Texture.Console
// < Handle:Texture=Texture handle to console texture
// ? Returns the handle to the console texture. Useful if you want to reuse the
// ? texture in your application. Careful not to mess around with it's
// ? properties!
/* ------------------------------------------------------------------------- */
LLFUNCEX(Console, 1, LCCLASSCREATEPTR(Texture, cConsole->GetTexture()));
/* ========================================================================= */
/* ######################################################################### */
/* ## Texture.* namespace functions structure                             ## */
/* ######################################################################### */
/* ------------------------------------------------------------------------- */
LLRSBEGIN                              // Texture.* namespace functions begin
  LLRSFUNC(Create), LLRSFUNC(CreateTS), LLRSFUNC(Console),
LLRSEND                                // Texture.* namespace functions end
/* ========================================================================= */
}                                      // End of Texture namespace
/* == EoF =========================================================== EoF == */

/* == LLFONT.HPP =========================================================== */
/* ######################################################################### */
/* ## MS-ENGINE              Copyright (c) MS-Design, All Rights Reserved ## */
/* ######################################################################### */
/* ## Defines the 'Font' namespace and methods for the guest to use in    ## */
/* ## Lua. This file is invoked by 'lualib.hpp'.                          ## */
/* ######################################################################### */
/* ------------------------------------------------------------------------- */
#pragma once                           // Only one incursion allowed
/* ========================================================================= */
/* ######################################################################### */
/* ========================================================================= */
// % Font
/* ------------------------------------------------------------------------- */
// ! The font class allows dynamic creation from font files using the FREETYPE
// ! library.
/* ========================================================================= */
LLNAMESPACEBEGIN(Font)                 // Font namespace
/* -- Includes ------------------------------------------------------------- */
using namespace IfFont;                // Using font namespace
/* ========================================================================= */
/* ######################################################################### */
/* ========================================================================= */
// $ Font:SetSize
// > Scale:number=The new scale of the font.
// ? Changes the scale of the font.
/* ------------------------------------------------------------------------- */
LLFUNC(SetSize, LCGETPTR(1, Font)->
  SetSize(LCGETNUMLG(GLfloat, 2, 0, 4096, "Scale")));
/* ========================================================================= */
// $ Font:SetGSize
// > Scale:number=The new scale of the font glyphs.
// ? Changes the scale of the font glyphs.
/* ------------------------------------------------------------------------- */
LLFUNC(SetGSize, LCGETPTR(1, Font)->
  SetGlyphSize(LCGETNUMLG(GLfloat, 2, 0, 4096, "Scale")));
/* ========================================================================= */
// $ Font:SetLSpacing
// > Adjust:number=The new line spacing adjustment
// ? Changes the line spacing adjustment
/* ------------------------------------------------------------------------- */
LLFUNC(SetLSpacing, LCGETPTR(1, Font)->
  SetLineSpacing(LCGETNUM(GLfloat, 2, "Spacing")));
/* ========================================================================= */
// $ Font:SetSpacing
// > Adjust:number=The new character spacing adjustment
// ? Changes the character spacing adjustment
/* ------------------------------------------------------------------------- */
LLFUNC(SetSpacing, LCGETPTR(1, Font)->
  SetCharSpacing(LCGETNUM(GLfloat, 2, "Spacing")));
/* ========================================================================= */
// $ Font:SetCRGB
// > Red:number=The colour intensity of the texture's red component (0-1).
// > Green:number=The colour intensity of the texture's green component (0-1).
// > Blue:number=The colour intensity of the texture's blue omponent (0-1).
// ? Sets the colour intensity of the texture for each component. The change
// ? affects all subsequent calls to all blitting functions.
/* ------------------------------------------------------------------------- */
LLFUNC(SetCRGB, LCGETPTR(1, Font)->SetQuadRGB(LCGETNUM(GLfloat, 2, "Red"),
  LCGETNUM(GLfloat, 3, "Green"), LCGETNUM(GLfloat, 4, "Blue")));
/* ========================================================================= */
// $ Font:SetCRGBA
// > Red:number=The colour intensity of the texture's red component (0-1).
// > Green:number=The colour intensity of the texture's green component (0-1).
// > Blue:number=The colour intensity of the texture's blue omponent (0-1).
// > Alpha:number=The transparency of the texture (0-1).
// ? Sets the colour intensity of the texture for each component. The change
// ? affects all subsequent calls to all blitting functions.
/* ------------------------------------------------------------------------- */
LLFUNC(SetCRGBA, LCGETPTR(1, Font)->SetQuadRGBA(
  LCGETNUM(GLfloat, 2, "Red"), LCGETNUM(GLfloat, 3, "Green"),
  LCGETNUM(GLfloat, 4, "Blue"), LCGETNUM(GLfloat, 5, "Alpha")));
/* ========================================================================= */
// $ Font:SetCRGBAI
// > Colour:integer=The entire colour to set as an integer (0xAARRGGBB)
// ? Sets the colour intensity of the font face for each component using a 32
// ? bit integer.
/* ------------------------------------------------------------------------- */
LLFUNC(SetCRGBAI, LCGETPTR(1, Font)->
  SetQuadRGBAInt(LCGETINT(uint32_t, 2, "Colour")));
/* ========================================================================= */
// $ Font:SetCR
// > Red:number=The colour intensity of the texture's red component (0-1).
// ? Sets the colour intensity of the texture for the red component. The change
// ? affects all subsequent calls to all blitting functions.
/* ------------------------------------------------------------------------- */
LLFUNC(SetCR, LCGETPTR(1, Font)->SetQuadRed(LCGETNUM(GLfloat, 2, "Red")));
/* ========================================================================= */
// $ Font:SetCG
// > Green:number=The colour intensity of the texture's green component (0-1).
// ? Sets the colour intensity of the texture for the green component. The
// ? change affects all subsequent calls to all blitting functions.
/* ------------------------------------------------------------------------- */
LLFUNC(SetCG, LCGETPTR(1, Font)->SetQuadGreen(LCGETNUM(GLfloat, 2, "Green")));
/* ========================================================================= */
// $ Font:SetCB
// > Blue:number=The colour intensity of the texture's blue component (0-1).
// ? Sets the colour intensity of the texture for the blue component. The
// ? change affects all subsequent calls to all blitting functions.
/* ------------------------------------------------------------------------- */
LLFUNC(SetCB, LCGETPTR(1, Font)->SetQuadBlue(LCGETNUM(GLfloat, 2, "Blue")));
/* ========================================================================= */
// $ Font:SetCA
// > Alpha:number=The transparency of the texture (0-1).
// ? Sets the colour transparency of the texture. The change affects all
// ? subsequent calls to all blitting functions.
/* ------------------------------------------------------------------------- */
LLFUNC(SetCA, LCGETPTR(1, Font)->SetQuadAlpha(LCGETNUM(GLfloat, 2, "Alpha")));
/* ========================================================================= */
// $ Font:SetCORGB
// > Red:number=The colour intensity of the outline red component (0-1).
// > Green:number=The colour intensity of the outline green component (0-1).
// > Blue:number=The colour intensity of the outline blue omponent (0-1).
// ? Sets the colour intensity of the outline for each component. The change
// ? affects all subsequent calls to all blitting functions.
/* ------------------------------------------------------------------------- */
LLFUNC(SetCORGB, LCGETPTR(1, Font)->fiOutline.SetQuadRGB(
  LCGETNUM(GLfloat, 2, "Red"), LCGETNUM(GLfloat, 3, "Green"),
  LCGETNUM(GLfloat, 4, "Blue")));
/* ========================================================================= */
// $ Font:SetCORGBA
// > Red:number=The colour intensity of the outline red component (0-1).
// > Green:number=The colour intensity of the outline green component (0-1).
// > Blue:number=The colour intensity of the outline blue omponent (0-1).
// > Alpha:number=The transparency of the outline (0-1).
// ? Sets the colour intensity of the outline for each component. The change
// ? affects all subsequent calls to all blitting functions.
/* ------------------------------------------------------------------------- */
LLFUNC(SetCORGBA, LCGETPTR(1, Font)->fiOutline.SetQuadRGBA(
  LCGETNUM(GLfloat, 2, "Red"),  LCGETNUM(GLfloat, 3, "Green"),
  LCGETNUM(GLfloat, 4, "Blue"), LCGETNUM(GLfloat, 5, "Alpha")));
/* ========================================================================= */
// $ Font:SetCORGBAI
// > Colour:integer=The entire colour to set as an integer (0xAARRGGBB)
// ? Sets the colour intensity of the font outline for each component using a
// ? 32 bit integer.
/* ------------------------------------------------------------------------- */
LLFUNC(SetCORGBAI, LCGETPTR(1, Font)->
  fiOutline.SetQuadRGBAInt(LCGETINT(uint32_t, 2, "Colour")));
/* ========================================================================= */
// $ Font:SetCOR
// > Red:number=The colour intensity of red outline component (0-1).
// ? Sets the colour intensity of the outline for the red component. The change
// ? affects all subsequent calls to all blitting functions.
/* ------------------------------------------------------------------------- */
LLFUNC(SetCOR,
  LCGETPTR(1, Font)->fiOutline.SetQuadRed(LCGETNUM(GLfloat, 2, "Red")));
/* ========================================================================= */
// $ Font:SetCOG
// > Green:number=The colour intensity of green outline component (0-1).
// ? Sets the colour intensity of the outline for the green component. The
// ? change affects all subsequent calls to all blitting functions.
/* ------------------------------------------------------------------------- */
LLFUNC(SetCOG,
  LCGETPTR(1, Font)->fiOutline.SetQuadGreen(LCGETNUM(GLfloat, 2, "Green")));
/* ========================================================================= */
// $ Font:SetCOB
// > Blue:number=The colour intensity of the blue outline component (0-1).
// ? Sets the colour intensity of the outline for the blue component. The
// ? change affects all subsequent calls to all blitting functions.
/* ------------------------------------------------------------------------- */
LLFUNC(SetCOB,
  LCGETPTR(1, Font)->fiOutline.SetQuadBlue(LCGETNUM(GLfloat, 2, "Blue")));
/* ========================================================================= */
// $ Font:SetCOA
// > Alpha:number=The transparency of the outline texture (0-1).
// ? Sets the colour transparency of the outline texture. The change affects
// ? all subsequent calls to all blitting functions.
/* ------------------------------------------------------------------------- */
LLFUNC(SetCOA,
  LCGETPTR(1, Font)->fiOutline.SetQuadAlpha(LCGETNUM(GLfloat, 2, "Alpha")));
/* ========================================================================= */
// $ Font:Print
// > X:number=The X screen position of the string.
// > Y:number=The Y screen position of the string.
// > String:string=The string to print.
// ? Prints the specified string on screen with left alignment.
/* ------------------------------------------------------------------------- */
LLFUNC(Print, LCGETPTR(1, Font)->Print(LCGETNUM(GLfloat, 2, "Left"),
  LCGETNUM(GLfloat, 3, "Top"), LCGETSTRING(GLubyte, 4, "String")));
/* ========================================================================= */
// $ Font:PrintT
// > X:number=The X position of where to start printing the string
// > Y:number=The Y position of the string.
// > Text:string=The string to print.
// > Glyphs:Texture=The texture handle to use for printing glyphs.
// ? Same functionality as Print but with the option to print glyphs. You
// ? can do this by specifying \rthhhhhhhh anywhere in your text. Where 'h'
// ? is a hexadecimal number between 00000000 and ffffffff to match the tile
// ? id of the texture. You must zero pad the number as shown.
/* ------------------------------------------------------------------------- */
LLFUNC(PrintT, LCGETPTR(1, Font)->PrintT(
  LCGETNUM   (GLfloat, 2, "X"),    LCGETNUM(GLfloat, 3, "Y"),
  LCGETSTRING(GLubyte, 4, "String"), LCGETPTR(5, Texture)));
/* ========================================================================= */
// $ Font:PrintRT
// > X:number=The X position of where to start printing the string
// > Y:number=The Y position of the string.
// > Text:string=The string to print.
// > Glyphs:Texture=The texture handle to use for printing glyphs.
// ? Same functionality as PrintR but with the option to print glyphs. You
// ? can do this by specifying \rthhhhhhhh anywhere in your text. Where 'h'
// ? is a hexadecimal number between 00000000 and ffffffff to match the tile
// ? id of the texture. You must zero pad the number as shown.
/* ------------------------------------------------------------------------- */
LLFUNC(PrintRT, LCGETPTR(1, Font)->PrintRT(
  LCGETNUM   (GLfloat, 2, "X"),    LCGETNUM(GLfloat, 3, "Y"),
  LCGETSTRING(GLubyte, 4, "String"), LCGETPTR(5, Texture)));
/* ========================================================================= */
// $ Font:PrintM
// > X:number=The X position of where to draw the string.
// > Y:number=The Y position of where to draw the string.
// > Scroll:number=The amount to scroll leftwards by in pixels.
// > Width:number=The width of the string to print.
// > Text:string=The string to print
// ? Prints part of a string which helps one create a marquee effect.
/* ------------------------------------------------------------------------- */
LLFUNC(PrintM, LCGETPTR(1, Font)->PrintM(
  LCGETNUM   (GLfloat, 2, "X"),      LCGETNUM(GLfloat, 3, "Y"),
  LCGETNUM   (GLfloat, 4, "Scroll"), LCGETNUM(GLfloat, 5, "Width"),
  LCGETSTRING(GLubyte, 6, "String")));
/* ========================================================================= */
// $ Font:PrintMT
// > X:number=The X position of where to draw the string.
// > Y:number=The Y position of where to draw the string.
// > Scroll:number=The amount to scroll leftwards by in pixels.
// > Width:number=The width of the string to print.
// > Text:string=The string to print
// > Glphs:Texture=The texture to use to printing glyph
// ? Prints part of a string which helps one create a marquee effect with
// ? glyphs support.
/* ------------------------------------------------------------------------- */
LLFUNC(PrintMT, LCGETPTR(1, Font)->PrintMT(
  LCGETNUM   (GLfloat, 2, "X"),      LCGETNUM(GLfloat, 3, "Y"),
  LCGETNUM   (GLfloat, 4, "Scroll"), LCGETNUM(GLfloat, 5, "Width"),
  LCGETSTRING(GLubyte, 6, "String"),   LCGETPTR(7, Texture)));
/* ========================================================================= */
// $ Font:PrintS
// > Text:string=The string to simulate printing.
// < Width:Number=The width of the text in pixels.
// ? Simluates printing of the specified string on screen with left alignment
// ? and returns the width of the string printed
/* ------------------------------------------------------------------------- */
LLFUNCEX(PrintS, 1,
  LCPUSHNUM(LCGETPTR(1, Font)->PrintS(LCGETSTRING(GLubyte, 2, "String"))));
/* ========================================================================= */
// $ Font:PrintTS
// > Text:string=The string to simulate printing.
// > Glyphs:Texture=The glyph textures to use
// < Width:Number=The width of the text in pixels.
// ? Simluates printing of the specified string on screen with left alignment
// ? and returns the width of the string printed
/* ------------------------------------------------------------------------- */
LLFUNCEX(PrintTS, 1, LCPUSHNUM(LCGETPTR(1, Font)->PrintTS(
  LCGETSTRING(GLubyte, 2, "String"), LCGETPTR(3, Texture))));
/* ========================================================================= */
// $ Font:PrintW
// > X:number=The X position of where to start printing the string
// > Y:number=The Y screen position of the string.
// > Right:number=The right position of where to end printing the string
// > Indent:number=The width of the indentation on wrapping.
// > Text:string=The string to print.
// < Height:number=The height of the text printed on screen.
// ? Prints the specified string on screen with left alignment. While printing,
// ? the text will cleanly wrap if it cannot draw any word past the specified
// ? 'Right' position and position the cursor at the specified 'Indent'.
/* ------------------------------------------------------------------------- */
LLFUNCEX(PrintW, 1, LCPUSHNUM(LCGETPTR(1, Font)->PrintW(
  LCGETNUM(GLfloat, 2, "X"),      LCGETNUM(GLfloat, 3, "Y"),
  LCGETNUMLGE(GLfloat, 4, 0, FLT_MAX, "Width"),
  LCGETNUM(GLfloat, 5, "Indent"), LCGETSTRING(GLubyte, 6,"String"))));
/* ========================================================================= */
// $ Font:PrintWUT
// > X:number=The X position of where to start printing the string
// > Y:number=The Y position of the string.
// > Right:number=The right position of where to wrap text to the next line.
// > Indent:number=The width of the indentation on wrapping.
// > Text:string=The string to print.
// > Glyphs:texture=The texture handle to use for printing glyphs.
// ? Same functionality as PrintWU but with the option to print glyphs. You
// ? can do this by specifying \rthhhhhhhh anywhere in your text. Where 'h'
// ? is a hexadecimal number between 00000000 and ffffffff to match the tile
// ? id of the texture. You must zero pad the number as shown.
/* ------------------------------------------------------------------------- */
LLFUNCEX(PrintWUT, 1, LCPUSHNUM(LCGETPTR(1, Font)->PrintWUT(
  LCGETNUM   (GLfloat, 2, "X"),                 LCGETNUM(GLfloat, 3, "Y"),
  LCGETNUMLGE(GLfloat, 4, 0, FLT_MAX, "Width"), LCGETNUM(GLfloat, 5, "Indent"),
  LCGETSTRING(GLubyte, 6, "String"),              LCGETPTR(7, Texture))));
/* ========================================================================= */
// $ Font:PrintWT
// > X:number=The X position of where to start printing the string
// > Y:number=The Y position of the string.
// > Right:number=The right position of where to wrap text to the next line.
// > Indent:number=The width of the indentation on wrapping.
// > Text:string=The string to print.
// > Glyphs:texture=The texture handle to use for printing glyphs.
// ? Same functionality as PrintW but with the option to print glyphs. You
// ? can do this by specifying \rthhhhhhhh anywhere in your text. Where 'h'
// ? is a hexadecimal number between 00000000 and ffffffff to match the tile
// ? id of the texture. You must zero pad the number as shown.
/* ------------------------------------------------------------------------- */
LLFUNCEX(PrintWT, 1, LCPUSHNUM(LCGETPTR(1, Font)->PrintWT(
  LCGETNUM   (GLfloat, 2, "X"),                 LCGETNUM(GLfloat, 3, "Y"),
  LCGETNUMLGE(GLfloat, 4, 0, FLT_MAX, "Width"), LCGETNUM(GLfloat, 5, "Indent"),
  LCGETSTRING(GLubyte, 6, "String"),              LCGETPTR(7, Texture))));
/* ========================================================================= */
// $ Font:PrintWTS
// > Right:number=The right position of where to wrap text to the next line.
// > Indent:number=The width of the indentation on wrapping.
// > Text:string=The string to print.
// > Glyphs:texture=The texture handle to use for printing glyphs.
// ? Same functionality as PrintW but with the option to print glyphs. You
// ? can do this by specifying \rthhhhhhhh anywhere in your text. Where 'h'
// ? is a hexadecimal number between 00000000 and ffffffff to match the tile
// ? id of the texture. You must zero pad the number as shown.
/* ------------------------------------------------------------------------- */
LLFUNCEX(PrintWTS, 1, LCPUSHNUM(LCGETPTR(1, Font)->PrintWTS(
  LCGETNUMLGE(GLfloat, 2, 0, FLT_MAX, "Width"), LCGETNUM(GLfloat, 3, "Indent"),
  LCGETSTRING(GLubyte, 4, "String"),              LCGETPTR(5, Texture))));
/* ========================================================================= */
// $ Font:PrintWU
// > X:number=The X screen position of the string.
// > Y:number=The Y screen position of the string.
// > Width:number=The X+Width to wrap at.
// > Indent:number=The width of the indentation on wrapping.
// > String:string=The string to print.
// < Height:number=The height of the text printed on screen.
// ? Prints the specified string on screen with left alignment. While printing,
// ? if X exceeds the specified Width, the text is wrapped with the specified
// ? indent size on the following line.
/* ------------------------------------------------------------------------- */
LLFUNCEX(PrintWU, 1, LCPUSHNUM(LCGETPTR(1, Font)->PrintWU(
  LCGETNUM   (GLfloat, 2, "X"),                 LCGETNUM(GLfloat, 3, "Y"),
  LCGETNUMLGE(GLfloat, 4, 0, FLT_MAX, "Width"), LCGETNUM(GLfloat, 5, "Indent"),
  LCGETSTRING(GLubyte, 6, "String"))));
/* ========================================================================= */
// $ Font:PrintU
// > X:number=The X screen position of the string.
// > Y:number=The Y screen position of the string.
// > String:string=The string to print.
// < Height:number=The height of the text printed on screen.
// ? Prints the specified string on screen with bottom-left alignment
/* ------------------------------------------------------------------------- */
LLFUNC(PrintU, LCGETPTR(1, Font)->PrintU(
  LCGETNUM   (GLfloat, 2, "X"), LCGETNUM(GLfloat, 3, "Y"),
  LCGETSTRING(GLubyte, 4, "String")));
/* ========================================================================= */
// $ Font:PrintUS
// > String:string=The string to print.
// < Height:number=The height of the text printed on screen.
// ? Simulates printing a string. Returns height.
/* ------------------------------------------------------------------------- */
LLFUNCEX(PrintUS, 1,
  LCPUSHNUM(LCGETPTR(1, Font)->PrintSU(LCGETSTRING(GLubyte, 2, "String"))));
/* ========================================================================= */
// $ Font:PrintWS
// > WrapX:number=The X position to wrap the text at.
// > Indent:number=The width of the indentation on wrapping.
// > String:string=The string to print.
// < Height:number=The height of the text printed on screen.
// ? Simulates printing a string with word-wrap. Returns height.
/* ------------------------------------------------------------------------- */
LLFUNCEX(PrintWS, 1, LCPUSHNUM(LCGETPTR(1, Font)->PrintWS(
  LCGETNUMLGE(GLfloat, 2, 0, FLT_MAX, "WrapX"), LCGETNUM(GLfloat, 3, "Indent"),
  LCGETSTRING(GLubyte, 4, "String"))));
/* ========================================================================= */
// $ Font:PrintR
// > X:number=The X screen position of the string.
// > Y:number=The Y screen position of the string.
// > Text:string=The string to print.
// ? Prints the specified string on screen with right alignment.
/* ------------------------------------------------------------------------- */
LLFUNC(PrintR, LCGETPTR(1, Font)->PrintR(
  LCGETNUM   (GLfloat, 2, "X"), LCGETNUM(GLfloat, 3, "Y"),
  LCGETSTRING(GLubyte, 4, "String")));
/* ========================================================================= */
// $ Font:PrintC
// > X:number=The X screen position of the string.
// > Y:number=The Y screen position of the string.
// > Text:string=The string to print.
// ? Prints the specified string on screen with centre alignment.
/* ------------------------------------------------------------------------- */
LLFUNC(PrintC, LCGETPTR(1, Font)->PrintC(
  LCGETNUM   (GLfloat, 2, "X"), LCGETNUM(GLfloat, 3, "Y"),
  LCGETSTRING(GLubyte, 4, "String")));
/* ========================================================================= */
// $ Font:PrintCT
// > X:number=The X position of where to start printing the string
// > Y:number=The Y position of the string.
// > Text:string=The string to print.
// > Glyphs:Texture=The texture handle to use for printing glyphs.
// ? Prints the specified string on screen with centre alignment and glyphs.
/* ------------------------------------------------------------------------- */
LLFUNC(PrintCT, LCGETPTR(1, Font)->PrintCT(
  LCGETNUM   (GLfloat, 2, "X"),    LCGETNUM(GLfloat, 3, "Y"),
  LCGETSTRING(GLubyte, 4, "String"), LCGETPTR(        5, Texture)));
/* ========================================================================= */
// $ Font:Dump
// ? Dumps the texture to the specified file in TGA format.
/* ------------------------------------------------------------------------- */
LLFUNCBEGIN(Dump)
  const Font &tC = *LCGETPTR(1, Font);
  tC.Dump(LCGETINTLGE   (size_t, 2, 0, tC.GetSubCount(), "TexId"),
          LCGETCPPSTRING(        3,                      "File"));
LLFUNCEND
/* ========================================================================= */
// $ Font:GetHeight
// < Height:integer=The tile height of the font.
// ? Returns height of the font tile. If this font is a free-type font, this
// ? will just be the height of the white-space character only.
/* ------------------------------------------------------------------------- */
LLFUNCEX(GetHeight, 1, LCPUSHINT(LCGETPTR(1, Font)->duTile.DimGetHeight()));
/* ========================================================================= */
// $ Font:GetName
// < Name:string=Name of the font.
// ? If this font was loaded by a filename or it was set with a custom id.
// ? This function returns that name which was assigned to it.
/* ------------------------------------------------------------------------- */
LLFUNCEX(GetName, 1, LCPUSHXSTR(LCGETPTR(1, Font)->IdentGet()));
/* ========================================================================= */
// $ Font:GetWidth
// < Width:integer=The tile width of the font.
// ? Returns width of the font tile. If this font is a free-type font, this
// ? will just be the width of the white-space character only.
/* ------------------------------------------------------------------------- */
LLFUNCEX(GetWidth, 1, LCPUSHINT(LCGETPTR(1, Font)->duTile.DimGetWidth()));
/* ========================================================================= */
// $ Font:SetGlyphPadding
// > Padding:number=The top side Y padding to add to glyphs
// ? When using print functions with glyph texture addons, this controls the Y
// ? padding.
/* ------------------------------------------------------------------------- */
LLFUNC(SetGPad,
  LCGETPTR(1, Font)->SetGlyphPadding(LCGETNUM(GLfloat, 2, "Padding")));
/* ========================================================================= */
// $ Font:LoadRange
// > Start:integer=The starting UNICODE character index.
// > End:integer=The ending UNICODE character index.
// ? Caches the specified UNICODE character range to the texture. Although new
// ? characters are dynamically loaded on demand, this function will act as a
// ? 'pre-cache' for characters you KNOW you are going to use.
/* ------------------------------------------------------------------------- */
LLFUNC(LoadRange, LCGETPTR(1, Font)->InitFTCharRange(
  LCGETINT(size_t, 2, "Start"), LCGETINT(size_t, 3, "End")));
/* ========================================================================= */
// $ Font:LoadChars
// > Characters:string=A utf-8 string of characters you want to pre-cache.
// ? Caches all the characters in the specified utf-8 character range to the
// ? texture. Although new characters are dynamically loaded on demand, this
// ? function will act as a 'pre-cache' for characters you KNOW you are going
// ? to use.
/* ------------------------------------------------------------------------- */
LLFUNC(LoadChars,
  LCGETPTR(1, Font)->InitFTCharString(LCGETSTRING(GLubyte, 2, "String")));
/* ========================================================================= */
// $ Font:Destroy
// ? Destroys the font and frees all the memory associated with it. The OpenGL
// ? handles and VRAM allocated by it are freed after the main FBO has has been
// ? rendered. The object will no longer be useable after this call and an
// ? error will be generated if accessed.
/* ------------------------------------------------------------------------- */
LLFUNC(Destroy, LCCLASSDESTROY(1, Font));
/* ========================================================================= */
/* ######################################################################### */
/* ## Font:* member functions structure                                   ## */
/* ######################################################################### */
/* ------------------------------------------------------------------------- */
LLRSMFBEGIN                            // Font:* member functions begin
  LLRSFUNC(Destroy),   LLRSFUNC(Dump),       LLRSFUNC(GetHeight),
  LLRSFUNC(GetName),   LLRSFUNC(GetWidth),   LLRSFUNC(LoadChars),
  LLRSFUNC(LoadRange), LLRSFUNC(Print),      LLRSFUNC(PrintC),
  LLRSFUNC(PrintCT),   LLRSFUNC(PrintM),     LLRSFUNC(PrintMT),
  LLRSFUNC(PrintR),    LLRSFUNC(PrintRT),    LLRSFUNC(PrintS),
  LLRSFUNC(PrintT),    LLRSFUNC(PrintTS),    LLRSFUNC(PrintU),
  LLRSFUNC(PrintUS),   LLRSFUNC(PrintW),     LLRSFUNC(PrintWS),
  LLRSFUNC(PrintWT),   LLRSFUNC(PrintWTS),   LLRSFUNC(PrintWU),
  LLRSFUNC(PrintWUT),  LLRSFUNC(SetCA),      LLRSFUNC(SetCB),
  LLRSFUNC(SetCG),     LLRSFUNC(SetCOA),     LLRSFUNC(SetCOB),
  LLRSFUNC(SetCOG),    LLRSFUNC(SetCOR),     LLRSFUNC(SetCORGB),
  LLRSFUNC(SetCORGBA), LLRSFUNC(SetCORGBAI), LLRSFUNC(SetCR),
  LLRSFUNC(SetCRGBAI), LLRSFUNC(SetCRGB),    LLRSFUNC(SetCRGBA),
  LLRSFUNC(SetGPad),   LLRSFUNC(SetGSize),   LLRSFUNC(SetLSpacing),
  LLRSFUNC(SetSize),   LLRSFUNC(SetSpacing),
LLRSEND                                // Font:* member functions end
/* ========================================================================= */
// $ Font.Image
// > Source:Image=The source image for use with the font.
// < Handle:Font=A handle to the newly created texture.
// ? Creates a texture as a font tileset. The function looks for a file with
// ? the same name as 'filename' with the .txt extension which explains the
// ? parameters for the font tileset. These parameters are as follows...
// ?
// ? rangestart = The ASCII character to represent the first tile..
// ? range = The number of ASCII characters in the tileset..
// ? tilewidth = The width of the tile.
// ? tileheight = The height of the tile.
// ? tilespacingwidth = The horizontal spacing between each tile.
// ? tilespacingheight = The vertical spacing between each tile.
// ? filetype = The source type of texture (.bmp or .tga).
// ? width = The widths of each character separated by a whitespace.
// ? default = The default ASCII character to draw if char not available.
// ? name = A name to describe the font.
// ? scale = The starting scale of the characters (i.e. 1=100%, 0.5=50%).
/* ========================================================================= */
LLFUNCEX(Image, 1, LCCLASSCREATE(Font)->InitFont(*LCGETPTR(1, Image)));
/* ========================================================================= */
// $ Font.Create
// > Font:Ftf=An ftf object of a loaded freetype font.
// > Padding:integer=Amount of padding to use to prevent texture spilling.
// > Filter:integer=Font texture filtering mode to use.
// > Flags:integer=Font flags.
// < Handle:Font=A texture handle to the font.
// ? Creates a texture from the specified font using FreeType. Note that the
// ? ftf object invalidated externally as the new font object will assume
// ? private ownership of it.
/* ========================================================================= */
LLFUNCEX(Create, 1, LCCLASSCREATE(Font)->InitFTFont(
 *LCGETPTR (                  1, Ftf),
  LCGETINTLG(GLuint,          2, 0, cOgl->MaxTexSize(),  "TexSize"),
  LCGETINTLG(GLuint,          3, 0, 16,                  "Padding"),
  LCGETINTLG(GLuint,          4, 0, 11,                  "Filter"),
  LCGETFLAGS(ImageFlagsConst, 5, FF_MASK,                "Flags")));
/* ========================================================================= */
// $ Font.Console
// < Handle:Font=Font handle to console texture
// ? Returns the handle to the console font. Useful if you want to reuse the
// ? font in your application. Careful not to mess around with it's properties!
/* ------------------------------------------------------------------------- */
LLFUNCEX(Console, 1, LCCLASSCREATEPTR(Font, cConsole->GetFont()));
/* ========================================================================= */
/* ######################################################################### */
/* ## Font.* namespace functions structure                                ## */
/* ######################################################################### */
/* ------------------------------------------------------------------------- */
LLRSBEGIN                              // Font.* namespace functions begin
  LLRSFUNC(Create), LLRSFUNC(Image), LLRSFUNC(Console),
LLRSEND                                // Font.* namespace functions end
/* ========================================================================= */
/* ######################################################################### */
/* ## Font.* namespace constants structure                                ## */
/* ######################################################################### */
/* ========================================================================= */
// @ Font.Flags
// < Codes:table=The table of key/value pairs of available flags for use with
// ? the InitFTFont function.
/* ------------------------------------------------------------------------- */
LLRSKTBEGIN(Flags)                     // Beginning of ft font loading flags
LLRSKTITEM(IL_,NONE),                  LLRSKTITEM(FF_,USEGLYPHSIZE),
LLRSKTITEM(FF_,FLOORADVANCE),          LLRSKTITEM(FF_,CEILADVANCE),
LLRSKTITEM(FF_,ROUNDADVANCE),          LLRSKTITEM(FF_,STROKETYPE2),
LLRSKTEND                              // End of ft font loading flags
/* ========================================================================= */
LLRSCONSTBEGIN                         // Font.* namespace consts begin
LLRSCONST(Flags),                      // Font loading flags
LLRSCONSTEND                           // Font.* namespace consts end
/* ========================================================================= */
LLNAMESPACEEND                         // End of Font namespace
/* == EoF =========================================================== EoF == */

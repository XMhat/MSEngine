/* == LLFBO.HPP ============================================================ */
/* ######################################################################### */
/* ## MS-ENGINE              Copyright (c) MS-Design, All Rights Reserved ## */
/* ######################################################################### */
/* ## Defines the 'Fbo' namespace and methods for the guest to use in     ## */
/* ## Lua. This file is invoked by 'lualib.hpp'.                          ## */
/* ######################################################################### */
/* ------------------------------------------------------------------------- */
#pragma once                           // Only one incursion allowed
/* ========================================================================= */
/* ######################################################################### */
/* ========================================================================= */
// % Fbo
/* ------------------------------------------------------------------------- */
// ! The fbo class allows the programmer to create an OpenGL frame-buffer
// ! object which they can use to create seperate drawing canvases which they
// ! can apply special effects to without modifying other canvases.
/* ========================================================================= */
namespace NsFbo {                      // Fbo namespace
/* -- Includes ------------------------------------------------------------- */
using namespace IfFboMain;             // Using fbomain namespace
using namespace IfSShot;               // Using screenshot namespace
using namespace IfConsole;             // Using console namespace
/* ========================================================================= */
/* ######################################################################### */
/* ========================================================================= */
// $ Fbo.Blend
// > srcRGB:integer=How the source RGB blending factors are computed.
// > dstRGB:integer=How the dest RGB blending factors are computed.
// > srcAlpha:integer=How the alpha source blending factor is computed.
// > dstAlpha:integer=How the alpha destination blending factor is computed.
// ? Sets the alpha blending function formula of the back buffer using
// ? glBlendFuncSeparate. The parameters are a zero index of the formula you
// ? want to use. These values are available in Fbo.Blends().
/* ------------------------------------------------------------------------- */
LLFUNC(SetBlend, LCGETPTR(1, Fbo)->SetBlend(
  LCGETINTLGE(size_t, 2, 0, OB_MAX, "srcRGB"),
  LCGETINTLGE(size_t, 3, 0, OB_MAX, "dstRGB"),
  LCGETINTLGE(size_t, 4, 0, OB_MAX, "srcAlpha"),
  LCGETINTLGE(size_t, 5, 0, OB_MAX, "dstAlpha")));
/* ========================================================================= */
// $ Fbo:SetVertexEx
// > TriIndex:integer=Triangle index. Triangle #1 (zero) or triangle #2 (one).
// > C1V1:number=GLfloat for coord #1 of vertex #1 of the specified triangle.
// > C2V2:number=GLfloat for coord #2 of vertex #2 of the specified triangle.
// > C1V3:number=GLfloat for coord #1 of vertex #3 of the specified triangle.
// > C2V1:number=GLfloat for coord #2 of vertex #1 of the specified triangle.
// > C1V2:number=GLfloat for coord #1 of vertex #2 of the specified triangle.
// > C2V3:number=GLfloat for coord #2 of vertex #3 of the specified triangle.
// ? Allows you to full control over the fbo vertex co-ordinates apart from
// ? the Z co-ordinate which is not used in this 2D only engine.
/* ------------------------------------------------------------------------- */
LLFUNC(SetVertexEx, LCGETULPTR(1, Fbo)->
  SetVertexEx(LCGETINTLGE(size_t, 2, 0, stTrisPerQuad, "TriIndex"), {
    LCGETNUM(GLfloat, 3, "C1V1"), LCGETNUM(GLfloat, 4, "C2V1"),
    LCGETNUM(GLfloat, 5, "C1V2"), LCGETNUM(GLfloat, 6, "C2V2"),
    LCGETNUM(GLfloat, 7, "C1V3"), LCGETNUM(GLfloat, 8, "C2V3") }));
/* ========================================================================= */
// $ Fbo:SetTexCoordEx
// > TriIndex:integer=Triangle index. Triangle #1 (zero) or triangle #2 (one).
// > C1V1:number=GLfloat for coord #1 of vertex #1 of the specified triangle.
// > C2V2:number=GLfloat for coord #2 of vertex #2 of the specified triangle.
// > C1V3:number=GLfloat for coord #1 of vertex #3 of the specified triangle.
// > C2V1:number=GLfloat for coord #2 of vertex #1 of the specified triangle.
// > C1V2:number=GLfloat for coord #1 of vertex #2 of the specified triangle.
// > C2V3:number=GLfloat for coord #2 of vertex #3 of the specified triangle.
// ? Allows you to full control over the fbo texture co-ordinates.
/* ------------------------------------------------------------------------- */
LLFUNC(SetTexCoordEx, LCGETULPTR(1, Fbo)->
  SetTexCoordEx(LCGETINTLGE(size_t, 2, 0, stTrisPerQuad, "TriIndex"), {
    LCGETNUM(GLfloat, 3, "C1V1"), LCGETNUM(GLfloat, 4, "C2V1"),
    LCGETNUM(GLfloat, 5, "C1V2"), LCGETNUM(GLfloat, 6, "C2V2"),
    LCGETNUM(GLfloat, 7, "C1V3"), LCGETNUM(GLfloat, 8, "C2V3") }));
/* ========================================================================= */
// $ Fbo:SetColourEx
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
// ? Allows you to full control over the colour of the fbo texture colour.
/* ------------------------------------------------------------------------- */
LLFUNC(SetColourEx, LCGETPTR(1, Fbo)->
  SetColourEx(LCGETINTLGE(size_t, 2, 0, stTrisPerQuad, "TriIndex"), {
    LCGETNUM(GLfloat,  3, "RV1"), LCGETNUM(GLfloat,  4, "GV1"),
    LCGETNUM(GLfloat,  5, "BV1"), LCGETNUM(GLfloat,  6, "AV1"),
    LCGETNUM(GLfloat,  7, "RV2"), LCGETNUM(GLfloat,  8, "GV2"),
    LCGETNUM(GLfloat,  9, "BV2"), LCGETNUM(GLfloat, 10, "AV2"),
    LCGETNUM(GLfloat, 11, "RV3"), LCGETNUM(GLfloat, 12, "GV3"),
    LCGETNUM(GLfloat, 13, "BV3"), LCGETNUM(GLfloat, 14, "AV3") }));
/* ========================================================================= */
// $ Fbo:SetVertex
// > Left:number=The left co-ordinate.
// > Top:number=The top co-ordinate.
// > Right:number=The right co-ordinate.
// > Bottom:number=The bottom co-ordinate.
// ? Allows you to set basic vertex co-ordinates when blitting the fbo. For
// ? a more advanced version of this function, see Fbo:SetVertexEx().
/* ------------------------------------------------------------------------- */
LLFUNC(SetVertex, LCGETULPTR(1, Fbo)->SetVertex(
  LCGETNUM(GLfloat, 2, "Left"),  LCGETNUM(GLfloat, 3, "Top"),
  LCGETNUM(GLfloat, 4, "Right"), LCGETNUM(GLfloat, 5, "Bottom")));
/* ========================================================================= */
// $ Fbo:SetVertexA
// > Left:number=The left co-ordinate.
// > Top:number=The top co-ordinate.
// > Right:number=The right co-ordinate.
// > Bottom:number=The bottom co-ordinate.
// > Angle:number=The angle of the vertex
// ? Allows you to set basic vertex co-ordinates when blitting the fbo with
// ? angle calculations.
/* ------------------------------------------------------------------------- */
LLFUNC(SetVertexA, LCGETULPTR(1, Fbo)->SetVertex(
  LCGETNUM(GLfloat, 2, "Left"),  LCGETNUM(GLfloat, 3, "Top"),
  LCGETNUM(GLfloat, 4, "Right"), LCGETNUM(GLfloat, 5, "Bottom"),
  LCGETNUM(GLfloat, 6, "Angle")));
/* ========================================================================= */
// $ Fbo:SetOrtho
// > Left:number=The left co-ordinate.
// > Top:number=The top co-ordinate.
// > Right:number=The right co-ordinate.
// > Bottom:number=The bottom co-ordinate.
// ? Sets the ortho of the specified fbo, meaning, these are the bounds of
// ? where you can draw. Same as glOrtho().
// ? See: https://www.opengl.org/sdk/docs/man2/xhtml/glOrtho.xml
/* ------------------------------------------------------------------------- */
LLFUNC(SetOrtho, LCGETULPTR(1, Fbo)->SetOrtho(
  LCGETNUM(GLfloat, 2, "Left"),  LCGETNUM(GLfloat, 3, "Top"),
  LCGETNUM(GLfloat, 4, "Right"), LCGETNUM(GLfloat, 5, "Bottom")));
/* ========================================================================= */
// $ Fbo:SetTexCoord
// > Left:number=The left co-ordinate.
// > Top:number=The top co-ordinate.
// > Right:number=The right co-ordinate.
// > Bottom:number=The bottom co-ordinate.
// ? Allows you to set basic texture co-ordinates when blitting the fbo. For
// ? a more advanced version of this function, see Fbo:SetTexCoordEx().
/* ------------------------------------------------------------------------- */
LLFUNC(SetTexCoord, LCGETULPTR(1, Fbo)->SetTexCoord(
  LCGETNUM(GLfloat, 2, "Left"),  LCGETNUM(GLfloat, 3, "Top"),
  LCGETNUM(GLfloat, 4, "Right"), LCGETNUM(GLfloat, 5, "Bottom")));
/* ========================================================================= */
// $ Fbo:SetClear
// > State:bool=New clear state
// ? Sets if the fbo should be cleared before rendering to it
/* ------------------------------------------------------------------------- */
LLFUNC(SetClear, LCGETPTR(1, Fbo)->SetClear(LCGETBOOL(2, "Clear")));
/* ========================================================================= */
// $ Fbo:SetClearColour
// > Red:number=The red component intensity (0 to 1).
// > Green:number=The green component intensity (0 to 1).
// > Blue:number=The blue component intensity (0 to 1).
// > Alpha:number=The alpha component intensity (0 to 1).
// ? Same as glClearColour(). Allows you to set the colour for glClear(),
// ? meaning the fbo is cleared with this colour on each new frame loop.
// ? See: https://www.opengl.org/sdk/docs/man2/xhtml/glClear.xml.
// ? See: https://www.opengl.org/sdk/docs/man2/xhtml/glClearColor.xml.
/* ------------------------------------------------------------------------- */
LLFUNC(SetClearColour, LCGETPTR(1, Fbo)->SetClearColour(
  LCGETNUM(GLfloat, 2, "Red"),  LCGETNUM(GLfloat, 3, "Green"),
  LCGETNUM(GLfloat, 4, "Blue"), LCGETNUM(GLfloat, 5, "Alpha")));
/* ========================================================================= */
// $ Fbo:SetColour
// > Red:number=The entire fbo texture red colour intensity (0 to 1).
// > Green:number=The entire fbo texture green colour intensity (0 to 1).
// > Blue:number=The entire fbo texture blue colour intensity (0 to 1).
// > Alpha:number=The entire fbo texture alpha colour intensity (0 to 1).
// ? Sets the colour intensity of all the vertexes for the entire fbo texture.
// ? See Fbo:SetColourEx() for full control over each individual vertex.
/* ------------------------------------------------------------------------- */
LLFUNC(SetColour, LCGETPTR(1, Fbo)->SetQuadRGBA(
  LCGETNUM(GLfloat, 2, "Red"),  LCGETNUM(GLfloat, 3, "Green"),
  LCGETNUM(GLfloat, 4, "Blue"), LCGETNUM(GLfloat, 5, "Alpha")));
/* ========================================================================= */
// $ Fbo:SetFilter
// > Id:integer=The texture filter id.
// ? Sets the texture filtering id for the fbo texture. Mipmaps are
// ? automatically generated if a mipmapping id is selected.
// ? (Magnification / Minification). See 'Texture.Filters()' for more info.
/* ------------------------------------------------------------------------- */
LLFUNC(SetFilter, LCGETPTR(1, Fbo)->
  SetFilterCommit(LCGETINTLGE(GLuint, 2, 0, OF_NM_MAX, "Filter")));
/* ========================================================================= */
// $ Fbo:Activate
// ? Makes the specified fbo the active fbo and all subsequent Texture:* calls
// ? will apply to this fbo. This can be called on the main fbo.
/* ------------------------------------------------------------------------- */
LLFUNC(Activate, LCGETPTR(1, Fbo)->SetActive());
/* ========================================================================= */
// $ Fbo:Finish
// ? Queues the fbo for redrawing after LUA has finished it's tick function.
// ? You only need to run this once, subsequent calls in the same frame will
// ? be ignored as the fbo has already been queued for redrawing.
/* ------------------------------------------------------------------------- */
LLFUNC(Finish, LCGETPTR(1, Fbo)->FinishAndRender());
/* ========================================================================= */
// $ Fbo:Blit
// ? Blits the SPECIFIED fbo to the currently ACTIVE fbo. This just adds
// ? to the active FBO drawing arrays and doesn't actually render until the
// ? active fbo is finished and rendered. The currently stored vertex,
// ? texcoord, colour and ortho values are used.
/* ------------------------------------------------------------------------- */
LLFUNC(Blit, FboActive()->Blit(*LCGETPTR(1, Fbo)));
/* ========================================================================= */
// $ Fbo:BlitT
// > TriIndex:integer=Triangle index. Triangle #1 (zero) or triangle #2 (one).
// ? Blits the SPECIFIED triangle of the SPECIFIED fbo to the currently
// ? ACTIVE fbo. This just adds to the active FBO drawing arrays and doesn't
// ? actually render until the active fbo is finished and rendered. The
// ? currently stored vertex, texcoord, colour and ortho values are used.
/* ------------------------------------------------------------------------- */
LLFUNC(BlitT, FboActive()->BlitTri(*LCGETPTR(1, Fbo),
  LCGETINTLGE(size_t, 2, 0, stTrisPerQuad, "TriIndex")));
/* ========================================================================= */
// $ Fbo:GetLastFloatCount
// < Count:integer=Number of floats in display lists.
// ? Returns the number of floating point numbers entered into the display list
// ? on the last rendered frame.
/* ------------------------------------------------------------------------- */
LLFUNCEX(GetLFloatCount, 1, LCPUSHINT(LCGETPTR(1, Fbo)->GetTris()));
/* ========================================================================= */
// $ Fbo:GetFloatCount
// < Count:integer=Number of floats in display lists.
// ? Returns the number of floating point numbers currently entered into the
// ? display list.
/* ------------------------------------------------------------------------- */
LLFUNCEX(GetFloatCount, 1, LCPUSHINT(LCGETPTR(1, Fbo)->GetTrisNow()));
/* ========================================================================= */
// $ Fbo:GetMatrix
// < Width:number=Total count of horizontal viewable pixels.
// < Height:number=Total count of vertical viewable pixels.
// < Left:number=Minimum viewable matrix X position.
// < Top:number=Minimum viewable matrix Y position.
// < Right:number=Maximum viewable matrix X position.
// < Bottom:number=Maximum viewable matrix Y position.
// < OrthoWidth:number=Current requested ortho width.
// < OrthoHeight:number=Current requested ortho height.
// ? Returns the current matrix information for the specified fbo.
/* ------------------------------------------------------------------------- */
LLFUNCBEGIN(GetMatrix)
  const Fbo &fboCref = *LCGETPTR(1, Fbo);
  LCPUSHNUM(fboCref.GetCoRight());
  LCPUSHNUM(fboCref.GetCoBottom());
  LCPUSHNUM(fboCref.fcStage.GetCoLeft());
  LCPUSHNUM(fboCref.fcStage.GetCoTop());
  LCPUSHNUM(fboCref.fcStage.GetCoRight());
  LCPUSHNUM(fboCref.fcStage.GetCoBottom());
LLFUNCENDEX(6)
/* ========================================================================= */
// $ Fbo:Dump
// < Dest:Fbo=The frame buffer to dump.
// < File:string=(Optional) File to save to.
// ? Takes a screenshot of the specified FBO. You must completely omit the
// ? 'file' parameter to use an engine generated file.
/* ------------------------------------------------------------------------- */
LLFUNC(Dump, cSShot->DumpFBO(*LCGETPTR(1, Fbo),
  GetStackCount(lS) < 2 ? cCommon->Blank() : LCGETCPPFILE(2, "File")));
/* ========================================================================= */
// $ Fbo:Reserve
// > Vertexes:integer=How many 64-bit floats to reserve in GPU float list
// > Commands:integer=How many structs to reserve in GPU commands list
// ? Reserves memory the respective number of items. Use this if the first
// ? complex frame you draw is slow. It's probably because of this as the
// ? default reservation level for the lists is 1024 x 64-bit float/command.
/* ------------------------------------------------------------------------- */
LLFUNC(Reserve, LCGETPTR(1, Fbo)->Reserve(
  LCGETINTLG(size_t, 2, 1, 1000000, "Vertexes"),
  LCGETINTLG(size_t, 3, 1, 1000000, "Commands")));
/* ========================================================================= */
// $ Fbo:Destroy
// ? Destroys the FBO and frees all the memory associated with it. The OpenGL
// ? handles and VRAM allocated by it are freed after the main FBO has has
// ? been rendered. The object will no longer be useable after this call and an
// ? error will be generated if accessed.
/* ------------------------------------------------------------------------- */
LLFUNC(Destroy, LCCLASSDESTROY(1, Fbo));
/* ========================================================================= */
/* ######################################################################### */
/* ## Fbo:* member functions structure                                    ## */
/* ######################################################################### */
/* ------------------------------------------------------------------------- */
LLRSMFBEGIN                            // Fbo:* member functions begin
  LLRSFUNC(Activate),       LLRSFUNC(Blit),           LLRSFUNC(BlitT),
  LLRSFUNC(Destroy),        LLRSFUNC(Dump),           LLRSFUNC(Finish),
  LLRSFUNC(GetFloatCount),  LLRSFUNC(GetLFloatCount), LLRSFUNC(GetMatrix),
  LLRSFUNC(Reserve),        LLRSFUNC(SetBlend),       LLRSFUNC(SetClear),
  LLRSFUNC(SetClearColour), LLRSFUNC(SetColour),      LLRSFUNC(SetColourEx),
  LLRSFUNC(SetFilter),      LLRSFUNC(SetOrtho),       LLRSFUNC(SetTexCoord),
  LLRSFUNC(SetTexCoordEx),  LLRSFUNC(SetVertex),      LLRSFUNC(SetVertexA),
  LLRSFUNC(SetVertexEx),
LLRSEND                                // Fbo:* member functions end
/* ========================================================================= */
// $ Fbo.Main
// < Handle:Fbo=A handle to the main fbo object.
// ? Returns a handle to the main FBO so you can draw to it. Changing the
// ? properties of the main FBO may result in undefined behaviour. Note that
// ? the engine console shares the same FBO so you may want to create a
// ? separatte FBO to draw to.
/* ------------------------------------------------------------------------- */
LLFUNCEX(Main, 1, LCCLASSCREATEPTR(Fbo, &cFboMain->fboMain));
/* ========================================================================= */
// $ Fbo.Create
// > Identifier:string=Reference only user-defined identifier.
// > Width:integer=Width of the FBO.
// > Height:integer=Height of the FBO.
// < Handle:Fbo=A handle to the newly created main fbo object.
// ? Creates a new FBO of the specified size.
/* ------------------------------------------------------------------------- */
LLFUNCEX(Create, 1, LCCLASSCREATE(Fbo)->Init(
  LCGETCPPSTRINGNE(         1,                                 "Identifier"),
  LCGETINTLG      (GLsizei, 2, 1, cOgl->MaxTexSize<GLsizei>(), "Width"),
  LCGETINTLG      (GLsizei, 3, 1, cOgl->MaxTexSize<GLsizei>(), "Height")));
/* ========================================================================= */
// $ Fbo.Draw
// ? When the UI mode is set to 3 which is fully manual mode, you need to call
// ? this function for every frame that changes. The main screen FBO will be
// ? flushed and not finished and OpenGL will NOT swap buffers if you do not
// ? call this function.
/* ------------------------------------------------------------------------- */
LLFUNC(Draw, cFboMain->SetDraw());
/* ========================================================================= */
// $ Fbo.IsDrawing
// ? < State:boolean=Is the main fbo set to redraw?
// ? Returns if the main fbo is set to redraw.
/* ------------------------------------------------------------------------- */
LLFUNCEX(IsDrawing, 1, LCPUSHBOOL(cFboMain->CanDraw()));
/* ========================================================================= */
// $ Fbo.OnRedraw
// > Func:function=The main redraw function to change to
// ? When OpenGL needs to be reinitialised, the specified callback will be
// ? called so the guest can redraw FBO's and maybe do other stuff. All
// ? texture and fbo data is already preserved so there is no need to
// ? reinitialise any of that, you just need to redraw them.
/* ------------------------------------------------------------------------- */
LLFUNC(OnRedraw, LCSETEVENTCB(cLua->lrMainRedraw));
/* ========================================================================= */
// $ Fbo.ConSet
// > State:boolean=The console state.
// < Success:boolean=Returns if the requested function was successful.
// ? Shows or hides the console. This does not modify the cvar state. This
// ? function is in Fbo because it only applies to opengl mode. It also
// ? overrides the 'con_disabled' setting.
/* ------------------------------------------------------------------------- */
LLFUNCEX(ConSet, 1, LCPUSHBOOL(cConsole->DoSetVisible(LCGETBOOL(1, "State"))));
/* ========================================================================= */
// $ Fbo.ConHeight
// > State:number=The console height.
// ? Sets the console height normal without modifying the cvar. This function
// ? is in Fbo because it only applies to opengl mode.
/* ------------------------------------------------------------------------- */
LLFUNC(ConHeight, cConsole->SetHeight(LCGETNUMLG(GLfloat, 1, 0, 1, "Height")));
/* ========================================================================= */
// $ Fbo.ConEnabled
// < State:boolean=The console state.
// ? Returns true if the console is showing, false if it is not. This function
// ? is in Fbo because it only applies to opengl mode.
/* ------------------------------------------------------------------------- */
LLFUNCEX(ConEnabled, 1, LCPUSHBOOL(cConsole->IsVisible()));
/* ========================================================================= */
// $ Fbo.ConLock
// < State:boolean=The console visibility lock state.
// ? Lock the visibility of the console on or off. This function is in Fbo
// ? because it only applies to opengl mode.
/* ------------------------------------------------------------------------- */
LLFUNC(ConLock, cConsole->SetCantDisable(LCGETBOOL(1, "State")));
/* ========================================================================= */
// $ Fbo.Viewport
// > Left:number=The left co-ordinate.
// > Top:number=The top co-ordinate.
// > Right:number=The right co-ordinate.
// > Bottom:number=The bottom co-ordinate.
// ? Sets the viewport of the back buffer, meaning, these are the bounds of
// ? what can be seen. Same as glViewport().
// ? See: https://www.khronos.org/opengles/sdk/docs/man/xhtml/glViewport.xml
/* ------------------------------------------------------------------------- */
LLFUNC(Viewport, cFboMain->SetViewport(
  LCGETINT(GLint,   1, "Left"),  LCGETINT(GLint,   2, "Top"),
  LCGETINT(GLsizei, 3, "Right"), LCGETINT(GLsizei, 4, "Bottom")));
/* ========================================================================= */
/* ######################################################################### */
/* ## Fbo.* namespace functions structure                                 ## */
/* ######################################################################### */
/* ------------------------------------------------------------------------- */
LLRSBEGIN                              // Fbo.* namespace functions begin
  LLRSFUNC(ConEnabled), LLRSFUNC(ConHeight), LLRSFUNC(ConLock),
  LLRSFUNC(ConSet),     LLRSFUNC(Create),    LLRSFUNC(Draw),
  LLRSFUNC(IsDrawing),  LLRSFUNC(Main),      LLRSFUNC(OnRedraw),
  LLRSFUNC(Viewport),
LLRSEND                                // Fbo.* namespace functions end
/* ========================================================================= */
/* ######################################################################### */
/* ## Fbo.* namespace constants structure                                 ## */
/* ######################################################################### */
/* ========================================================================= */
// @ Fbo.Blends
// < Codes:table=The table of key/value pairs of available flags
// ? Returns the texture filters available. Returned as key/value pairs. The
// ? value is a unique identifier to the flag as below.
// ? Indx  Keyname   Actual GL value              Default Parameter?
// ? [00]  Z         GL_ZERO                      -
// ? [01]  O         GL_ONE                       srcAlpha
// ? [02]  S_C       GL_SRC_COLOR                 -
// ? [03]  O_M_S_C   GL_ONE_MINUS_SRC_COLOR       -
// ? [04]  D_C       GL_DST_COLOR                 -
// ? [05]  O_M_D_C   GL_ONE_MINUS_DST_COLOR       -
// ? [06]  S_A       GL_SRC_ALPHA                 srcRGB
// ? [07]  O_M_S_A   GL_ONE_MINUS_SRC_ALPHA       dstRGB, dstAlpha
// ? [08]  D_A       GL_DST_ALPHA                 -
// ? [09]  O_M_D_A   GL_ONE_MINUS_DST_ALPHA       -
// ? [10]  C_C       GL_CONSTANT_COLOR            -
// ? [11]  O_M_C_C   GL_ONE_MINUS_CONSTANT_COLOR  -
// ? [12]  C_A       GL_CONSTANT_ALPHA            -
// ? [13]  O_M_C_A   GL_ONE_MINUS_CONSTANT_ALPHA  -
// ? [14]  S_A_S     GL_SRC_ALPHA_SATURATE        -
// ? [15]  S1_C      GL_SRC1_COLOR                -
// ? [16]  O_M_S1_C  GL_ONE_MINUS_SRC1_COLOR      -
// ? [17]  S1_A      GL_SRC1_ALPHA                -
// ? [18]  O_M_S1_A  GL_ONE_MINUS_SRC1_ALPHA      -
// ? See https://www.khronos.org
// ?   /registry/OpenGL-Refpages/gl4/html/glBlendFuncSeparate.xhtml
// ? for more information about the formulas.
/* ------------------------------------------------------------------------- */
LLRSKTBEGIN(Blends)                    // Beginning of blending types
  LLRSKTITEM(OB_,Z),                   LLRSKTITEM(OB_,O),
  LLRSKTITEM(OB_,S_C),                 LLRSKTITEM(OB_,O_M_S_C),
  LLRSKTITEM(OB_,D_C),                 LLRSKTITEM(OB_,O_M_D_C),
  LLRSKTITEM(OB_,S_A),                 LLRSKTITEM(OB_,O_M_S_A),
  LLRSKTITEM(OB_,D_A),                 LLRSKTITEM(OB_,O_M_D_A),
  LLRSKTITEM(OB_,C_C),                 LLRSKTITEM(OB_,C_A),
  LLRSKTITEM(OB_,S_A_S),
LLRSKTEND                              // End of blending types
/* ======================================================================= */
// @ Fbo.Filters
// < Codes:table=The table of key/value pairs of available flags
// ? Returns the texture filters available. Returned as key/value pairs. The
// ? value is a unique identifier to the flag as below.
// ? # ID  KEYNAME   MAGNIFICATION  MINIFICATION
// ? # ==  ========  =============  =========================
// ? # 00  N_N       GL_NEAREST     GL_NEAREST
// ? # 01  N_L       GL_NEAREST     GL_LINEAR
// ? # 02  L_N       GL_LINEAR      GL_NEAREST
// ? # 03  L_L       GL_LINEAR      GL_LINEAR
// ? # 04  N_N_MM_N  GL_NEAREST     GL_NEAREST_MIPMAP_NEAREST
// ? # 05  L_N_MM_N  GL_LINEAR      GL_NEAREST_MIPMAP_NEAREST
// ? # 06  N_N_MM_L  GL_NEAREST     GL_NEAREST_MIPMAP_LINEAR
// ? # 07  L_N_MM_L  GL_LINEAR      GL_NEAREST_MIPMAP_LINEAR
// ? # 08  N_L_MM_N  GL_NEAREST     GL_LINEAR_MIPMAP_NEAREST
// ? # 09  L_L_MM_N  GL_LINEAR      GL_LINEAR_MIPMAP_NEAREST
// ? # 10  N_L_MM_L  GL_NEAREST     GL_LINEAR_MIPMAP_LINEAR
// ? # 11  L_L_MM_L  GL_LINEAR      GL_LINEAR_MIPMAP_LINEAR
// ? # ==  ========  =============  =========================
// ? Explanations for magnification and minification...
// ? # GL_NEAREST
// ? Returns the value of the texture element that is nearest (in Manhattan
// ? distance) to the center of the pixel being textured.
// ? # GL_LINEAR
// ? Returns the weighted average of the four texture elements that are closest
// ? to the center of the pixel being textured.
// ? # GL_NEAREST_MIPMAP_NEAREST
// ? Chooses the mipmap that most closely matches the size of the pixel being
// ? textured and uses the GL_NEAREST criterion (the texture element nearest to
// ? the center of the pixel) to produce a texture value.
// ? # GL_LINEAR_MIPMAP_NEAREST
// ? Chooses the mipmap that most closely matches the size of the pixel being
// ? textured and uses the GL_LINEAR criterion (a weighted average of the four
// ? texture elements that are closest to the center of the pixel) to produce a
// ? texture value.
// ? # GL_NEAREST_MIPMAP_LINEAR
// ? Chooses the two mipmaps that most closely match the size of the pixel
// ? being textured and uses the GL_NEAREST criterion (the texture element
// ? nearest to the center of the pixel) to produce a texture value from each
// ? mipmap. The final texture value is a weighted average of those two values.
// ? # GL_LINEAR_MIPMAP_LINEAR
// ? Chooses the two mipmaps that most closely match the size of the pixel
// ? being textured and uses the GL_LINEAR criterion (a weighted average of the
// ? four texture elements that are closest to the center of the pixel) to
// ? produce a texture value from each mipmap. The final texture value is a
// ? weighted average of those two values.
/* ------------------------------------------------------------------------- */
LLRSKTBEGIN(Filters)                   // Beginning of filter types
  LLRSKTITEM(OF_,N_N),                 LLRSKTITEM(OF_,N_L),
  LLRSKTITEM(OF_,L_N),                 LLRSKTITEM(OF_,L_L),
  LLRSKTITEM(OF_,N_N_MM_N),            LLRSKTITEM(OF_,L_N_MM_N),
  LLRSKTITEM(OF_,N_N_MM_L),            LLRSKTITEM(OF_,L_N_MM_L),
  LLRSKTITEM(OF_,N_L_MM_N),            LLRSKTITEM(OF_,L_L_MM_N),
  LLRSKTITEM(OF_,N_L_MM_L),            LLRSKTITEM(OF_,L_L_MM_L),
LLRSKTEND                              // End of filter types
/* ========================================================================= */
LLRSCONSTBEGIN                         // Fbo.* namespace consts begin
  LLRSCONST(Blends), LLRSCONST(Filters),
LLRSCONSTEND                           // Fbo.* namespace consts end
/* ========================================================================= */
}                                      // End of Fbo namespace
/* == EoF =========================================================== EoF == */

/* == LLJSON.HPP =========================================================== **
** ######################################################################### **
** ## MS-ENGINE              Copyright (c) MS-Design, All Rights Reserved ## **
** ######################################################################### **
** ## Defines the 'Json' namespace and methods for the guest to use in    ## **
** ## Lua. This file is invoked by 'lualib.hpp'.                          ## **
** ######################################################################### **
** ------------------------------------------------------------------------- */
#pragma once                           // Only one incursion allowed
/* ========================================================================= **
** ######################################################################### **
** ========================================================================= */
// % Json
/* ------------------------------------------------------------------------- */
// ! This allows the programmer to encode and decode Json objects.
/* ========================================================================= */
namespace LLJson {                     // Json namespace
/* -- Dependencies --------------------------------------------------------- */
using namespace IJson::P;
/* ========================================================================= **
** ######################################################################### **
** ## Json:* member functions                                             ## **
** ######################################################################### **
** ========================================================================= */
// $ Json:ToString
// < Data:string=Encoded JSON data
// ? Encodes the data inside the class to JSON string.
/* ------------------------------------------------------------------------- */
LLFUNCEX(ToString, 1,
  LCPUSHVAR(LCGETPTR(1, Json)->StrFromNum<Json::RJCompactWriter>()));
/* ========================================================================= */
// $ Json:ToHRString
// < Data:string=Encoded JSON data
// ? Encodes the data inside the class to JSON string in human readable format.
/* ------------------------------------------------------------------------- */
LLFUNCEX(ToHRString, 1,
  LCPUSHVAR(LCGETPTR(1, Json)->StrFromNum<Json::RJPrettyWriter>()));
/* ========================================================================= */
// $ Json:ToFile
// < Result:integer=Error number code returned (0 = success)
// > Filename:string=The filename to write to
// ? Dumps the entire json into the specified file.
/* ------------------------------------------------------------------------- */
LLFUNCEX(ToFile, 1, LCPUSHVAR(LCGETPTR(1, Json)->
  ToFile<Json::RJCompactWriter>(LCGETCPPFILE(2, "File"))));
/* ========================================================================= */
// $ Json:ToHRFile
// < Result:integer=Error number code returned (0 = success)
// > Filename:string=The filename to write to
// ? Dumps the entire json into the specified file in readable format.
/* ------------------------------------------------------------------------- */
LLFUNCEX(ToHRFile, 1, LCPUSHVAR(LCGETPTR(1, Json)->
  ToFile<Json::RJPrettyWriter>(LCGETCPPFILE(2, "File"))));
/* ========================================================================= */
// $ Json:ToTable
// < Result:table=The entire json scope converted to a lua table
// ? Dumps the entire contents of the JSON's current scope as a LUA table
/* ------------------------------------------------------------------------- */
LLFUNCEX(ToTable, 1, LCGETPTR(1, Json)->ToLuaTable(lS));
/* ========================================================================= */
// $ Json:Name
// < Name:string=Name of the json.
// ? If this json was loaded by a filename or it was set with a custom id.
// ? This function returns that name which was assigned to it.
/* ------------------------------------------------------------------------- */
LLFUNCEX(Name, 1, LCPUSHVAR(LCGETPTR(1, Json)->IdentGet()));
/* ========================================================================= */
// $ Json:Destroy
// ? Destroys the json object and frees all the memory associated with it. The
// ? object will no longer be useable after this call and an error will be
// ? generated if accessed.
/* ------------------------------------------------------------------------- */
LLFUNC(Destroy, LCCLASSDESTROY(1, Json));
/* ========================================================================= **
** ######################################################################### **
** ## Json:* member functions structure                                   ## **
** ######################################################################### **
** ------------------------------------------------------------------------- */
LLRSMFBEGIN                            // Json:* member functions begin
  LLRSFUNC(Destroy),  LLRSFUNC(Name),       LLRSFUNC(ToTable),
  LLRSFUNC(ToString), LLRSFUNC(ToHRString), LLRSFUNC(ToFile),
  LLRSFUNC(ToHRFile),
LLRSEND                                // Json:* member functions end
/* ========================================================================= */
// $ Json.File
// > Filename:string=The filename of the json to load
// ? Decodes the specified string as JSON encoded. The level depth is limited
// ? to 255 due to limitations with LUA's hardcoded stack level.
/* ------------------------------------------------------------------------- */
LLFUNCEX(File, 1,
  LCCLASSCREATE(Json)->SyncInitFileSafe(LCGETCPPFILE(1, "File")));
/* ========================================================================= */
// $ Json.FileAsync
// > Filename:string=The filename of the json to load
// > ErrorFunc:function=The function to call when there is an error
// > ProgressFunc:function=The function to call when there is progress
// > SuccessFunc:function=The function to call when the JSON string is laoded
// ? Decodes the specified string as JSON encoded asynchronously.
/* ------------------------------------------------------------------------- */
LLFUNC(FileAsync, LCCLASSCREATE(Json)->InitAsyncFile(lS));
/* ========================================================================= */
// $ Json.String
// > Code:string=The string of JSON encoded data to decode
// < Handle:Json=Handle to the Json object
// ? Decodes the specified string as JSON encoded. The level depth is limited
// ? to 255 due to limitations with LUA's hardcoded stack level.
/* ------------------------------------------------------------------------- */
LLFUNCEX(String, 1, LCCLASSCREATE(Json)->InitString(lS));
/* ========================================================================= */
// $ Json.StringAsync
// > Code:string=The string of JSON encoded data to decode
// > ErrorFunc:function=The function to call when there is an error
// > ProgressFunc:function=The function to call when there is progress
// > SuccessFunc:function=The function to call when the JSON string is laoded
// ? Decodes the specified string as JSON encoded asynchronously.
/* ------------------------------------------------------------------------- */
LLFUNC(StringAsync, LCCLASSCREATE(Json)->InitAsyncString(lS));
/* ========================================================================= */
// $ Json.Table
// > Table:string=The string of JSON encoded data to decode
// < Handle:Json=Handle to the Json object
// ? Encodes the specified string as JSON encoded. The level depth is limited
// ? to 255 due to limitations with LUA's hardcoded stack level.
/* ------------------------------------------------------------------------- */
LLFUNCEX(Table, 1, LCCLASSCREATE(Json)->InitFromTable(lS));
/* ========================================================================= */
// $ Json.WaitAsync
// ? Halts main-thread execution until all json pcm events have completed
/* ------------------------------------------------------------------------- */
LLFUNC(WaitAsync, cJsons->WaitAsync());
/* ========================================================================= **
** ######################################################################### **
** ## Json.* namespace functions structure                                ## **
** ######################################################################### **
** ------------------------------------------------------------------------- */
LLRSBEGIN                              // Json.* namespace functions begin
  LLRSFUNC(File),        LLRSFUNC(FileAsync), LLRSFUNC(String),
  LLRSFUNC(StringAsync), LLRSFUNC(Table),     LLRSFUNC(WaitAsync),
LLRSEND                                // Json.* namespace functions end
/* ========================================================================= */
}                                      // End of Json namespace
/* == EoF =========================================================== EoF == */

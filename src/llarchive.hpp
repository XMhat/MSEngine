/* == LLARCHIVE.HPP ======================================================== */
/* ######################################################################### */
/* ## MS-ENGINE              Copyright (c) MS-Design, All Rights Reserved ## */
/* ######################################################################### */
/* ## Defines the 'Archive' namespace and methods for the guest to use in ## */
/* ## Lua. This file is invoked by 'lualib.hpp'.                          ## */
/* ######################################################################### */
/* ------------------------------------------------------------------------- */
#pragma once                           // Only one incursion allowed
/* ========================================================================= */
/* ######################################################################### */
/* ========================================================================= */
// % Archive
/* ------------------------------------------------------------------------- */
// ! The archive class allows the programmer to load 7-zip archives into the
// ! engine in which any file loaded by the engine may exist in these archives
// ! as well. Although those files that exist on disk will always override
// ! any archive based file which is configured by the 'ast_override' cvar.
// !
// ! Archives with the extension specified in the cvar 'ast_bundles' in the
// ! current working directory will automatically be loaded at startup which
// ! is ".adb" by default (not .7z).
// !
// ! The executable file on Win32 systems is automatically loaded at startup
// ! in addition so that a programmer can store all assets inside the
// ! executable for portable distribution.
// !
// ! Of course the LZMA library handles the opening of such files so any such
// ! format of .7z file will work fine. Although it is recommended not to use
// ! solid archives as reading these archives will be extremely slow. Also note
// ! that encrypted or split archives are not supported yet as these features
// ! are not handled by the LZMA library.
/* ========================================================================= */
LLNAMESPACEBEGIN(Archive)              // Archive namespace
/* -- Includes ------------------------------------------------------------- */
using namespace IfArchive;             // Using archive interface
/* ========================================================================= */
/* ######################################################################### */
/* ========================================================================= */
// $ Archive:Dir
// > Id:integer=Zero-index id of the directory
// < Name:string=The filename of the directory inside the archive
// < UId:integer=The unique id of the directory inside the archive
// ? Returns the name and unique id of the directory in the archive
/* ------------------------------------------------------------------------- */
LLFUNCBEGIN(Dir)
  const Archive &aCref = *LCGETPTR(1, Archive);
  const StrUIntMapConstIt &itDir = aCref.GetDir(LCGETINTLGE(size_t,
    2, 0, aCref.GetDirList().size(), "Index"));
  LCPUSHXSTR(itDir->first);
  LCPUSHINT(itDir->second);
LLFUNCENDEX(2)
/* ========================================================================= */
// $ Archive:Dirs
// < Directories:integer=Total number of directories inside the archive
// ? Returns total number of the directories inside the archive.
/* ------------------------------------------------------------------------- */
LLFUNCEX(Dirs, 1,
  LCPUSHINT(LCGETPTR(1, Archive)->GetDirList().size()));
/* ========================================================================= */
// $ Archive:DirList
// < Directories:Table=A list of directories inside the archive
// ? Returns all the directories inside the archive.
/* ------------------------------------------------------------------------- */
LLFUNCEX(DirList, 1, LCTOTABLE(LCGETPTR(1, Archive)->GetDirList()));
/* ========================================================================= */
// $ Archive:File
// < Total:integer=Zero-index id of the file
// > Name:string=The filename of the file inside the archive
// > Id:integer=The unique id of the file inside the archive
// ? Returns the name and unique id of the file in the archive
/* ------------------------------------------------------------------------- */
LLFUNCBEGIN(File)
  const Archive &aCref = *LCGETPTR(1, Archive);
  const StrUIntMapConstIt &itFile = aCref.GetFile(LCGETINTLGE(size_t, 2, 0,
    aCref.GetFileList().size(), "Index"));
  LCPUSHXSTR(itFile->first);
  LCPUSHINT(itFile->second);
LLFUNCENDEX(2)
/* ========================================================================= */
// $ Archive:Files
// < Files:integer=Total number of files inside the archive
// ? Returns total number of the files inside the archive.
/* ------------------------------------------------------------------------- */
LLFUNCEX(Files, 1,
  LCPUSHINT(LCGETPTR(1, Archive)->GetFileList().size()));
/* ========================================================================= */
// $ Archive:FileList
// < Directories:Table=A list of files inside the archive
// ? Returns all the files inside the archive.
/* ------------------------------------------------------------------------- */
LLFUNCEX(FileList, 1, LCTOTABLE(LCGETPTR(1, Archive)->GetFileList()));
/* ========================================================================= */
// $ Archive:Size
// > Id:integer=The zero-index unique-id of the file/dir in archive
// < Size:integer=The uncompressed size of the file/dir in the archive
// ? Returns the uncompressed size of the file
/* ------------------------------------------------------------------------- */
LLFUNCBEGIN(Size)
  const Archive &aCref = *LCGETPTR(1, Archive);
  LCPUSHINT(aCref.GetSize(LCGETINTLGE(size_t,
    2, 0, aCref.GetTotal(), "Index")));
LLFUNCENDEX(1)
/* ========================================================================= */
// $ Archive:Total
// < Total:integer=Total number of files and directories in archive
// ? Returns the total number of files and directories in archive
/* ------------------------------------------------------------------------- */
LLFUNCEX(Total, 1, LCPUSHINT(LCGETPTR(1, Archive)->GetTotal()));
/* ========================================================================= */
// $ Archive:Name
// < Name:string=Name of the archive.
// ? Returns the name of the archive which was loaded.
/* ------------------------------------------------------------------------- */
LLFUNCEX(Name, 1, LCPUSHXSTR(LCGETPTR(1, Archive)->IdentGet()));
/* ========================================================================= */
// $ Archive:Destroy
// ? Destroys the archive and frees all the memory associated with it. The
// ? object will no longer be useable after this call and an error will be
// ? generated if accessed.
/* ------------------------------------------------------------------------- */
LLFUNCBEGIN(Destroy) LCCLASSDESTROY(1, Archive); LLFUNCEND
/* ========================================================================= */
/* ######################################################################### */
/* ## Archive:* member functions structure                                ## */
/* ######################################################################### */
/* ------------------------------------------------------------------------- */
LLRSMFBEGIN                            // Archive:* member functions begin
  LLRSFUNC(Destroy),                   // Destroy the internal object
  LLRSFUNC(Dir),                       // Get directory by index
  LLRSFUNC(Dirs),                      // Get directories count
  LLRSFUNC(DirList),                   // Get directories list
  LLRSFUNC(File),                      // Get file by index
  LLRSFUNC(Files),                     // Get files count
  LLRSFUNC(FileList),                  // Get files list
  LLRSFUNC(Name),                      // Get name of archive
  LLRSFUNC(Size),                      // Get size of specified file
  LLRSFUNC(Total),                     // Total files and directories
LLRSEND                                // Archive:* member functions end
/* ========================================================================= */
// $ Archive.Load
// > Filename:string=The filename of the archive to load
// < Handle:Archive=A handle to the loaded archive
// ? Loads the specified 7-zip archive. All subsequent top-level assets
// ? operations will operate with this archive included as well and
// ? all files in this archive will override any previously loaded archives.
// ? Files on disk will always override any archived files. Please note that
// ? you should NOT use solid archives or there could be extreme performance
// ? issues while extracting files from it, this is by LZMA design.
/* ========================================================================= */
LLFUNCEX(Load, 1,
  LCCLASSCREATE(Archive)->InitFromFileSafe(LCGETCPPFILE(1, "File")));
/* ========================================================================= */
// $ Archive.LoadAsync
// > Filename:string=The filename of the archive to load
// > ErrorFunc:function=The function to call when there is an error
// > SuccessFunc:function=The function to call when the file is laoded
// ? Loads a 7-zip archive ogg file off the main thread. The callback functions
// ? send an argument to the archive object that was created. See
// ? Asset.Archive' for more notes about loading 7-zip files.
/* ------------------------------------------------------------------------- */
LLFUNC(LoadAsync, LCCLASSCREATE(Archive)->InitAsyncFile(lS));
/* ========================================================================= */
// $ Archive.WaitAsync
// ? Halts main-thread execution until all async archive events have completed
/* ------------------------------------------------------------------------- */
LLFUNC(WaitAsync, cArchives->WaitAsync());
/* ========================================================================= */
/* ######################################################################### */
/* ## Archive.* namespace functions structure                             ## */
/* ######################################################################### */
/* ------------------------------------------------------------------------- */
LLRSBEGIN                              // Archive.* namespace functions begin
  LLRSFUNC(Load),                      // Load an archive
  LLRSFUNC(LoadAsync),                 // Load an archive async
  LLRSFUNC(WaitAsync),                 // Wait archive async event
LLRSEND                                // Archive.* namespace functions end
/* ========================================================================= */
LLNAMESPACEEND                         // End of Archive namespace
/* == EoF =========================================================== EoF == */

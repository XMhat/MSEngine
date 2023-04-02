/* == LLFILE.HPP =========================================================== */
/* ######################################################################### */
/* ## MS-ENGINE              Copyright (c) MS-Design, All Rights Reserved ## */
/* ######################################################################### */
/* ## Defines the 'File' namespace and methods for the guest to use in    ## */
/* ## Lua. This file is invoked by 'lualib.hpp'.                          ## */
/* ######################################################################### */
/* ------------------------------------------------------------------------- */
#pragma once                           // Only one incursion allowed
/* ========================================================================= */
/* ######################################################################### */
/* ========================================================================= */
// % File
/* ------------------------------------------------------------------------- */
// ! The file class allows the programmer to read and write files from the
// ! game directory.
/* ========================================================================= */
namespace NsFile {                     // File namespace
/* -- Includes ------------------------------------------------------------- */
using namespace IfFile;                // Using file namespace
using namespace IfAsset;               // Using asset namespace
/* ========================================================================= */
/* ######################################################################### */
/* ========================================================================= */
// $ File:FError
// < State:boolean=Error in stream?
// ? Returns the file stream encounted an error.
/* ------------------------------------------------------------------------- */
LLFUNCEX(FError, 1, LCPUSHBOOL(LCGETPTR(1, File)->FStreamFError()));
/* ========================================================================= */
// $ File:Opened
// < State:boolean=Error in stream?
// ? Returns if the file was successfully opened.
/* ------------------------------------------------------------------------- */
LLFUNCEX(Opened, 1, LCPUSHBOOL(LCGETPTR(1, File)->FStreamOpened()));
/* ========================================================================= */
// $ File:End
// < State:boolean=File is at EOF?
// ? Calls feof() and returns if the file pointer is at EOF.
/* ------------------------------------------------------------------------- */
LLFUNCEX(End, 1, LCPUSHBOOL(LCGETPTR(1, File)->FStreamIsEOF()));
/* ========================================================================= */
// $ File:Flush
// < State:boolean=Succeeded?
// ? By default writing to a file stream is buffered. This calls fflush() and
// ? makes sure that any unwritten data to disk is written straight away.
/* ------------------------------------------------------------------------- */
LLFUNCEX(Flush, 1, LCPUSHBOOL(LCGETPTR(1, File)->FStreamFlush()));
/* ========================================================================= */
// $ File:Rewind
// ? Calls frewind() to sets the file stream pointer to position 0.
/* ------------------------------------------------------------------------- */
LLFUNC(Rewind, LCGETPTR(1, File)->FStreamRewind());
/* ========================================================================= */
// $ File:Seek
// < State:boolean=Succeeded?
// > Offset:integer=Offset.
// ? Calls fseek() to set the file stream pointer to the specified position.
// ? from the start of the stream.
/* ------------------------------------------------------------------------- */
LLFUNCEX(Seek, 1, LCPUSHBOOL(LCGETPTR(1, File)->
  FStreamSetPosition(LCGETINT(int64_t, 1, "Position"), SEEK_SET)));
/* ========================================================================= */
// $ File:SeekEnd
// < State:boolean=Succeeded?
// > Offset:integer=Offset.
// ? Calls fseek() to set the file stream pointer to the specified position.
// ? from the end of the stream.
/* ------------------------------------------------------------------------- */
LLFUNCEX(SeekEnd, 1, LCPUSHBOOL(LCGETPTR(1, File)->
  FStreamSetPosition(LCGETINT(int64_t, 1, "Position"), SEEK_END)));
/* ========================================================================= */
// $ File:SeekCur
// < State:boolean=Succeeded?
// > Offset:integer=Offset.
// ? Calls fseek() to adjust the file stream pointer from the current position
// ? of the stream.
/* ------------------------------------------------------------------------- */
LLFUNCEX(SeekCur, 1, LCPUSHBOOL(LCGETPTR(1, File)->
  FStreamSetPosition(LCGETINT(int64_t, 1, "Position"), SEEK_CUR)));
/* ========================================================================= */
// $ File:Read
// < Data:array=The data read.
// > Bytes:integer=Maximum number of bytes to read.
// ? Uses fread() to read the specified number of bytes from the stream. The
// ? data is returned as an array class.
/* ------------------------------------------------------------------------- */
LLFUNCEX(Read, 1, LCCLASSCREATE(Asset)->SwapMemory(LCGETPTR(1, File)->
  FStreamReadBlock(LCGETINT(size_t, 2, "Bytes"))));
/* ========================================================================= */
// $ File:Write
// < Count:integer=Number of bytes written.
// > Data:Asset=The data to write.
// ? Uses fwrite() to write the specified data array to the specified file.
/* ------------------------------------------------------------------------- */
LLFUNCEX(Write, 1,
  LCPUSHINT(LCGETPTR(1, File)->FStreamWriteBlock(*LCGETPTR(2, Asset))));
/* ========================================================================= */
// $ File:ReadStr
// < Bytes:integer=Maximum number of bytes to read.
// > String:string=The string read
// ? Uses fgets() to reads the specified line from the stream. The line ends
// ? at the specified maximum number of bytes to read or until it reaches an
// ? end-of-line character.
/* ------------------------------------------------------------------------- */
LLFUNCEX(ReadStr, 1, LCPUSHXSTR(LCGETPTR(1, File)->FStreamReadString(
  LCGETINTLGE(size_t, 2, 0, UINT_MAX, "Bytes"))));
/* ========================================================================= */
// $ File:WriteStr
// < Count:integer=Number of bytes written
// > Text:string=Text to write.
// ? Writes the specified string to the file.
/* ------------------------------------------------------------------------- */
LLFUNCEX(WriteStr, 1, LCPUSHINT(LCGETPTR(1, File)->
  FStreamWriteString(LCGETCPPSTRING(2, "String"))));
/* ========================================================================= */
// $ File:Tell
// < Position:integer=Position
// ? Returns the current position in file
/* ------------------------------------------------------------------------- */
LLFUNCEX(Tell, 1, LCPUSHINT(LCGETPTR(1, File)->FStreamGetPosition()));
/* ========================================================================= */
// $ File:Size
// < Size:integer=Size in bytes
// ? Returns the current sie of the file
/* ------------------------------------------------------------------------- */
LLFUNCEX(Size, 1, LCPUSHINT(LCGETPTR(1, File)->FStreamSize()));
/* ========================================================================= */
// $ File:Close
// < Result:boolean=If file was closed
// ? Closes the file
/* ========================================================================= */
LLFUNCEX(Close, 1, LCPUSHBOOL(LCGETPTR(1, File)->FStreamClose()));
/* ========================================================================= */
// $ File:Name
// < Name:string=Name of the file.
// ? Returns the name of the loaded file.
/* ------------------------------------------------------------------------- */
LLFUNCEX(Name, 1, LCPUSHXSTR(LCGETPTR(1, File)->IdentGet()));
/* ========================================================================= */
// $ File:Destroy
// ? Closes and destroys the file stream and frees all the memory associated
// ? with it. The object will no longer be useable after this call and an
// ? error will be generated if accessed.
/* ------------------------------------------------------------------------- */
LLFUNC(Destroy, LCCLASSDESTROY(1, File));
/* ========================================================================= */
/* ######################################################################### */
/* ## File:* member functions structure                                   ## */
/* ######################################################################### */
/* ------------------------------------------------------------------------- */
LLRSMFBEGIN                            // File:* member functions begin
  LLRSFUNC(Close),                     // Close the file
  LLRSFUNC(Destroy),                   // Destroys the internal file object
  LLRSFUNC(End),                       // File stream is at eof
  LLRSFUNC(FError),                    // Last error code from file
  LLRSFUNC(Flush),                     // Flush current stream output
  LLRSFUNC(Name),                      // Name of the opened file
  LLRSFUNC(Opened),                    // File opened?
  LLRSFUNC(Read),                      // Read array from stream
  LLRSFUNC(ReadStr),                   // Read string from stream
  LLRSFUNC(Rewind),                    // Rewind to start of stream
  LLRSFUNC(Seek),                      // Seek from stream beginning
  LLRSFUNC(SeekCur),                   // Seek plus stream
  LLRSFUNC(SeekEnd),                   // Seek from stream end
  LLRSFUNC(Size),                      // Size of file
  LLRSFUNC(Tell),                      // Tell current position
  LLRSFUNC(Write),                     // Write array to stream
  LLRSFUNC(WriteStr),                  // Write string to stream
LLRSEND                                // File:* member functions end
/* ========================================================================= */
// $ File.Open
// < Handle:File=The handle to the file stream created.
// > Filename:string=File to open or create.
// > Mode:integer=The mode id to use.
// ? Uses fopen() to open or create a file stream for reading or writing. Note
// ? that this function will only allow access to files from the executable
// ? directory and it's children for security reasons.
/* ------------------------------------------------------------------------- */
LLFUNCEX(Open, 1, LCCLASSCREATE(File)->FStreamOpen(LCGETCPPFILE(1, "File"),
  LCGETINTLGE(FStream::Mode, 2, FStream::Mode(0), FStream::FM_MAX, "Mode")));
/* ========================================================================= */
// $ File.MkDir
// > Directory:string=Directory to create
// < Result:Integer=The errno return code from the C-Lib mkdir() function
// ? Creates the specified directory. Only allowed as a child of the working
// ? executable directory.
// * ----------------------------------------------------------------------- */
LLFUNCEX(MkDir, 1, LCPUSHINT(DirMkDir(LCGETCPPFILE(1, "Directory"))?0:errno));
/* ========================================================================= */
// $ File.MkDirEx
// > Directory:string=Directory to create
// < Result:Integer=The errno return code from the C-Lib mkdir() function
// ? Creates the specified directory. Only allowed as a child of the working
// ? executable directory. This creates interim directories that do not already
// ? exist. If the directory already exists than the function returns success.
// * ----------------------------------------------------------------------- */
LLFUNCEX(MkDirEx, 1,
  LCPUSHINT(DirMkDirEx(LCGETCPPFILE(1, "Directory"))?0:errno));
/* ========================================================================= */
// $ File.RmDir
// > Directory:string=Directory to create
// < Result:Integer=The errno return code from the C-Lib mkdir() function
// ? Creates the specified directory. Only allowed as a child of the working
// ? executable directory.
/* ------------------------------------------------------------------------- */
LLFUNCEX(RmDir, 1, LCPUSHINT(DirRmDir(LCGETCPPFILE(1, "Directory"))?0:errno));
/* ========================================================================= */
// $ File.RmDirEx
// > Directory:string=Directories to remove
// < Result:Integer=The errno return code from the C-Lib mkdir() function
// ? Removes the specified directory and interim directories. Only allowed as a
// ? child of the working executable directory. This removes interim
// ? directories that are empty.
/* ------------------------------------------------------------------------- */
LLFUNCEX(RmDirEx, 1,
  LCPUSHINT(DirRmDirEx(LCGETCPPFILE(1, "Directory"))?0:errno));
/* ========================================================================= */
// $ File.Unlink
// > File:string=File to delete
// < Result:Integer=The errno return code from the C-Lib unlink() function
// ? Deletes the specified file. Only allowed as a child of the working
// ? executable directory.
/* ------------------------------------------------------------------------- */
LLFUNCEX(Unlink, 1,
  LCPUSHINT(DirFileUnlink(LCGETCPPFILE(1, "File"))?0:errno));
/* ========================================================================= */
// $ File.Rename
// > Source:string=Source filename
// > Dest:string=Destination filename
// < Result:Integer=The errno return code from the C-Lib unlink() function
// ? Renames the specified file or directory. Only allowed as a child of the
// ? working executable directory for both parameters.
// * ----------------------------------------------------------------------- */
LLFUNCEX(Rename, 1, LCPUSHINT(DirFileRename(
  LCGETCPPFILE(1, "Source"), LCGETCPPFILE(2, "Destination"))));
/* ========================================================================= */
// $ File.DirExists
// > Source:string=Directory
// < Result:Boolean=Directory exists?
// ? Returns if the specified directory exists and is actually a directory.
/* ------------------------------------------------------------------------- */
LLFUNCEX(DirExists, 1, LCPUSHBOOL(DirLocalDirExists(LCGETCPPFILE(1,"Dir"))));
/* ========================================================================= */
// $ File.FileExists
// > Source:string=Filename
// < Result:Boolean=File exists?
// ? Returns if the specified directory exists and is actually a directory.
/* ------------------------------------------------------------------------- */
LLFUNCEX(FileExists, 1,
  LCPUSHBOOL(DirLocalFileExists(LCGETCPPFILE(1,"File"))));
/* ========================================================================= */
// $ File.Executable
// > Source:string=Filename
// < Result:Boolean=File executable?
// ? Returns if the file on disk is executable. Only allowed as a child of the
// ? working executable directory. Should always return 'false' on Windows.
/* ------------------------------------------------------------------------- */
LLFUNCEX(Executable, 1,
  LCPUSHBOOL(DirIsFileExecutable(LCGETCPPFILE(1, "File"))));
/* ========================================================================= */
// $ File.Exists
// > Source:string=File name or directory name
// < Result:Boolean=File or directory exists?
// ? Returns if the specified file or directory exists on disk.
/* ------------------------------------------------------------------------- */
LLFUNCEX(Exists, 1,
  LCPUSHBOOL(DirLocalResourceExists(LCGETCPPFILE(1,"Resource"))));
/* ========================================================================= */
// $ File.Readable
// > Source:string=Filename
// < Result:Boolean=File readable?
// ? Returns if the file on disk is readable. Only allowed as a child of the
// ? working executable directory.
/* ------------------------------------------------------------------------- */
LLFUNCEX(Readable, 1, LCPUSHBOOL(DirIsFileReadable(LCGETCPPFILE(1, "File"))));
/* ========================================================================= */
// $ File.Writable
// > Source:string=Filename
// < Result:Boolean=File writable?
// ? Returns if the file on disk is writable. Only allowed as a child of the
// ? working executable directory.
/* ------------------------------------------------------------------------- */
LLFUNCEX(Writable, 1,
  LCPUSHBOOL(DirIsFileWritable(LCGETCPPFILE(1, "File"))));
/* ========================================================================= */
// $ File.ReadWritable
// > Source:string=Filename
// < Result:Boolean=File readable and writable?
// ? Returns if the file on disk is readable and writable. Only allowed as a
// ? child of the working executable directory.
/* ------------------------------------------------------------------------- */
LLFUNCEX(ReadWritable, 1,
  LCPUSHBOOL(DirIsFileReadWriteable(LCGETCPPFILE(1, "File"))));
/* ========================================================================= */
// $ File.Info
// > Source:string=Filename
// < Result:boolean=Error number result
// < Size:integer=File size
// < Mode:integer=File attributes
// < Timestamp:integer=File creation time
// < Timestamp:integer=File modification time
// < Timestamp:integer=File access time
// < Links:integer=Number of hard links in the file
// < UserId:integer=User id number
// < GroupId:integer=Group id number
// < Device:integer=Device id
// < RDevice:integer=Device id of special file
// < INode:integer=Inode id
// ? Returns information about a file without opening it.
/* ------------------------------------------------------------------------- */
LLFUNCBEGIN(Info)
  StdFStatStruct stData;
  LCPUSHINT(DirFileSize(LCGETCPPFILE(1, "File"), stData));
  LCPUSHINT(stData.st_size);           LCPUSHINT(stData.st_mode);
  LCPUSHINT(stData.st_ctime);          LCPUSHINT(stData.st_mtime);
  LCPUSHINT(stData.st_atime);          LCPUSHINT(stData.st_nlink);
  LCPUSHINT(stData.st_uid);            LCPUSHINT(stData.st_gid);
  LCPUSHINT(stData.st_dev);            LCPUSHINT(stData.st_rdev);
  LCPUSHINT(stData.st_ino);
LLFUNCENDEX(12)
/* ========================================================================= */
// $ File.Enumerate
// > Source:string=Directory to enumerate
// < Files:table=File list in a indexed table
// < Directories:table=Directory list in a indexed table
// ? Dumps all the files in the specified directory to two tables
/* ------------------------------------------------------------------------- */
LLFUNCBEGIN(Enumerate)
  const Dir dData{ LCGETCPPFILE(1, "Directory") };
  LCTOTABLE(dData.dFiles);
  LCTOTABLE(dData.dDirs);
LLFUNCENDEX(2)
/* ========================================================================= */
// $ File.EnumerateEx
// > Source:string=Directory to enumerate
// > Extension:string=Extension to filter
// < Files:table=File list in a indexed table
// < Directories:table=Directory list in a indexed table
// ? Dumps all the files ending in the specified extension in the specified
// ? directory to two tables.
/* ------------------------------------------------------------------------- */
LLFUNCBEGIN(EnumerateEx)
  const Dir dData{ LCGETCPPFILE(1, "Directory"),
                   LCGETCPPFILE(2, "Extension") };
  LCTOTABLE(dData.dFiles);
  LCTOTABLE(dData.dDirs);
LLFUNCENDEX(2)
/* ========================================================================= */
// $ File.Error
// < ErrNo:integer=Error code returned
// ? Returns the last system error number.
// ? See: https://en.hppreference.com/w/cpp/error/errno_macros
/* ------------------------------------------------------------------------- */
LLFUNCEX(Error, 1, LCPUSHINT(GetErrNo()));
/* ========================================================================= */
// $ File.AppendOneStr
// > Source:string=Filename to append string to.
// > String:string=The string to append.
// < Bytes:integer=Number of bytes written.
// ? Opens/Creates the specified file, appends the specified string and closes
// ? the file.
/* ------------------------------------------------------------------------- */
LLFUNCEX(AppendOneStr, 1,
  LCPUSHINT(FStream(LCGETCPPFILE(1, "File"), FStream::FM_A_B).
    FStreamWriteString(LCGETCPPSTRING(2, "String"))));
/* ========================================================================= */
// $ File.AppendOne
// > Source:string=Filename to append asset to
// > Data:Asset=The asset to write to the file.
// < Bytes:integer=Number of bytes written
// ? Create or open existing file, appends the specified asset and closes
// ? the file.
/* ------------------------------------------------------------------------- */
LLFUNCEX(AppendOne, 1,
  LCPUSHINT(FStream(LCGETCPPFILE(1, "File"), FStream::FM_A_B).
    FStreamWriteBlock(*LCGETPTR(2, Asset))));
/* ========================================================================= */
// $ File.ReadOneStr
// > Source:string=Filename to read string from
// < Mode:integer=The mode to use
// ? Opens the specified file in 'text' mode, reads a line of text into a
// ? string. To read the whole file to a string, use the 'Asset.String(Async)'
/* ------------------------------------------------------------------------- */
LLFUNCEX(ReadOneStr, 1,
  LCPUSHXSTR(FStream(LCGETCPPFILE(1, "File"), FStream::FM_R_T).
    FStreamReadString()));
/* ========================================================================= */
// $ File.WriteOne
// > Source:string=Filename to append asset to
// > Data:Asset=The asset to write to the file.
// < Bytes:integer=Number of bytes written
// ? Create or open existing file, and write the specified asset and closes
// ? the file.
// ------------------------------------------------------------------------- */
LLFUNCEX(WriteOne, 1,
  LCPUSHINT(FStream(LCGETCPPFILE(1, "File"), FStream::FM_W_B).
    FStreamWriteBlock(*LCGETPTR(2, Asset))));
/* ========================================================================= */
// $ File.WriteOneStr
// > Source:string=Filename to write string to
// > String:string=The string to write.
// < Bytes:integer=Number of bytes written
// ? Opens/Creates the specified file, writes the specified string and closes
// ? the file.
/* ------------------------------------------------------------------------- */
LLFUNCEX(WriteOneStr, 1,
  LCPUSHINT(FStream(LCGETCPPFILE(1, "File"), FStream::FM_W_B).
    FStreamWriteString(LCGETCPPSTRING(2, "String"))));
/* ========================================================================= */
// $ File.ValidName
// > Source:string=Filename to check
// < Result:integer=The result of the check
// ? Returns 0 if the filename is valid, non-zero if not.
/* ------------------------------------------------------------------------- */
LLFUNCEX(ValidName, 1, LCPUSHINT(DirValidName(LCGETCPPSTRING(1, "File"))));
/* ========================================================================= */
/* ######################################################################### */
/* ## File.* namespace functions structure                                ## */
/* ######################################################################### */
/* ------------------------------------------------------------------------- */
LLRSBEGIN                              // File.* namespace functions begin
  LLRSFUNC(AppendOne),    LLRSFUNC(AppendOneStr), LLRSFUNC(DirExists),
  LLRSFUNC(Enumerate),    LLRSFUNC(EnumerateEx),  LLRSFUNC(Error),
  LLRSFUNC(Executable),   LLRSFUNC(Exists),       LLRSFUNC(FileExists),
  LLRSFUNC(Info),         LLRSFUNC(MkDir),        LLRSFUNC(MkDirEx),
  LLRSFUNC(Open),         LLRSFUNC(Readable),     LLRSFUNC(ReadOneStr),
  LLRSFUNC(ReadWritable), LLRSFUNC(Rename),       LLRSFUNC(RmDir),
  LLRSFUNC(RmDirEx),      LLRSFUNC(Unlink),       LLRSFUNC(ValidName),
  LLRSFUNC(Writable),     LLRSFUNC(WriteOne),     LLRSFUNC(WriteOneStr),
LLRSEND                                // File.* namespace functions end
/* ------------------------------------------------------------------------- */
LLRSKTBEGIN(Flags)                     // Beginning of open mode flags
  LLRSKTITEM(FStream::FM_,R_T),        LLRSKTITEM(FStream::FM_,W_T),
  LLRSKTITEM(FStream::FM_,A_T),        LLRSKTITEM(FStream::FM_,R_P_T),
  LLRSKTITEM(FStream::FM_,W_P_T),      LLRSKTITEM(FStream::FM_,A_P_T),
  LLRSKTITEM(FStream::FM_,R_B),        LLRSKTITEM(FStream::FM_,W_B),
  LLRSKTITEM(FStream::FM_,A_B),        LLRSKTITEM(FStream::FM_,R_P_B),
  LLRSKTITEM(FStream::FM_,W_P_B),      LLRSKTITEM(FStream::FM_,A_P_B),
  LLRSKTITEM(FStream::FM_,MAX),
LLRSKTEND                              // End of open mode flags
/* ========================================================================= */
LLRSCONSTBEGIN                         // File.* namespace consts begin
  LLRSCONST(Flags),
LLRSCONSTEND                           // File.* namespace consts end
/* ========================================================================= */
}                                      // End of File namespace
/* == EoF =========================================================== EoF == */

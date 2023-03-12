/* == MSENGINE.CPP ========================================================= */
/* ######################################################################### */
/* ## MS-ENGINE              Copyright (c) MS-Design, All Rights Reserved ## */
/* ######################################################################### */
/* ## This the file that handles the inclusing of engine subsystems in a  ## */
/* ## tidy namespace and handles the main entry point.                    ## */
/* ######################################################################### */
/* == Setup includes ======================================================= */
#include "setup.hpp"                   // Setup the compilation environment
/* -- Engine includes ------------------------------------------------------ */
namespace Engine {                     // Start of engine namespace
/* ------------------------------------------------------------------------- */
#include "msengine.hpp"                // Build version numbers
#include "stdtypes.hpp"                // Standard types
#include "flags.hpp"                   // Flags class
#include "utf.hpp"                     // Unicode transformation format utils
#include "std.hpp"                     // Std function wrappers
#include "string.hpp"                  // String utilities
#include "error.hpp"                   // Error class
#include "token.hpp"                   // Token class
#include "vars.hpp"                    // Vars class
#include "psplit.hpp"                  // PathSplit class
#include "ident.hpp"                   // Identifier class
#include "dir.hpp"                     // Dir class
#include "util.hpp"                    // Miscellenious utilities
#include "sysutil.hpp"                 // System utilities
#include "cvardef.hpp"                 // CVar definition registry
#include "clock.hpp"                   // Clock helper classes
#include "args.hpp"                    // Arguments class
#include "cmdline.hpp"                 // Command-line helper class
#include "memory.hpp"                  // Memory management system
#include "fstream.hpp"                 // File stream wrapper
#include "log.hpp"                     // Logging helper
#include "collect.hpp"                 // Collector helpers
#include "stat.hpp"                    // Statistic class
#include "thread.hpp"                  // Thread helper
#include "evtcore.hpp"                 // Thread-safe events system core
#include "evtmain.hpp"                 // Main engine thread events system
#include "condef.hpp"                  // Console definitions
#include "glfwutil.hpp"                // GLFW utilities
#include "glfwwin.hpp"                 // GLFW window class
#include "glfw.hpp"                    // GLFW wrapper
#include "dim.hpp"                     // Dimensions header file
#include "syscore.hpp"                 // Operating system interface
#include "filemap.hpp"                 // File map interface
#include "luautil.hpp"                 // Lua utility functions
#include "luaref.hpp"                  // Lua reference helper class
#include "luaevent.hpp"                // Lua event helper class
#include "luafunc.hpp"                 // Lua function callback helper class
#include "async.hpp"                   // Async file loading helper
#include "crypt.hpp"                   // Cryptography header file
#include "uuid.hpp"                    // UuId class
#include "codec.hpp"                   // Codec header file
#include "archive.hpp"                 // Archive header file
#include "asset.hpp"                   // Asset header file
#include "timer.hpp"                   // Timer header file
#include "sql.hpp"                     // SQL header file
#include "luacode.hpp"                 // Lua code subsystem
#include "luadef.hpp"                  // Lua definitions file
#include "lua.hpp"                     // Lua core subsystem
#include "cvar.hpp"                    // CVar item header file
#include "cvars.hpp"                   // CVars header file
#include "oal.hpp"                     // OpenAL header file
#include "pcmdef.hpp"                  // Pcm data header file
#include "pcmlib.hpp"                  // Pcm formats header file
#include "pcmfmt.hpp"                  // Pcm format plugins
#include "pcm.hpp"                     // Pcm loader header file
#include "fbodef.hpp"                  // Fbo defs header file
#include "ogl.hpp"                     // OpenGL header file
#include "imagedef.hpp"                // Image data header file
#include "imagelib.hpp"                // Image formats header file
#include "imagefmt.hpp"                // Image format plugins
#include "bin.hpp"                     // Bin packing header file
#include "image.hpp"                   // Image loader header file
#include "shader.hpp"                  // Shader program header file
#include "fbobase.hpp"                 // Context, thread and fbo base vars
#include "fboitem.hpp"                 // Frame buffer object render item
#include "fbo.hpp"                     // Frame buffer object header file
#include "fbomain.hpp"                 // Core fbos header file
#include "sshot.hpp"                   // Screenshot header file
#include "texture.hpp"                 // Texture header file
#include "palette.hpp"                 // Palette header file
#include "ftf.hpp"                     // Freetype font header file
#include "font.hpp"                    // Font header file
#include "file.hpp"                    // File class header file
#include "cert.hpp"                    // Certificates include file
#include "socket.hpp"                  // Sockets include file
#include "console.hpp"                 // Console header file
#include "mask.hpp"                    // Mask header file
#include "source.hpp"                  // Source header file
#include "stream.hpp"                  // Stream header file
#include "sample.hpp"                  // Sample header file
#include "video.hpp"                   // Video class
#include "audio.hpp"                   // Audio class
#include "json.hpp"                    // Json class
#include "cursor.hpp"                  // Cursor class
#include "evtwin.hpp"                  // Window thread events system
#include "input.hpp"                   // Input class
#include "display.hpp"                 // Display class
#include "credits.hpp"                 // Credits class
#include "core.hpp"                    // Core class file
#include "cvarlib.hpp"                 // Lua cvar function library
#include "conlib.hpp"                  // Lua console function library
#include "lualib.hpp"                  // Lua lua function api library
/* ------------------------------------------------------------------------- */
};                                     // End of engine namespace
/* == The main entry point (defined in 'core.cpp') ========================= */
int ENTRYFUNC { return Engine::IfCore::CoreMain(__argc, __wargv, _wenviron); }
/* == End-of-File ========================================================== */

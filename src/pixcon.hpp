/* == PIXCON.HPP =========================================================== */
/* ######################################################################### */
/* ## MS-ENGINE              Copyright (c) MS-Design, All Rights Reserved ## */
/* ######################################################################### */
/* ## This is a POSIX specific module that handles text only mode output  ## */
/* ## which is needed by the engines bot mode and uses ncurses. Since we  ## */
/* ## support MacOS and Linux, we can support both systems very simply    ## */
/* ## with POSIX compatible calls.                                        ## */
/* ######################################################################### */
/* ========================================================================= */
#pragma once                           // Only one incursion allowed
/* == Includes ============================================================= */
/* Because ncurses shares function names with STL, we need to put it all the */
/* Curses API in it's own namespace to prevent ambiguity problems.           */
/* ------------------------------------------------------------------------- */
namespace IfCurses {                   // Start of Curses interface
# include <ncurses.h>                  // Using Curses for fancy term effects
}                                      // End of Curses interface
/* == Console Class ======================================================== */
class SysCon :
  /* -- Base classes ------------------------------------------------------- */
  public SysBase,                      // Defined in '(nix/mac)base.hpp'
  public SysConBase,                   // Defined in 'syscore.hpp'
  private IHelper                      // Allow access to windows console
{  /* -- Typedefs ---------------------------------------------------------- */
  typedef IfCurses::attr_t attr_t;     // NCurses alias
  /* -- Console data ------------------------------------------------------- */
  __sighandler_t   fpSignal;           // Old signal handler
  attr_t           aColour;            // Current colour
  attr_t           aColourSaved;       // Saved colour
  /* -- Co-ordinates and limits -------------------------------------------- */
  size_t           stPalette;          // Palette colours count
  size_t           stPairs;            // Pairs count
  int              iCX, iCY;           // Cursor position on commit buffer
  int              iX, iY;             // X/Y drawing position
  int              iW, iH;             // Console width and height
  int              iWm1, iWm2;         // Console width minus 1 and 2
  int              iHm1, iHm2;         // Console height minus 1 and 2
  int              iCursor;            // Current cursor setting
  /* -- Storing original palette ------------------------------------------- */
  typedef array<short,3>             ShortTri;
  typedef array<ShortTri,COLOUR_MAX> ColourTable;
  typedef array<short,2>             ShortPair;
  typedef vector<ShortPair>          PairTable;
  ColourTable      ctPalette;          // Saved colour palette
  PairTable        ptPairs;            // Saved colour pairs
  /* -- Signal handler ----------------------------------------------------- */
  // Please do remember that this call could happen in any thread!
  static void OnTerminalResized(int) { cEvtMain->Add(EMC_CON_RESIZE); }
  /* -- Do set a character at the current position ------------------------- */
  void DoSetChar(const unsigned int uiChar)
  { // Include curses namespace
    using namespace IfCurses;
    // Build character with specified colour
    const cchar_t ccChar{ aColour, { static_cast<wchar_t>(uiChar) },
#ifdef __linux__
      0
#endif
    };
    // Print character and throw error if failed and if it was because we
    // tried to write the bottom-right most character? Then ignore
    // reporting because MacOS is returning error for this but actually
    // changing the colour for us. So that's obviously a bug!
    const int iResultAdd = add_wch(&ccChar);
    if(iResultAdd != OK && (iX != iWm1 || iY != iHm1))
      LW(LH_WARNING,
        "SysCon set character failed! (C:$<$$>;A:$$<$$>;P:$$x$;R:$)",
          uiChar, hex, uiChar, dec, aColour, hex, aColour, dec, iX, iY,
          iResultAdd);
  }
  /* -- Set maximum console line length ---------------------------- */ public:
  CVarReturn RowsModified(const size_t stRows)
  { // Deny if out of range. The maximum value is a SHORT from Win32 API.
    if(stRows < 25 || IntWillOverflow<int>(stRows)) return DENY;
    // Value allowed
    return ACCEPT;
  }
  /* -- Set maximum console line length ------------------------------------ */
  CVarReturn ColsModified(const size_t stCols)
  { // Deny if out of range. The maximum value is a SHORT from Win32 API.
    if(stCols < 80 || IntWillOverflow<int>(stCols)) return DENY;
    // Value allowed
    return ACCEPT;
  }
  /* -- Return console window handle --------------------------------------- */
  void *GetHandle(void) { return nullptr; }
  /* -- Check for and update size ------------------------------------------ */
  void CheckAndUpdateSize(void)
  { // Include curses namespace
    using namespace IfCurses;
    // Return if ncurses is not available. Not seen this trigger yet but I
    // am just going to put this here just incase.
    if(isendwin()) return;
    // Update size of terminal window
    int iNewW, iNewH; getmaxyx(stdscr, iNewH, iNewW);
    // Ignore if not changed
    if(iNewW == iW && iNewH == iH) return;
    // Log the new size
    LW(LH_DEBUG, "SysCon resized from $x$ to $x$.", iNewW, iNewH, iW, iH);
    // Update sizes
    iW = iNewW;
    iWm1 = iW-1;
    iWm2 = iW-2;
    iH = iNewH;
    iHm1 = iH-1;
    iHm2 = iH-2;
    // Update cursor position if invalid
    if(iCX >= iW) iCX = iWm1;
    if(iCY >= iH) iCY = iHm1;
    // Refresh the window which stops failures of other routines
    refresh();
  }
  /* -- Check keys ------------------------------------------------- */ public:
  KeyType GetKey(int &iKey, int &iMods)
  { // Include curses namespace
    using namespace IfCurses;
    // Get key and return if it is anything but OK. There is KEY_CODE_YES to
    // signify function keys but we don't care about them at the moment.
    wint_t wChar = 0;
    switch(const int iResult = get_wch(&wChar))
    { // If no key is waiting then return no key
      case ERR: return KT_NONE;
      // If it's a normal key?
      case OK:
        // Check key
        switch(wChar)
        { // Home key pressed? (Not mapped by default)
          case 1: iMods = 0; iKey = GLFW_KEY_HOME; break;
          // End key pressed? (Not mapped by default)
          case 5: iMods = 0; iKey = GLFW_KEY_END; break;
          // Escape key pressed?
          case 27: iMods = 0; iKey = GLFW_KEY_ESCAPE; break;
          // Return key pressed?
          case '\r': case '\n': iMods = 0; iKey = GLFW_KEY_ENTER; break;
          // Backspace key pressed?
          case '\b': case 127: iMods = 0; iKey = GLFW_KEY_BACKSPACE; break;
          // Tab key pressed?
          case '\t': iMods = 0; iKey = GLFW_KEY_TAB; break;
          // Other key?
          default:
            // Ignore other control keys
            if(wChar < ' ') return KT_NONE;
            // Set key
            iKey = static_cast<int>(wChar);
            // No mods on plain keys
            iMods = 0;
            // Return whether it is an valid key or not
            return KT_CHAR;
        } // Treated as virtual key
        return KT_KEY;
      // Is a function key?
      case KEY_CODE_YES:
        // Test key
        switch(wChar)
        { // Control+Home pressed?
          case 531: /* MacOS \033[1;5H */ case 535: /* Linux default */
            iMods = GLFW_MOD_CONTROL; iKey = GLFW_KEY_HOME; break;
          // Control+End pressed?
          case 526: /* MacOS \033[1;5F */ case 530: /* Linux default */
            iMods = GLFW_MOD_CONTROL; iKey = GLFW_KEY_END; break;
          // Control+PageUp pressed?
          case 551: /* MacOS \033[5;5~ */ case 555: /* Linux default */
            iMods = GLFW_MOD_CONTROL; iKey = GLFW_KEY_PAGE_UP; break;
          // Control+PageDown pressed?
          case 546: /* MacOS \033[6;5~ */ case 550: /* Linux default */
            iMods = GLFW_MOD_CONTROL; iKey = GLFW_KEY_PAGE_DOWN; break;
          // Control+Delete (MacOS KB) or Insert (PC KB) pressed?
          case 515: case KEY_IC: iMods = 0; iKey = GLFW_KEY_INSERT; break;
          // Trivial keys pressed?
          case KEY_BACKSPACE : iMods = 0; iKey = GLFW_KEY_BACKSPACE; break;
          case KEY_ENTER     : iMods = 0; iKey = GLFW_KEY_ENTER; break;
          case KEY_HOME      : iMods = 0; iKey = GLFW_KEY_HOME; break;
          case KEY_END       : iMods = 0; iKey = GLFW_KEY_END; break;
          case KEY_PPAGE     : iMods = 0; iKey = GLFW_KEY_PAGE_UP; break;
          case KEY_NPAGE     : iMods = 0; iKey = GLFW_KEY_PAGE_DOWN; break;
          case KEY_UP        : iMods = 0; iKey = GLFW_KEY_UP; break;
          case KEY_DOWN      : iMods = 0; iKey = GLFW_KEY_DOWN; break;
          case KEY_LEFT      : iMods = 0; iKey = GLFW_KEY_LEFT; break;
          case KEY_RIGHT     : iMods = 0; iKey = GLFW_KEY_RIGHT; break;
          case KEY_DC        : iMods = 0; iKey = GLFW_KEY_DELETE; break;
          // Resized terminal? Send event that the terminal resized.
          case KEY_RESIZE    : OnTerminalResized(0); return KT_NONE;
          // Unknown key
          default:
            // Report it
            LW(LH_DEBUG, "SysCon ignoring $ scan code $ (H:0x$$;O:$$).",
              wChar >= KEY_MAX ? "invalid" : "unsupported", wChar,
              hex, wChar, oct, wChar);
            // Done
            return KT_NONE;
        } // Treated as virtual key
        return KT_KEY;
      // Unknown error
      default: XC("Unknown error reading input!", "Error", iResult);
    }
  }
  /* -- Commit argument for curs_set() ------------------------------------- */
  void CommitCursor(void)
  { // Change it and save new cursor setting
    switch(const int iResult = IfCurses::curs_set(iCursor))
    { // Record new value and break if okay
      case 0: case 1: case 2:break;
      // Anything else report in log?
      default: LW(LH_WARNING, "SysCon set cursor type to $ result $!",
                 iCursor, iResult); break;
    }
  }
  /* -- Update argument for curs_set() ------------------------------------- */
  void UpdateCursorId(void)
  { // Update cursor id
    iCursor = FlagIsSet(SCO_CURVISIBLE) ?
      (FlagIsSet(SCO_CURINSERT) ? 2 : 1) : 0;
  }
  /* -- Set colour --------------------------------------------------------- */
  void SetColour(const size_t stColour)
  { // Include curses namespace for use of COLOR_PAIR
    using namespace IfCurses;
    // Set the colour
    aColour = static_cast<attr_t>(COLOR_PAIR(stColour % stPairs));
  }
  /* -- Set cursor size ---------------------------------------------------- */
  void SetCursorMode(const bool bInsert)
  { // Set insert cursor setting, update and commit it
    FlagSetOrClear(SCO_CURINSERT, bInsert);
    UpdateCursorId();
    CommitCursor();
  }
  /* -- Set cursor visibility state ---------------------------------------- */
  void SetCursorVisibility(const bool bEnabled)
    { FlagSetOrClear(SCO_CURVISIBLE, bEnabled); UpdateCursorId(); }
  /* -- Set cursor position ------------------------------------------------ */
  void SetCursor(const int iNX, const int iNY) { iX = iNX; iY = iNY; }
  /* -- Commit buffer ------------------------------------------------------ */
  void CommitBuffer(void)
  { // Include curses namespace
    using namespace IfCurses;
    // Move cursor to specified position
    switch(const int iResult = move(iCY, iCX))
    { // Success? Break
      case OK: break;
      // Unknown result
      default:
      { // Show error
        LW(LH_WARNING, "SysCon move cursor to $x$ result $!",
          iCX, iCY, iResult); break;
        // Done
        break;
      }
    } // Commit cursor if not changed
    CommitCursor();
    // Commit all settings and buffer
    switch(const int iResult = refresh())
    { // Success? Break
      case OK: break;
      // Unknown result
      default: LW(LH_WARNING, "SysCon refresh result $!", iResult); break;
    }
  }
  /* -- Set a character at the current position ---------------------------- */
  void SetChar(const unsigned int uiChar=' ')
  { // Include curses namespace
    using namespace IfCurses;
    // Move to the specified position and if succeeded?
    const int iResultMove = move(iY, iX);
    if(iResultMove == OK)
    { // If size of unsigned int isn't equal to wchar_t? We will need to check
      // for overflow and print a '?' if so.
      if constexpr(sizeof(uiChar) != sizeof(wchar_t))
        DoSetChar(uiChar > numeric_limits<wchar_t>::max() ? '?' : uiChar);
      // Size is equal to wchar_t?
      else DoSetChar(uiChar);
    } // Failed so write error to log
    else LW(LH_WARNING,
      "SysCon move cursor failed! (C:$<$$>;A:$$<$$>;P:$$x$;R:$)",
        uiChar, hex, uiChar, dec, aColour, hex, aColour, dec, iX, iY,
        iResultMove);
  }
  /* -- Set a character at the specified screen buffer position ------------ */
  void SetCharPos(const int iNX, const int iNY, const unsigned int uiC=' ')
    { SetCursor(iNX, iNY); SetChar(uiC); }
  /* -- Do set characters until we reach the maximum ----------------------- */
  void ClearLine(const int iMax) { for(; iX < iMax; ++iX) SetChar(); }
  void ClearLine(void) { ClearLine(iW); }
  /* -- Save and restore colour -------------------------------------------- */
  void PushColour(void) { aColourSaved = aColour; }
  void PopColour(void) { aColour = aColourSaved; }
  /* -- Distance ----------------------------------------------------------- */
  unsigned int Distance(const unsigned int uiD1, const unsigned int uiD2)
  { // Calculate deltas of components
    const unsigned int
      uiDeltaR = ((uiD1&0x00FF0000) >> 16) - ((uiD2&0x00FF0000) >> 16),
      uiDeltaG = ((uiD1&0x0000FF00) >>  8) - ((uiD2&0x0000FF00) >>  8),
      uiDeltaB =  (uiD1&0x000000FF)        -  (uiD2&0x000000FF);
    // Return distance
    return uiDeltaR * uiDeltaR + uiDeltaG * uiDeltaG + uiDeltaB * uiDeltaB;
  }
  /* -- Set colour as integer helper function ------------------------------ */
  void SetColourInteger(const unsigned int uiColour, const size_t stShift,
    const size_t stXOR)
  { // Minimum distance between colour and closest colour
    unsigned int uiMinDist = Distance(uiColour, uiNDXtoRGB[0]);
    // Prepare first colour
    size_t stChosen = 0;
    // Walk through available colours
    for(size_t stIndex = 1; stIndex < stPalette; ++stIndex)
    { // Get current distance between specified colour and colour in pallette
      unsigned int uiCurDist = Distance(uiColour, uiNDXtoRGB[stIndex]);
      // Ignore the entry if is not near the minimum
      if(uiCurDist >= uiMinDist) continue;
      // Update new selected colour and chosen index
      uiMinDist = uiCurDist;
      stChosen = stIndex;
    } // Set chosen colour
    SetColour((stChosen << stShift) + stXOR);
  }
  /* -- Set foreground colour as int --------------------------------------- */
  void SetForegroundInteger(const unsigned int uiColour)
    { SetColourInteger(uiColour, 0, 0); }
  /* -- Set background colour as int --------------------------------------- */
  void SetBackgroundInteger(const unsigned int uiColour)
    { SetColourInteger(uiColour, 7, 128); }
  /* -- Handle print control character ------------------------------------- */
  void HandlePrintControl(Decoder &utfStr, const bool bSimulation)
  { // Get next character
    switch(utfStr.Next())
    { // Colour selection
      case 'c':
      { // Scan for the hexadecimal value and if we found it? Set Tint if not
        // simulation and we read 8 bytes
        unsigned int uiCol;
        if(utfStr.ScanValue(uiCol) == 8 && !bSimulation)
          SetForegroundInteger(uiCol);
        // Done
        break;
      } // Outline colour selection
      case 'o':
      { // Scan for the hexadecimal value and if we found it? Set Tint if not
        // simulation and we read 8 bytes
        unsigned int uiCol;
        if(utfStr.ScanValue(uiCol) == 8 && !bSimulation)
          SetBackgroundInteger(uiCol);
        // Done
        break;
      } // Reset colour
      case 'r': if(!bSimulation) PopColour(); break;
      // Print glyph? Ignore texture and skip the next value and break
      case 't': utfStr.SkipValue(); break;
      // Invalid control character.
      default: break;
    }
  }
  /* -- Locate a supported character while checking if word can be printed - */
  bool PrintGetWord(Decoder &utfStr, int iXp, const int iWi)
  { // Save position because we're not drawing anything
    const unsigned char *ucpPtr = utfStr.GetCPtr();
    // Until null character. Which control token?
    while(const unsigned int uiChar = utfStr.Next()) switch(uiChar)
    { // Carriage return or space char? Restore position and return no wrap
      case '\n': case ' ': utfStr.SetCPtr(ucpPtr); return false;
      // Other control character? Handle print control characters
      case '\r': HandlePrintControl(utfStr, true); break;
      // Normal character
      default:
        // Printing next character would exceed wrap width?
        if(1 + iXp >= iWi)
        { // Restore position
          utfStr.SetCPtr(ucpPtr);
          // Wrap to next position
          return true;
        } // Move X along
        ++iXp;
        // Done
        break;
    } // Restore position
    utfStr.SetCPtr(ucpPtr);
    // No wrap occured and caller should not Y adjust
    return false;
  }
  /* -- Handle return on print --------------------------------------------- */
  void HandleReturnSimulated(Decoder &utfStr, int &iXp, int &iYp,
    const int iIn)
  { // Go down own line and set indentation
    iXp = iIn;
    ++iYp;
    // Discard further spaces and return string minus one space
    utfStr.Ignore(' ');
  }
  /* -- If co-ordinates are in valid range to draw ------------------------- */
  bool ValidY(const int iYp) { return iYp >= 1 && iYp <= iHm1; }
  /* -- Handle return on print --------------------------------------------- */
  void HandleReturn(Decoder &utfStr, const int iIn)
  { // If we can draw on this Y? Clear up till the end
    if(ValidY(iY)) ClearLine();
    // If we can draw on the next line too? Clear up to indentation
    if(ValidY(++iY)) { iX = 0; ClearLine(iIn); }
    // We cann't draw on the next line? Just set new indentation position
    else iX = iIn;
    // Discard further spaces and return string minus one space
    utfStr.Ignore(' ');
  }
  /* -- Write data upwards and wrapping (same as what Char::* does) -------- */
  int WriteLineWU(Decoder &&utfStr)
  { // Check the string is valid
    if(!utfStr.Valid()) return 1;
    // Save current colour
    PushColour();
    // Indent
    const int iIndent = 1;
    // Simulated cursor position
    int iXp = 0, iYp = 1;
    // Until null character, which character?
    while(const unsigned int uiChar = utfStr.Next()) switch(uiChar)
    { // Carriage return?
      case '\n': HandleReturnSimulated(utfStr, iXp, iYp, iIndent); break;
      // Other control character? Handle print control characters
      case '\r': HandlePrintControl(utfStr, true); break;
      // Whitespace character?
      case ' ':
      { // Move X position forward
        ++iXp;
        // Ignore if the space character processed went over the limit OR
        // Check if the draw length of the next word would go off the limit
        // and if either did? Handle the return!
        if(1 + iXp > iW || PrintGetWord(utfStr, iXp, iW))
          HandleReturnSimulated(utfStr, iXp, iYp, iIndent);
        // Done
        break;
      } // Normal character
      default:
      { // Printing next character would exceed wrap width? Wrap and indent
        if(1 + iXp > iW) HandleReturnSimulated(utfStr, iXp, iYp, iIndent);
        // No exceed, move X position forward
        iXp += 1;
        // Done
        break;
      }
    } // Restore previous colour incase it was changed
    PopColour();
    // Reset the iterator on the utf string.
    utfStr.Reset();
    // Set actual cursor position
    iX = 0;
    iY -= iYp;
    // Record original X and Y position
    const int iXO = iX + iIndent;
    // Until null character, which character?
    while(const unsigned int uiChar = utfStr.Next()) switch(uiChar)
    { // Carriage return?
      case '\n': HandleReturn(utfStr, iXO); break;
      // Other control character? Handle print control characters
      case '\r': HandlePrintControl(utfStr, false); break;
      // Whitespace character?
      case ' ':
      { // Do the print if we can
        if(ValidY(iY)) SetChar(uiChar);
        // Move the next X position
        iX++;
        // Ignore if the space character processed went over the limit OR
        // Check if the draw length of the next word would go off the limit
        // and if either did? Handle the wrapping
        if(1 + iX > iW || PrintGetWord(utfStr, iX, iW))
          HandleReturn(utfStr, iXO);
        // Done
        break;
      } // Normal character
      default:
      { // Printing next character would exceed wrap width?
        if(iX + 1 > iW)
        { // Handle the wrapping
          HandleReturn(utfStr, iXO);
          // If we can print the character? Print the character
          if(ValidY(iY)) SetChar(uiChar);
        } // Wouldn't wrap so if we can print the character? Print the char
        else if(ValidY(iY)) SetChar(uiChar);
        // Move along the X position
        iX++;
        // Done
        break;
      }
    } // Clear the rest of the line
    if(ValidY(iY)) ClearLine();
    // Restore colour
    PopColour();
    // Return height of printed text
    return iYp;
  }
  /* -- Write data --------------------------------------------------------- */
  void WriteLine(Decoder &&utfString, const int iMax, const bool bClrEOL)
  { // For each character index in the buffer
    while(const unsigned int uiChar = utfString.Next())
    { // Compare character
      switch(uiChar)
      { // Carriage return? (impossible).
        case '\n': break;
        // Is a control character?
        case '\r': HandlePrintControl(utfString, false); break;
        // Get pointer to character ata
        default : SetChar(uiChar); iX++; break;
      } // Increase position in screen buffer and break if at end of string/buf
      if(iX >= iMax) break;
    } // Now we need to clear old characters up to the end of line
    if(bClrEOL) ClearLine();
  }
  /* -- Redraw status input text ------------------------------------------- */
  void RedrawInputBar(const string &strIL, const string &strIR)
  { // Set cursor position
    SetCursor(0, iHm1);
    // Set input bar colour
    SetColour(79);
    // Clear the line with the above colour
    ClearLine();
    // Length of left part of text
    iX = 1;
    // Left string length
    int iLen;
    // Have left side text?
    if(!strIL.empty())
    { // Put left text in a UTF container
      Decoder utfString{ strIL };
      // Set length
      iLen = static_cast<int>(utfString.Length());
      // Reset at scrolled position
      utfString.Reset(strIL.c_str() + abs(Maximum(0, iLen-iWm2)));
      // Draw start of input text
      WriteLine(move(utfString), iWm1, false);
    } // Left size of text is zero long
    else iLen = 0;
    // Have right side of text and have characters left on screen to spare?
    if(!strIR.empty() && iLen < iWm2)
    { // Set cursor position
      SetCursor(1 + iLen, iHm1);
      // Reset again and write the string
      WriteLine(Decoder(strIR), iWm1, false);
    } // Actually set cursor position
    iCY = iHm1;
    iCX = Minimum(iWm1, 1 + iLen);
    // Cursor is visible
    SetCursorVisibility(true);
  }
  /* -- Set a space at the specified X position ---------------------------- */
  void SetWhitespace(const int iPos=0)
  { // Set start position
    iX = iPos;
    // Set the whitespace
    SetChar();
    // Move on
    ++iX;
  }
  /* -- Redraw a status bar ------------------------------------------------ */
  void RedrawStatus(const int iSY, const string &strL, const string &strR)
  { // Reset drawing row
    SetCursor(0, iSY);
    // Set colour
    SetColour(31);
    // Get length of left and right part of string
    Decoder utfL{ strL }, utfR{ strR };
    const int iL = static_cast<int>(utfL.Length()),
              iLC = static_cast<int>(Minimum(iL, iWm2)),
              iR = static_cast<int>(utfR.Length()),
              iRC = static_cast<int>(Minimum(iR, iWm2));
    // If we have left status text length?
    if(iLC)
    { // If the right text would not completely obscure the left text?
      if(iRC < iWm2)
      { // Set start position, clear the character and move forward
        SetWhitespace();
        // Reset left text position
        utfL.Reset();
        // Put string in a container and draw the string with left align
        WriteLine(move(utfL), iX+iLC, false);
        // If there is no right text we can just clear to the end of line
        if(!iRC) return ClearLine();
      } // No length of left text? Set initial position.
      else iX = 1;
    } // If there is no right next
    else if(!iRC)
    { // Select start of line
      iX = 0;
      // Clear to end of line
      return ClearLine();
    } // No length of left text and have right text? Set starting whitespace.
    else SetWhitespace();
    // Both texts can only share 'iWm2' characters so they need to share
    // this limited space. If there is enough space for both?
    const int iTotal = Minimum(iWm2, iLC + iRC);
    if(iTotal < iWm2)
    { // Get position to write at
      const int iSpaces = iWm2 - iTotal;
      const int iEnd = iX + iSpaces;
      // Repeat writing spaces until we've reached the end position
      do SetChar(); while(++iX < iEnd);
    } // We didn't need to draw any spaces? Set final whitespace.
    else SetWhitespace(iWm2 - iRC);
    // Reset right string position
    utfR.Reset();
    // Write the line and clear the rest
    WriteLine(move(utfR), iX+iRC, true);
  }
  /* -- Redraw bottom status bar ------------------------------------------- */
  void RedrawStatusBar(const string &strSL, const string &strSR)
  { // Redraw the status bar
    RedrawStatus(iHm1, strSL, strSR);
    // Set cursor position on commit
    iCX = iWm1;
    iCY = iHm1;
    // No input bar so hide cursor
    SetCursorVisibility(false);
  }
  /* -- Redraw title status bar -------------------------------------------- */
  void RedrawTitleBar(const string &strTL, const string &strTR)
    { RedrawStatus(0, strTL, strTR); }
  /* -- Redraw console buffer ---------------------------------------------- */
  void RedrawBuffer(const ConLines &cL, const ConLinesConstRevIt &lS)
  { // Reset cursor position to top left of drawing area
    SetCursor(0, iHm1);
    // Set default text colour
    SetColour(15);
    // Draw until we go off the top of the screen
    for(ConLinesConstRevIt lL(lS); lL != cL.rend() && ValidY(iY); ++lL)
    { // Get structure
      const ConLine &lD = *lL;
      // Convert RGB colour to Win32 console id colour
      SetColour(lD.cColour);
      // Put text in a utf container and write the data. Note: Although
      // iY could go effectively negative and we're not using a signed value
      // we can make sure we don't access any OOB memory by just checking that
      // the co-ordinates while drawing, we don't have to worry about the
      // integer wrapping at all we are not drawing.
      iY -= WriteLineWU(Decoder(lD.strLine)) - 1;
    } // Clear extra lines we didn't draw too
    while(iY-- > 1) { iX = 0; ClearLine(); }
  }
  /* -- Update size (from event) ------------------------------------------- */
  void OnResize(void)
  { // Include curses namespace
    using namespace IfCurses;
    // Return if size didn't change
    CheckAndUpdateSize();
    // De-initialise ncurses
    switch(const int iResult = endwin())
    { // Break if OK
      case OK: break;
      // Any other value
      default: XC("Failed to de-initialise ncurses!", "Code", iResult);
    } //Re-initialise ncurses
    if(!initscr()) XC("Failed to re-initialise ncurses!");
  }
  /* -- DeInitialise ------------------------------------------------------- */
  void SysConDeInit(void)
  { // Include curses namespace
    using namespace IfCurses;
    // Ignore if not initialised
    if(IHNotDeInitialise()) return;
    // System console is de-initialising
    LW(LH_DEBUG, "SysCon de-initialising...");
    // If we have a context?
    if(stdscr)
    { // If we can use colour?
      if(can_change_color())
      { // Make sure we initialise all the pairs
        for(size_t stIndex = 1; stIndex < stPairs; ++stIndex)
        { // Get saved palette entry
          ShortPair &spEntry = ptPairs[stIndex];
          // Restore pair entry
          init_pair(static_cast<short>(stIndex), spEntry[0], spEntry[1]);
        } // For each palette entry
        for(size_t stIndex = 1; stIndex < stPalette; ++stIndex)
        { // Get saved palette entry
          ShortTri &stEntry = ctPalette[stIndex];
          // Restore palette entry
          init_color(static_cast<short>(stIndex),
            stEntry[0], stEntry[1], stEntry[2]);
        }
      } // Restore terminal settings
      endwin();
    } // Install old signal handler
    if(fpSignal) signal(SIGWINCH, fpSignal);
    // System console is de-initialised
    LW(LH_INFO, "SysCon de-initialised!");
  }
  /* -- Initialise --------------------------------------------------------- */
  void SysConInit(const char*const, const size_t, const size_t, const bool)
  { // Include curses namespace
    using namespace IfCurses;
    // Console class initialised
    IHInitialise();
    // System console is initialising
    LW(LH_DEBUG, "SysCon initialising...");
    // Set locale
    if(const char*const cpLocale = setlocale(LC_ALL, cpBlank))
      { LW(LH_DEBUG, "SysCon initialised locale to $.", cpLocale); }
    else XCL("Failed to initialse default locale!");
    // Init ncurses
    if(!initscr()) XC("Failed to initialise terminal window!");
    // Disable echo
    if(noecho() == ERR) XC("Failed to disable echo!");
    // No newlines on writing output
    if(nonl() == ERR) XC("Failed to disable newline mode!");
    // No delay to getch() please
    if(nodelay(stdscr, true) == ERR) XC("Failed to enable nodelay mode!");
    // Disable ctrl+c?
    if(cbreak() == ERR) XC("Failed to set cbreak mode!");
    // Enable raw mode
    if(raw() == ERR) XC("Failed to set raw mode!");
    // Enable special keys
    if(keypad(stdscr, TRUE) == ERR) XC("Failed to enable special keys!");
    // Disable interrupt fluash
    if(intrflush(stdscr, FALSE) == ERR)
      XC("Failed to disable interrupt flush!");
    // If we can use colour?
    if(can_change_color())
    { // Using colour
      if(start_color() == ERR) XC("Failed to initialise colours!");
      // For each colour in OUR palette...
      for(stPalette = 0; stPalette < COLOUR_MAX; ++stPalette)
      { // Get saved palette entry
        ShortTri &stEntry = ctPalette[stPalette];
        // Narrow the id
        const short sPalette = static_cast<short>(stPalette);
        // Save old colour so we can restore it
        if(color_content(sPalette,
          &stEntry[0], &stEntry[1], &stEntry[2]) == ERR)
            stEntry[0] = stEntry[1] = stEntry[2] = -1;
        // Grab the colour required by the engine
        unsigned int uiRGB = uiNDXtoRGB[stPalette];
        // Calculate new value based on our required colour.
        const short sR = static_cast<short>(static_cast<double>
            (((uiRGB & 0xFF0000) >> 16)) / 0xFF * 1000),
                    sG = static_cast<short>(static_cast<double>
            (((uiRGB & 0x00FF00) >>  8)) / 0xFF * 1000),
                    sB = static_cast<short>(static_cast<double>
            ( (uiRGB & 0x0000FF)       ) / 0xFF * 1000);
        // Feed the RGB to set the palette colours and if failed just break
        // because we don't really know how many colours the terminal supports.
        if(init_color(sPalette, sR, sG, sB) == ERR) continue;
      } // If we loaded colours?
      if(stPalette > 0)
      { // Get maximum supported colours
        const size_t stPairsMaxSupported = static_cast<size_t>(COLOR_PAIRS);
        // Create number of pairs we need. We only need COLOUR_MAX squared so
        // we dont need to load the reset if there are any.
        if(const size_t stPairsMax =
          Minimum<size_t>(stPairsMaxSupported, COLOUR_MAX*COLOUR_MAX))
        { // Size palette array to save colours
          ptPairs.resize(stPairsMax);
          // For each background colour we support. Ignore the first entry.
          for(stPairs = 1; stPairs < stPairsMax; ++stPairs)
          { // Save old foreground and background palette entry
            ShortPair &spEntry = ptPairs[stPalette];
            if(pair_content(static_cast<short>(stPairs),
              &spEntry[0], &spEntry[1]) == ERR)
                spEntry[0] = spEntry[1] = -1;
            // Calculate foreground and background positions
            const size_t stIndexBG = stPairs / COLOUR_MAX,
                         stIndexFG = stPairs % COLOUR_MAX,
            // If the pairs are the same colour then we don't want to
            // initialise these pairs as whats the point in trying to
            // display the same colour on the same background?
                         stAdd = stIndexFG == stIndexBG ? 1 : 0;
            // Initialise the colour code Wrap the id's to the number of
            // palette entries the terminal let us initialise. Goto the
            // next pair if it succeded else fall through to finish.
            init_pair(static_cast<short>(stPairs),
              static_cast<short>((stIndexFG + stAdd) % stPalette),
              static_cast<short>(stIndexBG % stPalette));
          } // Log number of colours and pairs loaded
          LW(LH_DEBUG, "SysCon set $/$ palette entries and $/$($) pairs!",
            stPalette, COLOUR_MAX, stPairs, stPairsMax,
            stPairsMaxSupported );
        } // No
        else
        { // Clear the saved pairs table
          ptPairs.clear();
          // Log that we don't have any colour pairs
          LW(LH_DEBUG, "SysCon set no pairs from the $ palette entries!",
            stPalette);
        }
      } // No palette entries?
      else LW(LH_WARNING, "SysCon set no palette entries from $!", COLOUR_MAX);
    } // Install window update signal
    fpSignal = signal(SIGWINCH, OnTerminalResized);
    if(!fpSignal) XCL("Failed to install window update handler!");
    // Update size
    CheckAndUpdateSize();
    // System console is initialised
    LW(LH_INFO, "SysCon initialised.");
  }
  /* -- Constructor -------------------------------------------------------- */
  SysCon(void) :
    /* -- Initialisation of members ---------------------------------------- */
    IHelper{ __FUNCTION__ },           // Initialise init helper
    fpSignal(nullptr),                 // Signal handler on standby
    aColour(0),                        // Black colour
    aColourSaved(0),                   // Black saved colour
    stPalette(0),                      // No palette colours
    stPairs(0),                        // No colour pairs
    iCX(0), iCY(0),                    // Cursor in top-left
    iX(0), iY(0),                      // Drawing position in top left
    iW(0), iH(0),                      // No screen dimension size
    iWm1(-1), iWm2(-1),                // Reset screen dimensions minus 1
    iHm1(-2), iHm2(-2),                // Reset screen dimensions minus 2
    iCursor(-1)                        // Current cursor setting uninitialised
    /* -- No code ---------------------------------------------------------- */
    { }
  /* -- Destructor --------------------------------------------------------- */
  DTORHELPER(~SysCon, SysConDeInit());
  /* ----------------------------------------------------------------------- */
  DELETECOPYCTORS(SysCon);             // Do not need defaults
};/* ----------------------------------------------------------------------- */
#define MSENGINE_SYSCON_CALLBACKS()    // Not required
/* == EoF =========================================================== EoF == */

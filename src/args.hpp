/* == ARGS.HPP ============================================================= */
/* ######################################################################### */
/* ## MS-ENGINE              Copyright (c) MS-Design, All Rights Reserved ## */
/* ######################################################################### */
/* ## This module defines a arguments parsing class you can use to parse  ## */
/* ## a plain string of command-line into a array indexed vector.         ## */
/* ######################################################################### */
/* ========================================================================= */
#pragma once                           // Only one incursion allowed
/* ------------------------------------------------------------------------- */
namespace IfArgs {                     // Start of module namespace
/* ------------------------------------------------------------------------- */
struct Arguments :                     // Arguments list
  /* -- Dependencies ------------------------------------------------------- */
  public StrVector                     // List of arguments parsed
{ /* -- Direct access using class variable name which returns if !empty ---- */
  operator bool(void) const { return !empty(); }
  /* -- Constructor -------------------------------------------------------- */
  Arguments(void) { }
  /* -- Constructor with string argument ----------------------------------- */
  explicit Arguments(const string &strArgs)
  { // The current character position in the arguments string
    size_t stPos = 0;
    // Ignore the prefixed whitespace characters
    while(strArgs[stPos] == ' ') if(++stPos >= strArgs.length()) return;
    // This is the length of the usable string
    size_t stLength = strArgs.length();
    // Ignore all suffixed whitespace characters
    for(; strArgs[stLength-1] == ' '; --stLength);
    // This tells us if we're in a argument that started with a separator
    bool bQuote;
    // Set if we're starting an argument with a valid separator character
    switch(strArgs[stPos]) { case '\'': case '"': bQuote = true; break;
                                         default: bQuote = false; break; }
    // Set starting position because this current character is valid
    size_t stStart = stPos;
    // Until we reach end of string
    for(char cSepChar = '\0'; stPos < stLength; ++stPos)
    { // Store and check current character
      Again: switch(const char cChar = strArgs[stPos])
      { // Whitespace?
        case ' ':
        { // Break if we already have a seperator character because whitespaces
          // are allowed until we find the end of it.
          if(cSepChar != '\0') break;
          // If we have an argument to save?
          if(stPos > stStart)
            emplace_back(strArgs.substr(stStart, stPos-stStart));
          // Skip spaces and return the list if we've run out of characters
          while(strArgs[stPos] == ' ') if(++stPos >= stLength) return;
          // Reset starting and ending position
          stStart = stPos;
          // Set if we're in quotation marks
          switch(strArgs[stPos])
            { case '\'' : case '"': bQuote = true; break;
                           default: bQuote = false; break; }
          // Test current character, don't move position ahead
          goto Again;
        } // Apostrophe or quotation mark?
        case '\'' : case '"':
        { // Have a separator character?
          if(cSepChar != '\0')
          { // Its the seperator char? Reset seperator character
            if(cChar == cSepChar)
            { // Get position plus one
              const size_t stPosP1 = stPos + 1;
              // At end of string?
              if(stPosP1 >= stLength)
              { // If we're in quotation marks
                if(bQuote)
                { // Move start forward to ignore the starting quote character
                  ++stStart;
                  // Add new entry
                  emplace_back(strArgs.substr(stStart, stPos-stStart));
                } // Not in quotation marks? Add new entry
                else emplace_back(strArgs.substr(stStart,
                  stPosP1-stStart));
                // Return the list
                return;
              } // Is a space character?
              if(strArgs[stPosP1] == ' ')
              { // If we're in quotation marks
                if(bQuote)
                { // Move start forward to ignore the starting quote character
                  ++stStart;
                  // Add new entry
                  emplace_back(strArgs.substr(stStart, stPos-stStart));
                } // Not in quotation marks? Add new entry
                else emplace_back(
                  strArgs.substr(stStart, stPosP1-stStart));
                // Skip spaces
                while(strArgs[++stPos] == ' ')
                  if(stPos >= stLength) return;
                // Reset starting and ending position
                stStart = stPos;
                // Set if we're in quotation marks
                switch(strArgs[stPos]) { case '\'': case '"': bQuote = true;
                  break; default: bQuote = false; break; }
                // Reset separator character
                cSepChar = '\0';
                // Test current character, don't move position ahead
                goto Again;
              } // Not at end of string and not a space character
            } // Not separator character
          } // Set new seperator character
          else cSepChar = cChar;
          // Done
          break;
        } // Everything else
        default: break;
      } // Character check
    } // If we have an argument to save then add the final argument to the list
    if(stPos > stStart)
      emplace_back(strArgs.substr(stStart, stPos-stStart));
  }
};
/* -- Build an array of arguments from a string ---------------------------- */
static const Arguments ArgumentsBuildSafe(const string &strArgs)
  { return strArgs.empty() ? Arguments{} : Arguments{ strArgs }; }
/* ------------------------------------------------------------------------- */
};                                     // End of module namespace
/* == EoF =========================================================== EoF == */

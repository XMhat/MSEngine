/* == STAT.HPP ============================================================= */
/* ######################################################################### */
/* ## MS-ENGINE              Copyright (c) MS-Design, All Rights Reserved ## */
/* ######################################################################### */
/* ## A class that arranges data into a table and neatly provides a       ## */
/* ## cleanly, human-readable, easy to read formatted output. Mainly used ## */
/* ## for justified data output in console.                               ## */
/* ######################################################################### */
/* ========================================================================= */
#pragma once                           // Only one incursion allowed
/* ------------------------------------------------------------------------- */
namespace IfStat {                     // Start of module namespace
/* -- Includes ------------------------------------------------------------- */
using namespace IfCollector;           // Using collector namespace
using namespace IfUtil;                // Using util namespace
/* -- Statistic ------------------------------------------------------------ */
class Statistic
{ /* -- Private typedefs --------------------------------------------------- */
  typedef ios_base &(*JCCallback)(ios_base&);
  /* ----------------------------------------------------------------------- */
  struct Head                          // Column data
  { /* --------------------------------------------------------------------- */
    const string   strName;            // Column name
    JCCallback     jFunc;              // Justification callback function
    int            iMaxLen;            // Maximum header length
  };/* --------------------------------------------------------------------- */
  typedef deque<Head> HeadDeque;       // Column header data list
  /* -- Private variables -------------------------------------------------- */
  HeadDeque        hdHeaders;          // Headers data
  StrList          slValues;           // Values list
  /* ----------------------------------------------------------------------- */
  void ProcValNoSuf(ostringstream &osS, const StrListConstIt &vIt,
    const Head &ttchD) { osS << ttchD.jFunc << setw(ttchD.iMaxLen) <<
      std::move(*vIt); }
  void ProcValSuf(ostringstream &osS, const StrListConstIt &vIt,
    const Head &ttchD, const string &strSuffix) { osS << ttchD.jFunc <<
      setw(ttchD.iMaxLen) << std::move(*vIt) << strSuffix; }
  /* ----------------------------------------------------------------------- */
  void ProcHdrNoSuf(ostringstream &osS,
    const Head &ttchD) { osS << ttchD.jFunc << setw(ttchD.iMaxLen) <<
      std::move(ttchD.strName); }
  void ProcHdrSuf(ostringstream &osS,
    const Head &ttchD, const string &strSuffix) { osS << ttchD.jFunc <<
      setw(ttchD.iMaxLen) << std::move(ttchD.strName) << strSuffix; }
  /* ----------------------------------------------------------------------- */
  Head &GetLastHdr(const size_t stHM1)
  { // Get the last header item
    Head &ttchD = hdHeaders[stHM1];
    // Set the length to zero if left align is selected
    if(ttchD.jFunc == left) ttchD.iMaxLen = 0;
    // Return the iterator
    return ttchD;
  }
  /* -- Finish with existing string stream ------------------------- */ public:
  void Finish(ostringstream &osS, const bool bLF=true, const size_t stG=1)
  { // Return if there are no headers.
    if(hdHeaders.empty()) return;
    // Get headers size minus one
    const size_t stHM1 = hdHeaders.size() - 1;
    // Create string for gap
    const string strGap(stG, ' '), strLF{ "\n" };
    // Proc headers except the last header item
    for(size_t stHIndex = 0; stHIndex < stHM1; ++stHIndex)
      ProcHdrSuf(osS, hdHeaders[stHIndex], strGap);
    // We're on the last header cell and no values? so we're done
    if(slValues.empty())
    { // Write last item with line-feed?
      if(bLF) ProcHdrSuf(osS, GetLastHdr(stHM1), strLF);
      // Write last item without line-feed?
      else ProcHdrNoSuf(osS, GetLastHdr(stHM1));
    } // We have values?
    else
    { // Write the last item with a line feed
      ProcHdrSuf(osS, GetLastHdr(stHM1), strLF);
      // Fill in rest of missing header columns with blanks. This is so we
      // don't need to add extra condition checks which would increase
      // processing time.
      while(slValues.size() % hdHeaders.size()) slValues.push_back({});
      // Get last iterator and iterator to first item
      const StrListConstIt vLast{ prev(slValues.cend()) };
      StrListIt vIt{ slValues.begin() };
      // Get total row count and if we have more than zero rows to print?
      if(size_t stRows = slValues.size() / hdHeaders.size())
      { // Get last iterator of the penultimate row
        const StrListConstIt vLastRow{
          prev(slValues.cend(), static_cast<ssize_t>(hdHeaders.size())) };
        // If we have more than 1 row? Repeat...
        if(stRows > 1) do
        { // Proc headers except the last header item
          for(size_t stHIndex = 0; stHIndex < stHM1; ++stHIndex, ++vIt)
            ProcValSuf(osS, vIt, hdHeaders[stHIndex], strGap);
          // Add the last item in row
          ProcValSuf(osS, vIt, hdHeaders[stHM1], strLF);
        } // ...Until we are at the last row and first header in the last
        while(++vIt != vLastRow);
        // Proc headers on the last row except the last header item
        for(size_t stHIndex = 0; stHIndex < stHM1; ++stHIndex, ++vIt)
          ProcValSuf(osS, vIt, hdHeaders[stHIndex], strGap);
      } // Proc the last item with carriage return if reuqested
      if(bLF) ProcValSuf(osS, vLast, hdHeaders[stHM1], strLF);
      // Not requested so process the last item without a carriage return
      else ProcValNoSuf(osS, vLast, hdHeaders[stHM1]);
      // Clear values
      slValues.clear();
    } // Clear headers
    hdHeaders.clear();
  }
  /* -- Finish with new string stream -------------------------------------- */
  const string Finish(const bool bLF=true, const size_t stG=1)
  { // Output stream
    ostringstream osS;
    // Do the format
    Finish(osS, bLF, stG);
    // Return the string
    return osS.str();
  }
  /* ----------------------------------------------------------------------- */
  template<typename T>Statistic &DataN(const T tNum, const int iP=0)
    { return Data(std::move(ToString<T>(tNum, 0, iP))); }
  /* ----------------------------------------------------------------------- */
  template<typename T>Statistic &DataB(const T tNum, const int iP=0)
    { return Data(ToBytesStr(static_cast<uint64_t>(tNum), iP)); }
  /* ----------------------------------------------------------------------- */
  template<typename T>Statistic &DataH(const T tNum,
    const int iP=0) { return Data(std::move(ToHex<T>(tNum, iP))); }
  /* -- Data by character -------------------------------------------------- */
  Statistic &DataC(const char cCharacter = '?')
  { // Return if there are no headers
    if(hdHeaders.empty()) return *this;
    // Get pointer to header data
    Head &ttchD = hdHeaders[slValues.size() % hdHeaders.size()];
    // Copy the character into the last slot
    slValues.insert(slValues.cend(), { &cCharacter, 1 });
    // Initialise maximum length if not set
    if(ttchD.iMaxLen <= 0) ttchD.iMaxLen = 1;
    // Return self so we can daisy chain
    return *this;
  }
  /* -- Data by read-only lvalue string copy ------------------------------- */
  Statistic &Data(const string &strVal={})
  { // Return if there are no headers
    if(hdHeaders.empty()) return *this;
    // Get pointer to header data
    Head &ttchD = hdHeaders[slValues.size() % hdHeaders.size()];
    // Copy the value into the last and put the string we inserted into a
    // decoder and get length of the utf8 string
    const int iLength = IntOrMax<int>(
      Decoder{ *slValues.insert(slValues.cend(), strVal) }.Length());
    // If the length of this value is longer and is not the last header value
    // then set the header longer
    if(iLength > ttchD.iMaxLen) ttchD.iMaxLen = iLength;
    // Return self so we can daisy chain
    return *this;
  }
  /* -- Data by rvalue string move ----------------------------------------- */
  Statistic &Data(string &&strVal)
  { // Return if there are no headers
    if(hdHeaders.empty()) return *this;
    // Get pointer to header data
    Head &ttchD = hdHeaders[slValues.size() % hdHeaders.size()];
    // Move the value into the list and put the tring we inserted into
    // a decoder and get length of the utf8 string
    const int iLength = IntOrMax<int>(
      Decoder(*slValues.insert(slValues.cend(), std::move(strVal))).Length());
    // If the length of this value is longer and is not the last header value
    // then set the header longer
    if(iLength > ttchD.iMaxLen) ttchD.iMaxLen = iLength;
    // Return self so we can daisy chain
    return *this;
  }
  /* -- Data by read-only lvalue widestring copy --------------------------- */
  Statistic &DataW(const wstring &wstrVal={})
    { return Data(FromWideStringPtr(wstrVal.c_str())); }
  /* ----------------------------------------------------------------------- */
  Statistic &DupeHeader(const size_t stCount=1)
  { // Have headers? Duplicate the headers the specified number of times
    if(!hdHeaders.empty())
      for(size_t stIndex = 0, stHdrCount = hdHeaders.size();
                 stIndex < stCount;
               ++stIndex)
        for(size_t stHdrIndex = 0; stHdrIndex < stHdrCount; ++stHdrIndex)
          hdHeaders.emplace_back(hdHeaders[stHdrIndex]);
    // Return self so we can daisy chain
    return *this;
  }
  /* ----------------------------------------------------------------------- */
  Statistic &Header(const string &strH, const bool bRJ, const size_t stL=0)
  { // Push the header item if there are values as this will mess everything
    // up. Make sure the first column is always left justified.
    if(slValues.empty())
      hdHeaders.push_back({ strH, bRJ ? right : left,
        IntOrMax<int>(Maximum(stL, strH.length())) });
    // Return self so we can daisy chain
    return *this;
  }
  /* ----------------------------------------------------------------------- */
  Statistic &Header(const string &strH={}, const size_t stL=0)
    { return Header(strH, !hdHeaders.empty(), stL); }
  /* -- Data by pointer ---------------------------------------------------- */
  Statistic &DataV(const void*const vpAddr) { return Data(Append(vpAddr)); }
  /* ----------------------------------------------------------------------- */
  Statistic(void) { }
};/* ----------------------------------------------------------------------- */
/* == Stat object collector and object class =============================== */
BEGIN_COLLECTORDUO(Stats, Stat, CLHelperUnsafe, ICHelperUnsafe),
  /* -- Base classes ------------------------------------------------------- */
  public Lockable,                     // Lua garbage collect instruction
  public Ident,                        // Identifier
  public Statistic                     // Statistics data
{ /* -- Constructor ------------------------------------------------ */ public:
  Stat(void) :                         // No parameters
    /* -- Initialisation of members ---------------------------------------- */
    ICHelperStat{ *cStats, this },     // Automatic (de)registration
    IdentCSlave{ cParent.CtrNext() }   // Initialise identification number
    /* -- No code ---------------------------------------------------------- */
    { }                                // Do nothing else
  /* ----------------------------------------------------------------------- */
  DELETECOPYCTORS(Stat);               // No copy constructors
};/* ----------------------------------------------------------------------- */
END_COLLECTOR(Stats);                  // End of stat objects collector
/* ------------------------------------------------------------------------- */
};                                     // End of module namespace
/* == EoF =========================================================== EoF == */

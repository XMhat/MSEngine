/* == URL.HPP ============================================================== */
/* ######################################################################### */
/* ## MS-ENGINE              Copyright (c) MS-Design, All Rights Reserved ## */
/* ######################################################################### */
/* ## A parser for Uniform Resource Locator strings.                      ## */
/* ######################################################################### */
/* ========================================================================= */
#pragma once                           // Only one incursion allowed
/* ------------------------------------------------------------------------- */
namespace IfUrl {                      // Start of module namespace
/* -- Includes ------------------------------------------------------------- */
using namespace IfString;              // Using string namespace
using namespace IfToken;               // Using token namespace
/* == Class to break apart urls ============================================ */
struct Url                             // Members initially public
{ /* -- Public typedefs ---------------------------------------------------- */
  enum Result                          // Result codes
  {/* ---------------------------------------------------------------------- */
    R_GOOD,                            // Url is good
    R_EMPTYURL,                        // Empty URL specified
    R_NOPROTO,                         // No protocol delimiter ':'
    R_INVPROTO,                        // Invalid protocol (not ://)
    R_EMPROTO,                         // Empty protocol after processing
    R_EMHOST,                          // Empty hostname
    R_EMPORT,                          // Empty port number
    R_INVPORT,                         // Invalid port number (1-65535)
    R_UNKPROTOPORT,                    // Unknown protocol with portr specified
    R_EMHOSTPORT,                      // Empty hostname after port parsed
    R_UNKPROTO,                        // Unknown protocol without port
    R_MAX                              // Maximum number of codes
  };/* -- Private variables --------------------------------------- */ private:
  Result           rResult;            // Result
  string           strProtocol;        // 'http' or 'https'
  string           strHost;            // Hostname
  string           strResource;        // The request uri
  string           strBookmark;        // Bookmark
  unsigned int     uiPort;             // Port number (1-65535)
  bool             bSecure;            // Connection would require SSL?
  /* -- Return data ------------------------------------------------ */ public:
  Result GetResult(void) const { return rResult; }
  const string &GetProtocol(void) const { return strProtocol; }
  const string &GetHost(void) const { return strHost; }
  const string &GetResource(void) const { return strResource; }
  const string &GetBookmark(void) const { return strBookmark; }
  unsigned int GetPort(void) const { return uiPort; }
  bool GetSecure(void) const { return bSecure; }
  /* -- Constructor -------------------------------------------------------- */
  explicit Url(const string &strUrl)
  { // Error if url is empty
    if(strUrl.empty()) { rResult = R_EMPTYURL; return; }
    // Find protocol and throw if error
    const size_t stProtPos = strUrl.find(':');
    if(stProtPos == string::npos) { rResult = R_NOPROTO; return; }
    for(size_t stPos = stProtPos + 1, stPosEnd = stPos + 1;
               stPos < stPosEnd;
             ++stPos)
      if(strUrl[stPos] != '/') { rResult = R_INVPROTO; return; }
    // Copy the protocol part and throw error if empty string
    strProtocol = strUrl.substr(0, stProtPos);
    if(strProtocol.empty()) { rResult = R_EMPROTO; return; }
    // Find hostname and if we couldn't find it?
    const size_t stUrlStartPos = stProtPos + 3;
    const size_t stHostPos = strUrl.find('/', stUrlStartPos);
    if(stHostPos == string::npos)
    { // Set default resource to root
      strResource = '/';
      // Finalise hostname
      strHost = strUrl.substr(stUrlStartPos);
    } // Request not found
    else
    { // Copy request part of url
      strResource = strUrl.substr(stHostPos);
      // Copy host part of url
      strHost = strUrl.substr(stUrlStartPos, stHostPos-stUrlStartPos);
    } // Check hostname not empty
    if(strHost.empty()) { rResult = R_EMHOST; return; }
    // Find port in hostname
    const size_t stPortPos = strHost.find(':');
    if(stPortPos != string::npos)
    { // Get port part of string and throw error if empty string
      const string strPort{ strHost.substr(stPortPos + 1) };
      if(strPort.empty()) { rResult = R_EMPORT; return; }
      // Convert port to number and throw error if invalid
      uiPort = ToNumber<unsigned int>(strPort);
      if(!uiPort || uiPort > 65535) { rResult = R_INVPORT; return; }
      // Set insecure if http was requested
      if(strProtocol == "http") bSecure = false;
      // Set secure if https was requested
      else if(strProtocol == "https") bSecure = true;
      // Unknown protocol error
      else { rResult = R_UNKPROTOPORT; return; }
      // Copy host part without port
      strHost.resize(stPortPos);
      if(strHost.empty()) { rResult = R_EMHOSTPORT; return; }
    } // Port number not found?
    else
    { // We have to find the port so set insecure port 80 if scheme is http
      if(strProtocol == "http") { uiPort = 80; bSecure = false; }
      // Set 443 if protocol is secure https
      else if(strProtocol == "https") { uiPort = 443; bSecure = true; }
      // Unknown protocol error
      else { rResult = R_UNKPROTO; return; }
    } // Find bookmark and if there is one?
    const size_t stBookmarkPos = strResource.find('#');
    if(stBookmarkPos != string::npos)
    { // Copy in bookmark string
      strBookmark = strResource.substr(stBookmarkPos + 1);
      // Trim resource
      strResource.resize(stBookmarkPos);
    } // Good result
    rResult = R_GOOD;
  }
};/* -- End of module namespace -------------------------------------------- */
};                                     // End of module namespace
/* == EoF =========================================================== EoF == */

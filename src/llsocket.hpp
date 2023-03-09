/* == LLSOCKET.HPP ========================================================= */
/* ######################################################################### */
/* ## MS-ENGINE              Copyright (c) MS-Design, All Rights Reserved ## */
/* ######################################################################### */
/* ## Defines the 'Socket' namespace and methods for the guest to use in  ## */
/* ## Lua. This file is invoked by 'lualib.hpp'.                          ## */
/* ######################################################################### */
/* ------------------------------------------------------------------------- */
#pragma once                           // Only one incursion allowed
/* ========================================================================= */
/* ######################################################################### */
/* ========================================================================= */
// % Socket
/* ------------------------------------------------------------------------- */
// ! The socket class allows the programmer to connect to a remote network
// ! using OpenSSL encryption or unencrypted connection. There is also basic
// ! support for HTTP connections, or you can write your own routines for that
// ! or any other protocol in Lua!
/* ========================================================================= */
LLNAMESPACEBEGIN(Socket)               // Socket namespace
/* -- Includes ------------------------------------------------------------- */
using namespace IfSocket;              // Using socket interface
/* ========================================================================= */
/* ######################################################################### */
/* ========================================================================= */
// $ Socket:Disconnect
// > Socket:socket=The socket to disconnect.
// ? Disconnects the specified socket. All current asynchronous
// ? operations are cancelled so make sure they are finished before calling.
/* ------------------------------------------------------------------------- */
LLFUNC(Disconnect, LCGETPTR(1, Socket)->SendDisconnect());
/* ========================================================================= */
// $ Socket:GetError
// < Code:integer=The OpenSSL error code.
// ? Returns the current OpenSSL error code associated with the socket.
/* ------------------------------------------------------------------------- */
LLFUNCEX(GetError, 1, LCPUSHINT((LCGETPTR(1, Socket))->GetError()));
/* ========================================================================= */
// $ Socket:GetReason
// < Reason:string=Reason for last error
// ? Returns the current OpenSSL error string associated with the socket.
/* ------------------------------------------------------------------------- */
LLFUNCEX(GetReason, 1, LCPUSHXSTR(LCGETPTR(1, Socket)->GetErrorStrSafe()));
/* ========================================================================= */
// $ Socket:GetStatus
// < Status:integer=The current socket status.
// ? Returns the current flags of the socket (See GetStatusCodes()).
/* ------------------------------------------------------------------------- */
LLFUNCEX(GetStatus, 1, LCPUSHINT((LCGETPTR(1, Socket))->FlagGet()));
/* ========================================================================= */
// $ Socket:RecvQCount
// < Count:integer=The current number of packets waiting to be processed.
// ? Because socket operations are asynchronous, the caller needs to process
// ? the packets manually. This function shows how many 'read' packets are
// ? waiting to be processed.
/* ------------------------------------------------------------------------- */
LLFUNCEX(RecvQCount, 1, LCPUSHINT(LCGETPTR(1, Socket)->GetRXQCountSafe()));
/* ========================================================================= */
// $ Socket:SendQCount
// < Count:integer=The current number of packets waiting to be written.
// ? When a send operation is requested by the programmer, each packet is
// ? put in a queue and the worker thread dispatches these packets when it
// ? can. This functions shows how many packets are waiting to be 'written'.
/* ------------------------------------------------------------------------- */
LLFUNCEX(SendQCount, 1, LCPUSHINT(LCGETPTR(1, Socket)->GetTXQCountSafe()));
/* ========================================================================= */
// $ Socket:PopRecvQ
// < Data:Asset=An array of data at the front of the read queue.
// < Count:integer=The number of bytes removed from the read queue.
// ? Removes the packet from the front of the read queue. If used as a HTTP
// ? socket then this may contain return header information (use PopSendQT()
// ? instead).
/* ------------------------------------------------------------------------- */
LLFUNCEX(PopRecvQ, 2,
  LCPUSHNUM(LCGETPTR(1, Socket)->GetPacketRXSafe(*LCCLASSCREATE(Asset))));
/* ========================================================================= */
// $ Socket:PopSendQ
// < Data:Asset=The data packet at the front of the send queue.
// < Count:integer=The number of bytes removed from the queue.
// ? Removes the packet from the front of the write queue. If used as a HTTP
// ? socket then this may contain return header information (use PopSendQT()
// ? instead).
/* ------------------------------------------------------------------------- */
LLFUNCEX(PopSendQ, 2,
  LCPUSHNUM(LCGETPTR(1, Socket)->GetPacketTXSafe(*LCCLASSCREATE(Asset))));
/* ========================================================================= */
// $ Socket:PopSendQT
// < Data:Asset=An array of data that was waiting to be written to the queue.
// < Count:integer=The number of bytes removed from the queue.
// ? Removes all data from the pending write queue. If used as a HTTP socket
// ? then this may contain return header information (use PopSendQT() instead).
/* ------------------------------------------------------------------------- */
LLFUNCEX(PopSendQT, 1, LCGETPTR(1, Socket)->ToLuaTable(lS));
/* ========================================================================= */
// $ Socket:Write
// > Data:Asset=The data to write.
// ? Places the specified data packet into the sockets send queue. The worker
// ? thread will dispatch this data as soon as it can. The function returns
// ? immediately.
/* ------------------------------------------------------------------------- */
LLFUNC(Write, LCGETPTR(1, Socket)->SendSafe(*LCGETPTR(2, Asset)));
/* ========================================================================= */
// $ Socket:WriteString
// > Text:string=The data string to write.
// ? Places the specified string packet into the sockets send queue. The worker
// ? thread will dispatch this string as soon as it can. The function returns
// ? immediately.
/* ------------------------------------------------------------------------- */
LLFUNCEX(WriteString, 1,
  LCGETPTR(1, Socket)->SendStringSafe(LCGETCPPSTRING(2, "String")));
/* ========================================================================= */
// $ Socket:CompactRecvQ
// < Data:Asset=An array of data that was read from the queue.
// < Count:integer=The number of bytes removed from the read queue.
// ? This function collects all the data in the 'read' queue and moves it all
// ? into an array for you to manipulate as you desire.
/* ------------------------------------------------------------------------- */
LLFUNCEX(CompactRecvQ, 1,
  LCGETPTR(1, Socket)->CompactRXSafe(*LCCLASSCREATE(Asset)));
/* ========================================================================= */
// $ Socket:CompactSendQ
// < Data:Asset=An array of data that was waiting to be written to the queue.
// < Count:integer=The number of bytes removed from the queue.
// ? Removes all data from the pending write queue. If used as a HTTP socket
// ? then this may contain return header information (use PopSendQT() instead).
/* ------------------------------------------------------------------------- */
LLFUNCEX(CompactSendQ, 1,
  LCGETPTR(1, Socket)->CompactTXSafe(*LCCLASSCREATE(Asset)));
/* ========================================================================= */
// $ Socket:GetRXBytes
// < Total:integer=The number of bytes read from this socket.
// ? Returns the total number of bytes read from this socket.
/* ------------------------------------------------------------------------- */
LLFUNCEX(GetRXBytes, 1, LCPUSHINT(LCGETPTR(1, Socket)->GetRX()));
/* ========================================================================= */
// $ Socket:GetTXBytes
// < Total:integer=The number of bytes written to this socket.
// ? Returns the total number of bytes written to this socket.
/* ------------------------------------------------------------------------- */
LLFUNCEX(GetTXBytes, 1, LCPUSHINT(LCGETPTR(1, Socket)->GetTX()));
/* ========================================================================= */
// $ Socket:GetRXPackets
// < Total:integer=The number of packets read from this socket.
// ? Returns the total number of packets read from this socket.
/* ------------------------------------------------------------------------- */
LLFUNCEX(GetRXPackets, 1, LCPUSHINT(LCGETPTR(1, Socket)->GetRXpkt()));
/* ========================================================================= */
// $ Socket:GetTXPackets
// < Total:integer=The number of packets written to this socket.
// ? Returns the total number of packets written to this socket.
/* ------------------------------------------------------------------------- */
LLFUNCEX(GetTXPackets, 1, LCPUSHINT(LCGETPTR(1, Socket)->GetTXpkt()));
/* ========================================================================= */
// $ Socket:GetSecure
// < State:boolean=Encryption state.
// ? Returns if this socket was initialised using SSL.
/* ------------------------------------------------------------------------- */
LLFUNCEX(GetSecure, 1, LCPUSHBOOL(LCGETPTR(1, Socket)->IsSecure()));
/* ========================================================================= */
// $ Socket:GetAddress
// < Address:string=The address of the socket.
// ? Returns the hostname of the socket.
/* ------------------------------------------------------------------------- */
LLFUNCEX(GetAddress, 1, LCPUSHXSTR(LCGETPTR(1, Socket)->GetAddressSafe()));
/* ========================================================================= */
// $ Socket:GetCipher
// < Cipher:string=Cipher tokens
// ? Returns the current cipher used for this connection
/* ------------------------------------------------------------------------- */
LLFUNCEX(GetCipher, 1, LCPUSHXSTR(LCGETPTR(1, Socket)->GetCipherSafe()));
/* ========================================================================= */
// $ Socket:GetAddressEx
// < Address:string=The address and port of the socket in string format.
// ? Returns the hostname and port of the socket as a string.
/* ------------------------------------------------------------------------- */
LLFUNCEX(GetAddressEx, 1,
  LCPUSHXSTR(LCGETPTR(1, Socket)->GetAddressAndPortSafe()));
/* ========================================================================= */
// $ Socket:GetIPAddress
// < Address:string=The IP address of the socket.
// ? Returns the IP address of the socket. Only valid when the hostname has
// ? been resolved.
/* ------------------------------------------------------------------------- */
LLFUNCEX(GetIPAddress, 1, LCPUSHXSTR(LCGETPTR(1, Socket)->GetIPAddressSafe()));
/* ========================================================================= */
// $ Socket:GetIPAddressAndPortEx
// < Address:string=The IP address of the socket.
// ? Returns the IP address of the socket. Only valid when the hostname has
// ? been resolved.
/* ------------------------------------------------------------------------- */
LLFUNCEX(GetIPAddressEx, 1,
  LCPUSHXSTR(LCGETPTR(1, Socket)->GetIPAddressAndPortSafe()));
/* ========================================================================= */
// $ Socket:GetId
// < Id:integer=The id of the socket.
// ? Returns the unique id of the socket.
/* ------------------------------------------------------------------------- */
LLFUNCEX(GetId, 1, LCPUSHINT(LCGETPTR(1, Socket)->GetThreadId()));
/* ========================================================================= */
// $ Socket:Callback
// > Callback:function=The new callback function
// ? Sets a new callback function for the socket. This is useful - for example,
// ? if you have an addon system and you need to re-assign the newer callback
// ? so the older overwritten callback is no longer referenced.
/* ------------------------------------------------------------------------- */
LLFUNC(Callback, LCGETPTR(1, Socket)->SetNewCallback(lS));
/* ========================================================================= */
// $ Socket:TConnect
// < Time:number=The time the socket entered SS_CONNECTING state
// ? Returns the processor time that the socket entered the SS_CONNECTING
// ? state.
/* ------------------------------------------------------------------------- */
LLFUNCEX(TConnect, 1, LCPUSHNUM(cLog->
  CCDeltaToClampedDouble(LCGETPTR(1, Socket)->GetTConnect())));
/* ========================================================================= */
// $ Socket:TConnected
// < Time:number=The time the socket entered SS_CONNECTED state
// ? Returns the processor time that the socket entered the SS_CONNECTED
// ? state.
/* ------------------------------------------------------------------------- */
LLFUNCEX(TConnected, 1, LCPUSHNUM(cLog->
  CCDeltaToClampedDouble(LCGETPTR(1, Socket)->GetTConnected())));
/* ========================================================================= */
// $ Socket:TRead
// < Time:number=The time the socket last completed a recv() operation.
// ? Returns the processor time that the socket last completed a recv()
// ? receive operation.
/* ------------------------------------------------------------------------- */
LLFUNCEX(TRead, 1, LCPUSHNUM(cLog->
  CCDeltaToClampedDouble(LCGETPTR(1, Socket)->GetTRead())));
/* ========================================================================= */
// $ Socket:TWrite
// < Time:number=The time the socket last completed a send() operation.
// ? Returns the processor time that the socket last completed a send()
// ? send operation.
/* ------------------------------------------------------------------------- */
LLFUNCEX(TWrite, 1, LCPUSHNUM(cLog->
  CCDeltaToClampedDouble(LCGETPTR(1, Socket)->GetTWrite())));
/* ========================================================================= */
// $ Socket:TDisconnect
// < Time:number=The time the socket was closing.
// ? Returns the processor time that the socket initiated a close() operation.
/* ------------------------------------------------------------------------- */
LLFUNCEX(TDisconnect, 1, LCPUSHNUM(cLog->
  CCDeltaToClampedDouble(LCGETPTR(1, Socket)->GetTDisconnect())));
/* ========================================================================= */
// $ Socket:TDisconnected
// < Time:number=The time the socket was closed.
// ? Returns the processor time that the socket completed a close() operation.
/* ------------------------------------------------------------------------- */
LLFUNCEX(TDisconnected, 1, LCPUSHNUM(cLog->
  CCDeltaToClampedDouble(LCGETPTR(1, Socket)->GetTDisconnected())));
/* ========================================================================= */
// $ Socket:Destroy
// ? Aborts, disconnects and destroys the socket object and frees all the
// ? memory associated with it. The object will no longer be useable after
// ? this call and an error will be generated if accessed.
/* ------------------------------------------------------------------------- */
LLFUNC(Destroy, LCCLASSDESTROY(1, Socket));
/* ========================================================================= */
/* ######################################################################### */
/* ## Socket:* member functions structure                                 ## */
/* ######################################################################### */
/* ------------------------------------------------------------------------- */
LLRSMFBEGIN                            // Socket:* member functions begin
  LLRSFUNC(Callback),                  // Set new callback function
  LLRSFUNC(Destroy),                   // Destroy internal socket object
  LLRSFUNC(Disconnect),                // Disconnect socket
  LLRSFUNC(GetStatus),                 // Socket status
  LLRSFUNC(GetId),                     // Socket ID
  LLRSFUNC(GetIPAddress),              // Socket IP address
  LLRSFUNC(GetIPAddressEx),            // Socket IP address and port
  LLRSFUNC(GetAddress),                // Socket address
  LLRSFUNC(GetAddressEx),              // Socket address and port
  LLRSFUNC(GetCipher),                 // Socket cipher mode
  LLRSFUNC(GetError),                  // Socket error status
  LLRSFUNC(GetReason),                 // Socket error reason
  LLRSFUNC(GetSecure),                 // Encryption enabled?
  LLRSFUNC(GetRXBytes),                // Get bytes downloaded
  LLRSFUNC(GetTXBytes),                // Get bytes uploaded
  LLRSFUNC(GetRXPackets),              // Get packets downloaded
  LLRSFUNC(GetTXPackets),              // Get packets uploaded
  LLRSFUNC(CompactRecvQ),              // Compact recv queue
  LLRSFUNC(CompactSendQ),              // Compact send queue
  LLRSFUNC(RecvQCount),                // Socket recv queue count
  LLRSFUNC(SendQCount),                // Socket send queue count
  LLRSFUNC(PopRecvQ),                  // Get oldest stored packet
  LLRSFUNC(PopSendQ),                  // Get oldest stored packet
  LLRSFUNC(PopSendQT),                 //  " as key->value table
  LLRSFUNC(Write),                     // Write specified array
  LLRSFUNC(WriteString),               // Write specified string
  LLRSFUNC(TConnect),                  // Time socket connecting
  LLRSFUNC(TConnected),                // Time socket connected
  LLRSFUNC(TRead),                     // Time socket read from
  LLRSFUNC(TWrite),                    // Time socket written to
  LLRSFUNC(TDisconnect),               // Time socket disconnecting
  LLRSFUNC(TDisconnected),             // Time socket disconnected
LLRSEND                                // Socket:* member functions end
/* ========================================================================= */
// $ Socket.ValidAddress
// > Address:string=The address to check.
// < State:boolean=The address is valid or not.
// ? Returns if the specified hostname or IP address is valid.
/* ------------------------------------------------------------------------- */
LLFUNCEX(ValidAddress, 1,
  LCPUSHBOOL(Socket::ValidAddress(LCGETCPPSTRING(1, "String"))));
/* ========================================================================= */
// $ Socket.Create
// > Address:string=The network destination address to connect to.
// > Port:integer=The port to connect to (1-65535).
// > Cipher:string=Make SSL connection and try this cipher.
// > Delay:string=The worker thread throttling rate.
// > Error:function=Function to call when a critical error occurs
// > Success:function=Function to call when any other event occurs
// ? Makes a standard point-to-point connection to the specified address.
// ? The function returns immediately as all socket operations are
// ? asynchronous. If you need SSL, make sure you specify the cipher suite
// ? you want to use or blank if you don't want to use SSL. Since a worker
// ? thread is used for each socket, you might want to specify a delay to
// ? throttle the loop as all operations are non-blocking. A value of 0 will
// ? make the thread consume 100% of a CPU core so be careful.
/* ------------------------------------------------------------------------- */
LLFUNC(Create, LCCLASSCREATE(Socket)->Connect(lS));
/* ========================================================================= */
// $ Socket.CreateHTTP
// > Cipher:string=Make SSL connection and try this cipher.
// > Address:string=The network destination address to connect to.
// > Port:integer=The port to connect to (1-65535).
// > Request:string=The HTTP URI request resource.
// > Scheme:string=The HTTP scheme (GET, POST, PUT, DELETE, etc.).
// > Headers:string=A carriage return separated list of headers.
// > Body:string=A body of text to send with the request.
// > Error:function=Function to call when a critical error occurs
// > Success:function=Function to call when any other event occurs
// ? This is a minimalist implementation of a HTTP request and will support
// ? most (if not all) operations one would need. The function returns
// ? immediately. The worker thread uses blocking socket operations.
/* ------------------------------------------------------------------------- */
LLFUNC(CreateHTTP, LCCLASSCREATE(Socket)->HTTPRequest(lS));
/* ========================================================================= */
// $ Socket.Count
// < Count:integer=Total number of sockets created.
// ? Returns the total number of socket classes currently active.
/* ------------------------------------------------------------------------- */
LLFUNCEX(Count, 1, LCPUSHINT(cSockets->CollectorCount()));
/* ========================================================================= */
// $ Socket.Connected
// < Count:integer=Total number of sockets connected
// ? Returns the total number of socket classes currently connected.
/* ------------------------------------------------------------------------- */
LLFUNCEX(Connected, 1, LCPUSHINT(cSockets->stConnected.load()));
/* ========================================================================= */
// $ Socket.Flush
// < Count:integer=Total number of sockets disconnected.
// ? Disconnects all active Sockets. Returns immediately.
/* ------------------------------------------------------------------------- */
LLFUNCEX(Flush, 1, LCPUSHINT(SocketReset()));
/* ========================================================================= */
// $ Socket.SocketWaitAsync
// < Count:integer=Total number of sockets disconnected.
// ? Disconnects all active Sockets and waits until all sockets are closed.
/* ------------------------------------------------------------------------- */
LLFUNCEX(WaitAsync, 1, LCPUSHINT(SocketWaitAsync()));
/* ========================================================================= */
// $ Socket.TotalRXBytes
// < Total:integer=The number of bytes read from this socket.
// ? Returns the total number of bytes read from this socket.
/* ------------------------------------------------------------------------- */
LLFUNCEX(TotalRXBytes, 1, LCPUSHINT(cSockets->qRX.load()));
/* ========================================================================= */
// $ Socket.TotalTXBytes
// < Total:integer=The number of bytes written to this socket.
// ? Returns the total number of bytes written to this socket.
/* ------------------------------------------------------------------------- */
LLFUNCEX(TotalTXBytes, 1, LCPUSHINT(cSockets->qTX.load()));
/* ========================================================================= */
// $ Socket.TotalRXPackets
// < Total:integer=The total number of packets read from this socket.
// ? Returns the total number of packets read from this socket.
/* ------------------------------------------------------------------------- */
LLFUNCEX(TotalRXPackets, 1, LCPUSHINT(cSockets->qRXp.load()));
/* ========================================================================= */
// $ Socket.TotalTXPackets
// < Total:integer=The total number of packets written to all sockets.
// ? Returns the total number of packets written to all sockets.
/* ------------------------------------------------------------------------- */
LLFUNCEX(TotalTXPackets, 1, LCPUSHINT(cSockets->qTXp.load()));
/* ========================================================================= */
// $ Socket.OAuth11
// > Method:string=The HTTP method (GET, PUT, etc.).
// > Scheme:string=The HTTP scheme (HTTP, HTTPS, etc.);
// > Port:integer=The HTTP port connected.
// > Resource:string=The HTTP resource requested.
// > Params:string=A HTTP URI list of variables that will be sent.
// > Body:string=Any HTTP body text sent.
// < Text:string=An OAuth11 compatible string that you can pass to web request.
// ? Calculates a OAuth v1.1 string to use with HTTP requests. Twitter is an
// ? example that uses this system.
/* ------------------------------------------------------------------------- */
LLFUNCEX(OAuth11, 1, LCTOTABLE(SocketOAuth11(
  LCGETCPPSTRING(1, "Method"),   LCGETCPPSTRING(2, "Scheme"),
  LCGETCPPSTRING(3, "Address"),  LCGETCPPSTRING(4, "Port"),
  LCGETCPPSTRING(5, "Resource"), LCGETCPPSTRING(6, "Params"),
  LCGETCPPSTRING(7, "Body"))));
/* ========================================================================= */
/* ######################################################################### */
/* ## Socket.* namespace functions structure                              ## */
/* ######################################################################### */
/* ------------------------------------------------------------------------- */
LLRSBEGIN                              // Socket.* namespace functions begin
  LLRSFUNC(Create),                    // Connect tcp socket
  LLRSFUNC(CreateHTTP),                // Connect HTTP socket
  LLRSFUNC(Count),                     // Sockets created
  LLRSFUNC(Connected),                 // Sockets connected
  LLRSFUNC(Flush),                     // Flush all sockets
  LLRSFUNC(OAuth11),                   // Build a OAuth11 request
  LLRSFUNC(TotalRXBytes),              // Get total bytes downloaded
  LLRSFUNC(TotalTXBytes),              // Get total bytes uploaded
  LLRSFUNC(TotalRXPackets),            // Get total packets downloaded
  LLRSFUNC(TotalTXPackets),            // Get total packets uploaded
  LLRSFUNC(WaitAsync),                 // Flush all sockets and wait
  LLRSFUNC(ValidAddress),              // Check if address valid
LLRSEND                                // Socket.* namespace functions end
/* ========================================================================= */
/* ######################################################################### */
/* ## Socket.* namespace constants structure                              ## */
/* ######################################################################### */
/* ========================================================================= */
// @ Socket.Flags
// < Data:table=A list of socket status operations.
// ? Returns a key/value table of socket connction status codes supported.
/* ------------------------------------------------------------------------- */
LLRSKTBEGIN(Flags)                     // Beginning of socket status flags
LLRSKTITEM(SS_,STANDBY),               LLRSKTITEM(SS_,INITIALISING),
LLRSKTITEM(SS_,ENCRYPTION),            LLRSKTITEM(SS_,CONNECTING),
LLRSKTITEM(SS_,CONNECTED),             LLRSKTITEM(SS_,DISCONNECTING),
LLRSKTITEM(SS_,SENDREQUEST),           LLRSKTITEM(SS_,CLOSEDBYCLIENT),
LLRSKTITEM(SS_,CLOSEDBYSERVER),        LLRSKTITEM(SS_,REPLYWAIT),
LLRSKTITEM(SS_,DOWNLOADING),           LLRSKTITEM(SS_,EVENTERROR),
LLRSKTITEM(SS_,READPACKET),
LLRSKTEND                              // End of socket status flags
/* ========================================================================= */
LLRSCONSTBEGIN                         // Socket.* namespace consts begin
LLRSCONST(Flags),                      // Socket status flags
LLRSCONSTEND                           // Socket.* namespace consts end
/* ========================================================================= */
LLNAMESPACEEND                         // End of Socket namespace
/* == EoF =========================================================== EoF == */

/* == LLCREDIT.HPP ========================================================= */
/* ######################################################################### */
/* ## MS-ENGINE              Copyright (c) MS-Design, All Rights Reserved ## */
/* ######################################################################### */
/* ## Defines the 'Credit' namespace and methods for the guest to use in ## */
/* ## Lua. This file is invoked by 'lualib.hpp'.                          ## */
/* ######################################################################### */
/* ------------------------------------------------------------------------- */
#pragma once                           // Only one incursion allowed
/* ========================================================================= */
/* ######################################################################### */
/* ========================================================================= */
// % Credit
/* ------------------------------------------------------------------------- */
// ! Allows access to engine credits information.
/* ========================================================================= */
LLNAMESPACEBEGIN(Credit)               // Credit namespace
/* -- Includes ------------------------------------------------------------- */
using namespace IfCredit;              // Using credit namespace
/* ========================================================================= */
// $ Credit.Credit
// > Id:integer=The index of the licence.
// < Name:string=Name of the credit.
// < Version:string=Version of the api.
// < Id:integer=Licence index.
// < Website:string=Website of the api.
// < Copyright:string=Copyright of the api.
// < Author:string=Author of the api.
// ? Shows the full credits information of the specified api index.
/* ------------------------------------------------------------------------- */
LLFUNCBEGIN(Item)
  const CreditLib &libItem = CreditGetItem(
    LCGETINTLGE(CreditEnums, 1, CL_FIRST, CL_MAX, "Id"));
  LCPUSHXSTR(libItem.GetName());       LCPUSHXSTR(libItem.GetVersion());
  LCPUSHBOOL(libItem.IsCopyright());   LCPUSHXSTR(libItem.GetAuthor());
LLFUNCENDEX(4)
/* ========================================================================= */
// $ Credit.Licence
// > Id:integer=The index of the licence.
// < Name:string=Name of the licence.
// < Text:string=Full text file of the licence.
// ? Shows the full licence information of the specified index.
/* ------------------------------------------------------------------------- */
LLFUNCEX(Licence, 1, LCPUSHXSTR(CreditGetItemText(
  LCGETINTLGE(CreditEnums, 1, CL_FIRST, CL_MAX, "Id"))));
/* ========================================================================= */
// $ Credit.Total
// < Total:integer=Number of credits used in the executable.
// ? Returns the number of API's stored in the executable.
/* ------------------------------------------------------------------------- */
LLFUNCEX(Total, 1, LCPUSHINT(CreditGetItemCount()));
/* ========================================================================= */
/* ######################################################################### */
/* ## Credit.* namespace functions structure                              ## */
/* ######################################################################### */
/* ------------------------------------------------------------------------- */
LLRSBEGIN                              // Credit.* namespace functions begin
  LLRSFUNC(Item),                      // Return specific credit information
  LLRSFUNC(Licence),                   // Return specific licence information
  LLRSFUNC(Total),                     // Return total credits
LLRSEND                                // Credit.* namespace functions end
/* ========================================================================= */
LLNAMESPACEEND                         // End of Credit namespace
/* == EoF =========================================================== EoF == */

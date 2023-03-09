/* == LLCREDIT.HPP ========================================================= **
** ######################################################################### **
** ## MS-ENGINE              Copyright (c) MS-Design, All Rights Reserved ## **
** ######################################################################### **
** ## Defines the 'Credit' namespace and methods for the guest to use in  ## **
** ## Lua. This file is invoked by 'lualib.hpp'.                          ## **
** ######################################################################### **
** ------------------------------------------------------------------------- */
#pragma once                           // Only one incursion allowed
/* ========================================================================= **
** ######################################################################### **
** ========================================================================= */
// % Credit
/* ------------------------------------------------------------------------- */
// ! Allows access to engine credits information.
/* ========================================================================= */
namespace LLCredit {                   // Credit namespace
/* -- Dependencies --------------------------------------------------------- */
using namespace ICredit::P;
/* ========================================================================= **
** ######################################################################### **
** ## Credit.* namespace functions                                        ## **
** ######################################################################### **
** ========================================================================= */
// $ Credit.Credit
// > Id:integer=The index of the license.
// < Name:string=Name of the credit.
// < Version:string=Version of the api.
// < Id:integer=License index.
// < Website:string=Website of the api.
// < Copyright:string=Copyright of the api.
// < Author:string=Author of the api.
// ? Shows the full credits information of the specified api index.
/* ------------------------------------------------------------------------- */
LLFUNCBEGIN(Item)
  const CreditLib &libItem = cCredits->CreditGetItem(
    LCGETINTLGE(CreditEnums, 1, CL_FIRST, CL_MAX, "Id"));
  LCPUSHVAR(libItem.GetName(),     libItem.GetVersion(),
            libItem.IsCopyright(), libItem.GetAuthor());
LLFUNCENDEX(4)
/* ========================================================================= */
// $ Credit.License
// > Id:integer=The index of the license.
// < Name:string=Name of the license.
// < Text:string=Full text file of the license.
// ? Shows the full license information of the specified index.
/* ------------------------------------------------------------------------- */
LLFUNCEX(License, 1, LCPUSHVAR(cCredits->CreditGetItemText(
  LCGETINTLGE(CreditEnums, 1, CL_FIRST, CL_MAX, "Id"))));
/* ========================================================================= */
// $ Credit.Total
// < Total:integer=Number of credits used in the executable.
// ? Returns the number of API's stored in the executable.
/* ------------------------------------------------------------------------- */
LLFUNCEX(Total, 1, LCPUSHVAR(cCredits->CreditGetItemCount()));
/* ========================================================================= **
** ######################################################################### **
** ## Credit.* namespace functions structure                              ## **
** ######################################################################### **
** ------------------------------------------------------------------------- */
LLRSBEGIN                              // Credit.* namespace functions begin
  LLRSFUNC(Item), LLRSFUNC(License), LLRSFUNC(Total),
LLRSEND                                // Credit.* namespace functions end
/* ========================================================================= */
}                                      // End of Credit namespace
/* == EoF =========================================================== EoF == */

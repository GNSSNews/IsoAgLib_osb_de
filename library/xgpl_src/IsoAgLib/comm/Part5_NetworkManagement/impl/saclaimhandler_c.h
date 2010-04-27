/*
  saclaimhandler_c.h

  (C) Copyright 2009 - 2010 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/
#ifndef SA_CLAIM_HANDLER_C_H
#define SA_CLAIM_HANDLER_C_H

#include <IsoAgLib/hal/hal_typedef.h>
#include "isorequestpgnhandler_c.h"

/* *************************************** */
/* ********** include headers ************ */
/* *************************************** */

namespace __IsoAgLib {



/** Handler class which can be used to react on all actions depending on address-claims.
 *  Actions that can take place are listed in the class-embdedded enum.
 */
class SaClaimHandler_c
{
public:

  typedef enum IsoItemAction_en
  { /// ATTENTION: If a node is added and "AddToMonitorList" is broadcast,
    /// ~~~~~~~~~  the node doesn't need to have a valid Address at this moment!!
    AddToMonitorList,        /// Initial Address-Claim --> Node added to IsoMonitor
    ChangedAddress,          /// Changed Address
    LostAddress,             /// Address stolen due to Address-Claim of other node with Same SA (New Address is 254)
    ReclaimedAddress,        /// Reclaimed an Address after Address was stolen (Old Address was 254)
    RemoveFromMonitorList    /// No more Address-Claims (after requests) --> Node removed from IsoMonitor
  } IsoItemModification_t;

   SaClaimHandler_c() {}
   virtual ~SaClaimHandler_c() {}

   /** this function is called by IsoMonitor_c on addition, state-change and removal of an IsoItem.
     <!-- @param at_action enumeration indicating what happened to this IsoItem. @see IsoItemModification_en / IsoItemModification_t
      @param acrc_isoItem reference to the (const) IsoItem which is changed (by existance or state)-->
     */
  virtual void reactOnIsoItemModification(
      IsoItemModification_t at_action,
      IsoItem_c const &acrc_isoItem) = 0;

  void reactOnIsoItemModificationDefault(
      IsoItemModification_t /*at_action*/,
      IsoItem_c const &/*acrc_isoItem*/)
  {}
};

/** Proxy for SaClaimHandler_c.
  * Having such a proxy as component is an alternative to subclassing
  * SaClaimHandler_c directly.
  */
template < typename OWNER_T >
class SaClaimHandlerProxy_c : public SaClaimHandler_c {
public:
  typedef OWNER_T Owner_t;

  SaClaimHandlerProxy_c(Owner_t &art_owner) : mrt_owner(art_owner) {}

  virtual ~SaClaimHandlerProxy_c() {}

private:
  virtual void reactOnIsoItemModification(
      IsoItemModification_t at_action,
      IsoItem_c const &acrc_isoItem)
  {
    mrt_owner.reactOnIsoItemModification(at_action, acrc_isoItem);
  }

  // SaClaimHandlerProxy_c shall not be copyable. Otherwise the
  // reference to the containing object would become invalid.
  SaClaimHandlerProxy_c(SaClaimHandlerProxy_c const &);

  SaClaimHandlerProxy_c &operator=(SaClaimHandlerProxy_c const &);

  Owner_t &mrt_owner;
};

}
#endif

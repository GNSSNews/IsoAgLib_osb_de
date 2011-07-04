/*
  istate_c.cpp: object which manages state of item in monitor list

  (C) Copyright 2009 - 2011 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/
#include "istate_c.h"


/**
  constructor of IState_c which can set the state to given initial value
  @param ren_itemState optional wanted state information (default \<empty\> value)
*/
__IsoAgLib::IState_c::IState_c(itemState_t ren_itemState, int ai_multitonInst)
	: ClientBase( ai_multitonInst )
{
  en_itemState = ren_itemState;
}

/**
  constructor of IState_c which can set the state to given initial value
  @param aui8_state optional wanted state information (default \<empty\> value)
*/
__IsoAgLib::IState_c::IState_c(uint8_t aui8_state, int ai_multitonInst )
	: ClientBase( ai_multitonInst )
{
  en_itemState = itemState_t(aui8_state);
}

/**
  copy constructor for IState_c
  @param acrc_src source for this new instance
*/
__IsoAgLib::IState_c::IState_c(const __IsoAgLib::IState_c& acrc_src)
	: ClientBase( acrc_src )
{
  en_itemState = acrc_src.en_itemState;
}



/**
  set the state of an monitor list item and
  return the resulting state value

  set state to Active, and reset OffExplicitly and OffUnable on
  Active, PreAddressClaim, AddressClaim, ClaimedAddress;

  set: PreAddressClaim, AddressClaim and ClaimedAddress exclusive

  set: OffExplicitly, OffUnable, Active exclusive
  @param ren_itemState state information
  @param ab_clearOld optional clear old value for complete new set (default no clear)
  @return resulting state information
*/
__IsoAgLib::IState_c::itemState_t __IsoAgLib::IState_c::setItemState(itemState_t ren_itemState, bool ab_clearOld)
{
  if (ab_clearOld) en_itemState = ren_itemState;
  // set state to Active, and reset OffExplicitly and OffUnable on
  // Active, PreAddressClaim, AddressClaim, ClaimedAddress
  if ((ren_itemState & (Active|PreAddressClaim|AddressClaim|ClaimedAddress|AddressLost)) > 0)
  { // one of these states set -> clear OffExplicitly and OffUnable
    clearItemState(itemState_t(OffExplicitly|OffUnable));
    // set PreAddressClaim, AddressClaim, ClaimedAddress and AddressLost exclusive
    if ((ren_itemState & (PreAddressClaim|AddressClaim|ClaimedAddress|AddressLost)) > 0)
    { // one of PreAddressClaim, AddressClaim, ClaimedAddress and AddressLost exclusive
      // clear before set
      clearItemState(itemState_t(PreAddressClaim|AddressClaim|ClaimedAddress|AddressLost|PossiblyOffline));
    }
    // now set always additionally the Active flag
    en_itemState = itemState_t(en_itemState | Active);
  }

  // set: OffExplicitly, OffUnable, Active exclusive
  if ((ren_itemState & (OffExplicitly|OffUnable)) > 0)
  { // one of OffExplicitly, OffUnable, Active (handled above)
    // clear: OffExplicitly, OffUnable, Active
    clearItemState(itemState_t(OffExplicitly|OffUnable|Active));
  }

  // now simple set the new value
  en_itemState = itemState_t(en_itemState | ren_itemState);

  return en_itemState;
};



/* ********************* */
/* methods of IStateExt_c */
/* ********************* */

/**
  constructor of IStateExt_c which can set the state to given initial value
  @param ren_itemState optional wanted state information (default \<empty\> value)
*/
__IsoAgLib::IStateExt_c::IStateExt_c(itemState_t ren_itemState, int ai_multitonInst)  : __IsoAgLib::IState_c(ren_itemState, ai_multitonInst)
{
  counter.b_addressClaimCnt = counter.b_causedConflictCnt = counter.b_affectedConflictCnt = 0;
  mi16_lastCausedConflictTime = mi16_lastAffectedConflictTime = 0;
};

/**
  constructor of IState_c which can set the state to given initial value
  @param aui8_itemState optional wanted state information (default \<empty\> value)
*/
__IsoAgLib::IStateExt_c::IStateExt_c(uint8_t aui8_itemState, int ai_multitonInst) : __IsoAgLib::IState_c(aui8_itemState, ai_multitonInst)
{
  counter.b_addressClaimCnt = counter.b_causedConflictCnt = counter.b_affectedConflictCnt = 0;
  mi16_lastCausedConflictTime = mi16_lastAffectedConflictTime = 0;
}

/**
  copy constructor for IState_c
  @param acrc_src source for this new instance
*/
__IsoAgLib::IStateExt_c::IStateExt_c(const IStateExt_c& acrc_src) : __IsoAgLib::IState_c(acrc_src)
{
  counter.b_addressClaimCnt = acrc_src.counter.b_addressClaimCnt;
  counter.b_causedConflictCnt = acrc_src.counter.b_causedConflictCnt;
  counter.b_affectedConflictCnt = acrc_src.counter.b_affectedConflictCnt;
  mi16_lastCausedConflictTime = acrc_src.mi16_lastCausedConflictTime;
  mi16_lastAffectedConflictTime = acrc_src.mi16_lastAffectedConflictTime;
}

/**
  set and/or retreive the AddressClaim counter
  @param ac_cnt optional new AddressClaim counter  (default only Request)
  @return actual or resulting AddressClaim cnt
*/
uint8_t __IsoAgLib::IStateExt_c::addressClaimCnt(int8_t ac_cnt)
{
  if (ac_cnt > Request)
  {
    setItemState(__IsoAgLib::IState_c::AddressClaim);
    counter.b_addressClaimCnt = ac_cnt;
  }
  return (itemState(__IsoAgLib::IState_c::AddressClaim))?counter.b_addressClaimCnt:0;
};

/**
  set and/or retreive the counter of false alive msgs
  @param ac_cnt optional new false alive counter  (default only Request)
  @return actual or resulting false alive cnt
*/
uint8_t __IsoAgLib::IStateExt_c::causedConflictCnt(int8_t ac_cnt, int32_t ai32_time)
{
  switch (ac_cnt)
  {
    case Incr: // -2 --> new conflict
      mi16_lastCausedConflictTime = (ai32_time / 1000);
      if (counter.b_causedConflictCnt < 0xF) counter.b_causedConflictCnt++;
      break;
    case Decr: // -3 --> timeEvent without conflict -> test if cnt can be reduced
       if (counter.b_causedConflictCnt > 0)
       {
        if ( (ai32_time / 1000) != mi16_lastCausedConflictTime)
        { // last conflict over 1 sec. away
          counter.b_causedConflictCnt--;
        }
      }
      break;
    case Request: // -1 --> only answer cnt
      break;
    default: // new dircet given value >= 0
      if (ac_cnt > 0) setItemState(__IsoAgLib::IState_c::CausedConflict);
      else clearItemState(__IsoAgLib::IState_c::CausedConflict);
      if ((ac_cnt >= 0) && (ac_cnt < 0xF)) counter.b_causedConflictCnt = ac_cnt;
      break;
  }

  return (itemState(__IsoAgLib::IState_c::CausedConflict))?counter.b_causedConflictCnt:0;
};

/**
  set and/or retreive the counter of false alive msgs
  @param ac_cnt optional new false alive counter  (default only Request)
  @return actual or resulting false alive cnt
*/
uint8_t __IsoAgLib::IStateExt_c::affectedConflictCnt(int8_t ac_cnt, int32_t ai32_time)
{
  switch (ac_cnt)
  {
    case Incr: // -2 --> new conflict
      mi16_lastAffectedConflictTime = (ai32_time / 1000);
      if (counter.b_affectedConflictCnt < 0xF) counter.b_affectedConflictCnt++;
      break;
    case Decr: // -3 --> timeEvent without conflict -> test if cnt can be reduced
       if (counter.b_affectedConflictCnt > 0)
       {
        if ( (ai32_time / 1000) != mi16_lastAffectedConflictTime)
        { // last conflict over 1 sec. away
          counter.b_affectedConflictCnt--;
        }
      }
      break;
    case Request: // -1 --> only answer cnt
      break;
    default: // new dircet given value >= 0
      if (ac_cnt > 0) setItemState(__IsoAgLib::IState_c::AffectedConflict);
      else clearItemState(__IsoAgLib::IState_c::AffectedConflict);
      if ((ac_cnt >= 0) && (ac_cnt < 0xF)) counter.b_affectedConflictCnt = ac_cnt;
      break;
  }

  return (itemState(__IsoAgLib::IState_c::AffectedConflict))?counter.b_affectedConflictCnt:0;
};

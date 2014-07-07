/*
  system.h: definition of Hardware Abstraction Layer for central
    system functions for debug/simulation system on C2C

  (C) Copyright 2009 - 2014 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/

/** \file IsoAgLib/hal/c2c/system/system.h
 * The header <i>\<target\>/\<device\>/\<device\>.h</i> performs a name
   mapping between platform specific BIOS / OS function names
   and the function names, the IsoAgLib uses for hardware access.
   In this header the groups for error codes, global system
   functions and CAN must be adapted to a new platform.
   If a simple name mapping is not possible for a specific
   function, the module targetExtensions can be used to implement
   the needed activity.
*/

#ifndef _HAL_C2C_SYSTEM_H_
#define _HAL_C2C_SYSTEM_H_

#include "system_target_extensions.h"

#include <IsoAgLib/util/impl/util_funcs.h>


namespace HAL
{
  inline void openSystem() { __HAL::openSystem(); }

  inline void closeSystem() { __HAL::closeSystem(); }

  inline bool isSystemOpened() { return __HAL::isSystemOpened(); }

  inline void initWatchdog( void* ) {}

  inline void closeWatchdog() {}

  inline void triggerWatchdog() { __HAL::trigger_wd(); }

  inline int32_t getTime() { return __HAL::get_time(); }

  inline int16_t getSnr(uint8_t *snrDat) { return __HAL::get_snr(snrDat); }

  inline int32_t getSerialNr(int16_t* pi16_errCode)
  {
    uint8_t uint8 [6];
    int16_t errCode = __HAL::get_snr(uint8);
    if (pi16_errCode) *pi16_errCode = errCode;
    // Serial number is coded in BCD. As we only get 21 bits,
    // we can take only a part of the information transmitted here.
    //  - uint8[0] is the year of construction -> 7 bits
    //  - uint8[2] and uint8[3] a contract numering -> 14 bits
    return (__IsoAgLib::bcd2dec(uint8[2]) * 100 + __IsoAgLib::bcd2dec(uint8[3])) + (__IsoAgLib::bcd2dec(uint8[0]) << 14);
  };

  inline int16_t getAdcUbat()
  {
	return (40 * __HAL::get_adc(GET_U_E));
  }

  inline int16_t getAdc_u85() { return 8500; }

  inline bool getOn_offSwitch() { return ( ON == __HAL::get_on_off_switch() ); }

  inline void powerHold( bool ab_on ) { ab_on ? __HAL::staying_alive() : (void)__HAL::power_down(); }

  inline void delay_us(unsigned int i_tm) { __HAL::delay_us(i_tm); }
   
  inline void sleep_max_ms( uint32_t ms ) { __HAL::delay_us( ms * 1000 ); }
}


#ifdef USE_MUTUAL_EXCLUSION
#include <IsoAgLib/hal/generic_utils/system/mutex_emulated.h>
#endif


#endif

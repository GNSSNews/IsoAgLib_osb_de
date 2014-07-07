/*
  inputs_target_extensions.h:
    header for ESXu specific extensions for the HAL for input

  (C) Copyright 2009 - 2014 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/

#ifndef _HAL_ESXu_INPUTS_TARGET_EXTENSIONS_H_
#define _HAL_ESXu_INPUTS_TARGET_EXTENSIONS_H_

namespace __HAL {
  extern "C" {
    /** include the BIOS specific header into __HAL */
    #include <commercial_BIOS/bios_esxu/mos10osy.h>
  }
}

#include <IsoAgLib/isoaglib_config.h>
#include <IsoAgLib/hal/esxu/typedef.h>
#include <IsoAgLib/hal/esxu/errcodes.h>

             
namespace __HAL {

/* ********************************************* */
/** \name specific BIOS Extenders                */
/*@{*/

int16_t init_counter(uint8_t ab_channel, uint16_t aui16_timebase, bool ab_activHigh, bool ab_risingEdge);
uint32_t getCounter(uint8_t ab_channel);
int16_t resetCounter(uint8_t ab_channel);
uint32_t getCounterPeriod_us(uint8_t ab_channel);
uint32_t getCounterFrequency(uint8_t ab_channel);
uint16_t getCounterLastSignalAge(uint8_t ab_channel);

/*@}*/

} // __HAL

#endif

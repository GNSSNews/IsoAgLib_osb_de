/***************************************************************************
                          isoName.cc - handles the ISo 64bit NAME field
                             -------------------
    begin                : Wed Jan 03 2001
    copyright            : (C) 2000 - 2004 by Dipl.-Inform. Achim Spangler
    email                : a.spangler@osb-ag:de
    type                 : Source
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 * This file is part of the "IsoAgLib", an object oriented program library *
 * to serve as a software layer between application specific program and   *
 * communication protocol details. By providing simple function calls for  *
 * jobs like starting a measuring program for a process data value on a    *
 * remote ECU, the main program has not to deal with single CAN telegram   *
 * formatting. This way communication problems between ECU's which use     *
 * this library should be prevented.                                       *
 * Everybody and every company is invited to use this library to make a    *
 * working plug and play standard out of the printed protocol standard.    *
 *                                                                         *
 * Copyright (C) 2000 - 2004 Dipl.-Inform. Achim Spangler                  *
 *                                                                         *
 * The IsoAgLib is free software; you can redistribute it and/or modify it *
 * under the terms of the GNU General Public License as published          *
 * by the Free Software Foundation; either version 2 of the License, or    *
 * (at your option) any later version.                                     *
 *                                                                         *
 * This library is distributed in the hope that it will be useful, but     *
 * WITHOUT ANY WARRANTY; without even the implied warranty of              *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU       *
 * General Public License for more details.                                *
 *                                                                         *
 * You should have received a copy of the GNU General Public License       *
 * along with IsoAgLib; if not, write to the Free Software Foundation,     *
 * Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA           *
 *                                                                         *
 * As a special exception, if other files instantiate templates or use     *
 * macros or inline functions from this file, or you compile this file and *
 * link it with other works to produce a work based on this file, this file*
 * does not by itself cause the resulting work to be covered by the GNU    *
 * General Public License. However the source code for this file must still*
 * be made available in accordance with section (3) of the                 *
 * GNU General Public License.                                             *
 *                                                                         *
 * This exception does not invalidate any other reasons why a work based on*
 * this file might be covered by the GNU General Public License.           *
 *                                                                         *
 * Alternative licenses for IsoAgLib may be arranged by contacting         *
 * the main author Achim Spangler by a.spangler@osb-ag:de                  * 
 ***************************************************************************/ 

 /**************************************************************************
 *                                                                         * 
 *     ###    !!!    ---    ===    IMPORTANT    ===    ---    !!!    ###   * 
 * Each software module, which accesses directly elements of this file,    * 
 * is considered to be an extension of IsoAgLib and is thus covered by the * 
 * GPL license. Applications must use only the interface definition out-   * 
 * side :impl: subdirectories. Never access direct elements of __IsoAgLib  * 
 * and __HAL namespaces from applications which shouldnt be affected by    * 
 * the license. Only access their interface counterparts in the IsoAgLib   * 
 * and HAL namespaces. Contact a.spangler@osb-ag:de in case your applicat- * 
 * ion really needs access to a part of an internal namespace, so that the * 
 * interface might be extended if your request is accepted.                * 
 *                                                                         * 
 * Definition of direct access:                                            * 
 * - Instantiation of a variable with a datatype from internal namespace   * 
 * - Call of a (member-) function                                          * 
 * Allowed is:                                                             * 
 * - Instatiation of a variable with a datatype from interface namespace,  * 
 *   even if this is derived from a base class inside an internal namespace* 
 * - Call of member functions which are defined in the interface class     * 
 *   definition ( header )                                                 * 
 *                                                                         * 
 * Pairing of internal and interface classes:                              * 
 * - Internal implementation in an :impl: subdirectory                     * 
 * - Interface in the parent directory of the corresponding internal class * 
 * - Interface class name IsoAgLib::iFoo_c maps to the internal class      * 
 *   __IsoAgLib::Foo_c                                                     * 
 *                                                                         * 
 * AS A RULE: Use only classes with names beginning with small letter :i:  *
 ***************************************************************************/
#include "isoname_c.h"
#include <IsoAgLib/typedef.h>
#include <cstring>

/* ************************************** */
/* import some namespaces for easy access */
/* ************************************** */
using __IsoAgLib::ISOName_c;


/**
  constructor which can read in initial data from uint8_t string
  @param rpb_src 64bit input data string
*/
ISOName_c::ISOName_c(const uint8_t* rpb_src)
{ // simply copy 8byte string
  if (rpb_src != NULL) CNAMESPACE::memmove(pb_data, rpb_src, 8);
}
/**
  constructor which format data string from series of input flags
  @param rb_selfConf true -> indicate sefl configuring ECU
  @param rui8_indGroup industry group of device (2 for agriculture)
  @param rb_devClass device class of ECU (equivalent to GETY in DIN)
  @param rb_devClassInst instance number of ECU with same devClass
        in the network (comparable to POS in DIN9684)
  @param rb_func function of the ECU (usual 25 for network interconnect)
  @param rui16_manufCode code of manufactor (11bit)
  @param rui32_serNo serial no of specific device (21bit)
  @param rb_funcInst instance number of ECU with same function and device class
      (default 0 - normally)
  @param rb_funcInst instance number of ECU with same function, device class and function instance
      (default 0 - normally)
*/
ISOName_c::ISOName_c(bool rb_selfConf, uint8_t rui8_indGroup, uint8_t rb_devClass, uint8_t rb_devClassInst,
      uint8_t rb_func, uint16_t rui16_manufCode, uint32_t rui32_serNo, uint8_t rb_funcInst, uint8_t rb_ecuInst)
{ // call the set function for the single flags
  set(rb_selfConf, rui8_indGroup, rb_devClass, rb_devClassInst, rb_func, rui16_manufCode, rui32_serNo,
      rb_funcInst, rb_ecuInst);
}
/**
  copy constructor for ISOName
  @param rrefc_src source ISOName_c instance
*/
ISOName_c::ISOName_c(const ISOName_c& rrefc_src)
{ // simply copy data string
  CNAMESPACE::memmove(pb_data, rrefc_src.pb_data, 8);
}
/**
  assign constructor for ISOName
  @param rrefc_src source ISOName_c object
*/
const ISOName_c& ISOName_c::operator=(const ISOName_c& rrefc_src)
{ // simply copy data string
  CNAMESPACE::memmove(pb_data, rrefc_src.pb_data, 8);
  return rrefc_src;
}
/**
  default destructor
*/
ISOName_c::~ISOName_c()
{ // do nothing
}

/**
  set data string with all flags with one call
  @param rb_selfConf true -> indicate sefl configuring ECU
  @param rui8_indGroup industry group of device (2 for agriculture)
  @param rb_devClass device class of ECU (equivalent to GETY in DIN)
  @param rb_devClassInst instance number of ECU with same devClass
        in the network (comparable to POS in DIN9684)
  @param rb_func function of the ECU (usual 25 for network interconnect)
  @param rui16_manufCode code of manufactor (11bit)
  @param rui32_serNo serial no of specific device (21bit)
  @param rb_funcInst instance number of ECU with same function and device class
      (default 0 - normally)
  @param rb_funcInst instance number of ECU with same function, device class and function instance
      (default 0 - normally)
*/
void ISOName_c::set(bool rb_selfConf, uint8_t rui8_indGroup, uint8_t rb_devClass, uint8_t rb_devClassInst,
      uint8_t rb_func, uint16_t rui16_manufCode, uint32_t rui32_serNo, uint8_t rb_funcInst, uint8_t rb_ecuInst)
{ // call the set function for the single flags
  setSelfConf(rb_selfConf);
  setIndGroup(rui8_indGroup);
  setDevClass(rb_devClass);
  setDevClassInst(rb_devClassInst);
  setFunc(rb_func);
  setManufCode(rui16_manufCode);
  setSerNo(rui32_serNo);
  setFuncInst(rb_funcInst);
  setEcuInst(rb_ecuInst);
}




/**
  get self config mode
  @return self configuration adress state
*/
uint8_t ISOName_c::selfConf() const
{
  return (pb_data[7] >> 7);
}

/**
  get industry group code
  @return industry group of device
*/
uint8_t ISOName_c::indGroup() const
{
  return ((pb_data[7] >> 4) & 0x7) ;
}

/**
  get device class instance number
  @return:device class instance number
*/
uint8_t ISOName_c::devClassInst() const
{
  return (pb_data[7] & 0xF);
}

/**
  get device class code
  @return:device class
*/
uint8_t ISOName_c::devClass() const
{
  return (pb_data[6] >> 1);
}

/**
  get function code
  @return function code
*/
uint8_t ISOName_c::func() const
{
  return pb_data[5];
}

/**
  get function instance code
  @return function instance code
*/
uint8_t ISOName_c::funcInst() const
{
  return (pb_data[4] >> 3);
}

/**
  get ECU instance code
  @return ECU instance code
*/
uint8_t ISOName_c::ecuInst() const
{
  return (pb_data[4] & 0x7);
}

/**
  get manufactor code
  @return manufactor code
*/
uint16_t ISOName_c::manufCode() const
{
  return ((pb_data[3] << 3) | (pb_data[2] >> 5));
}


/**
  get serial number
  @return serial number
*/
uint32_t ISOName_c::serNo() const
{
  return ((static_cast<uint32_t>(pb_data[2] & 0x1F) << 16) | (static_cast<uint32_t>(pb_data[1]) << 8) | pb_data[0]);
}






/**
  set the NAME data from 8 uint8_t string
  @param rpb_src pointer to 8byte source string
*/
void ISOName_c::inputString(const uint8_t* rpb_src)
{
  if (rpb_src != NULL) CNAMESPACE::memmove(pb_data, rpb_src, 8);
}

/**
  set self config mode
  @param rb_selfConf true -> indicate sefl configuring ECU
*/
void ISOName_c::setSelfConf(bool rb_selfConf)
{
  pb_data[7] = ((pb_data[7] & 0x7F) | (rb_selfConf << 7));
}
/**
  set industry group code
  @param rui8_indGroup industry group of device (2 for agriculture)
*/
void ISOName_c::setIndGroup(uint8_t rui8_indGroup)
{
  pb_data[7] = ((pb_data[7] & 0x8F) | ((rui8_indGroup & 0x7) << 4));
}
/**
  set device class instance number
  @param rb_devClassInst instance number of ECU with same devClass
        in the network (comparable to POS in DIN9684)
*/
void ISOName_c::setDevClassInst(uint8_t rb_devClassInst)
{
  pb_data[7] = ((pb_data[7] & 0xF0) | (rb_devClassInst));
}
/**
  set device class code
  @param rb_devClass device class of ECU (equivalent to GETY in DIN)
*/
void ISOName_c::setDevClass(uint8_t rb_devClass)
{
  pb_data[6] = ((0 /* reserved bit set to zero!*/) | (rb_devClass << 1));
/* old version, which would be right if the reserved bit would have been set somewhere else.
  pb_data[6] = ((pb_data[6] & 0x1) | (rb_devClass << 1));
*/
}
/**
  set function code
  @param rb_func function of the ECU (usual 25 for network interconnect)
*/
void ISOName_c::setFunc(uint8_t rb_func)
{
  pb_data[5] = rb_func;
}
/**
  set function instance code
  @param rb_funcInst instance number of ECU with same function and device class
      (default 0 - normally)
*/
void ISOName_c::setFuncInst(uint8_t rb_funcInst)
{
  pb_data[4] = ((pb_data[4] & 0x7) | (rb_funcInst << 3));
}
/**
  set ECU instance code
  @param rb_funcInst instance number of ECU with same function, device class and function instance
      (default 0 - normally)
*/
void ISOName_c::setEcuInst(uint8_t rb_ecuInst)
{
  pb_data[4] = ((pb_data[4] & 0xF8) | (rb_ecuInst & 0x7));
}
/**
  set manufactor code
  @param rui16_manufCode code of manufactor (11bit)
*/
void ISOName_c::setManufCode(uint16_t rui16_manufCode)
{
  pb_data[3] = (rui16_manufCode >> 3);
  pb_data[2] = ((pb_data[2] & 0x1F) | ((rui16_manufCode & 0x7) << 5));
}

/**
  set serial number (Identity Number)
  @param rui32_serNo serial no of specific device (21bit)
*/
void ISOName_c::setSerNo(uint32_t rui32_serNo)
{
  pb_data[2] = ( (pb_data[2] & 0xE0) | ((rui32_serNo >> 16) & 0x1F) );
  pb_data[1] = ((rui32_serNo >> 8) & 0xFF);
  pb_data[0] = (rui32_serNo & 0xFF);
}


/**
  check if this NAME has higher prio
  than the given NAME 8-uint8_t string
  @param rpb_compare
*/
bool ISOName_c::higherPriThanPar(const uint8_t* rpb_compare)
{
  bool b_lowerVal = true;
  for (int8_t ui8_cnt = 8; ui8_cnt >= 0; ui8_cnt -= 1)
  { // compare starting with self_conf and indGroup flag
    // in parts of uint16_t (2-uint8_t)
    if (pb_data[ui8_cnt] > rpb_compare[ui8_cnt])
    { // compared value has smaller val -> %e.g. higher prio
      b_lowerVal = false;
      break;
    }
  }
  return b_lowerVal;
}

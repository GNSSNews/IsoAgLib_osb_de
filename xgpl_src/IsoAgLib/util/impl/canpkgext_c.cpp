/***************************************************************************
                          canpkgext_c.cpp - header for extended CANPkg_c object,
                                           which calls data flag converting
                                           functions on assign operations
                             -------------------
    begin                : Tue Jan 11 2000
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
 * Copyright (C) 1999 - 2004 Dipl.-Inform. Achim Spangler                  *
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

#include "canpkgext_c.h"

// Begin Namespace __IsoAgLib
namespace __IsoAgLib {

bool CANPkgExt_c::b_runFlag2String = true;

/** default constructor, which has nothing to do */
CANPkgExt_c::CANPkgExt_c( int ri_singletonVecKey ) : CANPkg_c( ri_singletonVecKey ) {
}
/** virtual default destructor, which has nothing to do */
CANPkgExt_c::~CANPkgExt_c(){
}
/**
  ==> OBSOLETE, because now all can-pkg-data is STATIC!
  ==> REACTIVATE if some NON-STATIC member vars will be added!
  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

  assign operator to insert informations from one CANPkg_c into another
  @see __IsoAgLib::FilterBox_c::operator>>
  @see CANPkgExt_c::operator=
  @see CANPkgExt_c::getData
  @param rrefc_right reference to the source CANPkg_c on the right
  @return reference to the source CANPkg_c to enable assign chains like
      "pkg1 = pkg2 = pkg3 = pkg4;"
const CANPkg_c& CANPkgExt_c::operator=(const CANPkg_c& rrefc_right)
{
  return CANPkg_c::operator =( rrefc_right );
}
*/

/**
  abstract function to transform the string data into flag values
  => derived class must implement suitable data conversion function

  needed for assigning informations from another CANPkg_c or CANPkgExt
  @see CANPkgExt_c::operator=
*/
void CANPkgExt_c::string2Flags()
{ // dummy body - normally NOT called
};

/**
  abstract transform flag values to data string
  => derived class must implement suitable data converting function

  needed for sending informations from this object via CANIO_c on CAN BUS,
  because CANIO_c doesn't know anything about the data format of this type of msg
  so that it can only use an unformated data string from CANPkg
  @see CANPkgExt_c::getData
  @see __IsoAgLib::CANIO_c::operator<<
*/
void CANPkgExt_c::flags2String()
{ // dummy body - normally NOT called
};

/**
  put data into given reference to BIOS related data structure with data, len
  @param reft_ident     reference where the ident is placed for send
  @param refui8_identType reference to the ident type val: 0==std, 1==ext
  @param refb_dlcTarget reference to the DLC field of the target
  @param pb_dataTarget pointer to the data string of the target
*/
void CANPkgExt_c::getData(uint32_t& reft_ident, uint8_t& refui8_identType,
                     uint8_t& refb_dlcTarget, uint8_t* pb_dataTarget)
{
  if ( ! b_runFlag2String )
  { // data is already setup -> don't call flags2String - but reset it again to true for the next message
    b_runFlag2String = true;
  }
  else
  {
    flags2String();
  }
  CANPkg_c::getData(reft_ident, refui8_identType, refb_dlcTarget, pb_dataTarget);
}

#ifdef USE_ISO_11783
/**
  set the value of the ISO11783 ident field PGN
  @return parameter group number
*/
void CANPkgExt_c::setIsoPgn(uint32_t rui32_val)
{
  uint16_t ui16_val = rui32_val & 0x1FFFF;
  setIdent( (ui16_val & 0xFF), 1, Ident_c::ExtendedIdent);
  setIdent( (ui16_val >> 8), 2, Ident_c::ExtendedIdent);
  ui16_val = (rui32_val >> 16) & 0x1;
  ui16_val |= (ident(3) & 0x1E);
  setIdent( uint8_t(ui16_val & 0xFF), 3, Ident_c::ExtendedIdent);
}
#endif
} // end of namespace __IsoAgLib

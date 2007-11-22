/***************************************************************************
                          measureprogbase_c.cpp - base class for measure
                                                 values and programs
                             -------------------
    begin                : Fri Apr 07 2000
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

/* *************************************** */
/* ********** include headers ************ */
/* *************************************** */
#include "measureprogbase_c.h"
#include "../../impl/process_c.h"
#include "../../processdatachangehandler_c.h"
#include <algorithm>

#if defined(DEBUG) || defined(DEBUG_HEAP_USEAGE)
  #include <supplementary_driver/driver/rs232/impl/rs232io_c.h>
  #include <IsoAgLib/util/impl/util_funcs.h>
#endif

#ifdef DEBUG_HEAP_USEAGE
static uint16_t sui16_MeasureProgBaseTotal = 0;
static uint16_t sui16_printedMeasureProgBaseTotal = 0;
static uint16_t sui16_deconstructMeasureProgBaseTotal = 0;
static uint16_t sui16_printedDeconstructMeasureProgBaseTotal = 0;
#endif

namespace __IsoAgLib {

/** initialise the measure prog instance, to set this instance to a well defined starting condition
    @param arc_processData optional reference to containing ProcDataBase_c instance (default NULL)
    @param ren_progType optional program type: Proc_c::Base, Proc_c::Target (default Proc_c::UndefinedProg)
    @param ai32_val optional individual measure val for this program instance (can differ from master measure value)
    @param ac_isoName optional ISOName of partner member for this measure program
  */
void MeasureProgBase_c::init( ProcDataBase_c *const apc_processData,
  Proc_c::progType_t ren_progType, int32_t ai32_val,
  const IsoName_c& ac_isoName)
{ // set the dynamic list to a well defined cleared starting condition
  #ifdef DEBUG_HEAP_USEAGE
  static bool b_doPrint = true;
  if ( b_doPrint )
  {
    b_doPrint = false;
    INTERNAL_DEBUG_DEVICE
      << "sizeof(MeasureSubprog_c) ==  " << sizeof(MeasureSubprog_c)
      << " Bytes" << INTERNAL_DEBUG_DEVICE_ENDL;
  }
  if ( sui16_MeasureProgBaseTotal > 0 )
  {
    sui16_MeasureProgBaseTotal -= ( mvec_measureSubprog.size() * ( sizeof(MeasureSubprog_c) + 2 * sizeof(MeasureSubprog_c*) ) );
  }
  #endif

  mvec_measureSubprog.clear();
  // set the pointers in the baseClass ProcessElementBase
  set(apc_processData);
  // store the parameter init vals
  mc_isoName = ac_isoName;
  mi32_val = ai32_val;
  men_progType = ren_progType;

  // set the rest of element vals to defined init
  men_accumProp = Proc_c::AccumNone;
  men_doSend = Proc_c::DoNone;
  men_type = Proc_c::DistProp;

  mi32_accel = mi32_delta = mi32_integ = mi32_lastTime = mi32_max = mi32_min = 0;
}


/** assignment of MeasureProgBase_c objects
    @param arc_src source MeasureProgBase_c instance
    @return reference to source instance for cmd like "prog1 = prog2 = prog3;"
  */
const MeasureProgBase_c& MeasureProgBase_c::operator=(const MeasureProgBase_c& arc_src){
  // call base class operator
  ProcessElementBase_c::operator=(arc_src);

  assignFromSource( arc_src );

  // return reference to source
  return arc_src;
}


/** copy constructor
    @param arc_src source MeasureProgBase_c instance
  */
MeasureProgBase_c::MeasureProgBase_c(const MeasureProgBase_c& arc_src)
 : ProcessElementBase_c(arc_src)  {
  assignFromSource( arc_src );
}


/** base function for assignment of element vars for copy constructor and operator= */
void MeasureProgBase_c::assignFromSource( const MeasureProgBase_c& arc_src )
{ // copy element vars
  mc_isoName = arc_src.mc_isoName;
  men_accumProp =  arc_src.men_accumProp;
  men_doSend = arc_src.men_doSend;
  men_progType = arc_src.men_progType;
  mi32_accel = arc_src.mi32_accel;
  mi32_delta = arc_src.mi32_delta;
  mi32_integ = arc_src.mi32_integ;
  mi32_lastTime = arc_src.mi32_lastTime;
  mi32_max = arc_src.mi32_max;
  mi32_min = arc_src.mi32_min;
  mi32_val = arc_src.mi32_val;
  mvec_measureSubprog = arc_src.mvec_measureSubprog;

  if (mvec_measureSubprog.size() < arc_src.mvec_measureSubprog.size())
  { // not all items copied
    getILibErrInstance().registerError( iLibErr_c::BadAlloc, iLibErr_c::Process );
  }
  #ifdef DEBUG_HEAP_USEAGE
  else
  {
    sui16_MeasureProgBaseTotal += mvec_measureSubprog.size();

    if ( mvec_measureSubprog.size() > 0 )
    {
      INTERNAL_DEBUG_DEVICE
        << sui16_MeasureProgBaseTotal << " x MeasureSubprog_c: Mal-Alloc: "
        <<  sizeSlistTWithMalloc( sizeof(MeasureSubprog_c), sui16_MeasureProgBaseTotal )
        << "/" << sizeSlistTWithMalloc( sizeof(MeasureSubprog_c), 1 )
        << ", Chunk-Alloc: "
        << sizeSlistTWithChunk( sizeof(MeasureSubprog_c), sui16_MeasureProgBaseTotal )
        << INTERNAL_DEBUG_DEVICE_NEWLINE << INTERNAL_DEBUG_DEVICE_ENDL;
    }
  }
  #endif
}

/** default destructor which has nothing to do */
MeasureProgBase_c::~MeasureProgBase_c(){
  #ifdef DEBUG_HEAP_USEAGE
  if ( mvec_measureSubprog.size() > 0 )
  {
    sui16_deconstructMeasureProgBaseTotal++;
    sui16_MeasureProgBaseTotal -= mvec_measureSubprog.size();
  }
  #endif
}

/** add an aditional subprog or update if one with same kind exist already

    possible errors:
        * Err_c::badAlloc not enough memory to add new subprog
    @param ren_type increment type: Proc_c::TimeProp, Proc_c::DistProp, Proc_c::ValIncr
    @param ai32_increment increment value
    @param ren_doSend set process data subtype to send (Proc_c::DoNone, Proc_c::DoVal, Proc_c::DoValForExactSetpoint...)
    @return true -> subprog was created successfully; fals -> out-of-memory error
  */
 bool MeasureProgBase_c::addSubprog(Proc_c::type_t ren_type, int32_t ai32_increment, Proc_c::doSend_t ren_doSend){
  if (ren_type == Proc_c::TimeProp) men_accumProp = Proc_c::AccumTime;
  else if (ren_type == Proc_c::DistProp) men_accumProp = Proc_c::AccumDist;

  // if subprog with this type exist, update only value
  Vec_MeasureSubprog::iterator pc_subprog = mvec_measureSubprog.end();
  for (pc_subprog = mvec_measureSubprog.begin();
       pc_subprog != mvec_measureSubprog.end(); pc_subprog++)
  {
     if ((pc_subprog->type() == ren_type) && (pc_subprog->doSend() == ren_doSend)) break;
  }

  if (pc_subprog != mvec_measureSubprog.end())
  { // subprog with same type found -> update val
    pc_subprog->setIncrement(ai32_increment);
  }
  else
  { // no subprog with same type exist -> insert new one
    const uint8_t b_oldSize = mvec_measureSubprog.size();
    mvec_measureSubprog.push_front(MeasureSubprog_c(ren_type, ren_doSend, ai32_increment SINGLETON_VEC_KEY_WITH_COMMA));
    if (b_oldSize >= mvec_measureSubprog.size())
    { // array didn't grow
      getILibErrInstance().registerError( iLibErr_c::BadAlloc, iLibErr_c::Process );
      return false;
    }
    #ifdef DEBUG_HEAP_USEAGE
    else
    {
      sui16_MeasureProgBaseTotal++;

      INTERNAL_DEBUG_DEVICE
        << sui16_MeasureProgBaseTotal << " x MeasureSubprog_c: Mal-Alloc: "
        <<  sizeSlistTWithMalloc( sizeof(MeasureSubprog_c), sui16_MeasureProgBaseTotal )
        << "/" << sizeSlistTWithMalloc( sizeof(MeasureSubprog_c), 1 )
        << ", Chunk-Alloc: "
        << sizeSlistTWithChunk( sizeof(MeasureSubprog_c), sui16_MeasureProgBaseTotal )
        << INTERNAL_DEBUG_DEVICE_NEWLINE << INTERNAL_DEBUG_DEVICE_ENDL;
    }
    #endif
  }

  // setting of isoName in MeasureProg is normally done via ProcDataRemote_c::timeEvent( void )
  // if start follows immedeately addSubprog timeEvent is not called yet => do it here
  // remote: virtual ProcDataRemote::commanderISOName() can give a value different to IsoName_c::IsoNameUnspecified
  // local: virtual ProcDataLocal::commanderISOName() gives IsoName_c::IsoNameUnspecified
  if (pprocessDataConst()->commanderISOName().isSpecified())
    setISOName(pprocessDataConst()->commanderISOName());

  return true;
}

/** ISO uses positive values even for time proportional measure prog
    -> only the start cmd choose increment type
    -> search for forced increment type and set first to according type if needed
  */
void MeasureProgBase_c::forceSubprogType(Proc_c::type_t ren_type)
{
  Vec_MeasureSubprog::iterator pc_subprog;
  for (pc_subprog = mvec_measureSubprog.begin(); pc_subprog != mvec_measureSubprog.end(); pc_subprog++)
  {
    if (pc_subprog->type() == ren_type) break;
  }
  if ((pc_subprog == mvec_measureSubprog.end()) && (!mvec_measureSubprog.empty()))
  { // set increment type of first item
    mvec_measureSubprog.begin()->setType(ren_type);
  }
}


/** start a measuring programm
    @param ren_type wanted increment type (Proc_c::TimeProp, Proc_c::DistProp, Proc_c::ValIncr)
    @param ren_doSend set process data subtype to send (Proc_c::DoNone, Proc_c::DoVal, Proc_c::DoValForExactSetpoint...)
    @return always true; only relevant for overloaded methods in derived classes
  */
bool MeasureProgBase_c::start(Proc_c::type_t ren_type, Proc_c::doSend_t ren_doSend){
  // register values
  men_doSend = (ren_doSend != Proc_c::DoNone)?ren_doSend : men_doSend;
  if (men_doSend == Proc_c::DoNone) men_doSend = Proc_c::DoVal;
  forceSubprogType(ren_type);
  men_type = ren_type;
  return true;
}

/** stop all running subprog
    @param b_deleteSubProgs is only needed for remote ISO case (but is needed due to overloading here also)
    @param ren_type wanted increment type (Proc_c::TimeProp, Proc_c::DistProp, Proc_c::ValIncr)
    @param ren_doSend set process data subtype to stop (Proc_c::DoNone, Proc_c::DoVal, Proc_c::DoValForExactSetpoint...)
    @return always true; only relevant for overloaded methods in derived classes
  */
bool MeasureProgBase_c::stop(bool /*b_deleteSubProgs*/, Proc_c::type_t /* ren_type */, Proc_c::doSend_t /* ren_doSend */){
  // clear the array with all subprogs -> no trigger test is done on value set
  #ifdef DEBUG_HEAP_USEAGE
  sui16_MeasureProgBaseTotal -= mvec_measureSubprog.size();

  if ( ( sui16_MeasureProgBaseTotal != sui16_printedMeasureProgBaseTotal                     )
  || ( sui16_deconstructMeasureProgBaseTotal != sui16_printedDeconstructMeasureProgBaseTotal ) )
  {
    sui16_printedMeasureProgBaseTotal = sui16_MeasureProgBaseTotal;
    sui16_printedDeconstructMeasureProgBaseTotal = sui16_deconstructMeasureProgBaseTotal;
    INTERNAL_DEBUG_DEVICE
      << sui16_MeasureProgBaseTotal << " x MeasureSubprog_c: Mal-Alloc: "
      <<  sizeSlistTWithMalloc( sizeof(MeasureSubprog_c), sui16_MeasureProgBaseTotal )
      << "/" << sizeSlistTWithMalloc( sizeof(MeasureSubprog_c), 1 )
      << ", Chunk-Alloc: "
      << sizeSlistTWithChunk( sizeof(MeasureSubprog_c), sui16_MeasureProgBaseTotal )
      << INTERNAL_DEBUG_DEVICE_NEWLINE << INTERNAL_DEBUG_DEVICE_ENDL;
  }
  #endif
  mvec_measureSubprog.clear();
  men_doSend = Proc_c::DoNone;
  return true;
}

/** deliver actual last received value
    @param ab_sendRequest choose wether a request for value update should be
        sent (default false == send no request)
    @return measure val for this prog (can differ from master measure val)
  */
int32_t MeasureProgBase_c::val(bool ab_sendRequest) const
{
  if (ab_sendRequest) {
    // prepare general command in process pkg
    getProcessInstance4Comm().data().mc_generalCommand.setValues(false /* isSetpoint */, true /* isRequest */,
                                                                GeneralCommand_c::exactValue,
                                                                GeneralCommand_c::requestValue);

    processData().sendValISOName(isoName(), int32_t(0));
  }

  return mi32_val;
}

/** deliver integ val
    @param ab_sendRequest choose wether a request for value update should be
        sent (default false == send no request)
    @return integral val for this measure prog
  */
int32_t MeasureProgBase_c::integ(bool ab_sendRequest) const
{
  if (ab_sendRequest) {
    // prepare general command in process pkg
    getProcessInstance4Comm().data().mc_generalCommand.setValues(false /* isSetpoint */, true /* isRequest */,
                                                                GeneralCommand_c::integValue,
                                                                GeneralCommand_c::requestValue);

    processData().sendValISOName(isoName(), int32_t(0));
  }
  return mi32_integ;
}

/** deliver min val
    @param ab_sendRequest choose wether a request for value update should be
        sent (default false == send no request)
    @return MIN val for this measure prog
  */
int32_t MeasureProgBase_c::min(bool ab_sendRequest) const
{
  if(ab_sendRequest) {
    // prepare general command in process pkg
    getProcessInstance4Comm().data().mc_generalCommand.setValues(false /* isSetpoint */, true /* isRequest */,
                                                                GeneralCommand_c::minValue,
                                                                GeneralCommand_c::requestValue);

    processData().sendValISOName(isoName(), int32_t(0));
  }
  return mi32_min;
}

/** deliver max val
    @param ab_sendRequest choose wether a request for value update should be
        sent (default false == send no request)
    @return MAX val for this measure prog
  */
int32_t MeasureProgBase_c::max(bool ab_sendRequest) const
{
  if (ab_sendRequest) {
    // prepare general command in process pkg
    getProcessInstance4Comm().data().mc_generalCommand.setValues(false /* isSetpoint */, true /* isRequest */,
                                                                GeneralCommand_c::maxValue,
                                                                GeneralCommand_c::requestValue);

    processData().sendValISOName(isoName(), int32_t(0));
  }
  return mi32_max;
}


/** init the element vars
    @param ai32_val initial measure val
  */
void MeasureProgBase_c::initVal(int32_t ai32_val){
  #ifdef DEBUG_HEAP_USEAGE
  if ( ( sui16_MeasureProgBaseTotal != sui16_printedMeasureProgBaseTotal                     )
  || ( sui16_deconstructMeasureProgBaseTotal != sui16_printedDeconstructMeasureProgBaseTotal ) )
  {
    sui16_printedMeasureProgBaseTotal = sui16_MeasureProgBaseTotal;
    sui16_printedDeconstructMeasureProgBaseTotal = sui16_deconstructMeasureProgBaseTotal;
    INTERNAL_DEBUG_DEVICE
      << sui16_MeasureProgBaseTotal << " x MeasureSubprog_c: Mal-Alloc: "
      <<  sizeSlistTWithMalloc( sizeof(MeasureSubprog_c), sui16_MeasureProgBaseTotal )
      << "/" << sizeSlistTWithMalloc( sizeof(MeasureSubprog_c), 1 )
      << ", Chunk-Alloc: "
      << sizeSlistTWithChunk( sizeof(MeasureSubprog_c), sui16_MeasureProgBaseTotal )
      << INTERNAL_DEBUG_DEVICE_NEWLINE << INTERNAL_DEBUG_DEVICE_ENDL;
  }
  #endif
  mi32_val = mi32_min = mi32_max = mi32_integ = ai32_val;
}

#ifdef USE_FLOAT_DATA_TYPE
/** initialise the measure prog instance, to set this instance to a well defined starting condition
    @param arc_processData optional reference to containing ProcDataBase_c instance (default NULL)
    @param ren_progType optional program type: Proc_c::Base, Proc_c::Target (default Proc_c::UndefinedProg)
    @param af_val optional individual measure val for this program instance (can differ from master measure value)
    @param ac_isoName optional ISOName of partner member for this measure program
  */
void MeasureProgBase_c::init(
  ProcDataBase_c *const apc_processData,
  Proc_c::progType_t ren_progType,
  float af_val,
  const IsoName_c& ac_isoName)
{ // set the dynamic list to a well defined cleared starting condition
  #ifdef DEBUG_HEAP_USEAGE
  static bool b_doPrint = true;
  if ( b_doPrint )
  {
    b_doPrint = false;
    INTERNAL_DEBUG_DEVICE
      << "sizeof(MeasureSubprog_c) ==  " << sizeof(MeasureSubprog_c)
      << " Bytes" << INTERNAL_DEBUG_DEVICE_ENDL;
  }

  sui16_MeasureProgBaseTotal -= ( mvec_measureSubprog.size() * ( sizeof(MeasureSubprog_c) + 2 * sizeof(MeasureSubprog_c*) ) );
  #endif

  mvec_measureSubprog.clear();
  // set the pointers in the baseClass ProcessElementBase
  set(apc_processData);
  // store the parameter init vals
  mc_isoName = ac_isoName;
  f_val = af_val;
  men_progType = ren_progType;

  // set the rest of element vals to defined init
  men_accumProp = Proc_c::AccumNone;
  men_doSend = Proc_c::DoNone;
  men_type = Proc_c::DistProp;

  mi32_accel = mi32_delta = mi32_integ = mi32_lastTime = mi32_max = mi32_min = 0;
}

/** deliver actual last received value
    @param ab_sendRequest choose wether a request for value update should be
        sent (default false == send no request)
    @return measure val for this prog (can differ from master measure val)
  */
float MeasureProgBase_c::valFloat(bool ab_sendRequest) const
{
  if (ab_sendRequest) {
    // prepare general command in process pkg
    getProcessInstance4Comm().data().mc_generalCommand.setValues(false /* isSetpoint */, true /* isRequest */,
                                                                GeneralCommand_c::exactValue,
                                                                GeneralCommand_c::requestValue);

    processData().sendValISOName(isoName(), int32_t(0));
  }
  return f_val;
}

/** deliver integ val
    @param ab_sendRequest choose wether a request for value update should be
        sent (default false == send no request)
    @return integral val for this measure prog
  */
float MeasureProgBase_c::integFloat(bool ab_sendRequest) const
{
  if (ab_sendRequest) {
    // prepare general command in process pkg
    getProcessInstance4Comm().data().mc_generalCommand.setValues(false /* isSetpoint */, true /* isRequest */,
                                                                GeneralCommand_c::integValue,
                                                                GeneralCommand_c::requestValue);

    processData().sendValISOName(isoName(), int32_t(0));
  }
  return f_integ;
}


/** deliver min val
    @param ab_sendRequest choose wether a request for value update should be
        sent (default false == send no request)
    @return MIN val for this measure prog
  */
float MeasureProgBase_c::minFloat(bool ab_sendRequest) const
{
  if (ab_sendRequest) {
    // prepare general command in process pkg
    getProcessInstance4Comm().data().mc_generalCommand.setValues(false /* isSetpoint */, true /* isRequest */,
                                                                GeneralCommand_c::minValue,
                                                                GeneralCommand_c::requestValue);

    processData().sendValISOName(isoName(), int32_t(0));
  }
  return f_min;
}


/** deliver max val
    @param ab_sendRequest choose wether a request for value update should be
        sent (default false == send no request)
    @return MAX val for this measure prog
  */
float MeasureProgBase_c::maxFloat(bool ab_sendRequest) const
{
  if (ab_sendRequest) {
    // prepare general command in process pkg
    getProcessInstance4Comm().data().mc_generalCommand.setValues(false /* isSetpoint */, true /* isRequest */,
                                                                GeneralCommand_c::maxValue,
                                                                GeneralCommand_c::requestValue);

    processData().sendValISOName(isoName(), int32_t(0));
  }
  return f_max;
}


/** init the element vars
    @param af_val initial measure val
  */
void MeasureProgBase_c::initVal(float af_val){
  #ifdef DEBUG_HEAP_USEAGE
  if ( ( sui16_MeasureProgBaseTotal != sui16_printedMeasureProgBaseTotal                     )
  || ( sui16_deconstructMeasureProgBaseTotal != sui16_printedDeconstructMeasureProgBaseTotal ) )
  {
    sui16_printedMeasureProgBaseTotal = sui16_MeasureProgBaseTotal;
    sui16_printedDeconstructMeasureProgBaseTotal = sui16_deconstructMeasureProgBaseTotal;
    INTERNAL_DEBUG_DEVICE
      << sui16_MeasureProgBaseTotal << " x MeasureSubprog_c: Mal-Alloc: "
      <<  sizeSlistTWithMalloc( sizeof(MeasureSubprog_c), sui16_MeasureProgBaseTotal )
      << "/" << sizeSlistTWithMalloc( sizeof(MeasureSubprog_c), 1 )
      << ", Chunk-Alloc: "
      << sizeSlistTWithChunk( sizeof(MeasureSubprog_c), sui16_MeasureProgBaseTotal )
      << INTERNAL_DEBUG_DEVICE_NEWLINE << INTERNAL_DEBUG_DEVICE_ENDL;
  }
  #endif
  f_val = f_min = f_max = f_integ = af_val;
}
#endif


/** process a message;
    MeasureProgBase_c::processMsg is responsible for measure prog
    controlling commands
    @return true -> message was already edited complete
  */
bool MeasureProgBase_c::processMsg(){
  ProcessPkg_c& c_pkg = getProcessInstance4Comm().data();
  GeneralCommand_c::CommandType_t en_command = c_pkg.mc_generalCommand.getCommand();

  #ifdef DEBUG_HEAP_USEAGE
  if ( ( sui16_MeasureProgBaseTotal != sui16_printedMeasureProgBaseTotal                     )
  || ( sui16_deconstructMeasureProgBaseTotal != sui16_printedDeconstructMeasureProgBaseTotal ) )
  {
    sui16_printedMeasureProgBaseTotal = sui16_MeasureProgBaseTotal;
    sui16_printedDeconstructMeasureProgBaseTotal = sui16_deconstructMeasureProgBaseTotal;
    INTERNAL_DEBUG_DEVICE
      << sui16_MeasureProgBaseTotal << " x MeasureSubprog_c: Mal-Alloc: "
      <<  sizeSlistTWithMalloc( sizeof(MeasureSubprog_c), sui16_MeasureProgBaseTotal )
      << "/" << sizeSlistTWithMalloc( sizeof(MeasureSubprog_c), 1 )
      << ", Chunk-Alloc: "
      << sizeSlistTWithChunk( sizeof(MeasureSubprog_c), sui16_MeasureProgBaseTotal )
      << INTERNAL_DEBUG_DEVICE_NEWLINE << INTERNAL_DEBUG_DEVICE_ENDL;
  }
  #endif
  bool b_edited = false;

  if (en_command == GeneralCommand_c::setValue)
    // setValue command indicates receiving of a measure value
    // => handle this in measure prog remote => return false
    // or a reset
    // => handle this in measure prog local => return false
    return false;

  // Not sure why this has problems, but it does. So, don't run it with ISO_TASK_CONTROLLER! -bac
  // check if PD==0 -> SET increment message
  if (!c_pkg.mc_generalCommand.checkIsRequest())
  { // mark that msg already edited
    b_edited = true;

    // set en_doSendPkg (for ISO)
    GeneralCommand_c::ValueGroup_t en_valueGroup = c_pkg.mc_generalCommand.getValueGroup();

    Proc_c::doSend_t en_doSendPkg = Proc_c::DoVal;  //default send data mode
    if (c_pkg.mc_generalCommand.checkIsSetpoint())
      en_doSendPkg = Proc_c::DoValForExactSetpoint; // measurement for exact value setpoint

    switch (en_valueGroup)
    {
      case GeneralCommand_c::minValue:
        en_doSendPkg = Proc_c::DoValForMinSetpoint; // measurement for min value setpoint
        if (!c_pkg.mc_generalCommand.checkIsSetpoint())
          en_doSendPkg = Proc_c::DoValForMinMeasurement; // measurement for min value measurement
        break;
      case GeneralCommand_c::maxValue:
        en_doSendPkg = Proc_c::DoValForMaxSetpoint; // measurement for max value setpoint
        if (!c_pkg.mc_generalCommand.checkIsSetpoint())
          en_doSendPkg = Proc_c::DoValForMaxMeasurement; // measurement for max value measurement
        break;
      case GeneralCommand_c::defaultValue:
        en_doSendPkg = Proc_c::DoValForDefaultSetpoint; // measurement for default value setpoint
        break;
      default:
        ;
    }

    // programm controlling command
    if (// ISO
        en_command == GeneralCommand_c::measurementDistanceValueStart ||
        en_command == GeneralCommand_c::measurementTimeValueStart ||
        en_command == GeneralCommand_c::measurementChangeThresholdValueStart ||
        en_command == GeneralCommand_c::measurementMinimumThresholdValueStart ||
        en_command == GeneralCommand_c::measurementMaximumThresholdValueStart)
      // increment
      processIncrementMsg(en_doSendPkg);


    uint8_t b_cmd = c_pkg.data(0);

    if (en_command == GeneralCommand_c::measurementReset)
    { // reset command (ISO reset: setValue command)
       reset(b_cmd);
       // call handler function if handler class is registered
       if ( processData().getProcessDataChangeHandler() != NULL )
         processData().getProcessDataChangeHandler()->processMeasurementReset( pprocessData(), 0, c_pkg.memberSend().isoName().toConstIisoName_c() );
    }

    if (en_command == GeneralCommand_c::measurementStop)
       stop();

    // ISO, local
    if (en_command == GeneralCommand_c::measurementDistanceValueStart ||
        en_command == GeneralCommand_c::measurementTimeValueStart ||
        en_command == GeneralCommand_c::measurementChangeThresholdValueStart ||
        en_command == GeneralCommand_c::measurementMinimumThresholdValueStart ||
        en_command == GeneralCommand_c::measurementMaximumThresholdValueStart)
    {
      Proc_c::type_t en_typePkg = Proc_c::NullType;
      int32_t i32_dataLong = c_pkg.dataLong();
      switch (en_command) {
        case GeneralCommand_c::measurementTimeValueStart:
          en_typePkg = Proc_c::TimeProp;
          break;
        case GeneralCommand_c::measurementDistanceValueStart:
          en_typePkg = Proc_c::DistProp;
          break;
        case GeneralCommand_c::measurementChangeThresholdValueStart:
          en_typePkg = Proc_c::OnChange;
          if (Proc_c::ThresholdChangeStopVal == i32_dataLong)
            i32_dataLong = 0; // stop command
          break;
        case GeneralCommand_c::measurementMaximumThresholdValueStart:
          en_typePkg = Proc_c::MaximumThreshold;
          if (Proc_c::ThresholdMaximumStopVal == i32_dataLong)
            i32_dataLong = 0; // stop command
          break;
        case GeneralCommand_c::measurementMinimumThresholdValueStart:
          en_typePkg = Proc_c::MinimumThreshold;
          if (Proc_c::ThresholdMinimumStopVal == i32_dataLong)
            i32_dataLong = 0; // stop command
          break;
        default: ;
      }

      // if dataLong() == 0 => stop
      if (i32_dataLong != 0)
      {
        if (en_typePkg != Proc_c::NullType)
          start(en_typePkg, en_doSendPkg);
      }
      else
       // call MeasureProgLocal_c::stop() with TRUE and en_typePkg != Proc_c::NullType
       // => only the appropriate MeasureSubprog_c is deleted (selective stop)
       stop(TRUE /* b_deleteSubProgs */, en_typePkg, en_doSendPkg);
    }
  }

#if 0
     // old code:
     // program controlling with start, stop or reset
      uint8_t b_cmd = c_pkg.data(0);
      #ifdef LAV_COMMENT_MEASUREPROG_CMD
      if ((b_cmd & 0x80) > 0)
      #else
      if ( ((b_cmd & 0xF) > 0) && ((b_cmd & 0xF) < 7))
      #endif
      { // start command
        start(static_cast<Proc_c::progType_t>(c_pkg.pri()),
             static_cast<Proc_c::type_t>(b_cmd & 0x7),
             static_cast<Proc_c::doSend_t>((b_cmd >> 4) & 0x7));
      }
      else
      {
        #ifdef LAV_COMMENT_MEASUREPROG_CMD
        if ((b_cmd & 0x8) > 0)
        #else
        if ((b_cmd & 0xF) == 0x8)
        #endif
        { // reset command
          reset(b_cmd);
          // call handler function if handler class is registered
          if ( processData().getProcessDataChangeHandler() != NULL )
            processData().getProcessDataChangeHandler()->processMeasurementReset( pprocessData(), 0, c_pkg.memberSend().isoName().toConstIisoName_c() );
        }
        else
        { // stop command
          stop();
        }
      }
    }
#endif
//#endif
  return b_edited;
}


/** perform periodic actions
    @param pui16_nextTimePeriod calculated new time period, based on current measure progs (only for local proc data)
    @return true -> all planned activities performed in available time
  */
bool MeasureProgBase_c::timeEvent( uint16_t* /* pui16_nextTimePeriod */ )
{return true;}


/** deliver to en_valueGroup according measure val type

    possible errors:
        * Err_c:range MOD is not in allowed range [0..6]
    @param en_valueGroup of wanted subtype
    @return value of specified subtype
  */
int32_t MeasureProgBase_c::valMod(GeneralCommand_c::ValueGroup_t en_valueGroup) const {
  int32_t i32_value = val();
  switch (en_valueGroup)
  {
    case GeneralCommand_c::exactValue:
      // set val with function, to calc delta and accel
      // i32_value = val();
      break;
    case GeneralCommand_c::minValue:
      i32_value = min();
      break;
    case GeneralCommand_c::maxValue:
      i32_value = max();
      break;
    case GeneralCommand_c::integValue:
      i32_value = integ();
      break;
    case GeneralCommand_c::medValue:
      i32_value = med();
      break;
    default:
      // wrong range
      getILibErrInstance().registerError( iLibErr_c::Range, iLibErr_c::Process );
  }

  return i32_value;
}


#ifdef USE_FLOAT_DATA_TYPE
/** deliver to en_valueGroup according measure val type
    as float value

    possible errors:
        * Err_c:range MOD is not in allowed range [0..6]
    @param en_valueGroup of wanted subtype
    @return value of specified subtype
  */
float MeasureProgBase_c::valModFloat(GeneralCommand_c::ValueGroup_t en_valueGroup) const
{
  float f_value = valFloat();
    switch (en_valueGroup)
    {
      case GeneralCommand_c::exactValue:
        // set val with function, to calc delta and accel
        // i32_value = val();
        break;
      case GeneralCommand_c::minValue:
        f_value = minFloat();
        break;
      case GeneralCommand_c::maxValue:
        f_value = maxFloat();
        break;
      case GeneralCommand_c::integValue:
        f_value = integFloat();
        break;
      case GeneralCommand_c::medValue:
        f_value = medFloat();
        break;
      default:
        getILibErrInstance().registerError( iLibErr_c::Range, iLibErr_c::Process );
    }
  return f_value;
}
#endif


/** reset according to the process msg command the appropriate value type
    @param ab_comm command from Scheduler_c reset message
  */
void MeasureProgBase_c::reset(uint8_t ab_comm){
  if ((ab_comm & 0x8) > 0)
  {
    if ((ab_comm & 0xF0) > 0)
    {
      for (uint8_t b_mask = 1; b_mask <= 4; b_mask *= 2)
      {
        if (( (ab_comm & 0xF0) & (b_mask << 4) ) > 0)
        { // bit of test b_mask set in reset cmd
          switch (b_mask)
          {
            case 1: // value reset
              resetVal();
              break;
            case 2: // medium reset
              resetMed();
              break;
            case 4: // integer reset
              resetInteg();
              break;
          } // switch
        } // if
      } // for
    } // if > 0
    else
    { // all values reset
      resetMin();
      resetMax();
      resetMed();
      resetVal();
      resetInteg();
    }
  }
}


/** process a message with an increment for a measuring program

    possible errors:
        * Err_c::badAlloc not enough memory to add new subprog
    @param ren_doSend set process data subtype to send (Proc_c::DoNone, Proc_c::DoVal, Proc_c::DoValForExactSetpoint...)
  */
void MeasureProgBase_c::processIncrementMsg(Proc_c::doSend_t ren_doSend){
  ProcessPkg_c& c_pkg = getProcessInstance4Comm().data();

  // set mc_isoName to caller of prog
  mc_isoName = c_pkg.memberSend().isoName();

  // get the int32_t data val without conversion
  const int32_t ci32_val = c_pkg.dataRawCmdLong();

  if ( c_pkg.mc_generalCommand.getCommand() == GeneralCommand_c::measurementTimeValueStart)
    // time proportional
    addSubprog(Proc_c::TimeProp, CNAMESPACE::labs(ci32_val), ren_doSend);

  if ( c_pkg.mc_generalCommand.getCommand() == GeneralCommand_c::measurementDistanceValueStart)
    // distance proportional
    addSubprog(Proc_c::DistProp, ci32_val, ren_doSend);

  if (c_pkg.mc_generalCommand.getCommand() == GeneralCommand_c::measurementChangeThresholdValueStart)
    // change threshold proportional
    // @todo: was DistProp ?
    addSubprog(Proc_c::OnChange, ci32_val, ren_doSend);

  if (c_pkg.mc_generalCommand.getCommand() == GeneralCommand_c::measurementMaximumThresholdValueStart)
    // change threshold proportional
    addSubprog(Proc_c::MaximumThreshold, ci32_val, ren_doSend);

  if (c_pkg.mc_generalCommand.getCommand() == GeneralCommand_c::measurementMinimumThresholdValueStart)
    // change threshold proportional
    addSubprog(Proc_c::MinimumThreshold, ci32_val, ren_doSend);
}


/** reset according to the MOD the appropriate value type

    possible errors:
        * Err_c:range MOD is not in allowed range [0..4]
    @param ab_mod MOD of wanted subtype
    @param ai32_val reset measure value to this value (ISO remote only)
  */
void MeasureProgBase_c::resetValMod(GeneralCommand_c::ValueGroup_t en_valueGroup, int32_t ai32_val){
    switch (en_valueGroup)
    {
      case GeneralCommand_c::exactValue:
        // set val with function, to calc delta and accel
        resetVal(ai32_val);
        break;
      case GeneralCommand_c::minValue:
        resetMin();
        break;
      case GeneralCommand_c::maxValue:
        resetMax();
        break;
      case GeneralCommand_c::integValue:
        resetInteg();
        break;
      case GeneralCommand_c::medValue:
        resetMed();
        break;
      default:
        getILibErrInstance().registerError( iLibErr_c::Range, iLibErr_c::Process );
    }
}


} // end of namespace __IsoAgLib

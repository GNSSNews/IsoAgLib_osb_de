/*
  singleton.h

  (C) Copyright 2009 - 2010 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/
#ifndef SINGLETON_H_
#define SINGLETON_H_

#include <IsoAgLib/isoaglib_config.h>

/// Check preconditions (defines generated by update_makefile.sh)
#if defined(CAN_INSTANCE_CNT) && defined(PRT_INSTANCE_CNT)
#  define PROP_INSTANCE_CNT (CAN_INSTANCE_CNT - PRT_INSTANCE_CNT)
#else
#  error "You need to specify CAN_INSTANCE_CNT and PRT_INSTANCE_CNT!"
#endif

/// Set default where applicable
#if !defined(RS232_INSTANCE_CNT)
#  define RS232_INSTANCE_CNT 0
#endif


#if defined(OPTIMIZE_HEAPSIZE_IN_FAVOR_OF_SPEED) && defined( __GNUC__ ) && __GNUC__ >= 4
#  include <ext/malloc_allocator.h>
#endif

#define SINGLETON_VEC_KEY_PARAMETER_DEF               int ai_singletonVecKey
#define SINGLETON_VEC_KEY_PARAMETER_DEFAULT_NULL_DEF  int ai_singletonVecKey = 0
#define SINGLETON_VEC_KEY_PARAMETER_DEF_WITH_COMMA  , int ai_singletonVecKey
#define SINGLETON_VEC_KEY_PARAMETER_DEFAULT_NULL_DEF_WITH_COMMA , int ai_singletonVecKey = 0
#define SINGLETON_VEC_KEY_PARAMETER_USE               ai_singletonVecKey
#define SINGLETON_VEC_KEY_PARAMETER_USE_WITH_COMMA  , ai_singletonVecKey

#define SINGLETON_PAR_DOT_DEF(PAR)                     int getSingletonVecKey() const { return PAR.getSingletonVecKey(); }
#define SINGLETON_PAR_ARR_DEF(PAR)                     int getSingletonVecKey() const { return PAR->getSingletonVecKey(); }
#define SINGLETON_PAR_BASE_DEF(PAR)       int getSingletonVecKey() const { return PAR::getSingletonVecKey(); }
#define SINGLETON_MC_DATA_ASSIGN                   mc_data.setSingletonKey( getSingletonVecKey() );
#define SINGLETON_MEMBER_DEF               ClientBase c_clientBase; \
                                                    int getSingletonVecKey() const { return c_clientBase.getSingletonVecKey(); }
#define SINGLETON_MEMBER_ASSIGN(PAR)     c_clientBase.setSingletonKey (PAR.c_clientBase.getSingletonVecKey());
#define SINGLETON_MEMBER_CONSTRUCTOR    c_clientBase( ai_singletonVecKey ),
#define SINGLETON_MEMBER_COPY_CONSTRUCTOR(PAR) c_clientBase( PAR.getSingletonVecKey() ),
#define SINGLETON_PARENT_CONSTRUCTOR      ClientBase( ai_singletonVecKey ),

#define SINGLETON_VEC_KEY_INIT_CALL                   setSingletonKey( ai_singletonVecKey );
#define SINGLETON_VEC_KEY                             getSingletonVecKey()
#define SINGLETON_VEC_KEY_WITH_COMMA                , getSingletonVecKey()


#define getForeignInstance4Comm(PAR)      PAR.getSingletonVecKey()
#define getIsoBusInstance4Comm()          getIsoBusInstance( getSingletonVecKey() )
#define getCanInstance4Comm()             getCanInstance( getSingletonVecKey() )
#define getTimePosGpsInstance4Comm()      getTimePosGpsInstance( getSingletonVecKey() )
#define getTracGeneralInstance4Comm()     getTracGeneralInstance( getSingletonVecKey() )
#define getTracFacilitiesInstance4Comm()  getTracFacilitiesInstance( getSingletonVecKey() )
#define getTracMoveInstance4Comm()        getTracMoveInstance( getSingletonVecKey() )
#define getTracLightInstance4Comm()       getTracLightInstance( getSingletonVecKey() )
#define getTracAuxInstance4Comm()         getTracAuxInstance( getSingletonVecKey() )
#define getTracPtoInstance4Comm()         getTracPtoInstance( getSingletonVecKey() )
#define getTracGuidanceInstance4Comm()    getTracGuidanceInstance( getSingletonVecKey() )
#define getTracGuidanceCommandInstance4Comm()  getTracGuidanceCommandInstance( getSingletonVecKey() )
#define getIsoMonitorInstance4Comm()      getIsoMonitorInstance( getSingletonVecKey() )
#define getProcessInstance4Comm()         getProcessInstance( getSingletonVecKey() )
#define getMultiSendInstance4Comm()       getMultiSendInstance( getSingletonVecKey() )
#define getMultiReceiveInstance4Comm()    getMultiReceiveInstance( getSingletonVecKey() )
#define getIsoTerminalInstance4Comm()     getIsoTerminalInstance( getSingletonVecKey() )
#define getIsoRequestPgnInstance4Comm()   getIsoRequestPgnInstance( getSingletonVecKey() )
#define getIsoFilterManagerInstance4Comm() getIsoFilterManagerInstance( getSingletonVecKey() )
#define getProprietaryMessageHandlerInstance4Comm() getProprietaryMessageHandlerInstance( getSingletonVecKey() )
#define getFsManagerInstance4Comm()       getFsManagerInstance( getSingletonVecKey() )

/** the class ClientBase delivers the base information, to concat client class instances
 * with the corresponding server class instance. This is realized by the single
 * attribute singletonVecKey
 */
class ClientBase {
public:
  ClientBase( int ai_singletonVecKey = 0 ) : mi_singletonVecKey(ai_singletonVecKey){};
  ClientBase( const ClientBase& acrc_src )
    : mi_singletonVecKey(acrc_src.mi_singletonVecKey) { };
  int getSingletonVecKey() const { return mi_singletonVecKey;};
  void setSingletonKey( int aiKey ) { mi_singletonVecKey = aiKey;};
protected:
  const ClientBase& operator=( const ClientBase& acrc_src )
  { mi_singletonVecKey = acrc_src.mi_singletonVecKey; return *this;};
private:
  int mi_singletonVecKey;
};

/** PROP_SINGLETON defines the number of proprietary busses to be supported.
  */
#if (PROP_INSTANCE_CNT == 0)
#  define getCanInstance4Prop()   ERROR_not_configured_for_Proprietary_CAN_usage_ERROR
#elif (PROP_INSTANCE_CNT > 1)
#  define getCanInstance4Prop()   ccc getCanInstance( (PRT_INSTANCE_CNT) + getSingletonVecKey() )
#else
#  if (PRT_INSTANCE_CNT == 0)
#    define getCanInstance4Prop()   getCanInstance()
#  else
#    define getCanInstance4Prop()   getCanInstance( (PRT_INSTANCE_CNT) )
#  endif
#endif


/** @todo ON REQUEST TASKING: try to remove these include hacks, as soon as Tasking provides a bugfix
    for their broken version 7.x STL headers.
    Official STL headers like "c166/include.cpp/stl_algobase.h"
    where the C-Header \<string.h\> is imported into the global namespace, prevent
    a later include of \<cstring\> ( the correct C++ standard method ) to get
    functions like memmove into "STL_NAMESPACE::" namespace.
    To avoid comparable problems with other compilers, and to avoid any compatibility
    problems with other correct compilers, but which are not used by the maintainers,
    these includes are used for ALL compiler types ( there is only a small impact on
    compile time - but no impact on resulting program ).
*/
#include <new>
#include <cstring>
#include <cstdio>
#include <cstdlib>  // Include before vector or else CNAMESPACE stuff is screwed up for Tasking
#include <vector>

#define MACRO_MULTITON_CONTRIBUTION(T, SIZE) \
private: \
  int singletonVecKey; \
  int getSingletonVecKey() const { return singletonVecKey; } \
  typedef T Instances_t[SIZE]

#define MACRO_MULTITON_GET_INSTANCE_BODY(T, instance) \
  static T::Instances_t st_instances; \
  for (int i = 0; i < int(sizeof st_instances / sizeof st_instances[0]); ++i) { \
    /* initialise the instance */ \
    st_instances[i].singletonVecKey = i; \
  } \
  return st_instances[instance]

#define MACRO_SINGLETON_CONTRIBUTION(T) MACRO_MULTITON_CONTRIBUTION(T, 1)

#endif // SINGLETON_H_

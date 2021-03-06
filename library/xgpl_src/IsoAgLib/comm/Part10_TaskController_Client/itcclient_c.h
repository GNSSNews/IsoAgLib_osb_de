/*
  itcclient_c.h: central managing instance for all process data

  (C) Copyright 2009 - 2019 by OSB AG

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Usage under Commercial License:
  Licensees with a valid commercial license may use this file
  according to their commercial license agreement. (To obtain a
  commercial license contact OSB AG via <http://isoaglib.com/en/contact>)

  Usage under GNU General Public License with exceptions for ISOAgLib:
  Alternatively (if not holding a valid commercial license)
  use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/
#ifndef ITCCLIENT_C_H
#define ITCCLIENT_C_H

#include "impl/tcclient_c.h"
#include "iprocdata.h"
#include "idevicepool_c.h"
#include "itcclientconnection_c.h"
#include "ipdpool_c.h"
#include "ipdconnection_c.h"
#include <IsoAgLib/comm/Part5_NetworkManagement/iidentitem_c.h>


namespace IsoAgLib {

  class iTcClient_c : private __IsoAgLib::TcClient_c
  {
  public:
      class iProvider_c : public __IsoAgLib::TcClient_c::Provider_c {
      public:
        virtual uint32_t provideDistance() const = 0;
        virtual uint16_t provideSpeed() const = 0;
      };

      // If a Distance-measurement is used, a provider needs to be supplied.
      void setProvider( iProvider_c * );

      // Client Connections (with DDOP-Upload)
      bool registerClient( iIdentItem_c& ident, const IsoAgLib::ProcData::ClientCapabilities_s& caps, IsoAgLib::iTcClientConnection_c::iStateHandler_c &sh );
      bool deregisterClient( iIdentItem_c& );

      iTcClientConnection_c* doConnect( const iIdentItem_c&, const iTcClientServer_c&, iDevicePool_c& );
      void                 dontConnect( const iIdentItem_c&, const iTcClientServer_c& );

      // Peer Connections (direct PD, no DDOP). Must NOT be a TC/DL.
      iPdConnection_c* connectPeer(          const iIdentItem_c&, const iIsoItem_c& pdItem, iPdPool_c& );
      iPdConnection_c* connectPeerBroadcast( const iIdentItem_c&, iPdPool_c& );

      void disconnectPeer( const iIdentItem_c & );
      void disconnectPeer( const iPdConnection_c & );

      // Do NOT call with "available=false" when "RemovedFromMonitorList" - this will be done automatically internally already!
      void proprietaryServer( const iIsoItem_c &, bool available );

#if 0
// currently not supported
      void processChangeDesignator( iIdentItem_c& identItem, uint16_t objID, const char* newDesignator );
#endif


#if defined(USE_DIRECT_PD_HANDLING)
      /// /////////////////////////////////////////////////
      /// WARNING: only use if you know what you are doing!
      /// /////////////////////////////////////////////////
      class iPdMessageHandler_c : private __IsoAgLib::TcClient_c::PdMessageHandler_c {
        public:
          virtual void eventPdMessageReceived(
              const iIsoItem_c& sa_item, 
              const iIsoItem_c* da_item,
              IsoAgLib::ProcData::CommandType_t command, uint16_t element, uint16_t ddi, int32_t value) = 0;

        private:
          virtual void _eventPdMessageReceived(
              const __IsoAgLib::IsoItem_c& sa_item, 
              const __IsoAgLib::IsoItem_c* da_item,
              IsoAgLib::ProcData::CommandType_t command, uint16_t element, uint16_t ddi, int32_t value) {
            eventPdMessageReceived(
                (const iIsoItem_c&) sa_item ,
                (const iIsoItem_c*) da_item ,
                command, element, ddi, value );
          }
          friend class iTcClient_c;
      };

      void setPdMessageHandler( iPdMessageHandler_c& hdl ) {
        TcClient_c::setPdMessageHandler( hdl );
      }
 
      void clearPdMessageHandler() {
        TcClient_c::clearPdMessageHandler();
      }

      void sendPdMessage( const iIsoItem_c& sa_item, 
                          const iIsoItem_c* da_item,
                          IsoAgLib::ProcData::CommandType_t command,
                          uint16_t element,
                          uint16_t ddi,
                          int32_t value ) {
        return TcClient_c::sendPdMessage(
            static_cast<const __IsoAgLib::IsoItem_c&>( sa_item ),
            static_cast<const __IsoAgLib::IsoItem_c*>( da_item ),
            command, element, ddi, value );
      }
#endif

    private:
#if ( PRT_INSTANCE_CNT == 1 )
      friend iTcClient_c& getItcClientInstance();
#endif
      friend iTcClient_c& getItcClientInstance( unsigned instance );
  };


#if ( PRT_INSTANCE_CNT == 1 )
  inline iTcClient_c& getItcClientInstance() {
    return static_cast<iTcClient_c&>( __IsoAgLib::getTcClientInstance( 0 ) );
  }
#endif
  inline iTcClient_c& getItcClientInstance( unsigned instance ) {
    return static_cast<iTcClient_c&>( __IsoAgLib::getTcClientInstance( instance ) );
  }

  inline void iTcClient_c::setProvider( iProvider_c *provider )
  {
    TcClient_c::setProvider( provider );
  }

  inline bool iTcClient_c::registerClient( iIdentItem_c& ident, const IsoAgLib::ProcData::ClientCapabilities_s& caps, IsoAgLib::iTcClientConnection_c::iStateHandler_c &sh )
  {
    return TcClient_c::registerClient( static_cast<__IsoAgLib::IdentItem_c&>( ident ), caps, static_cast< __IsoAgLib::TcClientConnection_c::StateHandler_c &>( sh ) );
  }

  inline bool iTcClient_c::deregisterClient( iIdentItem_c& ident )
  {
    return TcClient_c::deregisterClient( ident );
  }
      
  inline iTcClientConnection_c*
  iTcClient_c::doConnect( const iIdentItem_c& identItem, const iTcClientServer_c& server, iDevicePool_c& pool )
  {
    return static_cast<iTcClientConnection_c*>( TcClient_c::doConnect(
        static_cast<const __IsoAgLib::IdentItem_c&>( identItem ),
        static_cast<const __IsoAgLib::ServerInstance_c&>( server ),
        static_cast<__IsoAgLib::DevicePool_c&>( pool ) ) );
  }
 
  inline void
  iTcClient_c::dontConnect( const iIdentItem_c& identItem, const iTcClientServer_c& server )
  {
    TcClient_c::dontConnect(
        static_cast<const __IsoAgLib::IdentItem_c&>( identItem ),
        static_cast<const __IsoAgLib::ServerInstance_c&>( server ) );
  }

  inline iPdConnection_c*
  iTcClient_c::connectPeer( const iIdentItem_c& identItem, const iIsoItem_c& pdItem, iPdPool_c& pool )
  {
    return static_cast<iPdConnection_c*>( TcClient_c::connectPeer(
        static_cast<const __IsoAgLib::IdentItem_c&>( identItem ),
        static_cast<const __IsoAgLib::IsoItem_c&>( pdItem ),
        static_cast<__IsoAgLib::PdPool_c&>( pool ) ) );
  }

  inline iPdConnection_c*
  iTcClient_c::connectPeerBroadcast( const iIdentItem_c& identItem, iPdPool_c& pool )
  {
    return static_cast<iPdConnection_c*>( TcClient_c::connectPeerBroadcast(
        static_cast<const __IsoAgLib::IdentItem_c&>( identItem ),
        static_cast<__IsoAgLib::PdPool_c&>( pool ) ) );
  }

  inline void iTcClient_c::disconnectPeer( const iIdentItem_c & identItem )
  {
    TcClient_c::disconnectPeer( static_cast<const __IsoAgLib::IdentItem_c&>( identItem ) );
  }


  inline void iTcClient_c::disconnectPeer( const iPdConnection_c& connection )
  {
    TcClient_c::disconnectPeer( static_cast<const __IsoAgLib::PdConnection_c&>( connection ) );
  }

  inline void iTcClient_c::proprietaryServer( const iIsoItem_c &item, bool available )
  {
    TcClient_c::proprietaryServer( static_cast<const __IsoAgLib::IsoItem_c&>( item ), available );
  }

#if 0
// currently not supported
  inline void iTcClient_c::processChangeDesignator( iIdentItem_c& identItem, uint16_t objID, const char* newDesignator )
  {
    return TcClient_c::processChangeDesignator( static_cast<__IsoAgLib::IdentItem_c&>( identItem ), objID, newDesignator );
  }
#endif

}

#endif

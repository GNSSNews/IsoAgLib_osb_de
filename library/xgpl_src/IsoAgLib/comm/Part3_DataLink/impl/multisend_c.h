/*
  multisend_c.h: transfer of multi message data

  (C) Copyright 2009 - 2010 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/
#ifndef MULTI_SEND_H
#define MULTI_SEND_H


/* *************************************** */
/* ********** include headers ************ */
/* *************************************** */
#include <IsoAgLib/hal/hal_typedef.h>
#include <IsoAgLib/util/config.h>
#include <IsoAgLib/driver/can/impl/cancustomer_c.h>
#include <IsoAgLib/util/impl/singleton.h>
#include <IsoAgLib/scheduler/impl/schedulertask_c.h>
#include <IsoAgLib/comm/Part5_NetworkManagement/impl/isomonitor_c.h>
#include <IsoAgLib/comm/Part5_NetworkManagement/impl/isoname_c.h>

#include "multisendpkg_c.h"
#include "../imultisendstreamer_c.h"

#include <list>
#include <functional>


// Begin Namespace __IsoAgLib
namespace __IsoAgLib {

// forward declarations
class vtObjectString_c;


class SendUploadBase_c {
public:
  SendUploadBase_c () : vec_uploadBuffer() {}

  SendUploadBase_c (uint8_t byte1, uint8_t byte2, uint8_t byte3, uint8_t byte4, uint8_t byte5, uint8_t byte6, uint8_t byte7, uint8_t byte8, uint8_t byte9)
    { set(byte1, byte2, byte3, byte4, byte5, byte6, byte7, byte8, byte9); }

  SendUploadBase_c (uint8_t byte1, uint8_t byte2, uint8_t byte3, uint8_t byte4, uint8_t byte5, uint8_t byte6, uint8_t byte7, uint8_t byte8)
    {set(byte1, byte2, byte3, byte4, byte5, byte6, byte7, byte8); }

  SendUploadBase_c (const SendUploadBase_c& r_source);

  SendUploadBase_c (uint8_t* apui8_buffer, uint32_t bufferSize)
    { set(apui8_buffer, bufferSize); }

  const SendUploadBase_c& operator= (const SendUploadBase_c& r_source);

  void set (uint8_t byte1, uint8_t byte2, uint8_t byte3, uint8_t byte4, uint8_t byte5, uint8_t byte6, uint8_t byte7, uint8_t byte8, uint8_t byte9);
  void set (uint8_t byte1, uint8_t byte2, uint8_t byte3, uint8_t byte4, uint8_t byte5, uint8_t byte6, uint8_t byte7, uint8_t byte8);
  void set (uint8_t* apui8_buffer, uint32_t bufferSize);

  /// Use either a MultiSendStreamer or a direct ui8-Buffer
  STL_NAMESPACE::vector<uint8_t> vec_uploadBuffer;  // don't use malloc_alloc for uint8_t values - here the 8byte overhead per malloc item are VERY big
  // ==> chunk allocation which can be shared among instances is alot better
};


/**
  This class implements the ISO 11783 (Extended) Transport Protocol
  and FastPacket for send of more than 8 bytes of data.

  @author Dipl.-Inform. Achim Spangler
  @author Dipl.-Inf. Martin Wodok
*/
class MultiSend_c;
typedef SINGLETON_DERIVED(MultiSend_c, Scheduler_Task_c) SingletonMultiSend_c;
class MultiSend_c : public SingletonMultiSend_c
{
public:
  enum sendState_t   { AwaitCts, SendData, SendPauseTillCts, AwaitEndofmsgack };
  enum sendSuccess_t { SendSuccess, SendAborted, Running };
  enum msgType_t     { IsoTP, IsoETP, IsoTPbroadcast, NmeaFastPacket };


  class SendStream_c : public ClientBase
  {
  public:

    SendStream_c (MultiSend_c& arc_multiSend SINGLETON_VEC_KEY_PARAMETER_DEF_WITH_COMMA ) : SINGLETON_PARENT_CONSTRUCTOR mrc_multiSend (arc_multiSend)
      {}; // does NOT initialize anything, use "init(...)" directly after construction!!!!
    /**
      call "init" directly after construction!
    */
    void init (const IsoName_c& acrc_isoNameSender, const IsoName_c& acrc_isoNameReceiver, const HUGE_MEM uint8_t* rhpb_data, uint32_t aui32_dataSize, sendSuccess_t& rpen_sendSuccessNotify, uint32_t aui32_pgn, IsoAgLib::iMultiSendStreamer_c* apc_mss, msgType_t ren_msgType);

    /**
      perform periodical actions
      @return true -> remove me, I'm finished!
    */
    bool timeEvent (uint8_t aui8_pkgCnt);

    /**
      start processing of a process msg
      that matches this SendStream
    */
    bool processMsg();

    int32_t getNextTriggerTime() const { return mi32_timestampToWaitTo; }

    /**
      check if time delay is reached
      @return true -> time delay is over
    */
    bool timeHasCome() const {return (System_c::getTime() - mi32_timestampToWaitTo) >= 0; }

    /**
      check if current message is complete
    */
    bool isCompleteBurst() const { return (mui8_packetsLeftToSendInBurst == 0); }

    /**
      check if send of all data is complete
    */
    bool isCompleteData() const { return (mui32_dataBufferOffset >= mui32_dataSize); }

    /**
      deliver the data size in byte
      @return data size in uint32_t for RTS
    */
    uint32_t getDataSize() const { return mui32_dataSize; }

    bool isFinished() const { return (*mpen_sendSuccessNotify == SendAborted) || (*mpen_sendSuccessNotify == SendSuccess); }

    /**
      abort the multipacket send stream
    */
    void abortSend ();

    bool matchSaDa (const IsoName_c& acrc_sa, const IsoName_c& acrc_da) { return (acrc_sa == mc_isoNameSender) && (acrc_da == mc_isoNameReceiver); }

//    bool matchSa (uint8_t aui8_sa) { return (aui8_sa == ui8_sender); }
//    bool matchDa (uint8_t aui8_da) { return (aui8_da == ui8_receiver); }

  private: // methods
    /**
      send an ISO message -> set the ident (depending on ab_data) and initiate sending to CAN
      @param ab_data true -> use the (E)TP_DATA_TRANSFER_PGN
                     false-> use the (E)TP_CONN_MANAGE_PGN
                     --Value is not of interest in Fast-Packet sending!--
    */
    void sendPacketIso (bool ab_data);

#if defined (ENABLE_MULTIPACKET_VARIANT_FAST_PACKET)
    /**
      send a FP message -> set the ident and initiate sending to CAN
    */
    void sendPacketFp();
#endif

    /**
      calculate the actual sequence number and
      calculate the amount of data bytes which must be placed in new CAN pkg
      @param ui8_nettoDataCnt amount of data which should be sent within this msg
    */
    void prepareSendMsg (uint8_t &ui8_nettoDataCnt);

    void retriggerIn (int32_t i32_timeDelta) { mi32_timestampToWaitTo = System_c::getTime() + i32_timeDelta;}

    void switchToState (sendState_t ren_sendState, int32_t i32_timeDelta) { men_sendState = ren_sendState; retriggerIn (i32_timeDelta); }

    const SendStream_c& operator= (const SendStream_c&); // Only private declaration, no implementation: Forbid assignment of SendStream_c instances.

  private: // attributes
  /// Initialized on init(...)
    uint32_t mui32_pgn;
    IsoName_c mc_isoNameReceiver;
    IsoName_c mc_isoNameSender;

    #if defined (ENABLE_MULTIPACKET_VARIANT_FAST_PACKET)
    uint8_t mui8_FpSequenceCounter;
    #endif

    /** timestamp for time control */
    int32_t mi32_timestampToWaitTo;

    /** data counter for data to send
        will be set on CTS and incremented after each sent packet. */
    uint32_t mui32_dataBufferOffset;

    /** size of the data complete */
    uint32_t mui32_dataSize;

    /** standard delay between two sent packets (between 50 and 200)
    uint16_t ui16_delay;
    __-> now using maxDelay from MultiSend, because not everybody can have its own delay, we'll have to take the max. */

    /** pointer to the data */
    const HUGE_MEM uint8_t* mhpbui8_data;

    /** actual send state during Running process */
    sendState_t men_sendState;

    /** reference to variable that will be set upon state change */
    sendSuccess_t* mpen_sendSuccessNotify;

    /** are we broadcast, iso, extended, fast packet? */
    msgType_t men_msgType;

    /** pointer to an IsoAgLib::MultiSendStreamer_c class which streams out parts of the stream step by step */
    IsoAgLib::iMultiSendStreamer_c* mpc_mss;

    /** save ... from last CTS so we can see if the CTS was resent... */
    uint32_t mui32_packetNrRequestedInLastCts;

    /** cnt of pkg left to send in this burst */
    uint8_t mui8_packetsLeftToSendInBurst;

    /** cnt of pkg sent since the last DPO (ETP) - now also used to TP */
    uint8_t mui8_packetsSentInThisBurst;

  /// Back reference to MultiSend_c for setting the MAX of all delays, this can only be managed here...
    MultiSend_c& mrc_multiSend;
  }; // end class SendStream_c


public: // methods

  /** initialisation for MultiSend_c */
  void init();
  /** every subsystem of IsoAgLib has explicit function for controlled shutdown */
  void close();

  /** default destructor which has nothing special to do (only call close!) */
  virtual ~MultiSend_c() { close(); }

  /** this function is called by IsoMonitor_c on addition, state-change and removal of an IsoItem.
   * @param at_action enumeration indicating what happened to this IsoItem. @see IsoItemModification_en / IsoItemModification_t
   * @param acrc_isoItem reference to the (const) IsoItem which is changed (by existance or state)
   */
  void reactOnIsoItemModification (ControlFunctionStateHandler_c::IsoItemModification_t /*at_action*/, IsoItem_c const& /*acrc_isoItem*/);


  /**
    Send an ISO 11738 (E)TP targeted multipacket message using a given MultiSendStreamer
    @param acrc_isoNameSender ISOName of sender
    @param acrc_isoNameReceiver ISOName of receiver
    @param apc_mss Allow active build of data stream parts for upload by deriving data source class
                   from IsoAgLib::iMultiSendStreamer_c, which defines virtual functions to control the
                   retrieve of data to send. This is especially important for the VT-Client,
                   which assembles the data pool dependent on the terminal capabilities during upload
                   (e.g. bitmap variants, color-depth, positions, etc.)
    @param ai32_pgn PGN of the data
    @param rpen_sendSuccessNotify Pointer to send state var, where the current state is written by MultiSend_c
    @return true -> MultiSend_c was ready -> Transfer was started
  */
  bool sendIsoTarget (const IsoName_c& acrc_isoNameSender, const IsoName_c& acrc_isoNameReceiver, IsoAgLib::iMultiSendStreamer_c* apc_mss, int32_t ai32_pgn, sendSuccess_t& rpen_sendSuccessNotify);

  /**
    Send an ISO 11738 (E)TP targeted multipacket message using a given data-buffer
    @param acrc_isoNameSender ISOName of sender
    @param acrc_isoNameReceiver ISOName of receiver
    @param rhpb_data HUGE_MEM pointer to the data
    @param aui32_dataSize size of the complete data (should be >= 9 of course)
    @param ai32_pgn PGN of the data
    @param rpen_sendSuccessNotify Pointer to send state var, where the current state is written by MultiSend_c
    @return true -> MultiSend_c was ready -> Transfer was started
  */
  bool sendIsoTarget (const IsoName_c& acrc_isoNameSender, const IsoName_c& acrc_isoNameReceiver, const HUGE_MEM uint8_t* rhpb_data, uint32_t aui32_dataSize, int32_t ai32_pgn, sendSuccess_t& rpen_sendSuccessNotify);

  /**
    Send an ISO 11783 (E)TP broadcast multipacket message using a given data-buffer
    @param acrc_isoNameSender ISOName of sender
    @param rhpb_data HUGE_MEM pointer to the data
    @param aui16_dataSize Size of the complete buffer (should be in the range of [9..1785])
    @param ai32_pgn PGN of the data
    @param rpen_sendSuccessNotify Pointer to send state var, where the current state is written by MultiSend_c
    @return true -> MultiSend_c was ready -> Transfer was started
  */
  bool sendIsoBroadcast (const IsoName_c& acrc_isoNameSender, const HUGE_MEM uint8_t* rhpb_data, uint16_t aui16_dataSize, int32_t ai32_pgn, sendSuccess_t& rpen_sendSuccessNotify)
    { return sendIntern (acrc_isoNameSender, IsoName_c::IsoNameUnspecified(), rhpb_data, aui16_dataSize, rpen_sendSuccessNotify, ai32_pgn, NULL /* NOT "yet" supported */, IsoTPbroadcast); }


  /**
    Send an ISO 11783 (E)TP broadcast multipacket message using a given data-buffer. 
    Will check the size of the message and decide whether or not to use the transport protocol
    @param acrc_isoNameSender ISOName of sender
    @param rhpb_data HUGE_MEM pointer to the data
    @param aui16_dataSize Size of the complete buffer (should be >= 9 of course)
    @param ai32_pgn PGN of the data
    @param rpen_sendSuccessNotify Pointer to send state var, where the current state is written by MultiSend_c
    @return true -> MultiSend_c was ready -> Transfer was started
  */
  bool sendIsoBroadcastOrSinglePacket (const IsoName_c& acrc_isoNameSender, const HUGE_MEM uint8_t* rhpb_data, uint16_t aui16_dataSize, int32_t ai32_pgn, sendSuccess_t& rpen_sendSuccessNotify);

  /**
    send an ISO 11783 (E)TP broadcast multipacket message using a given MultiSendStreamer
    @param acrc_isoNameSender ISOName of sender
    @param apc_mss Allow active build of data stream parts for upload by deriving data source class
                   from IsoAgLib::iMultiSendStreamer_c, which defines virtual functions to control the
                   retrieve of data to send.
    @param ai32_pgn PGN of the data
    @param rpen_sendSuccessNotify Pointer to send state var, where the current state is written by MultiSend_c
    @return true -> MultiSend_c was ready -> Transfer was started
  */
  bool sendIsoBroadcast(const IsoName_c& acrc_isoNameSender, IsoAgLib::iMultiSendStreamer_c* apc_mss, int32_t ai32_pgn, sendSuccess_t& rpen_sendSuccessNotify)
    { return sendIntern(acrc_isoNameSender, IsoName_c::IsoNameUnspecified(), NULL, apc_mss->getStreamSize(), rpen_sendSuccessNotify, ai32_pgn, apc_mss, IsoTPbroadcast);}


#if defined(ENABLE_MULTIPACKET_VARIANT_FAST_PACKET)
  /**
    send a FastPacket targeted multipacket message using a given uint8_t* buffer
    @param acrc_isoNameSender ISOName of sender
    @param acrc_isoNameReceiver ISOName of receiver
    @param apc_mss Allow active build of data stream parts for upload by deriving data source class
                   from IsoAgLib::iMultiSendStreamer_c, which defines virtual functions to control the
                   retrieve of data to send.
    @param ai32_pgn PGN of the data
    @param rpen_sendSuccessNotify Pointer to send state var, where the current state is written by MultiSend_c
    @return true -> MultiSend_c was ready -> Transfer was started
  */
  bool sendIsoFastPacket (const IsoName_c& acrc_isoNameSender, const IsoName_c& acrc_isoNameReceiver, HUGE_MEM uint8_t* rhpb_data, uint16_t aui32_dataSize, int32_t ai32_pgn, sendSuccess_t& rpen_sendSuccessNotify)
    { return sendIntern (acrc_isoNameSender, acrc_isoNameReceiver, rhpb_data, aui32_dataSize, rpen_sendSuccessNotify, ai32_pgn, NULL, NmeaFastPacket); }

  /**
    Send a FastPacket targeted multipacket message using a given MultiSendStreamer
    @param acrc_isoNameSender ISOName of sender
    @param acrc_isoNameReceiver ISOName of receiver
    @param rhpb_data HUGE_MEM pointer to the data
    @param aui32_dataSize size of the complete data (should be >= 9 of course)
    @param ai32_pgn PGN of the data
    @param rpen_sendSuccessNotify Pointer to send state var, where the current state is written by MultiSend_c
    @return true -> MultiSend_c was ready -> Transfer was started
  */
  bool sendIsoFastPacket (const IsoName_c& acrc_isoNameSender, const IsoName_c& acrc_isoNameReceiver, IsoAgLib::iMultiSendStreamer_c* apc_mss, int32_t ai32_pgn, sendSuccess_t& rpen_sendSuccessNotify)
    { return sendIntern (acrc_isoNameSender, acrc_isoNameReceiver, NULL, apc_mss->getStreamSize(), rpen_sendSuccessNotify, ai32_pgn, apc_mss, NmeaFastPacket); }

  /**
    Send a FastPacket broadcast multipacket message using a given data-buffer
    @param acrc_isoNameSender ISOName of sender
    @param rhpb_data HUGE_MEM pointer to the data
    @param aui16_dataSize Size of the complete buffer (should be >= 9 of course)
    @param ai32_pgn PGN of the data
    @param rpen_sendSuccessNotify Pointer to send state var, where the current state is written by MultiSend_c
    @return true -> MultiSend_c was ready -> Transfer was started
  */
  bool sendIsoFastPacketBroadcast (const IsoName_c& acrc_isoNameSender, HUGE_MEM uint8_t* rhpb_data, uint16_t aui16_dataSize, int32_t ai32_pgn, sendSuccess_t& rpen_sendSuccessNotify)
    { return sendIntern (acrc_isoNameSender, IsoName_c::IsoNameUnspecified(), rhpb_data, aui16_dataSize, rpen_sendSuccessNotify, ai32_pgn, NULL, NmeaFastPacket); }

  /**
    Send a FastPacket broadcast multipacket message using a given MultiSendStreamer
    @param acrc_isoNameSender ISOName of sender
    @param apc_mss Allow active build of data stream parts for upload by deriving data source class
                   from IsoAgLib::iMultiSendStreamer_c, which defines virtual functions to control the
                   retrieve of data to send.
    @param ai32_pgn PGN of the data
    @param rpen_sendSuccessNotify Pointer to send state var, where the current state is written by MultiSend_c
    @return true -> MultiSend_c was ready -> Transfer was started
  */
  bool sendIsoFastPacketBroadcast (const IsoName_c& acrc_isoNameSender, IsoAgLib::iMultiSendStreamer_c* apc_mss, int32_t ai32_pgn, sendSuccess_t& rpen_sendSuccessNotify)
    { return sendIntern (acrc_isoNameSender, IsoName_c::IsoNameUnspecified(), NULL, apc_mss->getStreamSize(), rpen_sendSuccessNotify, ai32_pgn, apc_mss, NmeaFastPacket); }
#endif

  /**
    set the delay between two sent messages, called by SendStream_c's timeEvent() when neccessary
    @param aui16_delay wanted delay
  */
  void setSendStreamDelay(uint16_t aui16_delay);

  /**
    deliver reference to data pkg
    @return reference to MultiSendPkg_c which handles CAN I/O of process data
  */
  MultiSendPkg_c& data() { return mc_data; }

  /**
    deliver reference to data pkg as reference to CanPkgExt_c
    to implement the base virtual function correct
  */
  virtual CanPkgExt_c& dataBase() { return mc_data; }

  /**
    deliver reference to data pkg for const read access
    @return const reference to MultiSendPkg_c which handles CAN I/O of process data
  */
  const MultiSendPkg_c& constData() const { return mc_data; }

  /**
    start processing of a process msg: delegate to specific SendStream_c instance
  */
  bool processMsg();

  /**
    perform periodical actions
    @return true -> all planned activities performed in allowed time
  */
  bool timeEvent();

  /** user function for explicit abort of any running matching stream. */
  void abortSend (const IsoName_c& acrc_isoNameSender, const IsoName_c& acrc_isoNameReceiver);

  /** check if at least one multisend stream is running */
  bool isMultiSendRunning() const { return (!mlist_sendStream.empty()); }

  ///  Used for Debugging Tasks in Scheduler_c
  virtual const char* getTaskName() const;

protected: // methods
  //! Function set ui16_earlierInterval and
  //! ui16_laterInterval that will be used by
  //! getTimeToNextTrigger(retriggerType_t)
  //! can be overloaded by Childclass for special condition
  virtual void updateEarlierAndLatestInterval();


private:
  class CanCustomerProxy_c : public CanCustomer_c {
  public:
    typedef MultiSend_c Owner_t;

    CanCustomerProxy_c(Owner_t &art_owner) : mrt_owner(art_owner) {}

    virtual ~CanCustomerProxy_c() {}

  private:
    virtual CanPkgExt_c& dataBase() {
      return mrt_owner.dataBase();
    }

    virtual bool processMsg() {
      return mrt_owner.processMsg();
    }

    virtual bool processInvalidMsg() {
      return mrt_owner.processInvalidMsg();
    }

    virtual bool isNetworkMgmt() const {
      return mrt_owner.isNetworkMgmt();
    }

    virtual bool reactOnStreamStart(
        ReceiveStreamIdentifier_c const &ac_ident,
        uint32_t aui32_totalLen)
    {
      return mrt_owner.reactOnStreamStart(ac_ident, aui32_totalLen);
    }

    virtual void reactOnAbort(Stream_c &arc_stream)
    {
      mrt_owner.reactOnAbort(arc_stream);
    }

    virtual bool processPartStreamDataChunk(
        Stream_c &apc_stream,
        bool ab_isFirstChunk,
        bool ab_isLastChunk)
    {
      return mrt_owner.processPartStreamDataChunk(
          apc_stream,
          ab_isFirstChunk,
          ab_isLastChunk);
    }

    virtual void notificationOnMultiReceiveError(
        ReceiveStreamIdentifier_c const &ac_streamIdent,
        uint8_t aui8_multiReceiveError,
        bool ab_isGlobal)
    {
      mrt_owner.notificationOnMultiReceiveError(
          ac_streamIdent,
          aui8_multiReceiveError,
          ab_isGlobal);
    }

    // CanCustomerProxy_c shall not be copyable. Otherwise the
    // reference to the containing object would become invalid.
    CanCustomerProxy_c(CanCustomerProxy_c const &);

    CanCustomerProxy_c &operator=(CanCustomerProxy_c const &);

    Owner_t &mrt_owner;
  };
  typedef CanCustomerProxy_c Customer_t;
  class ControlFunctionStateHandlerProxy_c : public ControlFunctionStateHandler_c {
  public:
    typedef MultiSend_c Owner_t;

    ControlFunctionStateHandlerProxy_c(Owner_t &art_owner) : mrt_owner(art_owner) {}

    virtual ~ControlFunctionStateHandlerProxy_c() {}

  private:
    virtual void reactOnIsoItemModification(
        IsoItemModification_t at_action,
        IsoItem_c const &acrc_isoItem)
    {
      mrt_owner.reactOnIsoItemModification(at_action, acrc_isoItem);
    }

    // ControlFunctionStateHandlerProxy_c shall not be copyable. Otherwise the
    // reference to the containing object would become invalid.
    ControlFunctionStateHandlerProxy_c(ControlFunctionStateHandlerProxy_c const &);

    ControlFunctionStateHandlerProxy_c &operator=(ControlFunctionStateHandlerProxy_c const &);

    Owner_t &mrt_owner;
  };
  typedef ControlFunctionStateHandlerProxy_c Handler_t;

  // Private methods
  friend class SINGLETON_DERIVED(MultiSend_c, Scheduler_Task_c);
  friend class iMultiSend_c;
  friend class SendStream_c;

  /**
    HIDDEN constructor for a MultiSend_c object instance
    NEVER instantiate a variable of type MultiSend_c within application
    only access MultiSend_c via getMultiSendInstance() or getMultiSendInstance( int riLbsBusNr )
    in case more than one ISO11783 BUS is used for IsoAgLib
    */
  MultiSend_c();

  /**
    initialize directly after the singleton instance is created.
    this is called from singleton.h and should NOT be called from the user again.
    users please use init(...) instead.
  */
  void singletonInit();

  SendStream_c* addSendStream (const IsoName_c& acrc_isoNameSender, const IsoName_c& acrc_isoNameReceiver);
  SendStream_c* getSendStream (const IsoName_c& acrc_isoNameSender, const IsoName_c& acrc_isoNameReceiver);

  /** this function should NOT be called from INSIDE of timeEvent !
      ==> Only call it from OUTSIDE functions like init(), processMsg(), addSendStream, etc.
  */
  void calcAndSetNextTriggerTime();

  /**
    internal function to send a ISO target multipacket message
    @param acrc_isoNameSender
    @param acrc_isoNameReceiver
    @param rhpb_data HUGE_MEM pointer to the data
    @param ai32_dataSize size of the complete mask
    @param rpen_sendSuccessNotify -> pointer to send state var, where the current state
            is written by MultiSend_c
    @param ai32_pgn PGN to use for the upload
    @param apc_mss allow active build of data stream parts for upload by deriving data source class
                  from IsoAgLib::MultiSendStreamer_c, which defines virtual functions to control the
                  retrieve of data to send. This is especially important for ISO_Terminal,
                  which assembles the data pool dependent on the terminal capabilities during upload
                  ( e.g. bitmap variants )
    @return true -> MultiSend_c was ready -> mask is spooled to target
  */
  bool sendIntern (const IsoName_c& acrc_isoNameSender, const IsoName_c& acrc_isoNameReceiver, const HUGE_MEM uint8_t* rhpb_data, uint32_t aui32_dataSize, sendSuccess_t& rpen_sendSuccessNotify, int32_t ai32_pgn, IsoAgLib::iMultiSendStreamer_c* apc_mss, msgType_t ren_msgType);

  #if defined(ENABLE_MULTIPACKET_VARIANT_FAST_PACKET)
  uint8_t allocFpSequenceCounter() { const uint8_t cui8_returnVal = mui8_nextFpSequenceCounter;
                                     mui8_nextFpSequenceCounter = (mui8_nextFpSequenceCounter+1) & 0x7;
                                     return cui8_returnVal; }
  #endif

  virtual bool processInvalidMsg() {
    return false;
  }

  virtual bool processPartStreamDataChunk(
      Stream_c &apc_stream,
      bool ab_isFirstChunk,
      bool ab_isLastChunk)
  {
    return mt_customer.processPartStreamDataChunkDefault(
        apc_stream,
        ab_isFirstChunk,
        ab_isLastChunk);
  }

  virtual void reactOnAbort(Stream_c &arc_stream)
  {
    mt_customer.reactOnAbortDefault(arc_stream);
  }

  virtual bool reactOnStreamStart(
      ReceiveStreamIdentifier_c const &ac_ident,
      uint32_t aui32_totalLen)
  {
    return mt_customer.reactOnStreamStartDefault(ac_ident, aui32_totalLen);
  }

  virtual bool isNetworkMgmt() const {
    return false;
  }

  virtual void notificationOnMultiReceiveError(
      ReceiveStreamIdentifier_c const &ac_streamIdent,
      uint8_t aui8_multiReceiveError,
      bool ab_isGlobal)
  {
    mt_customer.notificationOnMultiReceiveErrorDefault(
        ac_streamIdent,
        aui8_multiReceiveError,
        ab_isGlobal);
  }

  virtual uint16_t getForcedMinExecTime() const
  {
    return getForcedMinExecTimeDefault();
  }

  static msgType_t protocolTypeByPacketSize(uint32_t ui32_size);
private:
  enum {
    beginSinglePacketSize = 0,
    endSinglePacketSize = 9,
    endNmeaFastPacketSize = 224,
    beginTpPacketSize = endSinglePacketSize,
    endTpPacketSize = 1786,
    beginEtpPacketSize = endTpPacketSize,
  };
  // Private attributes
  #if defined(ENABLE_MULTIPACKET_VARIANT_FAST_PACKET)
  uint8_t mui8_nextFpSequenceCounter;
  #endif

  /** msg object for CAN I/O */
  MultiSendPkg_c mc_data;

  STL_NAMESPACE::list<SendStream_c> mlist_sendStream;
  Handler_t mt_handler;
  Customer_t mt_customer;
};

inline MultiSend_c::msgType_t MultiSend_c::protocolTypeByPacketSize(uint32_t ui32_size)
{
  return ui32_size >= beginEtpPacketSize ? IsoETP : IsoTP;
}

inline bool MultiSend_c::sendIsoTarget(const IsoName_c& acrc_isoNameSender, const IsoName_c& acrc_isoNameReceiver, IsoAgLib::iMultiSendStreamer_c* apc_mss, int32_t ai32_pgn, sendSuccess_t& rpen_sendSuccessNotify)
{
  return sendIntern(acrc_isoNameSender, acrc_isoNameReceiver, NULL, apc_mss->getStreamSize(), rpen_sendSuccessNotify, ai32_pgn, apc_mss, protocolTypeByPacketSize(apc_mss->getStreamSize()));
}

inline bool MultiSend_c::sendIsoTarget (const IsoName_c& acrc_isoNameSender, const IsoName_c& acrc_isoNameReceiver, const HUGE_MEM uint8_t* rhpb_data, uint32_t aui32_dataSize, int32_t ai32_pgn, sendSuccess_t& rpen_sendSuccessNotify)
{
  return sendIntern(acrc_isoNameSender, acrc_isoNameReceiver, rhpb_data, aui32_dataSize, rpen_sendSuccessNotify, ai32_pgn, NULL, protocolTypeByPacketSize(aui32_dataSize));
}

#if defined( PRT_INSTANCE_CNT ) && ( PRT_INSTANCE_CNT > 1 )
  /** C-style function, to get access to the unique MultiSend_c singleton instance
    * if more than one CAN BUS is used for IsoAgLib, an index must be given to select the wanted BUS
    */
  MultiSend_c& getMultiSendInstance( uint8_t aui8_instance = 0 );
#else
  /** C-style function, to get access to the unique Process_c singleton instance */
  MultiSend_c& getMultiSendInstance( void );
#endif

} // End Namespace __IsoAgLib
#endif

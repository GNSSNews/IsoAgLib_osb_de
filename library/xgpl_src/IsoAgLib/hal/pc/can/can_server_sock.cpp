/***************************************************************************
              can_server_sock.cpp -
                    can server communicates with clients through sockets
                    message forwarding to other clients

                             -------------------
    begin                : Tue Oct 2 2001
    copyright            : (C) 1999 - 2009 Dipl.-Inform. Achim Spangler
    email                : a.spangler@osb-ag:de
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
 * Copyright (C) 1999 - 2009 Dipl.-Inform. Achim Spangler                  *
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

#include <cstring>
#include <cstdio>
#include <cctype>

#include <stdio.h>
#include <stdlib.h>

#include <list>
#include <string>
#include <sstream>

#define DEF_USE_SERVER_SPECIFIC_HEADER
#include "can_server_common.h"
#include "can_server.h"

#ifdef WIN32
  #ifndef WINCE
    #include <ws2tcpip.h>
  #endif
#else
  #include <fcntl.h>
  #include <sys/types.h>
  #include <sys/socket.h>
  #include <sys/un.h>
  #include <netinet/in.h>
  #include <arpa/inet.h>
  #include <netdb.h>
  #include <sys/ioctl.h>
  #include <sys/time.h>
  #include <sys/times.h>
  #include <linux/version.h>
#endif

#ifndef WINCE
  #include <sys/stat.h>
#endif

#include <time.h>
#include <errno.h>

static const uint32_t scui32_selectTimeoutMax = 50000;
static const uint32_t scui32_selectTimeoutMin = 1000;


__HAL::server_c::server_c() :
  mb_logMode(FALSE),
  mvec_logFile( cui32_maxCanBusCnt, 0 ),
  mb_monitorMode(FALSE),
  mb_inputFileMode(FALSE),
  mi16_reducedLoadOnIsoBus(-1),
  mb_interactive(true),
  mi_canReadNiceValue(0),
  mi_highPrioModeIfMin(0)
{
  memset(marrui16_globalMask, 0, sizeof(marrui16_globalMask));
  memset(marrb_remoteDestinationAddressInUse, 0, sizeof(marrb_remoteDestinationAddressInUse));
  memset(marrui16_busRefCnt, 0, sizeof(marrui16_busRefCnt));

  for (uint32_t i=0; i<__HAL::cui32_maxCanBusCnt; i++)
  {
    marri32_sendDelay[i] = 0;
    marri_pendingMsgs[i] = 0;
    marri32_can_device[i] = 0;
    marrb_deviceConnected[i] = false;
  }

  pthread_mutex_init(&mt_protectClientList, NULL);

}

__HAL::client_c::client_c()
  : ui16_pid(0), i32_msecStartDeltaClientMinusServer(0)
{
  memset(b_busUsed, 0, sizeof(b_busUsed));
  memset(marrui16_globalMask, 0, sizeof(marrui16_globalMask));
  memset(ui32_globalMask, 0, sizeof(ui32_globalMask));
  memset(ui32_lastMask, 0, sizeof(ui32_lastMask));
  memset(marri32_sendDelay, 0, sizeof(marri32_sendDelay));
  memset(b_initReceived, 0, sizeof(b_initReceived));
}


namespace __HAL {

#ifndef WIN32
/** linux-begin */
#ifndef LINUX_VERSION_CODE
#error "LINUX_VERSION_CODE is not defined"
#endif
#if (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,0))
/** On Linux 2.4: Define the amount of MSec per Clock_t, in case the project config didn't this before */
#ifndef msecPerClock
#define msecPerClock 10
#define clocksPer100Msec 10
#endif
#endif

clock_t getStartUpTime()
{
#if (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,0))
  if ( msecPerClock != (1000 / sysconf(_SC_CLK_TCK)) )
  { // BIG PROBLEM -> THE DEFINE DURING COMPILE TIME DOES NOT MATCH THE RUNTIME
    std::cerr << "\n\nVERY BIG PROBLEM!!!\nThis program was compiled with\n#define msecPerClock " << msecPerClock
        << "\nwhile the runtime system has " << (1000 / sysconf(_SC_CLK_TCK))
        << "\n\nSO PLEASE add\n#define msecPerClock " << (1000 / sysconf(_SC_CLK_TCK))
        << "\nto your project configuration header or Makefile, so that a matching binary is built. This program is aborted now, as none of any time calculations will match with this problem.\n\n"
        << std::endl;
    abort();
  }
  static clock_t st_startup4Times = times(NULL);
#else
  static clock_t st_startup4Times = -1;
  if (st_startup4Times < 0)
  {
    timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    st_startup4Times = int32_t(ts.tv_sec)*1000 + int32_t(ts.tv_nsec/1000000);
  }
#endif
  return st_startup4Times;
}
/** linux-end */
#else
int32_t getStartUpTime()
{
  static int32_t st_startup4Times = timeGetTime();
  return st_startup4Times;
}
#endif




#ifdef WIN32
#if defined( _MSC_VER )
  // VC++ with native Win32 API provides very accurate
  // msec timer - use that
  int32_t getTime()
  { // returns time in msec
    return timeGetTime() - getStartUpTime();
  }
#else
  // MinGW has neither simple access to timeGetTime()
  // nor to gettimeofday()
  // - but beware: at least in LINUX clock() measures
  //   only the times of the EXE in CPU core
  int32_t getTime()
  { // returns time in msec
    return (clock()/(CLOCKS_PER_SEC/1000));
  }
#endif
#else
  /** linux-begin */
  int32_t getTime()
  {
#if (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,0))
    // use gettimeofday for native LINUX 2.4 system
    // sysconf(_SC_CLK_TCK) provides clock_t ticks per second
    //  static const int64_t ci64_mesecPerClock = 1000 / sysconf(_SC_CLK_TCK);
    static struct timeval now;
    gettimeofday(&now, 0);
    // fetch RAW - non normalized - time in scaling of gettimeofday()
    int64_t i64_time4Timeofday = int64_t(now.tv_sec)*1000LL + int64_t(now.tv_usec/1000);


    // store offset between gettimeofday() and system start
    static int64_t si64_systemStart4Timeofday = i64_time4Timeofday;
    // static store delta between times() normalization and gettimeofday() norm
    static int64_t si64_deltaStartTimes = i64_time4Timeofday - int64_t(getStartUpTime()) * msecPerClock;

    // derive change of the normalization delta
    const int64_t ci64_deltaChange = i64_time4Timeofday - int64_t(times(NULL)) * msecPerClock - si64_deltaStartTimes;
    if ( ( ci64_deltaChange >= 1000 ) || ( ci64_deltaChange <= -1000 ) )
    { // user changed the system clock inbetween
      si64_deltaStartTimes += ci64_deltaChange;
      si64_systemStart4Timeofday += ci64_deltaChange;
    }

    // now calculate the real time in [msec] since startup
    i64_time4Timeofday -= si64_systemStart4Timeofday;
    // now derive the well define overflows
    while ( i64_time4Timeofday > 0x7FFFFFFFLL )
    {
      i64_time4Timeofday         -= 0xFFFFFFFF;
      si64_systemStart4Timeofday += 0xFFFFFFFF;
      si64_deltaStartTimes       += 0xFFFFFFFF;
    }

    return i64_time4Timeofday;
#else
    /** linux-2.6 */
    timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    const int32_t ci_now = int32_t(ts.tv_sec)*1000 + int32_t(ts.tv_nsec/1000000);
    return ci_now - getStartUpTime();
#endif
  }
  /** linux-end */
#endif


void initClientTime( __HAL::client_c& ref_receiveClient, clock_t rt_startupClock )
{
#ifndef WIN32
  #if (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,0))
    ref_receiveClient.i32_msecStartDeltaClientMinusServer = (rt_startupClock - getStartUpTime())*msecPerClock;
  #else
    ref_receiveClient.i32_msecStartDeltaClientMinusServer = (rt_startupClock - getStartUpTime()); // we get msec directly with Linux 2.6. upwards!
  #endif
#else
  ref_receiveClient.i32_msecStartDeltaClientMinusServer = (rt_startupClock - getStartUpTime()); // we get msec directly with Linux 2.6. upwards!
#endif
  DEBUG_PRINT1 ("Initialized ref_receiveClient.i32_msecStartDeltaClientMinusServer to %d\n", ref_receiveClient.i32_msecStartDeltaClientMinusServer);
}

int32_t getClientTime( __HAL::client_c& ref_receiveClient )
{
  return getTime() - ref_receiveClient.i32_msecStartDeltaClientMinusServer;
}

//int32_t getServerTimeFromClientTime( __HAL::client_c& ref_receiveClient, int32_t ri32_clientTime )
//{
//  return ri32_clientTime + ref_receiveClient.i32_msecStartDeltaClientMinusServer;
//}

} // end namespace


SOCKET_TYPE establish(unsigned short portnum)
{
  SOCKET_TYPE listenSocket;

#ifdef WIN32
  // Create a SOCKET for listening for
  // incoming connection requests
  listenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (listenSocket == INVALID_SOCKET) {
    printf("Error at socket(): %ld\n", WSAGetLastError());
    WSACleanup();
    return -1;
  }
  // The sockaddr_in structure specifies the address family,
  // IP address, and port for the socket that is being bound.
  sockaddr_in service;
  service.sin_family = AF_INET;
  service.sin_addr.s_addr = inet_addr(CAN_SERVER_HOST);
  service.sin_port = htons(portnum);

  // Bind the socket.
  if (bind( listenSocket, (SOCKADDR*) &service, sizeof(service)) == SOCKET_ERROR) {
    printf("bind() failed.\n");
    closesocket(listenSocket);
    return -1;
  }

  // Listen for incoming connection requests.
  // on the created socket
  if (listen( listenSocket, 1 ) == SOCKET_ERROR) {
    printf("Error listening on socket.\n");
    closesocket(listenSocket);
    WSACleanup();
    return -1;
  }

#else

  uint32_t ui32_len;

#ifdef USE_UNIX_SOCKET
  struct sockaddr_un sa;
  memset(&sa, 0, sizeof(struct sockaddr_un));   /* clear our address */
  sa.sun_family = SOCKET_TYPE_INET_OR_UNIX;
  sprintf(sa.sun_path, "%s.%d", SOCKET_PATH, portnum);
  unlink(sa.sun_path);
  ui32_len = strlen(sa.sun_path) + sizeof(sa.sun_family);
#else
  struct sockaddr_in sa;
  memset(&sa, 0, sizeof(struct sockaddr_in));   /* clear our address */
  sa.sin_family = SOCKET_TYPE_INET_OR_UNIX;
  sa.sin_addr.s_addr = inet_addr(CAN_SERVER_HOST);
  sa.sin_port = htons(portnum);                  /* this is our port number */
  ui32_len = sizeof(struct sockaddr_in);
#endif

  if ((listenSocket= socket(SOCKET_TYPE_INET_OR_UNIX, SOCK_STREAM, 0)) < 0) /* create socket */
  {
    MACRO_ISOAGLIB_PERROR("socket");
    return(-1);
  }

  if (bind(listenSocket, (struct sockaddr *)&sa, ui32_len) < 0) {
    MACRO_ISOAGLIB_PERROR("bind");
    printf("\nmaybe socket is in TIME_WAIT state, check this with the netstat command\n");
    printf("=> please wait one minute (or stop all clients before stopping the can_server)\n\n");
    close(listenSocket);
    return(-1);                                  /* bind address to socket */
  }

  listen(listenSocket, 10);                                  /* max # of queued connects */

#endif

  return listenSocket;
}


SOCKET_TYPE get_connection(SOCKET_TYPE listenSocket) {

  SOCKET_TYPE newSocket;                              /* socket of connection */

#ifdef WIN32
  if ((newSocket = accept(listenSocket,NULL,NULL)) == INVALID_SOCKET)  /* accept connection if there is one */
#else
  if ((newSocket = accept(listenSocket,NULL,NULL)) < 0)  /* accept connection if there is one */
#endif
    return(-1);

  return(newSocket);
}

int read_data(SOCKET_TYPE s,     /* connected socket */
              char *buf, /* pointer to the buffer */
              int n      /* number of characters (bytes) we want */
             )
{
  int bcount; /* counts bytes read */
  int br;     /* bytes read this pass */

  //printf("read_data size %d\n", n);
  bcount= 0;
  br= 0;
  while (bcount < n) {             /* loop until full buffer */
    if ((br= recv(s,buf,n-bcount,0)) > 0) {
      bcount += br;                /* increment byte counter */
      buf += br;                   /* move buffer ptr for next read */
    }
    else if (br <= 0)               /* signal an error or end of communication (0) to the caller */
      return br;
  }
  return(bcount);
}

void dumpCanMsg (__HAL::transferBuf_s *ps_transferBuf, FILE& f_handle)
{
#if WIN32
  clock_t t_sendTimestamp = timeGetTime();
  uint64_t ui64_timeStamp10 = (uint64_t)t_sendTimestamp;
#else
  clock_t t_sendTimestamp = times(NULL);
  uint64_t ui64_timeStamp10 = (uint64_t)t_sendTimestamp * 10;
#endif

  /* split of fprintf 64bit-value is needed for windows! */
  fprintf(&f_handle, "%Ld ", ui64_timeStamp10);
  fprintf(&f_handle, "%-2d %-2d %-2d %-2d %-2d %-8x  ",
      ps_transferBuf->s_data.ui8_bus, ps_transferBuf->s_data.ui8_obj, ps_transferBuf->s_data.s_canMsg.i32_msgType, ps_transferBuf->s_data.s_canMsg.i32_len,
      (ps_transferBuf->s_data.s_canMsg.ui32_id >> 26) & 7 /* priority */, ps_transferBuf->s_data.s_canMsg.ui32_id);
  for (uint8_t ui8_i = 0; (ui8_i < ps_transferBuf->s_data.s_canMsg.i32_len) && (ui8_i < 8); ui8_i++)
    fprintf(&f_handle, " %-3hx", ps_transferBuf->s_data.s_canMsg.ui8_data[ui8_i]);
  fprintf(&f_handle, "\n");
  fflush(&f_handle);
}

void dumpCanMsg(__HAL::transferBuf_s *ap_transferBuf, __HAL::server_c *ap_server)
{
  size_t n_bus = ap_transferBuf->s_data.ui8_bus;
  if (!ap_server->mvec_logFile[n_bus]) {
    bool b_error = newFileLog( ap_server, n_bus );
    if (b_error)
      return;
  }
  dumpCanMsg(
      ap_transferBuf,
      *(ap_server->mvec_logFile[n_bus]->getRaw()) );
}

void monitorCanMsg (__HAL::transferBuf_s *ps_transferBuf)
{
#if WIN32
  clock_t t_sendTimestamp = timeGetTime();
  uint64_t ui64_timeStamp10 = (uint64_t)t_sendTimestamp;
#else
  clock_t t_sendTimestamp = times(NULL);
  uint64_t ui64_timeStamp10 = (uint64_t)t_sendTimestamp * 10;
#endif

  /* split of fprintf 64bit-value is needed for windows! */
  printf("%Ld ", ui64_timeStamp10);
  printf("%-2d %-2d %-2d %-2d %-2d %-8x  ",
         ps_transferBuf->s_data.ui8_bus, ps_transferBuf->s_data.ui8_obj, ps_transferBuf->s_data.s_canMsg.i32_msgType, ps_transferBuf->s_data.s_canMsg.i32_len,
         (ps_transferBuf->s_data.s_canMsg.ui32_id >> 26) & 7 /* priority */, ps_transferBuf->s_data.s_canMsg.ui32_id);
  for (uint8_t ui8_i = 0; (ui8_i < ps_transferBuf->s_data.s_canMsg.i32_len) && (ui8_i < 8); ui8_i++)
     printf(" %-3hx", ps_transferBuf->s_data.s_canMsg.ui8_data[ui8_i]);
  printf("\n");
  fflush(0);
};


void releaseClient(__HAL::server_c* pc_serverData, std::list<__HAL::client_c>::iterator& iter_delete)
{
  printf("release client\n");

  for (uint8_t ui8_cnt=0; ui8_cnt<__HAL::cui32_maxCanBusCnt; ui8_cnt++)
  {
    if (iter_delete->b_initReceived[ui8_cnt] && (pc_serverData->marrui16_busRefCnt[ui8_cnt] > 0))
    {
      pc_serverData->marrui16_busRefCnt[ui8_cnt]--; // decrement bus ref count when client dropped off
      if (!pc_serverData->marrui16_busRefCnt[ui8_cnt])
        closeBusOnCard(ui8_cnt, pc_serverData);
    }
  }

#ifdef SYSTEM_WITH_ENHANCED_CAN_HAL
  for (uint8_t k=0; k<__HAL::cui32_maxCanBusCnt; k++)
    iter_delete->arrMsgObj[k].clear();
#endif

  // erase sets iterator to next client
  iter_delete = pc_serverData->mlist_clients.erase(iter_delete);

}

static void enqueue_msg(__HAL::transferBuf_s* p_sockBuf, SOCKET_TYPE i32_socketSender, __HAL::server_c* pc_serverData)
{
  std::list<__HAL::client_c>::iterator iter, iter_delete = pc_serverData->mlist_clients.end();

  const uint8_t ui8_bus = p_sockBuf->s_data.ui8_bus;

  // mutex to prevent client list modification already got in calling function

  for (iter = pc_serverData->mlist_clients.begin(); iter != pc_serverData->mlist_clients.end(); iter++) {

    if (!iter->b_busUsed[ui8_bus])
      continue;

    // i32_clientID != 0 in forwarding mode during send, do not enqueue this message for sending client
    if (i32_socketSender && (iter->i32_dataSocket == i32_socketSender))
      continue;

    // now search for MsgObj queue on this ui8_bus, where new message from ui8_bus maps
    // start with 1 since MsgObj with id 0 is anyway planned for sending
    const int32_t i32_maxObj = iter->arrMsgObj[ui8_bus].size();
    for (int32_t i32_obj = 1; i32_obj < i32_maxObj; i32_obj++) {

      if ( (iter->arrMsgObj[ui8_bus][i32_obj].ui8_bMsgType != RX )
        || (iter->arrMsgObj[ui8_bus][i32_obj].ui16_size == 0     ) )
      { // this MsgObj is no candidate for message receive
        continue;
      }

      if (!iter->arrMsgObj[ui8_bus][i32_obj].b_canObjConfigured) {
        continue;
      }

      if ( iter->arrMsgObj[ui8_bus][i32_obj].b_canBufferLock ) {
        // don't even check this MsgObj as it shall not receive messages
        DEBUG_PRINT2("lock bus %d, obj %d\n", ui8_bus, i32_obj);
        continue;
      }

      // compare received msg with filter
      if (
          ( (iter->arrMsgObj[ui8_bus][i32_obj].ui8_bufXtd > 0)
                && (p_sockBuf->s_data.s_canMsg.i32_msgType > 0)
                && ( (p_sockBuf->s_data.s_canMsg.ui32_id & iter->arrMsgObj[ui8_bus][i32_obj].ui32_mask_xtd) == (iter->arrMsgObj[ui8_bus][i32_obj].ui32_filter & iter->arrMsgObj[ui8_bus][i32_obj].ui32_mask_xtd) )
              )
              ||
              ( (iter->arrMsgObj[ui8_bus][i32_obj].ui8_bufXtd == 0)
                && (p_sockBuf->s_data.s_canMsg.i32_msgType == 0)
                && ( (p_sockBuf->s_data.s_canMsg.ui32_id & iter->arrMsgObj[ui8_bus][i32_obj].ui16_mask_std) == (iter->arrMsgObj[ui8_bus][i32_obj].ui32_filter & iter->arrMsgObj[ui8_bus][i32_obj].ui16_mask_std) )
              )
        )
        { // received msg fits actual filter
          //DEBUG_PRINT("queueing message\n");
          // update send time stamp in paket
          p_sockBuf->s_data.i32_sendTimeStamp = getClientTime(*iter);

          p_sockBuf->s_data.ui8_obj = i32_obj;

          if (send(iter->i32_dataSocket, (char*)p_sockBuf, sizeof(__HAL::transferBuf_s),
#ifdef WIN32
              0
#else
              MSG_DONTWAIT
#endif
             ) < 0)
          {
            MACRO_ISOAGLIB_PERROR("send");
          }

          // don't check following objects if message is already enqueued for this client
          break;

        } // if fit
    } // for objNr
  }// for iter

}


std::list<int32_t> __HAL::list_sendTimeStamps;


void send_command_ack(SOCKET_TYPE ri32_commandSocket, int32_t ri32_dataContent, int32_t ri32_data)
{
  __HAL::transferBuf_s s_transferBuf;

  s_transferBuf.ui16_command = COMMAND_ACKNOWLEDGE;
  s_transferBuf.s_acknowledge.i32_dataContent = ri32_dataContent;
  s_transferBuf.s_acknowledge.i32_data = ri32_data;

  if (send(ri32_commandSocket, (char*)&s_transferBuf, sizeof(__HAL::transferBuf_s),
#ifdef WIN32
           0
#else
           MSG_DONTWAIT
#endif
          ) < 0)
  {
    MACRO_ISOAGLIB_PERROR("send");
  }
}


/////////////////////////////////////////////////////////////////////////
void handleCommand(__HAL::server_c* pc_serverData, std::list<__HAL::client_c>::iterator& iter_client, __HAL::transferBuf_s* p_writeBuf)
{
  int32_t i32_error;
  int32_t i32_dataContent;
  int32_t i32_data=0;

  DEBUG_PRINT1("cmd %d\n", p_writeBuf->ui16_command);

  i32_error = 0;

  // default to simple ACK which returns the error.
  // Overwrite if special answers needed like PIPE_ID or SEND_DELAY
  i32_dataContent = ACKNOWLEDGE_DATA_CONTENT_ERROR_VALUE;
  // in DATA_CONTENT_ERROR_VALUE case i32_error will be used as i32_data!!!

    switch (p_writeBuf->ui16_command)
    {

      case COMMAND_REGISTER:
      {
        DEBUG_PRINT("COMMAND_REGISTER\n");

        // no need to set to 0, as everything got set to zero before!
        // s_tmpClient.marri32_sendDelay[all-buses] = 0;

        DEBUG_PRINT1 ("Client registering with startTimeClock_t from his REGISTER message as %d\n", p_writeBuf->s_startTimeClock.t_clock);
        initClientTime(*iter_client, p_writeBuf->s_startTimeClock.t_clock );

        DEBUG_PRINT1("client start up time (absolute value in clocks): %d\n", p_writeBuf->s_startTimeClock.t_clock);

        if (!i32_error)
        { // no error
          // transmit current pipeId to client (for composition of pipe name)
          i32_dataContent = ACKNOWLEDGE_DATA_CONTENT_PIPE_ID;
        }
      }
      break;

      case COMMAND_DEREGISTER: {
        DEBUG_PRINT("COMMAND_DEREGISTER\n");

        for (uint8_t j=0; j<__HAL::cui32_maxCanBusCnt; j++)
        {
          iter_client->arrMsgObj[j].clear();
          if (iter_client->b_initReceived[j] && (pc_serverData->marrui16_busRefCnt[j] > 0))
            pc_serverData->marrui16_busRefCnt[j]--; // decrement ref count only when we received the INIT command before
        }

        releaseClient(pc_serverData, iter_client);
        // i32_error will stay at 0 for "no error"
      } break;


      case COMMAND_INIT:

        if (p_writeBuf->s_config.ui8_bus > HAL_CAN_MAX_BUS_NR)
          i32_error = HAL_RANGE_ERR;
        else if (!pc_serverData->marrui16_busRefCnt[p_writeBuf->s_init.ui8_bus])
        { // first init command for current bus
          // open log file only once per bus
          if (pc_serverData->mb_logMode) {
            newFileLog( pc_serverData, p_writeBuf->s_init.ui8_bus);
          }

          // just to get sure that we reset the number of pending write-messages
          pc_serverData->marri_pendingMsgs[p_writeBuf->s_init.ui8_bus] = 0;

          if (!openBusOnCard(p_writeBuf->s_init.ui8_bus,  // 0 for CANLPT/ICAN, else 1 for first BUS
                             p_writeBuf->s_init.ui16_wBitrate,  // BTR0BTR1
                             pc_serverData))
          {
            printf("can't initialize CAN\n");
            printf("CAN device not ready or wrong PRJ_CAN_DRIVER_DEVICE selected?\n");
            i32_error = HAL_CONFIG_ERR;
            MACRO_ISOAGLIB_ABORT();
          }
        }

        if (!i32_error) {
          pc_serverData->marrui16_busRefCnt[p_writeBuf->s_init.ui8_bus]++;
          iter_client->b_initReceived[p_writeBuf->s_init.ui8_bus] = true; // when the CLOSE command is received => allow decrement of ref count
          iter_client->b_busUsed[p_writeBuf->s_init.ui8_bus] = true; // when the CLOSE command is received => allow decrement of ref count
        }
        // do rest of init handling in next case statement (no break!)

      case COMMAND_CHG_GLOBAL_MASK:
        if (!i32_error) {
          iter_client->marrui16_globalMask[p_writeBuf->s_init.ui8_bus] = p_writeBuf->s_init.ui16_wGlobMask;
          iter_client->ui32_globalMask[p_writeBuf->s_init.ui8_bus] = p_writeBuf->s_init.ui32_dwGlobMask;
          iter_client->ui32_lastMask[p_writeBuf->s_init.ui8_bus] = p_writeBuf->s_init.ui32_dwGlobMaskLastmsg;
        }
        break;


      case COMMAND_CLOSE:
      {
        if (p_writeBuf->s_config.ui8_bus > HAL_CAN_MAX_BUS_NR )
          i32_error = HAL_RANGE_ERR;
        else
        {
          if (iter_client->b_initReceived[p_writeBuf->s_init.ui8_bus] && (pc_serverData->marrui16_busRefCnt[p_writeBuf->s_init.ui8_bus] > 0))
          {
            pc_serverData->marrui16_busRefCnt[p_writeBuf->s_init.ui8_bus]--; // decrement ref count only when we received the INIT command before
          }

          iter_client->b_initReceived[p_writeBuf->s_init.ui8_bus] = false; // reset flag

          if (pc_serverData->marrui16_busRefCnt[p_writeBuf->s_init.ui8_bus] == 0)
          { // last connection on bus closed, so reset pending msgs...
            pc_serverData->marri_pendingMsgs[p_writeBuf->s_init.ui8_bus] = 0;
            // close can device
            closeBusOnCard(p_writeBuf->s_init.ui8_bus, pc_serverData);
          }

          if (!pc_serverData->marrui16_busRefCnt[p_writeBuf->s_init.ui8_bus] && pc_serverData->mb_logMode) {
            pc_serverData->mvec_logFile[p_writeBuf->s_init.ui8_bus] =
              yasper::ptr< __HAL::LogFile_c >( 0 );
          }

          iter_client->arrMsgObj[p_writeBuf->s_config.ui8_bus].clear();
          // i32_error will stay at 0 for "no error"
        }
      }
      break;


      case COMMAND_CONFIG:
        if ( p_writeBuf->s_config.ui8_bus > HAL_CAN_MAX_BUS_NR )
        {
          i32_error = HAL_RANGE_ERR;
          break;
        }

        if (p_writeBuf->s_config.ui8_obj >= iter_client->arrMsgObj[p_writeBuf->s_config.ui8_bus].size()) {
          // add new elements in the vector with resize
          iter_client->arrMsgObj[p_writeBuf->s_config.ui8_bus].resize(p_writeBuf->s_config.ui8_obj+1);
        } else {
          // reconfigure element
        }
        // break left out intentionally

      case COMMAND_CHG_CONFIG:

        if ((p_writeBuf->s_config.ui8_bus > HAL_CAN_MAX_BUS_NR ) || ( p_writeBuf->s_config.ui8_obj > iter_client->arrMsgObj[p_writeBuf->s_config.ui8_bus].size()-1 ))
          i32_error = HAL_RANGE_ERR;
        else
        {
          iter_client->arrMsgObj[p_writeBuf->s_config.ui8_bus][p_writeBuf->s_config.ui8_obj].b_canObjConfigured = TRUE;

          iter_client->arrMsgObj[p_writeBuf->s_config.ui8_bus][p_writeBuf->s_config.ui8_obj].ui8_bufXtd = p_writeBuf->s_config.ui8_bXtd;
          iter_client->arrMsgObj[p_writeBuf->s_config.ui8_bus][p_writeBuf->s_config.ui8_obj].ui32_filter = p_writeBuf->s_config.ui32_dwId;

          if (p_writeBuf->s_config.ui8_bXtd)
              iter_client->arrMsgObj[p_writeBuf->s_config.ui8_bus][p_writeBuf->s_config.ui8_obj].ui32_mask_xtd = p_writeBuf->s_config.ui32_mask;
          else
              iter_client->arrMsgObj[p_writeBuf->s_config.ui8_bus][p_writeBuf->s_config.ui8_obj].ui16_mask_std = p_writeBuf->s_config.ui32_mask;

          if (p_writeBuf->ui16_command == COMMAND_CONFIG) {
//              clearReadQueue (p_writeBuf->s_config.ui8_bus, p_writeBuf->s_config.ui8_obj, pc_serverData->ms_msqDataServer.i32_rdHandle, iter_client->ui16_pid);
//            clearWriteQueue(p_writeBuf->s_config.ui8_bus, p_writeBuf->s_config.ui8_obj, pc_serverData->ms_msqDataServer.i32_wrHandle, iter_client->ui16_pid);

            iter_client->arrMsgObj[p_writeBuf->s_config.ui8_bus][p_writeBuf->s_config.ui8_obj].ui8_bMsgType = p_writeBuf->s_config.ui8_bMsgType;
            iter_client->arrMsgObj[p_writeBuf->s_config.ui8_bus][p_writeBuf->s_config.ui8_obj].ui16_size = p_writeBuf->s_config.ui16_wNumberMsgs;
            iter_client->arrMsgObj[p_writeBuf->s_config.ui8_bus][p_writeBuf->s_config.ui8_obj].b_canBufferLock = false;
          }
        }
        break;


      case COMMAND_LOCK:
      case COMMAND_UNLOCK:

        if ((p_writeBuf->s_config.ui8_bus > HAL_CAN_MAX_BUS_NR ) || ( p_writeBuf->s_config.ui8_obj > iter_client->arrMsgObj[p_writeBuf->s_config.ui8_bus].size()-1 ))
          i32_error = HAL_RANGE_ERR;
        else {
          if (p_writeBuf->ui16_command == COMMAND_LOCK) {
            iter_client->arrMsgObj[p_writeBuf->s_config.ui8_bus][p_writeBuf->s_config.ui8_obj].b_canBufferLock = TRUE;
            DEBUG_PRINT2("locked buf %d, obj %d\n", p_writeBuf->s_config.ui8_bus, p_writeBuf->s_config.ui8_obj);
          } else {
            iter_client->arrMsgObj[p_writeBuf->s_config.ui8_bus][p_writeBuf->s_config.ui8_obj].b_canBufferLock = FALSE;
            DEBUG_PRINT2("unlocked buf %d, obj %d\n", p_writeBuf->s_config.ui8_bus, p_writeBuf->s_config.ui8_obj);
          }
        }
        break;


      case COMMAND_QUERYLOCK:

        if ((p_writeBuf->s_config.ui8_bus > HAL_CAN_MAX_BUS_NR ) || ( p_writeBuf->s_config.ui8_obj > iter_client->arrMsgObj[p_writeBuf->s_config.ui8_bus].size()-1 ))
          i32_error = HAL_RANGE_ERR;
        else {
          i32_dataContent = ACKNOWLEDGE_DATA_CONTENT_QUERY_LOCK;
          i32_data = iter_client->arrMsgObj[p_writeBuf->s_config.ui8_bus][p_writeBuf->s_config.ui8_obj].b_canBufferLock;
        }
        break;


      case COMMAND_CLOSEOBJ:

        if ((p_writeBuf->s_config.ui8_bus > HAL_CAN_MAX_BUS_NR ) || ( p_writeBuf->s_config.ui8_obj > iter_client->arrMsgObj[p_writeBuf->s_config.ui8_bus].size()-1 ))
          i32_error = HAL_RANGE_ERR;
        else {
          iter_client->arrMsgObj[p_writeBuf->s_config.ui8_bus][p_writeBuf->s_config.ui8_obj].b_canObjConfigured = FALSE;

          iter_client->arrMsgObj[p_writeBuf->s_config.ui8_bus][p_writeBuf->s_config.ui8_obj].b_canBufferLock = FALSE;
          iter_client->arrMsgObj[p_writeBuf->s_config.ui8_bus][p_writeBuf->s_config.ui8_obj].ui16_size = 0;
 //           clearReadQueue (p_writeBuf->s_config.ui8_bus, p_writeBuf->s_config.ui8_obj, pc_serverData->ms_msqDataServer.i32_rdHandle, iter_client->ui16_pid);
//          clearWriteQueue(p_writeBuf->s_config.ui8_bus, p_writeBuf->s_config.ui8_obj, pc_serverData->ms_msqDataServer.i32_wrHandle, iter_client->ui16_pid);

          // erase element if it is the last in the vector, otherwise it can stay there
          while (iter_client->arrMsgObj[p_writeBuf->s_config.ui8_bus].back().b_canObjConfigured == FALSE)
              iter_client->arrMsgObj[p_writeBuf->s_config.ui8_bus].pop_back();
        }
        break;


      case COMMAND_SEND_DELAY:
        if ( p_writeBuf->s_config.ui8_bus > HAL_CAN_MAX_BUS_NR )
        {
          i32_error = HAL_RANGE_ERR;
        } else {
          i32_dataContent = ACKNOWLEDGE_DATA_CONTENT_SEND_DELAY;
          i32_data = iter_client->marri32_sendDelay [p_writeBuf->s_config.ui8_bus];
          if (i32_data < 0) i32_data = -i32_data; // so we always return a positive send delay!
          else iter_client->marri32_sendDelay [p_writeBuf->s_config.ui8_bus] = -i32_data; // stamp a positive stored delay as READ by negating it!
        }
        break;

      default:
        i32_error = HAL_UNKNOWN_ERR;
        break;
    } // end switch

    // do centralized error-answering here
    if (i32_dataContent == ACKNOWLEDGE_DATA_CONTENT_ERROR_VALUE) i32_data = i32_error;

    send_command_ack(iter_client->i32_commandSocket, i32_dataContent, i32_data);
}


void readWrite(__HAL::server_c* pc_serverData)
{
  fd_set rfds;
  __HAL::transferBuf_s s_transferBuf;
  uint16_t ui16_bytesRead;
  int i_selectResult;
  struct timeval t_timeout;
  bool b_deviceHandleFound;
  bool b_deviceConnected;

  for (;;) {

    pthread_mutex_lock( &(pc_serverData->mt_protectClientList) );

    if (!pc_serverData->mlist_clients.size())
    {
      pthread_mutex_unlock( &(pc_serverData->mt_protectClientList) );

#ifdef WIN32
      Sleep( 50 );
#else
      usleep( 50000 );
#endif
      continue;
    }

    FD_ZERO(&rfds);

    std::list<__HAL::client_c>::iterator iter_client;

    for (iter_client = pc_serverData->mlist_clients.begin(); iter_client != pc_serverData->mlist_clients.end(); iter_client++)
    {
      FD_SET(iter_client->i32_commandSocket, &rfds);
      FD_SET(iter_client->i32_dataSocket, &rfds);
    }

    b_deviceHandleFound=false;
    b_deviceConnected = false;

    for (uint32_t ui32=0; ui32<__HAL::cui32_maxCanBusCnt; ui32++ )
    {
      if (pc_serverData->marri32_can_device[ui32] > 0)
      {
        FD_SET(pc_serverData->marri32_can_device[ui32], &rfds);
        b_deviceHandleFound=true;
      }

      if (pc_serverData->marrb_deviceConnected[ui32])
        b_deviceConnected = true;
    }

    pthread_mutex_unlock( &(pc_serverData->mt_protectClientList) );

    t_timeout.tv_sec = 0;
    // do long timeout if we have a device handle or if we do not have hardware device at all
    if (b_deviceHandleFound || !b_deviceConnected)
      t_timeout.tv_usec = scui32_selectTimeoutMax;
    else
      t_timeout.tv_usec = scui32_selectTimeoutMin;

    // timeout to check for
    // 1. modified client list => new sockets to wait for
    // new incoming can messages when can device has no file handle (WIN32 PEAK can card and RTE)
    i_selectResult = select(FD_SETSIZE, &rfds, NULL, NULL, &t_timeout);

    if (i_selectResult < 0)
      // error
      continue;

    // new message from can device ?
    for (uint32_t ui32_cnt = 0; ui32_cnt < __HAL::cui32_maxCanBusCnt; ui32_cnt++ )
    {
      if (!pc_serverData->marrui16_busRefCnt[ui32_cnt])
        continue; // unused bus

      if ( !b_deviceHandleFound ||
           (b_deviceHandleFound && pc_serverData->marri32_can_device[ui32_cnt] && FD_ISSET(pc_serverData->marri32_can_device[ui32_cnt], &rfds)) 
         )
      { // do card read
        if (readFromBus(ui32_cnt, &(s_transferBuf.s_data.s_canMsg), pc_serverData))
        {
          pthread_mutex_lock( &(pc_serverData->mt_protectClientList) );
          s_transferBuf.s_data.ui8_bus = ui32_cnt;
          enqueue_msg(&s_transferBuf, 0, pc_serverData);
          pthread_mutex_unlock( &(pc_serverData->mt_protectClientList) );
          if (pc_serverData->mb_logMode) {
            dumpCanMsg(
                &s_transferBuf,
                pc_serverData);
          }
          if (pc_serverData->mb_monitorMode)
            monitorCanMsg (&s_transferBuf);
        }
      }
    }

    pthread_mutex_lock( &(pc_serverData->mt_protectClientList) );

    // new message from socket ?
    for (iter_client = pc_serverData->mlist_clients.begin(); iter_client != pc_serverData->mlist_clients.end(); iter_client++)
    {
      if (FD_ISSET(iter_client->i32_commandSocket, &rfds))
      {
        // socket still alive? (returns 0 (peer shutdown) or -1 (error))
#ifdef WINCE
        // @todo WINCE Windows CE has a bug with MSG_PEEK (bytes are actually read)
        //int bytesRecv = select( 0, &rfds, NULL, NULL, 0 ); // this was a try for a workaround
        #error "This place has to be done correctly for Windows CE"
#else
        int bytesRecv = recv(iter_client->i32_commandSocket, (char*)&s_transferBuf, sizeof(__HAL::transferBuf_s),
        #ifndef WIN32
          MSG_DONTWAIT|
        #endif
          MSG_PEEK);
#endif
        if (bytesRecv == 0 || bytesRecv == -1)
        {
          #ifdef WIN32
          if( bytesRecv == -1 && WSAGetLastError() == WSAEOPNOTSUPP )
            printf("The attempted operation is not supported for the type of object referenced.\n");
          #endif
          printf( "connection closed.\n");
          releaseClient(pc_serverData, iter_client);
          break;
        }

        ui16_bytesRead = read_data(iter_client->i32_commandSocket, (char*)&s_transferBuf, sizeof(s_transferBuf));

        if (s_transferBuf.ui16_command != COMMAND_DATA)
        {
          handleCommand(pc_serverData, iter_client, &s_transferBuf);
        }
      }
      if (FD_ISSET(iter_client->i32_dataSocket, &rfds))
      {
        // socket still alive? (returns 0 (peer shutdown) or -1 (error))
#ifdef WINCE
        // @todo WINCE Windows CE has a bug with MSG_PEEK (bytes are actually read)
        //int bytesRecv = select( 0, &rfds, NULL, NULL, 0 ); // this was a try for a workaround
        #error "This place has to be done correctly for Windows CE"
#else
        int bytesRecv = recv(iter_client->i32_dataSocket, (char*)&s_transferBuf, sizeof(__HAL::transferBuf_s),
        #ifndef WIN32
          MSG_DONTWAIT|
        #endif
          MSG_PEEK);
#endif

        if (bytesRecv == 0 || bytesRecv == -1)
        {
          #ifdef WIN32
          if( bytesRecv == -1 && WSAGetLastError() == WSAEOPNOTSUPP )
            printf("The attempted operation is not supported for the type of object referenced.\n");
          #endif
          printf( "connection closed.\n");
          releaseClient(pc_serverData, iter_client);
          break;
        }

        ui16_bytesRead = read_data(iter_client->i32_dataSocket, (char*)&s_transferBuf, sizeof(s_transferBuf));

        if (s_transferBuf.ui16_command == COMMAND_DATA)
        {
          // process data message
          enqueue_msg(&s_transferBuf, iter_client->i32_dataSocket, pc_serverData); // not done any more: disassemble_client_id(msqWriteBuf.i32_mtype)

          sendToBus(s_transferBuf.s_data.ui8_bus, &(s_transferBuf.s_data.s_canMsg), pc_serverData);

          if (pc_serverData->mb_logMode) {
            dumpCanMsg(
                &s_transferBuf,
                pc_serverData);
          }
          if (pc_serverData->mb_monitorMode)
          {
            monitorCanMsg (&s_transferBuf);
          }
        }
      }
    }

    pthread_mutex_unlock( &(pc_serverData->mt_protectClientList) );

  }
}



/////////////////////////////////////////////////////////////////////////


static void* collectClient(void* ptr) {

  SOCKET_TYPE new_socket, base_commandSocket, base_dataSocket;
  __HAL::client_c s_tmpClient; // constructor initialize values to zero

  __HAL::server_c* pc_serverData = static_cast<__HAL::server_c*>(ptr);

  if ((base_commandSocket = establish(COMMAND_TRANSFER_PORT)) < 0) {
    MACRO_ISOAGLIB_PERROR("establish");
    exit(1);
  }
  printf("command socket for port %d established\n", COMMAND_TRANSFER_PORT);

  if ((base_dataSocket = establish(DATA_TRANSFER_PORT)) < 0) {
    MACRO_ISOAGLIB_PERROR("establish");
    exit(1);
  }
  printf("data socket for port %d established\n", DATA_TRANSFER_PORT);

  while (1) {

#ifdef WIN32
    if ((new_socket=get_connection(base_commandSocket)) == INVALID_SOCKET) {
      MACRO_ISOAGLIB_PERROR("socket connect failed");
      exit(1);
    }
#else
    if ((new_socket=get_connection(base_commandSocket)) == -1) {
      perror("socket connect failed");
      exit(1);
    }
#endif

    s_tmpClient.i32_commandSocket = new_socket;

#ifdef WIN32
    if ((new_socket=get_connection(base_dataSocket)) == INVALID_SOCKET) {
      MACRO_ISOAGLIB_PERROR("socket connect failed");
      exit(1);
    }
#else
    if ((new_socket=get_connection(base_dataSocket)) == -1) {
      perror("socket connect failed");
      exit(1);
    }
#endif

    s_tmpClient.i32_dataSocket = new_socket;

    pthread_mutex_lock( &(pc_serverData->mt_protectClientList) );

    pc_serverData->mlist_clients.push_back(s_tmpClient);

    pthread_mutex_unlock( &(pc_serverData->mt_protectClientList) );

    printf("command and data socket connected\n");
  }
}

Option_s const ga_options[] = {
  makeOption< OPTION_MONITOR >(),
  makeOption< OPTION_LOG >(),
  makeOption< OPTION_HIGH_PRIO_MINIMUM >(),
  makeOption< OPTION_REDUCED_LOAD_ISO_BUS_NO >(),
  makeOption< OPTION_INTERACTIVE >(),
  makeOption< OPTION_PRODUCTIVE >(),
  makeOption< OPTION_HELP >()
};

Option_s const *const gp_optionsBegin = ga_options;
Option_s const *const gp_optionsEnd = ga_options +
  (sizeof ga_options) / (sizeof ga_options[0]);

int main(int argc, char *argv[])
{
  pthread_t threadCollectClient;
  int i_collectClientThreadHandle;
  __HAL::server_c c_serverData;

  checkOptions( argc, argv, c_serverData );

  // some time init
  __HAL::getTime();

  const uint32_t apiversion = initCardApi();
  if ( apiversion == 0 ) { // failure - nothing found
    DEBUG_PRINT("FAILURE - No CAN card was found with automatic search\n");
    exit(1);
  }

  if (!resetCard()) {
    DEBUG_PRINT("Reset card not ok\n");
    exit(1);
  }


#ifdef WIN32
  WSADATA wsaData;
  // Initialize Winsock
  int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
  if (iResult != 0) {
     printf("WSAStartup failed: %d\n", iResult);
     exit(1);
  }
#endif

  printf("creating collect client thread\n");

  i_collectClientThreadHandle = pthread_create( &threadCollectClient, NULL, &collectClient, &c_serverData);

  if (c_serverData.mb_interactive) {
    pthread_t thread_readUserInput;
    int i_status = pthread_create( &thread_readUserInput, NULL, &readUserInput, &c_serverData );
    if (i_status)
      exit( i_status ); // thread could not be created
    printf("operating\n");
  }

  readWrite(&c_serverData);
}

#if defined(WINCE) || (defined(WIN32) && defined(UNICODE))
// CE is unicode, so we need to convert the commandline...
// However, it will be only called when using "mainWCRTStartup" as entry point
// (using "mainCRTStartup" atm).
int _tmain( int argc, _TCHAR* argv[] )
{
	char** argv_char = new char*[argc];

	// convert command line	
	for( int i = 0; i < argc; i++ )
	{
		size_t arglen = wcslen(argv[i]);	
		char* arg = new char[arglen+1];
		WideCharToMultiByte( CP_ACP, 0, argv[i], arglen, arg, arglen, NULL, NULL );
		argv_char[i] = arg;
	}

	// call main
	int ret = main( argc, argv_char );

	// cleanup
	for( int i = 0; i < argc; i++ )
	{
		delete[] argv_char[i];
	}
	delete[] argv_char;

	return ret;
}
#endif

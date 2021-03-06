/*
  typedef.h: system dependent basic typedef's

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
#ifndef TYPEDEF_P1MC_H
#define TYPEDEF_P1MC_H

typedef unsigned short byte;
typedef unsigned int word;

typedef byte uint8_t;
typedef char int8_t;
typedef word uint16_t;
typedef int int16_t;
typedef unsigned long uint32_t;
typedef long int32_t;


//typedef long off_t;
#define off_t long

typedef unsigned long long int uint64_t;
typedef long long int int64_t;

#ifdef USE_TIMEBASE_INT64
#error "64 Bit timebase not supported."
#else
    typedef long ecutime_t;
#endif

#endif

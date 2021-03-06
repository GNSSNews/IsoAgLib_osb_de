/*
  filestreaminput_c.cpp

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

#include "filestreaminput_c.h"


bool
FileStreamInput_c::open ( STD_TSTRING& filename, FileMode_t at_mode )
{
  bool b_result = c_targetHandle.open( filename, at_mode );
  if (b_result) str_openedFile = filename;
  return b_result;
};


bool
FileStreamInput_c::open( const TCHAR* filename, FileMode_t at_mode )
{
  bool b_result = c_targetHandle.open( filename, at_mode);
  if (b_result) str_openedFile = filename;
  return b_result;
};


bool
FileStreamInput_c::close(bool b_deleteFile)
{
  c_targetHandle.close();
  if (b_deleteFile) {
    #if defined(WINCE) //use this only for CE!
      return DeleteFile( str_openedFile.c_str() );
    #else
      return (remove( str_openedFile.c_str() ) == 0);
    #endif
  }
  return true; // success
};


StreamInput_c&
FileStreamInput_c::operator>>(uint8_t& ui8_data)
{
  c_targetHandle.operator>>( ui8_data );
  return *this;
}

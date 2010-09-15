#include "DeviceDescription.xml-bytestream.inc"

IsoAgLib::iDevKey_c myDeviceDevKey(0x5, 0x0);

IsoAgLib::iIdentItem_c c_myIdent(&myDeviceDevKey, true, 0x2, 0x19, 0x7ff, 0x1b, 0x80, 0xffff, 0x0, 0x0
 #ifdef USE_ISO_TERMINAL 
 , 0, NULL
 #endif
);

#if defined(USE_ISO_11783)
static const uint16_t scui16_workStateElementNumber = 0;
const IsoAgLib::ElementDDI_s s_workStateElementDDI[] =
{
	{0x008D, true, IsoAgLib::GeneralCommand_c::exactValue},
	// termination entry
	{0xFFFF, false, IsoAgLib::GeneralCommand_c::noValue}
};
#endif

IsoAgLib::iProcDataLocal_c c_workState(
#ifdef USE_ISO_11783
s_workStateElementDDI,
scui16_workStateElementNumber, 
#endif
#ifdef USE_DIN_9684
0x0, 0x1, 0x0, 0xff, 
#endif
myDeviceDevKey, 0x2, myDeviceDevKey, &myDeviceDevKey, false
#ifdef USE_EEPROM_IO
, 0xffff
#endif
);

#if defined(USE_ISO_11783)
static const uint16_t scui16_applicationRateElementNumber = 0;
const IsoAgLib::ElementDDI_s s_applicationRateElementDDI[] =
{
	{0x0001, true, IsoAgLib::GeneralCommand_c::exactValue},
	{0x0002, false, IsoAgLib::GeneralCommand_c::exactValue},
	{0x0003, true, IsoAgLib::GeneralCommand_c::defaultValue},
	{0x0004, true, IsoAgLib::GeneralCommand_c::minValue},
	// termination entry
	{0xFFFF, false, IsoAgLib::GeneralCommand_c::noValue}
};
#endif

IsoAgLib::iProcDataLocal_c c_applicationRate(
#ifdef USE_ISO_11783
s_applicationRateElementDDI,
scui16_applicationRateElementNumber, 
#endif
#ifdef USE_DIN_9684
0x0, 0x5, 0x0, 0xff, 
#endif
myDeviceDevKey, 0x2, myDeviceDevKey, &myDeviceDevKey, false
#ifdef USE_EEPROM_IO
, 0xffff
#endif
);

#if defined(USE_ISO_11783)
static const uint16_t scui16_defaultLoggingElementNumber = 0;
const IsoAgLib::ElementDDI_s s_defaultLoggingElementDDI[] =
{
	{0xDFFF, false, IsoAgLib::GeneralCommand_c::exactValue},
	// termination entry
	{0xFFFF, false, IsoAgLib::GeneralCommand_c::noValue}
};
#endif

IsoAgLib::iProcDataLocal_c c_defaultLogging(
#ifdef USE_ISO_11783
s_defaultLoggingElementDDI,
scui16_defaultLoggingElementNumber, 
#endif
#ifdef USE_DIN_9684
0x0, 0x1, 0x0, 0xff, 
#endif
myDeviceDevKey, 0x2, myDeviceDevKey, &myDeviceDevKey, false
#ifdef USE_EEPROM_IO
, 0xffff
#endif
);


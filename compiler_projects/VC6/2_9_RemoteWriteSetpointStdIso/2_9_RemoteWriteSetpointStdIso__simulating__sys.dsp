# Microsoft Developer Studio Project File - Name="2_9_RemoteWriteSetpointStdIso" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** NICHT BEARBEITEN **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=2_9_RemoteWriteSetpointStdIso - Win32 Debug
!MESSAGE Dies ist kein g�ltiges Makefile. Zum Erstellen dieses Projekts mit NMAKE
!MESSAGE verwenden Sie den Befehl "Makefile exportieren" und f�hren Sie den Befehl
!MESSAGE
!MESSAGE NMAKE /f "2_9_RemoteWriteSetpointStdIso.mak".
!MESSAGE
!MESSAGE Sie k�nnen beim Ausf�hren von NMAKE eine Konfiguration angeben
!MESSAGE durch Definieren des Makros CFG in der Befehlszeile. Zum Beispiel:
!MESSAGE
!MESSAGE NMAKE /f "2_9_RemoteWriteSetpointStdIso.mak" CFG="2_9_RemoteWriteSetpointStdIso - Win32 Debug"
!MESSAGE
!MESSAGE F�r die Konfiguration stehen zur Auswahl:
!MESSAGE
!MESSAGE "2_9_RemoteWriteSetpointStdIso - Win32 Release" (basierend auf  "Win32 (x86) Console Application")
!MESSAGE "2_9_RemoteWriteSetpointStdIso - Win32 Debug" (basierend auf  "Win32 (x86) Console Application")
!MESSAGE

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "2_9_RemoteWriteSetpointStdIso - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2  /I "..\..\..\IsoAgLib_Examples\tutorial\2_ProcessData\2_9_RemoteWriteSetpointStdIso" /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS"  /D "SYSTEM_PC_VC" /D "PRJ_USE_AUTOGEN_CONFIG=config_2_9_RemoteWriteSetpointStdIso.h" /YX /FD /TP /c
# ADD BASE RSC /l 0x407 /d "NDEBUG" /d "SYSTEM_PC_VC" /d "PRJ_USE_AUTOGEN_CONFIG=config_2_9_RemoteWriteSetpointStdIso.h"
# ADD RSC /l 0x407 /d "NDEBUG" /d "SYSTEM_PC_VC" /d "PRJ_USE_AUTOGEN_CONFIG=config_2_9_RemoteWriteSetpointStdIso.h"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib  kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib  kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib winmm.lib /nologo /subsystem:console /machine:I386 /libpath:""

!ELSEIF  "$(CFG)" == "2_9_RemoteWriteSetpointStdIso - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /GZ  /c
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od  /I "..\..\..\IsoAgLib_Examples\tutorial\2_ProcessData\2_9_RemoteWriteSetpointStdIso" /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS"  /D "SYSTEM_PC_VC" /D "PRJ_USE_AUTOGEN_CONFIG=config_2_9_RemoteWriteSetpointStdIso.h" /YX /FD /TP /GZ /c
# ADD BASE RSC /l 0x407 /d "_DEBUG" /d "SYSTEM_PC_VC" /d "PRJ_USE_AUTOGEN_CONFIG=config_2_9_RemoteWriteSetpointStdIso.h"
# ADD RSC /l 0x407 /d "_DEBUG" /d "SYSTEM_PC_VC" /d "PRJ_USE_AUTOGEN_CONFIG=config_2_9_RemoteWriteSetpointStdIso.h"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib  kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib winmm.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept /libpath:""

!ENDIF

# Begin Target

# Name "2_9_RemoteWriteSetpointStdIso - Win32 Release"
# Name "2_9_RemoteWriteSetpointStdIso - Win32 Debug"
# Begin Group "Quellcodedateien"

# PROP Default_Filter "cc;cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File
SOURCE=..\..\..\xgpl_src\IsoAgLib\comm\SystemMgmt\impl\monitoritem_c.cpp
# End Source File

# Begin Source File
SOURCE=..\..\..\xgpl_src\IsoAgLib\comm\SystemMgmt\impl\identitem_c.cpp
# End Source File

# Begin Source File
SOURCE=..\..\..\xgpl_src\IsoAgLib\comm\SystemMgmt\impl\baseitem_c.cpp
# End Source File

# Begin Source File
SOURCE=..\..\..\xgpl_src\IsoAgLib\comm\SystemMgmt\impl\istate_c.cpp
# End Source File

# Begin Source File
SOURCE=..\..\..\xgpl_src\IsoAgLib\comm\SystemMgmt\impl\systemmgmt_c.cpp
# End Source File

# Begin Source File
SOURCE=..\..\..\xgpl_src\IsoAgLib\comm\SystemMgmt\ISO11783\impl\isomonitor_c.cpp
# End Source File

# Begin Source File
SOURCE=..\..\..\xgpl_src\IsoAgLib\comm\SystemMgmt\ISO11783\impl\isosystempkg_c.cpp
# End Source File

# Begin Source File
SOURCE=..\..\..\xgpl_src\IsoAgLib\comm\SystemMgmt\ISO11783\impl\isoitem_c.cpp
# End Source File

# Begin Source File
SOURCE=..\..\..\xgpl_src\IsoAgLib\comm\SystemMgmt\ISO11783\impl\isoname_c.cpp
# End Source File

# Begin Source File
SOURCE=..\..\..\xgpl_src\IsoAgLib\comm\SystemMgmt\ISO11783\impl\isorequestpgn_c.cpp
# End Source File

# Begin Source File
SOURCE=..\..\..\xgpl_src\IsoAgLib\comm\Scheduler\impl\scheduler_c.cpp
# End Source File

# Begin Source File
SOURCE=..\..\..\xgpl_src\IsoAgLib\util\impl\cancustomer_c.cpp
# End Source File

# Begin Source File
SOURCE=..\..\..\xgpl_src\IsoAgLib\util\impl\canpkg_c.cpp
# End Source File

# Begin Source File
SOURCE=..\..\..\xgpl_src\IsoAgLib\util\impl\canpkgext_c.cpp
# End Source File

# Begin Source File
SOURCE=..\..\..\xgpl_src\IsoAgLib\util\impl\util_funcs.cpp
# End Source File

# Begin Source File
SOURCE=..\..\..\xgpl_src\IsoAgLib\util\liberr_c.cpp
# End Source File

# Begin Source File
SOURCE=..\..\..\xgpl_src\IsoAgLib\comm\Process\impl\generalcommand_c.cpp
# End Source File

# Begin Source File
SOURCE=..\..\..\xgpl_src\IsoAgLib\comm\Process\impl\process_c.cpp
# End Source File

# Begin Source File
SOURCE=..\..\..\xgpl_src\IsoAgLib\comm\Process\impl\processpkg_c.cpp
# End Source File

# Begin Source File
SOURCE=..\..\..\xgpl_src\IsoAgLib\comm\Process\impl\processelementbase_c.cpp
# End Source File

# Begin Source File
SOURCE=..\..\..\xgpl_src\IsoAgLib\comm\Process\impl\procident_c.cpp
# End Source File

# Begin Source File
SOURCE=..\..\..\xgpl_src\IsoAgLib\comm\Process\impl\procdatabase_c.cpp
# End Source File

# Begin Source File
SOURCE=..\..\..\xgpl_src\IsoAgLib\comm\Process\StdMeasureElements\impl\measureprogbase_c.cpp
# End Source File

# Begin Source File
SOURCE=..\..\..\xgpl_src\IsoAgLib\comm\Process\StdMeasureElements\impl\measuresubprog_c.cpp
# End Source File

# Begin Source File
SOURCE=..\..\..\xgpl_src\IsoAgLib\comm\Process\Remote\impl\procdataremotebase_c.cpp
# End Source File

# Begin Source File
SOURCE=..\..\..\xgpl_src\IsoAgLib\comm\Process\Remote\StdMeasureElements\impl\measureprogremote_c.cpp
# End Source File

# Begin Source File
SOURCE=..\..\..\xgpl_src\IsoAgLib\comm\Process\Remote\Std\impl\procdataremote_c.cpp
# End Source File

# Begin Source File
SOURCE=..\..\..\xgpl_src\IsoAgLib\comm\Process\Remote\StdSetpointElements\impl\setpointremote_c.cpp
# End Source File

# Begin Source File
SOURCE=..\..\..\xgpl_src\IsoAgLib\comm\Process\StdSetpointElements\impl\setpointregister_c.cpp
# End Source File

# Begin Source File
SOURCE=..\..\..\xgpl_src\IsoAgLib\comm\Process\StdSetpointElements\impl\setpointbase_c.cpp
# End Source File

# Begin Source File
SOURCE=..\..\..\xgpl_src\IsoAgLib\comm\Process\processdatachangehandler_c.cpp
# End Source File

# Begin Source File
SOURCE=..\..\..\xgpl_src\supplementary_driver\hal\pc\rs232\target_extension_rs232_w32_sys.cpp
# End Source File

# Begin Source File
SOURCE=..\..\..\xgpl_src\supplementary_driver\driver\rs232\impl\rs232io_c.cpp
# End Source File

# Begin Source File
SOURCE=..\..\..\xgpl_src\IsoAgLib\hal\pc\can\target_extension_can_simulating.cpp
# End Source File

# Begin Source File
SOURCE=..\..\..\xgpl_src\IsoAgLib\hal\pc\can\hal_can_interface.cpp
# End Source File

# Begin Source File
SOURCE=..\..\..\xgpl_src\IsoAgLib\hal\pc\system\system_target_extensions.cpp
# End Source File

# Begin Source File
SOURCE=..\..\..\xgpl_src\IsoAgLib\driver\can\impl\filterbox_c.cpp
# End Source File

# Begin Source File
SOURCE=..\..\..\xgpl_src\IsoAgLib\driver\can\impl\msgobj_c.cpp
# End Source File

# Begin Source File
SOURCE=..\..\..\xgpl_src\IsoAgLib\driver\can\impl\ident_c.cpp
# End Source File

# Begin Source File
SOURCE=..\..\..\xgpl_src\IsoAgLib\driver\can\impl\canio_c.cpp
# End Source File

# Begin Source File
SOURCE=..\..\..\xgpl_src\IsoAgLib\driver\system\impl\system_c.cpp
# End Source File

# Begin Source File
SOURCE=..\..\..\IsoAgLib_Examples\tutorial\2_ProcessData\2_9_RemoteWriteSetpointStdIso\2_9_RemoteWriteSetpointStdIso.cpp
# End Source File

# End Group

# Begin Group "Header-Dateien"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File
SOURCE=..\..\..\xgpl_src\IsoAgLib\comm\SystemMgmt\impl\systemmgmt_c.h
# End Source File

# Begin Source File
SOURCE=..\..\..\xgpl_src\IsoAgLib\comm\SystemMgmt\impl\baseitem_c.h
# End Source File

# Begin Source File
SOURCE=..\..\..\xgpl_src\IsoAgLib\comm\SystemMgmt\impl\istate_c.h
# End Source File

# Begin Source File
SOURCE=..\..\..\xgpl_src\IsoAgLib\comm\SystemMgmt\impl\identitem_c.h
# End Source File

# Begin Source File
SOURCE=..\..\..\xgpl_src\IsoAgLib\comm\SystemMgmt\impl\monitoritem_c.h
# End Source File

# Begin Source File
SOURCE=..\..\..\xgpl_src\IsoAgLib\comm\SystemMgmt\ISO11783\impl\isoitem_c.h
# End Source File

# Begin Source File
SOURCE=..\..\..\xgpl_src\IsoAgLib\comm\SystemMgmt\ISO11783\impl\isoname_c.h
# End Source File

# Begin Source File
SOURCE=..\..\..\xgpl_src\IsoAgLib\comm\SystemMgmt\ISO11783\impl\isosystempkg_c.h
# End Source File

# Begin Source File
SOURCE=..\..\..\xgpl_src\IsoAgLib\comm\SystemMgmt\ISO11783\impl\isomonitor_c.h
# End Source File

# Begin Source File
SOURCE=..\..\..\xgpl_src\IsoAgLib\comm\SystemMgmt\ISO11783\impl\isorequestpgnhandler_c.h
# End Source File

# Begin Source File
SOURCE=..\..\..\xgpl_src\IsoAgLib\comm\SystemMgmt\ISO11783\impl\isorequestpgn_c.h
# End Source File

# Begin Source File
SOURCE=..\..\..\xgpl_src\IsoAgLib\comm\SystemMgmt\ISO11783\iisomonitor_c.h
# End Source File

# Begin Source File
SOURCE=..\..\..\xgpl_src\IsoAgLib\comm\SystemMgmt\ISO11783\iisoitem_c.h
# End Source File

# Begin Source File
SOURCE=..\..\..\xgpl_src\IsoAgLib\comm\SystemMgmt\ISO11783\iisoname_c.h
# End Source File

# Begin Source File
SOURCE=..\..\..\xgpl_src\IsoAgLib\comm\SystemMgmt\iidentitem_c.h
# End Source File

# Begin Source File
SOURCE=..\..\..\xgpl_src\IsoAgLib\comm\SystemMgmt\istate_c.h
# End Source File

# Begin Source File
SOURCE=..\..\..\xgpl_src\IsoAgLib\comm\SystemMgmt\imonitoritem_c.h
# End Source File

# Begin Source File
SOURCE=..\..\..\xgpl_src\IsoAgLib\comm\SystemMgmt\isystemmgmt_c.h
# End Source File

# Begin Source File
SOURCE=..\..\..\xgpl_src\IsoAgLib\comm\Scheduler\impl\scheduler_c.h
# End Source File

# Begin Source File
SOURCE=..\..\..\xgpl_src\IsoAgLib\comm\Scheduler\ischeduler_c.h
# End Source File

# Begin Source File
SOURCE=..\..\..\xgpl_src\IsoAgLib\util\impl\devkey_c.h
# End Source File

# Begin Source File
SOURCE=..\..\..\xgpl_src\IsoAgLib\util\impl\singleton_container_decl.h
# End Source File

# Begin Source File
SOURCE=..\..\..\xgpl_src\IsoAgLib\util\impl\util_funcs.h
# End Source File

# Begin Source File
SOURCE=..\..\..\xgpl_src\IsoAgLib\util\impl\singleton.h
# End Source File

# Begin Source File
SOURCE=..\..\..\xgpl_src\IsoAgLib\util\impl\singleton_container.h
# End Source File

# Begin Source File
SOURCE=..\..\..\xgpl_src\IsoAgLib\util\impl\cancustomer_c.h
# End Source File

# Begin Source File
SOURCE=..\..\..\xgpl_src\IsoAgLib\util\impl\canpkg_c.h
# End Source File

# Begin Source File
SOURCE=..\..\..\xgpl_src\IsoAgLib\util\impl\canpkgext_c.h
# End Source File

# Begin Source File
SOURCE=..\..\..\xgpl_src\IsoAgLib\util\impl\elementbase_c.h
# End Source File

# Begin Source File
SOURCE=..\..\..\xgpl_src\IsoAgLib\util\icancustomer_c.h
# End Source File

# Begin Source File
SOURCE=..\..\..\xgpl_src\IsoAgLib\util\icanpkgext_c.h
# End Source File

# Begin Source File
SOURCE=..\..\..\xgpl_src\IsoAgLib\util\icanpkg_c.h
# End Source File

# Begin Source File
SOURCE=..\..\..\xgpl_src\IsoAgLib\util\iutil_funcs.h
# End Source File

# Begin Source File
SOURCE=..\..\..\xgpl_src\IsoAgLib\util\liberr_c.h
# End Source File

# Begin Source File
SOURCE=..\..\..\xgpl_src\IsoAgLib\util\idevkey_c.h
# End Source File

# Begin Source File
SOURCE=..\..\..\xgpl_src\IsoAgLib\util\config.h
# End Source File

# Begin Source File
SOURCE=..\..\..\xgpl_src\IsoAgLib\util\compiler_adaptation.h
# End Source File

# Begin Source File
SOURCE=..\..\..\xgpl_src\IsoAgLib\comm\Process\impl\procident_c.h
# End Source File

# Begin Source File
SOURCE=..\..\..\xgpl_src\IsoAgLib\comm\Process\impl\procdatabase_c.h
# End Source File

# Begin Source File
SOURCE=..\..\..\xgpl_src\IsoAgLib\comm\Process\impl\proc_c.h
# End Source File

# Begin Source File
SOURCE=..\..\..\xgpl_src\IsoAgLib\comm\Process\impl\generalcommand_c.h
# End Source File

# Begin Source File
SOURCE=..\..\..\xgpl_src\IsoAgLib\comm\Process\impl\process_c.h
# End Source File

# Begin Source File
SOURCE=..\..\..\xgpl_src\IsoAgLib\comm\Process\impl\processpkg_c.h
# End Source File

# Begin Source File
SOURCE=..\..\..\xgpl_src\IsoAgLib\comm\Process\impl\processelementbase_c.h
# End Source File

# Begin Source File
SOURCE=..\..\..\xgpl_src\IsoAgLib\comm\Process\StdMeasureElements\impl\measuresubprog_c.h
# End Source File

# Begin Source File
SOURCE=..\..\..\xgpl_src\IsoAgLib\comm\Process\StdMeasureElements\impl\measureprogbase_c.h
# End Source File

# Begin Source File
SOURCE=..\..\..\xgpl_src\IsoAgLib\comm\Process\Remote\impl\procdataremotebase_c.h
# End Source File

# Begin Source File
SOURCE=..\..\..\xgpl_src\IsoAgLib\comm\Process\Remote\StdMeasureElements\impl\measureprogremote_c.h
# End Source File

# Begin Source File
SOURCE=..\..\..\xgpl_src\IsoAgLib\comm\Process\Remote\StdMeasureElements\imeasureprogremote_c.h
# End Source File

# Begin Source File
SOURCE=..\..\..\xgpl_src\IsoAgLib\comm\Process\Remote\Std\impl\procdataremote_c.h
# End Source File

# Begin Source File
SOURCE=..\..\..\xgpl_src\IsoAgLib\comm\Process\Remote\Std\iprocdataremote_c.h
# End Source File

# Begin Source File
SOURCE=..\..\..\xgpl_src\IsoAgLib\comm\Process\Remote\StdSetpointElements\impl\setpointremote_c.h
# End Source File

# Begin Source File
SOURCE=..\..\..\xgpl_src\IsoAgLib\comm\Process\Remote\StdSetpointElements\isetpointremote_c.h
# End Source File

# Begin Source File
SOURCE=..\..\..\xgpl_src\IsoAgLib\comm\Process\StdSetpointElements\impl\setpointregister_c.h
# End Source File

# Begin Source File
SOURCE=..\..\..\xgpl_src\IsoAgLib\comm\Process\StdSetpointElements\impl\setpointbase_c.h
# End Source File

# Begin Source File
SOURCE=..\..\..\xgpl_src\IsoAgLib\comm\Process\StdSetpointElements\isetpointregister_c.h
# End Source File

# Begin Source File
SOURCE=..\..\..\xgpl_src\IsoAgLib\comm\Process\iprocess_c.h
# End Source File

# Begin Source File
SOURCE=..\..\..\xgpl_src\IsoAgLib\comm\Process\processdatachangehandler_c.h
# End Source File

# Begin Source File
SOURCE=..\..\..\xgpl_src\IsoAgLib\comm\Process\proc_c.h
# End Source File

# Begin Source File
SOURCE=..\..\..\xgpl_src\Application_Config\isoaglib_config.h
# End Source File

# Begin Source File
SOURCE=..\..\..\xgpl_src\IsoAgLib\hal\pc\typedef.h
# End Source File

# Begin Source File
SOURCE=..\..\..\xgpl_src\IsoAgLib\hal\config.h
# End Source File

# Begin Source File
SOURCE=..\..\..\xgpl_src\IsoAgLib\typedef.h
# End Source File

# Begin Source File
SOURCE=..\..\..\xgpl_src\supplementary_driver\hal\pc\rs232\rs232_target_extensions.h
# End Source File

# Begin Source File
SOURCE=..\..\..\xgpl_src\supplementary_driver\hal\rs232.h
# End Source File

# Begin Source File
SOURCE=..\..\..\xgpl_src\supplementary_driver\driver\rs232\impl\rs232io_c.h
# End Source File

# Begin Source File
SOURCE=..\..\..\xgpl_src\supplementary_driver\driver\rs232\irs232io_c.h
# End Source File

# Begin Source File
SOURCE=..\..\..\xgpl_src\IsoAgLib\hal\pc\can\can_msq.h
# End Source File

# Begin Source File
SOURCE=..\..\..\xgpl_src\IsoAgLib\hal\pc\can\can.h
# End Source File

# Begin Source File
SOURCE=..\..\..\xgpl_src\IsoAgLib\hal\pc\can\can_typedef.h
# End Source File

# Begin Source File
SOURCE=..\..\..\xgpl_src\IsoAgLib\hal\pc\can\can_server.h
# End Source File

# Begin Source File
SOURCE=..\..\..\xgpl_src\IsoAgLib\hal\pc\can\hal_can_interface.h
# End Source File

# Begin Source File
SOURCE=..\..\..\xgpl_src\IsoAgLib\hal\pc\can\can_target_extensions.h
# End Source File

# Begin Source File
SOURCE=..\..\..\xgpl_src\IsoAgLib\hal\pc\system\system.h
# End Source File

# Begin Source File
SOURCE=..\..\..\xgpl_src\IsoAgLib\hal\pc\system\system_target_extensions.h
# End Source File

# Begin Source File
SOURCE=..\..\..\xgpl_src\IsoAgLib\hal\pc\config.h
# End Source File

# Begin Source File
SOURCE=..\..\..\xgpl_src\IsoAgLib\hal\pc\errcodes.h
# End Source File

# Begin Source File
SOURCE=..\..\..\xgpl_src\IsoAgLib\hal\can.h
# End Source File

# Begin Source File
SOURCE=..\..\..\xgpl_src\IsoAgLib\hal\system.h
# End Source File

# Begin Source File
SOURCE=..\..\..\xgpl_src\IsoAgLib\driver\can\impl\canio_c.h
# End Source File

# Begin Source File
SOURCE=..\..\..\xgpl_src\IsoAgLib\driver\can\impl\filterbox_c.h
# End Source File

# Begin Source File
SOURCE=..\..\..\xgpl_src\IsoAgLib\driver\can\impl\msgobj_c.h
# End Source File

# Begin Source File
SOURCE=..\..\..\xgpl_src\IsoAgLib\driver\can\impl\ident_c.h
# End Source File

# Begin Source File
SOURCE=..\..\..\xgpl_src\IsoAgLib\driver\can\iident_c.h
# End Source File

# Begin Source File
SOURCE=..\..\..\xgpl_src\IsoAgLib\driver\can\icanio_c.h
# End Source File

# Begin Source File
SOURCE=..\..\..\xgpl_src\IsoAgLib\driver\system\impl\system_c.h
# End Source File

# Begin Source File
SOURCE=..\..\..\xgpl_src\IsoAgLib\driver\system\isystem_c.h
# End Source File

# End Group
# End Target
# End Project

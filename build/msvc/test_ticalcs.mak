# Microsoft Developer Studio Generated NMAKE File, Based on test_ticalcs.dsp
!IF "$(CFG)" == ""
CFG=test_ticalcs - Win32 Debug
!MESSAGE No configuration specified. Defaulting to test_ticalcs - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "test_ticalcs - Win32 Release" && "$(CFG)" != "test_ticalcs - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "test_ticalcs.mak" CFG="test_ticalcs - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "test_ticalcs - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "test_ticalcs - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "test_ticalcs - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release

!IF "$(RECURSE)" == "0" 

ALL : ".\test_ticalcs.exe"

!ELSE 

ALL : "ticables - Win32 Release" "ticalcs - Win32 Release" ".\test_ticalcs.exe"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :"ticalcs - Win32 ReleaseCLEAN" "ticables - Win32 ReleaseCLEAN" 
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\test_ticalcs.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase ".\test_ticalcs.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /ML /W3 /GX /O2 /I "../../cables/src/" /I "../../files/src" /I "../../" /D "NDEBUG" /D "WIN32" /D "_CONSOLE" /D "_MBCS" /D "__WIN32__" /Fp"$(INTDIR)\test_ticalcs.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\test_ticalcs.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /incremental:no /pdb:"$(OUTDIR)\test_ticalcs.pdb" /machine:I386 /out:"test_ticalcs.exe" 
LINK32_OBJS= \
	"$(INTDIR)\test_ticalcs.obj" \
	".\ticalcs.lib" \
	"..\..\..\libticables-devel\build\msvc\ticables.lib"

".\test_ticalcs.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "test_ticalcs - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug

!IF "$(RECURSE)" == "0" 

ALL : "..\..\..\tilp\win32\test\test_ticalcs.exe"

!ELSE 

ALL : "ticables - Win32 Debug" "ticalcs - Win32 Debug" "..\..\..\tilp\win32\test\test_ticalcs.exe"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :"ticalcs - Win32 DebugCLEAN" "ticables - Win32 DebugCLEAN" 
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\test_ticalcs.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\test_ticalcs.pdb"
	-@erase "..\..\..\tilp\win32\test\test_ticalcs.exe"
	-@erase "..\..\..\tilp\win32\test\test_ticalcs.ilk"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MLd /W3 /Gm /GX /ZI /Od /I "../../cables/src/" /I "../../files/src" /I "../../" /I "../../../libticables/src" /I "../../../libtifiles/src" /D "_DEBUG" /D "__WIN32__" /D "WIN32" /D "_CONSOLE" /D "_MBCS" /Fp"$(INTDIR)\test_ticalcs.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\test_ticalcs.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /incremental:yes /pdb:"$(OUTDIR)\test_ticalcs.pdb" /debug /machine:I386 /out:"..\..\..\tilp\win32\test\test_ticalcs.exe" /pdbtype:sept 
LINK32_OBJS= \
	"$(INTDIR)\test_ticalcs.obj" \
	"C:\msvc\tilp\ticalcs.lib" \
	"C:\msvc\tilp\ticables.lib"

"..\..\..\tilp\win32\test\test_ticalcs.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 

.c{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.c{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("test_ticalcs.dep")
!INCLUDE "test_ticalcs.dep"
!ELSE 
!MESSAGE Warning: cannot find "test_ticalcs.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "test_ticalcs - Win32 Release" || "$(CFG)" == "test_ticalcs - Win32 Debug"
SOURCE=..\..\tests\test_ticalcs.c

"$(INTDIR)\test_ticalcs.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!IF  "$(CFG)" == "test_ticalcs - Win32 Release"

"ticalcs - Win32 Release" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F .\ticalcs.mak CFG="ticalcs - Win32 Release" 
   cd "."

"ticalcs - Win32 ReleaseCLEAN" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F .\ticalcs.mak CFG="ticalcs - Win32 Release" RECURSE=1 CLEAN 
   cd "."

!ELSEIF  "$(CFG)" == "test_ticalcs - Win32 Debug"

"ticalcs - Win32 Debug" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F .\ticalcs.mak CFG="ticalcs - Win32 Debug" 
   cd "."

"ticalcs - Win32 DebugCLEAN" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F .\ticalcs.mak CFG="ticalcs - Win32 Debug" RECURSE=1 CLEAN 
   cd "."

!ENDIF 

!IF  "$(CFG)" == "test_ticalcs - Win32 Release"

"ticables - Win32 Release" : 
   cd "\sources\misc\libticables-devel\build\msvc"
   $(MAKE) /$(MAKEFLAGS) /F ".\ticables.mak" CFG="ticables - Win32 Release" 
   cd "..\..\..\libticalcs\build\msvc"

"ticables - Win32 ReleaseCLEAN" : 
   cd "\sources\misc\libticables-devel\build\msvc"
   $(MAKE) /$(MAKEFLAGS) /F ".\ticables.mak" CFG="ticables - Win32 Release" RECURSE=1 CLEAN 
   cd "..\..\..\libticalcs\build\msvc"

!ELSEIF  "$(CFG)" == "test_ticalcs - Win32 Debug"

"ticables - Win32 Debug" : 
   cd "\sources\misc\libticables-devel\build\msvc"
   $(MAKE) /$(MAKEFLAGS) /F ".\ticables.mak" CFG="ticables - Win32 Debug" 
   cd "..\..\..\libticalcs\build\msvc"

"ticables - Win32 DebugCLEAN" : 
   cd "\sources\misc\libticables-devel\build\msvc"
   $(MAKE) /$(MAKEFLAGS) /F ".\ticables.mak" CFG="ticables - Win32 Debug" RECURSE=1 CLEAN 
   cd "..\..\..\libticalcs\build\msvc"

!ENDIF 


!ENDIF 


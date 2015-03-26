CC=cl
CXX=cl
AR=lib
RC=rc
LINK=link
MT=mt
RM=del /Q

CFLAGS=/nologo /GF /W4 /c
ARFLAGS=/nologo
RCFLAGS=/nologo
LFLAGS=/nologo /LIBPATH:..\lib
MTFLAGS=/nologo

DEFINES=/DWIN32 /D_CONSOLE /DPCRE_STATIC
INCLUDES=/I. /I..\pcre /I..\squirrel
LIBS=kernel32.lib user32.lib shell32.lib advapi32.lib squirrel.lib pcre.lib

!if "$(DYNAMIC_CRT)" == "y"
CFLAGS_CRT=/MD
!else
CFLAGS_CRT=/MT
!endif

!if "$(DEBUG)" == "y"
CFLAGS=$(CFLAGS) /Od /GeS /Zi $(CFLAGS_CRT)d
LFLAGS=$(LFLAGS) /DEBUG /PDB:$(target).pdb
DEFINES=$(DEFINES) /D_DEBUG
!else
CFLAGS=$(CFLAGS) /O2isy /GL $(CFLAGS_CRT)
ARFLAGS=$(ARFLAGS) /LTCG
LFLAGS=$(LFLAGS) /RELEASE /OPT:REF /OPT:ICF /LTCG
DEFINES=$(DEFINES) /DNDEBUG
!endif

!if "$(X64)" == "y"
ARFLAGS=$(ARFLAGS)
LFLAGS=$(LFLAGS) /SUBSYSTEM:CONSOLE,5.02
DEFINES=$(DEFINES) /DWIN64 /D_WIN64 /D_SQ64
!else
ARFLAGS=$(ARFLAGS)
LFLAGS=$(LFLAGS) /SUBSYSTEM:CONSOLE,5.00
!endif

# Use manifest in Visual Studio >=2008 builds to add UAC stuff
!if "$(_NMAKE_VER)" == "9.00.21022.08" || "$(_NMAKE_VER)" == "9.00.30729.01" || "$(_NMAKE_VER)" == "10.00.30319.01"
LFLAGS=$(LFLAGS) /MANIFEST /MANIFESTFILE:$(manifestfile)
EMBED_MANIFEST=$(MT) $(MTFLAGS) /manifest $(manifestfile) /outputresource:$(target);1
!endif

CXXFLAGS=$(CFLAGS) /GR- /EHsc
mkoutdir=@if not exist "$(target_dir)" mkdir "$(target_dir)"

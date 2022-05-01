{ Copyright (c) 2021, Wojciech Owczarek }
{ All rights reserved }
{ BSD 2-clause licence - see LICENSE in the top directory }
@cls
@echo Builiding PBNET unit...
@comp PBNET.INT
@cls
@echo Builiding PBNET library...
@comp PBNET.PAS
@cls
@echo Building net.exe tool...
@comp net.pas
@cls
@echo Cleaning up...
@del net.pas
@del PBNET.INT
@del PBNET.PAS
@echo Done!
@del build.bat

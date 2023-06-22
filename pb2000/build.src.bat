{ Copyright (c) 2021, Wojciech Owczarek }
{ All rights reserved }
{ BSD 2-clause licence - see LICENSE in the top directory }
@XXnoclean=1
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
@if XX%1=1 then goto noclean
@echo Cleaning up...
@del net.pas
@del PBNET.INT
@del PBNET.PAS
@echo Done!
@del build.bat
@goto end
:noclean
@echo Done. Source files not removed.
:end

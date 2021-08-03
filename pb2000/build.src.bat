{ Copyright (c) 2021, Wojciech Owczarek }
{ All rights reserved }
{ BSD 2-clause licence - see LICENSE in the top directory }

@echo Builiding PBNET library...
@comp PBNET.INT
@comp PBNET.PAS
@echo Building net.exe tool...
@comp net.pas
@echo Cleaning up...
@del net.pas
@del PBNET.INT
@del PBNET.PAS
@echo Done!
@del build.bat

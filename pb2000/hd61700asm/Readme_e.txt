-------------------------------------------------------------------------
 HD61     :  HD61700 CROSS ASSEMBLER for Win32 (Rev 0.44)
 System   :  Win95/98/98SE/NT4/2000/XP/Vista/8/10
          :  PB-1000 + RP-32 & RS232C Interface (FA-7 or MD-100)
 Produce  :  Copyright (c) BLUE (QZE12045@NIFTY.COM) 2003 - 2021
 Homepage :  http://hd61700.yukimizake.net/ (source code)
          :  http://hd61700.yukimizake.net/  (Casio PB-1000 Forever!)
-------------------------------------------------------------------------
About HD61.
-----------
HD61 is a Win9x/NT4/2000/XP based HD61700 cross assembler for the CASIO PB-1000/
AI-1000/FX-870P/VX-3/VX-4 systems.

HD61 is free for your own personal use and can be distributed freely as long
as it is not modified. This text file must be included in any distribution.

item.
------
1. HD61 has the upper compatibility function to PB-1000 built-in assembler. 
2. The symbol label can be defined up to 16 character length. 
   And, it can be defined as long as the memory capacity permits. 
3. Output file form is DATA sentence of BASIC or format of PBF or QL(note 1)
   or PAS.
4. Supporting the all of documented or undocumented HD61700 instructions.
   Supporting mnemonic description of "Europe" from revision 0.41.
   (option '/eu' or directive '#EU')
5. Supporting directives are ORG/START/EQU/DB/DW/DS/LEVEL/#IF/#ELSE/#ENDIF/
   #LIST/#NOLIST/#INCLUDE/.. etc.

(note 1)
PBF form is used 'The mutual change program of the binary file, the text file'.
It is published 'Casio PB-1000 Forever!( http://www.lsigame.com/ )'.

/////////////////////////
/////   Attention   /////
/////////////////////////
A mnemonic description method of 'JP($)' is changed from revision 0.34.
Please use HD61 Rev0.33 when you want to use a past mnemonic'JP($)'. 
 Changed mnemonic.
  Rev 0.33      Rev 0.34(or later)
  JP ($C5)  --> JP $C5

 Download Rev 0.33
  http://hd61700.yukimizake.net/HD61R033.zip (Win32)
  http://hd61700.yukimizake.net/HD61D033.zip (Dos)

Contents in the compression file.
---------------------------------
Readme_e.txt ----- Document(English) :This File
Readme.txt   ----- Document(Japanese)
HD61.EXE     ----- HD61700 cross assembler execute file.
HD61700.s    ----- All operation sample for HD61.(Japanese Commented)
LCD.s        ----- LCD control sample (for PB-1000/C/AI-1000/FX-870P/VX-4)
PbfToBin.bas ----- ASCII-> BINARY converter for PBF form.(for PB-1000/C (c)Jun Amano )
Trans.b      ----- ASCII-> BINARY converter for BAS form.(for PB-1000/C/AI-1000)
TransVx.b    ----- ASCII-> BINARY converter for PBF form.(for FX-870P/VX-4/VX-3)

Using the HD61
--------------
HD61 [name of the source file] [Output options] [Option for symbol definition ...]

When the assembly of the source file ends normally,it is displayed,
 "ASSEMBLY COMPLETE ,NO ERRORS FOUND". 
And, '*.bas' or '*.pbf' or '*.ql' or '*.pas' and '*.lst' file are output. 
When the error occurs ,the error line is displayed.

for example)
 >hd61 hd61700.s [Enter]
 HD61700 ASSEMBLER Rev 0.43
 Input : hd61700.s 
  PASS 1 END
  PASS 2 END
 ASSEMBLY COMPLETE, NO ERRORS FOUND
 >
 --> 'hd61700.bas' and 'hd61700.lst' file are output.

(1)Output options.
 /p  --------- Output file is PBF format.(default: BASIC format.)
 /q  --------- Output file is QL(Quick Loader) format.
 /s  --------- Output file is PAS(Dl-Pascal inline) format.()
 /n  --------- Not optimize.(note 2)
 /tab -------- Output '*.lst' use tab=8 code.
 /w  --------- Selects the 16bit(word size) addressing.(LEVEL 0 fixation)
               The assembly code for internal ROM is output.
 /r  --------- Output relocate information file.(*.roc)
 /o filename - Set filename in BAS/PBF headder.
 /eu  -------- Setting mnemonic description of "Europe" to HD61.

 (note 2)
 When '/n' option is used, a compatible code with the PB-1000 inclusion assembler is output.
 for example)
  Mnemonic       Output code
  LD $2,$0   -->   02 42    ; default( no /n options). output 2 byte code.
  LD $2,$0   -->   02 62 00 ; added /n option. output 3 byte code.
  LD $2,$30  -->   02 22    ; default( no /n options). output 2 byte code.
  LD $2,$30  -->   02 62 1E ; added /n option. output 3 byte code.
 etc.

 Please refer to hd61700.s (And, hd61700.lst) for detailed information on the output code
 when '/n' option is used.

(2)Option for symbol definition.
  /set [symbol]=[value/symbol] or [symbol]=[value/symbol]
  The symbol label can be defined by using this option in the command line. 
  for example)
   >HD61 SAMPLE.S /SET BASE=0x7000 /SET MODEL=PB1000
  or
   >HD61 SAMPLE.S BASE=0x7000 MODEL=PB1000

The file is forwarded to PB-1000.
---------------------------------
[a] BASIC format.
 (1)make a *.bas file.
 >HD61 source.asm [enter]
   --> output 'source.bas'.
 (2)The made '*.bas' file is merged with 'Trans.b'.
   5 '*** ASC2BIN for PB-1000/C,AI-1000 ***
   10 CLEAR:READ F$,ST,ED,EX:A=ST:ED=ED+1:L=1000
   20 READ A$,S$:S=0
   30 FOR I=1 TO LEN(A$) STEP2
   40 D=VAL("&H"+MID$(A$,I,2)):POKE A,D
   50 S=S+D:A=A+1:NEXT
   60 IF RIGHT$(HEX$(S),2)<>S$ THEN BEEP:PRINT"SUM ERROR:LINE=";L:END
   70 IF A<ED THEN L=L+1:GOTO 20
   80 IF EX<>0 THEN BSAVE F$,ST,ED-ST,EX ELSE BSAVE F$,ST,ED-ST
   90 BEEP1:PRINT"FILE CREATED":END
      <---  merged '*.bas' file.
 (3)The merged file is forwarded from PC to PB-1000 by using the RS232C interface.
 (4)The binary file can be made by doing the forwarded BASIC program is RUN.

[b] PBF format.
 (1) make a *.pbf file.
 >HD61 source.asm /p [enter]
   --> output 'source.pbf'.
 (2) Transmit "PbfToBin.bas" from PC to PB-1000 and execute it.
 (3) "file =" is displayed on the screen.
     input "COM0:7" and pushes "EXE" key.
     ('COM0:7' =9600bps,'COM0:6' =4800bps)
 (4) The made '*.pbf' file is forwarded from PC to PB-1000 by using the RS232C interface.
   After the file transfer is completed, the binary file is made.

[c] QL format.
 Please refer to "README_E.TXT" appended to DA61 for information on the QL form file.
 ..sorry..(^^;

[d] PAS format. (By Wojciech Owczarek)
 DL-Pascal inline() assembler code with extension .pas.
for example)
 >HD61 source.asm /s [enter]
 ->Output 'source.pas'.
*.pas format
-----
inline(
	$D1,$02,$38,$93,$77,$AB,$02,$D6,$40,$D5,$12,$D6,$00,$D4,$02,$D6,
	$D8,$D6,$40,$55,$14,$D6,$00,$F8,$02,$D6,$20
);


HD61/HD61700 assembly language manual
--------------------------------------
The latest document can be downloaded from this site.
 http://hd61700.yukimizake.net/


Error Messages
---------------
Invalid Source File Name
Line Length is Too Long
Oprand Length is Too Long
LABEL Length is Too Long
ORG Not Entry
Operand Not Entry
EQU without Label
Illegal Oprand
START Already Defined
Illegal [,]
Illegal ['']or[(]or[)]
LABEL Already Defined
LABEL Type Mismatch
Undefined LABEL
Operation Type Mismatch
Operand Range Over
Jump Address Over
Output Buffer Over Flow
Assemble Address Over Flow
Execute Address Illegal
Could not calculate
Illegal [#if] - [#endif]
Invalid Include File Name
Could Not Nest Include
Illegal Register Number

Reference when HD61 is developed
---------------------------------
1. [CASIO PB-1000 command reference] CASIO Co.,LTD.
2. [CASIO PB-1000 Technical Handbook] Modern Co.,LTD.
3. [Pockecom Journal]1989/12 [Undocument instructions of HD61700 I] by MAIMU
4. [Pockecom Journal]1990/1  [Undocument instructions of HD61700 II] by MAIMU
5. [Pockecom Journal]1990/8  ¢KC-Disasembler£ by KOTACHAN
6. [The mutual change program of the binary file, the text file.] by Jun Amano
   ('CASIO PB-1000 Forever!' http://www.lsigame.com/ )
7. [Pockecom Journal]1992/12 [AI-Assembler] by P,H,M
8. [Pockecom Journal]1993/4  [FX-870P Assembler] by ì©é°ç äG
9. [Pockecom Journal]1995/2  [X-Assembler Ver.6Åvby N.Hayashi
10.[Analysis of undocument instructions.(PSR/GSR/ST IM8,$/LDC)] 2006/8 by Miyura(Y.ONOZAWA)
11.[Analysis of undocument instructions.(DFH JP($C5))] 2006/9 by Piotr Piatek
   ('Vintage programmable calculators' http://www.pisi.com.pl/piotr433/ )
12.[Analysis of IB register.] by Miyura / Piotr Piatek 2007/2
13.[DL-Pascal] 1988-1990 Hans Larsson/(c)DATA-LARSSON
14.[DL-Pascal inline() assembler format] By Wojciech Owczarek 2021/7/8

History of HD61
----------------
Rev 0.01  2003.01.06  First version.
Rev 0.16  2003.03.14  Support undocument instructions.(BUPS/BDNS/PPO/STL)
Rev 0.17  2003.03.18  Support undocument instructions.(BYU/BYD)
                      Fixed STL/STLW/STLM.
Rev 0.18  2003.04.01  Fixed a minor bug in DW/DB.
Rev 0.19  2003.04.06  Support JR +-IM7 form.
Rev 0.20  2003.04.22  Fixed a problem in loading of a source file.
                      Rewrite the error process.
Rev 0.21  2003.04.26  Support TS register.(PST/GST)
Rev 0.22  2003.04.26  Disable jump extension.(op:D2H,D4H,DAH,DBH,DCH,DDH)
Rev 0.23  2003.05.16  "/SET" option Added.
Rev 0.24  2003.05.25  Fixed a problrem in assembling a 'null file'.
Rev 0.25  2003.05.30  Fixed a string immediate.
Rev 0.26  2003.06.27  Support 'single quotation' string is added.
                      for example)
                      LD  $0,'"'   ; Load $0 <- &H22(Double quotation chracter)
                      LD  $0,"'"   ; Load $0 <- &H27(Single quotation chracter)
Rev 0.27  2003.06.29  Fixed '#include'.
Rev 0.28  2003.07.25  Fixed 'JR'.
                      /SET option modified.
                      /TAB option is added. List file used 'TAB=8'.(Test implement)
Rev 0.29  2003.09.01  Fixed output PBF form.
Rev 0.30  2003.09.10  Changed output name '*.BIN'->'*.EXE' in bas/pbf file,
                      when 'START 0' setting.
Rev 0.31  2003.09.10  Fixed '()' in quotation string.
Rev 0.32  2004.01.16  Fixed a minor bug.
Rev 0.33  2004.12.24  'EQU' modified.
                      /Q option added. Output quick loader form.(Test implement)
Rev 0.34  2006.09.01  (1)'#incbin()' added.
                        The file of the Windows bitmap form can be read. 
                      (2)The support of undocument instructions are added.
                        (PSR*/GSR*/LDC*/SNL*/ST IM8,$/JP($))
                      (3)'JP($)' instruction word (DEH) corrected to the mnemonic'JP $'.
                        This is needed by adding 'JP($)' instruction word(DFH).
                      (4)The specific index registers 'SX/SY/SZ' are added.
                        Main register'$0-$31' can be specified for indirect by the
                        following marks.
                        $SX or $(SX). (An initial value of SX is 31.)
                        $SY or $(SY). (An initial value of SY is 30.)
                        $SZ or $(SZ). (An initial value of SZ is 0.)
Rev 0.35  2006.09.10  The mnemonic of 'SNL*' instructions are changed to 'LDL/LDLW/LDLM'.
          2006.09.18  LCD.S was rewritten.
Rev 0.36  2006.09.25  (1)'#kc'/'#ai' added.
                      (2)Correction of error message.
                      (3)Addition of Warning.
                        Warning was displayed when there was LDM/STM instruction
                        without the third operand.
                       for example:
                        "WARNING: 'LDM' was interpreted to 'LDD' of the KC form."
Rev 0.37  2006.10.02  (1)'/w' option added.
                        Selects the 16bit(word size) addressing.(LEVEL 0 fixation)
                        The assembly code for internal ROM is output.
                      (2)It's corrected encoding of the 'GRE KY,$C5' instruction.
                        2nd op: 2xh -> 6xh
Rev 0.38  2006.11.06  Corrected a minor bug.
Rev 0.39  2006.11.08  It's corrected encoding of the 'STLW/LDLW/PPOW' instructions.
                      The priority level of the %(MOD) was changed.
                      LCD.S was rewritten.
Rev 0.40  2007.03.16  The mnemonic of 'TS' register is changed to 'IB'.
                      /r option added. Output relocate information form.(*.roc)
                      /o [filename] option added.
Rev 0.41  2008.05.05  Supporting mnemonic description of Europe.(note 3)
                      Two features added for setting mnemonic description of "Europe" to HD61.
                      (1)/eu option added.
                      (2)#eu directive added.
(note 3) Difference of mnemonic description format AI and Europe.
  +--------------------------+--------------------------+
  |description of AI(default)| description of "Europe"  |
  +--------------------------+--------------------------+
  | Specific Index Registers | Short Registers          |
  |  SX , $SX                |  #0                      |
  |  SY , $SY                |  #1                      |
  |  SZ , $SZ                |  #2                      |
  +--------------------------+--------------------------+
  | Register                 | Register                 |
  |  IB                      |  CS                      |
  +--------------------------+--------------------------+
  |  PSR                     |  PRA (Put Ram Address)   |
  |  GSR                     |  GRA (Get Ram Address)   |
  |  STL                     |  OCB (Output Casio Bus)  |
  |  LDL                     |  ICB (Input Casio Bus)   |
  |  PPO                     |  PCB (Put Casio Bus)     |
  |  BUPS IM8                |  BUP IM8                 |
  |  BDNS IM8                |  BDN IM8                 |
  |  JP $C5                  |  JPW $C5                 |
  |  JP ($C5)                |  JPW ($C5)               |
  +--------------------------+--------------------------+
  | multi byte instructions  |                          |
  |  *M(multi byte)          |  *L(long word?)          |
  |  2 - 8                   |  L2 - L8                 |
  +--------------------------+--------------------------+
  | Jump extension tags (can be omitted)                |
  |  JR                        J.                       |
  +--------------------------+--------------------------+

Rev 0.41  2009.02.12  recompiled.
Rev 0.42  2011.01.03  /r option modified.
Rev 0.43  2011.01.07  /r option modified.(.roc format renewal)
Rev 0.44  2021.07.09  (1)'/s' option added.(support *.pas format)
                      (2)Changed type from unsigned long to unsigned int.
                      (3)EOFERR is changed to NORMAL (0).
                      (4)Stopped adding EOF to output files.(Win32)

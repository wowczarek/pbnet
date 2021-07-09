/************************************************************************/
/*                                                                      */
/*  NAME : HD61700 CROSS ASSEMBLER SOURCE CODE                          */
/*  FILE : hd61700.c                                                    */
/*  Copyright (c) ���� 'BLUE' 2003-2011                                 */
/*                                                                      */
/*  REVISION HISTORY:                                                   */
/*  Rev : 0.01  2003.01.06  �ŏ��̃o�[�W����                            */
/*                          ��肠���������A�Z���u���݊��œ���          */
/*  Rev : 0.02  2003.01.07  ���������p���������P�B�A�Z���u���\��      */
/*                          �s�����ő�4096�s���疳�����ɂ���            */
/*  Rev : 0.03  2003.01.09  ����`�I�y�����h�w����\�Ƃ���            */
/*  Rev : 0.04  2003.01.16  ����`���߂�����ɒǉ�                      */
/*                          �o�͌`����pbf/bas����I���ł���l�ɂ���     */
/*  Rev : 0.05  2003.01.17  ���x���w�����(�������C���f�b�N�X���p��)    */
/*  Rev : 0.05a 2003.01.21  �G���[���b�Z�[�W�C��                        */
/*  Rev : 0.06  2003.01.22  �o�b�t�@�����64KB�t���܂őΉ��Ƃ���        */
/*  Rev : 0.07  2003.02.03  AI-1000/FX-870P/VX-4�A�Z���u���ɏ���        */
/*                          �E���x�����Z(�D�揇�ʂȂ�)�T�|�[�g          */
/*                            ���p�\�ȉ��Z�q(+-/*&|)                  */
/*                          �E���x�����w��(.U .D)�ABit���](.N)�T�|�[�g  */
/*                          �E�I�y�����h�ɕ�������w��\�Ƃ���        */
/*                          �E�}���`�o�C�g�n�j���j�b�N��XXW��XXM�ɕύX  */
/*                          �E�I�y�R�[�h0x0�`0xf�0x80�`0x8f�ɑ΂��āA   */
/*                            Jump�g�����C�l�[�u���Ƃ����B              */
/*                          �E��LJump�g�����̃��x���w���JR LABEL�`��  */
/*                            �����p�\�Ƃ����B(�ʏ��LABEL�̂݋L�q)   */
/*                          �EDW�^�����߂��C�l�[�u���Ƃ����B            */
/*                          �E��L�ύX�ɔ���/u ��p�~���A�Z�J���h�I�y   */
/*                            ���[�V�����w����f�B�t�H���g�Ƃ����B      */
/*                            (/n�I�v�V������PB-1000�݊�����)           */
/*                          �ELEVEL�^�����߃T�|�[�g                     */
/*  Rev : 0.08  2003.02.09  ���x�����Z�����̌�����                      */
/*                          ���Z�q�D�揇�ʂ⊇�ʂ̎g�p���\�Ƃ���      */
/*                          �������������P�B�I�y�����h�����������O�����B*/
/*                          ���ߌ�T�|�[�g�ǉ�(����ŏI���\��)          */
/*                          �ׂ����o�O�C��                              */
/*  Rev : 0.09  2003.02.16  #if�`#else�`#endif�}�N���̃T�|�[�g�B        */
/*                          ���̕]���p��[!]���Z�q��ǉ��B               */
/*                          �t�@�C������8�����ȏ�̎����[�j���O��\���B */
/*                          ���x���擪�����������̏ꍇ��G���[�Ƃ���悤 */
/*                          �C���B                                      */
/*  Rev : 0.10  2003.02.21  #INCLUDE,#LIST,#NOLIST,#EJECT�T�|�[�g       */
/*                          �E�C���N���[�h�t�@�C����������悤�ɏC��    */
/*                            (�l�X�g��256���x���܂�)                   */
/*                          �E���X�g�o�͂̋���/�֎~/���y�[�W���T�|�[�g  */
/*                          EQU�錾�ɂĕ������������悤�ɂ����B       */
/*                          ���߃R�[�h(0xDA,0xDB)�ɑ΂���Jump�g�������� */
/*                          �Ƃ����B(DIDM,DIUM,BYDM,BYUM,INVM,CMPM)     */
/*                          2�i��(&B)�w���ǉ��B                       */
/*                          �G���[�`�F�b�N�����C��                      */
/*  Rev : 0.11  2003.02.22  EQU,LDW���̕�����w��̃G���f�B�A�����t��   */
/*                          �����B                                      */
/*  Rev : 0.12  2003.02.28  ���x���e�[�u���𓮓I�Ɋm�ۂ���悤�ɉ��ǁB  */
/*                          �ő�o�^���������O�����B                    */
/*                          �o�̓��b�Z�[�W�C���B                        */
/*                          Win32/DOS�ł̃\�[�X�𓝍��B                 */
/*  Rev : 0.12a 2003.03.04  pbf�t�@�C���o�͂��C���B(DOS�ł̂ݔ���)      */
/*  Rev : 0.13  2003.03.04  ���ߌ�ǉ��B                                */
/*  Rev : 0.14  2003.03.05  �\��ꐧ�����ɘa�B                          */
/*                          �������W�X�^�������x���Ƃ��ė��p�Ƃ����B  */
/*  Rev : 0.15  2003.03.07  �ŏI�A�h���X�o�͕����C��                    */
/*                          ������̈������g�����A���Z���\�Ƃ����B    */
/*                          KC�`���̃j���j�b�N�ɑΉ��B                  */
/*  Rev : 0.16  2003.03.14  ���ʌ��o���̉��s�������C���B                */
/*                          �֌W(��r)���Z�q�ɑΉ��B                    */
/*                          ���Z�q��ʂ��_��ɂ����B                    */
/*  Rev : 0.17  2003.03.17  BYD/BYU�ǉ��B                               */
/*  Rev : 0.18  2003.03.25  �Q�p�X�ڂŒl���m�肵�Ȃ����x���������Ă�    */
/*                          �G���[�ƂȂ�Ȃ��ꍇ���������̂��C���B      */
/*  Rev : 0.19  2003.04.06  JR �}IM7�`���̃T�|�[�g�B                    */
/*  Rev : 0.20  2003.04.22  �\�[�X�t�@�C���ɏo�͗p�̊g���q(lst/bas/pbf) */
/*                          ���w�肵���ꍇ�̖����C���B                */
/*                          �G���[�����������B���b�Z�[�W�Ó���������    */
/*  Rev : 0.21  2003.04.25  PST TS,xx,GST TS,$��ǉ��B                  */
/*  Rev : 0.22  2003.04.26  ���߃R�[�h(0xD2�`0xDB)�ɑ΂�Jump�g�����֎~  */
/*                          (DIDM,DIUM,BYDM,BYUM,INVM,CMPM)             */
/*  Rev : 0.23  2003.05.15  /SET�I�v�V������ǉ�                        */
/*  Rev : 0.24  2003.05.25  CODE�̖����\�[�X�ɑΉ��B                    */
/*                          �S�p�X�y�[�X�𖳎�����悤�ɂ����B          */
/*  Rev : 0.25  2003.05.29  �I�y�����h������̗\�񕶎��������Ȃ������B  */
/*  Rev : 0.26  2003.06.26  �V���O���R�[�e�[�V�����w����T�|�[�g        */
/*  Rev : 0.27  2003.06.29  #include���Ńt���p�X�w���ǉ�              */
/*  Rev : 0.28  2003.07.25  ���΃W�����v�͈̓`�F�b�N�������C��          */
/*                          �R�}���h���C�������̉��ǁB                  */
/*                          /SET�I�v�V������s�v�ɂ����B                */
/*                          ���X�g�o�͎���TAB�w��(/tab)��ǉ��B         */
/*  Rev : 0.29  2003.09.01  PBF�o�͎��ɃS�~���c��ꍇ������̂��C��     */
/*  Rev : 0.30  2003.09.10  START�錾�̂���\�[�X�́A�g���q��EXE�ŏo��  */
/*                          ����悤�ɏC��                              */
/*  Rev : 0.31  2003.09.14  �R�[�e�[�V�������̊���"()"�������C��        */
/*                          �R���p�C�����Ƀ��[�j���O���o�Ă����̂��C��  */
/*  Rev : 0.32  2004.01.16  "JR"�^�O�̎w�肪OPR2�łł��Ȃ��̂��C��      */
/*  Rev : 0.33  2004.12.24  �V���{�����x���o�^(EQU)���������ǁB         */
/*                          �E1�p�X�ڂɒl���m�肵�Ă��Ȃ����x���ł����p */
/*                            �\�Ƃ���                                */
/*                          �E�V���{���o�^���̃G���[���b�Z�[�W���C��    */
/*                          �I�y�����h���Z����������                    */
/*                          �E�����\�����g����悤�ɂ���                */
/*                          /q�I�v�V�����ǉ�                            */
/*                          �E�N�C�b�N���[�_�`�����T�|�[�g              */
/*  Rev : 0.34  2006.06.12  #INCBIN�[�����߂�ǉ��B                     */
/*                          �o�C�i��/BMP�ǂݍ��݂��T�|�[�g�B            */
/*  Rev : 0.34  2006.09.01  PSR/GSR���ߌQ�ɑΉ��B                       */
/*                          ����C���f�b�N�X���W�X�^(SX,SY,SZ)�ɂ��    */
/*                          ���C�����W�X�^�\�L( $(SX)/$(SY)/$(SZ) )�ǉ� */
/*                          $SX,$SY,$SZ�\�L���\�Ƃ����B               */
/*  Rev : 0.35  2006.09.09  �j���j�b�N��ύX(SNL->LDL)                  */
/*              2006.09.10  ���b�Z�[�W���C��                            */
/*  Rev : 0.36  2006.09.25  LDM/STM���߂�LDD/STD(KC�`��)�ɐU��ւ���ꂽ*/
/*                          �ꍇ�A���[�j���O�\������悤�ɂ����B        */
/*                          #KC,#AI�^�����߂�ǉ��B                     */
/*  Rev : 0.37  2006.09.30  �o�[�W�����ԍ��̏C��Rev0.36��0.37           */
/*                          /w�I�v�V������ǉ��B16�r�b�g�A�h���X�ɑΉ��B*/
/*              2006.10.02  �f�o�b�O�R�[�h���폜�i����ɉe���Ȃ��j      */
/*  Rev : 0.38  2006.11.06  �V�t�g���Z��ǉ��B�ׂ����o�O���C���B        */
/*                          �R���p�C���I�v�V����(WITH_EOF)�ǉ��B        */
/*  Rev : 0.39  2006.11.08  % ��](MOD)�̗D�揇�ʂ�ύX�B(C���ꏀ��)    */
/*  Rev : 0.40  2007.03.16  /r�I�v�V�����w��ɂă����P�[�g���t�@�C��  */
/*                          (*.roc)���o�͂���悤�ɂ����B               */
/*                          /o [�t�@�C����] �ɂ�*.bas/*pbf�ɏo�͂���    */
/*                          �t�@�C�������w��ł���悤�ɂ����B          */
/*  Rev : 0.41  2008.05.05  Europe�`���j���j�b�N(casio original?)�ǉ��B */
/*                          /eu �I�v�V��������сA�[������ #eu�ǉ��B    */
/*  Rev : 0.42  2011.01.03  /r�I�v�V������������                        */
/*                          (1)EQU���x���o�^���Ƀ��x����ʂ������p��    */
/*                          (2)ORG���p���x�����A�h���X��ʂɍēo�^����  */
/*                          (3)DW���߂�.roc�t�@�C���̏o�͑ΏۂƂ���     */
/*                          �A�Z���u�����x������(outtbl.kind�ǉ��ɂ��) */
/*  Rev : 0.43  2011.01.07  /r�I�v�V������������                        */
/*                          RR�`��Ver.2�Ή�                             */
/*  Rev : 0.44  2021.07.09  unsigned long -> unsigned int �ɕύX        */
/*                          EOF�I�v�V�����𖳌���(DOS�ł͕ύX�Ȃ�)      */
/*                          EOFERR(�t�@�C���I��)���͐���I���Ƃ���      */
/*                          /s�I�v�V���������ǉ�                        */
/*                          DL-PASCAL�p��inline�`���o�͂�ǉ�           */
/*                                                                      */
/************************************************************************/
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<fcntl.h>
#include"hd61700.h"
/*------------------------------------------------------------------*/
/*  �萔��`                                                        */
/*------------------------------------------------------------------*/
#if __FORDOS
char	name[]	="HD61700 ASSEMBLER FOR DOS ";	/* �A�Z���u������   */
#else
char	name[]	="HD61700 ASSEMBLER ";	/* �A�Z���u������           */
#endif
char	rev[]	="Rev 0.44";			/* Revision                 */
/* ���p�\������ */
char	LabelStr[]	= "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789@_";
char	DecStr[]	= "0123456789";
char	HexStr[]	= "0123456789ABCDEFabcdef";
/*------------------------------------------------------------------*/
/*  �ϐ���`                                                        */
/*------------------------------------------------------------------*/
char	SrcFile[MAXLINE];			/* �\�[�X�t�@�C����             */
char	IncFile[MAXLINE];			/* �C���N���[�h�t�@�C����       */
char	BinFile[MAXLINE];			/* �o�C�i���t�@�C����           */
char	LstFile[MAXLINE];			/* ���X�g�t�@�C����             */
char	OutFile[MAXLINE];			/* �o�̓t�@�C����               */
char	ExeFile[MAXLINE];			/* ���s�t�@�C����               */
char	RocFile[MAXLINE];			/* ���A���P�[�g���t�@�C����   */
FILE	*IncFD[MAXINC];				/* INCLUDE�t�@�C���f�B�X�N���v�^*/
FILE	*SrcFD;						/* �\�[�X�t�@�C���f�B�X�N���v�^ */
FILE	*BinFD;						/* Binary�t�@�C���f�B�X�N���v�^ */
FILE	*LstFD;						/* ���X�g�t�@�C���f�B�X�N���v�^ */
FILE	*OutFD;						/* �o�̓t�@�C���f�B�X�N���v�^   */
FILE	*RocFD;						/* roc�t�@�C���f�B�X�N���v�^    */
unsigned short	pass;				/* �A�Z���u���p�X��(0�A1)       */
unsigned short	SetLabel;			/* /SET �I�v�V�����o�^��        */
SETOPT			SetTbl[MAXOPT];		/* /SET�I�v�V�����\����         */
unsigned short	DefInstMode;		/* 0:AI�`���A1:KC�`���A2:EU�`�� */
unsigned short	InstMode;			/* 0:AI�`���A1:KC�`���A2:EU�`�� */
unsigned short	DefUndefOpr;		/* 1:�����J�I�v�V�����I��(��)   */
unsigned short	UndefOpr;			/* 1:�����J�I�v�V�����I��       */
unsigned short	OutType;			/* 0:BASIC�A1:PBF�`��           */
unsigned short	pr;					/* ��ʏo�̓t���O               */
unsigned short	Tab;				/* TAB�o�̓t���O                */
unsigned short	OutRealloc;			/* 1:�����P�[�g���o��         */
unsigned short	ExeName;			/* 0:�Ȃ��A1:�o�̓t�@�C���w��L */
#if __WORD_ADDRESS
unsigned short	Wadr;				/* 1:16Bit Addressing�Ή�       */
#endif
unsigned int 	StartAdr;			/* �A�Z���u���J�n�A�h���X       */
unsigned int 	ExecAdr;			/* ���s�J�n�A�h���X             */
unsigned int 	AsmAdr;				/* �A�Z���u���A�h���X           */
unsigned short	StartAdrFlag;		/* �A�Z���u���J�n�A�h���X�t���O */
unsigned short	ExecAdrFlag;		/* ���s�J�n�A�h���X�t���O       */
unsigned short	LabelCnt;			/* ���x���o�^��                 */
LBL				* LabelTbl;			/* ���x���o�^�e�[�u���|�C���^   */
struct	outtbl	OutTbl;				/* �\����͌��ʃe�[�u��         */
unsigned char*	OutBuf;				/* ���ߏo�̓o�b�t�@�|�C���^     */
char			calcwk[MAXLINE+2];	/* ���x�����Z�o�b�t�@           */
int				CalcPtr;			/* ���Z�o�b�t�@�|�C���^         */
unsigned short	Ckind;				/* ���Z���̖��̉����t���O       */
int				IfLevel;			/* #if�`#endif ���x��           */
unsigned char	LblFlg;				/* ���x����ʏ��(0:EQU,1:ADR)  */
unsigned char	AsmFlag;			/* �A�Z���u���֎~/����          */
unsigned char	IfStk[IFLEVEL];		/* �A�Z���u���֎~/���X�^�b�N  */
unsigned short	ListFlag;			/* ���X�g�o�̓t���O(0:�o��)     */
unsigned short	LineFeed;			/* #eject�v���t���O(1:�v������) */
unsigned short	IncLevel;			/* INCLUDE���x��(���x��1�܂ŉ�) */
unsigned short	SrcLine;			/* �\�[�X���X�g�s�ԍ�           */
unsigned short	IncLine[MAXINC];	/* �C���N���[�h���X�g�s�ԍ�     */
char 			oprwk[MAXLEN+2];	/* �j���j�b�N/�I�y�����h�p���[�N*/
									/* ���r�����p�ɒ��ӂ��邱��     */

/*------------------------------------------------------------------*/
/*  �v���g�^�C�v�錾                                                */
/*------------------------------------------------------------------*/
int main(int argc, char *argv[]);
int AsmProcess( char * );
int InitAsm( char * File );
void ClearFlag( void );
int AsmLine( void );
int AsmCodeSet( void );
int SearchOpr( unsigned short op1 ,unsigned short op2 ,unsigned short op3 ,unsigned short op4 );
int GetJumpData( unsigned short jadr , unsigned short adr  , unsigned short byte ,unsigned short *op ,unsigned short *opdat );
int GetIndexKind( char * buff , unsigned short * kind , unsigned short * data ,unsigned short *si);
int GetOprKind( int num , char * buff , unsigned short * kind , unsigned short * data ,unsigned short *si );
int SetLabelTbl( char * buff , unsigned short adr , unsigned char flag );
int GetLabelAdr( char * buff , unsigned short * adr );
int GetIRegKind( char * buff , unsigned short * kind );
int GetFlagKind( char * buff , unsigned short * kind );
int GetMacKind( char * buff );
unsigned short GetMnemonicKind( char * buff );
int CheckLabel( char * buff );
int ChgCode( char * dst , char * src );
int ChgKcName( char * dst , char * src );
#if __EUR_MNEMONIC
int ChgEuropeName( char * dst , char * src );
#endif /* __EUR_MNEMONIC */
int GetReg( char * buff , unsigned short * data ,unsigned short * sir);
int GetLine( FILE *fd ,char *buff );
int GetParam( char *buff );
int GetData( char *buff , unsigned short * data );
int GetCalcData( char * buff , unsigned short * kind ,unsigned short * adr );
int CalcVal(unsigned short * value );
int CalcVal0(unsigned short * value );
int CalcValShift(unsigned short * value );
int CalcVal1(unsigned short * value );
int CalcVal2(unsigned short * value );
int CalcVal3( unsigned short *value );
int GetValue(unsigned short *value );
int CheckSetOpt( char * name );
int CheckSetLbl( char * name ,unsigned short sts );
int IncludeBin( unsigned short *cnt ,char *File );
int ReadBin( unsigned int * Size );
int ReadBmp( unsigned int * Size );
void PrintList( int cnt );
void ErrPut( int cnt , int err );

/**********************************************************************/
/*   main    : main routine                                           */
/*                                                                    */
/*   ����    : ���C������                                             */
/*   ����    : �R�}���h���C���I�v�V����                               */
/*   �o��    : �G���[���i0:����A0�ȊO:�ُ�j                       */
/*                                                                    */
/**********************************************************************/
int main(int argc, char *argv[])
{
int	rc,i;
char * ptr;

	/* �N���o�[�W�����̕\�� */
	printf( "%s%s\n",name,rev );

	/* �R�}���h���C���̃`�F�b�N */
	if( argc > (MAXOPT*2) ){
		printf("Invalid Parameters.\n");
		exit(1);
	}

	/* �\�[�X�t�@�C���w�肠�� */
	if ( !argv[1] ){
		printf("Invalid Source File Name.\n");
		exit (1);
	}
	/* �t�@�C�����̍쐬 */
	sprintf( SrcFile , "%s" , argv[1] );

	/* SET�I�v�V���������� */
	SetLabel = 0;
	memset( oprwk , 0 , MAXLEN+2 );
	memset( SetTbl , 0 , sizeof(SetTbl) );

	/* �I�v�V���������� */
	pr = 0;
	Tab = 0;
	DefUndefOpr = 1;
	OutType = 0;
	DefInstMode = 0; /* AI�`���I�� */
	OutRealloc = 0;
	ExeName = 0;
#if __WORD_ADDRESS
	Wadr = 0;
#endif
	/* �N���I�v�V�����擾 */
	i=2;
	while( argv[i] ){
#if __EUR_MNEMONIC
		/* �f�t�H���g�j���j�b�N���[�h�ݒ� */
		if (!strcmp("/ai",argv[i])||!strcmp("/AI",argv[i])){ DefInstMode = 0;i++; continue; }
		if (!strcmp("/kc",argv[i])||!strcmp("/KC",argv[i])){ DefInstMode = 1;i++; continue; }
		if (!strcmp("/eu",argv[i])||!strcmp("/EU",argv[i])){ DefInstMode = 2;i++; continue; }
#endif /* __EUR_MNEMONIC */
#if __WORD_ADDRESS
		/* 16�r�b�g�A�h���b�V���O�I�� */
		if (!strcmp("/w",argv[i])||!strcmp("/W",argv[i])){ Wadr = 1;i++; continue; }
#endif
		/* �����P�[�g���o�͑I�� */
		if (!strcmp("/r",argv[i])||!strcmp("/R",argv[i])){ OutRealloc = 1;i++; continue; }
		/* TAB���X�g�o�͑I�� */
		if (!strcmp("/tab",argv[i])||!strcmp("/TAB",argv[i])){ Tab = 1;i++; continue; }
		/* SIR�œK��OFF�A�Z���u���I��(LEVEL 0) */
		if (!strcmp("/n",argv[i])||!strcmp("/N",argv[i])){ DefUndefOpr = 0;i++; continue; }
		/* PBF�t�@�C���o�͑I�� */
		if (!strcmp("/p",argv[i])||!strcmp("/P",argv[i])){ OutType = 1;i++; continue; }
		/* QL�t�@�C���o�͑I�� */
		if (!strcmp("/q",argv[i])||!strcmp("/Q",argv[i])){ OutType = 2;i++; continue; }
#if __DLP_INLINE
		/* DL-Pascal inline() format */
		if (!strcmp("/s",argv[i])||!strcmp("/S",argv[i])){ OutType = 3;i++; continue; }
#endif
		/* bas/pbf�t�@�C�����w�� */
		if (!strcmp("/o",argv[i])||!strcmp("/O",argv[i])){
			/* ���p�����[�^���� */
			if( !argv[++i] || argv[i][0]=='/' ){
				/* �R�}���h���C���ُ� */
				printf("Invalid /O Parameters.\n");
				exit(1);
			}
			/* bas/pbf�p�t�@�C�������w�肷�� */
			sprintf(ExeFile,"%s",argv[i++]);
			ExeName = 1;
			continue;
		}
		/* SET�I�v�V���� */
		if (!strcmp("/set",argv[i])||!strcmp("/SET",argv[i])){
			/* ���p�����[�^���� */
			if( !argv[++i] ){
				/* �R�}���h���C���ُ� */
				printf("Invalid /SET Parameters.\n");
				exit(1);
			}
		}
		/***********************/
		/* �������o�^����    */
		/***********************/
		/* �R�}���h���C�����o�� */
		sprintf( oprwk , "%s" , argv[i] );
		/* "="������ */
		if ((ptr = strrchr( oprwk , '=' ))){
			if( ((int)(ptr - oprwk ) > MAXNAME )||( strlen(ptr) > (MAXNAME+1) ) ){
				/* �R�}���h���C���ُ� */
				printf("Invalid /SET name.\n");
				exit(1);
			}
			/* �o�^���x�����̎擾 */
			memcpy(SetTbl[SetLabel].ent,oprwk,(int)(ptr-oprwk));
			/* ���������擾 */
			sprintf(SetTbl[SetLabel].let,"%s", ptr+1 );
			/* ����=�E�ӂ܂��́A�����V���{���o�^�Ȃ�G���[�I���Ƃ��� */
			if (!strcmp(SetTbl[SetLabel].ent,SetTbl[SetLabel].let)||!CheckSetOpt( SetTbl[SetLabel].ent ) ){
				/* �R�}���h���C���ُ� */
				printf("Invalid /SET name.\n");
				exit(1);
			}
			/* 16�i�\�L��0x��&H�ɏC�� */
			if( !memcmp(SetTbl[SetLabel].let,"0x",2)||!memcmp(SetTbl[SetLabel].let,"0X",2) ){
				SetTbl[SetLabel].let[0]='&';
				SetTbl[SetLabel].let[1]='H';
			}
			/* SET�V���{�����X�V */
			SetLabel++;
			i++;
			continue;
		}
		/* �R�}���h���C���ُ� */
		printf("Invalid Parameters.\n");
		exit(1);
	}
#if __WORD_ADDRESS
	/* �œK�����֎~ */
	if (Wadr) UndefOpr = 0;
#endif
	/* �A�Z���u������ */
	rc = AsmProcess( SrcFile );

	return(rc);
}

/**********************************************************************/
/*   InitAsm : Initialize Assembler Process                           */
/*                                                                    */
/*   ����    : �A�Z���u������������                                   */
/*   ����    : �\�[�X�t�@�C����                                       */
/*   �o��    : �G���[���i0:����A0�ȊO:�ُ�j                       */
/*                                                                    */
/**********************************************************************/
int InitAsm( char * File )
{
char * fptr;
char out[8];
	/* �t�@�C���f�B�X�N���v�^������ */
	SrcFD = 0;
	BinFD = 0;
	LstFD = 0;
	OutFD = 0;
	RocFD = 0;
	/* �o�̓o�b�t�@�|�C���^������ */
	OutBuf = 0;
	/* �e��t���O/�J�E���^������ */
	ClearFlag();
	/* LABEL�e�[�u�������� */
	LabelCnt = 0;
	LabelTbl = 0;
	/* �t�@�C�����̍쐬 */
	sprintf( LstFile , "%s" , File );
	sprintf( OutFile , "%s" , File );

	if ( !ExeName ){
		/* ���s�t�@�C�����쐬 */
		if(fptr = strrchr(File,  0x5c ))
			ChgCode( ExeFile , &fptr[1] );
		else ChgCode( ExeFile , File );
	}

	/* List�t�@�C�����쐬 */
	if ((fptr = strrchr(LstFile,  '.' )))
		sprintf( fptr,".lst" );
	else	strcat( LstFile, ".lst" ); 

	/* �o�̓t�@�C�����쐬 */
	switch ( OutType ){
	case 0:
		sprintf( out ,".bas" );
		break;
	case 1:
		sprintf( out ,".pbf" );
		break;
	case 2:
		sprintf( out ,".ql" );
		break;
#if __DLP_INLINE
	case 3:
		sprintf( out ,".pas" );
		break;
#endif
	}
	if ((fptr = strrchr(OutFile, '.' )))
		sprintf( fptr,"%s", out );
	else	strcat( OutFile, out );

	/* ���A���P�[�g���t�@�C���쐬�v���L�� */
	if ( OutRealloc ){
		sprintf( RocFile , "%s" , File );
		/* ���A���P�[�g���t�@�C�����쐬 */
		if ((fptr = strrchr(RocFile,  '.' )))
			sprintf( fptr,".roc" );
		else	strcat( RocFile, ".roc" );
	}

	/* ���̓t�@�C������lst/bas/pbf�w��̏ꍇ�A�G���[�I������ */
	if( !strcmp(OutFile,SrcFile) || !strcmp(LstFile,SrcFile) ){
		printf("Invalid Source File Name.\n");
		return INZERR;
	}
	/* �\�[�X�t�@�C��OPEN */
	if ( ( SrcFD = fopen( SrcFile ,"rb" ) ) <= 0 ){
		printf("Invalid Source File Name.\n");
		return INZERR;
	}
	printf( "Input : %s \n", SrcFile );
	/* �t�@�C�������W�����ȏ�̏ꍇ�A���[�j���O��\�� */
	if ( (strrchr(ExeFile, '.' ) - ExeFile ) > 8 ){
		printf("Warning! File Name Over 8 Chracters.\n");
	}
	return NORM;
}

/**********************************************************************/
/*   ClearFlag : Clear Flag Data                                      */
/*                                                                    */
/*   ����    : �e��t���O������                                       */
/*   ����    : �Ȃ�                                                   */
/*   �o��    : �Ȃ�                                                   */
/*                                                                    */
/**********************************************************************/
void ClearFlag( void )
{
	/* �A�Z���u���J�n�A�h���X������ */
	SrcLine = 0;
	ListFlag = 0;
	LineFeed = 0;
	IncLevel = 0;
	AsmAdr = 0;
	ExecAdr = 0;
	StartAdr = 0;
	StartAdrFlag = 0;
	ExecAdrFlag = 0;
	/* �A�Z���u������ */
	AsmFlag = 0;
	/* #if�l�X�g���x�������� */
	IfLevel = 0;
	/* #if�`#endif�X�^�b�N������ */
	memset( IfStk , 0 , sizeof(IfStk) );
	/* #include��񏉊��� */
	memset( IncLine , 0 ,sizeof(IncLine));
	memset( IncFD , 0 , sizeof(IncFD));
}

/**********************************************************************/
/*   AsmProcess : Assembler Process Main Routine                      */
/*                                                                    */
/*   ����    : �A�Z���u������                                         */
/*   ����    : �\�[�X�t�@�C����                                       */
/*   �o��    : �G���[���i0:����A0�ȊO:�ُ�j                       */
/*                                                                    */
/**********************************************************************/
int AsmProcess( char * File )
{
char mnwk[MAXMN];	/* �j���j�b�N�ϊ��p���[�N */
char * fptr;
char out[8];
int rc,rc2,i,n;
unsigned short	op;
unsigned short	sum,sts,line;
unsigned short	LineCnt;	/* �s�ԍ��J�E���^ */
unsigned int	OutCnt,cnt;	/* �o�̓o�C�g�� */
unsigned int	BuffSize;	/* �o�̓o�b�t�@�T�C�Y */
LBL *Label;
LBL *Labelwk;
	/* �����ݒ� */
	if( rc = InitAsm( File ) ) return rc;

	/* SET�I�v�V�������s(�m��l�̂݃V���{���o�^����) */
	if ( SetLabel ){
		/* ���x���o�^���s */
		for ( i = 0 ; i < SetLabel ; i++ ){
			/* ����l���擾 */
			if( !( rc = GetData( SetTbl[i].let , &sts ) ) ){
				/* ���x��������`�F�b�N */
				if ( rc = CheckLabel( SetTbl[i].ent ) ){
					/* �G���[�\�� */
					printf("Invalid /SET name.\n");
					goto asm_end;
				}
				/* ���x���e�[�u���ɓo�^���� */
				if(rc = SetLabelTbl( SetTbl[i].ent , sts ,LBL_EQU )){
					/* �G���[�\�� */
					printf("Invalid /SET name.\n");
					goto asm_end;
				}
			}
			/* ����l�̓��x���ł��� */
			else{
				/* ���x��������`�F�b�N */
				if ( rc = CheckLabel(SetTbl[i].let) ){
					/* �G���[�\�� */
					printf("Invalid /SET name.\n");
					goto asm_end;
				}
			}
		}
	}
	/* ���X�g�t�@�C��OPEN */
	if ( ( LstFD = fopen( LstFile ,"w" ) ) <= 0 ){
		printf("File Create Error.\n");
		goto asm_end;
	}

	/* �P�p�X�ڃ��x���e�[�u�����쐬���� */
	UndefOpr = DefUndefOpr;
	InstMode = DefInstMode;
	LineCnt = 0;
	pass = 0;
	while( !rc ){
		rc = AsmLine();
#if __WORD_ADDRESS
		/* 16bit addressing�ɕϊ����� */
		if (Wadr){
			/* �A�h���X�␳ */
			AsmAdr -= OutTbl.byte;
			OutTbl.byte = (OutTbl.byte+1)&0xfe;
			AsmAdr += (OutTbl.byte/2);
		}
#endif
		if ( rc && (rc != EOFERR) ) {
			/* �G���[�\�� */
			ErrPut( LineCnt , rc );
			goto asm_end;
		}
		/* �s�ԍ��X�V */
		LineCnt = !IncLevel ? ++SrcLine : ++IncLine[IncLevel-1];
	}
	/* #if�`#enfif�l�X�g�ُ픭�� */
	if ( IfLevel ){
		/* �G���[�\�� */
		ErrPut( LineCnt-1 , IFNEST );
		goto asm_end;
	}
#if __WORD_ADDRESS
	/* �o�̓T�C�Y�����߂� */
	OutCnt = Wadr ? (AsmAdr*2) : AsmAdr;
#else
	OutCnt = AsmAdr;
#endif
	/* �R�[�h�͍ő�o�b�t�@�͈͓��Ɏ��܂� */
	if ( (BuffSize = (OutCnt - StartAdr) ) > MAXOBJ ){
		/* �o�̓o�b�t�@�I�[�o�[ */
		ErrPut( LineCnt-1 , AOFLOW );
		goto asm_end;
	}
	/* �\�[�X�t�@�C���擪�ɃV�[�N���� */
	if( fseek(SrcFD , 0 , SEEK_SET )){
		printf("Source File Seek Error.\n");
		goto asm_end;
	}
	/* �P�p�X�ڏI�� */
	printf( " PASS 1 END \n" );

	/* �o�̓o�b�t�@���m�ۂ���(1Byte�]���Ɏ��) */
	if ( !( OutBuf = malloc( (size_t)(BuffSize+1) ) ) ){
		printf("Output Buffer Not Allocated.\n");
		goto asm_end;
	}
	/* �o�̓o�b�t�@������ */
	memset( OutBuf, 0 , (unsigned short)(BuffSize+1) );
	/* �s�ԍ��A�A�Z���u���A�h���X�A���[�h������������ */
	ClearFlag();
	UndefOpr = DefUndefOpr;
	InstMode = DefInstMode;
	LineCnt = 0;
	OutCnt = 0;

	/* ���X�g�o�͊J�n */
	fprintf( LstFD , "%s%s - ",name,rev );
	fprintf( LstFD ,"ASSEMBLY LIST OF [%s]\n", SrcFile );

	/* �����P�[�g���t�@�C���쐬�v���L�� */
	if ( OutRealloc ){
		/* ���X�g�t�@�C��OPEN */
		if ( ( RocFD = fopen( RocFile ,"w" ) ) <= 0 ){
			printf("Roc File Create Error.\n");
			goto asm_end;
		}
		/* �w�b�_�o�� */
		fprintf(RocFD,"DW &H0000\n" );
		fprintf(RocFD,"DW &HFFFF\n" );
		/* �o�C�g�������� */
		n = 4;
	}

	/* �Q�p�X�ڃ��x���A�h���X�𔽉f���A���߃R�[�h���o�͂��� */
	rc = 0; rc2 = 0;pass++;
	while( !rc && !rc2 ){
		rc = AsmLine();
#if __WORD_ADDRESS
		/* 16bit addressing�ɕϊ����� */
		if (Wadr){
			/* �A�h���X�␳ */
			AsmAdr -= OutTbl.byte;
			OutTbl.byte = (OutTbl.byte+1)&0xfe;
			AsmAdr += (OutTbl.byte/2);
		}
#endif
		if ( rc && (rc != EOFERR) ) {
			/* �G���[�\�� */
			ErrPut( LineCnt , rc );
			goto asm_end;
		}
		/* �A�Z���u������Ԃ̎��̂ݎ��s���� */
		if (!AsmFlag){
			if ( rc2 = AsmCodeSet() ){
				ErrPut( LineCnt , rc2 );
				goto asm_end;
			}
			/* �R�[�h�̓o�b�t�@�͈͓��Ɏ��܂� */
			if ( ( OutCnt + (unsigned int)OutTbl.byte ) > BuffSize ){
				/* �o�̓o�b�t�@�I�[�o�[ */
				ErrPut( LineCnt , AOFLOW );
				goto asm_end;
			}
			/* AI�`�����[�h�Ȃ�LDM/STM���[�j���O�\��*/
			if( !InstMode && OutTbl.sts ){
				pr = 1;
				printf("LINE ");
				/* ���X�g�t�@�C���o�� */
				PrintList( LineCnt );
				/* ���[�N�G���A������ */
				memset( mnwk , 0 , sizeof(mnwk) );
				/* ���̓j���j�b�N���� */
				ChgCode( mnwk , OutTbl.opr[0] );
				mnwk[2] = 'D';
				printf("WARNING: '%s' was interpreted to '%s' of the KC form.\n",OutTbl.opr[0],mnwk);
				pr = 0;
			}
			else{
				/* ���X�g�t�@�C���o�� */
				PrintList( LineCnt );
			}
			/* �R�[�h�o�͗v������ */
			if ( OutTbl.byte ){
				/* �ʏ햽�߁^�^�����ߎ�ʂɂ�镪�� */
				switch( OutTbl.kind ){
				/* ORG/DS���� */
				case OP_ORG:
				case OP_DS:
					break;
				/* #INCBIN���� */
				case OP_INCBIN:
					/* �o�C�i���R�[�h�o�� */
					memcpy( &OutBuf[OutCnt] , OutTbl.bcode , OutTbl.byte );
					/* �o�b�t�@���J������ */
					free( OutTbl.bcode );
					break;
				default:
					/* ORG/DS/#INCBIN ���߈ȊO�Ȃ�A�f�[�^�o�� */
					memcpy( &OutBuf[OutCnt] , OutTbl.code , OutTbl.byte );
					/* �����P�[�g���t�@�C���쐬�v���L�� */
					if ( OutRealloc ){
						/* DW�^�����߂ł��� */
						if( OutTbl.kind == OP_DW ){
							cnt = 0;
							i = 0;
							/* �I�y�����h�G���g�����������o�^���� */
							while( OutTbl.opr[cnt+1] ){
								/* �擪OPR�ȊO�́A�J���}�̃`�F�b�N������ */
								if ( cnt ){
									/* �擪�������X�L�b�v */
									i = 1;
								}
								/* ���x����ʏ����� */
								LblFlg = 0;
								/* �v�Z���Ƃ��ď������� */
								if( !GetCalcData( OutTbl.opr[cnt+1]+i , &op , &sts ) ){
									/* �A�h���X���x���ł��A�R�[�h�͈͓��ł��� */
									if(( LblFlg == LBL_ADR )&&((unsigned short)StartAdr<=sts)&&((unsigned short)(StartAdr+BuffSize)>sts))
									{
										op = sts - (unsigned short)StartAdr;
										sts= OutTbl.adr + (cnt<<1) - (unsigned short)StartAdr;
										/* .roc�t�@�C���o�� */
										fprintf(RocFD,"DW &H%04X,&H%04X\n",sts, op );
										/* �o�C�g���X�V(+4) */
										n += 4;
									}
								}
								cnt++;
							}
							break;
						}
						/* ��ʖ��߈ȊO�ł��� */
						if( OutTbl.kind != UNDEFOPR ){
							break;
						}
						/* ���x����ʏ����� */
						LblFlg = 0;
						/* ���ߌ�ɂ�镪�� */
						switch( OutTbl.code[0] ){
						/* JP IM16 */
						case 0x30: case 0x31: case 0x32: case 0x33:
						case 0x34: case 0x35: case 0x36: case 0x37:
						/* CAL IM16 */
						case 0x70: case 0x71: case 0x72: case 0x73:
						case 0x74: case 0x75: case 0x76: case 0x77:
						/* LDW $,IM16 /PRE Reg,IM16 */
						case 0xD1: case 0xD6: case 0xD7:
							i = ( OutTbl.code[0]==0x37 || OutTbl.code[0]==0x77 ) ? 1 : 2;
							if( !GetCalcData( &OutTbl.opr[i][(i==2)?1:0], &op , &sts ) ){
								/* �A�h���X���x���ł��A�R�[�h�͈͓��ł��� */
								if(( LblFlg == LBL_ADR )&&((unsigned short)StartAdr<=sts)&&((unsigned short)(StartAdr+BuffSize)>sts))
								{
									/* 4�o�C�g���߂Ȃ�I�y�����h2�A3�o�C�g���߂Ȃ�I�y�����h1���ǂݏo�� */
									i = ( OutTbl.byte == 4 ) ? 2 : 1;
									op =((unsigned short)OutTbl.code[i]|(unsigned short)OutTbl.code[i+1]<<8)-(unsigned short)StartAdr;
									sts =  OutTbl.adr - (unsigned short)StartAdr +i;
									/* .roc�t�@�C���o�� */
									fprintf(RocFD,"DW &H%04X,&H%04X\n",sts, op );
									/* �o�C�g���X�V(+4) */
									n += 4;
								}
							}
							break;
						default:
							break;
						}
					}
					break;
				}
			}
			/* �o�̓o�C�g���X�V */
			OutCnt += OutTbl.byte;
		}
		else if (AsmFlag==1){
			/* ���X�g�t�@�C���o�� */
			PrintList( LineCnt );
			AsmFlag++;
		}
		/* �s�ԍ��X�V */
		LineCnt = !IncLevel ? ++SrcLine : ++IncLine[IncLevel-1];
	}
	/* �A�Z���u������I�� */
	printf( " PASS 2 END\n" );
	printf("ASSEMBLY COMPLETE, NO ERRORS FOUND\n");
	fprintf(LstFD,"\nASSEMBLY COMPLETE, NO ERRORS FOUND\n");

	/* ���x���e�[�u�����o�͂��� */
	if ( LabelCnt ){
		/* �w�b�_�o�� */
		fprintf( LstFD ,"\n%s%s - ",name,rev );
		fprintf( LstFD ,"MAP LIST OF [%s]\n", SrcFile );
		fprintf( LstFD ," LABEL           : ADDRESS(hex)  | LABEL           : ADDRESS(hex)\n" );
		fprintf( LstFD ,"-------------------------------------------------------------------\n" );
		Label = LabelTbl;
		i = 0;
		while ( Label ){
			if (!(i&1))	fprintf( LstFD ," %-16s:   %04Xh     %s |" , Label->name , Label->adr ,(Label->flag==LBL_ADR?"A":" "));
			else fprintf( LstFD ," %-16s:   %04Xh      %s\n" , Label->name , Label->adr,(Label->flag==LBL_ADR?"A":" ") );
			Label = Label->np;
			i++;
		}
		if (i&1) fprintf( LstFD ,"\n");
	}
	/* �A�Z���u�����o�� */
	fprintf( LstFD ,"\n START ADDRESS   = %04Xh\n", StartAdr );
	fprintf( LstFD ," END ADDRESS     = %04Xh\n", AsmAdr-1 );
	fprintf( LstFD ," EXECUTE ADDRESS = %04Xh\n", ExecAdr );

	/* �����P�[�g���t�@�C���쐬�v���L�� */
	if ( OutRealloc ){
		/* �t�@�C���e�ʏo�� */
		fprintf(RocFD,"DW &H%04X\n",OutCnt+n+2 );
		/* ���s�A�h���X���Έʒu�i�w�b�_�擪�j�o�� */
		fseek(RocFD,0,SEEK_SET);
		fprintf(RocFD,"DW &H%04X\n", (ExecAdrFlag ? (unsigned short)(ExecAdr-StartAdr) : 0) );
	}

	/* �o�̓t�@�C���쐬 */
	if ( OutCnt ){
#if __WORD_ADDRESS
		/* �f�[�^�o�͎��́A�I���A�h���X�����ɖ߂� */
		if (Wadr) AsmAdr *= 2;
#endif
		/* �o�͗p�t�@�C��OPEN */
		if ( ( OutFD = fopen( OutFile ,"w" ) ) <= 0 ){
			printf("File Create Error.\n");
			goto asm_end;
		}
		if ( !ExeName ){
			/* ���s�t�@�C�����쐬 */
			if ( !ExecAdrFlag ) sprintf( out ,".BIN" );
			else sprintf( out ,".EXE" );
			if ((fptr = strrchr(ExeFile, '.' ))) sprintf( fptr ,"%s" ,out );
			else strcat( ExeFile ,out );
		}
		/* �t�@�C���쐬 */
		switch( OutType ){
		/* bas�t�@�C���쐬 */
		case 0:
			line = 999;
#if __FORDOS
			fprintf( OutFD ,"%d DATA %s,&H%X,&H%X,&H%X\n", line++,ExeFile,(unsigned short)StartAdr,(unsigned short)(AsmAdr-1),(unsigned short)ExecAdr );
#else
			fprintf( OutFD ,"%d DATA %s,&H%X,&H%X,&H%X\n", line++,ExeFile,StartAdr,AsmAdr-1,ExecAdr );
#endif
			for ( cnt = 0 , n = 0 , sum = 0 ; cnt < OutCnt ; cnt ++ ){
				/* �s�ԍ��o�� */
				if (!n) fprintf( OutFD ,"%d DATA ", line++ );
				fprintf( OutFD ,"%02X", OutBuf[cnt] );
				sum += OutBuf[cnt];
				n++;
				/* �`�F�b�N�T��/���s�o�� */
				if ( n >= 8 ){
					fprintf( OutFD ,",%02X\n", sum&0xff );
					n = 0;
					sum = 0;
				}
			}
			/* �`�F�b�N�T���o�� */
			if (n) fprintf( OutFD ,",%02X\n", sum&0xff );
			break;
		/* pbf�t�@�C���쐬�v������ */
		case 1:
#if __FORDOS
			fprintf( OutFD ,"%s,%u,%u,%u\n", ExeFile,(unsigned short)StartAdr,(unsigned short)(AsmAdr-1),(unsigned short)ExecAdr );
#else
			fprintf( OutFD ,"%s,%u,%u,%u\n", ExeFile,StartAdr,AsmAdr-1,ExecAdr );
#endif
			for ( cnt = 0 , n = 0 , sum = 0 ; cnt < OutCnt ; cnt ++ ){
				fprintf( OutFD ,"%02X", OutBuf[cnt] );
				sum += OutBuf[cnt];
				n++;
				/* �`�F�b�N�T��/���s�o�� */
				if ( n >= 120 ){
					fprintf( OutFD ,",%u\n", sum );
					n = 0;
					sum = 0;
				}
			}
			/* �`�F�b�N�T���o�� */
			if (n) fprintf( OutFD ,",%u\n", sum );
			break;
		/* �N�C�b�N���[�_�t�@�C���쐬 */
		case 2:
			line = 1000;
#if __FORDOS
			fprintf( OutFD ,"%d DATA %u,%u,%u\n", line++,(unsigned short)StartAdr,(unsigned short)(AsmAdr-1),(unsigned short)ExecAdr );
#else
			fprintf( OutFD ,"%d DATA %u,%u,%u\n", line++,StartAdr,AsmAdr-1,ExecAdr );
#endif
			for ( cnt = 0 , n = 0 ; cnt < OutCnt ; cnt ++ ){
				/* �s�ԍ��o�� */
				if (!n) fprintf( OutFD ,"%d DATA ", line++ );
				fprintf( OutFD ,"%02X", OutBuf[cnt] );
				n++;
				if ( n >= 24 ){
					fprintf( OutFD ,"\n" );
					n = 0;
					continue;
				}
				/* �J���}�o�� */
				if ( !( n % 6 ) ) fprintf( OutFD ,"," );
			}
			/* �c���0�o�� */
			while( n && ( n < 24 ) ){
				fprintf( OutFD ,"00" );
				n++;
				if ( n == 24 ){
					fprintf( OutFD ,"\n" );
					break;
				}
				/* �J���}�o�� */
				if ( !( n % 6 ) ) fprintf( OutFD ,"," );
			}
			break;
#if __DLP_INLINE
		/* DL-Pascal inline() format �o�� */
		case 3:
			/* �w�b�_�o�� */
			fprintf(OutFD,"inline(\n");
			for ( cnt = 0 , n = 0 ; cnt < OutCnt ; cnt ++ ){
				if ( n == 0 ) {
				    fprintf( OutFD, "\t");
				}
				fprintf( OutFD ,"$%02X", OutBuf[cnt] );
				/* �ŏI�f�[�^�̓J���}��t���Ȃ� */
				if( cnt < ( OutCnt - 1 ) ){
					fprintf( OutFD, ",");
				}
				n++;
				if ( n == 16 || cnt == ( OutCnt - 1 ) ){
					fprintf( OutFD ,"\n" );
					n = 0;
				}
			}
			fprintf(OutFD,");\n");
			break;
#endif
		}
#if __WITH_EOF
 #if __DLP_INLINE
		/* inline�`���o�͎���EOF��t�����Ȃ� */
		if( OutType != 3 )
 #endif
			/* EOF�o�� */
			fprintf( OutFD ,"%c",0x1a);
#endif
		/* �o�̓t�@�C���N���[�Y */
		fclose(OutFD);
	}

asm_end:
	/* �����P�[�g���t�@�C���쐬�v���L�� */
	if ( OutRealloc ){
		if (RocFD) fclose(RocFD);
	}
	/* ���x���e�[�u����������� */
	Label = LabelTbl;
	while ( Label ){
		Labelwk = Label->np;
		free( Label );
		Label = Labelwk;
	}
	/* �o�̓o�b�t�@��������� */
	if ( OutBuf ) free( OutBuf );
	/* INCLUDE�t�@�C���N���[�Y */
	if ( IncLevel ){
		for ( i = 0 ; i < IncLevel ;i++){
			fclose( IncFD[i] );
		}
	}
	/* �t�@�C���N���[�Y */
	if (SrcFD) fclose(SrcFD);
	if (LstFD) fclose(LstFD);

	/* �t�@�C���ŏI(EOFERR)�͐��튮���Ƃ��� */
	return (rc == EOFERR) ? NORM : rc;
}
/**********************************************************************/
/*   AsmLine : Assembler Process ( one line )                         */
/*                                                                    */
/*   ����    : �\�[�X�s���P�s�A�Z���u�����A���ʂ�OutTbl�Ɋi�[����     */
/*   ����    : �Ȃ�                                                   */
/*   �o��    : �G���[���i0:����A0�ȊO:�ُ�j                       */
/*                                                                    */
/**********************************************************************/
int AsmLine( void )
{
FILE	*SourceFD;/* �\�[�X�t�@�C���f�B�X�N���v�^ */
char Work[MAXLINE+2];/* �s�f�[�^�擾�p���[�N */
int  rc,i,n,cnt,len,opr,kc;
unsigned short	op,opdat,sts;
unsigned short	si = 0;

	/* �G���[�X�e�[�^�X������ */
	rc = 0;

	/* �R�[�h�ϊ��o�b�t�@������ */
	memset( &OutTbl.kind,0, sizeof(OUTTBL));
	OutTbl.kind = UNDEFOPR;
	
	/* �s���[�N�o�b�t�@������ */
	memset( Work , 0 , sizeof(Work) );

	/* ���݂̃A�Z���u���A�h���X��o�^���� */
	OutTbl.adr = (unsigned short)AsmAdr;

	/* �t�@�C���؂�ւ� */
	SourceFD = !IncLevel ? SrcFD : IncFD[IncLevel-1];

	/* �\�[�X�t�@�C������P�s���o�� */
	rc = GetLine( SourceFD , Work );

	/* �C���N���[�h�t�@�C���I�� */
	if ( rc == EOFERR && IncLevel ){
		IncLevel--;
		fclose( IncFD[IncLevel] );
		IncFD[IncLevel] = 0;
		return NORM;
	}

	/* �G���[���󔒍s�ł���ꍇ�A�I�� */
	if ( ( rc && ( rc!=EOFERR ) ) || !Work[0]) return rc;

	/* ���x��/�j���j�b�N/�I�y�����h/�R�����g�ɕ��� */
	if( rc = GetParam( Work ) ) return rc;

	/******************************************/
	/* #if�`#else�`#endif�}�N������           */
	/******************************************/
	/* �j���j�b�N�o�^���� */
	if ( OutTbl.opr[0] ){
		/* �^�����ߎ�ʃ`�F�b�N */
		ChgCode( oprwk , OutTbl.opr[0] );
		opr = GetMacKind( oprwk );
		switch( opr ){
		case OP_IF:
			/* ���x�����I�y�����h2�G���g������ */
			if ( OutTbl.label || OutTbl.opr[2] ) return ILLOPR;
			/* ��1�I�y�����h�����ł��� */
			if( rc = GetCalcData( OutTbl.opr[1] , &op , &sts ) ) return rc;
			/* ���̂��������ȏꍇ�A�����I������ */
			if ( op == LBLNG ) return LBLNOENT;
			/* ���l�X�g���x����AsmFlag��ۑ����� */
			IfStk[IfLevel] = AsmFlag;
			/* �A�Z���u������Ԃł��� */
			if (!AsmFlag){
				/* �A�Z���u���֎~/���Z�b�g */
				AsmFlag = (unsigned char)(sts ? 0 : (!AsmFlag ? 1 : 2 ));
			}
			/* #if�l�X�g���x��+1 */
			IfLevel++;
			/* �l�X�g���x���I�[�o�[ */
			if (IfLevel>=IFLEVEL) return IFNEST;
			return NORM;
		case OP_ELSE:
			/* ���x�����I�y�����h�G���g������ */
			if ( OutTbl.label || OutTbl.opr[1] ) return ILLOPR;
			if ( !IfLevel ) return IFNEST;
			/* ��ʃl�X�g�̓A�Z���u������ */
			if ( !IfStk[IfLevel-1] ){
				/* �A�Z���u����Ԕ��] */
				AsmFlag = (unsigned char)(AsmFlag ? 0 : 1);
			}
			return NORM;
		case OP_ENDIF:
			/* ���x�����I�y�����h�G���g������ */
			if ( OutTbl.label || OutTbl.opr[1] ) return ILLOPR;
			if ( !IfLevel ) return IFNEST;
			/* #if�l�X�g���x��-1 */
			IfLevel--;
			if ( IfLevel < 0 ) return IFNEST;
			/* �A�Z���u���ĊJ */
			AsmFlag = IfStk[IfLevel];
			return NORM;
		default:
			break;
		}
	}
	/* �A�Z���u���֎~�Ȃ珈���I�� */
	if ( AsmFlag ) return NORM;

	/******************************************/
	/* �V���{�����x���o�^����                 */
	/******************************************/
	/* ���x������ */
	if ( OutTbl.label ){
		/* �P�p�X�ڂł��� */
		if (!pass){
			/* ���x��������`�F�b�N */
			if ( rc = CheckLabel(OutTbl.label) ) return rc;
			/* EQU�v���ł��� */
			ChgCode( oprwk , OutTbl.opr[0] );
			if (!strcmp( "EQU" , oprwk )){
				/* �I�y�����h�G���g��������ł��� */
				if ( !OutTbl.opr[1] || OutTbl.opr[2] ) return ILLOPR;
				/* ���x����ʂ𐔒l(EQU)�w��Ƃ��� */
				LblFlg = LBL_EQU;
				/* ��1�I�y�����h�����l�ł��� */
				if( !(rc = GetCalcData( OutTbl.opr[1] , &op , &sts )) ){
					/* ���̂��������ȏꍇ�A�V���{���o�^�����Ȃ� */
					if ( op == LBLNG ) return NORM;
					/* SET�I�v�V�������s(�m��l�̂݃V���{���o�^����) */
					if ( SetLabel ){
						/* ���x���́A/SET���̂ƈ�v */
						if ( (rc = CheckSetLbl( OutTbl.label, sts )) == 1 ) return NORM;
						/* ���x���o�^�G���[���� */
						if ( rc ) return rc;
					}
					/* ���x���e�[�u���ɓo�^���� */
					rc = SetLabelTbl( OutTbl.label, sts ,LblFlg );
				}
				/* �s�����I�� */
				return rc;
			}
			/* EQU�ȊO�ł��� */
			else{
				if ( !StartAdrFlag ) { rc = NOORG; return rc; }/* ORG�Ȃ� */
				/* ���݂̃A�Z���u���A�h���X�����x���e�[�u���ɓo�^���� */
				if(rc = SetLabelTbl( OutTbl.label, (unsigned short)AsmAdr ,LBL_ADR )) return rc;
			}
		}
		/* �Q�p�X�ڂł��� */
		else{
			/* EQU�v���ł��� */
			ChgCode( oprwk , OutTbl.opr[0] );
			if (!strcmp( "EQU" , oprwk )){
				/* ���ɓo�^�ς݂Ȃ甲���� */
				if ( GetLabelAdr( OutTbl.label , &sts ) != LBLNOENT ) return NORM;
				/* ���x����ʂ𐔒l(EQU)�w��Ƃ��� */
				LblFlg = LBL_EQU;
				/* ��1�I�y�����h�����l�ł��� */
				if( !(rc = GetCalcData( OutTbl.opr[1] , &op , &sts )) ){
					/* ���̂��������ȏꍇ�A�����I������ */
					if ( op == LBLNG ) return LBLNOENT;
					/* ���x���e�[�u���ɓo�^���� */
					if ( (rc = SetLabelTbl( OutTbl.label, sts ,LblFlg )) ) return ILLLBL;
				}
				/* �s�����I�� */
				return rc;
			}
		}
	}
	/******************************************/
	/* �^������/��ʖ��ߏ���                  */
	/******************************************/
	/* �j���j�b�N�o�^���� */
	if ( OutTbl.opr[0] ){
		/* �ʏ햽�߁^�^�����ߎ�ʃ`�F�b�N */
		ChgCode( oprwk , OutTbl.opr[0] );
		OutTbl.kind = GetMnemonicKind( oprwk );
		switch( OutTbl.kind ){
		case OP_EQU:
			/* ���x���Ȃ�EQU����(�G���[�I��) */
			if ( !pass ) rc = EQUNOLBL;
			break;
		case OP_ORG:
			/* ORG���� */
			/* ��Q�I�y�����h�ȍ~�ɓo�^������ꍇ�A�G���[�I������ */
			if ( OutTbl.opr[2] ){
				rc = ILLOPR;
				break;
			}
			/* �I�y�����h�G���g������ */
			if ( !OutTbl.opr[1] ) return ILLOPR;
			/* ��P�I�y�����h�����l�ł��� */
			if( !(rc = GetCalcData( OutTbl.opr[1] , &op , &sts )) ){
				/* ���̂��������ȏꍇ�A�����I������ */
				if ( op == LBLNG ){ rc = LBLNOENT; break; }
				if ( sts < (unsigned short)AsmAdr ){
					rc = OFLOW;
					break;
				}
				/* ���݂̃A�Z���u���A�h���X���X�V���� */
				if ( !StartAdrFlag ){
					StartAdr = sts;
					StartAdrFlag = 1;
				}
				else{
					/* �Q��ڈȍ~�́A�w��o�C�g�������m�ۂ��� */
					OutTbl.byte = (unsigned short)( sts - (unsigned short)AsmAdr );
				}
				OutTbl.adr = sts;
				AsmAdr = sts;
			}
			break;
		case OP_START:
			/* START���� */
			/* ��Q�I�y�����h�ȍ~�ɓo�^������ꍇ�A�G���[�I������ */
			if ( OutTbl.opr[2] ){
				rc = ILLOPR;
				break;
			}
			/* ����ExecAdr�G���g���[���� */
			if ( ExecAdrFlag ){
				rc = ILLSTART;
				break;
			}
			/* �I�y�����h�G���g������ */
			if ( !OutTbl.opr[1] ) return ILLOPR;
			/* ��P�I�y�����h�����l�ł��� */
			if( !(rc = GetCalcData( OutTbl.opr[1] , &op , &sts )) ){
				/* ���̉������Ă���ꍇ�A�A�h���X��o�^���� */
				if ( op == LBLOK ){
					/* �A�h���X��o�^���� */
					ExecAdr = sts;
					ExecAdrFlag = 1;
				}
				/* ���̖������ł���2�p�X�ڂȂ�G���[�I������ */
				else if (pass){
					rc = LBLNOENT;
				}
			}
			break;
		case OP_DS:
			/* DS���� */
			if ( !StartAdrFlag ) { rc = NOORG; break; }/* ORG�Ȃ� */
			/* ��Q�I�y�����h�ȍ~�ɓo�^������ꍇ�A�G���[�I������ */
			if ( OutTbl.opr[2] ){
				rc = ILLOPR;
				break;
			}
			/* ��P�I�y�����h�����l�ł��� */
			if( !(rc = GetCalcData( OutTbl.opr[1] , &op , &sts )) ){
				/* ���̂��������ȏꍇ�A�����I������ */
				if ( op == LBLNG ){ rc = LBLNOENT; break; }
				/* �w��o�C�g�������m�� */
				OutTbl.byte = sts;
				/* �A�h���X���X�V���� */
				if (( AsmAdr + sts ) <= MAXOBJ ) AsmAdr += sts;
				else rc = ADOFLOW;
				break;
			}
			rc = ILLOPR;
			break;
		case OP_DB:
			/* DB���� */
			if ( !StartAdrFlag ) { rc = NOORG; break; }/* ORG�Ȃ� */
			/* ��P�I�y�����h�ȍ~�ɓo�^���Ȃ��ꍇ�A�G���[�I������ */
			if ( !OutTbl.opr[1] ){
				rc = ILLOPR;
				break;
			}
			cnt = 0;
			i = 0;
			n = 0;
			/* �I�y�����h�G���g�����������o�^���� */
			while( OutTbl.opr[cnt+1] ){
				/* ����ȊO�́A�J���}�̃`�F�b�N������ */
				if ( cnt ){
					/* �擪���J���}�ȊO�ł���ꍇ�A�G���[�I�� */
					if ( *OutTbl.opr[cnt+1] != ',' ){
						rc = ILLCANMA;
						break;
					}
					/* �擪�������X�L�b�v */
					i = 1;
				}
				/* ������ł��� */
				len = strlen( OutTbl.opr[cnt+1]+i );
				/* �擪����эŏI�̓R�[�e�[�V����( = ������G���g��)�ł��� */
				if ( ((*(OutTbl.opr[cnt+1]+i) == 0x22 )&&(*(OutTbl.opr[cnt+1]+i+len-1) == 0x22 ))
					||((*(OutTbl.opr[cnt+1]+i) == 0x27 )&&(*(OutTbl.opr[cnt+1]+i+len-1) == 0x27 )) ){
					/* �����񂪂R�ȉ��Ȃ�I�y�����h�ُ� */
					if( len < 3 ) {
						rc = ILLOPR;
						break;
					}
					/* �R�[�h�o�^���� */
					memcpy( &OutTbl.code[n] , OutTbl.opr[cnt+1]+i+1 , len-2 );
					n += ( len - 2 );
					rc = NORM;
				}
				else{
					/* �v�Z���Ƃ��ď������� */
					if ( !(rc = GetCalcData( OutTbl.opr[cnt+1]+i , &op , &sts )) ) {
						/* �͈͊O�Ȃ�G���[ */
						if ( sts >= 256 ){
							rc = OFLOW;
							break;
						}
						/* �R�[�h�o�^���� */
						OutTbl.code[n++] = sts;
					}
					else break;
				}
				OutTbl.byte = (unsigned short)n;
				cnt++;
			}
			/* �A�h���X���X�V���� */
			if (( AsmAdr + n ) <= MAXOBJ ) AsmAdr += n;
			else rc = ADOFLOW;
			break;
		case OP_DW:
			/* DW���� */
			if ( !StartAdrFlag ) { rc = NOORG; break; }/* ORG�Ȃ� */
			/* ��P�I�y�����h�ȍ~�ɓo�^���Ȃ��ꍇ�A�G���[�I������ */
			if ( !OutTbl.opr[1] ){
				rc = ILLOPR;
				break;
			}
			cnt = 0;
			i = 0;
			n = 0;
			/* �I�y�����h�G���g�����������o�^���� */
			while( OutTbl.opr[cnt+1] ){
				/* ����ȊO�́A�J���}�̃`�F�b�N������ */
				if ( cnt ){
					/* �擪���J���}�ȊO�ł���ꍇ�A�G���[�I�� */
					if ( *OutTbl.opr[cnt+1] != ',' ){
						rc = ILLCANMA;
						break;
					}
					/* �擪�������X�L�b�v */
					i = 1;
				}
				/* �v�Z���Ƃ��ď������� */
				if ( rc = GetCalcData( OutTbl.opr[cnt+1]+i , &op , &sts ) ) break;
				/* ���[�h�P�ʂŃR�[�h�o�^���� */
				OutTbl.code[n++] = (unsigned char)(sts & 0xff);
				OutTbl.code[n++] = (unsigned char)((sts>>8) & 0xff);
				OutTbl.byte = (unsigned short)n;
				cnt++;
			}
			/* �A�h���X���X�V���� */
			if (( AsmAdr + n ) <= MAXOBJ ) AsmAdr += n;
			else rc = ADOFLOW;
			break;
		case OP_LEVEL:
			/* ��Q�I�y�����h�ȍ~�ɓo�^������ꍇ�A�G���[�I������ */
			if ( OutTbl.opr[2] ){
				rc = ILLOPR;
				break;
			}
			/* ��P�I�y�����h�����l�ł��� */
			if ( !(rc = GetCalcData( OutTbl.opr[1] ,&op ,&sts )) ){
				/* ���̂��������ȏꍇ�A�����I������ */
				if ( op == LBLNG ){ rc = LBLNOENT; break; }
				/* PB-1000�݊��R�[�h�؂�ւ� */
				UndefOpr = (unsigned short)( sts ? 1 : 0 );
			}
			break;
		case OP_INCLUDE:
			/* ���ɃC���N���[�h�t�@�C��OPEN�� */
			if ( IncLevel >= (MAXINC-1) ){ rc = INCERR; break; }
			/* �I�y�����h�G���g������ */
			if ( !OutTbl.opr[1] ){ rc = ILLOPR; break; }
			if ( (len = strlen(OutTbl.opr[1])) < 3 ){ rc = INCNOFILE; break; }
			/* �擪����эŏI�͊��ʂł��� */
			if ( ( OutTbl.opr[1][0] == '(' ) && ( OutTbl.opr[1][len-1] == ')' ) ){
				/* �C���N���[�h�t�@�C�������o�� */
				memset( IncFile , 0 , sizeof(IncFile) );
				memcpy( IncFile , &OutTbl.opr[1][1] , len-2 );
				/* �C���N���[�h�t�@�C��OPEN */
				if ( ( IncFD[IncLevel] = fopen( IncFile ,"rb" ) ) <= 0 ){ rc = INCNOFILE; break; }
				/* �s�ԍ��N���A(���̏����I����+1�����̂�-1���Z�b�g) */
				IncLine[IncLevel] = -1;
				/* �C���N���[�hLEVEL�X�V */
				IncLevel++;
				rc = NORM;
			}
			else rc = ILLOPR;
			break;
		case OP_LIST:
			/* �I�y�����h�G���g������ */
			if ( OutTbl.opr[1] ) rc = ILLOPR;
			ListFlag = 0;
			break;
		case OP_NOLIST:
			/* �I�y�����h�G���g������ */
			if ( OutTbl.opr[1] ) rc = ILLOPR;
			/* ���X�g�o�͋֎~ */
			ListFlag = 1;
			break;
		case OP_EJECT:
			/* �I�y�����h�G���g������ */
			if ( OutTbl.opr[1] ) rc = ILLOPR;
			/* ���X�gLineFeed�}���v���Z�b�g */
			LineFeed = 1;
			break;
		case OP_INCBIN:
			/* �I�y�����h�G���g������ */
			if ( !OutTbl.opr[1] ){ rc = ILLOPR; break; }
			if ( (len = strlen(OutTbl.opr[1])) < 3 ){ rc = INCNOFILE; break; }
			/* �擪����эŏI�͊��ʂł��� */
			if ( ( OutTbl.opr[1][0] == '(' ) && ( OutTbl.opr[1][len-1] == ')' ) ){
				/* �o�C�i���t�@�C�������o�� */
				memset( BinFile , 0 , sizeof(BinFile) );
				memcpy( BinFile , &OutTbl.opr[1][1] , len-2 );
				/* �o�C�i���t�@�C���ǂݏo�� */
				rc = IncludeBin( &OutTbl.byte , BinFile );
				/* �A�h���X�X�V */
				AsmAdr += OutTbl.byte;
			}
			else rc = ILLOPR;
			break;
		case OP_AI:
			/* �I�y�����h�G���g������ */
			if ( OutTbl.opr[1] ) rc = ILLOPR;
			InstMode = 0;
			break;
		case OP_KC:
			/* �I�y�����h�G���g������ */
			if ( OutTbl.opr[1] ) rc = ILLOPR;
			InstMode = 1;
			break;
#if __EUR_MNEMONIC
		case OP_EU:
			/* �I�y�����h�G���g������ */
			if ( OutTbl.opr[1] ) rc = ILLOPR;
			InstMode = 2;
			break;
#endif /* __EUR_MNEMONIC */
		default:
			/* ��ʖ��ߏ��� */
			/* �j���j�b�N�������͐��� */
			if ( strlen( OutTbl.opr[0] ) >= MAXMN ) { rc = UNDEFOPR ; break; }
			/* ORG�Ȃ� */
			if ( !StartAdrFlag ) { rc = NOORG; break; }
			/* ��5�I�y�����h�ȍ~�ɓo�^������ꍇ�A�G���[�I�� */
			if ( OutTbl.opr[5] ){
				rc = ILLOPR;
				break;
			}
			/* �I�y�����h1�`5����͂��� */
			for ( i = 1 ; i < 5 ; i++ ){
				kc = 0;
				/* �I�y�����h�o�^�Ȃ��Ȃ烋�[�v�𔲂��� */
				if (!OutTbl.opr[i]) break;
				/* ��Q�I�y�����h�ȍ~�Ȃ�A�擪���J���} */
				if ( ( i >= 2 ) && *OutTbl.opr[i] &&( *OutTbl.opr[i] != ',' ) ){
					/* KC�����ł͖����������[(]�ȊO�ł��� */
					if( !OutTbl.kc || *OutTbl.opr[i] != '(' ){
						rc = ILLCANMA;
						break;
					}
					kc = 1;
				}
				/* �I�y�����h��ʎ擾 */
				if ( rc = GetOprKind( i, OutTbl.opr[i]+( (i==1)||kc ? 0 : 1 ) , &op , &opdat ,&si ) ){
					if ( rc == NOENT ) rc = 0;
					break;
				}
				/* �A�h���X�^���l �������Ȃ�G���[�I��(�Q�p�X��) */
				if ( pass && (( op & LBLMASK ) == LBLNG ) ){
					rc = LBLNOENT;
					break;
				}
				/* OPR2�ł��A����C���f�b�N�X�w�肠�� */
				if ( ( i == 2 ) && si ){
					/* OPR1�́A$31�A$30�A$0�w��ł��� */
					switch ( OutTbl.opkind[0] ){
					case RSX:
					case RSY:
					case RSZ:
						/* �ʏ탌�W�X�^�w��ɕύX���� */
						OutTbl.opkind[0] = REG;
						break;
					case MRSX:
					case MRSY:
					case MRSZ:
						/* �ʏ탌�W�X�^�w��ɕύX���� */
						OutTbl.opkind[0] = MREG;
						break;
					default:
						break;
					}
				}
				/* ����C���f�b�N�X�w��t���O�Z�b�g */
				if ( !OutTbl.si && si ){
					OutTbl.si = si;
				}
				/* �I�y�����h��ʁ^�f�[�^��ۑ����� */
				OutTbl.opkind[i-1] = op;
				OutTbl.opdata[i-1] = opdat;
#if __DEBUG
				printf("kind=%x data=%x\n",op,opdat);
#endif
			}
			/* �I�y�����h��ʃ`�F�b�N�ɂăG���[���� */
			if (rc) break;
			/* ����C���f�b�N�X�w�肠�� */
			if( OutTbl.si ){
				/* �j���j�b�N�{��P�`��S�I�y�����h�Ō���(OP1/OP2�Ƃ�R0,R30,R31�L��) */
				rc = SearchOpr( MASKOP2 , MASKOP2 , MASKOP , MASKOP );
				break;
			}
			/* �Z�J���h�I�y���[�V�����g���I�v�V�����iCASIO�����J�j�I�� */
			if( UndefOpr ){
				/* �j���j�b�N�{��P�`��S�I�y�����h�Ō���(OP1/OP2�Ƃ�R0,R30,R31�L��) */
				if (!(rc=SearchOpr( MASKOP2 , MASKOP2 , MASKOP , MASKOP ))) break;
				/* �j���j�b�N�{��P�`��S�I�y�����h�Ō���(OP2�̂�R0,R30,R31�L��) */
				if (!(rc=SearchOpr( MASKOP , MASKOP2 , MASKOP , MASKOP ))) break;
				/* �j���j�b�N�{��P�`��S�I�y�����h�Ō���(OP1�̂�R0,R30,R31�L��) */
				if (!(rc=SearchOpr( MASKOP2 , MASKOP , MASKOP , MASKOP ))) break;
			}
			/* �j���j�b�N�{��P�`��R�I�y�����h��� �ɂČ���(R0,R30,R31�w��Ȃ�) */
			rc = SearchOpr( MASKOP , MASKOP , MASKOP , MASKOP );
			break;
		}
	}
	return rc;
}

/**********************************************************************/
/*   SearchOpr : Search Operation Code                                */
/*                                                                    */
/*   ����    : �w�肳�ꂽ�I�y�����h������OutBuf�̖��߂���������       */
/*             �������ʂ́AOutBuf�Ɋi�[�����                         */
/*   ����    : �I�y�����h��������(op1,op2,op3,op4)                    */
/*   �o��    : �G���[���i0:�Y�����߂���A0�ȊO:�Ȃ��j               */
/*                                                                    */
/**********************************************************************/
int SearchOpr( unsigned short op1 ,unsigned short op2 ,unsigned short op3 ,unsigned short op4 )
{
unsigned short i;
int def_opr = 0;
#if __DEBUG
	printf("opr = %s\n",oprwk);
#endif
	/* �����Ώۃj���j�b�N��HD61�`���ɕϊ����� */
	if ( ChgKcName( oprwk , OutTbl.opr[0] ) ) return UNDEFOPR;
#if __EUR_MNEMONIC
	/* #EU /EU�w��L�� */
	if ( InstMode == 2 ){
		/* EU(Europe)�`���j���j�b�N��HD61�`���ɂ��� */
		if ( ChgEuropeName( oprwk , OutTbl.opr[0] ) ) return UNDEFOPR;
	}
#endif /* __EUR_MNEMONIC */
	/* �j���j�b�N�{��P�`��R�I�y�����h��� �ɂČ��� */
	for ( i = 0 ; i < MAXOP ; i++ ){
		/* �Y�����߂��� */
		if ( !strcmp( codetbl[i].name , oprwk ) ){
			/* �j���j�b�N�͈�v */
			def_opr = 1;
			/* �I�y�����h��ʂ���v */
			if ( ((codetbl[i].op1&MASKOP2) == (OutTbl.opkind[0]&op1) )
				&& ((codetbl[i].op2&MASKOP2) == (OutTbl.opkind[1]&op2) )
				&& ((codetbl[i].op3&MASKOP)  == (OutTbl.opkind[2]&op3) )
				&& ((codetbl[i].op4&MASKOP)  == (OutTbl.opkind[3]&op4) )){
				/* �R�[�h�e�[�u���̃C���f�b�N�X���i�[ */
				OutTbl.idx = i;
				OutTbl.byte = codetbl[i].byte;
				/* �A�Z���u���A�h���X�X�V */
				if (( AsmAdr + codetbl[i].byte ) <= MAXOBJ ){
					AsmAdr += codetbl[i].byte;
					return NORM;
				}
				else return ADOFLOW;
			}
		}
	}
	/* �I�y�����h��ʕs��v/�Y�����߂Ȃ� */
	return ( def_opr ? ILLOPR : UNDEFOPR );
}
/**********************************************************************/
/*   AsmCodeSet : Assembler Process ( 2 pass )                        */
/*                                                                    */
/*   ����    : �P�s���̃A�Z���u�����ʂ��R�[�h������i�Q�p�X��)        */
/*   ����    : �Ȃ�                                                   */
/*   �o��    : �G���[���i0:����A0�ȊO:�ُ�j                       */
/*                                                                    */
/**********************************************************************/
int AsmCodeSet( void )
{
int  rc,n;
unsigned short op,opdat,sts,adr;

	/* �G���[�X�e�[�^�X������ */
	rc = 0;

	/* �A�Z���u���A�h���X�X�V */
	adr = OutTbl.adr;

	/******************************************/
	/* #if�`#else�`#endif�}�N������           */
	/******************************************/
	/* �j���j�b�N�o�^���� */
	if ( OutTbl.opr[0] ){
		/* #if,#else,#endif�`�F�b�N */
		ChgCode( oprwk , OutTbl.opr[0] );
		sts = (unsigned short)GetMacKind( oprwk );
		switch( sts ){
		case OP_IF:
		case OP_ELSE:
		case OP_ENDIF:
			return NORM;
		default:
			break;
		}
	}
	/******************************************/
	/* �^������/��ʖ��ߏ���                  */
	/******************************************/
	/* �j���j�b�N�o�^���� */
	if ( OutTbl.opr[0] ){
		/* �ʏ햽�߁^�^�����ߎ�ʃ`�F�b�N */
		switch( OutTbl.kind ){
		case OP_ORG:
		case OP_EQU:
		case OP_DS:
		case OP_DB:
		case OP_DW:
		case OP_INCLUDE:
		case OP_LIST:
		case OP_NOLIST:
		case OP_EJECT:
		case OP_INCBIN:
			break;
		case OP_START:
			/* START���� */
			/* ���s�J�n�A�h���X���擪�A�h���X��菬�����ꍇ�̓G���[�I�� */
			if ( ExecAdrFlag && ( StartAdr > ExecAdr ) ) return EOFLOW;
			break;
		default:
			/* ��ʖ��ߓo�^���� */
			if ( OutTbl.byte ){
				/* JR���ߗp�I�y�����h�ԍ������� */
				n = 1;
				/* ���߃R�[�h�o�� */
				OutTbl.code[0] = codetbl[OutTbl.idx].code;
				/* ���ߎ�����ʂɂĕ��� */
				switch( codetbl[OutTbl.idx].kind ){
				case NU:	/* �I�y�����h�Ȃ��^1 byte ����  */
					break;
				case IM8_NU:/* IM8        (�Q�o�C�g���߁j   */
					/* �I�y�����h�`�F�b�N */
					if (OutTbl.opdata[1] >= IM8bit ){ rc = OFLOW ; break; }
				case REG_NU:/* REG        (�Q�o�C�g���߁j   */
				case REG_NUJ:/* REG : IM7    (3�o�C�g���߁j   */
					/* �I�y�����h�R�[�h�o�� */
					OutTbl.code[1] = codetbl[OutTbl.idx].secop |(unsigned char)OutTbl.opdata[0];
					/* Jump�g������ */
					if ( codetbl[OutTbl.idx].kind == REG_NUJ ){
						/* ���΃W�����v�A�h���X�v�Z */
						if(rc = GetJumpData( OutTbl.opdata[1] , adr ,OutTbl.byte ,&op , &opdat )) break;
						OutTbl.code[2] = (unsigned char)( op | opdat );
#if __WORD_ADDRESS
						/* �I�y�����h�R�[�h�o�� */
						if (Wadr){
							OutTbl.code[2] = 0;
							OutTbl.code[3] = (unsigned char)( op | opdat );
						}
#endif
					}
					break;
				case IR_REG:/* IREG�FREG  (�Q�o�C�g)        */
				case IR_REGJ:/* IREG�FREG :IM7 (3�o�C�g)     */
					/* �I�y�����h�R�[�h�o�� */
					OutTbl.code[1] = codetbl[OutTbl.idx].secop |(unsigned char)OutTbl.opdata[1];
					/* Jump�g������ */
					if ( codetbl[OutTbl.idx].kind == IR_REGJ ){
						/* ���΃W�����v�A�h���X�v�Z */
						if(rc = GetJumpData( OutTbl.opdata[2] , adr ,OutTbl.byte ,&op , &opdat )) break;
						OutTbl.code[2] = (unsigned char)( op | opdat );
#if __WORD_ADDRESS
						/* �I�y�����h�R�[�h�o�� */
						if (Wadr){
							OutTbl.code[2] = 0;
							OutTbl.code[3] = (unsigned char)( op | opdat );
						}
#endif
					}
					break;
				case IR_IM5:/* IREG�FIM5  (�Q�o�C�g)        */
					/* �I�y�����h�`�F�b�N */
					if (OutTbl.opdata[1] >= IM5bit ){ rc = OFLOW ; break; }
					/* �I�y�����h�R�[�h�o�� */
					OutTbl.code[1] = codetbl[OutTbl.idx].secop |(unsigned char)OutTbl.opdata[1];
					break;
				case IR_IM8:/* IREG�FIM8  (�R�o�C�g)        */
					/* �I�y�����h�`�F�b�N */
					if (OutTbl.opdata[1] >= IM8bit ){ rc = OFLOW ; break; }
					/* �I�y�����h�R�[�h�o�� */
					OutTbl.code[1] = codetbl[OutTbl.idx].secop;
					OutTbl.code[2] = (unsigned char)OutTbl.opdata[1];
					break;
				case IR_RIM3:/* IREG: REG: IM3  (�R�o�C�g)        */
					/* �I�y�����h�`�F�b�N */
					if ((OutTbl.opdata[2] <= 1 )||(OutTbl.opdata[2] > IM3bit )){ rc = OFLOW ; break; }
					/* �I�y�����h�R�[�h�o�� */
					OutTbl.code[1] = codetbl[OutTbl.idx].secop|(unsigned char)OutTbl.opdata[1];
					OutTbl.code[2] = (unsigned char)((OutTbl.opdata[2]-1)<<5);
					break;
				case REG_IM3:/* REG : IM3  (�R�o�C�g)        */
					/* �I�y�����h�`�F�b�N */
					if ((OutTbl.opdata[1] <= 1 )||(OutTbl.opdata[1] > IM3bit )){ rc = OFLOW ; break; }
					/* �I�y�����h�R�[�h�o�� */
					OutTbl.code[1] = codetbl[OutTbl.idx].secop|(unsigned char)OutTbl.opdata[0];
					OutTbl.code[2] = (unsigned char)((OutTbl.opdata[1]-1)<<5);
					break;
				case REG_I5I3:/* REG : IM5 : IM3  (�R�o�C�g)     */
				case REG_I5I3J:/* REG : IM5 : IM3 : IM7 (4�o�C�g) */
					/* �I�y�����h�`�F�b�N */
					if ( OutTbl.opdata[1] > 31 ){ rc = OFLOW ; break; }
				case REG2_IM3:/* REG : REG : IM3  (�R�o�C�g)     */
				case REG2_IM3J:/* REG : REG : IM3 : IM7 (4�o�C�g) */
					/* �I�y�����h�`�F�b�N */
					if ((OutTbl.opdata[2] <= 1 )||(OutTbl.opdata[2] > IM3bit )){ rc = OFLOW ; break; }
					/* �I�y�����h�R�[�h�o�� */
					OutTbl.code[1] = codetbl[OutTbl.idx].secop |(unsigned char)OutTbl.opdata[0];
					/* �I�y�����h�R�[�h�o�� */
					if (( codetbl[OutTbl.idx].kind == REG2_IM3 )||( codetbl[OutTbl.idx].kind == REG2_IM3J ) ){
						opdat = (!OutTbl.opdata[1]||(OutTbl.opdata[1]==31)||(OutTbl.opdata[1]==30)) ? (unsigned short)0 : OutTbl.opdata[1];
						OutTbl.code[2] = (unsigned char)opdat|(unsigned char)((OutTbl.opdata[2]-1)<<5);
					}
					else{
						/* �I�y�����h�R�[�h�o�� */
						OutTbl.code[2] = (unsigned char)OutTbl.opdata[1]|(unsigned char)((OutTbl.opdata[2]-1)<<5);
					}
					/* Jump�g������ */
					if (( codetbl[OutTbl.idx].kind == REG2_IM3J )||( codetbl[OutTbl.idx].kind == REG_I5I3J )){
						/* ���΃W�����v�A�h���X�v�Z */
						if(rc = GetJumpData( OutTbl.opdata[3] , adr ,OutTbl.byte ,&op , &opdat )) break;
						/* �I�y�����h�R�[�h�o�� */
						OutTbl.code[3] = (unsigned char)( op | opdat );
					}
					break;
				case REG_IM7:/* REG �FIM7  (�R�o�C�g)        */
					/* �I�y�����h�R�[�h�o�� */
					OutTbl.code[1] = codetbl[OutTbl.idx].secop |(unsigned char)OutTbl.opdata[0];
					/* ���΃W�����v�A�h���X�v�Z */
					if(rc = GetJumpData( OutTbl.opdata[2] , adr ,OutTbl.byte ,&op , &opdat )) break;
					/* �I�y�����h�R�[�h�o�� */
					OutTbl.code[2] = (unsigned char)( op | opdat );
#if __WORD_ADDRESS
					if (Wadr){
						OutTbl.code[2] = 0;
						OutTbl.code[3] = (unsigned char)( op | opdat );
					}
#endif
					break;
				case REG2_IM7:	/* REG : REG �FIM7  (�S�o�C�g)        */
								/* REG : IM8 �FIM7  (�S�o�C�g)        */
					/* �I�y�����h�`�F�b�N */
					if (OutTbl.opdata[1] >= IM8bit ){ rc = OFLOW ; break; }
					/* �I�y�����h�R�[�h�o�� */
					OutTbl.code[1] = codetbl[OutTbl.idx].secop |(unsigned char)OutTbl.opdata[0];
					OutTbl.code[2] = (unsigned char)OutTbl.opdata[1];
					/* ���΃W�����v�A�h���X�v�Z */
					if(rc = GetJumpData( OutTbl.opdata[2] , adr ,OutTbl.byte ,&op , &opdat )) break;
					/* �I�y�����h�R�[�h�o�� */
					OutTbl.code[3] = (unsigned char)( op | opdat );
					break;
				case REG_REG:/* REG �FREG  (�R�o�C�g)        */
				case REG_IM8:/* REG �FIM8  (�R�o�C�g)        */
					/* �I�y�����h�`�F�b�N */
					if (OutTbl.opdata[1] >= IM8bit ){ rc = OFLOW ; break; }
					/* �I�y�����h�R�[�h�o�� */
					OutTbl.code[1] = codetbl[OutTbl.idx].secop |(unsigned char)OutTbl.opdata[0];
					OutTbl.code[2] = OutTbl.opdata[1];
					break;
				case NU_IM8:/* NU �FIM8  (�R�o�C�g)        */
					/* �I�y�����h�`�F�b�N */
					if (OutTbl.opdata[0] >= IM8bit ){ rc = OFLOW ; break; }
					/* �I�y�����h�R�[�h�o�� */
					OutTbl.code[1] = codetbl[OutTbl.idx].secop;
					OutTbl.code[2] = OutTbl.opdata[0];
					break;
				case IM8_REG:/* IM8 �FREG  (�R�o�C�g)        */
				case MREG_REG:/* MREG �FREG  (�R�o�C�g)        */
					/* �I�y�����h�`�F�b�N */
					if (OutTbl.opdata[0] >= IM8bit ){ rc = OFLOW ; break; }
					/* �I�y�����h�R�[�h�o�� */
					OutTbl.code[1] = codetbl[OutTbl.idx].secop |(unsigned char)OutTbl.opdata[1];
					OutTbl.code[2] = OutTbl.opdata[0];
					break;
				case IM16_REG:/* IM16 : REG (�S�o�C�g) �I�y�R�[�h0xD0����       */
					/* �I�y�����h�R�[�h�o�� */
					OutTbl.code[1] = codetbl[OutTbl.idx].secop;
					OutTbl.code[2] = (unsigned char)( OutTbl.opdata[0] & 0xff );
					OutTbl.code[3] = (unsigned char)( OutTbl.opdata[0] >> 8 );
					break;
				case REG_IM16:/* REG �FIM16 (�S�o�C�g) �I�y�R�[�h0xD1����       */
					/* �I�y�����h�R�[�h�o�� */
					OutTbl.code[1] = codetbl[OutTbl.idx].secop |(unsigned char)OutTbl.opdata[0];
					OutTbl.code[2] = (unsigned char)( OutTbl.opdata[1] & 0xff );
					OutTbl.code[3] = (unsigned char)( OutTbl.opdata[1] >> 8 );
					break;
				case IR_IM16:/* IREG �FIM16 (�S�o�C�g)        */
					/* �I�y�����h�R�[�h�o�� */
					OutTbl.code[1] = codetbl[OutTbl.idx].secop;
					OutTbl.code[2] = (unsigned char)( OutTbl.opdata[1] & 0xff );
					OutTbl.code[3] = (unsigned char)( OutTbl.opdata[1] >> 8 );
					break;
				case IM7_NU:/* ���l(7bit):�Ȃ� JR����       */
					n = 0;
				case F_IM7:	/* �t���O:���l(7bit) JR����     */
					if ( OutTbl.pm7 ){
						if ( OutTbl.opdata[n] >= 128 ) { rc = JOFLOW ; break; }
						/* ���΃W�����v�A�h���X�ݒ�(�}IM7�`��) */
						op = (unsigned short)(( OutTbl.pm7 == 2 ) ? 0x80 : 0 );
						opdat = OutTbl.opdata[n];
					}
					/* ���΃W�����v�A�h���X�v�Z */
					else if(rc = GetJumpData( OutTbl.opdata[n] , adr ,OutTbl.byte ,&op , &opdat )) break;
					/* �I�y�����h�R�[�h�o�� */
					OutTbl.code[1] = (unsigned char)( op | opdat );
					break;
				/* �t���O:���l(16bit)JP/CAL���� */
				case F_IM16:
					/* �I�y�����h�R�[�h�o�� */
					OutTbl.code[1] = (unsigned char)( OutTbl.opdata[1] & 0xff );
					OutTbl.code[2] = (unsigned char)( OutTbl.opdata[1] >> 8 );
#if __WORD_ADDRESS
					if (Wadr){
						OutTbl.code[2] = 0;
						OutTbl.code[3] = (unsigned char)( OutTbl.opdata[1] >> 8 );
					}
#endif
					break;
				/* ���l(16bit):�Ȃ� JP/CAL����  */
				case IM16_NU:
					/* �I�y�����h�R�[�h�o�� */
					OutTbl.code[1] = codetbl[OutTbl.idx].secop |(unsigned char)( OutTbl.opdata[0] & 0xff );
					OutTbl.code[2] = (unsigned char)( OutTbl.opdata[0] >> 8 );
#if __WORD_ADDRESS
					if (Wadr){
						OutTbl.code[2] = 0;
						OutTbl.code[3] = (unsigned char)( OutTbl.opdata[0] >> 8 );
					}
#endif
					break;
				default:
					rc = UNDEFOPR;
					break;
				}
			}
			break;
		}
	}

	return rc;
}
/**********************************************************************/
/*   GetJumpData : Get Jump Address data                              */
/*                                                                    */
/*   ����    : ���΃W�����v���l�f�[�^���擾����                       */
/*   ����    : ������|�C���^�A�f�[�^�|�C���^                         */
/*   �o��    : �G���[���i0:����A0�ȊO:�ُ�j                       */
/*                                                                    */
/**********************************************************************/
int GetJumpData( unsigned short jadr , unsigned short adr  , unsigned short byte ,unsigned short *op ,unsigned short *opdat )
{
#if __WORD_ADDRESS
	if (Wadr) byte /= 2;
#endif
	/* ���΃W�����v�A�h���X�v�Z */
	if ( jadr > adr ){
		/* �{�W�����v */
		*opdat = jadr - ( adr + ( byte - 1 ));
		*op = 0;
	}
	else{
		/* �|�W�����v */
		*opdat = ( adr + ( byte - 1 ) ) - jadr;
		*op = 0x80;
	}
	/* �W�����v�A�h���X�I�[�o�[ */
	if ( *opdat >= IM7bit ) return JOFLOW;

	return NORM;
}
/**********************************************************************/
/*   GetLine : Get Source Line                                        */
/*                                                                    */
/*   ����    : �\�[�X�t�@�C������P�s���̕�������擾����             */
/*   ����    : �\�[�X�t�@�C����                                       */
/*   ����    : ���̓t�@�C���h�c�A�o�̓o�b�t�@�|�C���^                 */
/*   �o��    : �G���[���i0:����A0�ȊO:�ُ�j                       */
/*                                                                    */
/**********************************************************************/
int GetLine( FILE *fd ,char *buff )
{
int	sts;
int	i;
	/* 1�s�I���܂Ŏ��o�� */
	for( i = 0 ; i < MAXLINE ; i++ ){
		/* 1�������o�� */
		sts = fgetc( fd );
		switch ( sts ){
		case '\r':
			break;
		/* �P�s�I��� */
		case '\n':
			return NORM;
		/* �t�@�C���I�� */
		case 0x1a:
		case EOF:
			return EOFERR;
		default:
			*buff++ = ( char )sts;
			break;
		}
	}
	return LOFLOW;
}

/**********************************************************************/
/*   GetParam : Get Parameter String                                  */
/*                                                                    */
/*   ����    : �e�p�����[�^��������擾����                           */
/*   ����    : �s�o�b�t�@�|�C���^                                     */
/*   �o��    : �G���[���i0:����A0�ȊO:�Y��������Ȃ��j             */
/*                                                                    */
/**********************************************************************/
/* 
 �����F�^����ꂽ���[�N�o�b�t�@���̕�������p�����[�^���ɕ�������OutTbl�ɓo�^����B
       �����T�v�͈ȉ��̒ʂ�B
      (1)�s�擪�ɂ���^�u�^�X�y�[�X�͍폜����B
      (2)�ŏ��̕�����ɃR����[:]�����o����ƃ��x���Ƃ��Ċi�[����B
      (3)�j���j�b�N�o�^����[&]�[$]�[(]��^�u��X�y�[�X�����o����Ɖ��s���ăI�y�����h�Ƃ��Ċi�[����B
      (4)������̐擪�ȊO�ɃJ���}[,]�����o����Ɖ��s�i�C���f�b�N�X���X�V�j����B
      (5)������擪�������́A���ʓ�[( �` )]�ɋL�q���ꂽ�^�u�^�X�y�[�X�͋l�߂Ċi�[����B
       �i�^�u�^�X�y�[�X�͊�{�I�Ɋi�[���Ȃ��j
        �������A��3�I�y�����h�ȍ~�AJR�^�O�����o�����ꍇ�ͤ�X�y�[�X��1�����}������B
      (6)�_�u���R�[�e�[�V����["]�����o����ƁA����["]���o�ꂷ�邩�s���܂ł��̂܂܊i�[����B
      (7)�Z�~�R����[;]�����o����ƁA����ȍ~�A�s���܂ł��R�����g�Ƃ��ēo�^����B
        �������_�u���R�[�e�[�V�������͂��̂܂ܓo�^����B
      (8)KC�����Ή��ŁA������擪�ȊO�̊���[(]�����o���ɑO�̕�����[!+-*�^��|^%,($]�ȊO�̏ꍇ�A
         ���s����B���̎��AKC�t���O��ON�ɂ���B(�j���j�b�N�ϊ��̑Ó������`�F�b�N����ׁj
      (9)�V���O���R�[�e�[�V����[']�����o����ƁA����[']���o�ꂷ�邩�s���܂ł��̂܂܊i�[����B
         �_�u���R�[�e�[�V�����Ƃ͔r�����삷��B
*/
int GetParam( char *buff )
{
int	len;
int	j,n;
int label;
int kakko;
int dq,sq;
char *Opr;
	/* �I�y�����h�o�b�t�@�|�C���^������ */
	OutTbl.opr[0] = OutTbl.line;
	Opr = OutTbl.line;

	/* 1�s�I���܂Ŏ��o�� */
	len = strlen( buff );
	for(  j = 0 , n = 0 ,label = 0,kakko = 0 ,dq = 0,sq = 0 ; ( n < MAXOPR )&&( j <len ) ; buff++ ){
		switch ( *buff & 0xff ){
		case ';':
			/* �R�����g���o */
			/* �R�[�e�V������ */
			if ( dq || sq ){
				/* �p�����[�^�o�^���� */
				Opr[j++] = *buff;
				break;
			}
			/* �R�����g�|�C���^�ݒ� */
			OutTbl.comment = Opr+j+1;
			/* �c��̕�������R�����g�Ƃ��Ċi�[ */
			memcpy( OutTbl.comment , buff , strlen(buff));
			/* �G���g���̂Ȃ��I�y�����h�|�C���^��NULL�ɂ��� */
			if (!j)	OutTbl.opr[n] = 0;
			return NORM;
		case '\n':
		case 0x0:
			/* ���s���o�ŏI�� */
			/* ���ʂ���уR�[�e�[�V�����������Ȃ�G���[ */
			if ( kakko || dq || sq ){
				return ILLDQUO;
			}
			/* �G���g���̂Ȃ��I�y�����h�|�C���^��NULL�ɂ��� */
			if (!j) OutTbl.opr[n] = 0;
			return NORM;
		case ':':
			/* �R�[�e�V����/���ʓ� */
			if ( dq || sq || kakko ){
				/* �p�����[�^�o�^���� */
				Opr[j++]=*buff;
				break;
			}
			/* ���x�����o�� */
			if ( !label && !n ){
				/* ���x����o�^���� */
				if ( j > MAXNAME ) return LBOFLOW;
				OutTbl.label = Opr;
				/* �i�[�o�b�t�@�|�C���^���X�V���� */
				Opr += j + 1;
				OutTbl.opr[0] = Opr;
				j = 0;
				label++;
				break;
			}
			/* �Q�d���x���̓G���[�I�� */
			else if (!n) return ILLLBL;
			return ILLOPR;
		case ',':
			/* �s�̓r���ŃJ���}���o */
			if ( !dq && !sq && j ){
				/* ���s���� */
				Opr += j + 1;
				OutTbl.opr[++n] = Opr;
				j = 0;
			}
			/* �p�����[�^�o�^���� */
			Opr[j++]=*buff;
			break;
		case ')':
			/* �p�����[�^�o�^���� */
			Opr[j++]=*buff;
			/* �R�[�e�V������ */
			if ( dq || sq ) break;
			/* �Ƃ����ʌ��o */
			if ( kakko ){
				kakko--;
				break;
			}
			/* �G���[�I�� */
			else return ILLDQUO;
#if __SJIS_SPACE
		case 0x81:
			/* �S�p�X�y�[�X�ł͂Ȃ� */
			if ( buff[1] != 0x40 ){
				/* �p�����[�^�o�^���� */
				Opr[j++]=*buff;
				break;
			}
			/* �R�[�e�[�V�������͓o�^���� */
			if ( dq||sq ){
				/* �p�����[�^�o�^���� */
				Opr[j++]=*buff;
				break;
			}
			/* �|�C���^�X�V */
			buff++;
			/* ������擪�����ʓ��Ȃ�ǂݔ�΂� */
			if ( !j ) break;
			/* �j���j�b�N�o�^���Ȃ�΁A���s���� */
			if ( !n ){
				/* ���s���� */
				Opr += j + 1;
				OutTbl.opr[++n] = Opr;
				j = 0;
			}
			break;
#endif
		case 0x20:
		case '\t':
			/* ��Q�I�y�����h�ȍ~�ł��AJR�^�O�ł��� */
			if ( (n >= 2)&&( j == 3 )&&
				( !strcmp( &Opr[0],",jr" )||!strcmp( &Opr[0],",JR" ))){
				/* �X�y�[�X�Ƃ��ēo�^���� */
				Opr[j++] = 0x20;
				break;
			}
			/* �R�[�e�[�V�������͓o�^���� */
			if ( dq || sq ){
				/* �p�����[�^�o�^���� */
				Opr[j++]=*buff;
				break;
			}
			/* ������擪�����ʓ��Ȃ�ǂݔ�΂� */
			if ( !j ) break;
			/* �j���j�b�N�o�^���Ȃ�΁A���s���� */
			if ( !n ){
				/* ���s���� */
				Opr += j + 1;
				OutTbl.opr[++n] = Opr;
				j = 0;
			}
			break;
		/* �V���O���R�[�e�[�V�����}�[�N���o */
		case 0x27:
			/* �_�u���R�[�e�[�V�������Ȃ�A�p�����[�^�o�^���� */
			if ( dq ){
				Opr[j++]=*buff;
				break;
			}
			/* �V���O���R�[�e�[�V�����}�[�N�����o */
			if ( !sq ){
				sq++;
				/* �擪�I�y�����h�ł��� */
				if ( j && !n ){
					/* ���s���� */
					Opr += j + 1;
					OutTbl.opr[++n] = Opr;
					j = 0;
				}
				/* �p�����[�^�o�^ */
				Opr[j++]=*buff;
			}
			else{
				sq--;
				/* �p�����[�^�o�^���� */
				Opr[j++]=*buff;
			}
			break;
		case 0x22:
			/* �V���O���R�[�e�[�V�������Ȃ�A�p�����[�^�o�^���� */
			if ( sq ){
				Opr[j++]=*buff;
				break;
			}
			/* �_�u���R�[�e�[�V�����}�[�N�����o */
			if ( !dq ){
				dq++;
				/* �擪�I�y�����h�ł��� */
				if ( j && !n ){
					/* ���s���� */
					Opr += j + 1;
					OutTbl.opr[++n] = Opr;
					j = 0;
				}
				/* �p�����[�^�o�^ */
				Opr[j++]=*buff;
			}
			else{
				dq--;
				/* �p�����[�^�o�^���� */
				Opr[j++]=*buff;
			}
			break;
		case '(':
			/* ���ʌ��o */
			if ( !dq && !sq ) kakko++;
			/* �P�O�̕��������Z�q����уJ���}�ȊO�̎��A���s����(KC�Ή�) */
			if ( n && j && !dq && !sq ){
				if ( !strchr( "!+-*/#|^%=<>,($&" , Opr[j-1] ) ){
					/* KC�����t���O�Z�b�g */
					OutTbl.kc = 1;
					Opr += j + 1;
					OutTbl.opr[++n] = Opr;
					j = 0;
				}
			}
		case '+':
		case '-':
		case '&':
		case '$':
			/* �����̕������擪�I�y�����h�Ō��o���ꂽ�ꍇ�A���s���� */
			if ( !n && !dq && !sq && j){
				Opr += j + 1;
				OutTbl.opr[++n] = Opr;
				j = 0;
			}
		default:
			/* ��L�ȊO�̏ꍇ�A�p�����[�^�o�^���� */
			Opr[j++]=*buff;
			break;
		}
		/* �ő�I�y�����h�����z�����ꍇ�A�����I�� */
		if ( j >= MAXLEN ) return OPOFLOW;
	}
	/* ���ʂ���уR�[�e�[�V�����������Ȃ�G���[ */
	if ( kakko || dq || sq ) return ILLDQUO;

	/* ����I�� */
	return NORM;
}

/**********************************************************************/
/*   GetOprKind : Get Opecode kind                                    */
/*                                                                    */
/*   ����    : �^����ꂽ��������e��I�y�R�[�h�ɕ�������             */
/*   ����    : ���̓|�C���^�A�I�y�R�[�h���                           */
/*   �o��    : �G���[���i0:����A0�ȊO:�G���[�j                     */
/*                                                                    */
/**********************************************************************/
int GetOprKind( int num , char * buff , unsigned short * kind , unsigned short * data , unsigned short *sir )
{
char mnwk[MAXMN];	/* �j���j�b�N�ϊ��p���[�N */
int rc;
unsigned short op,sts;
#if __DEBUG
printf("GetOprKind [%s]\n",buff);
#endif
	/* ��ʏ�񏉊��� */
	*kind = 0;
	*data = 0;
	*sir = 0;
	/* �v��������G���g�����Ȃ��ꍇ�A�G���[�I�� */
	if ( !buff[0] ) return NOENT;

	/* �����R�[�h�ϊ�(Null�Ȃ�G���[�I��) */
	if(!( ChgCode( oprwk , buff ) ) ) return NOENT;

	/* JR�^�O�ł��� */
	if( !memcmp(oprwk,"JR ", 3 ) ){
		/* �v�Z���Ƃ��ď������� */
		rc = GetCalcData( &buff[3] , kind , data );
		return rc;
	}
#if __EUR_MNEMONIC
	else if( !memcmp(oprwk,"J.", 2 ) ){
		/* �v�Z���Ƃ��ď������� */
		rc = GetCalcData( &buff[2] , kind , data );
		return rc;
	}
#endif /* __EUR_MNEMONIC */
	/* �I�y�����h1�̎��̂݃`�F�b�N���� */
	if ( num==1 ){
		/* �������W�X�^�w��ł��� */
		if ( !GetIRegKind( buff , &op ) ){
			*kind = op;
			return NORM;
		}
		/* �t���O�w��ł��� */
		if ( !GetFlagKind( buff , &op ) ){
			*kind = op;
			return NORM;
		}
	}
	/* ���C�����W�X�^�w��ł��� */
	if ( !( rc = GetReg( buff , &sts , sir ) ) ){
		*data = sts;
		/* SIR�w��Ȃ� */
		if ( !OutTbl.si ){
			switch( sts ){
			case 0:  *kind = RSZ; break;
			case 30: *kind = RSY; break;
			case 31: *kind = RSX; break;
			default: *kind = REG; break;
			}
		}
		/* ����SIR�w�肪����ꍇ�A�ʏ탌�W�X�^�w��Ƃ��� */
		else *kind = REG;
		return NORM;
	}
	/* �I�y�����h�L�q�����C�����W�X�^�w��ȊO */
	if ( rc != ILLOPR ) return rc;
	/* �������C���f�b�N�X�w��ł��� */
	if ( !( rc = GetIndexKind( buff , &op , &sts , sir ) ) ){
		*data = sts;
		*kind = op;
		return NORM;
	}
	/* �I�y�����h�L�q���C���f�b�N�X�w��ȊO */
	if ( rc != ILLOPR ) return rc;
	/* JR���߂ł��� */
	ChgCode( mnwk , OutTbl.opr[0] );
	if ( !strcmp( mnwk , "JR" ) ){
		/* �}IM7�`���ł��� */
		if( (buff[0]=='+')||(buff[0]=='-') ){
			OutTbl.pm7 = 1;
			if (buff[0]=='-')
				OutTbl.pm7 = 2;
			/* �|�C���^�X�V */
			buff++;
		}
	}
#if __EUR_MNEMONIC
	/* EU(Europe)�`���j���j�b�N�g�p���ł��� */
	if ( InstMode==2 ){
		/* �I�y�����h�Q���R�ł��A�擪������"L"���A������2�ł��� */
		if ( ( num==2 || num==3 )&&( buff[0]=='L'||buff[0]=='l' )&&( strlen(buff)==2 ) ){
			/* 2�`8�͈͓̔��ł��� */
			if ( 2<= (int)(buff[1]-'0') <=8 ){
				*data = (unsigned short)(buff[1]-'0');
				*kind =LBLOK;
				return NORM;
			}
		}
	}
#endif /* __EUR_MNEMONIC */
	/* �v�Z���Ƃ��ď������� */
	if ( !( rc = GetCalcData( buff , kind , data ) ) ) return NORM;
	return rc;
}

/**********************************************************************/
/*   GetIndexKind : Get Memory Index Kind                             */
/*                                                                    */
/*   ����    : �������C���f�b�N�X��ʂ��擾����                       */
/*   ����    : ���̓|�C���^�A�I�y�R�[�h���                           */
/*   �o��    : �j���j�b�N���(kind/opr)                               */
/*           : �G���[���i0:����A0�ȊO:�ُ�j                       */
/*                                                                    */
/**********************************************************************/
int GetIndexKind( char * buff , unsigned short * kind , unsigned short * data ,unsigned short *sir )
{
char Indexwk[6];
int	i,len,rc;
unsigned short opr,sts,knd;

	/* �������C���f�b�N�X�w��ł��� */
	len = strlen( buff );
	memcpy( oprwk , buff , len );
	if ( ( oprwk[0] == '(' )&&( oprwk[len-1] == ')' ) ){
		oprwk[len-1] = 0;
		/* ���C�����W�X�^�ɂ��C���f�b�N�X�ł��� */
		if ( !( rc = GetReg( &oprwk[1] , &sts ,sir ) ) ){
			*data = sts;
			/* SIR�w��Ȃ� */
			if ( !OutTbl.si ){
				switch(sts){
				case 0:  *kind = MRSZ; break;
				case 30: *kind = MRSY; break;
				case 31: *kind = MRSX; break;
				default: *kind = MREG; break;
				}
			}
			/* ����SIR�w�肪����ꍇ�A�ʏ탌�W�X�^�w��Ƃ��� */
			else *kind = MREG;
			return NORM;
		}
		/* �I�y�����h�L�q�����C�����W�X�^�w��ȊO */
		if ( rc != ILLOPR ) return rc;
		/* �C���f�b�N�X��ʂ��T�[�`���� */
		memcpy( Indexwk , oprwk , 4 );
		Indexwk[4] = 0;
		/* �e�[�u���T�[�` */
		for ( i = 0 ; i < MAXIDX ; i++ ){
			/* IX�AIZ�w��ł��� */
			if (!strcmp( Indexwk , moprtbl[i].name )){
				sts = moprtbl[i].code;
				/* ���C�����W�X�^�ɂ��C���f�b�N�X�ł��� */
				if ( !( rc = GetReg( &oprwk[4] , &opr ,sir ) ) ){
					*data = opr;
					/* SIR�w��Ȃ� */
					if ( !OutTbl.si ){
						switch(opr){
						case 0:  *kind = sts|(unsigned short)RSZ; break;
						case 30: *kind = sts|(unsigned short)RSY; break;
						case 31: *kind = sts|(unsigned short)RSX; break;
						default: *kind = sts|(unsigned short)REG; break;
						}
					}
					/* ����SIR�w�肪����ꍇ�A�ʏ탌�W�X�^�w��Ƃ��� */
					else{ *kind = sts|(unsigned short)REG; }
					return NORM;
				}
				/* �I�y�����h�L�q�����C�����W�X�^�w��ȊO */
				if ( rc != ILLOPR ) return rc;
				/* �v�Z���Ƃ��ď������� */
				if ( !( rc = GetCalcData( &oprwk[4] , &knd , &opr ) ) ){
					/* �͈͊O�̒l�ł��� */
					if ( opr > 255 ) return OFLOW;
					*data = opr;
					*kind = sts|knd;
					return NORM;
				}
				return rc;
			}
		}
	}
	/* �Y���Ȃ� */
	return ILLOPR;
}
/**********************************************************************/
/*   GetIRegKind : Get Internal Register Kind                         */
/*                                                                    */
/*   ����    : �������W�X�^��ʂ��擾����                             */
/*   ����    : ���̓|�C���^�A�I�y�R�[�h���                           */
/*   �o��    : �j���j�b�N���(kind)                                   */
/*           : �G���[���i0:����A0�ȊO:�ُ�j                       */
/*                                                                    */
/**********************************************************************/
int GetIRegKind( char * buff , unsigned short * kind )
{
int	i;
char mnwk[MAXMN];	/* �j���j�b�N�ϊ��p���[�N */
	i = 0;
	/* �j���j�b�N��ʃ`�F�b�N */
	ChgCode( mnwk , OutTbl.opr[0] );
#if __EUR_MNEMONIC
	/* EU(Europe)�`���j���j�b�N�g�p���ł��� */
	if ( InstMode==2 ){
		/* �������W�X�^���p���߃e�[�u���T�[�` */
		while( strcmp( mnwk , irtblC[i].name ) ){
			/* �Y�����߂ł͖����ꍇ�A�����I�� */
			if ( ++i >= MAXIR ) return ILLOPR;
		}
	}
	else
#endif /* __EUR_MNEMONIC */
	{
		/* �������W�X�^���p���߃e�[�u���T�[�` */
		while( strcmp( mnwk , irtbl[i].name ) ){
			/* �Y�����߂ł͖����ꍇ�A�����I�� */
			if ( ++i >= MAXIR ) return ILLOPR;
		}
	}
	/* �I�y�����h���͐���ł��� */
	if ( !strlen(buff) || strlen(buff)>MAXMN ) return ILLOPR;
	/* �啶���ɕϊ����� */
	ChgCode( mnwk , buff );
#if __EUR_MNEMONIC
	/* EU(Europe)�`���j���j�b�N�g�p���ł��� */
	if ( InstMode==2 ){
		/* �������W�X�^�e�[�u��(EU(Europe)�p�ǉ���)�T�[�` */
		for ( i=0 ; i < MAXIREG+IREGADD ; i++ ){
			if (!strcmp( mnwk , regtbl[i].name )){
				/* �������W�X�^�R�[�h��Ԃ� */
				*kind = regtbl[i].code;
				return NORM;
			}
		}
	}
	else
#endif /* __EUR_MNEMONIC */
	{
		/* �������W�X�^�e�[�u���T�[�` */
		for ( i = 0 ; i < MAXIREG ; i++ ){
			if (!strcmp( mnwk , regtbl[i].name )){
				/* �������W�X�^�R�[�h��Ԃ� */
				*kind = regtbl[i].code;
				return NORM;
			}
		}
	}
	/* �Y���Ȃ���Ԃ� */
	return ILLOPR;
}
/**********************************************************************/
/*   GetFlagKind : Get Flag Register Kind                             */
/*                                                                    */
/*   ����    : �����t���O��ʂ��擾����                               */
/*   ����    : ���̓|�C���^�A�I�y�R�[�h���                           */
/*   �o��    : �j���j�b�N���(kind)                                   */
/*           : �G���[���i0:����A0�ȊO:�ُ�j                       */
/*                                                                    */
/**********************************************************************/
int GetFlagKind( char * buff , unsigned short * kind )
{
int	i;
char mnwk[MAXMN];	/* �j���j�b�N�ϊ��p���[�N */
	i = 0;
	/* �j���j�b�N��ʃ`�F�b�N */
	ChgCode( mnwk , OutTbl.opr[0] );
	/* �t���O���p���߃e�[�u���T�[�` */
	while( strcmp( mnwk , fltbl[i].name ) ){
		/* �Y�����߂ł͖����ꍇ�A�����I�� */
		if ( ++i >= MAXFL ) return ILLOPR;
	}
	/* �I�y�����h���͐���ł��� */
	if ( !strlen(buff) || strlen(buff) > MAXMN ) return ILLOPR;
	/* �啶���ɕϊ����� */
	ChgCode( mnwk , buff );
	/* �t���O�e�[�u���T�[�` */
	for ( i = 0 ; i < MAXFLAG ; i++ ){
		if (!strcmp( mnwk , flagtbl[i].name )){
			/* �t���O�R�[�h��Ԃ� */
			*kind = flagtbl[i].code;
			return NORM;
		}
	}
	/* �Y���Ȃ���Ԃ� */
	return ILLOPR;
}
/**********************************************************************/
/*   GetReg : Get Register Number                                     */
/*                                                                    */
/*   ����    : �^����ꂽ�����񂩂烌�W�X�^�ԍ����擾����             */
/*   ����    : ���̓|�C���^�A�I�y�R�[�h���                           */
/*   �o��    : ���W�X�^�ԍ�(data)                                     */
/*           : �G���[���i0:����A0�ȊO:�ُ�j                       */
/*                                                                    */
/**********************************************************************/
int GetReg( char * buff , unsigned short * data ,unsigned short * sir )
{
char mnwk[MAXMN];	/* �j���j�b�N�ϊ��p���[�N */
unsigned short sts,kind;
int rc;
	/* SIR�w��t���O���N���A���� */
	*sir = 0;
#if __EUR_MNEMONIC
	/* EU(Europe)�`���j���j�b�N�g�p���ł��� */
	if ( InstMode==2 ){
		/* ���W�X�^�w��ł��� */
		if ( buff[0] == '#' ){
			/* ����C���f�b�N�X�w��`�F�b�N */
			if ( strlen(buff) < MAXMN ){
				ChgCode( mnwk , &buff[0] );
				/* #0�w��ł��� */
				if ( !strcmp( mnwk ,"#0" ) ){
					*data = 31;
					*sir = 1;
					return ( OutTbl.si ? REGERR : NORM );
				}
				/* #1�w��ł��� */
				if ( !strcmp( mnwk ,"#1" ) ){
					*data = 30;
					*sir = 1;
					return ( OutTbl.si ? REGERR : NORM );
				}
				/* #2�w��ł��� */
				if ( !strcmp( mnwk ,"#2" ) ){
					*data =  0;
					*sir = 1;
					return ( OutTbl.si ? REGERR : NORM );
				}
			}
			/* #0,#1,#2�ȊO�Ȃ�A�G���[�����Ƃ��� */
			*data = 0;
			return REGERR;
		}
	}
#endif /* __EUR_MNEMONIC */
	/* ���W�X�^�w��($0�`$31)�ł��� */
	if ( buff[0] == '$' ){
		/* ����C���f�b�N�X�w��`�F�b�N */
		if ( strlen(buff) < MAXMN ){
			ChgCode( mnwk , &buff[1] );
			/* $SX�w��ł��� */
			if ( !strcmp( mnwk ,"(SX)" ) || !strcmp( mnwk ,"SX" )
				){
				*data = 31;
				*sir = 1;
				return ( OutTbl.si ? REGERR : NORM );
			}
			/* $SY�w��ł��� */
			if ( !strcmp( mnwk ,"(SY)" ) || !strcmp( mnwk ,"SY" )
				){
				*data = 30;
				*sir = 1;
				return ( OutTbl.si ? REGERR : NORM );
			}
			/* $SZ�w��ł��� */
			if ( !strcmp( mnwk ,"(SZ)" ) || !strcmp( mnwk ,"SZ" )
				){
				*data =  0;
				*sir = 1;
				return ( OutTbl.si ? REGERR : NORM );
			}
		}
		/* ���W�X�^�ԍ��擾 */
		if(!(rc = GetCalcData( &buff[1] , &kind , &sts ) )){
			/* ���̖������̏ꍇ�A�G���[�I������ */
			if ( kind == LBLNG ) return LBLNOENT;
			/* ���W�X�^�ԍ����� */
			if ( sts <= 31 ){
				*data = sts;
				return NORM;
			}
			/* �G���[�I�� */
			*data = 0;
			return REGERR;
		}
		else return rc;
	}

	/* �G���[�I�� */
	*data = 0;
	/* ���W�X�^�w��ł͂Ȃ� */
	return ILLOPR;
}
/**********************************************************************/
/*   SetLabelTbl : Entry Label table                                  */
/*                                                                    */
/*   ����    : ���x���e�[�u���Ƀ��x���ƃA�h���X��o�^����             */
/*   ����    : ���x���|�C���^�A�o�^�A�h���X�l�i0:�ȊO�j               */
/*   �o��    : �G���[���i0:����A0�ȊO:�ُ�j                       */
/*                                                                    */
/**********************************************************************/
int SetLabelTbl( char * buff , unsigned short adr ,unsigned char flag )
{
LBL * Label;
LBL * Labelwk;

	/* �擪���x���|�C���^�Z�b�g */
	Label = LabelTbl;
	while( Label ){
		/* �Y�����x�������� */
		if (!strcmp( Label->name , buff )){
			/* ���ɓ������̂��g���Ă���ꍇ�A�G���[�I�� */
			return DUPLBL;
		}
		/* �ŏI���x���A�h���X���B */
		if ( !Label->np ) break;
		/* ���x���|�C���^�X�V */
		Label = Label->np;
	}
	/* ���x���e�[�u���m�� */
	if ( Labelwk = malloc( sizeof(LBL)+strlen(buff) ) ){
		memset( Labelwk , 0 , sizeof(LBL)+strlen(buff) );
	}
	else return LBNOALOC;

	/* ����o�^�Ȃ�擪�|�C���^�ɓo�^���� */
	if ( !LabelTbl ) LabelTbl = Labelwk;
	/* �m�ۂ����|�C���^�����|�C���^�Ƃ��ēo�^���� */
	else Label->np = Labelwk;

	/* ���x���|�C���^�X�V */
	Label = Labelwk;
	/* ���x�����̂��e�[�u���ɓo�^���� */
	memcpy( Label->name , buff , strlen(buff) );
	/* ���݂̃A�Z���u���A�h���X�����x���e�[�u���ɓo�^���� */
	Label->adr = adr;
	/* ���x����ʂ�o�^���� */
	Label->flag = flag;
	/* ���x���o�^���X�V */
	LabelCnt++;

	return NORM;
}

/**********************************************************************/
/*   GetLabelAdr : Get Label Address                                  */
/*                                                                    */
/*   ����    : ���x��������A�h���X����������                         */
/*   ����    : ���x���|�C���^�A�o�^�A�h���X�l                         */
/*   �o��    : �G���[���i0:����A0�ȊO:���x�����G���g���Ȃ��j       */
/*                                                                    */
/**********************************************************************/
int GetLabelAdr( char * buff , unsigned short * adr )
{
LBL * Label;
	/* �擪���x���|�C���^�Z�b�g */
	Label = LabelTbl;
	while( Label ){
		/* �Y�����x�������� */
		if (!strcmp( Label->name , buff )){
			/* ORG���߂ŗ��p�i���p�j���Ă���H */
			if( OutTbl.kind == OP_ORG ){
				/* �A�h���X���x����ʂɐU��ւ��� */
				Label->flag = LBL_ADR;
			}
			/* ���x���A�h���X/��ʂ�Ԃ� */
			*adr = Label->adr;
			LblFlg |= Label->flag;
			return NORM;
		}
		/* ���x���|�C���^�X�V */
		Label = Label->np;
	}
	return LBLNOENT;
}

/**********************************************************************/
/*   GetMacKind : Get Macro Kind                                      */
/*                                                                    */
/*   ����    : �}�N����ʂ����肷��                                   */
/*   ����    : �I�y�����h�|�C���^                                     */
/*   �o��    : �j���j�b�N���                                         */
/*                                                                    */
/**********************************************************************/
int GetMacKind( char * buff )
{
int	i;
	/* �^�����߃e�[�u���T�[�` */
	for ( i = 0 ; i < MACDIR ; i++ ){
		if (!strcmp( buff , MacTbl[i].name ))
			/* �^�����߃R�[�h��Ԃ� */
			return MacTbl[i].code;
	}
	/* �Y���Ȃ��i��ʖ��߁j��Ԃ� */
	return UNDEFOPR;
}

/**********************************************************************/
/*   GetMnemonicKind : Get Mnemonic Kind                              */
/*                                                                    */
/*   ����    : �j���j�b�N��ʂ����肷��                               */
/*   ����    : �I�y�����h�|�C���^                                     */
/*   �o��    : �j���j�b�N���                                         */
/*                                                                    */
/**********************************************************************/
unsigned short GetMnemonicKind( char * buff )
{
int	i;
	/* �^�����߃e�[�u���T�[�` */
	for ( i = 0 ; i < MAXDIR ; i++ ){
		if (!strcmp( buff , Directivetbl[i].name ))
			/* �^�����߃R�[�h��Ԃ� */
			return Directivetbl[i].code;
	}
	/* �Y���Ȃ��i��ʖ��߁j��Ԃ� */
	return UNDEFOPR;
}
/**********************************************************************/
/*   CheckLabel : Check Label String                                  */
/*                                                                    */
/*   ����    : ���x��������̌��S�����`�F�b�N����                     */
/*   ����    : �I�y�����h�|�C���^                                     */
/*   �o��    : �G���[���i0:����A0�ȊO:�ُ�j                       */
/*                                                                    */
/**********************************************************************/
int CheckLabel( char * buff )
{
int	i,len;

	/* ���x�������񒷃`�F�b�N */
	len = strlen( buff );
	if( !len  ) return ILLOPR;
	if( len > MAXNAME ) return OPOFLOW;
	/* �擪�����������ł͂Ȃ� */
	if( strchr( DecStr , buff[0] ) ) return ILLLBL;
	/* ���x�����p�s�������������� */
	for( i = 0 ; i < len ; i++ ){
		/* ���x��������`�F�b�N */
		if( !strchr( LabelStr , buff[i] ) ) return ILLLBL;
	}
	/* ����I�� */
	return NORM;
}
/**********************************************************************/
/*   ChgCode : Change String code  (abc...->ABC...)                   */
/*                                                                    */
/*   ����    : �A���t�@�x�b�g��������啶���ɕϊ�����                 */
/*   ����    : �I�y�����h�|�C���^�A�o�̓o�b�t�@�|�C���^               */
/*   �o��    : �ϊ�������                                           */
/*                                                                    */
/**********************************************************************/
int ChgCode( char * dst , char * src )
{
int	i,len;
	/* �o�͐敶����N���A */
	dst[0] = 0;
	if (! src ) return 0;
	/* �I�y�����h�����񒷃`�F�b�N */
	len = strlen( src );
	if( !len || ( len > MAXLEN ) ) return 0;
	/* �ϊ����� */
	for( i = 0 ; i < len ; i++ ){
		/* ������`�F�b�N */
		if( (src[i] >= 'a')&&(src[i] <= 'z') ){
			/* �A���t�@�x�b�g�������Ȃ�啶���ɕϊ� */
			dst[i] = (char)(src[i] - 0x20);
		}
		else{
			/* ���̂܂܃R�s�[ */
			dst[i] = src[i];
		}
	}
	/* �ŏI��Null�}�� */
	dst[i] = 0;
	return len;
}

/**********************************************************************/
/*   ChgKcName : Change KC Mnemonic Name  (KC->HD61)                  */
/*                                                                    */
/*   ����    : KC�`���̃j���j�b�N��HD61�`���ɕϊ�����                 */
/*   ����    : �I�y�����h�|�C���^�A�o�̓o�b�t�@�|�C���^               */
/*   �o��    : �G���[���i0:����A0�ȊO:�ُ�j                       */
/*                                                                    */
/**********************************************************************/
int ChgKcName( char * dst , char * src )
{
int len;
char mnwk[MAXMN+1];	/* �j���j�b�N�ϊ��p���[�N */
	/* ���[�N�G���A������ */
	memset( mnwk , 0 , sizeof(mnwk) );
	/* ���̓j���j�b�N���� */
	if (!( len = ChgCode( mnwk , src ) )) return UNDEFOPR;
	/* LDD/STD�n�񖽗߂ł��� */
	if ( !memcmp( mnwk , "LDM" , 3 )||!memcmp( mnwk , "STM" , 3 ) ){
		/* 8�r�b�g�A16�r�b�g���߂ł���(��3�I�y�����h���Ȃ�) */
		if(!OutTbl.opr[3]){
			mnwk[2]='D';
			if ( mnwk[3]!='W' ){
				OutTbl.sts = 1;
			}
		}
		/* �}���`���[�h�v���ł��� */
		else{
			/* KC�`���v������ */
			if( OutTbl.kc ){
				/* STMW�LDMW �ł��� */
				if( mnwk[3] == 'W' ){
					mnwk[2] = 'D';
					mnwk[3] = 'M';
				}
				else return UNDEFOPR;
			}
		}
	}
	else{
		/* KC�`���v������ */
		if( OutTbl.kc ){
			/* xxxxW �ł��� */
			if( mnwk[len-1] == 'W' ){
				mnwk[len-1] = 'M';
			}
			else  return UNDEFOPR;
		}
	}
	memcpy( dst , mnwk ,MAXMN );
	return NORM;
}
#if __EUR_MNEMONIC
/**********************************************************************/
/*   ChgEuropeName : Change Europe Style Mnemonic Name  (EUR->HD61)   */
/*                                                                    */
/*   ����    : Europe�`���̃j���j�b�N��HD61�`���ɕϊ�����             */
/*   ����    : �I�y�����h�|�C���^�A�o�̓o�b�t�@�|�C���^               */
/*   �o��    : �G���[���i0:����A0�ȊO:�ُ�j                       */
/*                                                                    */
/**********************************************************************/
int ChgEuropeName( char * dst , char * src )
{
int len;
char mnwk[MAXMN+1];	/* �j���j�b�N�ϊ��p���[�N */
	/* ���[�N�G���A������ */
	memset( mnwk , 0 , sizeof(mnwk) );
	/* ���̓j���j�b�N���� */
	if (!( len = ChgCode( mnwk , src ) )) return UNDEFOPR;
	/* AI�`���̃}���`���[�h�w��Ȃ�G���[�Ƃ��� */
	if (mnwk[len-1] == 'M') return UNDEFOPR;
	/* OCB�n�񖽗߂ł��� */
	if ( !memcmp( mnwk , "OCB" , 3 ) ){
		/* STL�n��ɕύX���� */
		memcpy( mnwk, "STL" , 3 );
		/* �}���`���[�h�w��ł͂Ȃ� */
		if (mnwk[3] != 'L'){
			goto ChgEnd;
		}
	}
	/* ICB�n�񖽗߂ł��� */
	else if ( !memcmp( mnwk , "ICB" , 3 ) ){
		/* LDL�n��ɕύX���� */
		memcpy( mnwk, "LDL" , 3 );
		/* �}���`���[�h�w��ł͂Ȃ� */
		if (mnwk[3] != 'L'){
			goto ChgEnd;
		}
	}
	/* PCB�n�񖽗߂ł��� */
	else if ( !memcmp( mnwk , "PCB" , 3 ) ){
		/* LDL�n��ɕύX���� */
		memcpy( mnwk, "PPO" , 3 );
		/* �}���`���[�h�w��ł͂Ȃ� */
		if (mnwk[3] != 'L'){
			goto ChgEnd;
		}
	}
	/* PRA�n�񖽗߂ł��� */
	else if ( !memcmp( mnwk , "PRA" , 3 ) ){
		/* PSR�n��ɕύX���� */
		memcpy( mnwk, "PSR" , 3 );
		/* �}���`���[�h�w��ł͂Ȃ� */
		if (mnwk[len-1] != 'L'){
			goto ChgEnd;
		}
	}
	/* GRA�n�񖽗߂ł��� */
	else if ( !memcmp( mnwk , "GRA" , 3 ) ){
		/* GSR�n��ɕύX���� */
		memcpy( mnwk, "GSR" , 3 );
		/* �}���`���[�h�w��ł͂Ȃ� */
		if (mnwk[len-1] != 'L'){
			goto ChgEnd;
		}
	}
	/* BDN/BUP�n�񖽗߂ł��� */
	else if ( !memcmp( mnwk , "BDN" , 3 ) || !memcmp( mnwk , "BUP" , 3 ) ){
		/* ���I�y�����h��IM8�ł��� */
		if ( (OutTbl.opkind[0] &MASKOP2)==(IM8 &MASKOP2) ){
			/* BDNS/BUPS�n��ɕύX���� */
			mnwk[3]='S';
			goto ChgEnd;
		}
	}
	/* JPW�n�񖽗߂ł��� */
	else if ( !memcmp( mnwk , "JPW" , 3 ) ){
		/* �I�y�����h1�Ȃ� */
		if( !OutTbl.opr[1] ){
			return ILLOPR;
		}
		/* �I�y�����h1�����W�X�^�w��ł��� */
		if ( OutTbl.opr[1][0]=='$' || (OutTbl.opr[1][0]=='('&& OutTbl.opr[1][1]=='$') ){
			/* JP�n��ɕύX���� */
			mnwk[2]=0;
			goto ChgEnd;
		}
	}
	/* CAL/PFL/GFL�ȊO�ł��A�}���`���[�h�w�� xxxxL �ł��� */
	if ( memcmp( mnwk , "CAL" , 3 ) && memcmp( mnwk , "PFL" , 3 ) && memcmp( mnwk , "GFL" , 3 ) && mnwk[len-1] == 'L' ){
		/* �}���`���[�h�w��Ƃ��� */
		mnwk[len-1] = 'M';
	}
ChgEnd:
	memcpy( dst , mnwk ,MAXMN );
	return NORM;
}
#endif /* __EUR_MNEMONIC */
/**********************************************************************/
/*   GetCalcData : Get Calculate Data                                 */
/*                                                                    */
/*   ����    : ������Ƃ��ė^����ꂽ�����v�Z����                     */
/*   ����    : ������擪�|�C���^�A�o�^�A�h���X�l                     */
/*   �o��    : �G���[���i0:����A0�ȊO:�G���[�����j                 */
/*           : ���(kind)  LBLOK(���̉����ς�)�LBLNG(���̖�����)      */
/*           : �v�Z�l(adr) LBLOK���̂ݗL���B����ȊO�͕s���l          */
/**********************************************************************/
int GetCalcData( char * buff , unsigned short * kind ,unsigned short * adr )
{
int rc;
unsigned short val;

	/* �I�y�����h�G���g������ */
	if ( !buff ) return ILLOPR;

	/* �v�Z�p�����񂠂� */
	if ( !strlen(buff) ) return ILLOPR;

	/* �v�Z�o�b�t�@������ */
	CalcPtr = 0;
	memset( calcwk , 0 , sizeof(calcwk) );
	/* �����w�肠�� */
	if ( (buff[0]=='-') || (buff[0]=='+') )
		sprintf( calcwk , "0%s" , buff );
	else
		sprintf( calcwk , "%s" , buff );

	/* ���̉����ς݃Z�b�g */
	Ckind = LBLOK;

	/* �����v�Z */
	if ( rc = CalcVal( &val ) ) return rc;

	/* 2�p�X�ڂɖ���`���x������ */
	if ( pass && (Ckind==LBLNG) ) return LBLNOENT;

	/* ����I�� */
	*adr = val;
	*kind = Ckind;
	return NORM;
}

/**********************************************************************/
/*   CalcVal : Calculate compare Logical                              */
/*                                                                    */
/*   ����    : �^����ꂽ���������r���Z����(�D�揇�ʍŒ�)           */
/*   ����    : ���l�|�C���^                                           */
/*   �o��    : �G���[���i0:����A0�ȊO:�G���[�j                     */
/*           : �v�Z�l(value)                                          */
/*                                                                    */
/**********************************************************************/
int CalcVal(unsigned short * value )
{
int rc;
unsigned short val,wval;

	/* �擪�����̒l���擾 */
	if( rc = CalcVal0( &val ) ) return rc;
	while( 1 ){
		switch( calcwk[CalcPtr] ){
		/* = ���� */
		case '=':
			CalcPtr ++;
			if ( calcwk[CalcPtr] == '>' ){
				CalcPtr ++;
				if ( rc = CalcVal0( &wval ) ) return rc;
				val = (unsigned short)(val >= wval);
				break;
			}
			if ( calcwk[CalcPtr] == '<' ){
				CalcPtr ++;
				if ( rc = CalcVal0( &wval ) ) return rc;
				val = (unsigned short)(val <= wval);
				break;
			}
			if ( rc = CalcVal0( &wval ) ) return rc;
			val = (unsigned short)(val == wval);
			break;
		/* > ���� */
		case '>':
			CalcPtr ++;
			if ( calcwk[CalcPtr] == '=' ){
				CalcPtr ++;
				if ( rc = CalcVal0( &wval ) ) return rc;
				val = (unsigned short)(val >= wval);
				break;
			}
			if ( calcwk[CalcPtr] == '<' ){
				CalcPtr ++;
				if ( rc = CalcVal0( &wval ) ) return rc;
				val = (unsigned short)(val != wval);
				break;
			}
			if ( rc = CalcVal0( &wval ) ) return rc;
			val = (unsigned short)(val > wval);
			break;
		/* < ���� */
		case '<':
			CalcPtr ++;
			if ( calcwk[CalcPtr] == '=' ){
				CalcPtr ++;
				if ( rc = CalcVal0( &wval ) ) return rc;
				val = (unsigned short)(val <= wval);
				break;
			}
			if ( calcwk[CalcPtr] == '>' ){
				CalcPtr ++;
				if ( rc = CalcVal0( &wval ) ) return rc;
				val = (unsigned short)(val != wval);
				break;
			}
			if ( rc = CalcVal0( &wval ) ) return rc;
			val = (unsigned short)(val < wval);
			break;
		case '(':
			return CALERR;
		/* �����I�� */
		default:
			*value = val;
			return NORM;
		}
	}
}

/**********************************************************************/
/*   CalcVal0 : Calculate Logical                                     */
/*                                                                    */
/*   ����    : �^����ꂽ��������_�����Z����(�D�揇��+1)             */
/*   ����    : ���l�|�C���^                                           */
/*   �o��    : �G���[���i0:����A0�ȊO:�G���[�j                     */
/*           : �v�Z�l(value)                                          */
/*                                                                    */
/**********************************************************************/
int CalcVal0(unsigned short * value )
{
int rc;
unsigned short val,wval;

	/* �擪�����̒l���擾 */
	if( rc = CalcValShift( &val ) ) return rc;
	while( 1 ){
		switch( calcwk[CalcPtr] ){
		/* AND���� */
		case '#':
		case '&':
			CalcPtr ++;
			if ( rc = CalcValShift( &wval ) ) return rc;
			val &= wval;
			break;
		/* OR���� */
		case '|':
			CalcPtr ++;
			if ( rc = CalcValShift( &wval ) ) return rc;
			val |= wval;
			break;
		/* XOR���� */
		case '^':
			CalcPtr ++;
			if ( rc = CalcValShift( &wval ) ) return rc;
			val ^= wval;
			break;
		case '(':
			return CALERR;
		/* �����I�� */
		default:
			*value = val;
			return NORM;
		}
	}
}

/**********************************************************************/
/*   CalcValShift : Calculate Shift.                                  */
/*                                                                    */
/*   ����    : �^����ꂽ���������V�t�g���Z����(�D�揇��+2)           */
/*   ����    : ���l�|�C���^                                           */
/*   �o��    : �G���[���i0:����A0�ȊO:�G���[�j                     */
/*           : �v�Z�l(value)                                          */
/*                                                                    */
/**********************************************************************/
int CalcValShift(unsigned short * value )
{
int rc;
unsigned short val,wval;

	/* �擪���l�擾 */
	if( rc = CalcVal1( &val ) ) return rc;
	while( 1 ) {
		/* �E�V�t�g���Z���� */
		if ( (calcwk[CalcPtr]=='>')&&(calcwk[CalcPtr+1]=='>') ){
			CalcPtr += 2;
			if ( rc = CalcVal1( &wval ) ) return rc;
			val = val>>wval;
		}
		else if ( (calcwk[CalcPtr]=='<')&&(calcwk[CalcPtr+1]=='<') ){
			CalcPtr += 2;
			if ( rc = CalcVal1( &wval ) ) return rc;
			val = val<<wval;
		}
		else {
			*value = val;
			return NORM;
		}
	}
}

/**********************************************************************/
/*   CalcVal1 : Calculate Add/Sub.                                    */
/*                                                                    */
/*   ����    : �^����ꂽ�������������Z����(�D�揇��+3)               */
/*   ����    : ���l�|�C���^                                           */
/*   �o��    : �G���[���i0:����A0�ȊO:�G���[�j                     */
/*           : �v�Z�l(value)                                          */
/*                                                                    */
/**********************************************************************/
int CalcVal1(unsigned short * value )
{
int rc;
unsigned short val,wval;

	/* �擪���l�擾 */
	if( rc = CalcVal2( &val ) ) return rc;
	while( 1 ) {
		switch( calcwk[CalcPtr] ){
		/* ���Z���� */
		case '+':
			CalcPtr ++;
			if ( rc = CalcVal2( &wval ) ) return rc;
			val += wval;
			break;
		/* ���Z���� */
		case '-':
			CalcPtr ++;
			if ( rc = CalcVal2( &wval ) ) return rc;
			val -= wval;
			break;
		/* �����I�� */
		default:
			*value = val;
			return NORM;
		}
	}
}

/**********************************************************************/
/*   CalcVal2 : Calculate Multiple/Divide.                            */
/*                                                                    */
/*   ����    : �^����ꂽ���������揜�Z����(�D�揇��+4)               */
/*   ����    : ���l�|�C���^                                           */
/*   �o��    : �G���[���i0:����A0�ȊO:�G���[�j                     */
/*           : �v�Z�l(value)                                          */
/*                                                                    */
/**********************************************************************/
int CalcVal2(unsigned short * value )
{
int rc;
unsigned short val,wval;

	/* �擪���l�擾 */
	if( rc = CalcVal3( &val ) ) return rc;
	while( 1 ) {
		switch( calcwk[CalcPtr] ){
		/* ��Z���� */
		case '*':
			CalcPtr ++;
			if ( rc = CalcVal3( &wval ) ) return rc;
			val *= wval;
			break;
		/* ���Z���� */
		case '/':
			CalcPtr ++;
			if ( rc = CalcVal3( &wval ) ) return rc;
			/* 0�ŏ��Z */
			if ( !wval ) return CALERR;
			val /= wval;
			break;
		/* MOD���� */
		case '%':
			CalcPtr ++;
			if ( rc = CalcVal3( &wval ) ) return rc;
			/* 0�ŏ��Z */
			if ( !wval ) return CALERR;
			val %= wval;
			break;
		/* �����I�� */
		default:
			*value = val;
			return NORM;
		}
	}
}

/**********************************************************************/
/*   CalcVal3 : Calculate ()                                          */
/*                                                                    */
/*   ����    : ���ʓ��̉��Z���s��(�D�揇�ʍō�)                       */
/*   ����    : ���l�|�C���^                                           */
/*   �o��    : �G���[���i0:����A0�ȊO:�G���[�j                     */
/*           : �v�Z�l(value)                                          */
/*                                                                    */
/**********************************************************************/
int CalcVal3( unsigned short *value )
{
int rc;
unsigned short val,flag;

	flag = 0;

	/* ���]�����]�v������ */
	if( calcwk[CalcPtr] == '!' ){
		CalcPtr++;
		flag = 1;
	}
	/* ���ʓ������ł��� */
	if( calcwk[CalcPtr] == '(' ) {
		CalcPtr++;
		/* ���ʓ��̌v�Z�͍ł��D�揇�ʂ̒Ⴂ���ɍs�� */
		if( rc = CalcVal( &val ) ) return rc;
		/* ���ʂ����Ă��Ȃ��B*/
		if( calcwk[CalcPtr++] != ')' ) return ILLDQUO;
	}
	else{
		/* ���l�܂��̓��x���ł��� */
		if( rc = GetValue( &val ) ) return rc;
	}
	/* ���w��ɂ��l�C�� */
	if ( calcwk[CalcPtr] == '.'){
		CalcPtr++;
		switch( calcwk[CalcPtr] ){
		/* ��ʌ��w�� */
		case 'u':
		case 'U':
		case 'h':
		case 'H':
			val = (unsigned short)(val >> 8);
			break;
		/* ���ʌ��w�� */
		case 'd':
		case 'D':
		case 'l':
		case 'L':
			val &= 0xff;
			break;
		/* �r�b�g���] */
		case 'n':
		case 'N':
			val = (unsigned short)(~val);
			break;
		default:
			return ILLOPR;
		}
		CalcPtr++;
	}
	/* �v�Z�������l��Ԃ� */
	*value = (unsigned short)(!flag ? val : !val);
	return NORM;
}

/**********************************************************************/
/*   GetValue : Get Data Value                                        */
/*                                                                    */
/*   ����    : ���x���܂��͐��l����A�l�����o��                     */
/*   ����    : ���l�|�C���^                                           */
/*   �o��    : �G���[���i0:����A0�ȊO:�G���[�j                     */
/*           : �擾�l(value)                                          */
/*           : ���x�����̉����t���O(Ckind) (LBLNG:���̖�����)         */
/*                                                                    */
/**********************************************************************/
int GetValue(unsigned short *value ) {
unsigned short val;
int		i,rc,quo;
char	lblwk[MAXLINE+2];   /* �v�Z�l�擾�p���x�������[�N   */
char	QuoChar;
	/* �f�[�^������ */
	*value = 0;
	/* ���x��/���l�擾�p�o�b�t�@������ */
	memset( lblwk , 0 , sizeof(lblwk) );

	/* ���Z�q���o�܂ŌJ��Ԃ� */
	i = 0;
	quo = 0;
	QuoChar = 0;
	while ( 1 ){
		/* �����񌟏o */
		if ( !quo ){
			/* �R�[�e�[�V������ʁi�V���O��/�_�u��)���擾 */
			if ( ( calcwk[CalcPtr] == 0x22 )||( calcwk[CalcPtr] == 0x27 )){
				QuoChar = calcwk[CalcPtr];
				quo ++;
			}
		}
		/* �R�[�e�[�V���������H */
		else if ( calcwk[CalcPtr] == QuoChar ) quo ++;
		/* ���Z�q�܂���Null���o�ɂďI�� */
		if ( !calcwk[CalcPtr] || (!quo && strchr( "+-*/#|^%().<>=" , calcwk[CalcPtr] )) ) break;
		/* �擪�ȊO��&���o�ŏI�� */
		if ( !quo && ( calcwk[CalcPtr] == '&' && i ) ) break;
		/* ������R�s�[ */
		lblwk[i++] = calcwk[CalcPtr++];
		/* �R�[�e�[�V������������I������ */
		if ( quo == 2 ) break;
	}
	/* ���o����������͐��l�ł��� */
	if ( rc = GetData( lblwk , &val ) ){
		/* �I�y�����h�L�q�����l�ȊO */
		if (rc != ILLOPR ) return rc;
		/* ���x���ł��� */
		if ( rc = CheckLabel( lblwk ) ) return ILLOPR;
		/* ���x���A�h���X���o�� */
		if ( GetLabelAdr ( lblwk , &val ) ){
			/* ���x�����̖������Ƃ��� */
			Ckind = LBLNG;
			val = 0;
		}
	}
	/* ���߂��l��Ԃ� */
	*value = val;
	return NORM;
}

/**********************************************************************/
/*   GetData : Get Immediate Data                                     */
/*                                                                    */
/*   ����    : �����񂩂琔�l�f�[�^���擾����                         */
/*   ����    : ������|�C���^�A�f�[�^�|�C���^                         */
/*   �o��    : �l(data)                                               */
/*           : �G���[���i0:����A0�ȊO:�ُ�j                       */
/*                                                                    */
/**********************************************************************/
int GetData( char *buff , unsigned short * data )
{
int	i,len;
unsigned int sts;
	/* �I�y�����h�����񒷃`�F�b�N */
	len = strlen( buff );
	if( !len  ) return ILLOPR;
	if( len > MAXLEN ) return OPOFLOW;

	/* �擪����эŏI�̓R�[�e�[�V����(=������G���g��)�ł��� */
	if ( (( buff[0] == 0x22 ) && ( buff[len-1] == 0x22 ))
		|| (( buff[0] == 0x27 ) && ( buff[len-1] == 0x27 )) ){
		/* �����񒷂��R���S�ł��� */
		if( len == 3 ){
			/* ��ʂ͎b���16�r�b�g���l�Ƃ��� */
			*data = (unsigned short)(buff[1]&0xff);
			return NORM;
		}
		else{
			if( len == 4 ){
				/* ��ʂ͎b���16�r�b�g���l�Ƃ��� */
				*data = (unsigned short)( (buff[1]&0xff)|((buff[2]&0xff)<<8) );
				return NORM;
			}
			else return ILLOPR;
		}
	}
	/* �Q�i���w��ł��� */
	if ( (buff[0] == '&')&&((buff[1] == 'B')||(buff[1] == 'b')) ){
		for( i = 2 , sts = 0; i < len ; i++ ){
			/* 2�i�����グ */
			sts *= 2;
			/* ������`�F�b�N */
			switch ( buff[i] ){
			case '1':
				sts |= 0x1;
			case '0':
				break;
			default:
				return ILLOPR;
			}
		}
		/* �͈̓I�[�o�[�ł��� */
		if ( sts >= IM16bit ) return OFLOW;
		/* ����I�� */
		*data = (unsigned short )sts;
		return NORM;
	}
	/* �P�U�i���w��ł��� */
	if ( (buff[0] == '&')&&((buff[1] == 'H')||(buff[1] == 'h')) ){
		for( i = 2 ; i < len ; i++ ){
			/* ������`�F�b�N */
			if( !strchr( HexStr , (int)buff[i] ) ){
			 return ILLOPR;
			 }
		}
		/* �P�U�i���ɕϊ� */
#if __FORDOS
		if ( sscanf( &buff[2] , "%Lx" , &sts ) == EOF ) return ILLOPR;
#else
		if ( sscanf( &buff[2] , "%x" , &sts ) == EOF ) return ILLOPR;
#endif
		/* �͈̓I�[�o�[�ł��� */
		if ( sts >= IM16bit ) return OFLOW;
		/* ����I�� */
		*data = (unsigned short)sts;
		return NORM;
	}
	/* �P�O�i������ */
	for( i = 0 ; i < len ; i++ ){
		/* ������`�F�b�N */
		if( !strchr( DecStr , (int)buff[i] ) ) return ILLOPR;
	}
	/* �P�O�i���ɕϊ� */
#if __FORDOS
	if ( sscanf( buff , "%Ld" , &sts ) == EOF ) return ILLOPR;
#else
	if ( sscanf( buff , "%d" , &sts ) == EOF ) return ILLOPR;
#endif
	/* �͈̓I�[�o�[�ł��� */
	if ( sts >= IM16bit ) return OFLOW;

	/* ����I�� */
	*data = (unsigned short )sts;
	return NORM;
}

/**********************************************************************/
/*   CheckSetOpt : Check /SET Option Name                             */
/*                                                                    */
/*   ����    : /SET�I�v�V�������̓o�^�`�F�b�N�������s��               */
/*   ����    : �Ȃ�                                                   */
/*   �o��    : �G���[���i0:����A0�ȊO:�ُ�j                       */
/*                                                                    */
/**********************************************************************/
int CheckSetOpt( char * name )
{
int	i;
	/* ���x���o�^���`�F�b�N���� */
	for ( i = 0 ; i < SetLabel ; i++ ){
		/* �o�^���x���ƈ�v */
		if( !strcmp( SetTbl[i].ent , name ) ) return 0;
	}
	return 2;
}

/**********************************************************************/
/*   CheckSetLbl : Check /SET Option Name                             */
/*                                                                    */
/*   ����    : /SET�I�v�V�������̓o�^�`�F�b�N�������s��               */
/*   ����    : �Ȃ�                                                   */
/*   �o��    : �G���[���i0:����A0�ȊO:�ُ�j                       */
/*                                                                    */
/**********************************************************************/
int CheckSetLbl( char * name ,unsigned short sts )
{
int	i,ent,rc;
	ent = 0;
	/* ���x���o�^���`�F�b�N���� */
	for ( i = 0 ; i < SetLabel ; i++ ){
		/* �o�^���x���ƈ�v */
		if( !strcmp( SetTbl[i].ent , name ) ){
			ent++;
		}
		/* ������x���ƈ�v */
		if( !strcmp( SetTbl[i].let , name ) ){
			/* ���x���e�[�u���ɓo�^���� */
			if( rc = SetLabelTbl( SetTbl[i].ent , sts ,LBL_EQU ) ) return rc;
		}
	}
	/* �o�^���x���ƈ�v�����ꍇ�A���x���o�^������Ԃ� */
	if ( ent ) return 1;

	/* �Y���Ȃ� */
	return NORM;
}
/**********************************************************************/
/*   IncludeBin : Include BIN/BMP file                                */
/*                                                                    */
/*   ����    : Binary�t�H�[�}�b�g�̃t�@�C����OutBuf�ɓǂݏo��         */
/*   ����    : �o�b�t�@���ʒm�|�C���^                                 */
/*   �o��    : �G���[���i0:����A0�ȊO:�ُ�j                       */
/*                                                                    */
/**********************************************************************/
int IncludeBin( unsigned short *cnt ,char *File )
{
int rc,type;
char * fptr;
unsigned int Size = 0;

	/* �o�b�t�@�|�C���^������ */
	type =0;

	/* �g���q�ɂ��t�@�C����ʂ̔��� */
	if ((fptr = strrchr( File,  '.' ))){
		ChgCode( oprwk , fptr );
		/* BMP�t�@�C���ł��� */
		if ( !strcmp(".BMP",oprwk ) ){
			type = 1;
		}
	}
	/* �o�C�i���t�@�C��OPEN */
	if ( ( BinFD = fopen( File ,"rb" ) ) <= 0 ){
		printf("Invalid Binary File Name.\n");
		return ILLBIN;
	}

	if (!type){
		/* �o�C�i���ǂݏo�� */
		rc = ReadBin( &Size );
	}
	else{
		/* �r�b�g�}�b�v�ǂݏo�� */
		rc = ReadBmp( &Size );
	}

	/* �o�C�i���t�@�C���N���[�Y */
	fclose(BinFD);

	/* �R�[�h�T�C�Y���n�� */
	*cnt = (unsigned short)Size;
	return rc;
}
/**********************************************************************/
/*   ReadBin : Read BIN file                                          */
/*                                                                    */
/*   ����    : Binary�t�H�[�}�b�g�̃t�@�C����OutTbl.bcode�ɓǂݏo��   */
/*   ����    : �o�b�t�@���ʒm�|�C���^                                 */
/*   �o��    : �G���[���i0:����A0�ȊO:�ُ�j                       */
/*                                                                    */
/**********************************************************************/
int ReadBin( unsigned int * Size )
{
unsigned int BuffSize=0;	/* �o�̓o�b�t�@�T�C�Y */

	*Size = 0;
	/* �t�@�C���T�C�Y�擾 */
	if (fseek( BinFD , 0 , SEEK_END )){
		return ILLBIN;
	}
	BuffSize = (unsigned int)ftell( BinFD );
	if (fseek( BinFD , 0 , SEEK_SET )){
		return ILLBIN;
	}
	/* �o�b�t�@�T�C�Y�`�F�b�N */
	if ( BuffSize > ( MAXOBJ - AsmAdr )){
		/* �ǂݏo���T�C�Y�I�[�o�[ */
		return ADOFLOW;
	}
	/* 1 pass�ڂȂ�I�� */
	if ( !pass ){
		*Size = BuffSize;
		return NORM;
	}
	/* �o�̓o�b�t�@���m�ۂ��� */
	if ( !(OutTbl.bcode = malloc( (size_t)BuffSize ))){
		printf("Output Buffer Not Allocated.\n");
		return ILLBIN;
	}
	/* BIN�t�@�C���ǂ݂��� */
	if ( fread( OutTbl.bcode , (size_t)BuffSize , (size_t)1 , BinFD ) != 1 ){
		printf("File read error.\n");
		return ILLBIN;
	}

	/* �o�b�t�@���Z�b�g */
	*Size = BuffSize;
	return NORM;
}
/**********************************************************************/
/*   ReadBmp : Read Bitmap file                                       */
/*                                                                    */
/*   ����    : BMP�t�H�[�}�b�g�̃t�@�C����OutTbl.bcode�ɓǂݏo��      */
/*   ����    : �o�b�t�@���ʒm�|�C���^                                 */
/*   �o��    : �G���[���i0:����A0�ȊO:�ُ�j                       */
/*                                                                    */
/**********************************************************************/
int ReadBmp( unsigned int * Size )
{
unsigned char ImgTbl[62];
unsigned int bfsize;	/* �t�@�C���T�C�Y */
unsigned int boffset;	/* �摜�f�[�^�܂ł̃I�t�Z�b�g */
unsigned int bisize;	/* �w�b�_�̑傫�� */
unsigned int width;	/* �摜�̕� */
unsigned int width2;	/* �摜�̕�(���ۂ̃f�[�^�ʒu) */
unsigned int height;	/* �摜�̍��� */
unsigned int imgsize;	/* �摜�̃T�C�Y */
unsigned short bitcount;/* �F�� */
unsigned short parret1,parret2;/* �p���b�g�f�[�^ */
unsigned int x,y;
unsigned char * ImageTbl=0;/* �ǂݏo���p�o�b�t�@�|�C���^ */
unsigned int BuffSize=0;	/* �o�̓o�b�t�@�T�C�Y */
unsigned int ImgSize=0;	/* �摜�o�b�t�@�T�C�Y */

	*Size = 0;
	/* �C���[�W�e�[�u�������� */
	memset( ImgTbl , 0 , sizeof(ImgTbl) );
	/* BMP�t�@�C���w�b�_�ǂݏo��(62�o�C�g�Œ�) */
	if ( fread( ImgTbl , 1 , 62 , BinFD ) != 62 )
		goto bmp_read_error;
	/* BMP�w�b�_���� */
	if ( (ImgTbl[0] != 'B')||(ImgTbl[1] != 'M'))
		goto bmp_read_error;
	/* BMP�t�@�C���T�C�Y�擾 */
	bfsize = Getlong(2);
	/* �摜�f�[�^�܂ł̃I�t�Z�b�g�擾 */
	boffset = Getlong(10);
	/* �w�b�_�̑傫�� */
	bisize = Getlong(14);
	/* �w�b�_�T�C�Y�`�F�b�N */
	if ( (( bisize != 40 )&&( bisize != 12 ))||( bisize > boffset )||( bfsize <= boffset ) )
		goto bmp_read_error;
	/* BMP�p�����[�^�擾 */
	switch( bisize ){
	/* Windows�`���t�H�[�}�b�g�ł��� */
	case 40:
		width = Getlong(18);/* ���� */
		height = Getlong(22);/* �� */
		bitcount = Getword(28);/* �F�� */
		imgsize = Getlong(34);
		parret1 = Getparret(54);
		parret2 = ( (boffset-bisize-14) == 6 ) ? Getparret(57) : Getparret(58);
		break;
	/* OS2�`���t�H�[�}�b�g�ł��� */
	case 12:
		width = (unsigned int)Getword(18);/* ���� */
		height = (unsigned int)Getword(20);/* �� */
		bitcount = Getword(24);/* �F�� */
		imgsize = 0;
		parret1 = Getparret(26);
		parret2 = ( (boffset-bisize-14) == 6 ) ? Getparret(29) : Getparret(30);
		break;
	/* ���m�̃t�H�[�}�b�g�̏ꍇ�A�G���[�I�� */
	default:
		goto bmp_read_error;
	}
	/* ���m�N���摜�ł��� */
	if ( bitcount != 1 )
		goto bmp_read_error;
	/* �o�b�t�@�T�C�Y�쐬 */
	ImgSize = bfsize - boffset;

	/* �𑜓x�̃f�[�^��M�p���� */
	BuffSize = width * height / 8;

	/* �o�b�t�@�T�C�Y�`�F�b�N */
	if ( BuffSize > ( MAXOBJ - AsmAdr )){
		/* �ǂݏo���T�C�Y�I�[�o�[ */
		return ADOFLOW;
	}
	/* 1 pass�ڂȂ�I�� */
	if ( !pass ){
		*Size = BuffSize;
		return NORM;
	}
	/* �ǂݏo���e�[�u�����m�ۂ��� */
	if ( !(ImageTbl = malloc( (size_t)ImgSize ))){
		printf("Read Buffer Not Allocated.\n");
		return ILLBMP;
	}
	/* �ǂݏo���e�[�u�������� */
	memset( ImageTbl , 0xff , (size_t)ImgSize );
	/* Bitmap�f�[�^�ǂݏo�� */
	if ( fseek( BinFD , boffset , SEEK_SET ) )
		goto bmp_read_error;
	if ( fread( ImageTbl , (size_t)1 , (size_t)ImgSize , BinFD ) != ImgSize )
		goto bmp_read_error;

	/* �o�̓o�b�t�@���m�ۂ��� */
	if ( !( OutTbl.bcode = malloc( (size_t)BuffSize ))){
		printf("Output Buffer Not Allocated.\n");
		/* �ǂݏo���o�b�t�@�J�� */
		free( ImageTbl );
		return ILLBMP;
	}
	/* �o�b�t�@������ */
	memset( OutTbl.bcode, 0 , (size_t)BuffSize );
	/* width�␳ */
	width2 = ( width & 0x1f ) ? (width+32)/32*32 : width;
	/* BMP --> �o�C�i���ϊ� */
	for ( x = 0 ; x < width ; x++ ){
		for ( y = 0 ; y < height ; y++ ){
			/* �����𔻒肷�� */
			if (parret1 < parret2){
				/* if point(x,y) = 0 */
				if ( !(ImageTbl[(height-y-1)*(width2/8)+(x/8)]&((unsigned char)(0x80>>(x&7)))) )
					/* then pset(x,y) */
					OutTbl.bcode[x+((y/8)*width)] |= (unsigned char)(0x80>>(y&7));
			}
			else{
				/* if point(x,y) = 1 */
				if ( (ImageTbl[(height-y-1)*(width2/8)+(x/8)]&((unsigned char)(0x80>>(x&7)))) )
					/* then pset(x,y) */
					OutTbl.bcode[x+((y/8)*width)] |= (unsigned char)(0x80>>(y&7));
			}
		}
	}
	/* �ǂݏo���o�b�t�@�J�� */
	free( ImageTbl );
	*Size = BuffSize;
	return NORM;

bmp_read_error:
	/* �ǂݏo���o�b�t�@�J�� */
	if (ImageTbl) free( ImageTbl );
	return ILLBMP;
}

/**********************************************************************/
/*   PrintList : Print List File                                      */
/*                                                                    */
/*   ����    : OutTbl�ɏ]���A�P�s���̃��X�g���o�͂���                 */
/*   ����    : �s�ԍ��J�E���^                                         */
/*   �o��    : �Ȃ�                                                   */
/*                                                                    */
/**********************************************************************/
void PrintList( int count )
{
int	i,cnt,n,len;
unsigned short sts;
	/* ���X�g�o�͗v�����Ȃ��ꍇ�A�����I�� */
	if ( ListFlag ) return;
	len = 0;
	cnt = 0;
	/* �s�ԍ��o�� */
	if (pr) printf( "%05d:%04X  " , count+1 , OutTbl.adr );
	fprintf( LstFD ,"%05d:%04X  " , count+1 , OutTbl.adr );

	/* �ʏ햽�߁^�^�����ߎ�ʎ��o�� */
	sts = OutTbl.kind;
	/* ORG/#INCBIN���߈ȊO�͖��ߌ�R�[�h�o�� */
	if (OutTbl.byte && ( sts != OP_ORG )&&( sts != OP_INCBIN ) ){
		/* �f�[�^���� */
		for ( cnt = 0 ; cnt < 4 ; cnt++ ){
			if ( cnt < OutTbl.byte){
				/* DS���ߎ��́A0���o�͂��� */
				if ( sts == OP_DS ){
					if (pr) printf( "00" );
					fprintf( LstFD ,"00" );
				}
				 else{
					if (pr) printf( "%02X" , OutTbl.code[cnt] );
					fprintf( LstFD ,"%02X" , OutTbl.code[cnt] );
				}
			}
			else{
				if (pr) printf( "  " );
				if (!Tab) fprintf( LstFD ,"  " );
			}
		}
		if (pr) printf( "  " );
		if (!Tab) fprintf( LstFD ,"  " );
		else fprintf( LstFD ,"%s", ( ( OutTbl.byte & 0x3 ) == 1) ? "\t\t" : "\t" );
	}
	else{
		/* �f�[�^�Ȃ� */
		if ( sts != OP_INCBIN ) cnt = OutTbl.byte;
		if (pr) printf( "          " );
		if ( !Tab ) fprintf( LstFD ,"          " );
		else fprintf( LstFD ,"\t\t" );
	}
	/* ���x���R�[�h�o�� */
	if ( OutTbl.label ){
		sprintf( oprwk , "%s:" , OutTbl.label );
		if (pr) printf("%-13s" , oprwk );
		if (!Tab) fprintf( LstFD ,"%-13s" , oprwk );
		else{
			fprintf( LstFD ,"%s" , oprwk );
			if ( strlen(oprwk) < 8 )
				fprintf( LstFD ,"\t" );
		}
	}
	else{
		if ( OutTbl.opr[0] ){
			/* �󔒏o�� */
			if (pr) printf( "             " );
			if ( !Tab )	fprintf( LstFD ,"             " );
			else fprintf( LstFD ,"\t" );
		}
	}
	/* �j���j�b�N�o�� */
	if ( OutTbl.opr[0] ){
		if (pr) printf( "%-08s" , OutTbl.opr[0] );
		if (!Tab) fprintf( LstFD ,"%-08s" , OutTbl.opr[0] );
		else{
			fprintf( LstFD ,"%s" , OutTbl.opr[0] );
			if ( strlen(OutTbl.opr[0]) < 8 )
				fprintf( LstFD ,"\t" );
		}
		len += 8;
	}
	/* �I�y�����h�o�� */
	i = 1;
	while( OutTbl.opr[i] ){
		if (pr) printf( "%s" , OutTbl.opr[i] );
		fprintf( LstFD ,"%s" , OutTbl.opr[i] );
		len += strlen(OutTbl.opr[i]);
		i++;
	}
	/* �R�����g�o�� */
	if ( OutTbl.comment ){
		if (pr) printf( "%s" , OutTbl.comment );
		/* �󔒏o�� */
		if ( len ){
			if (!Tab){
				n = ( len > 30 ) ? 0 : ( 30 - len );
				for ( i = 0 ; i < n ; i++ ) fprintf( LstFD ," " );
			}
			else{
				n = ( len > 31 ) ? 0 : ( 4 - (len/8) );
				for ( i = 0 ; i < n ; i++ ) fprintf( LstFD ,"\t" );
			}
		}
		fprintf( LstFD ,"%s" , OutTbl.comment );
	}
	/* ���s */
	if (pr) printf( "\n" );
	fprintf( LstFD ,"\n" );
	/* ���ߌ�R�[�h�o�� */
	while( OutTbl.byte && ( cnt < OutTbl.byte ) ){
		if (pr) printf("            ");
		fprintf(LstFD , "            ");
		/* �f�[�^���� */
		for ( i = 0 ; i < 4 ; i++ , cnt++ ){
			if ( cnt < OutTbl.byte){
				/* DS���ߎ��́A0���o�͂��� */
				if ( sts == OP_DS ){
					if (pr) printf( "00" );
					fprintf( LstFD ,"00" );
				}
				 else{
				 	if ( sts != OP_INCBIN ){
						if (pr) printf( "%02X" , OutTbl.code[cnt] );
						fprintf( LstFD ,"%02X" , OutTbl.code[cnt] );
					}
					else{
						if (pr) printf( "%02X" , OutTbl.bcode[cnt] );
						fprintf( LstFD ,"%02X" , OutTbl.bcode[cnt] );
					}
				}
			}
			else{
				if (pr) printf( "  " );
				if (!Tab) fprintf( LstFD ,"  " );
			}
		}
		/* TAB�o�� */
		if ( Tab ) {
			fprintf( LstFD ,"\t");
			if ( ( OutTbl.byte & 0x3 ) == 1 ) fprintf( LstFD ,"\t");
		}
		/* #incbin�j���j�b�N�o�� */
		if ( sts == OP_INCBIN ){
			/* DB���ߏo�� */
			if (pr) printf( "               DB      " );
			if ( !Tab ) fprintf( LstFD ,"               DB      " );
			else fprintf( LstFD ,"\tDB\t" );
			for ( n = cnt-i ; n < cnt ; n++){
				if ( n < OutTbl.byte){
					if (pr) printf( "&H%02X" , OutTbl.bcode[n] );
					fprintf( LstFD ,"&H%02X" , OutTbl.bcode[n] );
					/* �J���}�o�� */
					if ( ( n < cnt-1 )&&( n < OutTbl.byte-1) ){
						if (pr) printf( "," );
						fprintf( LstFD ,"," );
					}
				}
			}
		}
		/* ���s */
		if (pr) printf( "\n" );
		fprintf( LstFD ,"\n" );
	}
	/* EJECT�v������ */
	if ( LineFeed ){
		/* Line Feed(0xc)�o�� */
		fprintf( LstFD ,"%c", 0xc );
		/* �w�b�_�o�� */
		fprintf( LstFD , "%s%s - ",name,rev );
		fprintf( LstFD ,"ASSEMBLY LIST OF [%s]\n", SrcFile );
		LineFeed = 0;
	}
}

/**********************************************************************/
/*   ErrPut : Error Print                                             */
/*                                                                    */
/*   ����    : �G���[���b�Z�[�W���o�͂���                             */
/*   ����    : �s�ԍ��J�E���^�A�G���[�ԍ�                             */
/*   �o��    : �Ȃ�                                                   */
/*                                                                    */
/**********************************************************************/
void ErrPut( int cnt , int err )
{
int	i;
	/* �G���[�����s�̏o�� */
	pr = 1;
	ListFlag = 0;
	PrintList( cnt );

	/* ���̓R�[�h�ɊY������G���[���b�Z�[�W���o�� */
	for ( i = 0 ; i < MAXERR-1 ; i++ ){
		if ( errtbl[i].code==err )	break;
	}
	printf("LINE:%d ERROR CODE:%03d %s\n" , cnt+1, err , errtbl[i].msg );
	fprintf( LstFD ,"LINE:%d ERROR CODE:%03d %s\n" , cnt+1, err , errtbl[i].msg );
}

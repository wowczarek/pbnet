/********************************************************************/
/*                                                                  */
/*  NAME : HD61700 ASSEMBLER HEADDER CODE                           */
/*  FILE : hd61700.h                                                */
/*                                                                  */
/*  REVISION HISTORY:                                               */
/*  Rev : 0.01 2003.01.06  最初のバージョン                         */
/*  Rev : 0.03 2003.01.09  未定義命令を追加                         */
/*  Rev : 0.04 2003.01.16  未定義命令を追加                         */
/*  Rev : 0.05 2003.01.18  未定義命令を追加                         */
/*  Rev : 0.07 2003.02.02  オペコード0x0～0xf､0x80～0x8fに対して    */
/*                         Jump拡張をイネーブルとした。             */
/*  Rev : 0.08 2003.02.06  演算時のエラーメッセージ追加             */
/*             2003.02.07  OutTbl構造体を修正                       */
/*             2003.02.09  命令語サポート追加                       */
/*  Rev : 0.10 2003.02.21  エラーメッセージ追加/修正                */
/*                         命令コード(0xDA,0xDB)に対してJump拡張を  */
/*                         許可                                     */
/*  Rev : 0.12 2003.02.28  ラベル長を最大16バイトとする             */
/*  Rev : 0.13 2003.03.04  命令語追加。                             */
/*  Rev : 0.14 2003.03.05  内蔵レジスタ/フラグテーブル追加          */
/*  Rev : 0.15 2003.03.07  命令語追加                               */
/*  Rev : 0.16 2003.03.14  命令語追加                               */
/*  Rev : 0.19 2003.04.09  JR ±IM7形式のサポート                   */
/*  Rev : 0.20 2003.04.22  エラーコード/メッセージ追加              */
/*  Rev : 0.21 2003.04.25  PST TS,xx,GST TS,$ を追加。              */
/*  Rev : 0.22 2003.04.26  命令コード(0xD2～0xDB)のJump拡張を禁止   */
/*                          (DIDM,DIUM,BYDM,BYUM,INVM,CMPM)         */
/*  Rev : 0.23 2003.05.15  /SETオプション用構造体を追加             */
/*  Rev : 0.34 2006.06.12  #INCBIN擬似命令を追加。                  */
/*             2006.08.27  PSR/GSR命令群/GPOW/GFLWを追加。          */
/*             2006.08.30  #defineを変更、命令テーブルを修正。      */
/*             2006.09.01  JP ($C5)->JP $C5へ修正。                 */
/*                         DFHへJP ($C5)を追加。                    */
/*  Rev : 0.35 2006.09.09  ニモニックを変更(SNL->LDL)               */
/*  Rev : 0.36 2006.09.25  エラーメッセージを修正/OUTTBL構造体変更  */
/*                         #KC,#AI疑似命令を追加。                  */
/*  Rev : 0.37 2006.09.29  GRE KY,$C5 のセカンドオペレーション      */
/*                         コードを修正(20H→60H)                   */
/*  Rev : 0.39 2006.11.08  STLW/LDLW/PPOWの種別テーブルを修正       */
/*  Rev : 0.40 2007.03.15  レジスタ名称を修正(TS→IB)               */
/*                         リロケート情報出力用フラグを追加         */
/*  Rev : 0.41 2008.05.05  Europe式(casio original?)ニモニック追加  */
/*                         (CS,#EU,#0,#1,#2,PRA,GRA)                */
/*  Rev : 0.42 2011.01.03  UNDEFOPR定義値変更                       */
/*                         OUTTBL構造体メンバー(kind)追加           */
/*  Rev : 0.44 2021.07.09  EOFオプションを無効化(DOS版は変更なし)   */
/*                         DL-PASCAL用のinline形式出力を追加        */
/*                                                                  */
/********************************************************************/
/*------------------------------------------------------------------*/
/*  コンパイルスイッチ定義                                          */
/*------------------------------------------------------------------*/
#define __FORDOS  		0			/* 1:DOS用にコンパイルする      */
#if __FORDOS
 #define __WITH_EOF		1			/* 1:出力ファイルにEOFを付ける  */
#endif /*__FORDOS */
#define __WORD_ADDRESS	1			/* 1:16bit addressing 許可      */
#define __SJIS_SPACE	1			/* 1:全角スペースを無視する     */
#define __EUR_MNEMONIC	1			/* 1:Europe形式ニモニック追加   */
#define __DLP_INLINE	1			/* 1:DL-Pascal inline形式追加   */
#define __DEBUG			0			/* 1:for Debug                  */
/*------------------------------------------------------------------*/
/*  定数定義                                                        */
/*------------------------------------------------------------------*/
#define		IM3bit		8			/* ３ビット長最大値             */
#define		IM5bit		32			/* ５ビット長最大値             */
#define		IM7bit		128			/* ７ビット長最大値             */
#define		IM8bit		256			/* ８ビット長最大値             */
#define		IM16bit		65536		/* １６ビット長最大値           */
#define		MAXNAME		16			/* ラベル最大長                 */
#define		MAXLEN		256			/* オペランド最大長             */
#define		MAXOPR		128			/* オペランド総数               */
#define		MAXOBJ		65536		/* 最大ファイルサイズ           */
#define		FILEEND		2			/* ファイル終了                 */
#define		MAXLINE		256			/* １行最大バイト数             */
#define		MAXINC		256			/* 最大インクルードネスト数     */
#define		MAXOPT		8			/* コマンドラインオプション数   */
/*------------------------------------------------------------------*/
/*  ラベル情報定義                                                  */
/*------------------------------------------------------------------*/
#define		LBLMASK		0x000f		/* ラベル種別マスク値           */
/*------------------------------------------------------------------*/
/*  レジスタ／オペランド種別定義（第１、第２オペランド共用）        */
/*------------------------------------------------------------------*/
#define		MASKOP		0xff88		/* オペランド検索用マスク値     */
#define		MASKOP2		0xfff8		/* オペランド検索用マスク値     */
/*------------------------------------------------------------------*/
#define		IM3			0x0008		/* イミディエイト指定(3bit)     */
#define		IM5			0x0009		/* イミディエイト指定(5bit)     */
#define		IM7			0x000a		/* イミディエイト指定(7bit)     */
#define		IM8			0x000b		/* イミディエイト指定(8bit)     */
#define		IM16		0x000c		/* イミディエイト指定(16bit)    */
#define		LBLOK		0x000d		/* 種別ラベル アドレス解決済み  */
#define		LBLNG		0x000e		/* 種別ラベル アドレス未解決    */
#define		REG			0x0080		/* レジスタ指定 $0～$31         */
#define		RSZ			0x0090		/* SIRレジスタ指定 $(SZ) = $0   */
#define		RSY			0x00a0		/* SIRレジスタ指定 $(SY) = $30  */
#define		RSX			0x00b0		/* SIRレジスタ指定 $(SX) = $31  */
#define		MREG		0x0100		/* インデックス指定($1)～($31)  */
#define		MRSZ		0x0110		/* インデックス指定($(SZ))      */
#define		MRSY		0x0120		/* インデックス指定($(SY))      */
#define		MRSX		0x0130		/* インデックス指定($(SX))      */
#define		MIX_p		0x0200		/* インデックス指定(IX+  )      */
#define		MIX_m		0x0300		/* インデックス指定(IX-  )      */
#define		MIZ_p		0x0400		/* インデックス指定(IZ+  )      */
#define		MIZ_m		0x0500		/* インデックス指定(IZ-  )      */
/*------------------------------------------------------------------*/
/*  レジスタ／オペランド種別定義(第１オペランドのみ)                */
/*------------------------------------------------------------------*/
#define		PE			0x0600		/* PE レジスタ                  */
#define		PD			0x0700		/* PD レジスタ                  */
#define		IB			0x0800		/* IB レジスタ                  */
#define		UA			0x0900		/* UA レジスタ                  */
#define		IA			0x0a00		/* IA レジスタ                  */
#define		IE			0x0b00		/* IE レジスタ                  */
#define		TM			0x0c00		/* TM レジスタ                  */
#define		KY			0x0d00		/* KY レジスタ                  */
#define		US			0x0e00		/* US レジスタ                  */
#define		SS			0x0f00		/* SS レジスタ                  */
#define		IX			0x1000		/* IX レジスタ                  */
#define		IY			0x1100		/* IY レジスタ                  */
#define		IZ			0x1200		/* IZ レジスタ                  */
#define		SX			0x1300		/* SX レジスタ                  */
#define		SY			0x1400		/* SY レジスタ                  */
#define		SZ			0x1500		/* SZ レジスタ                  */
/*------------------------------------------------------------------*/
/*  フラグ／オペランド種別定義(第１オペランドのみ)                  */
/*------------------------------------------------------------------*/
#define		ZF			0x1500		/* Z  フラグ                    */
#define		LZF			0x1600		/* LZ フラグ                    */
#define		UZF			0x1700		/* UZ フラグ                    */
#define		NZF			0x1800		/* NZ フラグ                    */
#define		CF			0x1900		/* C  フラグ                    */
#define		NCF			0x1a00		/* NC フラグ                    */
#define		NLZF		0x1b00		/* NLZ フラグ                   */

/*------------------------------------------------------------------*/
/*  命令種別定義（オペレーション種別判別用）                        */
/*------------------------------------------------------------------*/
enum{
	NU		=	0x0	,	/* オペランドなし／1 byte 命令  */
	REG_NU			,	/* REG        (２バイト命令）   */
	REG_NUJ			,	/* REG        (３バイト命令）   */
	IR_REG			,	/* IREG：REG  (２バイト)        */
	IR_REGJ			,	/* IREG：REG : IM7 (3バイト)    */
	IR_IM5			,	/* IREG：IM5  (２バイト)        */
	IR_IM8			,	/* IREG：IM8  (３バイト)        */
	REG_REG			,	/* REG ：REG  (３バイト)        */
	MREG_REG		,	/* REG ：REG  (３バイト)        */
	REG_IM3			,	/* REG ：IM3  (２バイト)        */
	REG2_IM3		,	/* REG ：REG :IM3  (３バイト)   */
	REG2_IM3J		,	/* REG ：REG :IM3 :IM7(4バイト) */
	REG_IM7			,	/* REG ：IM7  (３バイト)        */
	REG_IM8			,	/* REG ：IM8  (３バイト)        */
	REG2_IM7		,	/* REG : REG ：IM8  (４バイト)  */
	IM8_REG			,	/* IM8 ：REG  (３バイト)        */
	IR_RIM3			,	/* IREG：REG :IM3 (３バイト)    */
	IR_IM16			,	/* IREG：IM16 (４バイト)        */
	IM7_NU			,	/* 数値(7bit):なし JR命令       */
	F_IM7			,	/* フラグ:数値(7bit) JR命令     */
	F_IM16			,	/* フラグ:数値(16bit)JP/CAL命令 */
	IM8_NU			,	/* 数値(16bit):なし JR/CAL命令  */
	IM16_NU			,	/* 数値(16bit):なし JR/CAL命令  */
	IM16_REG		,	/* IM16 : REG (４バイト)        */
	REG_IM16		,	/* REG ：IM16 (４バイト)        */
	NU_IM8			,	/* NU : IM8（３バイト) PFLのみ? */
	REG_I5I3		,	/* REG ：IM5 :IM3  (３バイト)   */
	REG_I5I3J			/* REG ：IM5 :IM3 :IM7(4バイト) */
};

/*------------------------------------------------------------------*/
/*  構文解析結果出力テーブル構造体定義（OutTbl）                    */
/*------------------------------------------------------------------*/
typedef struct outtbl {
	unsigned short kind;			/* ニモニック種別               */
	unsigned short idx;				/* codetblインデックス          */
	unsigned short adr;				/* 命令格納アドレス             */
	unsigned short byte;			/* 命令バイト数(0:コメントのみ) */
	unsigned char  code[256]; 		/* 命令コード                   */
	unsigned short sts;				/* 戻りステータス               */
	unsigned short si;				/* 特定Indexフラグ(1:Indexあり) */
	unsigned short kc;				/* KC方式対応フラグ(1:KC方式)   */
	unsigned short pm7;				/* ±IM7形式対応フラグ(1:+,2:-) */
	unsigned short opkind[4];		/* オペランド種別               */
	unsigned short opdata[4];		/* オペランド番号(ﾚｼﾞｽﾀ/数値)   */
	unsigned char	*bcode;			/* #INCBIN用コードポインタ      */
	char	*label; 				/* 先頭ラベル(NULL:登録なし)    */
	char	*opr[MAXOPR];			/* オペランド(NULL:登録なし)    */
	char	*comment;				/* コメント(NULL:登録なし)      */
	char	line[MAXLINE*2];		/* 入力バッファ(512バイト)      */
} OUTTBL;

/*------------------------------------------------------------------*/
/*  ラベルテーブル構造体定義（lbltbl）                              */
/*------------------------------------------------------------------*/
#define LBL_EQU		0				/* EQUによる定義                */
#define LBL_ADR		1				/* アドレスによる定義           */
typedef struct lbl {
	void	* np;					/* 次ラベル構造体ポインタ       */
	unsigned short	adr;			/* 対応アドレス                 */
	unsigned char	flag;			/* ラベル種別(0:EQU,1:アドレス) */
	char	name[1];				/* ラベル名称(1Byte以上)        */
} LBL;

/*------------------------------------------------------------------*/
/*  各種データ変換テーブル構造体定義                                */
/*------------------------------------------------------------------*/
/* オペランド変換テーブル構造 */
typedef struct opr {
	unsigned short code;			/* オペランドコード番号         */
	char	name[9];				/* オペランド名称               */
} OPR;

/* メモリオペランド種別テーブル */
#define		MAXIDX		12
struct opr moprtbl[MAXIDX] = {
{	MIX_p	,	"(IX+"		},
{	MIX_p	,	"(Ix+"		},
{	MIX_p	,	"(ix+"		},
{	MIX_m	,	"(IX-"		},
{	MIX_m	,	"(Ix-"		},
{	MIX_m	,	"(ix-"		},
{	MIZ_p	,	"(IZ+"		},
{	MIZ_p	,	"(Iz+"		},
{	MIZ_p	,	"(iz+"		},
{	MIZ_m	,	"(IZ-"		},
{	MIZ_m	,	"(Iz-"		},
{	MIZ_m	,	"(iz-"		}
};

/* レジスタオペランド種別テーブル */
	#define		MAXIREG		16
#if __EUR_MNEMONIC
	#define		IREGADD		4
#else
	#define		IREGADD		0
#endif /* __EUR_MNEMONIC */
struct opr regtbl[MAXIREG+IREGADD] = {
{	PE		,	"PE"		},
{	PD		,	"PD"		},
{	IB		,	"IB"		},
{	UA		,	"UA"		},
{	IA		,	"IA"		},
{	IE		,	"IE"		},
{	TM		,	"TM"		},
{	KY		,	"KY"		},
{	US		,	"US"		},
{	SS		,	"SS"		},
{	IX		,	"IX"		},
{	IY		,	"IY"		},
{	IZ		,	"IZ"		},
{	SX		,	"SX"		},
{	SY		,	"SY"		},
{	SZ		,	"SZ"		}
#if __EUR_MNEMONIC
,{	IB		,	"CS"		},
{	SX		,	"#0"		},
{	SY		,	"#1"		},
{	SZ		,	"#2"		}
#endif /* __EUR_MNEMONIC */
};

/* フラグ種別テーブル */
#define		MAXFLAG		8
struct opr flagtbl[MAXFLAG] = {
{	ZF		,	"Z"			},
{	LZF		,	"LZ"		},
{	UZF		,	"UZ"		},
{	NZF		,	"NZ"		},
{	CF		,	"C"			},
{	NCF		,	"NC"		},
{	NLZF	,	"LNZ"		},
{	NLZF	,	"NLZ"		}
};

/* 疑似命令種別 */
enum {
	OP_EQU		=	0	,	/* EQU      */
	OP_ORG				,	/* ORG      */
	OP_START			,	/* START    */
	OP_DS				,	/* DS       */
	OP_DB				,	/* DB       */
	OP_DW				,	/* DW       */
	OP_LEVEL			,	/* LEVEL    */
	OP_INCLUDE			,	/* #INCLUDE */
	OP_LIST				,	/* #LIST    */
	OP_NOLIST			,	/* #NOLIST  */
	OP_EJECT			,	/* #EJECT   */
	OP_INCBIN			,	/* #INCBIN  */
	OP_AI				,	/* #AI      */
	OP_KC					/* #KC      */
#if __EUR_MNEMONIC
	,OP_EU					/* #EU      */
#endif /* __EUR_MNEMONIC */
};
/* 疑似命令変換テーブル */
#if __EUR_MNEMONIC
	#define		MAXDIR		15
#else
	#define		MAXDIR		16
#endif /* __EUR_MNEMONIC */
struct opr Directivetbl[MAXDIR] = {
{	OP_EQU		,	"EQU"		},
{	OP_ORG		,	"ORG"		},
{	OP_START	,	"START"		},
{	OP_DS		,	"DS"		},
{	OP_DB		,	"DB"		},
{	OP_DW		,	"DW"		},
{	OP_LEVEL	,	"LEVEL"		},
{	OP_INCLUDE	,	"#INCLUDE"	},
{	OP_LIST		,	"#LIST"		},
{	OP_NOLIST	,	"#NOLIST"	},
{	OP_EJECT	,	"#EJECT"	},
{	OP_INCBIN	,	"#INCBIN"	},
{	OP_AI		,	"#AI"		},
{	OP_KC		,	"#KC"		}
#if __EUR_MNEMONIC
,{	OP_EU		,	"#EU"		}
#endif /* __EUR_MNEMONIC */
};

/* #if～#else～#endifマクロ変換テーブル */
#define		IFLEVEL     255         /* #ifネストレベル */
enum {
	OP_IF		=	0	,	/* #if   */
	OP_ELSE				,	/* #else */
	OP_ENDIF				/* #endif*/
};
#define		MACDIR		3
struct opr MacTbl[MACDIR] = {
{	OP_IF		,	"#IF"		},
{	OP_ELSE		,	"#ELSE"		},
{	OP_ENDIF	,	"#ENDIF"	}
};

#define	MAXMN		6				/* ニモニック最大長(6文字) */
/* 内部レジスタ/フラグ使用命令登録テーブル構造 */
typedef struct irfnc {
	char	name[MAXMN];
} IRFNC;

/* 内部レジスタ使用命令登録テーブル */
#define	MAXIR		9				/* 内部レジスタ利用命令数 */
struct irfnc irtbl[MAXIR] = {
{"PST"},{"GST"},{"PRE"},{"GRE"},{"PSR"},{"GSR"},{"PSRW"},{"GSRW"},{"PSRM"}
};
#if __EUR_MNEMONIC
/* 内部レジスタ使用命令登録テーブル(DL-Pascal形式用) */
struct irfnc irtblC[MAXIR] = {
{"PST"},{"GST"},{"PRE"},{"GRE"},{"PRA"},{"GRA"},{"PRAW"},{"GRAW"},{"PRAL"}
};
#endif /* __EUR_MNEMONIC */

/* フラグ使用命令登録テーブル */
#define	MAXFL		4				/* フラグレジスタ利用命令数 */
struct irfnc fltbl[MAXFL] = {
{"JR"},{"JP"},{"CAL"},{"RTN"}
};

/* 命令変換テーブル構造 */
typedef struct fnc {
	unsigned char	code;			/* 命令コード番号               */
	unsigned char	secop;			/* セカンド命令コード番号       */
	unsigned char	byte;			/* 命令バイト数                 */
	unsigned char	kind;			/* 命令語種別(メモリ格納情報)   */
	char			name[MAXMN];	/* ニモニック名称               */
	unsigned short	op1;			/* オペランド１種別             */
	unsigned short	op2;			/* オペランド２種別             */
	unsigned short	op3;			/* オペランド３種別             */
	unsigned short	op4;			/* オペランド４種別             */
} FNC;

/* 命令変換テーブル */
#define MAXOP		1125			/* 登録命令数                   */
struct fnc codetbl[MAXOP] = {
/*	opcode	,second	,byte,	kind	,func	,op1		,op2		,op3	,op4	*/
{	0x00	,0		,2	,REG_NU		,"ADC"	,REG		,RSX		,0		,0		},
{	0x00	,0x20	,2	,REG_NU		,"ADC"	,REG		,RSY		,0		,0		},
{	0x00	,0x40	,2	,REG_NU		,"ADC"	,REG		,RSZ		,0		,0		},
{	0x00	,0x60	,3	,REG_REG	,"ADC"	,REG		,REG		,0		,0		},
{	0x00	,0x80	,3	,REG_IM7	,"ADC"	,REG		,RSX		,IM7	,0		},
{	0x00	,0xa0	,3	,REG_IM7	,"ADC"	,REG		,RSY		,IM7	,0		},
{	0x00	,0xc0	,3	,REG_IM7	,"ADC"	,REG		,RSZ		,IM7	,0		},
{	0x00	,0xe0	,4	,REG2_IM7	,"ADC"	,REG		,REG		,IM7	,0		},
{	0x01	,0		,2	,REG_NU		,"SBC"	,REG		,RSX		,0		,0		},
{	0x01	,0x20	,2	,REG_NU		,"SBC"	,REG		,RSY		,0		,0		},
{	0x01	,0x40	,2	,REG_NU		,"SBC"	,REG		,RSZ		,0		,0		},
{	0x01	,0x60	,3	,REG_REG	,"SBC"	,REG		,REG		,0		,0		},
{	0x01	,0x80	,3	,REG_IM7	,"SBC"	,REG		,RSX		,IM7	,0		},
{	0x01	,0xa0	,3	,REG_IM7	,"SBC"	,REG		,RSY		,IM7	,0		},
{	0x01	,0xc0	,3	,REG_IM7	,"SBC"	,REG		,RSZ		,IM7	,0		},
{	0x01	,0xe0	,4	,REG2_IM7	,"SBC"	,REG		,REG		,IM7	,0		},
{	0x02	,0		,2	,REG_NU		,"LD"	,REG		,RSX		,0		,0		},
{	0x02	,0x20	,2	,REG_NU		,"LD"	,REG		,RSY		,0		,0		},
{	0x02	,0x40	,2	,REG_NU		,"LD"	,REG		,RSZ		,0		,0		},
{	0x02	,0x60	,3	,REG_REG	,"LD"	,REG		,REG		,0		,0		},
{	0x02	,0x80	,3	,REG_IM7	,"LD"	,REG		,RSX		,IM7	,0		},
{	0x02	,0xa0	,3	,REG_IM7	,"LD"	,REG		,RSY		,IM7	,0		},
{	0x02	,0xc0	,3	,REG_IM7	,"LD"	,REG		,RSZ		,IM7	,0		},
{	0x02	,0xe0	,4	,REG2_IM7	,"LD"	,REG		,REG		,IM7	,0		},
{	0x03	,0		,2	,REG_NU		,"LDC"	,REG		,RSX		,0		,0		},/* NOP instruction */
{	0x03	,0x20	,2	,REG_NU		,"LDC"	,REG		,RSY		,0		,0		},/* NOP instruction */
{	0x03	,0x40	,2	,REG_NU		,"LDC"	,REG		,RSZ		,0		,0		},/* NOP instruction */
{	0x03	,0x60	,3	,REG_REG	,"LDC"	,REG		,REG		,0		,0		},/* NOP instruction */
{	0x03	,0x80	,3	,REG_IM7	,"LDC"	,REG		,RSX		,IM7	,0		},/* NOP instruction */
{	0x03	,0xa0	,3	,REG_IM7	,"LDC"	,REG		,RSY		,IM7	,0		},/* NOP instruction */
{	0x03	,0xc0	,3	,REG_IM7	,"LDC"	,REG		,RSZ		,IM7	,0		},/* NOP instruction */
{	0x03	,0xe0	,4	,REG2_IM7	,"LDC"	,REG		,REG		,IM7	,0		},/* NOP instruction */
{	0x04	,0		,2	,REG_NU		,"ANC"	,REG		,RSX		,0		,0		},
{	0x04	,0x20	,2	,REG_NU		,"ANC"	,REG		,RSY		,0		,0		},
{	0x04	,0x40	,2	,REG_NU		,"ANC"	,REG		,RSZ		,0		,0		},
{	0x04	,0x60	,3	,REG_REG	,"ANC"	,REG		,REG		,0		,0		},
{	0x04	,0x80	,3	,REG_IM7	,"ANC"	,REG		,RSX		,IM7	,0		},
{	0x04	,0xa0	,3	,REG_IM7	,"ANC"	,REG		,RSY		,IM7	,0		},
{	0x04	,0xc0	,3	,REG_IM7	,"ANC"	,REG		,RSZ		,IM7	,0		},
{	0x04	,0xe0	,4	,REG2_IM7	,"ANC"	,REG		,REG		,IM7	,0		},
{	0x05	,0		,2	,REG_NU		,"NAC"	,REG		,RSX		,0		,0		},
{	0x05	,0x20	,2	,REG_NU		,"NAC"	,REG		,RSY		,0		,0		},
{	0x05	,0x40	,2	,REG_NU		,"NAC"	,REG		,RSZ		,0		,0		},
{	0x05	,0x60	,3	,REG_REG	,"NAC"	,REG		,REG		,0		,0		},
{	0x05	,0x80	,3	,REG_IM7	,"NAC"	,REG		,RSX		,IM7	,0		},
{	0x05	,0xa0	,3	,REG_IM7	,"NAC"	,REG		,RSY		,IM7	,0		},
{	0x05	,0xc0	,3	,REG_IM7	,"NAC"	,REG		,RSZ		,IM7	,0		},
{	0x05	,0xe0	,4	,REG2_IM7	,"NAC"	,REG		,REG		,IM7	,0		},
{	0x06	,0		,2	,REG_NU		,"ORC"	,REG		,RSX		,0		,0		},
{	0x06	,0x20	,2	,REG_NU		,"ORC"	,REG		,RSY		,0		,0		},
{	0x06	,0x40	,2	,REG_NU		,"ORC"	,REG		,RSZ		,0		,0		},
{	0x06	,0x60	,3	,REG_REG	,"ORC"	,REG		,REG		,0		,0		},
{	0x06	,0x80	,3	,REG_IM7	,"ORC"	,REG		,RSX		,IM7	,0		},
{	0x06	,0xa0	,3	,REG_IM7	,"ORC"	,REG		,RSY		,IM7	,0		},
{	0x06	,0xc0	,3	,REG_IM7	,"ORC"	,REG		,RSZ		,IM7	,0		},
{	0x06	,0xe0	,4	,REG2_IM7	,"ORC"	,REG		,REG		,IM7	,0		},
{	0x07	,0		,2	,REG_NU		,"XRC"	,REG		,RSX		,0		,0		},
{	0x07	,0x20	,2	,REG_NU		,"XRC"	,REG		,RSY		,0		,0		},
{	0x07	,0x40	,2	,REG_NU		,"XRC"	,REG		,RSZ		,0		,0		},
{	0x07	,0x60	,3	,REG_REG	,"XRC"	,REG		,REG		,0		,0		},
{	0x07	,0x80	,3	,REG_IM7	,"XRC"	,REG		,RSX		,IM7	,0		},
{	0x07	,0xa0	,3	,REG_IM7	,"XRC"	,REG		,RSY		,IM7	,0		},
{	0x07	,0xc0	,3	,REG_IM7	,"XRC"	,REG		,RSZ		,IM7	,0		},
{	0x07	,0xe0	,4	,REG2_IM7	,"XRC"	,REG		,REG		,IM7	,0		},
{	0x08	,0		,2	,REG_NU		,"AD"	,REG		,RSX		,0		,0		},
{	0x08	,0x20	,2	,REG_NU		,"AD"	,REG		,RSY		,0		,0		},
{	0x08	,0x40	,2	,REG_NU		,"AD"	,REG		,RSZ		,0		,0		},
{	0x08	,0x60	,3	,REG_REG	,"AD"	,REG		,REG		,0		,0		},
{	0x08	,0x80	,3	,REG_IM7	,"AD"	,REG		,RSX		,IM7	,0		},
{	0x08	,0xa0	,3	,REG_IM7	,"AD"	,REG		,RSY		,IM7	,0		},
{	0x08	,0xc0	,3	,REG_IM7	,"AD"	,REG		,RSZ		,IM7	,0		},
{	0x08	,0xe0	,4	,REG2_IM7	,"AD"	,REG		,REG		,IM7	,0		},
{	0x09	,0		,2	,REG_NU		,"SB"	,REG		,RSX		,0		,0		},
{	0x09	,0x20	,2	,REG_NU		,"SB"	,REG		,RSY		,0		,0		},
{	0x09	,0x40	,2	,REG_NU		,"SB"	,REG		,RSZ		,0		,0		},
{	0x09	,0x60	,3	,REG_REG	,"SB"	,REG		,REG		,0		,0		},
{	0x09	,0x80	,3	,REG_IM7	,"SB"	,REG		,RSX		,IM7	,0		},
{	0x09	,0xa0	,3	,REG_IM7	,"SB"	,REG		,RSY		,IM7	,0		},
{	0x09	,0xc0	,3	,REG_IM7	,"SB"	,REG		,RSZ		,IM7	,0		},
{	0x09	,0xe0	,4	,REG2_IM7	,"SB"	,REG		,REG		,IM7	,0		},
{	0x0A	,0		,2	,REG_NU		,"ADB"	,REG		,RSX		,0		,0		},
{	0x0A	,0x20	,2	,REG_NU		,"ADB"	,REG		,RSY		,0		,0		},
{	0x0A	,0x40	,2	,REG_NU		,"ADB"	,REG		,RSZ		,0		,0		},
{	0x0A	,0x60	,3	,REG_REG	,"ADB"	,REG		,REG		,0		,0		},
{	0x0A	,0x80	,3	,REG_IM7	,"ADB"	,REG		,RSX		,IM7	,0		},
{	0x0A	,0xa0	,3	,REG_IM7	,"ADB"	,REG		,RSY		,IM7	,0		},
{	0x0A	,0xc0	,3	,REG_IM7	,"ADB"	,REG		,RSZ		,IM7	,0		},
{	0x0A	,0xe0	,4	,REG2_IM7	,"ADB"	,REG		,REG		,IM7	,0		},
{	0x0B	,0		,2	,REG_NU		,"SBB"	,REG		,RSX		,0		,0		},
{	0x0B	,0x20	,2	,REG_NU		,"SBB"	,REG		,RSY		,0		,0		},
{	0x0B	,0x40	,2	,REG_NU		,"SBB"	,REG		,RSZ		,0		,0		},
{	0x0B	,0x60	,3	,REG_REG	,"SBB"	,REG		,REG		,0		,0		},
{	0x0B	,0x80	,3	,REG_IM7	,"SBB"	,REG		,RSX		,IM7	,0		},
{	0x0B	,0xa0	,3	,REG_IM7	,"SBB"	,REG		,RSY		,IM7	,0		},
{	0x0B	,0xc0	,3	,REG_IM7	,"SBB"	,REG		,RSZ		,IM7	,0		},
{	0x0B	,0xe0	,4	,REG2_IM7	,"SBB"	,REG		,REG		,IM7	,0		},
{	0x0C	,0		,2	,REG_NU		,"AN"	,REG		,RSX		,0		,0		},
{	0x0C	,0x20	,2	,REG_NU		,"AN"	,REG		,RSY		,0		,0		},
{	0x0C	,0x40	,2	,REG_NU		,"AN"	,REG		,RSZ		,0		,0		},
{	0x0C	,0x60	,3	,REG_REG	,"AN"	,REG		,REG		,0		,0		},
{	0x0C	,0x80	,3	,REG_IM7	,"AN"	,REG		,RSX		,IM7	,0		},
{	0x0C	,0xa0	,3	,REG_IM7	,"AN"	,REG		,RSY		,IM7	,0		},
{	0x0C	,0xc0	,3	,REG_IM7	,"AN"	,REG		,RSZ		,IM7	,0		},
{	0x0C	,0xe0	,4	,REG2_IM7	,"AN"	,REG		,REG		,IM7	,0		},
{	0x0D	,0		,2	,REG_NU		,"NA"	,REG		,RSX		,0		,0		},
{	0x0D	,0x20	,2	,REG_NU		,"NA"	,REG		,RSY		,0		,0		},
{	0x0D	,0x40	,2	,REG_NU		,"NA"	,REG		,RSZ		,0		,0		},
{	0x0D	,0x60	,3	,REG_REG	,"NA"	,REG		,REG		,0		,0		},
{	0x0D	,0x80	,3	,REG_IM7	,"NA"	,REG		,RSX		,IM7	,0		},
{	0x0D	,0xa0	,3	,REG_IM7	,"NA"	,REG		,RSY		,IM7	,0		},
{	0x0D	,0xc0	,3	,REG_IM7	,"NA"	,REG		,RSZ		,IM7	,0		},
{	0x0D	,0xe0	,4	,REG2_IM7	,"NA"	,REG		,REG		,IM7	,0		},
{	0x0E	,0		,2	,REG_NU		,"OR"	,REG		,RSX		,0		,0		},
{	0x0E	,0x20	,2	,REG_NU		,"OR"	,REG		,RSY		,0		,0		},
{	0x0E	,0x40	,2	,REG_NU		,"OR"	,REG		,RSZ		,0		,0		},
{	0x0E	,0x60	,3	,REG_REG	,"OR"	,REG		,REG		,0		,0		},
{	0x0E	,0x80	,3	,REG_IM7	,"OR"	,REG		,RSX		,IM7	,0		},
{	0x0E	,0xa0	,3	,REG_IM7	,"OR"	,REG		,RSY		,IM7	,0		},
{	0x0E	,0xc0	,3	,REG_IM7	,"OR"	,REG		,RSZ		,IM7	,0		},
{	0x0E	,0xe0	,4	,REG2_IM7	,"OR"	,REG		,REG		,IM7	,0		},
{	0x0F	,0		,2	,REG_NU		,"XR"	,REG		,RSX		,0		,0		},
{	0x0F	,0x20	,2	,REG_NU		,"XR"	,REG		,RSY		,0		,0		},
{	0x0F	,0x40	,2	,REG_NU		,"XR"	,REG		,RSZ		,0		,0		},
{	0x0F	,0x60	,3	,REG_REG	,"XR"	,REG		,REG		,0		,0		},
{	0x0F	,0x80	,3	,REG_IM7	,"XR"	,REG		,RSX		,IM7	,0		},
{	0x0F	,0xa0	,3	,REG_IM7	,"XR"	,REG		,RSY		,IM7	,0		},
{	0x0F	,0xc0	,3	,REG_IM7	,"XR"	,REG		,RSZ		,IM7	,0		},
{	0x0F	,0xe0	,4	,REG2_IM7	,"XR"	,REG		,REG		,IM7	,0		},
{	0x10	,0		,2	,REG_NU		,"ST"	,REG		,MRSX		,0		,0		},
{	0x10	,0x20	,2	,REG_NU		,"ST"	,REG		,MRSY		,0		,0		},
{	0x10	,0x40	,2	,REG_NU		,"ST"	,REG		,MRSZ		,0		,0		},
{	0x10	,0x60	,3	,REG_REG	,"ST"	,REG		,MREG		,0		,0		},
{	0x10	,0x80	,3	,REG_IM7	,"ST"	,REG		,MRSX		,IM7	,0		},
{	0x10	,0xa0	,3	,REG_IM7	,"ST"	,REG		,MRSY		,IM7	,0		},
{	0x10	,0xc0	,3	,REG_IM7	,"ST"	,REG		,MRSZ		,IM7	,0		},
{	0x10	,0xe0	,4	,REG2_IM7	,"ST"	,REG		,MREG		,IM7	,0		},
{	0x11	,0		,2	,REG_NU		,"LD"	,REG		,MRSX		,0		,0		},
{	0x11	,0x20	,2	,REG_NU		,"LD"	,REG		,MRSY		,0		,0		},
{	0x11	,0x40	,2	,REG_NU		,"LD"	,REG		,MRSZ		,0		,0		},
{	0x11	,0x60	,3	,REG_REG	,"LD"	,REG		,MREG		,0		,0		},
{	0x11	,0x80	,3	,REG_IM7	,"LD"	,REG		,MRSX		,IM7	,0		},
{	0x11	,0xa0	,3	,REG_IM7	,"LD"	,REG		,MRSY		,IM7	,0		},
{	0x11	,0xc0	,3	,REG_IM7	,"LD"	,REG		,MRSZ		,IM7	,0		},
{	0x11	,0xe0	,4	,REG2_IM7	,"LD"	,REG		,MREG		,IM7	,0		},
{	0x12	,0		,2	,REG_NU		,"STL"	,REG		,0			,0		,0		},/* LCD出力 */
{	0x12	,0x80	,3	,REG_NUJ	,"STL"	,REG		,IM7		,0		,0		},/* LCD出力 */
{	0x13	,0		,2	,REG_NU		,"LDL"	,REG		,0			,0		,0		},/* LCD入力 */
{	0x13	,0x80	,3	,REG_NUJ	,"LDL"	,REG		,IM7		,0		,0		},/* LCD入力 */
{	0x14	,0		,2	,REG_NU		,"PPO"	,REG		,0			,0		,0		},/* LCD制御ポート */
{	0x14	,0x40	,2	,REG_NU		,"PFL"	,REG		,0			,0		,0		},
{	0x14	,0x80	,3	,REG_NUJ	,"PPO"	,REG		,IM7		,0		,0		},/* LCD制御ポート */
{	0x14	,0xc0	,3	,REG_NUJ	,"PFL"	,REG		,IM7		,0		,0		},
{	0x15	,0		,2	,IR_REG		,"PSR"	,SX			,REG		,0		,0		},/* 特定Index指定(SX) */
{	0x15	,0x20	,2	,IR_REG		,"PSR"	,SY			,REG		,0		,0		},/* 特定Index指定(SY) */
{	0x15	,0x40	,2	,IR_REG		,"PSR"	,SZ			,REG		,0		,0		},/* 特定Index指定(SZ) */
{	0x15	,0x80	,3	,IR_REGJ	,"PSR"	,SX			,REG		,IM7	,0		},/* 特定Index指定(SX) */
{	0x15	,0xa0	,3	,IR_REGJ	,"PSR"	,SY			,REG		,IM7	,0		},/* 特定Index指定(SY) */
{	0x15	,0xc0	,3	,IR_REGJ	,"PSR"	,SZ			,REG		,IM7	,0		},/* 特定Index指定(SZ) */
{	0x16	,0		,2	,IR_REG		,"PST"	,PE			,REG		,0		,0		},
{	0x16	,0x20	,2	,IR_REG		,"PST"	,PD			,REG		,0		,0		},
{	0x16	,0x40	,2	,IR_REG		,"PST"	,IB			,REG		,0		,0		},
{	0x16	,0x60	,2	,IR_REG		,"PST"	,UA			,REG		,0		,0		},
{	0x16	,0x80	,3	,IR_REGJ	,"PST"	,PE			,REG		,IM7	,0		},
{	0x16	,0xa0	,3	,IR_REGJ	,"PST"	,PD			,REG		,IM7	,0		},
{	0x16	,0xc0	,3	,IR_REGJ	,"PST"	,IB			,REG		,IM7	,0		},
{	0x16	,0xe0	,3	,IR_REGJ	,"PST"	,UA			,REG		,IM7	,0		},
{	0x17	,0		,2	,IR_REG		,"PST"	,IA			,REG		,0		,0		},
{	0x17	,0x20	,2	,IR_REG		,"PST"	,IE			,REG		,0		,0		},
{	0x17	,0x80	,3	,IR_REGJ	,"PST"	,IA			,REG		,IM7	,0		},
{	0x17	,0xa0	,3	,IR_REGJ	,"PST"	,IE			,REG		,IM7	,0		},
{	0x18	,0		,2	,REG_NU		,"ROD"	,REG		,0			,0		,0		},
{	0x18	,0x20	,2	,REG_NU		,"ROU"	,REG		,0			,0		,0		},
{	0x18	,0x40	,2	,REG_NU		,"BID"	,REG		,0			,0		,0		},
{	0x18	,0x60	,2	,REG_NU		,"BIU"	,REG		,0			,0		,0		},
{	0x18	,0x80	,3	,REG_NUJ	,"ROD"	,REG		,IM7		,0		,0		},
{	0x18	,0xa0	,3	,REG_NUJ	,"ROU"	,REG		,IM7		,0		,0		},
{	0x18	,0xc0	,3	,REG_NUJ	,"BID"	,REG		,IM7		,0		,0		},
{	0x18	,0xe0	,3	,REG_NUJ	,"BIU"	,REG		,IM7		,0		,0		},
{	0x1A	,0		,2	,REG_NU		,"DID"	,REG		,0			,0		,0		},
{	0x1A	,0x20	,2	,REG_NU		,"DIU"	,REG		,0			,0		,0		},
{	0x1A	,0x40	,2	,REG_NU		,"BYD"	,REG		,0			,0		,0		},
{	0x1A	,0x60	,2	,REG_NU		,"BYU"	,REG		,0			,0		,0		},
{	0x1A	,0x80	,3	,REG_NUJ	,"DID"	,REG		,IM7		,0		,0		},
{	0x1A	,0xa0	,3	,REG_NUJ	,"DIU"	,REG		,IM7		,0		,0		},
{	0x1A	,0xc0	,3	,REG_NUJ	,"BYD"	,REG		,IM7		,0		,0		},
{	0x1A	,0xe0	,3	,REG_NUJ	,"BYU"	,REG		,IM7		,0		,0		},
{	0x1B	,0		,2	,REG_NU		,"CMP"	,REG		,0			,0		,0		},
{	0x1B	,0x40	,2	,REG_NU		,"INV"	,REG		,0			,0		,0		},
{	0x1B	,0x80	,3	,REG_NUJ	,"CMP"	,REG		,IM7		,0		,0		},
{	0x1B	,0xc0	,3	,REG_NUJ	,"INV"	,REG		,IM7		,0		,0		},
{	0x1C	,0		,2	,REG_NU		,"GPO"	,REG		,0			,0		,0		},
{	0x1C	,0x40	,2	,REG_NU		,"GFL"	,REG		,0			,0		,0		},
{	0x1C	,0x80	,3	,REG_NUJ	,"GPO"	,REG		,IM7		,0		,0		},
{	0x1C	,0xc0	,3	,REG_NUJ	,"GFL"	,REG		,IM7		,0		,0		},
{	0x1D	,0		,2	,IR_REG		,"GSR"	,SX			,REG		,0		,0		},/* 特定Index取得(SX) */
{	0x1D	,0x20	,2	,IR_REG		,"GSR"	,SY			,REG		,0		,0		},/* 特定Index取得(SY) */
{	0x1D	,0x40	,2	,IR_REG		,"GSR"	,SZ			,REG		,0		,0		},/* 特定Index取得(SZ) */
{	0x1D	,0x80	,3	,IR_REGJ	,"GSR"	,SX			,REG		,IM7	,0		},/* 特定Index取得(SX) */
{	0x1D	,0xa0	,3	,IR_REGJ	,"GSR"	,SY			,REG		,IM7	,0		},/* 特定Index取得(SY) */
{	0x1D	,0xc0	,3	,IR_REGJ	,"GSR"	,SZ			,REG		,IM7	,0		},/* 特定Index取得(SZ) */
{	0x1E	,0		,2	,IR_REG		,"GST"	,PE			,REG		,0		,0		},
{	0x1E	,0x20	,2	,IR_REG		,"GST"	,PD			,REG		,0		,0		},
{	0x1E	,0x40	,2	,IR_REG		,"GST"	,IB			,REG		,0		,0		},
{	0x1E	,0x60	,2	,IR_REG		,"GST"	,UA			,REG		,0		,0		},
{	0x1E	,0x80	,3	,IR_REGJ	,"GST"	,PE			,REG		,IM7	,0		},
{	0x1E	,0xa0	,3	,IR_REGJ	,"GST"	,PD			,REG		,IM7	,0		},
{	0x1E	,0xc0	,3	,IR_REGJ	,"GST"	,IB			,REG		,IM7	,0		},
{	0x1E	,0xe0	,3	,IR_REGJ	,"GST"	,UA			,REG		,IM7	,0		},
{	0x1F	,0		,2	,IR_REG		,"GST"	,IA			,REG		,0		,0		},
{	0x1F	,0x20	,2	,IR_REG		,"GST"	,IE			,REG		,0		,0		},
{	0x1F	,0x60	,2	,IR_REG		,"GST"	,TM			,REG		,0		,0		},
{	0x1F	,0x80	,3	,IR_REGJ	,"GST"	,IA			,REG		,IM7	,0		},
{	0x1F	,0xa0	,3	,IR_REGJ	,"GST"	,IE			,REG		,IM7	,0		},
{	0x1F	,0xe0	,3	,IR_REGJ	,"GST"	,TM			,REG		,IM7	,0		},
{	0x20	,0		,2	,REG_NU		,"ST"	,REG		,MIX_p|RSX	,0		,0		},
{	0x20	,0x20	,2	,REG_NU		,"ST"	,REG		,MIX_p|RSY	,0		,0		},
{	0x20	,0x40	,2	,REG_NU		,"ST"	,REG		,MIX_p|RSZ	,0		,0		},
{	0x20	,0x60	,3	,REG_REG	,"ST"	,REG		,MIX_p|REG	,0		,0		},
{	0x20	,0x80	,2	,REG_NU		,"ST"	,REG		,MIX_m|RSX	,0		,0		},
{	0x20	,0xa0	,2	,REG_NU		,"ST"	,REG		,MIX_m|RSY	,0		,0		},
{	0x20	,0xc0	,2	,REG_NU		,"ST"	,REG		,MIX_m|RSZ	,0		,0		},
{	0x20	,0xe0	,3	,REG_REG	,"ST"	,REG		,MIX_m|REG	,0		,0		},
{	0x21	,0		,2	,REG_NU		,"ST"	,REG		,MIZ_p|RSX	,0		,0		},
{	0x21	,0x20	,2	,REG_NU		,"ST"	,REG		,MIZ_p|RSY	,0		,0		},
{	0x21	,0x40	,2	,REG_NU		,"ST"	,REG		,MIZ_p|RSZ	,0		,0		},
{	0x21	,0x60	,3	,REG_REG	,"ST"	,REG		,MIZ_p|REG	,0		,0		},
{	0x21	,0x80	,2	,REG_NU		,"ST"	,REG		,MIZ_m|RSX	,0		,0		},
{	0x21	,0xa0	,2	,REG_NU		,"ST"	,REG		,MIZ_m|RSY	,0		,0		},
{	0x21	,0xc0	,2	,REG_NU		,"ST"	,REG		,MIZ_m|RSZ	,0		,0		},
{	0x21	,0xe0	,3	,REG_REG	,"ST"	,REG		,MIZ_m|REG	,0		,0		},
{	0x22	,0		,2	,REG_NU		,"STI"	,REG		,MIX_p|RSX	,0		,0		},
{	0x22	,0x20	,2	,REG_NU		,"STI"	,REG		,MIX_p|RSY	,0		,0		},
{	0x22	,0x40	,2	,REG_NU		,"STI"	,REG		,MIX_p|RSZ	,0		,0		},
{	0x22	,0x60	,3	,REG_REG	,"STI"	,REG		,MIX_p|REG	,0		,0		},
{	0x22	,0x80	,2	,REG_NU		,"STI"	,REG		,MIX_m|RSX	,0		,0		},
{	0x22	,0xa0	,2	,REG_NU		,"STI"	,REG		,MIX_m|RSY	,0		,0		},
{	0x22	,0xc0	,2	,REG_NU		,"STI"	,REG		,MIX_m|RSZ	,0		,0		},
{	0x22	,0xe0	,3	,REG_REG	,"STI"	,REG		,MIX_m|REG	,0		,0		},
{	0x23	,0		,2	,REG_NU		,"STI"	,REG		,MIZ_p|RSX	,0		,0		},
{	0x23	,0x20	,2	,REG_NU		,"STI"	,REG		,MIZ_p|RSY	,0		,0		},
{	0x23	,0x40	,2	,REG_NU		,"STI"	,REG		,MIZ_p|RSZ	,0		,0		},
{	0x23	,0x60	,3	,REG_REG	,"STI"	,REG		,MIZ_p|REG	,0		,0		},
{	0x23	,0x80	,2	,REG_NU		,"STI"	,REG		,MIZ_m|RSX	,0		,0		},
{	0x23	,0xa0	,2	,REG_NU		,"STI"	,REG		,MIZ_m|RSY	,0		,0		},
{	0x23	,0xc0	,2	,REG_NU		,"STI"	,REG		,MIZ_m|RSZ	,0		,0		},
{	0x23	,0xe0	,3	,REG_REG	,"STI"	,REG		,MIZ_m|REG	,0		,0		},
{	0x24	,0		,2	,REG_NU		,"STD"	,REG		,MIX_p|RSX	,0		,0		},
{	0x24	,0x20	,2	,REG_NU		,"STD"	,REG		,MIX_p|RSY	,0		,0		},
{	0x24	,0x40	,2	,REG_NU		,"STD"	,REG		,MIX_p|RSZ	,0		,0		},
{	0x24	,0x60	,3	,REG_REG	,"STD"	,REG		,MIX_p|REG	,0		,0		},
{	0x24	,0x80	,2	,REG_NU		,"STD"	,REG		,MIX_m|RSX	,0		,0		},
{	0x24	,0xa0	,2	,REG_NU		,"STD"	,REG		,MIX_m|RSY	,0		,0		},
{	0x24	,0xc0	,2	,REG_NU		,"STD"	,REG		,MIX_m|RSZ	,0		,0		},
{	0x24	,0xe0	,3	,REG_REG	,"STD"	,REG		,MIX_m|REG	,0		,0		},
{	0x25	,0		,2	,REG_NU		,"STD"	,REG		,MIZ_p|RSX	,0		,0		},
{	0x25	,0x20	,2	,REG_NU		,"STD"	,REG		,MIZ_p|RSY	,0		,0		},
{	0x25	,0x40	,2	,REG_NU		,"STD"	,REG		,MIZ_p|RSZ	,0		,0		},
{	0x25	,0x60	,3	,REG_REG	,"STD"	,REG		,MIZ_p|REG	,0		,0		},
{	0x25	,0x80	,2	,REG_NU		,"STD"	,REG		,MIZ_m|RSX	,0		,0		},
{	0x25	,0xa0	,2	,REG_NU		,"STD"	,REG		,MIZ_m|RSY	,0		,0		},
{	0x25	,0xc0	,2	,REG_NU		,"STD"	,REG		,MIZ_m|RSZ	,0		,0		},
{	0x25	,0xe0	,3	,REG_REG	,"STD"	,REG		,MIZ_m|REG	,0		,0		},
{	0x26	,0		,2	,REG_NU		,"PHS"	,REG		,0			,0		,0		},
{	0x27	,0		,2	,REG_NU		,"PHU"	,REG		,0			,0		,0		},
{	0x28	,0		,2	,REG_NU		,"LD"	,REG		,MIX_p|RSX	,0		,0		},
{	0x28	,0x20	,2	,REG_NU		,"LD"	,REG		,MIX_p|RSY	,0		,0		},
{	0x28	,0x40	,2	,REG_NU		,"LD"	,REG		,MIX_p|RSZ	,0		,0		},
{	0x28	,0x60	,3	,REG_REG	,"LD"	,REG		,MIX_p|REG	,0		,0		},
{	0x28	,0x80	,2	,REG_NU		,"LD"	,REG		,MIX_m|RSX	,0		,0		},
{	0x28	,0xa0	,2	,REG_NU		,"LD"	,REG		,MIX_m|RSY	,0		,0		},
{	0x28	,0xc0	,2	,REG_NU		,"LD"	,REG		,MIX_m|RSZ	,0		,0		},
{	0x28	,0xe0	,3	,REG_REG	,"LD"	,REG		,MIX_m|REG	,0		,0		},
{	0x29	,0		,2	,REG_NU		,"LD"	,REG		,MIZ_p|RSX	,0		,0		},
{	0x29	,0x20	,2	,REG_NU		,"LD"	,REG		,MIZ_p|RSY	,0		,0		},
{	0x29	,0x40	,2	,REG_NU		,"LD"	,REG		,MIZ_p|RSZ	,0		,0		},
{	0x29	,0x60	,3	,REG_REG	,"LD"	,REG		,MIZ_p|REG	,0		,0		},
{	0x29	,0x80	,2	,REG_NU		,"LD"	,REG		,MIZ_m|RSX	,0		,0		},
{	0x29	,0xa0	,2	,REG_NU		,"LD"	,REG		,MIZ_m|RSY	,0		,0		},
{	0x29	,0xc0	,2	,REG_NU		,"LD"	,REG		,MIZ_m|RSZ	,0		,0		},
{	0x29	,0xe0	,3	,REG_REG	,"LD"	,REG		,MIZ_m|REG	,0		,0		},
{	0x2A	,0		,2	,REG_NU		,"LDI"	,REG		,MIX_p|RSX	,0		,0		},
{	0x2A	,0x20	,2	,REG_NU		,"LDI"	,REG		,MIX_p|RSY	,0		,0		},
{	0x2A	,0x40	,2	,REG_NU		,"LDI"	,REG		,MIX_p|RSZ	,0		,0		},
{	0x2A	,0x60	,3	,REG_REG	,"LDI"	,REG		,MIX_p|REG	,0		,0		},
{	0x2A	,0x80	,2	,REG_NU		,"LDI"	,REG		,MIX_m|RSX	,0		,0		},
{	0x2A	,0xa0	,2	,REG_NU		,"LDI"	,REG		,MIX_m|RSY	,0		,0		},
{	0x2A	,0xc0	,2	,REG_NU		,"LDI"	,REG		,MIX_m|RSZ	,0		,0		},
{	0x2A	,0xe0	,3	,REG_REG	,"LDI"	,REG		,MIX_m|REG	,0		,0		},
{	0x2B	,0		,2	,REG_NU		,"LDI"	,REG		,MIZ_p|RSX	,0		,0		},
{	0x2B	,0x20	,2	,REG_NU		,"LDI"	,REG		,MIZ_p|RSY	,0		,0		},
{	0x2B	,0x40	,2	,REG_NU		,"LDI"	,REG		,MIZ_p|RSZ	,0		,0		},
{	0x2B	,0x60	,3	,REG_REG	,"LDI"	,REG		,MIZ_p|REG	,0		,0		},
{	0x2B	,0x80	,2	,REG_NU		,"LDI"	,REG		,MIZ_m|RSX	,0		,0		},
{	0x2B	,0xa0	,2	,REG_NU		,"LDI"	,REG		,MIZ_m|RSY	,0		,0		},
{	0x2B	,0xc0	,2	,REG_NU		,"LDI"	,REG		,MIZ_m|RSZ	,0		,0		},
{	0x2B	,0xe0	,3	,REG_REG	,"LDI"	,REG		,MIZ_m|REG	,0		,0		},
{	0x2C	,0		,2	,REG_NU		,"LDD"	,REG		,MIX_p|RSX	,0		,0		},
{	0x2C	,0x20	,2	,REG_NU		,"LDD"	,REG		,MIX_p|RSY	,0		,0		},
{	0x2C	,0x40	,2	,REG_NU		,"LDD"	,REG		,MIX_p|RSZ	,0		,0		},
{	0x2C	,0x60	,3	,REG_REG	,"LDD"	,REG		,MIX_p|REG	,0		,0		},
{	0x2C	,0x80	,2	,REG_NU		,"LDD"	,REG		,MIX_m|RSX	,0		,0		},
{	0x2C	,0xa0	,2	,REG_NU		,"LDD"	,REG		,MIX_m|RSY	,0		,0		},
{	0x2C	,0xc0	,2	,REG_NU		,"LDD"	,REG		,MIX_m|RSZ	,0		,0		},
{	0x2C	,0xe0	,3	,REG_REG	,"LDD"	,REG		,MIX_m|REG	,0		,0		},
{	0x2D	,0		,2	,REG_NU		,"LDD"	,REG		,MIZ_p|RSX	,0		,0		},
{	0x2D	,0x20	,2	,REG_NU		,"LDD"	,REG		,MIZ_p|RSY	,0		,0		},
{	0x2D	,0x40	,2	,REG_NU		,"LDD"	,REG		,MIZ_p|RSZ	,0		,0		},
{	0x2D	,0x60	,3	,REG_REG	,"LDD"	,REG		,MIZ_p|REG	,0		,0		},
{	0x2D	,0x80	,2	,REG_NU		,"LDD"	,REG		,MIZ_m|RSX	,0		,0		},
{	0x2D	,0xa0	,2	,REG_NU		,"LDD"	,REG		,MIZ_m|RSY	,0		,0		},
{	0x2D	,0xc0	,2	,REG_NU		,"LDD"	,REG		,MIZ_m|RSZ	,0		,0		},
{	0x2D	,0xe0	,3	,REG_REG	,"LDD"	,REG		,MIZ_m|REG	,0		,0		},
{	0x2E	,0		,2	,REG_NU		,"PPS"	,REG		,0			,0		,0		},
{	0x2F	,0		,2	,REG_NU		,"PPU"	,REG		,0			,0		,0		},
{	0x30	,0		,3	,F_IM16		,"JP"	,ZF			,IM16		,0		,0		},
{	0x31	,0		,3	,F_IM16		,"JP"	,NCF		,IM16		,0		,0		},
{	0x32	,0		,3	,F_IM16		,"JP"	,LZF		,IM16		,0		,0		},
{	0x33	,0		,3	,F_IM16		,"JP"	,UZF		,IM16		,0		,0		},
{	0x34	,0		,3	,F_IM16		,"JP"	,NZF		,IM16		,0		,0		},
{	0x35	,0		,3	,F_IM16		,"JP"	,CF			,IM16		,0		,0		},
{	0x36	,0		,3	,F_IM16		,"JP"	,NLZF		,IM16		,0		,0		},
{	0x37	,0		,3	,IM16_NU	,"JP"	,IM16		,0			,0		,0		},
{	0x38	,0		,2	,IR_REG		,"ADC"	,MIX_p|RSX	,REG		,0		,0		},
{	0x38	,0x20	,2	,IR_REG		,"ADC"	,MIX_p|RSY	,REG		,0		,0		},
{	0x38	,0x40	,2	,IR_REG		,"ADC"	,MIX_p|RSZ	,REG		,0		,0		},
{	0x38	,0x60	,3	,MREG_REG	,"ADC"	,MIX_p|REG	,REG		,0		,0		},
{	0x38	,0x80	,2	,IR_REG		,"ADC"	,MIX_m|RSX	,REG		,0		,0		},
{	0x38	,0xa0	,2	,IR_REG		,"ADC"	,MIX_m|RSY	,REG		,0		,0		},
{	0x38	,0xc0	,2	,IR_REG		,"ADC"	,MIX_m|RSZ	,REG		,0		,0		},
{	0x38	,0xe0	,3	,MREG_REG	,"ADC"	,MIX_m|REG	,REG		,0		,0		},
{	0x39	,0		,2	,IR_REG		,"ADC"	,MIZ_p|RSX	,REG		,0		,0		},
{	0x39	,0x20	,2	,IR_REG		,"ADC"	,MIZ_p|RSY	,REG		,0		,0		},
{	0x39	,0x40	,2	,IR_REG		,"ADC"	,MIZ_p|RSZ	,REG		,0		,0		},
{	0x39	,0x60	,3	,MREG_REG	,"ADC"	,MIZ_p|REG	,REG		,0		,0		},
{	0x39	,0x80	,2	,IR_REG		,"ADC"	,MIZ_m|RSX	,REG		,0		,0		},
{	0x39	,0xa0	,2	,IR_REG		,"ADC"	,MIZ_m|RSY	,REG		,0		,0		},
{	0x39	,0xc0	,2	,IR_REG		,"ADC"	,MIZ_m|RSZ	,REG		,0		,0		},
{	0x39	,0xe0	,3	,MREG_REG	,"ADC"	,MIZ_m|REG	,REG		,0		,0		},
{	0x3A	,0		,2	,IR_REG		,"SBC"	,MIX_p|RSX	,REG		,0		,0		},
{	0x3A	,0x20	,2	,IR_REG		,"SBC"	,MIX_p|RSY	,REG		,0		,0		},
{	0x3A	,0x40	,2	,IR_REG		,"SBC"	,MIX_p|RSZ	,REG		,0		,0		},
{	0x3A	,0x60	,3	,MREG_REG	,"SBC"	,MIX_p|REG	,REG		,0		,0		},
{	0x3A	,0x80	,2	,IR_REG		,"SBC"	,MIX_m|RSX	,REG		,0		,0		},
{	0x3A	,0xa0	,2	,IR_REG		,"SBC"	,MIX_m|RSY	,REG		,0		,0		},
{	0x3A	,0xc0	,2	,IR_REG		,"SBC"	,MIX_m|RSZ	,REG		,0		,0		},
{	0x3A	,0xe0	,3	,MREG_REG	,"SBC"	,MIX_m|REG	,REG		,0		,0		},
{	0x3B	,0		,2	,IR_REG		,"SBC"	,MIZ_p|RSX	,REG		,0		,0		},
{	0x3B	,0x20	,2	,IR_REG		,"SBC"	,MIZ_p|RSY	,REG		,0		,0		},
{	0x3B	,0x40	,2	,IR_REG		,"SBC"	,MIZ_p|RSZ	,REG		,0		,0		},
{	0x3B	,0x60	,3	,MREG_REG	,"SBC"	,MIZ_p|REG	,REG		,0		,0		},
{	0x3B	,0x80	,2	,IR_REG		,"SBC"	,MIZ_m|RSX	,REG		,0		,0		},
{	0x3B	,0xa0	,2	,IR_REG		,"SBC"	,MIZ_m|RSY	,REG		,0		,0		},
{	0x3B	,0xc0	,2	,IR_REG		,"SBC"	,MIZ_m|RSZ	,REG		,0		,0		},
{	0x3B	,0xe0	,3	,MREG_REG	,"SBC"	,MIZ_m|REG	,REG		,0		,0		},
{	0x3C	,0		,2	,IR_REG		,"AD"	,MIX_p|RSX	,REG		,0		,0		},
{	0x3C	,0x20	,2	,IR_REG		,"AD"	,MIX_p|RSY	,REG		,0		,0		},
{	0x3C	,0x40	,2	,IR_REG		,"AD"	,MIX_p|RSZ	,REG		,0		,0		},
{	0x3C	,0x60	,3	,MREG_REG	,"AD"	,MIX_p|REG	,REG		,0		,0		},
{	0x3C	,0x80	,2	,IR_REG		,"AD"	,MIX_m|RSX	,REG		,0		,0		},
{	0x3C	,0xa0	,2	,IR_REG		,"AD"	,MIX_m|RSY	,REG		,0		,0		},
{	0x3C	,0xc0	,2	,IR_REG		,"AD"	,MIX_m|RSZ	,REG		,0		,0		},
{	0x3C	,0xe0	,3	,MREG_REG	,"AD"	,MIX_m|REG	,REG		,0		,0		},
{	0x3D	,0		,2	,IR_REG		,"AD"	,MIZ_p|RSX	,REG		,0		,0		},
{	0x3D	,0x20	,2	,IR_REG		,"AD"	,MIZ_p|RSY	,REG		,0		,0		},
{	0x3D	,0x40	,2	,IR_REG		,"AD"	,MIZ_p|RSZ	,REG		,0		,0		},
{	0x3D	,0x60	,3	,MREG_REG	,"AD"	,MIZ_p|REG	,REG		,0		,0		},
{	0x3D	,0x80	,2	,IR_REG		,"AD"	,MIZ_m|RSX	,REG		,0		,0		},
{	0x3D	,0xa0	,2	,IR_REG		,"AD"	,MIZ_m|RSY	,REG		,0		,0		},
{	0x3D	,0xc0	,2	,IR_REG		,"AD"	,MIZ_m|RSZ	,REG		,0		,0		},
{	0x3D	,0xe0	,3	,MREG_REG	,"AD"	,MIZ_m|REG	,REG		,0		,0		},
{	0x3E	,0		,2	,IR_REG		,"SB"	,MIX_p|RSX	,REG		,0		,0		},
{	0x3E	,0x20	,2	,IR_REG		,"SB"	,MIX_p|RSY	,REG		,0		,0		},
{	0x3E	,0x40	,2	,IR_REG		,"SB"	,MIX_p|RSZ	,REG		,0		,0		},
{	0x3E	,0x60	,3	,MREG_REG	,"SB"	,MIX_p|REG	,REG		,0		,0		},
{	0x3E	,0x80	,2	,IR_REG		,"SB"	,MIX_m|RSX	,REG		,0		,0		},
{	0x3E	,0xa0	,2	,IR_REG		,"SB"	,MIX_m|RSY	,REG		,0		,0		},
{	0x3E	,0xc0	,2	,IR_REG		,"SB"	,MIX_m|RSZ	,REG		,0		,0		},
{	0x3E	,0xe0	,3	,MREG_REG	,"SB"	,MIX_m|REG	,REG		,0		,0		},
{	0x3F	,0		,2	,IR_REG		,"SB"	,MIZ_p|RSX	,REG		,0		,0		},
{	0x3F	,0x20	,2	,IR_REG		,"SB"	,MIZ_p|RSY	,REG		,0		,0		},
{	0x3F	,0x40	,2	,IR_REG		,"SB"	,MIZ_p|RSZ	,REG		,0		,0		},
{	0x3F	,0x60	,3	,MREG_REG	,"SB"	,MIZ_p|REG	,REG		,0		,0		},
{	0x3F	,0x80	,2	,IR_REG		,"SB"	,MIZ_m|RSX	,REG		,0		,0		},
{	0x3F	,0xa0	,2	,IR_REG		,"SB"	,MIZ_m|RSY	,REG		,0		,0		},
{	0x3F	,0xc0	,2	,IR_REG		,"SB"	,MIZ_m|RSZ	,REG		,0		,0		},
{	0x3F	,0xe0	,3	,MREG_REG	,"SB"	,MIZ_m|REG	,REG		,0		,0		},
{	0x40	,0		,3	,REG_IM8	,"ADC"	,REG		,IM8		,0		,0		},
{	0x40	,0x80	,4	,REG2_IM7	,"ADC"	,REG		,IM8		,IM7	,0		},
{	0x41	,0		,3	,REG_IM8	,"SBC"	,REG		,IM8		,0		,0		},
{	0x41	,0x80	,4	,REG2_IM7	,"SBC"	,REG		,IM8		,IM7	,0		},
{	0x42	,0		,3	,REG_IM8	,"LD"	,REG		,IM8		,0		,0		},
{	0x42	,0x80	,4	,REG2_IM7	,"LD"	,REG		,IM8		,IM7	,0		},
{	0x43	,0		,3	,REG_IM8	,"LDC"	,REG		,IM8		,0		,0		},/* NOP instruction */
{	0x43	,0x80	,4	,REG2_IM7	,"LDC"	,REG		,IM8		,IM7	,0		},/* NOP instruction */
{	0x44	,0		,3	,REG_IM8	,"ANC"	,REG		,IM8		,0		,0		},
{	0x44	,0x80	,4	,REG2_IM7	,"ANC"	,REG		,IM8		,IM7	,0		},
{	0x45	,0		,3	,REG_IM8	,"NAC"	,REG		,IM8		,0		,0		},
{	0x45	,0x80	,4	,REG2_IM7	,"NAC"	,REG		,IM8		,IM7	,0		},
{	0x46	,0		,3	,REG_IM8	,"ORC"	,REG		,IM8		,0		,0		},
{	0x46	,0x80	,4	,REG2_IM7	,"ORC"	,REG		,IM8		,IM7	,0		},
{	0x47	,0		,3	,REG_IM8	,"XRC"	,REG		,IM8		,0		,0		},
{	0x47	,0x80	,4	,REG2_IM7	,"XRC"	,REG		,IM8		,IM7	,0		},
{	0x48	,0		,3	,REG_IM8	,"AD"	,REG		,IM8		,0		,0		},
{	0x48	,0x80	,4	,REG2_IM7	,"AD"	,REG		,IM8		,IM7	,0		},
{	0x49	,0		,3	,REG_IM8	,"SB"	,REG		,IM8		,0		,0		},
{	0x49	,0x80	,4	,REG2_IM7	,"SB"	,REG		,IM8		,IM7	,0		},
{	0x4A	,0		,3	,REG_IM8	,"ADB"	,REG		,IM8		,0		,0		},
{	0x4A	,0x80	,4	,REG2_IM7	,"ADB"	,REG		,IM8		,IM7	,0		},
{	0x4B	,0		,3	,REG_IM8	,"SBB"	,REG		,IM8		,0		,0		},
{	0x4B	,0x80	,4	,REG2_IM7	,"SBB"	,REG		,IM8		,IM7	,0		},
{	0x4C	,0		,3	,REG_IM8	,"AN"	,REG		,IM8		,0		,0		},
{	0x4C	,0x80	,4	,REG2_IM7	,"AN"	,REG		,IM8		,IM7	,0		},
{	0x4D	,0		,3	,REG_IM8	,"NA"	,REG		,IM8		,0		,0		},
{	0x4D	,0x80	,4	,REG2_IM7	,"NA"	,REG		,IM8		,IM7	,0		},
{	0x4E	,0		,3	,REG_IM8	,"OR"	,REG		,IM8		,0		,0		},
{	0x4E	,0x80	,4	,REG2_IM7	,"OR"	,REG		,IM8		,IM7	,0		},
{	0x4F	,0		,3	,REG_IM8	,"XR"	,REG		,IM8		,0		,0		},
{	0x4F	,0x80	,4	,REG2_IM7	,"XR"	,REG		,IM8		,IM7	,0		},
{	0x50	,0		,3	,NU_IM8		,"ST"	,IM8		,MRSX		,0		,0		},
{	0x50	,0x20	,3	,NU_IM8		,"ST"	,IM8		,MRSY		,0		,0		},
{	0x50	,0x40	,3	,NU_IM8		,"ST"	,IM8		,MRSZ		,0		,0		},
{	0x51	,0		,3	,IM8_REG	,"ST"	,IM8		,REG		,0		,0		},/* LDW $C5,IM16 の 8Bit版 */
{	0x52	,0		,2	,IM8_NU		,"STL"	,IM8		,0			,0		,0		},/* LCD出力 */
{	0x54	,0		,3	,NU_IM8		,"PPO"	,IM8		,0			,0		,0		},/* LCD制御ポート */
{	0x54	,0x40	,3	,NU_IM8		,"PFL"	,IM8		,0			,0		,0		},
{	0x55	,0		,2	,IR_IM5		,"PSR"	,SX			,IM5		,0		,0		},/* 特定Index指定(SX) */
{	0x55	,0x20	,2	,IR_IM5		,"PSR"	,SY			,IM5		,0		,0		},/* 特定Index指定(SY) */
{	0x55	,0x40	,2	,IR_IM5		,"PSR"	,SZ			,IM5		,0		,0		},/* 特定Index指定(SZ) */
{	0x56	,0		,3	,IR_IM8		,"PST"	,PE			,IM8		,0		,0		},
{	0x56	,0x20	,3	,IR_IM8		,"PST"	,PD			,IM8		,0		,0		},
{	0x56	,0x40	,3	,IR_IM8		,"PST"	,IB			,IM8		,0		,0		},
{	0x56	,0x60	,3	,IR_IM8		,"PST"	,UA			,IM8		,0		,0		},
{	0x57	,0		,3	,IR_IM8		,"PST"	,IA			,IM8		,0		,0		},
{	0x57	,0x20	,3	,IR_IM8		,"PST"	,IE			,IM8		,0		,0		},
{	0x58	,0		,2	,IM8_NU		,"BUPS"	,IM8		,0			,0		,0		},
{	0x59	,0		,2	,IM8_NU		,"BDNS"	,IM8		,0			,0		,0		},
{	0x5C	,0		,2	,IM8_NU		,"SUP"	,IM8		,0			,0		,0		},
{	0x5D	,0		,2	,IM8_NU		,"SDN"	,IM8		,0			,0		,0		},
{	0x60	,0		,3	,REG_IM8	,"ST"	,REG		,MIX_p|IM8	,0		,0		},
{	0x60	,0x80	,3	,REG_IM8	,"ST"	,REG		,MIX_m|IM8	,0		,0		},
{	0x61	,0		,3	,REG_IM8	,"ST"	,REG		,MIZ_p|IM8	,0		,0		},
{	0x61	,0x80	,3	,REG_IM8	,"ST"	,REG		,MIZ_m|IM8	,0		,0		},
{	0x62	,0		,3	,REG_IM8	,"STI"	,REG		,MIX_p|IM8	,0		,0		},
{	0x62	,0x80	,3	,REG_IM8	,"STI"	,REG		,MIX_m|IM8	,0		,0		},
{	0x63	,0		,3	,REG_IM8	,"STI"	,REG		,MIZ_p|IM8	,0		,0		},
{	0x63	,0x80	,3	,REG_IM8	,"STI"	,REG		,MIZ_m|IM8	,0		,0		},
{	0x64	,0		,3	,REG_IM8	,"STD"	,REG		,MIX_p|IM8	,0		,0		},
{	0x64	,0x80	,3	,REG_IM8	,"STD"	,REG		,MIX_m|IM8	,0		,0		},
{	0x65	,0		,3	,REG_IM8	,"STD"	,REG		,MIZ_p|IM8	,0		,0		},
{	0x65	,0x80	,3	,REG_IM8	,"STD"	,REG		,MIZ_m|IM8	,0		,0		},
{	0x68	,0		,3	,REG_IM8	,"LD"	,REG		,MIX_p|IM8	,0		,0		},
{	0x68	,0x80	,3	,REG_IM8	,"LD"	,REG		,MIX_m|IM8	,0		,0		},
{	0x69	,0		,3	,REG_IM8	,"LD"	,REG		,MIZ_p|IM8	,0		,0		},
{	0x69	,0x80	,3	,REG_IM8	,"LD"	,REG		,MIZ_m|IM8	,0		,0		},
{	0x6A	,0		,3	,REG_IM8	,"LDI"	,REG		,MIX_p|IM8	,0		,0		},
{	0x6A	,0x80	,3	,REG_IM8	,"LDI"	,REG		,MIX_m|IM8	,0		,0		},
{	0x6B	,0		,3	,REG_IM8	,"LDI"	,REG		,MIZ_p|IM8	,0		,0		},
{	0x6B	,0x80	,3	,REG_IM8	,"LDI"	,REG		,MIZ_m|IM8	,0		,0		},
{	0x6C	,0		,3	,REG_IM8	,"LDD"	,REG		,MIX_p|IM8	,0		,0		},
{	0x6C	,0x80	,3	,REG_IM8	,"LDD"	,REG		,MIX_m|IM8	,0		,0		},
{	0x6D	,0		,3	,REG_IM8	,"LDD"	,REG		,MIZ_p|IM8	,0		,0		},
{	0x6D	,0x80	,3	,REG_IM8	,"LDD"	,REG		,MIZ_m|IM8	,0		,0		},
{	0x70	,0		,3	,F_IM16		,"CAL"	,ZF			,IM16		,0		,0		},
{	0x71	,0		,3	,F_IM16		,"CAL"	,NCF		,IM16		,0		,0		},
{	0x72	,0		,3	,F_IM16		,"CAL"	,LZF		,IM16		,0		,0		},
{	0x73	,0		,3	,F_IM16		,"CAL"	,UZF		,IM16		,0		,0		},
{	0x74	,0		,3	,F_IM16		,"CAL"	,NZF		,IM16		,0		,0		},
{	0x75	,0		,3	,F_IM16		,"CAL"	,CF			,IM16		,0		,0		},
{	0x76	,0		,3	,F_IM16		,"CAL"	,NLZF		,IM16		,0		,0		},
{	0x77	,0		,3	,IM16_NU	,"CAL"	,IM16		,0			,0		,0		},
{	0x78	,0		,3	,MREG_REG	,"ADC"	,MIX_p|IM8	,REG		,0		,0		},
{	0x78	,0x80	,3	,MREG_REG	,"ADC"	,MIX_m|IM8	,REG		,0		,0		},
{	0x79	,0		,3	,MREG_REG	,"ADC"	,MIZ_p|IM8	,REG		,0		,0		},
{	0x79	,0x80	,3	,MREG_REG	,"ADC"	,MIZ_m|IM8	,REG		,0		,0		},
{	0x7A	,0		,3	,MREG_REG	,"SBC"	,MIX_p|IM8	,REG		,0		,0		},
{	0x7A	,0x80	,3	,MREG_REG	,"SBC"	,MIX_m|IM8	,REG		,0		,0		},
{	0x7B	,0		,3	,MREG_REG	,"SBC"	,MIZ_p|IM8	,REG		,0		,0		},
{	0x7B	,0x80	,3	,MREG_REG	,"SBC"	,MIZ_m|IM8	,REG		,0		,0		},
{	0x7C	,0		,3	,MREG_REG	,"AD"	,MIX_p|IM8	,REG		,0		,0		},
{	0x7C	,0x80	,3	,MREG_REG	,"AD"	,MIX_m|IM8	,REG		,0		,0		},
{	0x7D	,0		,3	,MREG_REG	,"AD"	,MIZ_p|IM8	,REG		,0		,0		},
{	0x7D	,0x80	,3	,MREG_REG	,"AD"	,MIZ_m|IM8	,REG		,0		,0		},
{	0x7E	,0		,3	,MREG_REG	,"SB"	,MIX_p|IM8	,REG		,0		,0		},
{	0x7E	,0x80	,3	,MREG_REG	,"SB"	,MIX_m|IM8	,REG		,0		,0		},
{	0x7F	,0		,3	,MREG_REG	,"SB"	,MIZ_p|IM8	,REG		,0		,0		},
{	0x7F	,0x80	,3	,MREG_REG	,"SB"	,MIZ_m|IM8	,REG		,0		,0		},
{	0x80	,0		,2	,REG_NU		,"ADCW"	,REG		,RSX		,0		,0		},
{	0x80	,0x20	,2	,REG_NU		,"ADCW"	,REG		,RSY		,0		,0		},
{	0x80	,0x40	,2	,REG_NU		,"ADCW"	,REG		,RSZ		,0		,0		},
{	0x80	,0x60	,3	,REG_REG	,"ADCW"	,REG		,REG		,0		,0		},
{	0x80	,0x80	,3	,REG_IM7	,"ADCW"	,REG		,RSX		,IM7	,0		},
{	0x80	,0xa0	,3	,REG_IM7	,"ADCW"	,REG		,RSY		,IM7	,0		},
{	0x80	,0xc0	,3	,REG_IM7	,"ADCW"	,REG		,RSZ		,IM7	,0		},
{	0x80	,0xe0	,4	,REG2_IM7	,"ADCW"	,REG		,REG		,IM7	,0		},
{	0x81	,0		,2	,REG_NU		,"SBCW"	,REG		,RSX		,0		,0		},
{	0x81	,0x20	,2	,REG_NU		,"SBCW"	,REG		,RSY		,0		,0		},
{	0x81	,0x40	,2	,REG_NU		,"SBCW"	,REG		,RSZ		,0		,0		},
{	0x81	,0x60	,3	,REG_REG	,"SBCW"	,REG		,REG		,0		,0		},
{	0x81	,0x80	,3	,REG_IM7	,"SBCW"	,REG		,RSX		,IM7	,0		},
{	0x81	,0xa0	,3	,REG_IM7	,"SBCW"	,REG		,RSY		,IM7	,0		},
{	0x81	,0xc0	,3	,REG_IM7	,"SBCW"	,REG		,RSZ		,IM7	,0		},
{	0x81	,0xe0	,4	,REG2_IM7	,"SBCW"	,REG		,REG		,IM7	,0		},
{	0x82	,0		,2	,REG_NU		,"LDW"	,REG		,RSX		,0		,0		},
{	0x82	,0x20	,2	,REG_NU		,"LDW"	,REG		,RSY		,0		,0		},
{	0x82	,0x40	,2	,REG_NU		,"LDW"	,REG		,RSZ		,0		,0		},
{	0x82	,0x60	,3	,REG_REG	,"LDW"	,REG		,REG		,0		,0		},
{	0x82	,0x80	,3	,REG_IM7	,"LDW"	,REG		,RSX		,IM7	,0		},
{	0x82	,0xa0	,3	,REG_IM7	,"LDW"	,REG		,RSY		,IM7	,0		},
{	0x82	,0xc0	,3	,REG_IM7	,"LDW"	,REG		,RSZ		,IM7	,0		},
{	0x82	,0xe0	,4	,REG2_IM7	,"LDW"	,REG		,REG		,IM7	,0		},
{	0x83	,0		,2	,REG_NU		,"LDCW"	,REG		,RSX		,0		,0		},/* NOP instruction */
{	0x83	,0x20	,2	,REG_NU		,"LDCW"	,REG		,RSY		,0		,0		},/* NOP instruction */
{	0x83	,0x40	,2	,REG_NU		,"LDCW"	,REG		,RSZ		,0		,0		},/* NOP instruction */
{	0x83	,0x60	,3	,REG_REG	,"LDCW"	,REG		,REG		,0		,0		},/* NOP instruction */
{	0x83	,0x80	,3	,REG_IM7	,"LDCW"	,REG		,RSX		,IM7	,0		},/* NOP instruction */
{	0x83	,0xa0	,3	,REG_IM7	,"LDCW"	,REG		,RSY		,IM7	,0		},/* NOP instruction */
{	0x83	,0xc0	,3	,REG_IM7	,"LDCW"	,REG		,RSZ		,IM7	,0		},/* NOP instruction */
{	0x83	,0xe0	,4	,REG2_IM7	,"LDCW"	,REG		,REG		,IM7	,0		},/* NOP instruction */
{	0x84	,0		,2	,REG_NU		,"ANCW"	,REG		,RSX		,0		,0		},
{	0x84	,0x20	,2	,REG_NU		,"ANCW"	,REG		,RSY		,0		,0		},
{	0x84	,0x40	,2	,REG_NU		,"ANCW"	,REG		,RSZ		,0		,0		},
{	0x84	,0x60	,3	,REG_REG	,"ANCW"	,REG		,REG		,0		,0		},
{	0x84	,0x80	,3	,REG_IM7	,"ANCW"	,REG		,RSX		,IM7	,0		},
{	0x84	,0xa0	,3	,REG_IM7	,"ANCW"	,REG		,RSY		,IM7	,0		},
{	0x84	,0xc0	,3	,REG_IM7	,"ANCW"	,REG		,RSZ		,IM7	,0		},
{	0x84	,0xe0	,4	,REG2_IM7	,"ANCW"	,REG		,REG		,IM7	,0		},
{	0x85	,0		,2	,REG_NU		,"NACW"	,REG		,RSX		,0		,0		},
{	0x85	,0x20	,2	,REG_NU		,"NACW"	,REG		,RSY		,0		,0		},
{	0x85	,0x40	,2	,REG_NU		,"NACW"	,REG		,RSZ		,0		,0		},
{	0x85	,0x60	,3	,REG_REG	,"NACW"	,REG		,REG		,0		,0		},
{	0x85	,0x80	,3	,REG_IM7	,"NACW"	,REG		,RSX		,IM7	,0		},
{	0x85	,0xa0	,3	,REG_IM7	,"NACW"	,REG		,RSY		,IM7	,0		},
{	0x85	,0xc0	,3	,REG_IM7	,"NACW"	,REG		,RSZ		,IM7	,0		},
{	0x85	,0xe0	,4	,REG2_IM7	,"NACW"	,REG		,REG		,IM7	,0		},
{	0x86	,0		,2	,REG_NU		,"ORCW"	,REG		,RSX		,0		,0		},
{	0x86	,0x20	,2	,REG_NU		,"ORCW"	,REG		,RSY		,0		,0		},
{	0x86	,0x40	,2	,REG_NU		,"ORCW"	,REG		,RSZ		,0		,0		},
{	0x86	,0x60	,3	,REG_REG	,"ORCW"	,REG		,REG		,0		,0		},
{	0x86	,0x80	,3	,REG_IM7	,"ORCW"	,REG		,RSX		,IM7	,0		},
{	0x86	,0xa0	,3	,REG_IM7	,"ORCW"	,REG		,RSY		,IM7	,0		},
{	0x86	,0xc0	,3	,REG_IM7	,"ORCW"	,REG		,RSZ		,IM7	,0		},
{	0x86	,0xe0	,4	,REG2_IM7	,"ORCW"	,REG		,REG		,IM7	,0		},
{	0x87	,0		,2	,REG_NU		,"XRCW"	,REG		,RSX		,0		,0		},
{	0x87	,0x20	,2	,REG_NU		,"XRCW"	,REG		,RSY		,0		,0		},
{	0x87	,0x40	,2	,REG_NU		,"XRCW"	,REG		,RSZ		,0		,0		},
{	0x87	,0x60	,3	,REG_REG	,"XRCW"	,REG		,REG		,0		,0		},
{	0x87	,0x80	,3	,REG_IM7	,"XRCW"	,REG		,RSX		,IM7	,0		},
{	0x87	,0xa0	,3	,REG_IM7	,"XRCW"	,REG		,RSY		,IM7	,0		},
{	0x87	,0xc0	,3	,REG_IM7	,"XRCW"	,REG		,RSZ		,IM7	,0		},
{	0x87	,0xe0	,4	,REG2_IM7	,"XRCW"	,REG		,REG		,IM7	,0		},
{	0x88	,0		,2	,REG_NU		,"ADW"	,REG		,RSX		,0		,0		},
{	0x88	,0x20	,2	,REG_NU		,"ADW"	,REG		,RSY		,0		,0		},
{	0x88	,0x40	,2	,REG_NU		,"ADW"	,REG		,RSZ		,0		,0		},
{	0x88	,0x60	,3	,REG_REG	,"ADW"	,REG		,REG		,0		,0		},
{	0x88	,0x80	,3	,REG_IM7	,"ADW"	,REG		,RSX		,IM7	,0		},
{	0x88	,0xa0	,3	,REG_IM7	,"ADW"	,REG		,RSY		,IM7	,0		},
{	0x88	,0xc0	,3	,REG_IM7	,"ADW"	,REG		,RSZ		,IM7	,0		},
{	0x88	,0xe0	,4	,REG2_IM7	,"ADW"	,REG		,REG		,IM7	,0		},
{	0x89	,0		,2	,REG_NU		,"SBW"	,REG		,RSX		,0		,0		},
{	0x89	,0x20	,2	,REG_NU		,"SBW"	,REG		,RSY		,0		,0		},
{	0x89	,0x40	,2	,REG_NU		,"SBW"	,REG		,RSZ		,0		,0		},
{	0x89	,0x60	,3	,REG_REG	,"SBW"	,REG		,REG		,0		,0		},
{	0x89	,0x80	,3	,REG_IM7	,"SBW"	,REG		,RSX		,IM7	,0		},
{	0x89	,0xa0	,3	,REG_IM7	,"SBW"	,REG		,RSY		,IM7	,0		},
{	0x89	,0xc0	,3	,REG_IM7	,"SBW"	,REG		,RSZ		,IM7	,0		},
{	0x89	,0xe0	,4	,REG2_IM7	,"SBW"	,REG		,REG		,IM7	,0		},
{	0x8A	,0		,2	,REG_NU		,"ADBW"	,REG		,RSX		,0		,0		},
{	0x8A	,0x20	,2	,REG_NU		,"ADBW"	,REG		,RSY		,0		,0		},
{	0x8A	,0x40	,2	,REG_NU		,"ADBW"	,REG		,RSZ		,0		,0		},
{	0x8A	,0x60	,3	,REG_REG	,"ADBW"	,REG		,REG		,0		,0		},
{	0x8A	,0x80	,3	,REG_IM7	,"ADBW"	,REG		,RSX		,IM7	,0		},
{	0x8A	,0xa0	,3	,REG_IM7	,"ADBW"	,REG		,RSY		,IM7	,0		},
{	0x8A	,0xc0	,3	,REG_IM7	,"ADBW"	,REG		,RSZ		,IM7	,0		},
{	0x8A	,0xe0	,4	,REG2_IM7	,"ADBW"	,REG		,REG		,IM7	,0		},
{	0x8B	,0		,2	,REG_NU		,"SBBW"	,REG		,RSX		,0		,0		},
{	0x8B	,0x20	,2	,REG_NU		,"SBBW"	,REG		,RSY		,0		,0		},
{	0x8B	,0x40	,2	,REG_NU		,"SBBW"	,REG		,RSZ		,0		,0		},
{	0x8B	,0x60	,3	,REG_REG	,"SBBW"	,REG		,REG		,0		,0		},
{	0x8B	,0x80	,3	,REG_IM7	,"SBBW"	,REG		,RSX		,IM7	,0		},
{	0x8B	,0xa0	,3	,REG_IM7	,"SBBW"	,REG		,RSY		,IM7	,0		},
{	0x8B	,0xc0	,3	,REG_IM7	,"SBBW"	,REG		,RSZ		,IM7	,0		},
{	0x8B	,0xe0	,4	,REG2_IM7	,"SBBW"	,REG		,REG		,IM7	,0		},
{	0x8C	,0		,2	,REG_NU		,"ANW"	,REG		,RSX		,0		,0		},
{	0x8C	,0x20	,2	,REG_NU		,"ANW"	,REG		,RSY		,0		,0		},
{	0x8C	,0x40	,2	,REG_NU		,"ANW"	,REG		,RSZ		,0		,0		},
{	0x8C	,0x60	,3	,REG_REG	,"ANW"	,REG		,REG		,0		,0		},
{	0x8C	,0x80	,3	,REG_IM7	,"ANW"	,REG		,RSX		,IM7	,0		},
{	0x8C	,0xa0	,3	,REG_IM7	,"ANW"	,REG		,RSY		,IM7	,0		},
{	0x8C	,0xc0	,3	,REG_IM7	,"ANW"	,REG		,RSZ		,IM7	,0		},
{	0x8C	,0xe0	,4	,REG2_IM7	,"ANW"	,REG		,REG		,IM7	,0		},
{	0x8D	,0		,2	,REG_NU		,"NAW"	,REG		,RSX		,0		,0		},
{	0x8D	,0x20	,2	,REG_NU		,"NAW"	,REG		,RSY		,0		,0		},
{	0x8D	,0x40	,2	,REG_NU		,"NAW"	,REG		,RSZ		,0		,0		},
{	0x8D	,0x60	,3	,REG_REG	,"NAW"	,REG		,REG		,0		,0		},
{	0x8D	,0x80	,3	,REG_IM7	,"NAW"	,REG		,RSX		,IM7	,0		},
{	0x8D	,0xa0	,3	,REG_IM7	,"NAW"	,REG		,RSY		,IM7	,0		},
{	0x8D	,0xc0	,3	,REG_IM7	,"NAW"	,REG		,RSZ		,IM7	,0		},
{	0x8D	,0xe0	,4	,REG2_IM7	,"NAW"	,REG		,REG		,IM7	,0		},
{	0x8E	,0		,2	,REG_NU		,"ORW"	,REG		,RSX		,0		,0		},
{	0x8E	,0x20	,2	,REG_NU		,"ORW"	,REG		,RSY		,0		,0		},
{	0x8E	,0x40	,2	,REG_NU		,"ORW"	,REG		,RSZ		,0		,0		},
{	0x8E	,0x60	,3	,REG_REG	,"ORW"	,REG		,REG		,0		,0		},
{	0x8E	,0x80	,3	,REG_IM7	,"ORW"	,REG		,RSX		,IM7	,0		},
{	0x8E	,0xa0	,3	,REG_IM7	,"ORW"	,REG		,RSY		,IM7	,0		},
{	0x8E	,0xc0	,3	,REG_IM7	,"ORW"	,REG		,RSZ		,IM7	,0		},
{	0x8E	,0xe0	,4	,REG2_IM7	,"ORW"	,REG		,REG		,IM7	,0		},
{	0x8F	,0		,2	,REG_NU		,"XRW"	,REG		,RSX		,0		,0		},
{	0x8F	,0x20	,2	,REG_NU		,"XRW"	,REG		,RSY		,0		,0		},
{	0x8F	,0x40	,2	,REG_NU		,"XRW"	,REG		,RSZ		,0		,0		},
{	0x8F	,0x60	,3	,REG_REG	,"XRW"	,REG		,REG		,0		,0		},
{	0x8F	,0x80	,3	,REG_IM7	,"XRW"	,REG		,RSX		,IM7	,0		},
{	0x8F	,0xa0	,3	,REG_IM7	,"XRW"	,REG		,RSY		,IM7	,0		},
{	0x8F	,0xc0	,3	,REG_IM7	,"XRW"	,REG		,RSZ		,IM7	,0		},
{	0x8F	,0xe0	,4	,REG2_IM7	,"XRW"	,REG		,REG		,IM7	,0		},
{	0x90	,0		,2	,REG_NU		,"STW"	,REG		,MRSX		,0		,0		},
{	0x90	,0x20	,2	,REG_NU		,"STW"	,REG		,MRSY		,0		,0		},
{	0x90	,0x40	,2	,REG_NU		,"STW"	,REG		,MRSZ		,0		,0		},
{	0x90	,0x60	,3	,REG_REG	,"STW"	,REG		,MREG		,0		,0		},
{	0x90	,0x80	,3	,REG_IM7	,"STW"	,REG		,MRSX		,IM7	,0		},
{	0x90	,0xa0	,3	,REG_IM7	,"STW"	,REG		,MRSY		,IM7	,0		},
{	0x90	,0xc0	,3	,REG_IM7	,"STW"	,REG		,MRSZ		,IM7	,0		},
{	0x90	,0xe0	,4	,REG2_IM7	,"STW"	,REG		,MREG		,IM7	,0		},
{	0x91	,0		,2	,REG_NU		,"LDW"	,REG		,MRSX		,0		,0		},
{	0x91	,0x20	,2	,REG_NU		,"LDW"	,REG		,MRSY		,0		,0		},
{	0x91	,0x40	,2	,REG_NU		,"LDW"	,REG		,MRSZ		,0		,0		},
{	0x91	,0x60	,3	,REG_REG	,"LDW"	,REG		,MREG		,0		,0		},
{	0x91	,0x80	,3	,REG_IM7	,"LDW"	,REG		,MRSX		,IM7	,0		},
{	0x91	,0xa0	,3	,REG_IM7	,"LDW"	,REG		,MRSY		,IM7	,0		},
{	0x91	,0xc0	,3	,REG_IM7	,"LDW"	,REG		,MRSZ		,IM7	,0		},
{	0x91	,0xe0	,4	,REG2_IM7	,"LDW"	,REG		,MREG		,IM7	,0		},
{	0x92	,0		,2	,REG_NU		,"STLW"	,REG		,0			,0		,0		},/* LCD出力 */
{	0x92	,0x80	,3	,REG_NUJ	,"STLW"	,REG		,IM7		,0		,0		},/* LCD出力 */
{	0x93	,0		,2	,REG_NU		,"LDLW"	,REG		,0			,0		,0		},/* LCD入力 */
{	0x93	,0x80	,3	,REG_NUJ	,"LDLW"	,REG		,IM7		,0		,0		},/* LCD入力 */
{	0x94	,0		,2	,REG_NU		,"PPOW"	,REG		,0			,0		,0		},/* LCD制御ポート */
{	0x94	,0x80	,3	,REG_NUJ	,"PPOW"	,REG		,IM7		,0		,0		},/* LCD制御ポート */
{	0x95	,0		,2	,IR_REG		,"PSRW"	,SX			,REG		,0		,0		},/* 特定Index指定(SX) */
{	0x95	,0x20	,2	,IR_REG		,"PSRW"	,SY			,REG		,0		,0		},/* 特定Index指定(SY) */
{	0x95	,0x40	,2	,IR_REG		,"PSRW"	,SZ			,REG		,0		,0		},/* 特定Index指定(SZ) */
{	0x95	,0x80	,3	,IR_REGJ	,"PSRW"	,SX			,REG		,IM7	,0		},/* 特定Index指定(SX) */
{	0x95	,0xa0	,3	,IR_REGJ	,"PSRW"	,SY			,REG		,IM7	,0		},/* 特定Index指定(SY) */
{	0x95	,0xc0	,3	,IR_REGJ	,"PSRW"	,SZ			,REG		,IM7	,0		},/* 特定Index指定(SZ) */
{	0x96	,0		,2	,IR_REG		,"PRE"	,IX			,REG		,0		,0		},
{	0x96	,0x20	,2	,IR_REG		,"PRE"	,IY			,REG		,0		,0		},
{	0x96	,0x40	,2	,IR_REG		,"PRE"	,IZ			,REG		,0		,0		},
{	0x96	,0x60	,2	,IR_REG		,"PRE"	,US			,REG		,0		,0		},
{	0x96	,0x80	,3	,IR_REGJ	,"PRE"	,IX			,REG		,IM7	,0		},
{	0x96	,0xa0	,3	,IR_REGJ	,"PRE"	,IY			,REG		,IM7	,0		},
{	0x96	,0xc0	,3	,IR_REGJ	,"PRE"	,IZ			,REG		,IM7	,0		},
{	0x96	,0xe0	,3	,IR_REGJ	,"PRE"	,US			,REG		,IM7	,0		},
{	0x97	,0		,2	,IR_REG		,"PRE"	,SS			,REG		,0		,0		},
{	0x97	,0x80	,3	,IR_REGJ	,"PRE"	,SS			,REG		,IM7	,0		},
{	0x98	,0		,2	,REG_NU		,"RODW"	,REG		,0			,0		,0		},
{	0x98	,0x20	,2	,REG_NU		,"ROUW"	,REG		,0			,0		,0		},
{	0x98	,0x40	,2	,REG_NU		,"BIDW"	,REG		,0			,0		,0		},
{	0x98	,0x60	,2	,REG_NU		,"BIUW"	,REG		,0			,0		,0		},
{	0x98	,0x80	,3	,REG_NUJ	,"RODW"	,REG		,IM7		,0		,0		},
{	0x98	,0xa0	,3	,REG_NUJ	,"ROUW"	,REG		,IM7		,0		,0		},
{	0x98	,0xc0	,3	,REG_NUJ	,"BIDW"	,REG		,IM7		,0		,0		},
{	0x98	,0xe0	,3	,REG_NUJ	,"BIUW"	,REG		,IM7		,0		,0		},
{	0x9A	,0		,2	,REG_NU		,"DIDW"	,REG		,0			,0		,0		},
{	0x9A	,0x20	,2	,REG_NU		,"DIUW"	,REG		,0			,0		,0		},
{	0x9A	,0x40	,2	,REG_NU		,"BYDW"	,REG		,0			,0		,0		},
{	0x9A	,0x60	,2	,REG_NU		,"BYUW"	,REG		,0			,0		,0		},
{	0x9A	,0x80	,3	,REG_NUJ	,"DIDW"	,REG		,IM7		,0		,0		},
{	0x9A	,0xa0	,3	,REG_NUJ	,"DIUW"	,REG		,IM7		,0		,0		},
{	0x9A	,0xc0	,3	,REG_NUJ	,"BYDW"	,REG		,IM7		,0		,0		},
{	0x9A	,0xe0	,3	,REG_NUJ	,"BYUW"	,REG		,IM7		,0		,0		},
{	0x9B	,0		,2	,REG_NU		,"CMPW"	,REG		,0			,0		,0		},
{	0x9B	,0x40	,2	,REG_NU		,"INVW"	,REG		,0			,0		,0		},
{	0x9B	,0x80	,3	,REG_NUJ	,"CMPW"	,REG		,IM7		,0		,0		},
{	0x9B	,0xc0	,3	,REG_NUJ	,"INVW"	,REG		,IM7		,0		,0		},
{	0x9C	,0		,2	,REG_NU		,"GPOW"	,REG		,0			,0		,0		},
{	0x9C	,0x40	,2	,REG_NU		,"GFLW"	,REG		,0			,0		,0		},
{	0x9C	,0x80	,3	,REG_NUJ	,"GPOW"	,REG		,IM7		,0		,0		},
{	0x9C	,0xc0	,3	,REG_NUJ	,"GFLW"	,REG		,IM7		,0		,0		},
{	0x9D	,0		,2	,IR_REG		,"GSRW"	,SX			,REG		,0		,0		},/* 特定Index取得(SX) */
{	0x9D	,0x20	,2	,IR_REG		,"GSRW"	,SY			,REG		,0		,0		},/* 特定Index取得(SY) */
{	0x9D	,0x40	,2	,IR_REG		,"GSRW"	,SZ			,REG		,0		,0		},/* 特定Index取得(SZ) */
{	0x9D	,0x80	,3	,IR_REGJ	,"GSRW"	,SX			,REG		,IM7	,0		},/* 特定Index取得(SX) */
{	0x9D	,0xa0	,3	,IR_REGJ	,"GSRW"	,SY			,REG		,IM7	,0		},/* 特定Index取得(SY) */
{	0x9D	,0xc0	,3	,IR_REGJ	,"GSRW"	,SZ			,REG		,IM7	,0		},/* 特定Index取得(SZ) */
{	0x9E	,0		,2	,IR_REG		,"GRE"	,IX			,REG		,0		,0		},
{	0x9E	,0x20	,2	,IR_REG		,"GRE"	,IY			,REG		,0		,0		},
{	0x9E	,0x40	,2	,IR_REG		,"GRE"	,IZ			,REG		,0		,0		},
{	0x9E	,0x60	,2	,IR_REG		,"GRE"	,US			,REG		,0		,0		},
{	0x9E	,0x80	,3	,IR_REGJ	,"GRE"	,IX			,REG		,IM7	,0		},
{	0x9E	,0xa0	,3	,IR_REGJ	,"GRE"	,IY			,REG		,IM7	,0		},
{	0x9E	,0xc0	,3	,IR_REGJ	,"GRE"	,IZ			,REG		,IM7	,0		},
{	0x9E	,0xe0	,3	,IR_REGJ	,"GRE"	,US			,REG		,IM7	,0		},
{	0x9F	,0		,2	,IR_REG		,"GRE"	,SS			,REG		,0		,0		},
{	0x9F	,0x60	,2	,IR_REG		,"GRE"	,KY			,REG		,0		,0		},
{	0x9F	,0x80	,3	,IR_REGJ	,"GRE"	,SS			,REG		,IM7	,0		},
{	0x9F	,0xe0	,3	,IR_REGJ	,"GRE"	,KY			,REG		,IM7	,0		},
{	0xA0	,0		,2	,REG_NU		,"STW"	,REG		,MIX_p|RSX	,0		,0		},
{	0xA0	,0x20	,2	,REG_NU		,"STW"	,REG		,MIX_p|RSY	,0		,0		},
{	0xA0	,0x40	,2	,REG_NU		,"STW"	,REG		,MIX_p|RSZ	,0		,0		},
{	0xA0	,0x60	,3	,REG_REG	,"STW"	,REG		,MIX_p|REG	,0		,0		},
{	0xA0	,0x80	,2	,REG_NU		,"STW"	,REG		,MIX_m|RSX	,0		,0		},
{	0xA0	,0xa0	,2	,REG_NU		,"STW"	,REG		,MIX_m|RSY	,0		,0		},
{	0xA0	,0xc0	,2	,REG_NU		,"STW"	,REG		,MIX_m|RSZ	,0		,0		},
{	0xA0	,0xe0	,3	,REG_REG	,"STW"	,REG		,MIX_m|REG	,0		,0		},
{	0xA1	,0		,2	,REG_NU		,"STW"	,REG		,MIZ_p|RSX	,0		,0		},
{	0xA1	,0x20	,2	,REG_NU		,"STW"	,REG		,MIZ_p|RSY	,0		,0		},
{	0xA1	,0x40	,2	,REG_NU		,"STW"	,REG		,MIZ_p|RSZ	,0		,0		},
{	0xA1	,0x60	,3	,REG_REG	,"STW"	,REG		,MIZ_p|REG	,0		,0		},
{	0xA1	,0x80	,2	,REG_NU		,"STW"	,REG		,MIZ_m|RSX	,0		,0		},
{	0xA1	,0xa0	,2	,REG_NU		,"STW"	,REG		,MIZ_m|RSY	,0		,0		},
{	0xA1	,0xc0	,2	,REG_NU		,"STW"	,REG		,MIZ_m|RSZ	,0		,0		},
{	0xA1	,0xe0	,3	,REG_REG	,"STW"	,REG		,MIZ_m|REG	,0		,0		},
{	0xA2	,0		,2	,REG_NU		,"STIW"	,REG		,MIX_p|RSX	,0		,0		},
{	0xA2	,0x20	,2	,REG_NU		,"STIW"	,REG		,MIX_p|RSY	,0		,0		},
{	0xA2	,0x40	,2	,REG_NU		,"STIW"	,REG		,MIX_p|RSZ	,0		,0		},
{	0xA2	,0x60	,3	,REG_REG	,"STIW"	,REG		,MIX_p|REG	,0		,0		},
{	0xA2	,0x80	,2	,REG_NU		,"STIW"	,REG		,MIX_m|RSX	,0		,0		},
{	0xA2	,0xa0	,2	,REG_NU		,"STIW"	,REG		,MIX_m|RSY	,0		,0		},
{	0xA2	,0xc0	,2	,REG_NU		,"STIW"	,REG		,MIX_m|RSZ	,0		,0		},
{	0xA2	,0xe0	,3	,REG_REG	,"STIW"	,REG		,MIX_m|REG	,0		,0		},
{	0xA3	,0		,2	,REG_NU		,"STIW"	,REG		,MIZ_p|RSX	,0		,0		},
{	0xA3	,0x20	,2	,REG_NU		,"STIW"	,REG		,MIZ_p|RSY	,0		,0		},
{	0xA3	,0x40	,2	,REG_NU		,"STIW"	,REG		,MIZ_p|RSZ	,0		,0		},
{	0xA3	,0x60	,3	,REG_REG	,"STIW"	,REG		,MIZ_p|REG	,0		,0		},
{	0xA3	,0x80	,2	,REG_NU		,"STIW"	,REG		,MIZ_m|RSX	,0		,0		},
{	0xA3	,0xa0	,2	,REG_NU		,"STIW"	,REG		,MIZ_m|RSY	,0		,0		},
{	0xA3	,0xc0	,2	,REG_NU		,"STIW"	,REG		,MIZ_m|RSZ	,0		,0		},
{	0xA3	,0xe0	,3	,REG_REG	,"STIW"	,REG		,MIZ_m|REG	,0		,0		},
{	0xA4	,0		,2	,REG_NU		,"STDW"	,REG		,MIX_p|RSX	,0		,0		},
{	0xA4	,0x20	,2	,REG_NU		,"STDW"	,REG		,MIX_p|RSY	,0		,0		},
{	0xA4	,0x40	,2	,REG_NU		,"STDW"	,REG		,MIX_p|RSZ	,0		,0		},
{	0xA4	,0x60	,3	,REG_REG	,"STDW"	,REG		,MIX_p|REG	,0		,0		},
{	0xA4	,0x80	,2	,REG_NU		,"STDW"	,REG		,MIX_m|RSX	,0		,0		},
{	0xA4	,0xa0	,2	,REG_NU		,"STDW"	,REG		,MIX_m|RSY	,0		,0		},
{	0xA4	,0xc0	,2	,REG_NU		,"STDW"	,REG		,MIX_m|RSZ	,0		,0		},
{	0xA4	,0xe0	,3	,REG_REG	,"STDW"	,REG		,MIX_m|REG	,0		,0		},
{	0xA5	,0		,2	,REG_NU		,"STDW"	,REG		,MIZ_p|RSX	,0		,0		},
{	0xA5	,0x20	,2	,REG_NU		,"STDW"	,REG		,MIZ_p|RSY	,0		,0		},
{	0xA5	,0x40	,2	,REG_NU		,"STDW"	,REG		,MIZ_p|RSZ	,0		,0		},
{	0xA5	,0x60	,3	,REG_REG	,"STDW"	,REG		,MIZ_p|REG	,0		,0		},
{	0xA5	,0x80	,2	,REG_NU		,"STDW"	,REG		,MIZ_m|RSX	,0		,0		},
{	0xA5	,0xa0	,2	,REG_NU		,"STDW"	,REG		,MIZ_m|RSY	,0		,0		},
{	0xA5	,0xc0	,2	,REG_NU		,"STDW"	,REG		,MIZ_m|RSZ	,0		,0		},
{	0xA5	,0xe0	,3	,REG_REG	,"STDW"	,REG		,MIZ_m|REG	,0		,0		},
{	0xA6	,0		,2	,REG_NU		,"PHSW"	,REG		,0			,0		,0		},
{	0xA7	,0		,2	,REG_NU		,"PHUW"	,REG		,0			,0		,0		},
{	0xA8	,0		,2	,REG_NU		,"LDW"	,REG		,MIX_p|RSX	,0		,0		},
{	0xA8	,0x20	,2	,REG_NU		,"LDW"	,REG		,MIX_p|RSY	,0		,0		},
{	0xA8	,0x40	,2	,REG_NU		,"LDW"	,REG		,MIX_p|RSZ	,0		,0		},
{	0xA8	,0x60	,3	,REG_REG	,"LDW"	,REG		,MIX_p|REG	,0		,0		},
{	0xA8	,0x80	,2	,REG_NU		,"LDW"	,REG		,MIX_m|RSX	,0		,0		},
{	0xA8	,0xa0	,2	,REG_NU		,"LDW"	,REG		,MIX_m|RSY	,0		,0		},
{	0xA8	,0xc0	,2	,REG_NU		,"LDW"	,REG		,MIX_m|RSZ	,0		,0		},
{	0xA8	,0xe0	,3	,REG_REG	,"LDW"	,REG		,MIX_m|REG	,0		,0		},
{	0xA9	,0		,2	,REG_NU		,"LDW"	,REG		,MIZ_p|RSX	,0		,0		},
{	0xA9	,0x20	,2	,REG_NU		,"LDW"	,REG		,MIZ_p|RSY	,0		,0		},
{	0xA9	,0x40	,2	,REG_NU		,"LDW"	,REG		,MIZ_p|RSZ	,0		,0		},
{	0xA9	,0x60	,3	,REG_REG	,"LDW"	,REG		,MIZ_p|REG	,0		,0		},
{	0xA9	,0x80	,2	,REG_NU		,"LDW"	,REG		,MIZ_m|RSX	,0		,0		},
{	0xA9	,0xa0	,2	,REG_NU		,"LDW"	,REG		,MIZ_m|RSY	,0		,0		},
{	0xA9	,0xc0	,2	,REG_NU		,"LDW"	,REG		,MIZ_m|RSZ	,0		,0		},
{	0xA9	,0xe0	,3	,REG_REG	,"LDW"	,REG		,MIZ_m|REG	,0		,0		},
{	0xAA	,0		,2	,REG_NU		,"LDIW"	,REG		,MIX_p|RSX	,0		,0		},
{	0xAA	,0x20	,2	,REG_NU		,"LDIW"	,REG		,MIX_p|RSY	,0		,0		},
{	0xAA	,0x40	,2	,REG_NU		,"LDIW"	,REG		,MIX_p|RSZ	,0		,0		},
{	0xAA	,0x60	,3	,REG_REG	,"LDIW"	,REG		,MIX_p|REG	,0		,0		},
{	0xAA	,0x80	,2	,REG_NU		,"LDIW"	,REG		,MIX_m|RSX	,0		,0		},
{	0xAA	,0xa0	,2	,REG_NU		,"LDIW"	,REG		,MIX_m|RSY	,0		,0		},
{	0xAA	,0xc0	,2	,REG_NU		,"LDIW"	,REG		,MIX_m|RSZ	,0		,0		},
{	0xAA	,0xe0	,3	,REG_REG	,"LDIW"	,REG		,MIX_m|REG	,0		,0		},
{	0xAB	,0		,2	,REG_NU		,"LDIW"	,REG		,MIZ_p|RSX	,0		,0		},
{	0xAB	,0x20	,2	,REG_NU		,"LDIW"	,REG		,MIZ_p|RSY	,0		,0		},
{	0xAB	,0x40	,2	,REG_NU		,"LDIW"	,REG		,MIZ_p|RSZ	,0		,0		},
{	0xAB	,0x60	,3	,REG_REG	,"LDIW"	,REG		,MIZ_p|REG	,0		,0		},
{	0xAB	,0x80	,2	,REG_NU		,"LDIW"	,REG		,MIZ_m|RSX	,0		,0		},
{	0xAB	,0xa0	,2	,REG_NU		,"LDIW"	,REG		,MIZ_m|RSY	,0		,0		},
{	0xAB	,0xc0	,2	,REG_NU		,"LDIW"	,REG		,MIZ_m|RSZ	,0		,0		},
{	0xAB	,0xe0	,3	,REG_REG	,"LDIW"	,REG		,MIZ_m|REG	,0		,0		},
{	0xAC	,0		,2	,REG_NU		,"LDDW"	,REG		,MIX_p|RSX	,0		,0		},
{	0xAC	,0x20	,2	,REG_NU		,"LDDW"	,REG		,MIX_p|RSY	,0		,0		},
{	0xAC	,0x40	,2	,REG_NU		,"LDDW"	,REG		,MIX_p|RSZ	,0		,0		},
{	0xAC	,0x60	,3	,REG_REG	,"LDDW"	,REG		,MIX_p|REG	,0		,0		},
{	0xAC	,0x80	,2	,REG_NU		,"LDDW"	,REG		,MIX_m|RSX	,0		,0		},
{	0xAC	,0xa0	,2	,REG_NU		,"LDDW"	,REG		,MIX_m|RSY	,0		,0		},
{	0xAC	,0xc0	,2	,REG_NU		,"LDDW"	,REG		,MIX_m|RSZ	,0		,0		},
{	0xAC	,0xe0	,3	,REG_REG	,"LDDW"	,REG		,MIX_m|REG	,0		,0		},
{	0xAD	,0		,2	,REG_NU		,"LDDW"	,REG		,MIZ_p|RSX	,0		,0		},
{	0xAD	,0x20	,2	,REG_NU		,"LDDW"	,REG		,MIZ_p|RSY	,0		,0		},
{	0xAD	,0x40	,2	,REG_NU		,"LDDW"	,REG		,MIZ_p|RSZ	,0		,0		},
{	0xAD	,0x60	,3	,REG_REG	,"LDDW"	,REG		,MIZ_p|REG	,0		,0		},
{	0xAD	,0x80	,2	,REG_NU		,"LDDW"	,REG		,MIZ_m|RSX	,0		,0		},
{	0xAD	,0xa0	,2	,REG_NU		,"LDDW"	,REG		,MIZ_m|RSY	,0		,0		},
{	0xAD	,0xc0	,2	,REG_NU		,"LDDW"	,REG		,MIZ_m|RSZ	,0		,0		},
{	0xAD	,0xe0	,3	,REG_REG	,"LDDW"	,REG		,MIZ_m|REG	,0		,0		},
{	0xAE	,0		,2	,REG_NU		,"PPSW"	,REG		,0			,0		,0		},
{	0xAF	,0		,2	,REG_NU		,"PPUW"	,REG		,0			,0		,0		},
{	0xB0	,0		,2	,F_IM7		,"JR"	,ZF			,IM7		,0		,0		},
{	0xB1	,0		,2	,F_IM7		,"JR"	,NCF		,IM7		,0		,0		},
{	0xB2	,0		,2	,F_IM7		,"JR"	,LZF		,IM7		,0		,0		},
{	0xB3	,0		,2	,F_IM7		,"JR"	,UZF		,IM7		,0		,0		},
{	0xB4	,0		,2	,F_IM7		,"JR"	,NZF		,IM7		,0		,0		},
{	0xB5	,0		,2	,F_IM7		,"JR"	,CF			,IM7		,0		,0		},
{	0xB6	,0		,2	,F_IM7		,"JR"	,NLZF		,IM7		,0		,0		},
{	0xB7	,0		,2	,IM7_NU		,"JR"	,IM7		,0			,0		,0		},
{	0xB8	,0		,2	,IR_REG		,"ADCW"	,MIX_p|RSX	,REG		,0		,0		},
{	0xB8	,0x20	,2	,IR_REG		,"ADCW"	,MIX_p|RSY	,REG		,0		,0		},
{	0xB8	,0x40	,2	,IR_REG		,"ADCW"	,MIX_p|RSZ	,REG		,0		,0		},
{	0xB8	,0x60	,3	,MREG_REG	,"ADCW"	,MIX_p|REG	,REG		,0		,0		},
{	0xB8	,0x80	,2	,IR_REG		,"ADCW"	,MIX_m|RSX	,REG		,0		,0		},
{	0xB8	,0xa0	,2	,IR_REG		,"ADCW"	,MIX_m|RSY	,REG		,0		,0		},
{	0xB8	,0xc0	,2	,IR_REG		,"ADCW"	,MIX_m|RSZ	,REG		,0		,0		},
{	0xB8	,0xe0	,3	,MREG_REG	,"ADCW"	,MIX_m|REG	,REG		,0		,0		},
{	0xB9	,0		,2	,IR_REG		,"ADCW"	,MIZ_p|RSX	,REG		,0		,0		},
{	0xB9	,0x20	,2	,IR_REG		,"ADCW"	,MIZ_p|RSY	,REG		,0		,0		},
{	0xB9	,0x40	,2	,IR_REG		,"ADCW"	,MIZ_p|RSZ	,REG		,0		,0		},
{	0xB9	,0x60	,3	,MREG_REG	,"ADCW"	,MIZ_p|REG	,REG		,0		,0		},
{	0xB9	,0x80	,2	,IR_REG		,"ADCW"	,MIZ_m|RSX	,REG		,0		,0		},
{	0xB9	,0xa0	,2	,IR_REG		,"ADCW"	,MIZ_m|RSY	,REG		,0		,0		},
{	0xB9	,0xc0	,2	,IR_REG		,"ADCW"	,MIZ_m|RSZ	,REG		,0		,0		},
{	0xB9	,0xe0	,3	,MREG_REG	,"ADCW"	,MIZ_m|REG	,REG		,0		,0		},
{	0xBA	,0		,2	,IR_REG		,"SBCW"	,MIX_p|RSX	,REG		,0		,0		},
{	0xBA	,0x20	,2	,IR_REG		,"SBCW"	,MIX_p|RSY	,REG		,0		,0		},
{	0xBA	,0x40	,2	,IR_REG		,"SBCW"	,MIX_p|RSZ	,REG		,0		,0		},
{	0xBA	,0x60	,3	,MREG_REG	,"SBCW"	,MIX_p|REG	,REG		,0		,0		},
{	0xBA	,0x80	,2	,IR_REG		,"SBCW"	,MIX_m|RSX	,REG		,0		,0		},
{	0xBA	,0xa0	,2	,IR_REG		,"SBCW"	,MIX_m|RSY	,REG		,0		,0		},
{	0xBA	,0xc0	,2	,IR_REG		,"SBCW"	,MIX_m|RSZ	,REG		,0		,0		},
{	0xBA	,0xe0	,3	,MREG_REG	,"SBCW"	,MIX_m|REG	,REG		,0		,0		},
{	0xBB	,0		,2	,IR_REG		,"SBCW"	,MIZ_p|RSX	,REG		,0		,0		},
{	0xBB	,0x20	,2	,IR_REG		,"SBCW"	,MIZ_p|RSY	,REG		,0		,0		},
{	0xBB	,0x40	,2	,IR_REG		,"SBCW"	,MIZ_p|RSZ	,REG		,0		,0		},
{	0xBB	,0x60	,3	,MREG_REG	,"SBCW"	,MIZ_p|REG	,REG		,0		,0		},
{	0xBB	,0x80	,2	,IR_REG		,"SBCW"	,MIZ_m|RSX	,REG		,0		,0		},
{	0xBB	,0xa0	,2	,IR_REG		,"SBCW"	,MIZ_m|RSY	,REG		,0		,0		},
{	0xBB	,0xc0	,2	,IR_REG		,"SBCW"	,MIZ_m|RSZ	,REG		,0		,0		},
{	0xBB	,0xe0	,3	,MREG_REG	,"SBCW"	,MIZ_m|REG	,REG		,0		,0		},
{	0xBC	,0		,2	,IR_REG		,"ADW"	,MIX_p|RSX	,REG		,0		,0		},
{	0xBC	,0x20	,2	,IR_REG		,"ADW"	,MIX_p|RSY	,REG		,0		,0		},
{	0xBC	,0x40	,2	,IR_REG		,"ADW"	,MIX_p|RSZ	,REG		,0		,0		},
{	0xBC	,0x60	,3	,MREG_REG	,"ADW"	,MIX_p|REG	,REG		,0		,0		},
{	0xBC	,0x80	,2	,IR_REG		,"ADW"	,MIX_m|RSX	,REG		,0		,0		},
{	0xBC	,0xa0	,2	,IR_REG		,"ADW"	,MIX_m|RSY	,REG		,0		,0		},
{	0xBC	,0xc0	,2	,IR_REG		,"ADW"	,MIX_m|RSZ	,REG		,0		,0		},
{	0xBC	,0xe0	,3	,MREG_REG	,"ADW"	,MIX_m|REG	,REG		,0		,0		},
{	0xBD	,0		,2	,IR_REG		,"ADW"	,MIZ_p|RSX	,REG		,0		,0		},
{	0xBD	,0x20	,2	,IR_REG		,"ADW"	,MIZ_p|RSY	,REG		,0		,0		},
{	0xBD	,0x40	,2	,IR_REG		,"ADW"	,MIZ_p|RSZ	,REG		,0		,0		},
{	0xBD	,0x60	,3	,MREG_REG	,"ADW"	,MIZ_p|REG	,REG		,0		,0		},
{	0xBD	,0x80	,2	,IR_REG		,"ADW"	,MIZ_m|RSX	,REG		,0		,0		},
{	0xBD	,0xa0	,2	,IR_REG		,"ADW"	,MIZ_m|RSY	,REG		,0		,0		},
{	0xBD	,0xc0	,2	,IR_REG		,"ADW"	,MIZ_m|RSZ	,REG		,0		,0		},
{	0xBD	,0xe0	,3	,MREG_REG	,"ADW"	,MIZ_m|REG	,REG		,0		,0		},
{	0xBE	,0		,2	,IR_REG		,"SBW"	,MIX_p|RSX	,REG		,0		,0		},
{	0xBE	,0x20	,2	,IR_REG		,"SBW"	,MIX_p|RSY	,REG		,0		,0		},
{	0xBE	,0x40	,2	,IR_REG		,"SBW"	,MIX_p|RSZ	,REG		,0		,0		},
{	0xBE	,0x60	,3	,MREG_REG	,"SBW"	,MIX_p|REG	,REG		,0		,0		},
{	0xBE	,0x80	,2	,IR_REG		,"SBW"	,MIX_m|RSX	,REG		,0		,0		},
{	0xBE	,0xa0	,2	,IR_REG		,"SBW"	,MIX_m|RSY	,REG		,0		,0		},
{	0xBE	,0xc0	,2	,IR_REG		,"SBW"	,MIX_m|RSZ	,REG		,0		,0		},
{	0xBE	,0xe0	,3	,MREG_REG	,"SBW"	,MIX_m|REG	,REG		,0		,0		},
{	0xBF	,0		,2	,IR_REG		,"SBW"	,MIZ_p|RSX	,REG		,0		,0		},
{	0xBF	,0x20	,2	,IR_REG		,"SBW"	,MIZ_p|RSY	,REG		,0		,0		},
{	0xBF	,0x40	,2	,IR_REG		,"SBW"	,MIZ_p|RSZ	,REG		,0		,0		},
{	0xBF	,0x60	,3	,MREG_REG	,"SBW"	,MIZ_p|REG	,REG		,0		,0		},
{	0xBF	,0x80	,2	,IR_REG		,"SBW"	,MIZ_m|RSX	,REG		,0		,0		},
{	0xBF	,0xa0	,2	,IR_REG		,"SBW"	,MIZ_m|RSY	,REG		,0		,0		},
{	0xBF	,0xc0	,2	,IR_REG		,"SBW"	,MIZ_m|RSZ	,REG		,0		,0		},
{	0xBF	,0xe0	,3	,MREG_REG	,"SBW"	,MIZ_m|REG	,REG		,0		,0		},
{	0xC0	,0		,3	,REG2_IM3	,"ADBCM",REG		,RSX		,IM3	,0		},
{	0xC0	,0x20	,3	,REG2_IM3	,"ADBCM",REG		,RSY		,IM3	,0		},
{	0xC0	,0x40	,3	,REG2_IM3	,"ADBCM",REG		,RSZ		,IM3	,0		},
{	0xC0	,0x60	,3	,REG2_IM3	,"ADBCM",REG		,REG		,IM3	,0		},
{	0xC0	,0x80	,4	,REG2_IM3J	,"ADBCM",REG		,RSX		,IM3	,IM7	},
{	0xC0	,0xa0	,4	,REG2_IM3J	,"ADBCM",REG		,RSY		,IM3	,IM7	},
{	0xC0	,0xc0	,4	,REG2_IM3J	,"ADBCM",REG		,RSZ		,IM3	,IM7	},
{	0xC0	,0xe0	,4	,REG2_IM3J	,"ADBCM",REG		,REG		,IM3	,IM7	},
{	0xC1	,0		,3	,REG2_IM3	,"SBBCM",REG		,RSX		,IM3	,0		},
{	0xC1	,0x20	,3	,REG2_IM3	,"SBBCM",REG		,RSY		,IM3	,0		},
{	0xC1	,0x40	,3	,REG2_IM3	,"SBBCM",REG		,RSZ		,IM3	,0		},
{	0xC1	,0x60	,3	,REG2_IM3	,"SBBCM",REG		,REG		,IM3	,0		},
{	0xC1	,0x80	,4	,REG2_IM3J	,"SBBCM",REG		,RSX		,IM3	,IM7	},
{	0xC1	,0xa0	,4	,REG2_IM3J	,"SBBCM",REG		,RSY		,IM3	,IM7	},
{	0xC1	,0xc0	,4	,REG2_IM3J	,"SBBCM",REG		,RSZ		,IM3	,IM7	},
{	0xC1	,0xe0	,4	,REG2_IM3J	,"SBBCM",REG		,REG		,IM3	,IM7	},
{	0xC2	,0		,3	,REG2_IM3	,"LDM"	,REG		,RSX		,IM3	,0		},
{	0xC2	,0x20	,3	,REG2_IM3	,"LDM"	,REG		,RSY		,IM3	,0		},
{	0xC2	,0x40	,3	,REG2_IM3	,"LDM"	,REG		,RSZ		,IM3	,0		},
{	0xC2	,0x60	,3	,REG2_IM3	,"LDM"	,REG		,REG		,IM3	,0		},
{	0xC2	,0x80	,4	,REG2_IM3J	,"LDM"	,REG		,RSX		,IM3	,IM7	},
{	0xC2	,0xa0	,4	,REG2_IM3J	,"LDM"	,REG		,RSY		,IM3	,IM7	},
{	0xC2	,0xc0	,4	,REG2_IM3J	,"LDM"	,REG		,RSZ		,IM3	,IM7	},
{	0xC2	,0xe0	,4	,REG2_IM3J	,"LDM"	,REG		,REG		,IM3	,IM7	},
{	0xC3	,0		,3	,REG2_IM3	,"LDCM"	,REG		,RSX		,IM3	,0		},/* NOP instruction */
{	0xC3	,0x20	,3	,REG2_IM3	,"LDCM"	,REG		,RSY		,IM3	,0		},/* NOP instruction */
{	0xC3	,0x40	,3	,REG2_IM3	,"LDCM"	,REG		,RSZ		,IM3	,0		},/* NOP instruction */
{	0xC3	,0x60	,3	,REG2_IM3	,"LDCM"	,REG		,REG		,IM3	,0		},/* NOP instruction */
{	0xC3	,0x80	,4	,REG2_IM3J	,"LDCM"	,REG		,RSX		,IM3	,IM7	},/* NOP instruction */
{	0xC3	,0xa0	,4	,REG2_IM3J	,"LDCM"	,REG		,RSY		,IM3	,IM7	},/* NOP instruction */
{	0xC3	,0xc0	,4	,REG2_IM3J	,"LDCM"	,REG		,RSZ		,IM3	,IM7	},/* NOP instruction */
{	0xC3	,0xe0	,4	,REG2_IM3J	,"LDCM"	,REG		,REG		,IM3	,IM7	},/* NOP instruction */
{	0xC4	,0		,3	,REG2_IM3	,"ANCM"	,REG		,RSX		,IM3	,0		},
{	0xC4	,0x20	,3	,REG2_IM3	,"ANCM"	,REG		,RSY		,IM3	,0		},
{	0xC4	,0x40	,3	,REG2_IM3	,"ANCM"	,REG		,RSZ		,IM3	,0		},
{	0xC4	,0x60	,3	,REG2_IM3	,"ANCM"	,REG		,REG		,IM3	,0		},
{	0xC4	,0x80	,4	,REG2_IM3J	,"ANCM"	,REG		,RSX		,IM3	,IM7	},
{	0xC4	,0xa0	,4	,REG2_IM3J	,"ANCM"	,REG		,RSY		,IM3	,IM7	},
{	0xC4	,0xc0	,4	,REG2_IM3J	,"ANCM"	,REG		,RSZ		,IM3	,IM7	},
{	0xC4	,0xe0	,4	,REG2_IM3J	,"ANCM"	,REG		,REG		,IM3	,IM7	},
{	0xC5	,0		,3	,REG2_IM3	,"NACM"	,REG		,RSX		,IM3	,0		},
{	0xC5	,0x20	,3	,REG2_IM3	,"NACM"	,REG		,RSY		,IM3	,0		},
{	0xC5	,0x40	,3	,REG2_IM3	,"NACM"	,REG		,RSZ		,IM3	,0		},
{	0xC5	,0x60	,3	,REG2_IM3	,"NACM"	,REG		,REG		,IM3	,0		},
{	0xC5	,0x80	,4	,REG2_IM3J	,"NACM"	,REG		,RSX		,IM3	,IM7	},
{	0xC5	,0xa0	,4	,REG2_IM3J	,"NACM"	,REG		,RSY		,IM3	,IM7	},
{	0xC5	,0xc0	,4	,REG2_IM3J	,"NACM"	,REG		,RSZ		,IM3	,IM7	},
{	0xC5	,0xe0	,4	,REG2_IM3J	,"NACM"	,REG		,REG		,IM3	,IM7	},
{	0xC6	,0		,3	,REG2_IM3	,"ORCM"	,REG		,RSX		,IM3	,0		},
{	0xC6	,0x20	,3	,REG2_IM3	,"ORCM"	,REG		,RSY		,IM3	,0		},
{	0xC6	,0x40	,3	,REG2_IM3	,"ORCM"	,REG		,RSZ		,IM3	,0		},
{	0xC6	,0x60	,3	,REG2_IM3	,"ORCM"	,REG		,REG		,IM3	,0		},
{	0xC6	,0x80	,4	,REG2_IM3J	,"ORCM"	,REG		,RSX		,IM3	,IM7	},
{	0xC6	,0xa0	,4	,REG2_IM3J	,"ORCM"	,REG		,RSY		,IM3	,IM7	},
{	0xC6	,0xc0	,4	,REG2_IM3J	,"ORCM"	,REG		,RSZ		,IM3	,IM7	},
{	0xC6	,0xe0	,4	,REG2_IM3J	,"ORCM"	,REG		,REG		,IM3	,IM7	},
{	0xC7	,0		,3	,REG2_IM3	,"XRCM"	,REG		,RSX		,IM3	,0		},
{	0xC7	,0x20	,3	,REG2_IM3	,"XRCM"	,REG		,RSY		,IM3	,0		},
{	0xC7	,0x40	,3	,REG2_IM3	,"XRCM"	,REG		,RSZ		,IM3	,0		},
{	0xC7	,0x60	,3	,REG2_IM3	,"XRCM"	,REG		,REG		,IM3	,0		},
{	0xC7	,0x80	,4	,REG2_IM3J	,"XRCM"	,REG		,RSX		,IM3	,IM7	},
{	0xC7	,0xa0	,4	,REG2_IM3J	,"XRCM"	,REG		,RSY		,IM3	,IM7	},
{	0xC7	,0xc0	,4	,REG2_IM3J	,"XRCM"	,REG		,RSZ		,IM3	,IM7	},
{	0xC7	,0xe0	,4	,REG2_IM3J	,"XRCM"	,REG		,REG		,IM3	,IM7	},
{	0xC8	,0		,3	,REG2_IM3	,"ADBM"	,REG		,RSX		,IM3	,0		},
{	0xC8	,0x20	,3	,REG2_IM3	,"ADBM"	,REG		,RSY		,IM3	,0		},
{	0xC8	,0x40	,3	,REG2_IM3	,"ADBM"	,REG		,RSZ		,IM3	,0		},
{	0xC8	,0x60	,3	,REG2_IM3	,"ADBM"	,REG		,REG		,IM3	,0		},
{	0xC8	,0x80	,4	,REG2_IM3J	,"ADBM"	,REG		,RSX		,IM3	,IM7	},
{	0xC8	,0xa0	,4	,REG2_IM3J	,"ADBM"	,REG		,RSY		,IM3	,IM7	},
{	0xC8	,0xc0	,4	,REG2_IM3J	,"ADBM"	,REG		,RSZ		,IM3	,IM7	},
{	0xC8	,0xe0	,4	,REG2_IM3J	,"ADBM"	,REG		,REG		,IM3	,IM7	},
{	0xC9	,0		,3	,REG2_IM3	,"SBBM"	,REG		,RSX		,IM3	,0		},
{	0xC9	,0x20	,3	,REG2_IM3	,"SBBM"	,REG		,RSY		,IM3	,0		},
{	0xC9	,0x40	,3	,REG2_IM3	,"SBBM"	,REG		,RSZ		,IM3	,0		},
{	0xC9	,0x60	,3	,REG2_IM3	,"SBBM"	,REG		,REG		,IM3	,0		},
{	0xC9	,0x80	,4	,REG2_IM3J	,"SBBM"	,REG		,RSX		,IM3	,IM7	},
{	0xC9	,0xa0	,4	,REG2_IM3J	,"SBBM"	,REG		,RSY		,IM3	,IM7	},
{	0xC9	,0xc0	,4	,REG2_IM3J	,"SBBM"	,REG		,RSZ		,IM3	,IM7	},
{	0xC9	,0xe0	,4	,REG2_IM3J	,"SBBM"	,REG		,REG		,IM3	,IM7	},
{	0xCA	,0		,3	,REG_I5I3	,"ADBM"	,REG		,IM5		,IM3	,0		},
{	0xCA	,0x80	,4	,REG_I5I3J	,"ADBM"	,REG		,IM5		,IM3	,IM7	},
{	0xCB	,0		,3	,REG_I5I3	,"SBBM"	,REG		,IM5		,IM3	,0		},
{	0xCB	,0x80	,4	,REG_I5I3J	,"SBBM"	,REG		,IM5		,IM3	,IM7	},
{	0xCC	,0		,3	,REG2_IM3	,"ANM"	,REG		,RSX		,IM3	,0		},
{	0xCC	,0x20	,3	,REG2_IM3	,"ANM"	,REG		,RSY		,IM3	,0		},
{	0xCC	,0x40	,3	,REG2_IM3	,"ANM"	,REG		,RSZ		,IM3	,0		},
{	0xCC	,0x60	,3	,REG2_IM3	,"ANM"	,REG		,REG		,IM3	,0		},
{	0xCC	,0x80	,4	,REG2_IM3J	,"ANM"	,REG		,RSX		,IM3	,IM7	},
{	0xCC	,0xa0	,4	,REG2_IM3J	,"ANM"	,REG		,RSY		,IM3	,IM7	},
{	0xCC	,0xc0	,4	,REG2_IM3J	,"ANM"	,REG		,RSZ		,IM3	,IM7	},
{	0xCC	,0xe0	,4	,REG2_IM3J	,"ANM"	,REG		,REG		,IM3	,IM7	},
{	0xCD	,0		,3	,REG2_IM3	,"NAM"	,REG		,RSX		,IM3	,0		},
{	0xCD	,0x20	,3	,REG2_IM3	,"NAM"	,REG		,RSY		,IM3	,0		},
{	0xCD	,0x40	,3	,REG2_IM3	,"NAM"	,REG		,RSZ		,IM3	,0		},
{	0xCD	,0x60	,3	,REG2_IM3	,"NAM"	,REG		,REG		,IM3	,0		},
{	0xCD	,0x80	,4	,REG2_IM3J	,"NAM"	,REG		,RSX		,IM3	,IM7	},
{	0xCD	,0xa0	,4	,REG2_IM3J	,"NAM"	,REG		,RSY		,IM3	,IM7	},
{	0xCD	,0xc0	,4	,REG2_IM3J	,"NAM"	,REG		,RSZ		,IM3	,IM7	},
{	0xCD	,0xe0	,4	,REG2_IM3J	,"NAM"	,REG		,REG		,IM3	,IM7	},
{	0xCE	,0		,3	,REG2_IM3	,"ORM"	,REG		,RSX		,IM3	,0		},
{	0xCE	,0x20	,3	,REG2_IM3	,"ORM"	,REG		,RSY		,IM3	,0		},
{	0xCE	,0x40	,3	,REG2_IM3	,"ORM"	,REG		,RSZ		,IM3	,0		},
{	0xCE	,0x60	,3	,REG2_IM3	,"ORM"	,REG		,REG		,IM3	,0		},
{	0xCE	,0x80	,4	,REG2_IM3J	,"ORM"	,REG		,RSX		,IM3	,IM7	},
{	0xCE	,0xa0	,4	,REG2_IM3J	,"ORM"	,REG		,RSY		,IM3	,IM7	},
{	0xCE	,0xc0	,4	,REG2_IM3J	,"ORM"	,REG		,RSZ		,IM3	,IM7	},
{	0xCE	,0xe0	,4	,REG2_IM3J	,"ORM"	,REG		,REG		,IM3	,IM7	},
{	0xCF	,0		,3	,REG2_IM3	,"XRM"	,REG		,RSX		,IM3	,0		},
{	0xCF	,0x20	,3	,REG2_IM3	,"XRM"	,REG		,RSY		,IM3	,0		},
{	0xCF	,0x40	,3	,REG2_IM3	,"XRM"	,REG		,RSZ		,IM3	,0		},
{	0xCF	,0x60	,3	,REG2_IM3	,"XRM"	,REG		,REG		,IM3	,0		},
{	0xCF	,0x80	,4	,REG2_IM3J	,"XRM"	,REG		,RSX		,IM3	,IM7	},
{	0xCF	,0xa0	,4	,REG2_IM3J	,"XRM"	,REG		,RSY		,IM3	,IM7	},
{	0xCF	,0xc0	,4	,REG2_IM3J	,"XRM"	,REG		,RSZ		,IM3	,IM7	},
{	0xCF	,0xe0	,4	,REG2_IM3J	,"XRM"	,REG		,REG		,IM3	,IM7	},
{	0xD0	,0		,4	,IM16_REG	,"STW"	,IM16		,MRSX		,0		,0		},
{	0xD0	,0x20	,4	,IM16_REG	,"STW"	,IM16		,MRSY		,0		,0		},
{	0xD0	,0x40	,4	,IM16_REG	,"STW"	,IM16		,MRSZ		,0		,0		},
{	0xD1	,0		,4	,REG_IM16	,"LDW"	,REG		,IM16		,0		,0		},
{	0xD2	,0		,3	,REG_IM3	,"STLM"	,REG		,IM3		,0		,0		},/* LCD出力 */
{	0xD3	,0		,3	,REG_IM3	,"LDLM"	,REG		,IM3		,0		,0		},/* LCD入力 */
{	0xD4	,0		,3	,REG_IM3	,"PPOM"	,REG		,IM3		,0		,0		},/* LCD制御ポート */
{	0xD5	,0		,3	,IR_RIM3	,"PSRM"	,SX			,REG		,IM3	,0		},/* 特定Index指定(SX) */
{	0xD5	,0x20	,3	,IR_RIM3	,"PSRM"	,SY			,REG		,IM3	,0		},/* 特定Index指定(SY) */
{	0xD5	,0x40	,3	,IR_RIM3	,"PSRM"	,SZ			,REG		,IM3	,0		},/* 特定Index指定(SZ) */
{	0xD6	,0		,4	,IR_IM16	,"PRE"	,IX			,IM16		,0		,0		},
{	0xD6	,0x20	,4	,IR_IM16	,"PRE"	,IY			,IM16		,0		,0		},
{	0xD6	,0x40	,4	,IR_IM16	,"PRE"	,IZ			,IM16		,0		,0		},
{	0xD6	,0x60	,4	,IR_IM16	,"PRE"	,US			,IM16		,0		,0		},
{	0xD7	,0		,4	,IR_IM16	,"PRE"	,SS			,IM16		,0		,0		},
{	0xD8	,0		,1	,NU			,"BUP"	,0			,0			,0		,0		},
{	0xD9	,0		,1	,NU			,"BDN"	,0			,0			,0		,0		},
{	0xDA	,0		,3	,REG_IM3	,"DIDM"	,REG		,IM3		,0		,0		},
{	0xDA	,0x20	,3	,REG_IM3	,"DIUM"	,REG		,IM3		,0		,0		},
{	0xDA	,0x40	,3	,REG_IM3	,"BYDM"	,REG		,IM3		,0		,0		},
{	0xDA	,0x60	,3	,REG_IM3	,"BYUM"	,REG		,IM3		,0		,0		},
{	0xDB	,0		,3	,REG_IM3	,"CMPM"	,REG		,IM3		,0		,0		},
{	0xDB	,0x40	,3	,REG_IM3	,"INVM"	,REG		,IM3		,0		,0		},
{	0xDC	,0		,2	,REG_NU		,"SUP"	,REG		,0			,0		,0		},
{	0xDD	,0		,2	,REG_NU		,"SDN"	,REG		,0			,0		,0		},
{	0xDE	,0		,2	,REG_NU		,"JP"	,REG		,0			,0		,0		},/* JP $C5 */
{	0xDF	,0		,2	,REG_NU		,"JP"	,MREG		,0			,0		,0		},/* JP ($C5)*/
{	0xE0	,0		,3	,REG2_IM3	,"STM"	,REG		,MIX_p|RSX	,IM3	,0		},
{	0xE0	,0x20	,3	,REG2_IM3	,"STM"	,REG		,MIX_p|RSY	,IM3	,0		},
{	0xE0	,0x40	,3	,REG2_IM3	,"STM"	,REG		,MIX_p|RSZ	,IM3	,0		},
{	0xE0	,0x60	,3	,REG2_IM3	,"STM"	,REG		,MIX_p|REG	,IM3	,0		},
{	0xE0	,0x80	,3	,REG2_IM3	,"STM"	,REG		,MIX_m|RSX	,IM3	,0		},
{	0xE0	,0xa0	,3	,REG2_IM3	,"STM"	,REG		,MIX_m|RSY	,IM3	,0		},
{	0xE0	,0xc0	,3	,REG2_IM3	,"STM"	,REG		,MIX_m|RSZ	,IM3	,0		},
{	0xE0	,0xe0	,3	,REG2_IM3	,"STM"	,REG		,MIX_m|REG	,IM3	,0		},
{	0xE1	,0		,3	,REG2_IM3	,"STM"	,REG		,MIZ_p|RSX	,IM3	,0		},
{	0xE1	,0x20	,3	,REG2_IM3	,"STM"	,REG		,MIZ_p|RSY	,IM3	,0		},
{	0xE1	,0x40	,3	,REG2_IM3	,"STM"	,REG		,MIZ_p|RSZ	,IM3	,0		},
{	0xE1	,0x60	,3	,REG2_IM3	,"STM"	,REG		,MIZ_p|REG	,IM3	,0		},
{	0xE1	,0x80	,3	,REG2_IM3	,"STM"	,REG		,MIZ_m|RSX	,IM3	,0		},
{	0xE1	,0xa0	,3	,REG2_IM3	,"STM"	,REG		,MIZ_m|RSY	,IM3	,0		},
{	0xE1	,0xc0	,3	,REG2_IM3	,"STM"	,REG		,MIZ_m|RSZ	,IM3	,0		},
{	0xE1	,0xe0	,3	,REG2_IM3	,"STM"	,REG		,MIZ_m|REG	,IM3	,0		},
{	0xE2	,0		,3	,REG2_IM3	,"STIM"	,REG		,MIX_p|RSX	,IM3	,0		},
{	0xE2	,0x20	,3	,REG2_IM3	,"STIM"	,REG		,MIX_p|RSY	,IM3	,0		},
{	0xE2	,0x40	,3	,REG2_IM3	,"STIM"	,REG		,MIX_p|RSZ	,IM3	,0		},
{	0xE2	,0x60	,3	,REG2_IM3	,"STIM"	,REG		,MIX_p|REG	,IM3	,0		},
{	0xE2	,0x80	,3	,REG2_IM3	,"STIM"	,REG		,MIX_m|RSX	,IM3	,0		},
{	0xE2	,0xa0	,3	,REG2_IM3	,"STIM"	,REG		,MIX_m|RSY	,IM3	,0		},
{	0xE2	,0xc0	,3	,REG2_IM3	,"STIM"	,REG		,MIX_m|RSZ	,IM3	,0		},
{	0xE2	,0xe0	,3	,REG2_IM3	,"STIM"	,REG		,MIX_m|REG	,IM3	,0		},
{	0xE3	,0		,3	,REG2_IM3	,"STIM"	,REG		,MIZ_p|RSX	,IM3	,0		},
{	0xE3	,0x20	,3	,REG2_IM3	,"STIM"	,REG		,MIZ_p|RSY	,IM3	,0		},
{	0xE3	,0x40	,3	,REG2_IM3	,"STIM"	,REG		,MIZ_p|RSZ	,IM3	,0		},
{	0xE3	,0x60	,3	,REG2_IM3	,"STIM"	,REG		,MIZ_p|REG	,IM3	,0		},
{	0xE3	,0x80	,3	,REG2_IM3	,"STIM"	,REG		,MIZ_m|RSX	,IM3	,0		},
{	0xE3	,0xa0	,3	,REG2_IM3	,"STIM"	,REG		,MIZ_m|RSY	,IM3	,0		},
{	0xE3	,0xc0	,3	,REG2_IM3	,"STIM"	,REG		,MIZ_m|RSZ	,IM3	,0		},
{	0xE3	,0xe0	,3	,REG2_IM3	,"STIM"	,REG		,MIZ_m|REG	,IM3	,0		},
{	0xE4	,0		,3	,REG2_IM3	,"STDM"	,REG		,MIX_p|RSX	,IM3	,0		},
{	0xE4	,0x20	,3	,REG2_IM3	,"STDM"	,REG		,MIX_p|RSY	,IM3	,0		},
{	0xE4	,0x40	,3	,REG2_IM3	,"STDM"	,REG		,MIX_p|RSZ	,IM3	,0		},
{	0xE4	,0x60	,3	,REG2_IM3	,"STDM"	,REG		,MIX_p|REG	,IM3	,0		},
{	0xE4	,0x80	,3	,REG2_IM3	,"STDM"	,REG		,MIX_m|RSX	,IM3	,0		},
{	0xE4	,0xa0	,3	,REG2_IM3	,"STDM"	,REG		,MIX_m|RSY	,IM3	,0		},
{	0xE4	,0xc0	,3	,REG2_IM3	,"STDM"	,REG		,MIX_m|RSZ	,IM3	,0		},
{	0xE4	,0xe0	,3	,REG2_IM3	,"STDM"	,REG		,MIX_m|REG	,IM3	,0		},
{	0xE5	,0		,3	,REG2_IM3	,"STDM"	,REG		,MIZ_p|RSX	,IM3	,0		},
{	0xE5	,0x20	,3	,REG2_IM3	,"STDM"	,REG		,MIZ_p|RSY	,IM3	,0		},
{	0xE5	,0x40	,3	,REG2_IM3	,"STDM"	,REG		,MIZ_p|RSZ	,IM3	,0		},
{	0xE5	,0x60	,3	,REG2_IM3	,"STDM"	,REG		,MIZ_p|REG	,IM3	,0		},
{	0xE5	,0x80	,3	,REG2_IM3	,"STDM"	,REG		,MIZ_m|RSX	,IM3	,0		},
{	0xE5	,0xa0	,3	,REG2_IM3	,"STDM"	,REG		,MIZ_m|RSY	,IM3	,0		},
{	0xE5	,0xc0	,3	,REG2_IM3	,"STDM"	,REG		,MIZ_m|RSZ	,IM3	,0		},
{	0xE5	,0xe0	,3	,REG2_IM3	,"STDM"	,REG		,MIZ_m|REG	,IM3	,0		},
{	0xE6	,0		,3	,REG_IM3	,"PHSM"	,REG		,IM3		,0		,0		},
{	0xE7	,0		,3	,REG_IM3	,"PHUM"	,REG		,IM3		,0		,0		},
{	0xE8	,0		,3	,REG2_IM3	,"LDM"	,REG		,MIX_p|RSX	,IM3	,0		},
{	0xE8	,0x20	,3	,REG2_IM3	,"LDM"	,REG		,MIX_p|RSY	,IM3	,0		},
{	0xE8	,0x40	,3	,REG2_IM3	,"LDM"	,REG		,MIX_p|RSZ	,IM3	,0		},
{	0xE8	,0x60	,3	,REG2_IM3	,"LDM"	,REG		,MIX_p|REG	,IM3	,0		},
{	0xE8	,0x80	,3	,REG2_IM3	,"LDM"	,REG		,MIX_m|RSX	,IM3	,0		},
{	0xE8	,0xa0	,3	,REG2_IM3	,"LDM"	,REG		,MIX_m|RSY	,IM3	,0		},
{	0xE8	,0xc0	,3	,REG2_IM3	,"LDM"	,REG		,MIX_m|RSZ	,IM3	,0		},
{	0xE8	,0xe0	,3	,REG2_IM3	,"LDM"	,REG		,MIX_m|REG	,IM3	,0		},
{	0xE9	,0		,3	,REG2_IM3	,"LDM"	,REG		,MIZ_p|RSX	,IM3	,0		},
{	0xE9	,0x20	,3	,REG2_IM3	,"LDM"	,REG		,MIZ_p|RSY	,IM3	,0		},
{	0xE9	,0x40	,3	,REG2_IM3	,"LDM"	,REG		,MIZ_p|RSZ	,IM3	,0		},
{	0xE9	,0x60	,3	,REG2_IM3	,"LDM"	,REG		,MIZ_p|REG	,IM3	,0		},
{	0xE9	,0x80	,3	,REG2_IM3	,"LDM"	,REG		,MIZ_m|RSX	,IM3	,0		},
{	0xE9	,0xa0	,3	,REG2_IM3	,"LDM"	,REG		,MIZ_m|RSY	,IM3	,0		},
{	0xE9	,0xc0	,3	,REG2_IM3	,"LDM"	,REG		,MIZ_m|RSZ	,IM3	,0		},
{	0xE9	,0xe0	,3	,REG2_IM3	,"LDM"	,REG		,MIZ_m|REG	,IM3	,0		},
{	0xEA	,0		,3	,REG2_IM3	,"LDIM"	,REG		,MIX_p|RSX	,IM3	,0		},
{	0xEA	,0x20	,3	,REG2_IM3	,"LDIM"	,REG		,MIX_p|RSY	,IM3	,0		},
{	0xEA	,0x40	,3	,REG2_IM3	,"LDIM"	,REG		,MIX_p|RSZ	,IM3	,0		},
{	0xEA	,0x60	,3	,REG2_IM3	,"LDIM"	,REG		,MIX_p|REG	,IM3	,0		},
{	0xEA	,0x80	,3	,REG2_IM3	,"LDIM"	,REG		,MIX_m|RSX	,IM3	,0		},
{	0xEA	,0xa0	,3	,REG2_IM3	,"LDIM"	,REG		,MIX_m|RSY	,IM3	,0		},
{	0xEA	,0xc0	,3	,REG2_IM3	,"LDIM"	,REG		,MIX_m|RSZ	,IM3	,0		},
{	0xEA	,0xe0	,3	,REG2_IM3	,"LDIM"	,REG		,MIX_m|REG	,IM3	,0		},
{	0xEB	,0		,3	,REG2_IM3	,"LDIM"	,REG		,MIZ_p|RSX	,IM3	,0		},
{	0xEB	,0x20	,3	,REG2_IM3	,"LDIM"	,REG		,MIZ_p|RSY	,IM3	,0		},
{	0xEB	,0x40	,3	,REG2_IM3	,"LDIM"	,REG		,MIZ_p|RSZ	,IM3	,0		},
{	0xEB	,0x60	,3	,REG2_IM3	,"LDIM"	,REG		,MIZ_p|REG	,IM3	,0		},
{	0xEB	,0x80	,3	,REG2_IM3	,"LDIM"	,REG		,MIZ_m|RSX	,IM3	,0		},
{	0xEB	,0xa0	,3	,REG2_IM3	,"LDIM"	,REG		,MIZ_m|RSY	,IM3	,0		},
{	0xEB	,0xc0	,3	,REG2_IM3	,"LDIM"	,REG		,MIZ_m|RSZ	,IM3	,0		},
{	0xEB	,0xe0	,3	,REG2_IM3	,"LDIM"	,REG		,MIZ_m|REG	,IM3	,0		},
{	0xEC	,0		,3	,REG2_IM3	,"LDDM"	,REG		,MIX_p|RSX	,IM3	,0		},
{	0xEC	,0x20	,3	,REG2_IM3	,"LDDM"	,REG		,MIX_p|RSY	,IM3	,0		},
{	0xEC	,0x40	,3	,REG2_IM3	,"LDDM"	,REG		,MIX_p|RSZ	,IM3	,0		},
{	0xEC	,0x60	,3	,REG2_IM3	,"LDDM"	,REG		,MIX_p|REG	,IM3	,0		},
{	0xEC	,0x80	,3	,REG2_IM3	,"LDDM"	,REG		,MIX_m|RSX	,IM3	,0		},
{	0xEC	,0xa0	,3	,REG2_IM3	,"LDDM"	,REG		,MIX_m|RSY	,IM3	,0		},
{	0xEC	,0xc0	,3	,REG2_IM3	,"LDDM"	,REG		,MIX_m|RSZ	,IM3	,0		},
{	0xEC	,0xe0	,3	,REG2_IM3	,"LDDM"	,REG		,MIX_m|REG	,IM3	,0		},
{	0xED	,0		,3	,REG2_IM3	,"LDDM"	,REG		,MIZ_p|RSX	,IM3	,0		},
{	0xED	,0x20	,3	,REG2_IM3	,"LDDM"	,REG		,MIZ_p|RSY	,IM3	,0		},
{	0xED	,0x40	,3	,REG2_IM3	,"LDDM"	,REG		,MIZ_p|RSZ	,IM3	,0		},
{	0xED	,0x60	,3	,REG2_IM3	,"LDDM"	,REG		,MIZ_p|REG	,IM3	,0		},
{	0xED	,0x80	,3	,REG2_IM3	,"LDDM"	,REG		,MIZ_m|RSX	,IM3	,0		},
{	0xED	,0xa0	,3	,REG2_IM3	,"LDDM"	,REG		,MIZ_m|RSY	,IM3	,0		},
{	0xED	,0xc0	,3	,REG2_IM3	,"LDDM"	,REG		,MIZ_m|RSZ	,IM3	,0		},
{	0xED	,0xe0	,3	,REG2_IM3	,"LDDM"	,REG		,MIZ_m|REG	,IM3	,0		},
{	0xEE	,0		,3	,REG_IM3	,"PPSM"	,REG		,IM3		,0		,0		},
{	0xEF	,0		,3	,REG_IM3	,"PPUM"	,REG		,IM3		,0		,0		},
{	0xF0	,0		,1	,NU			,"RTN"	,ZF			,0			,0		,0		},
{	0xF1	,0		,1	,NU			,"RTN"	,NCF		,0			,0		,0		},
{	0xF2	,0		,1	,NU			,"RTN"	,LZF		,0			,0		,0		},
{	0xF3	,0		,1	,NU			,"RTN"	,UZF		,0			,0		,0		},
{	0xF4	,0		,1	,NU			,"RTN"	,NZF		,0			,0		,0		},
{	0xF5	,0		,1	,NU			,"RTN"	,CF			,0			,0		,0		},
{	0xF6	,0		,1	,NU			,"RTN"	,NLZF		,0			,0		,0		},
{	0xF7	,0		,1	,NU			,"RTN"	,0			,0			,0		,0		},
{	0xF8	,0		,1	,NU			,"NOP"	,0			,0			,0		,0		},
{	0xF9	,0		,1	,NU			,"CLT"	,0			,0			,0		,0		},
{	0xFA	,0		,1	,NU			,"FST"	,0			,0			,0		,0		},
{	0xFB	,0		,1	,NU			,"SLW"	,0			,0			,0		,0		},
{	0xFC	,0		,1	,NU			,"CANI"	,0			,0			,0		,0		},
{	0xFD	,0		,1	,NU			,"RTNI"	,0			,0			,0		,0		},
{	0xFE	,0		,1	,NU			,"OFF"	,0			,0			,0		,0		},
{	0xFF	,0		,1	,NU			,"TRP"	,0			,0			,0		,0		}
};

/*------------------------------------------------------------------*/
/*  エラー種別定義                                                  */
/*------------------------------------------------------------------*/
enum {
	NORM		=	0	,	/* 正常終了                     */
	INZERR				,	/* 初期化異常                   */
	EOFERR				,	/* ファイル終了                 */
	LOFLOW				,	/* １行の文字数がオーバーした   */
	OPOFLOW				,	/* オペランド文字数オーバーした */
	LBOFLOW				,	/* １行の文字数がオーバーした   */
	NOORG				,	/* ORG命令定義がない            */
	NOENT				,	/* オペランド記述なし           */
	EQUNOLBL			,	/* EQUにラベルエントリなし      */
	ILLOPR				,	/* オペランド記述ミス           */
	ILLSTART			,	/* START文が２つ以上ある        */
	ILLCANMA			,	/* カンマ記述がおかしい         */
	ILLDQUO				,	/* ﾀﾞﾌﾞﾙｺｰﾃｰｼｮﾝ/括弧 異常       */
	DUPLBL				,	/* ラベル記述が２回以上ある     */
	ILLLBL				,	/* ラベルに利用できない文字     */
	LBLNOENT			,	/* ラベル登録なし               */
	LBNOALOC			,	/* ラベル登録なし               */
	OFLOW				,	/* オペランド値が範囲外         */
	JOFLOW				,	/* 相対ジャンプが範囲外         */
	AOFLOW				,	/* 出力バッファオーバー         */
	ADOFLOW				,	/* アセンブルアドレスオーバー   */
	EOFLOW				,	/* 実行アドレスがORGより小さい  */
	CALERR				,	/* 計算異常( 0 除算等 )が発生   */
	IFNEST				,	/* #if～#else～#endifネスト異常 */
	INCNOFILE			,	/* #includeファイル名なし       */
	INCERR				,	/* #includeネスト異常           */
	REGERR				,	/* メインレジスタ番号異常       */
	ILLBIN				,	/* バイナリ読み込み異常         */
	ILLBMP				,	/* ビットマップ読み込み異常     */
	UNDEFOPR	= 0x80		/* 該当命令なし/記述方法のミス  */
};
/* エラーメッセージ変換テーブル構造 */
typedef struct err {
	unsigned short code;			/* コード番号                   */
	char	msg[32];				/* エラーメッセージ(32文字以内) */
} ERR;

/* エラーメッセージ変換テーブル */
#define		MAXERR		28
struct err errtbl[MAXERR] = {
{	LOFLOW	    ,	"Line Length is Too Long"		},
{	OPOFLOW	    ,	"Oprand Length is Too Long"		},
{	LBOFLOW	    ,	"LABEL Length is Too Long"		},
{	NOORG	    ,	"ORG Not Entry"					},
{	NOENT	    ,	"Operand Not Entry"				},
{	EQUNOLBL    ,	"EQU without Label"				},
{	ILLOPR	    ,	"Illegal Operand"				},
{	ILLSTART    ,	"START Already Defined"			},
{	ILLCANMA    ,	"Illegal [,]"					},
{	ILLDQUO	    ,	"Illegal ['']or[(]or[)]"		},
{	DUPLBL	    ,	"LABEL Already Defined"			},
{	ILLLBL	    ,	"LABEL Type Mismatch"			},
{	LBLNOENT    ,	"Undefined LABEL"				},
{	LBNOALOC    ,	"Could Not Defined LABEL"		},
{	OFLOW	    ,	"Operand Range Over"			},
{	JOFLOW	    ,	"Jump Address Over"				},
{	AOFLOW	    ,	"Output Buffer Over Flow"		},
{	ADOFLOW	    ,	"Assemble Address Over Flow"	},
{	EOFLOW	    ,	"Execute Address Illegal"		},
{	CALERR	    ,	"Could Not Calculate"			},
{	IFNEST	    ,	"Illegal [#if] - [#endif]"		},
{	INCNOFILE   ,	"Invalid Include File Name"		},
{	INCERR	    ,	"Could Not Nest Include"		},
{	REGERR	    ,	"Illegal Register Number"		},
{	ILLBMP	    ,	"Illegal Bitmap File Format"	},
{	ILLBIN	    ,	"Illegal Binary File Format"	},
{	UNDEFOPR    ,	"Operation Type Mismatch"		},
{	0		    ,	"Unknown"						}
};

/* /SETオプション構造体 */
typedef struct set {
	char	ent[MAXNAME+2];			/* 登録ラベル名称 */
	char	let[MAXNAME+2];			/* 代入ラベル名称 */
} SETOPT;

/* ビットマップ読み込み用マクロ定義 */
#define	Getlong(x) (((unsigned long)ImgTbl[x+3])<<24)+(((unsigned long)ImgTbl[x+2])<<16)+(((unsigned long)ImgTbl[x+1])<<8)+((unsigned long)ImgTbl[x]&0xff)
#define	Getword(x) ((((unsigned short)ImgTbl[x+1])<<8)+(unsigned short)ImgTbl[x])
#define	Getparret(x) (((unsigned short)ImgTbl[x+2])+((unsigned short)ImgTbl[x+1])+(unsigned short)ImgTbl[x])

ASMC=./hd61700asm/hd61700
ASMFLAGS=/s
OUTFILES = enc_bl.pas dec_bl.pas cksum.pas quad_eq.pas

%.pas:%.asm
	$(ASMC) $< $(ASMFLAGS)

all: pbnet

pbnet: hd61700asm/hd61700 $(OUTFILES) mkinclude

mkinclude:
	chmod +x ./preproc.sh
	./preproc.sh pbnet.src.pas pbnet.pas

.PHONY: clean

clean: 
	rm -rf *.lst *.o *~ $(OUTFILES) pbnet.pas
	$(MAKE) -C hd61700asm clean



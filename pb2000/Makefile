ASMC=./hd61700asm/hd61700
ASMFLAGS=/s
OUTFILES = enc_bl.pas dec_bl.pas cksum.pas quad_eq.pas

%.pas:%.asm
	$(ASMC) $< $(ASMFLAGS)

all: pbnet

pbnet: hd61700asm/hd61700 $(OUTFILES) mkinclude

mkinclude:
	mkdir -p build
	chmod +x ./preproc.sh
	./preproc.sh net.src.pas ./build/net.pas
	./preproc.sh pbnet.int.src.pas ./build/PBNET.INT
	./preproc.sh pbnet.src.pas ./build/PBNET.PAS
	./preproc.sh build.src.bat ./build/build.bat

.PHONY: clean

clean: 
	rm -rf build
	rm -rf *.lst *.o *~ $(OUTFILES)
	$(MAKE) -C hd61700asm clean



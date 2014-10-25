all: sample.tar sample.tar.gz sample.tar.bz2 sample.tar.lzma sample.tar.xz sample.zip

sample.tar:
	cd src; tar cvf sample.tar sample
	mv src/sample.tar .

sample.tar.gz:
	cd src; tar cvfz sample.tar.gz sample
	mv src/sample.tar.gz .

sample.tar.bz2:
	cd src; tar cvfy sample.tar.bz2 sample
	mv src/sample.tar.bz2 .

sample.tar.lzma:
	cd src; tar cvf sample.tar.lzma --lzma sample
	mv src/sample.tar.lzma .

sample.tar.xz:
	cd src; tar cvfJ sample.tar.xz sample
	mv src/sample.tar.xz .

sample.zip:
	cd src; zip -r sample.zip sample
	mv src/sample.zip .

clean: 
	rm sample.tar sample.tar.gz sample.tar.bz2 sample.tar.lzma sample.tar.xz sample.zip

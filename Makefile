obj-m +=cod_exem.o

all:
	make -C /lib/modules/5.4.0-91-generic/build M=$(shell pwd) modules

clean:
	make -C /lib/modules/5.4.0-91-generic/build M=$(shell pwd) clean
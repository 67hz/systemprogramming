# P=
OBJECTS=
CFLAGS=  -g -Wall -D_XOPEN_SOURCE=600	
LDLIBS=`pkg-config --libs gsl`
CC=c99

$(INFO $(P))
$(P): $(OBJECTS)


INDENT_FLAGS= -nbad -bap -nbc -bbo -hnl -br -brs -c33 -cd33 \
-ncdb -ce -ci4 -cli0 -d0 -di1 -nfc1 -i8 -ip0 -l80 -lp -npcs -nprs -npsl -sai \
-saf -saw -ncs -nsc -sob -nfca -cp33 -ss -ts8 -il1

indent:
	$(info indenting $(P) for Kernel style C)
	indent $(INDENT_FLAGS) $(P)

clear-indent-src:
	$(INFO $@)
	rm *.c~

clear-indent-headers:
	rm *.h~

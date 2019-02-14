OBJ=FEBDTP.o
HEADER=FEBDTP.hxx

LIB=FEBDTP
LIBFULL=lib$(LIB).so

DICT=$(LIB)_Dict.cxx
LINK=$(LIB)_LinkDef.h

EXE=test

$(LIBFULL):	$(DICT) $(OBJ)
	`root-config --cxx --cflags` -fPIC -shared -o $@ FEBDTP.o $(DICT)

$(DICT):$(HEADER) $(LINK)
	rootcling -f $@ -c -p $(HEADER) $(LINK)

%.o: %.cxx
	`root-config --cxx --cflags` -fPIC -o $@ -c $<

clean:
	rm *.o
distclean:
	rm *.o *Dict* *.so $(EXE)



test:  FEBDAQMULT.C $(LIBFULL)
	`root-config --cxx `	-o	$@	$<	-L.	-l$(LIB)	`root-config --cflags --glibs`
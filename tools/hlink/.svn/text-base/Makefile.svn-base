HETLINK_OBJS = args.o hetlink.o

ELFPP_OBJS = elfpp_access.o elfpp_object.o elfpp_reloc.o	\
elfpp_section.o elfpp_symbol.o

CXXFLAGS = -Wall -Ielfpp -I.
CFLAGS = -Wall -I.

hetlink: $(HETLINK_OBJS) $(ELFPP_OBJS)
	$(CXX) -o $@ $^

%.o: %.cc elfpp _dpp
	$(CXX) -o $@ -c $< $(CXXFLAGS)

%.o: %.c
	$(CXX) -o $@ -c $< $(CFLAGS)

%.o: elfpp/%.cc
	$(CXX) -o $@ -c $< $(CXXFLAGS)

elfpp/elfpp_access.cc elfpp/elfpp_object.cc elfpp/elfpp_reloc.cc	\
elfpp/elfpp_section.cc elfpp/elfpp_symbol.cc: \
	elfpp _dpp

elfpp: _dpp
	svn co https://www.ssji.net/svn/projets/trunk/libelfpp/src $@

_dpp: FORCE
	svn co https://www.ssji.net/svn/projets/trunk/libdpp $@

FORCE:
	@true

.PHONY: FORCE

clean:
	rm -f *.o hetlink
	rm -rf elfpp _dpp

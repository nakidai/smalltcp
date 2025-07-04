RM ?= rm -f
PREFIX ?= /usr/local/
BINDIR ?= ${PREFIX}/bin

CFLAGS.veryneeded != [ "${CC}" != tcc ] && echo -ansi -fno-builtin
CFLAGS += ${CFLAGS.veryneeded}

.PHONY: all
all: buc bus

install: all
	install -d ${BINDIR}
	install -m755 buc bus ${BINDIR}

uninstall:
	${RM} ${BINDIR}/buc ${BINDIR}/bus

.PHONY: clean
clean:
	${RM} buc bus

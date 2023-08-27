PRJNAME = lin_alg

INCPATH = ./include
SRCPATH = ./src
BINPATH = ./bin
OBJPATH = ./obj
LIBPATH = ./lib
EXT_INCPATH = /usr/include/mkl
EXT_LIBPATH = /usr/lib/x86_64-linux-gnu

DESTPATH = ${HOME}
LIBINSTPATH = ${DESTPATH}/lib
INCINSTPATH = ${DESTPATH}/include

RLS_FLT32_LIB = $(PRJNAME)_flt32
RLS_FLT64_LIB = $(PRJNAME)_flt64
DBG_FLT32_LIB = $(PRJNAME)_flt32_dbg
DBG_FLT64_LIB = $(PRJNAME)_flt64_dbg

CC = gcc
LD = gcc
AR = ar

COM_CFLAGS = -std=c11 -Wall -Wextra -I$(INCPATH) -I$(EXT_INCPATH) -DINDEX_T=INT32
OPT_CFLAGS = -flto -O3

RLS_CFLAGS = -DNDEBUG $(COM_CFLAGS) $(OPT_CFLAGS)
RLS_LDFLAGS = $(OPT_CFLAGS) -L$(LIBPATH) -L$(EXT_LIBPATH)
DBG_CFLAGS = -DDEBUG -g $(COM_CFLAGS) 
DBG_LDFLAGS = -L$(LIBPATH) -L$(EXT_LIBPATH) -g
LD_LIBS = -lmkl_rt -lm # -lmkl_intel_lp64 -lmkl_sequential -lmkl_core -lpthread -lm -ldl

CFILES = $(wildcard $(SRCPATH)/*.c)
HFILES = $(wildcard $(INCPATH)/*.h)
RLS_FLT32_OBJS = $(patsubst $(SRCPATH)/%.c, $(OBJPATH)/%_flt32.o, $(CFILES))
RLS_FLT64_OBJS = $(patsubst $(SRCPATH)/%.c, $(OBJPATH)/%_flt64.o, $(CFILES))
DBG_FLT32_OBJS = $(patsubst $(SRCPATH)/%.c, $(OBJPATH)/%_flt32_dbg.o, $(CFILES))
DBG_FLT64_OBJS = $(patsubst $(SRCPATH)/%.c, $(OBJPATH)/%_flt64_dbg.o, $(CFILES))

.PHONY: all clean release debug test run_test

all: debug release test
	@echo "====== make all ======"

$(OBJPATH)/%_flt32.o: $(SRCPATH)/%.c $(HFILES)
	@mkdir -p $(OBJPATH)
	$(CC) $(RLS_CFLAGS) -DFEILD_T=FLT32 -o $@ -c $<

$(OBJPATH)/%_flt64.o: $(SRCPATH)/%.c $(HFILES)
	@mkdir -p $(OBJPATH)
	$(CC) $(RLS_CFLAGS) -DFEILD_T=FLT64 -o $@ -c $<

$(OBJPATH)/%_flt32_dbg.o: $(SRCPATH)/%.c $(HFILES)
	@mkdir -p $(OBJPATH)
	$(CC) $(DBG_CFLAGS) -DFEILD_T=FLT32 -o $@ -c $<

$(OBJPATH)/%_flt64_dbg.o: $(SRCPATH)/%.c $(HFILES)
	@mkdir -p $(OBJPATH)
	$(CC) $(DBG_CFLAGS) -DFEILD_T=FLT64 -o $@ -c $<

$(LIBPATH)/lib$(RLS_FLT32_LIB).a: $(RLS_FLT32_OBJS)
	@mkdir -p $(LIBPATH)
	$(AR) rcs $@ $^

$(LIBPATH)/lib$(RLS_FLT64_LIB).a: $(RLS_FLT64_OBJS)
	@mkdir -p $(LIBPATH)
	$(AR) rcs $@ $^

$(LIBPATH)/lib$(DBG_FLT32_LIB).a: $(DBG_FLT32_OBJS)
	@mkdir -p $(LIBPATH)
	$(AR) rcs $@ $^

$(LIBPATH)/lib$(DBG_FLT64_LIB).a: $(DBG_FLT64_OBJS)
	@mkdir -p $(LIBPATH)
	$(AR) rcs $@ $^

$(BINPATH)/$(DBG_FLT32_LIB)_test.out: $(SRCPATH)/$(PRJNAME)_test.c $(LIBPATH)/lib$(DBG_FLT32_LIB).a
	@mkdir -p $(BINPATH)
	$(LD) $(DBG_LDFLAGS) -o $@ -l$(DBG_FLT32_LIB) $(LD_LIBS)

$(BINPATH)/$(DBG_FLT64_LIB)_test.out: $(SRCPATH)/$(PRJNAME)_test.c $(LIBPATH)/lib$(DBG_FLT64_LIB).a
	@mkdir -p $(BINPATH)
	$(LD) $(DBG_LDFLAGS) -o $@ -l$(DBG_FLT64_LIB) $(LD_LIBS)

release: $(LIBPATH)/lib$(RLS_FLT32_LIB).a $(LIBPATH)/lib$(RLS_FLT64_LIB).a
	@echo "====== make release ======"

debug: $(LIBPATH)/lib$(DBG_FLT32_LIB).a $(LIBPATH)/lib$(DBG_FLT64_LIB).a
	@echo "====== make debug ======"

test: $(BINPATH)/$(DBG_FLT32_LIB)_test.out $(BINPATH)/$(DBG_FLT64_LIB)_test.out
	@echo "====== make test ======"

run_test: test
	$(BINPATH)/$(DBG_FLT32_LIB)_test.out
	@echo "****** FLT32 finished ******"
	$(BINPATH)/$(DBG_FLT64_LIB)_test.out
	@echo "****** FLT64 finished ******"
	@echo "====== make run_test ======"

install: release debug
	install -d $(LIBINSTPATH)
	install -m 644 $(LIBPATH)/lib$(RLS_FLT32_LIB).a $(LIBPATH)/lib$(RLS_FLT64_LIB).a ${LIBINSTPATH}
	install -m 644 $(LIBPATH)/lib$(DBG_FLT32_LIB).a $(LIBPATH)/lib$(DBG_FLT64_LIB).a ${LIBINSTPATH}
	install -d ${INCINSTPATH}
	install -m 644 ${INCPATH}/* ${INCINSTPATH}
	@echo "====== make install ======"

clean:
	rm -rf $(OBJPATH) $(BINPATH) $(LIBPATH)
	@echo "====== make clean ======"

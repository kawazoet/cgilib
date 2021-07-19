# Makefile for the cgilib
# Copyright (C) 2001  Kawazoe Tomonori

# 通常このファイルを変更する必要はありません。
# 設定を変更する場合は、一つ上のディレクトリの中の、
# Configureファイルを編集してください。

include ../Configure

OBJS = CGIOut.o Cookie.o CookieOut.o Form-multipart.o Form.o \
       jstring.o string_misc.o

all : $(LIBNAME)

install :
	test -d $(HEADER_DIR) || mkdir -p $(HEADER_DIR)
	cp *.h $(HEADER_DIR)
	test -d $(LIBRARY_DIR) || mkdir -p $(LIBRARY_DIR)
	cp $(LIBNAME) $(LIBRARY_DIR)

clean :
	rm -f $(LIBNAME) $(OBJS) *~ *.bak

$(LIBNAME) : $(OBJS)
	ar r $(LIBNAME) $(OBJS)
	ranlib $(LIBNAME)

.SUFFIXES :
.SUFFIXES : .cc .o

.cc.o :
	$(CXX) $(FLAGS) -c $<

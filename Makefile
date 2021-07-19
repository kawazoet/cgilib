# Makefile for the cgilib
# Copyright (C) 2001  Kawazoe Tomonori

# 通常このファイルを変更する必要はありません。
# 設定を変更する場合は、このディレクトリの中の、
# Configureファイルを編集してください。

all : 
	cd src; make all

install : all
	cd src; make install

clean :
	rm -f *~ *.bak
	rm -rf doc
	cd src; make clean

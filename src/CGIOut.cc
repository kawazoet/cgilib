/*
cgilib - a C++ library for CGI

Copyright (C) 2001-2004
  Kawazoe Tomonori, Ueda Satoshi, Hayase Yasuhiro, Harada Erimu
*/

#include "CGIOut.h"
#include <stdlib.h>
#include <time.h>
#include <cstdio>

namespace cgi {

CGIOut::CGIOut(void) : contentType("Content-type: text/html"), noCache(false)
{
}

void CGIOut::setContentType(const string& type)
{
    contentType = "Content-type: ";
    contentType += type;
}

void CGIOut::setLocation(const string& loc)
{
    location = "Location: ";
    location += loc;
}

void CGIOut::setNoCache(bool output)
{
    noCache = output;
}

void CGIOut::output(void)
{

    // もしあれば、Locationを優先
    
    if( location.length() ){
	puts(location.c_str());
	puts("");
	return;
    }
    
    // クッキーの出力
    cookie.output_header();
    
    // ヘッダを吐く
    puts(contentType.c_str());
    printf("Content-length: %d\n", document.length());
    
    // no-cache
    if( noCache ){
        puts("Pragma: no-cache");
        puts("Cache-Control: no-cache");
    }
    
    // 空行
    puts("");
    
    // 本体を吐く
    fputs(document.c_str(), stdout);
}

void CGIOut::nph_output(void)
{
    puts("HTTP/1.0 200 OK");
    output();
}

} // namespace cgi

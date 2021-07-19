/*
cgilib - a C++ library for CGI

Copyright (C) 2001-2004
  Kawazoe Tomonori, Ueda Satoshi, Hayase Yasuhiro, Harada Erimu
*/

#include "Cookie.h"
#include <time.h>
#include <stdio.h>
#include <string>
#include <map>
#include <ctype.h>

namespace cgi {

// CookieDecodeクラス

Cookie::CookieDecode::CookieDecode()
{
    char *cookie;
    char *p;
    string token, key;
    char buf[3];
    
    if( (cookie = getenv("HTTP_COOKIE")) == NULL ){
	return;
    }
    
    for( p=cookie; *p != '\0'; p++ ){
	switch( *p ){
	case '%':
	    if (!isxdigit(p[1]) ||
		!isxdigit(p[2])) {
		return;
	    }
	    buf[0] = p[1];
	    buf[1] = p[2];
	    buf[2] = '\0';
	    token += (char)strtoul(buf, NULL, 16);
	    p += 2;
	    break;
	case '+':
	    token += ' ';
	    break;
	case '=':
	    key = token;
	    token.erase();
	    break;
	case ';':
	    if( key.length() )
		Pair.insert(associArray::value_type(key, token));
	    token.erase();
	    break;
	case ' ':
	    break;
	default:
	    token += *p;
	    break;
	}
    }
    
    if( key.length() )
	Pair.insert(associArray::value_type(key, token));
    
    return;
}


// Cookieクラス

// static変数

Cookie::CookieDecode Cookie::rep;

// public

bool Cookie::find(const string &key) const
{
    if( rep.Pair.find(key) != rep.Pair.end() )
	return true;
    else
	return false;
}

string Cookie::operator [](const string& key) const
{
    const_iterator i = rep.Pair.find(key);
    if( i != rep.Pair.end() ){
	return i->second;
    } else {
	return string("");
    }
}

bool Cookie::getValue(const string& key, string& value) const
{
    const_iterator i = rep.Pair.find(key);
    if( i != rep.Pair.end() ){
	value = i->second;
	return true;
    } else {
	return false;
    }    
}

} // namespace cgi

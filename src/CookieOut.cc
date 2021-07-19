/*
cgilib - a C++ library for CGI

Copyright (C) 2001-2004
  Kawazoe Tomonori, Ueda Satoshi, Hayase Yasuhiro, Harada Erimu
*/

#include "CookieOut.h"
#include <stdlib.h>
#include <time.h>
#include <cstdio>
#include <ctype.h>

namespace cgi {

CookieOut::CookieOut()
{
    expireDays = 30;
    secure = false;
    expires = true;
}
    
void CookieOut::set(const string &key, const string &value)
{
    // cookies.insert( map<string, string>::value_type(key, value) );
    cookies[key] = value;
}

string & CookieOut::operator[](const string &key)
{
    return cookies[key];
}

void CookieOut::setExpireDays(int days)
{
    expireDays = days;
}

void CookieOut::setPath(const string& p)
{
    path = p;
}

void CookieOut::setDomain(const string& dom)
{
    domain = dom;
}

void CookieOut::setSecure(bool x)
{
    secure = x;
}

void CookieOut::setExpires(bool x)
{
    expires = x;
}

static string encode(const string& str)
{
    string encoded;
    char buffer[4];
    string::const_iterator i;
    
    for( i=str.begin(); i != str.end(); i++ ){
        // isalnum が正しいのかもしれないが...
        if( isgraph(*i) && *i != ',' && *i != ';' ){
            encoded += *i;
        } else {
            sprintf(buffer, "%%%02x", static_cast<unsigned char>(*i));
            encoded += buffer;
        }
    }
    
    return encoded;
}

static string makeExpireTime(int expireDays)
{
    struct tm *gmt_tm;
    time_t expire;
    char buf[40];  // 30でちょうど
    char *month[] = {
        "Jan", "Feb", "Mar", "Apr", "May", "Jun",
            "Jul", "Aug", "Sep", "Oct", "Nov", "Dec" };
    char *week[] = {
        "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat" };
    
    expire = time(NULL);
    gmt_tm = gmtime(&expire);
    gmt_tm->tm_mday += expireDays;
    mktime(gmt_tm);
    sprintf(buf, "%3s, %02d-%3s-%04d %02d:%02d:%02d GMT",
            week[gmt_tm->tm_wday],
            gmt_tm->tm_mday, month[gmt_tm->tm_mon], gmt_tm->tm_year + 1900,
            gmt_tm->tm_hour, gmt_tm->tm_min, gmt_tm->tm_sec);
    
    return string(buf);
}

void CookieOut::output_header(void)
{
    multimap<string, string>::iterator it;
    string str;
    
    for( it=cookies.begin(); it!=cookies.end(); it++ ){        
        str = "Set-Cookie: ";
        str += encode( it->first );
        str += "=";
        str += encode( it->second );
        str += ';';
        // Expires
        if ( expires ){
            str += " expires=";
            str += makeExpireTime(expireDays);
            str += ";";
        }
        // Domain
        if( domain.length() ){
            str += " domain=";
            str += domain;
            str += ";";
        }
        // Path
        if( path.length() ){
            str += " path=";
            str += path;
            str += ";";
        }
        // secure
        if( secure ){
            str += " secure";
        }
        
        // 出力
        puts(str.c_str() );
        str.erase();
    }
}


void CookieOut::output_html(void)
{
    multimap<string, string>::iterator it;
    string str;
    
    for( it=cookies.begin(); it!=cookies.end(); it++ ){        
        str = "<META HTTP-EQUIV=\"Set-Cookie\" CONTENT=\"";
        str += encode( it->first );
        str += "=";
        str += encode( it->second );
        str += ';';
        // Expires
        if ( expireDays > 0){
            str += " expires=";
            str += makeExpireTime(expireDays);
            str += ";";
        }
        // Domain
        if( domain.length() ){
            str += " domain=";
            str += domain;
            str += ";";
        }
        // Path
        if( path.length() ){
            str += " path=";
            str += path;
            str += ";";
        }
        // secure
        if( secure ){
            str += " secure";
        }
        
        // 出力
        str += "\">";
        puts(str.c_str() );
        str.erase();
    }
}

/*
int main(void)
{
    CookieOut c;
    
    c.set("key", "value");
    c.set("キー2", "value2");
    c.set("key3", "バリュー3");
    
    c.output_header();
    c.output_html();
    
    return 0;
}
*/


} // namespace cgi

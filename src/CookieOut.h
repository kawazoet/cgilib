/*
cgilib - a C++ library for CGI

Copyright (C) 2001-2004
  Kawazoe Tomonori, Ueda Satoshi, Hayase Yasuhiro, Harada Erimu
*/

/** @file CookieOut.h
  * このファイルでは、cgi::CookieOutクラスの宣言を行っています。
  */

#ifndef _COOKIEOUT_H_
# define _COOKIEOUT_H_

# include <string>
# include <map>

using namespace std;

namespace cgi {

class CookieOut {
public:
    CookieOut();
    void set(const string &key, const string &value);
    string & operator[](const string &key);
    void setExpireDays(int days);
    void setPath(const string& path);
    void setDomain(const string& domain);
    void setSecure(bool);
    void setExpires(bool);
    
    // 標準出力へと出力
    
    // ヘッダ形式 (Set-Cookie: ...)
    void output_header(void);
    // html形式 ( <meta http-equiv="Set-Cookie" CONTENT="..."> )
    void output_html(void);
    
private:
    map<string, string> cookies;
    int expireDays;
    string path;
    string domain;
    bool expires;
    bool secure;
};

}  // namespace cgi

#endif // _COOKIEOUT_H_

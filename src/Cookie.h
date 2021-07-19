/*
cgilib - a C++ library for CGI

Copyright (C) 2001-2004
  Kawazoe Tomonori, Ueda Satoshi, Hayase Yasuhiro, Harada Erimu
*/

/** @file Cookie.h
  * このファイルでは、cgi::Cookieクラス、
  * およびcgi::Cookie::CookieDecodeクラス(cgi::Cookieクラスが内部で利用)の宣言を行っています。
  */

#ifndef _COOKIE_H_
# define _COOKIE_H_

# include<string>
# include<map>

using namespace std;

namespace cgi {

class Cookie {
public:
    
    typedef map<string, string> associArray;
    typedef associArray::iterator iterator;
    typedef associArray::const_iterator const_iterator;
    
    class CookieDecode {
    public:
        CookieDecode();
        associArray Pair;
    };
    
    /// 指定したキーが存在するかどうか調べる
    bool find(const string &key) const;
    
    /// 指定されたキーに対応する値を返す
    /// キーに対応するデータがなければ、空文字列を返す
    string operator [](const string& key) const;
    
    /// 指定されたキーに対応する値を value にいれる
    /// キーに対するデータがなければ false を返す
    bool getValue(const string& key, string& value) const;
    
private:
    static class CookieDecode rep;
    
};
    
}  // namespace cgi

#endif // _COOKIE_H_

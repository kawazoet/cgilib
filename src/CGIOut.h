/*
cgilib - a C++ library for CGI

Copyright (C) 2001-2004
  Kawazoe Tomonori, Ueda Satoshi, Hayase Yasuhiro, Harada Erimu
*/

/** @file CGIOut.h
  * このファイルでは、cgi::CGIOutクラスの宣言を行っています。
  */

#ifndef _CGIOUT_H_
# define _CGIOUT_H_

# include "CookieOut.h"
# include <list>

using namespace std;

namespace cgi {

// CGI出力クラス

class CGIOut {
public:
   
    CGIOut();
    
    // Cookie
    class CookieOut cookie;
    // document
    string document;
    
    /// ヘッダー中のContent-Typeを設定する。
    void setContentType(const string& type);
    
    /// ヘッダー中のLocationを設定する
    void setLocation(const string& loc);
    
    /// Pragma: no-cache と Cache-Control: no-cache を出すかどうかを設定する
    void setNoCache(bool output);
    
    /// ドキュメントの出力を行う
    void output(void);
    /// ドキュメントの出力を行う(NPH版)
    void nph_output(void);
    
private:
    string contentType;
    string location;
    bool noCache;
};

} // namespace cgi

#endif

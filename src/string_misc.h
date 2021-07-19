/*
cgilib - a C++ library for CGI

Copyright (C) 2001-2004
  Kawazoe Tomonori, Ueda Satoshi, Hayase Yasuhiro, Harada Erimu
*/

#ifndef _STRING_MISC_H_
# define _STRING_MISC_H_

# include <string>
# include <list>
# include <map>
# include <cctype>

using namespace std;

/** @file string_misc.h
  * このファイルでは、文字列の処理を行う各種の関数を宣言しています。
  */

namespace string_misc {
    /// 文字列 target 中の pattern を見つけたら、replacement と置き換えます。
    /// 1-パスのみの処理であり、再帰的な置換は行いません。
    string & replace(string &target,
                          const string &pattern,
                          const string &replacement);
    string & replace(string &target,
                          const char *pattern,
                          const string &replacement);
    string & replace(string &target,
                          const string &pattern,
                          const char *replacement);
    string & replace(string &target,
                          const char *pattern,
                          const char *replacement);
    
    /// 複数の文字列を一斉に置換します。
    string & replace2(string &target, const map<string, string> &repmap);
    
    /// 文字列 target を、separatorsで渡された文字をセパレータとみなし、
    /// 分割した結果のリストを返します。
    /// セパレータが続く場合には、空文字列の要素として処理されます。
    list<string> split(const string &target, char separators[]);
    
    /// サニタイズを行います。
    string sanitize(const string &src);
    string sanitize_url_scheme(const string &src,
                                    const list<string> &schemes);
    string sanitize_url(const string &src);
    
    /// 文字列先頭、末尾の空白文字を取り去った文字列を返します。
    ///   空白文字: スペース、タブ、LF、CR、垂直タブ
    string trim(const string &s);
}

#endif

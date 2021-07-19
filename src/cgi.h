/*
cgilib - a C++ library for CGI

Copyright (C) 2001-2004
  Kawazoe Tomonori, Ueda Satoshi, Hayase Yasuhiro, Harada Erimu
*/

/*
This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

/** @file cgi.h
 *  このファイルでは、cgilibのすべてのヘッダのインクルードを行っています。
 *  また、名前空間jstringとstring_miscを名前空間cgi内に展開します。
 *  cgilibを利用する時には、このファイルをインクルードすると便利です。
 */


/** @mainpage
 * cgilibは、C++でCGIを作成するためのライブラリです。
 * このライブラリは、以下の機能を提供します。
 * - フォームとCookieのデコード 
 * - 日本語文字コードの変換
 * - 文字列の処理
 * - ドキュメントとCookieの出力 
 * 
 * 主要なクラスの一覧:
 * - cgi::Form : フォームのデコードを行います。
 * - cgi::Cookie : Cookieのデコードを行います。
 * - cgi::CookieOut : Cookieの出力を行う時に用います。
 * - cgi::CGIOut : ドキュメント及びCookieの出力を行います。
 * 
 * 主要な関数群の一覧:
 * - string_misc : 文字列処理を行う各種関数を含みます。
 * - jstring : 日本語文字コード変換を行う各種関数を含みます。
 * 
 */


#ifndef _CGI_H_
# define _CGI_H_

// 全てのヘッダのインクルードを行う

# include "Form.h"
# include "Cookie.h"
# include "jstring.h"
# include "CGIOut.h"
# include "string_misc.h"

// 全ての名前空間を展開する

namespace cgi
{
    using namespace jstring;
    using namespace string_misc;
}

#endif // _CGI_H_

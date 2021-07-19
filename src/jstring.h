/*
cgilib - a C++ library for CGI

Copyright (C) 2001-2004
  Kawazoe Tomonori, Ueda Satoshi, Hayase Yasuhiro, Harada Erimu
*/

/** @file jstring.h
  * このファイルでは、日本語文字コードの変換などを行う
  * 各種の関数を宣言しています。
  */
#ifndef _JSTRING_H_
# define _JSTRING_H_

# include <string>

using namespace std;

namespace jstring {

const int UNKNOWN   = 0;  // 不明
const int JIS       = 1;  // ISO-2022-JP(ISO-2022-JP-3)
const int SJIS      = 2;  // Shift JIS
const int EUC       = 3;  // EUC-JP

const int LF_ONLY = 1;  // LF (Unix)
const int CR_ONLY = 2;  // CR (Mac)
const int CR_LF   = 3;  // CR LF (DOS / Windows)


/**
 *  機能: input の文字コードを判別する
 *  返値: UNKNOWN, JIS, EUC, SJIS のどれか
 *  オプション:
 *        [checkHankana(false)]
 *            true のとき半角カタカナの存在を考慮する
 *            ただし、EUC と Shift JIS の判別能力が悪くなるのでお勧めしない
 *            (具体的には UNKNOWN が返りやすくなる。この場合 EUC である確率が高い)
 *  制限: JISX0212(補助漢字), JISX0213(JIS2000)は存在しないと仮定している
 *        これらが入っていると SJIS と EUC の判別を誤る可能性が高い
 */
int getcode(const string& input, const bool checkHankana = false);

/**
 *  機能: input を outputCode で指定された文字コードに変換
 *  返値: 変換された文字列
 *  オプション:
 *        [outputCode]
 *            変換する文字コード
 *        [inputCode(UNKNOWN)]
 *            input の文字コードを決めうちする
 *            UNKNOWN の場合は自動判別する(失敗したときは SJIS とみなす)
 *        [newlineCode(0)]
 *            LF_ONLY, CR_ONLY, CR_LF のいずれかを渡すことで改行コードを指定
 *            0 ならば変換しない
 *        [convKana(false)]
 *            true の場合、半角カタカナを全角カタカナに変換
 *        [convAlnum(false)]
 *            true の場合、全角アルファベットと一部の全角記号を半角に変換
 *  制限: 内部でいったん EUC にしているため JIS → SJIS の変換に無駄がある
 */
string convert(const string& input, const int outputCode, const int inputCode = UNKNOWN,
const int newlineCode = 0, const bool convKana = false, const bool convAlnum = false);

/**
 *  機能: JIS を EUC に変換
 *  返値: 変換された文字列
 */
string jis2euc(const string& input);

/**
 *  機能: SJIS を EUC に変換
 *  返値: 変換された文字列
 */
string sjis2euc(const string& input);

/**
 *  機能: EUC 文字列の改行コード、半角カナ、全角アルファベットを変換する
 *  返値: 変換された文字列
 *  オプション:
 *        convert() を参照
 */
string conv_euc(const string& input, const int newlineCode = 0, const bool convKana = false, const bool convAlnum = false);

/**
 *  機能: EUC を JIS に変換
 *  返値: 変換された文字列
 */
string euc2jis(const string& input);

/**
 *  機能: EUC を SJIS に変換
 *  返値: 変換された文字列
 */
string euc2sjis(const string& input);

}  // namespace jstring

#endif  // _JSTRING_H_

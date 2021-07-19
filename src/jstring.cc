/*
cgilib - a C++ library for CGI

Copyright (C) 2001-2004
  Kawazoe Tomonori, Ueda Satoshi, Hayase Yasuhiro, Harada Erimu
*/

#include "jstring.h"

/* 文字集合 */
enum CharactorSet {
	ASCII,      // ラテン集合(ISO/IEC 646)     JIS X 0201
	HANKANA,    // 半角カタカナ集合            JIS X 0201
	X0208,      // 漢字集合第1面(第1・第2水準) JIS X 0208
	X0213P1,    // 漢字集合第1面(第3水準)      JIS X 0213
	X0213P2,    // 漢字集合第2面(第4水準)      JIS X 0213
	X0212,      // 漢字集合第2面(補助漢字)     JIS X 0212
};


/* 制御文字コード */
static const unsigned int LF  = 0x0a;
static const unsigned int CR  = 0x0d;
static const unsigned int SO  = 0x0e;
static const unsigned int SI  = 0x0f;
static const unsigned int ESC = 0x1b;
static const unsigned int SP  = 0x20;
static const unsigned int DEL = 0x7f;
static const unsigned int SS2 = 0x8e;
static const unsigned int SS3 = 0x8f;


/* 半角カタカナ → 全角カタカナ */
static const unsigned char KanaTable[][2] = {
	{0x21, 0x23}, {0x21, 0x56}, {0x21, 0x57}, {0x21, 0x22}, //。「」、
	{0x21, 0x26}, {0x25, 0x72}, {0x25, 0x21}, {0x25, 0x23}, //・ヲァィ
	{0x25, 0x25}, {0x25, 0x27}, {0x25, 0x29}, {0x25, 0x63}, //ゥェォャ
	{0x25, 0x65}, {0x25, 0x67}, {0x25, 0x43}, {0x21, 0x3c}, //ュョッー
	{0x25, 0x22}, {0x25, 0x24}, {0x25, 0x26}, {0x25, 0x28}, //アイウエ
	{0x25, 0x2a}, {0x25, 0x2b}, {0x25, 0x2d}, {0x25, 0x2f}, //オカキク
	{0x25, 0x31}, {0x25, 0x33}, {0x25, 0x35}, {0x25, 0x37}, //ケコサシ
	{0x25, 0x39}, {0x25, 0x3b}, {0x25, 0x3d}, {0x25, 0x3f}, //スセソタ
	{0x25, 0x41}, {0x25, 0x44}, {0x25, 0x46}, {0x25, 0x48}, //チツテト
	{0x25, 0x4a}, {0x25, 0x4b}, {0x25, 0x4c}, {0x25, 0x4d}, //ナニヌネ
	{0x25, 0x4e}, {0x25, 0x4f}, {0x25, 0x52}, {0x25, 0x55}, //ノハヒフ
	{0x25, 0x58}, {0x25, 0x5b}, {0x25, 0x5e}, {0x25, 0x5f}, //ヘホマミ
	{0x25, 0x60}, {0x25, 0x61}, {0x25, 0x62}, {0x25, 0x64}, //ムメモヤ
	{0x25, 0x66}, {0x25, 0x68}, {0x25, 0x69}, {0x25, 0x6a}, //ユヨラリ
	{0x25, 0x6b}, {0x25, 0x6c}, {0x25, 0x6d}, {0x25, 0x6f}, //ルレロワ
	{0x25, 0x73}, {0x21, 0x2b}, {0x21, 0x2c}                //ン゛゜
};

/* 2バイト記号 → 1バイト記号 */
static const unsigned char MarkTable[] = {
	   0,   0,   0,   0, ',', '.',   0, ':',
	 ';', '?', '!',   0,   0,'\'', '`',   0,
	 '^',   0, '_',   0,   0,   0,   0,   0,
	   0,   0,   0,   0,   0, '-',   0, '/',
	'\\',   0,   0, '|',   0,   0, '`','\'',
	'\"','\"', '(', ')',   0,   0, '[', ']',
	 '{', '}', '<', '>',   0,   0,   0,   0,
	   0,   0,   0,   0, '+', '-',   0,   0,
	   0, '=',   0, '<', '>',   0,   0,   0,
	   0,   0,   0,   0,   0,   0,   0,   0,
	 '$',   0,   0, '%', '#', '&', '*', '@',
	   0,   0,   0,   0,   0,   0,   0,   0
};

/*  指示エスケープシーケンス  */
static const string ESC_ASCII      = "\x1b(B";
static const string ESC_X0201LATIN = "\x1b(J";
static const string ESC_X0201KANA  = "\x1b(I";
static const string ESC_X0208_1978 = "\x1b$@";
static const string ESC_X0208_1983 = "\x1b$B";
static const string ESC_X0208_1990 = "\x1b&@\1b$B";
static const string ESC_X0212      = "\x1b$(D";
static const string ESC_X0213P1    = "\x1b$(O";
static const string ESC_X0213P2    = "\x1b$(P";

static const string ESC_X0208      = "\x1b$(B";  // 一般的でない
static const string ESC_SWEDISH    = "\x1b(H";   // ESC_ASCII(誤)
static const string ESC_NEC_ASCII  = "\x1bH";    // ESC_ASCII(誤)
static const string ESC_NEC_JIS    = "\x1bK";    // ESC_X0208(誤)


/***************************************************************
 *
 *      インライン関数の宣言
 */

bool tokenCompare(const string&, const unsigned int, const string&);
bool isCL(const unsigned int);
bool isGL(const unsigned int);
bool isGR(const unsigned int);
bool isHANKANA(const unsigned int);
bool isJIS_KANJI(const unsigned int, const unsigned int);
bool isEUC_KANJI(const unsigned int, const unsigned int);
bool isEUC_KANA(const unsigned int, const unsigned int);
bool isSJIS_KANJI(const unsigned int, const unsigned int);
void han2zenKana(string&, const unsigned int);
void zen2hanAlnum(string&, const unsigned int, const unsigned int);
unsigned char toEucChar(const unsigned int);


/***************************************************************
 *
 *      インライン関数定義
 */


inline bool tokenCompare(const string& str, const unsigned int pos,
                         const string& token)
{
#ifdef __BASTRING__
	return (str.compare(token, pos, token.length()) == 0);
#else
	return (str.compare(pos, token.length(), token) == 0);
#endif
}


inline bool isCL(const unsigned int c)
{
	return (0 <= c && c <= SP || c == DEL);
}

inline bool isGL(const unsigned int c)
{
	return (SP < c && c < DEL);
}

inline bool isGR(const unsigned int c)
{
	return ((SP | 0x80) < c && c < (DEL | 0x80));
}

inline bool isHANKANA(const unsigned int c)
{
	return (0xa0 < c && c < 0xe0);
}

inline bool isJIS_KANJI(const unsigned int c0, const unsigned int c1)
{
	return (isGL(c0) && isGL(c1));
}

inline bool isEUC_KANJI(const unsigned int c0, const unsigned int c1)
{
	return (isGR(c0) && isGR(c1));
}

inline bool isEUC_KANA(const unsigned int c0, const unsigned int c1)
{
	return (c0 == SS2 && isHANKANA(c1));
}

inline bool isSJIS_KANJI(const unsigned int c0, const unsigned int c1)
{
	return ((0x80 < c0 && c0 < 0xfc && c0 != 0xa0 && !isHANKANA(c0)) &&
	        (0x40 <= c1 && c1 <= 0xfc && c1 != DEL));
}


inline void han2zenKana(string& output, const unsigned int hankana)
{
	const unsigned int length = output.length();
	int canDakuon = 0;
	
	if (length == 2 || (length >= 3 && static_cast<unsigned char>(output[length - 3]) != SS3)) {
		if (output[length - 2] == '\xa5') {
			const unsigned int c = static_cast<unsigned char>(output[length - 1]);
			if (0xab <= c && c <= 0xc1 && (c & 1) || c == 0xc4 || c == 0xc6 || c == 0xc8) {
				// 前の文字が"カ行サ行タ行"
				canDakuon = 1;
			} else if (c == 0xcf || c == 0xd2 || c == 0xd5 || c == 0xd8 || c == 0xdb) {
				// 前の文字が"ハ行"
				canDakuon = 2;
			} else if (c == 0xa6) {
				// 前の文字が"ウ"
				canDakuon = 3;
			}
		}
	}
	
	switch (hankana) {
	case 0xde:  // 濁音
		switch (canDakuon) {
		case 1:
		case 2:
			// 濁音付きカタカナ "ガ行ザ行ダ行バ行"
			output[length - 1] += 1;
			break;
		case 3:
			// "ヴ"
			output[length - 1] = '\xf4';
			break;
		default:
			output += '\xa1';
			output += '\xab';
			break;
		}
		break;
	case 0xdf:  // 半濁音
		switch (canDakuon) {
		case 2:
			// 半濁音付きカタカナ "パ行"
			output[length - 1] += 2;
			break;
		default:
			output += '\xa1';
			output += '\xac';
			break;
		}
		break;
	default:  // カタカナ
		output += toEucChar(KanaTable[hankana - 0xa1][0]);
		output += toEucChar(KanaTable[hankana - 0xa1][1]);
		break;
	}
}

inline void zen2hanAlnum(string& output, const unsigned int c0, const unsigned int c1)
{
	if (c0 == 0xa1) {
		const int c = MarkTable[c1 - 0xa0];
		
		if (c) {
			output += static_cast<unsigned char>(c);
		} else {
			output += static_cast<unsigned char>(c0);
			output += static_cast<unsigned char>(c1);
		}
	} else if (c0 == 0xa3 && (0xb0 <= c1 && c1 <= 0xb9 ||
	                          0xc1 <= c1 && c1 <= 0xda ||
	                          0xe1 <= c1 && c1 <= 0xfa)) {
		output += static_cast<unsigned char>(c1 & 0x7f);
	} else {
		output += static_cast<unsigned char>(c0);
		output += static_cast<unsigned char>(c1);
	}
}

inline void outputNewlineCode(string &output, const unsigned int newlineCode)
{
	switch (newlineCode) {
	case jstring::LF_ONLY:
		output += static_cast<unsigned char>(LF);
		break;
	case jstring::CR_ONLY:
		output += static_cast<unsigned char>(CR);
		break;
	case jstring::CR_LF:
		output += static_cast<unsigned char>(CR);
		output += static_cast<unsigned char>(LF);
		break;
	default:
		break;
	}
}

inline unsigned char toEucChar(const unsigned int ch)
{
	return static_cast<unsigned char>(ch | 0x80);
}

/***************************************************************
 *
 *      関数定義
 */


int jstring::getcode(const string& input, const bool checkHankana)
{
	const unsigned int length = input.length();
	register unsigned int c0, c1 = 0;
	bool twoCharsChecked = true;
	
	if (input.empty()) {
		return jstring::UNKNOWN;
	}
	
	for (unsigned int i = 0; i < length; ++i) {
		c0 = c1;
		c1 = static_cast<unsigned char>(input[i]);
		// static_castしないと符号拡張されてしまう
		if (twoCharsChecked) {
			twoCharsChecked = false;
			continue;
		}
		
		// ISO-2022-JP
		if (c1 == ESC) {
			if (tokenCompare(input, i, ESC_X0208_1978) ||
			    tokenCompare(input, i, ESC_X0208_1983) ||
			    tokenCompare(input, i, ESC_X0208_1990) ||
			    tokenCompare(input, i, ESC_X0208) ||
			    tokenCompare(input, i, ESC_NEC_JIS)) {
				return jstring::JIS;
			}
			twoCharsChecked = false;
			continue;
		}
		
		// ShiftJIS or EUC-JP
		else if (isSJIS_KANJI(c0, c1)) {
			if (isEUC_KANJI(c0, c1)) {
				twoCharsChecked = true;
				continue;
			}
			if (checkHankana && isEUC_KANA(c0, c1)) {
				twoCharsChecked = true;
				continue;
			}
			return jstring::SJIS;
		}
		else if (isEUC_KANJI(c0, c1)) {
			if (checkHankana && isHANKANA(c0)) {
				while (true) {
					// (EUC1, EUC2), (kana || SJIS2, ?)
					if (isHANKANA(c1)) {
						// (EUC1, EUC2), (kana || SJIS2, kana)
						break;
					}
					// (EUC1, EUC2), (kana || SJIS2, !kana)
					if (length <= ++i) {
						return jstring::EUC;
					}
					c0 = c1;
					c1 = static_cast<unsigned char>(input[i]);
					
					// (EUC2, ?), (!kana, ?)
					if (!isSJIS_KANJI(c0, c1)) {
						return jstring::EUC;
					}
					// (EUC2, ?), (SJIS1, SJIS2)
					if (isGL(c1)) {
						// (EUC2, ASCII), (SJIS1, SJIS2)
						break;
					}
					// (EUC2, !ASCII), (SJIS1, SJIS2)
					if (length <= ++i) {
						return jstring::SJIS;
					}
					c0 = c1;
					c1 = static_cast<unsigned char>(input[i]);
					
					// (!ASCII, ?), (SJIS2, ?)
					if (!isEUC_KANJI(c0, c1) && !isEUC_KANA(c0, c1)) {
						return jstring::SJIS;
					}
					// (EUC1, EUC2), (SJIS2, ?)
				}
				twoCharsChecked = true;
				continue;
			}
			return jstring::EUC;
		}
		else if (checkHankana && isHANKANA(c0)) {
			return jstring::SJIS;
		}
	}  // for (unsigned int i = 0; i <= length; ++i)
	
	return jstring::UNKNOWN;
}


string jstring::convert(const string& input, const int outputCode, const int inputCode, const int newlineCode, const bool convKana, const bool convAlnum)
{
	string euc;
	int icode = inputCode;
	
	if (icode == jstring::UNKNOWN) {
		if ((icode = jstring::getcode(input)) == jstring::UNKNOWN) {
			icode = SJIS;
		}
	}
	switch (icode) {
	case jstring::JIS:
		euc = jstring::jis2euc(input);
		break;
	case jstring::SJIS:
		euc = jstring::sjis2euc(input);
		break;
	case jstring::EUC:
	default:
		euc = input;
		break;
	}
	if (newlineCode || convKana || convAlnum) {
		euc = jstring::conv_euc(euc, newlineCode, convKana, convAlnum);
	}
	switch (outputCode) {
	case jstring::JIS:
		return jstring::euc2jis(euc);
	case jstring::SJIS:
		return jstring::euc2sjis(euc);
	case jstring::EUC:
	default:
		return euc;
	}
}


string jstring::jis2euc(const string& input)
{
	string output;
	const unsigned int length = input.length();
	register unsigned int prev = 0, ch;
	CharactorSet g0 = ASCII;
	bool invokeG1 = false;
	
	for (unsigned int i = 0; i < length; ++i) {
		ch = static_cast<unsigned char>(input[i]);
		
		if (isCL(ch)) {
			if (ch == ESC) {
				// エスケープシーケンスの判定
				if (tokenCompare(input, i, ESC_ASCII) ||
				    tokenCompare(input, i, ESC_X0201LATIN) ||
				    tokenCompare(input, i, ESC_SWEDISH)) {
					g0 = ASCII;
					i += 2;
				} else if (tokenCompare(input, i, ESC_NEC_ASCII)) {
					g0 = ASCII;
					++i;
				} else if (tokenCompare(input, i, ESC_X0201KANA)) {
					g0 = HANKANA;
					i += 2;
				} else if (tokenCompare(input, i, ESC_X0208_1978) ||
				           tokenCompare(input, i, ESC_X0208_1983)) {
					g0 = X0208;
					prev = 0;
					i += 2;
				} else if (tokenCompare(input, i, ESC_X0208_1990)) {
					g0 = X0208;
					prev = 0;
					i += 5;
				} else if (tokenCompare(input, i, ESC_X0208)) {
					g0 = X0208;
					prev = 0;
					i += 3;
				} else if (tokenCompare(input, i, ESC_NEC_JIS)) {
					g0 = X0208;
					prev = 0;
					i += 1;
				} else if (tokenCompare(input, i, ESC_X0213P1)) {
					g0 = X0213P1;
					prev = 0;
					i += 3;
				} else if (tokenCompare(input, i, ESC_X0213P2)) {
					g0 = X0213P2;
					prev = 0;
					i += 3;
				} else if (tokenCompare(input, i, ESC_X0212)) {
					g0 = X0212;
					prev = 0;
					i += 3;
				} else {
					// 本当にエスケープシーケンス? よく分からないので残す
					output += static_cast<unsigned char>(ch);
				}
			} else if (ch == SI) {
				invokeG1 = false;
			} else if (ch == SO) {
				invokeG1 = true;
			} else {
				// JISX0201のラテン部分[半角アルファベット]
				output += static_cast<unsigned char>(ch);
			}
		} else if (isGL(ch)) {
			if (invokeG1) {
				// GLにG0が呼び出されている
				// JISX0201のカナ部分[半角カタカナ]
				output += static_cast<unsigned char>(SS2);
				output += toEucChar(ch);
			} else {
				switch (g0) {
				case ASCII:
					// JISX0201のラテン部分[半角アルファベット]
					output += static_cast<unsigned char>(ch);
					break;
				case HANKANA:
					// JISX0201のカナ部分[半角カタカナ]
					output += static_cast<unsigned char>(SS2);
					output += toEucChar(ch);
					break;
				case X0208:
				case X0213P1:
					if (prev) {
						// JISX0208(第一水準・第二水準)とJISX0213の第三水準
						output += toEucChar(prev);
						output += toEucChar(ch);
						prev = 0;
					} else {
						prev = ch;
					}
					break;
				case X0213P2:
				case X0212:
					if (prev) {
						// JISX0212(補助漢字)とJISX0213の第四水準
						output += static_cast<unsigned char>(SS3);
						output += toEucChar(prev);
						output += toEucChar(ch);
						prev = 0;
					} else {
						prev = ch;
					}
					break;
				default:
					break;
				}
			}
		} else if (isHANKANA(ch)) {
			// JISX0201のカナ部分[半角カタカナ]
			output += static_cast<unsigned char>(SS2);
			output += static_cast<unsigned char>(ch);
		}
	}
	return output;
}



string jstring::sjis2euc(const string& input)
{
	string output;
	const unsigned int length = input.length();
	register unsigned int prev = 0, ch;
	
	for (unsigned int i = 0; i < length; ++i) {
		ch = static_cast<unsigned char>(input[i]);
		
		if (prev) {
			if (prev < DEL) {
				// JISX0208(第一水準・第二水準)とJISX0213の第三水準
				if (ch < 0x9f) {
					output += toEucChar(prev - 1);
					output += toEucChar(ch - (ch < DEL ? 0x1f : 0x20));
				} else {
					output += toEucChar(prev);
					output += toEucChar(ch - 0x7e);
				}
			} else {
				// JISX0212(補助漢字)とJISX0213の第四水準
				if (ch < 0x9f) {
					output += static_cast<unsigned char>(SS3);
					output += toEucChar(prev - (prev < 0x86 ? 0x5f : (prev < 0x8a ? 0x59 : 0x1b)));
					output += toEucChar(ch - (ch < DEL ? 0x1f : 0x20));
				} else {
					output += static_cast<unsigned char>(SS3);
					output += toEucChar(prev - (prev == 0x82 ? 0x5e : (prev < 0x88 ? 0x58 : 0x1a)));
					output += toEucChar(ch - 0x7e);
				}
			}
			prev = 0;
		} else {
			if (0x81 <= ch && ch <= 0x9f) {
				// SJIS 2バイト文字の1バイト目
				prev = (ch - 0x70) << 1;
			} else if (isHANKANA(ch)) {
				// JISX0201のカナ部分[半角カタカナ]
				output += static_cast<unsigned char>(SS2);
				output += static_cast<unsigned char>(ch);
				prev = 0;
			} else if (0xe0 <= ch && ch <= 0xfc) {
				// SJIS 2バイト文字の1バイト目
				prev = (ch - 0xb0) << 1;
			} else {
				// JISX0201のラテン部分[半角アルファベット]
				output += static_cast<unsigned char>(ch);
			}
		}
	}
	return output;
}



string jstring::conv_euc(const string& input, const int newlineCode, const bool convKana, const bool convAlnum)
{
	string output;
	const unsigned int length = input.length();
	register unsigned int prev = 0, ch;
	CharactorSet single_shift = X0208;
	
	for (unsigned int i = 0; i < length; ++i) {
		ch = static_cast<unsigned char>(input[i]);
		
		if (isCL(ch) || isGL(ch)) {
			// 改行コード
			if (ch == CR && newlineCode) {
				outputNewlineCode(output, newlineCode);
				prev = CR;
			} else if (ch == LF && newlineCode) {
				// CR LFと来た場合は一つの改行とみなす
				if (prev != CR) {
					outputNewlineCode(output, newlineCode);
				}
				prev = 0;
			} else {
				// 解釈しない
				output += static_cast<unsigned char>(ch);
				prev = 0;
			}
			single_shift = X0208;
		} else if (isGR(ch)) {
			if (prev && prev != CR) {
				if (single_shift == X0208) {
					if (convAlnum) {
						zen2hanAlnum(output, prev, ch);
					} else {
						output += static_cast<unsigned char>(prev);
						output += static_cast<unsigned char>(ch);
					}
				} else if (single_shift == X0212) {
					output += static_cast<unsigned char>(SS3);
					output += static_cast<unsigned char>(prev);
					output += static_cast<unsigned char>(ch);
					single_shift = X0208;
				}
				prev = 0;
			} else {
				if (single_shift == HANKANA && isHANKANA(ch)) {
					if (convKana) {
						han2zenKana(output, ch);
					} else {
						output += static_cast<unsigned char>(SS2);
						output += static_cast<unsigned char>(ch);
					}
				} else {
					prev = ch;
				}
				single_shift = X0208;
			}
		} else {
			if (ch == SS2) {
				single_shift = HANKANA;
			} else if (ch == SS3) {
				single_shift = X0212;
			}
			prev = 0;
		}
	}
	return output;
}



string jstring::euc2jis(const string& input)
{
	string output;
	const unsigned int length = input.length();
	register unsigned int prev = 0, ch;
	CharactorSet single_shift = X0208, g0 = ASCII;
	
	for (unsigned int i = 0; i < length; ++i) {
		ch = static_cast<unsigned char>(input[i]);
		
		if (isCL(ch) || isGL(ch)) {
			// ラテン集合
			if (g0 != ASCII) {
				// G0にUS-ASCII(ISO-646)を読み込むエスケープシーケンス
				// JISX0201ラテン集合のほうが良いのだろうか?
				output += ESC_ASCII;
				g0 = ASCII;
			}
			output += static_cast<unsigned char>(ch);
			prev = 0;
			single_shift = X0208;
		} else if (isGR(ch)) {
			if (prev) {
				if (single_shift == X0208) {
					// JISX0208(第一水準・第二水準)とJISX0213の第三水準
					if (g0 != X0208) {
						output += ESC_X0208_1983;
						g0 = X0208;
					}
					output += static_cast<unsigned char>(prev & 0x7f);
					output += static_cast<unsigned char>(ch & 0x7f);
				} else if (single_shift == X0212) {
					// JISX0212(補助漢字)とJISX0213の第四水準
					if (g0 != X0212) {
						output += ESC_X0212;
						g0 = X0212;
					}
					output += static_cast<unsigned char>(prev & 0x7f);
					output += static_cast<unsigned char>(ch & 0x7f);
				}
				prev = 0;
				single_shift = X0208;
			} else {
				if (single_shift == HANKANA && isHANKANA(ch)) {
					// JISX0201のカナ部分[半角カタカナ]
					if (g0 != HANKANA) {
						output += ESC_X0201KANA;
						g0 = HANKANA;
					}
					output += static_cast<unsigned char>(ch & 0x7f);
					single_shift = X0208;
				} else {
					prev = ch;
				}
			}
		} else {
			if (ch == SS2) {
				single_shift = HANKANA;
			} else if (ch == SS3) {
				single_shift = X0212;
			}
			prev = 0;
		}
	}
	if (g0 != ASCII) {
		output += ESC_ASCII;
	}
	return output;
}


string jstring::euc2sjis(const string& input)
{
	string output;
	const unsigned int length = input.length();
	register unsigned int prev = 0, ch;
	CharactorSet single_shift = X0208;
	
	for (unsigned int i = 0; i < length; ++i) {
		ch = static_cast<unsigned char>(input[i]);
		
		if (isCL(ch) || isGL(ch)) {
			// ラテン集合
			output += static_cast<unsigned char>(ch);
			prev = 0;
			single_shift = X0208;
		} else if (isGR(ch)) {
			if (prev) {
				prev &= 0x7f;
				ch &= 0x7f;
				if (single_shift == X0208) {
					// JISX0208(第一水準・第二水準)とJISX0213の第三水準
					output += static_cast<unsigned char>(((prev + 1) >> 1) + (prev < 0x5f ? 0x70 : 0xb0));
					output += static_cast<unsigned char>((prev % 2 ? (ch <= 0x5f ? 0x1f : 0x20) : 0x7e) + ch);
				} else if (single_shift == X0212) {
					// JISX0212(補助漢字)とJISX0213の第四水準
					output += static_cast<unsigned char>(((prev + 1) >> 1) + (prev < 0x28 ? 0xdf : (prev < 0x6f ? 0xdc : 0xbd)));
					output += static_cast<unsigned char>((prev % 2 ? (ch <= 0x5f ? 0x1f : 0x20) : 0x7e) + ch);
				}
				prev = 0;
				single_shift = X0208;
			} else {
				if (single_shift == HANKANA && isHANKANA(ch)) {
					// JISX0201のカナ部分[半角カタカナ]
					output += static_cast<unsigned char>(ch);
					single_shift = X0208;
				} else {
					prev = ch;
				}
			}
		} else {
			if (ch == SS2) {
				single_shift = HANKANA;
			} else if (ch == SS3) {
				single_shift = X0212;
			}
			prev = 0;
		}
	}
	return output;
}

/*** テスト用

#include <iostream>
// #include "jstring.h"
using namespace std;

int main()
{
	string s;
	
	while (cin) {
		s += cin.get();
	}
	cerr << "読み込み完了" << endl;
	cout << jstring::convert(s, jstring::EUC, jstring::UNKNOWN, jstring::LF_ONLY, true, true);
	
	return 0;
}

***/


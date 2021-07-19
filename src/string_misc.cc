/*
cgilib - a C++ library for CGI

Copyright (C) 2001-2004
  Kawazoe Tomonori, Ueda Satoshi, Hayase Yasuhiro, Harada Erimu
*/

#include "string_misc.h"
#include <string.h>

// すべての replace はこれを呼び出す.
static string & replace_internal(string &target,
                               const string &pattern,
                               const string &replacement)
{
    string::size_type n = 0;
    
    if( pattern.length() == 0 ){
	return target;
    }
    
    while( (n = target.find(pattern, n)) != string::npos ){
	target.replace(n, pattern.length(), replacement);
	n += replacement.length();
    }
    
    return target;
}

string & string_misc::replace(string &target,
                              const string &pattern,
                              const string &replacement)
{
    return replace_internal(target, pattern, replacement);
}

string & string_misc::replace(string &target,
                              const char *pattern,
                              const string &replacement)
{
    return replace_internal(target, pattern, replacement);
}

string & string_misc::replace(string &target,
                              const string &pattern,
                              const char *replacement)
{
    return replace_internal(target, pattern, replacement);
}

string & string_misc::replace(string &target,
                              const char *pattern,
                              const char *replacement)
{
    return replace_internal(target, pattern, replacement);
}

string & string_misc::replace2(string &target, const map<string, string> &repmap)
{
    string firstchars;
    map<string, string>::const_iterator mapitr;
    multimap<char, string> charcache;
    
    // firstchars : 置換ターゲットの先頭1文字ずつを格納、重複は許さない
    // charcache  : 先頭1文字をキーに置換ターゲットを格納
    for( mapitr=repmap.begin(); mapitr!=repmap.end(); mapitr++ ){
        if( mapitr->first.length() == 0 ){
            continue;
        }
        char c = mapitr->first[0];
        if( firstchars.find(c) == string::npos ){
            firstchars += c;
        }
        charcache.insert(std::pair<char, string>(c, mapitr->first));
    }
    // printf("first chars: '%s'\n", firstchars.c_str());
    
    string::size_type n = 0;
    multimap<char, string>::const_iterator cacheitr;
    
    while( (n=target.find_first_of(firstchars,n)) != string::npos ){
        // printf("target[%d] = '%c'\n", n, target[n]);
        for( cacheitr = charcache.lower_bound(target[n]);
             cacheitr != charcache.upper_bound(target[n]);
             cacheitr++ ){
            const string &pattern = cacheitr->second;
            // printf(" target  '%s'\n", target.substr(n, pattern.length()).c_str());
            // printf(" pattern '%s'\n", pattern.c_str());
            if( target.substr(n, pattern.length()).compare(pattern) == 0 ){
                // operator [] は const では使えないのでリライト.
                /*
                // puts("match..replace");
                target.replace(n, pattern.length(), repmap[pattern]);
                // printf("replaced string: '%s'\n", target.c_str());
                n += repmap[pattern].length();
                goto loop_end; // 2段またいでのcontinueがないので
                 */
                mapitr = repmap.find(pattern);
                target.replace(n, pattern.length(), mapitr->second);
                n += mapitr->second.length();
                goto loop_end;
            }
        }
        n += 1;
        loop_end:
        ; // NOP
    }
    
    return target;
}

list<string> string_misc::split(const string &target, char separators[])
{
    list<string> ls;
    string src(target);
    string::size_type cur, pos;
    
    // 空なら、すぐ返る
    if( target.length() == 0 || separators[0] == '\0' )
        return ls;
    
    cur = 0;
    // 番兵をおく
    src += separators[0];
    
    while( (pos=src.find_first_of(separators, cur)) != string::npos ){
        ls.push_back( src.substr(cur, pos-cur) );
        cur = pos+1;
    }
    
    return ls;
}

// サニタイジング用関数
// HTMLのタグでない部分、HTMLタグの属性値(<img src=XXXここXXX>)用
// 
// 以下の置換を行う
// & → &amp;
// < → &lt;
// > → &gt;
// " → &quot;
// ' → &#39;

string string_misc::sanitize(const string &src)
{
    string::const_iterator iter;
    string dst;
    
    for( iter=src.begin(); iter!=src.end(); iter++ ){
	switch( *iter ){
	case '&':
	    dst += "&amp;";
	    break;
	case '<':
	    dst += "&lt;";
	    break;
	case '>':
	    dst += "&gt;";
	    break;
	case '\"':
	    dst += "&quot;";
	    break;
	case '\'':
	    dst += "&#39;";
	    break;
	default:
	    dst += *iter;
	    break;
	}
    }
    return dst;
}

string string_misc::sanitize_url_scheme(const string &src,
                                        const list<string> &schemes)
{
    string::const_iterator iter;
    list<string>::const_iterator iter2;
    
    // 許されている文字だけかどうかチェック.
    // ; / ? : @ & = + $ ,
    // a-z, A-Z, 0-9
    // - _ . ! ~ * ' ( )
    for( iter=src.begin(); iter!=src.end(); iter++ ){
        if( isalnum( *iter ) )
          continue;
        switch( *iter ){
        case ';':
        case '/':
        case '?':
        case ':':
        case '@':
        case '&':
        case '=':
        case '+':
        case '$':
        case ',':
        case '\\':
        case '-':
        case '_':
        case '.':
        case '!':
        case '~':
        case '*':
        case '\'':
        case '(':
        case ')':
        case '%':
            continue;
        default:
            return "";
        }
    }
    
    // schemeのチェック
    bool allowed = false;
    for( iter2=schemes.begin(); iter2!=schemes.end(); iter2++ ){
        if( strncasecmp( src.c_str(), iter2->c_str(), iter2->length() ) == 0 ){
            allowed = true;
            break;
        }
    }
    if( !allowed ){
        return "";
    }
    
    // 最後に & と ' だけ置換してreturn
    string dst( src );
    
    string_misc::replace(dst, "&", "&amp;");
    string_misc::replace(dst, "\'", "&#39;");
    
    return dst;
}

string string_misc::sanitize_url(const string &src)
{
    list<string> l;
    
    l.push_back("http:");
    l.push_back("https:");
    l.push_back("mailto:");
    
    return sanitize_url_scheme(src, l);
}

string string_misc::trim(const string &s)
{
    const char spaces[] = " \t\n\r\v";
    string::size_type p1, p2;
    
    p1 = s.find_first_not_of(spaces);
    
    if( p1 == string::npos ){
        return "";
    }
    
    p2 = s.find_last_not_of(spaces);
    
    return s.substr(p1, p2-p1+1);
}

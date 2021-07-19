/*
cgilib - a C++ library for CGI

Copyright (C) 2001-2004
  Kawazoe Tomonori, Ueda Satoshi, Hayase Yasuhiro, Harada Erimu
*/

/** @file Form.cc
  * このファイルでは、cgi::Formクラスの定義を行っています。
  */

#include "Form.h"
#include <memory>
#include <string.h>

namespace cgi {

// static変数
Form::associArray Form::Pair;
string Form::formData;
string Form::ErrMesg;
bool Form::decoded = false;
bool Form::parseFlag = false;

// public

Form::Form()
{
    // 初回だけ、フォームをデコードする
    if( decoded == false ){
        parseFlag = parseFormData();
	decoded = true;
    }
}

bool Form::find(const string &name) const
{
    if( Pair.find(name) != Pair.end() )
	return true;
    else
	return false;
}

string Form::operator [](const string& name) const
{
    const_iterator i = Pair.find(name);
    
    if( i != Pair.end() ){
	return i->second;
    } else {
	return string("");
    }
}

bool Form::getValue(const string& name, string& value) const
{
    const_iterator i = Pair.find(name);
    if( i != Pair.end() ) {
	value = i->second;
	return true;
    } else {
	return false;
    }
}

bool Form::getMultiValue(const string& name,
			 pair<iterator, iterator>& p) const
{
    pair<iterator, iterator> tmp_p;
    
    tmp_p = Pair.equal_range(name);
    if( tmp_p.first == tmp_p.second)
	return false;
    p = tmp_p;
    return true;
}

string Form::getErrMesg() const
{
    return ErrMesg;
}

string Form::getFormData() const
{
    return formData;
}

// private
// 
// フォームの入力を連想配列 Pair に格納する
// POST メソッドと GET メソッドに対応
// 失敗すれば false を返す
// 
// (注) このメソッドを複数回呼んではいけない
// 
bool Form::parseFormData()
{
    char    *method, *content_length, *query_string;
    int     length;
    
    method = getenv("REQUEST_METHOD");
    
    if( method == NULL ){
        fatal("Form: Cannot get REQUEST_METHOD.");
        return false;
    }
    
    if( !strcasecmp(method, "POST") ){
        // POSTメソッドでとる場合
        char *p = getenv("CONTENT_TYPE");
	
	if( p == NULL ){
	    fatal("Form: Cannot get CONTENT_TYPE.");
	    return false;
	}
	
	string content_type = p;
        
        if( content_type.substr(0, 19).compare("multipart/form-data") == 0 ){
            // multipartで来た場合
            string::size_type boundary_pos = content_type.find("boundary=");
            if( boundary_pos == string::npos ){
                fatal("Form: Boundary doesn't defined.");
                return false;
            }
	    
            string boundary = content_type.substr(boundary_pos+9);
            content_length = getenv("CONTENT_LENGTH");
	    if( content_length == NULL ){
                fatal("Form: Cannot get CONTENT_LENGTH.");
                return false;
	    }
	    length = atoi(content_length);
	    char *content = new char[length+1];
	    cin.read(content, length);
	    formData = string(content, length);
	    
	    /*
	    cin >> str_buff;
	    if( (int)str_buff.length() != length ){
		fatal("Form: Read length doesn't correspond to"
		      " CONTENT_LENGTH.");
	        return false;
	    }
	    */
	    bool ret= ParseMultipart(formData, boundary);
	    
	    delete[] content;
	    return ret;
	    
        } else {
	    // multipartではない場合
            content_length = getenv("CONTENT_LENGTH");
            if( content_length == NULL) {
                fatal("Form: Cannot get CONTENT_LENGTH.");
                return false;
	    }
	    length = atoi(content_length);
	    cin >> formData;
	    if( (int)formData.length() != length ){
		fatal("Form: Read length doesn't correspond to"
		      " CONTENT_LENGTH.");
		return false;
	    }
	    
            return ParseURLEncoded(formData);
        }
    } else if( !strcasecmp(method, "GET") ) {
        // GETメソッドでとる場合
        query_string = getenv("QUERY_STRING");
	if( query_string == NULL) {
            fatal("Form: Cannot get QUERY_STRING.");
            return false;
	}
	formData = query_string;
	return ParseURLEncoded(formData);
    } else {
	// メソッドが不明
        fatal("Form: REQUEST_METHOD is unspecified.");
        return false;
    }
    
    // never reach
    return true;
}

bool Form::ParseURLEncoded(const string& str_buff)
{
    string token, key;
    int i, length;
    char buf[3];
    
    // フォームから渡された文字列を分解し、連想配列に格納する
    length = str_buff.size();
    for( i=0; i < length; i++ ){
	switch( str_buff[i] ){
	case '%':
	    if (!isxdigit((int)str_buff[i + 1]) ||
		!isxdigit((int)str_buff[i + 2])) {
		//fatal("Form: The character which couldn't be expected"
		//" appeared.");
		return false;
	    }
	    buf[0] = str_buff[++i];
	    buf[1] = str_buff[++i];
	    buf[2] = '\0';
	    token += (char)strtoul(buf, NULL, 16);
	    break;
	case '+':
	    token += ' ';
	    break;
	case '=':
	    key = token;
	    token.erase();
	    break;
	case '&':
	    if( key.size() )
		Pair.insert(associArray::value_type(key, token));
	    key.erase();
	    token.erase();
	    break;
	default:
	    token += str_buff[i];
	    break;
	}
    }
    // 最後の一つ
    if( key.size() )
	Pair.insert(associArray::value_type(key, token));

    return true;
}

} // namespace cgi

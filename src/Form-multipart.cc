/*
cgilib - a C++ library for CGI

Copyright (C) 2001-2004
  Kawazoe Tomonori, Ueda Satoshi, Hayase Yasuhiro, Harada Erimu
*/

#include "Form.h"

// 次の行(LF)があるかどうかを調べる.
static string::size_type next_line_begin(const string &str,
					      string::size_type pos)
{
    string::size_type cur=pos;
    
    for( ; cur<str.length(); cur++ ){
        if( str[cur] == '\n' )
            return cur+1;
    }
    
    return string::npos;
}

static bool isBlankLine(const string &src, string::size_type pos,
                        string::size_type n)
{
    if( src[pos+n-1] != '\n' )
        return false;
    
    for(string::size_type i=pos+n-2; i>=pos; i--){
        if( src[i]!='\r' )
            return false;
    }
    
    return true;
}

static string::size_type ParseMultipartHeader(string &name,
                                              string &filename,
                                              string &type,
                                              const string &src)
{
    string::size_type next_line = 0;
    string::size_type cur;
    while(1){
        cur = next_line;
        next_line = next_line_begin(src, cur);
        
        if( isBlankLine(src, cur, next_line-cur) )
            return cur;
        
        string line = src.substr(cur, next_line-cur);
        
        // name の処理
        string::size_type begin_pos = line.find("name=\"");
        if( begin_pos!=string::npos ){
            name.assign(src, begin_pos+6,
                        line.find('"', begin_pos+6)-(begin_pos+6));
        } else {
            begin_pos = line.find("name=");
            if( begin_pos!=string::npos ){
                name.assign(src, begin_pos+5,
                            line.find_first_of(" \t\r\n", begin_pos+5)-(begin_pos+5) );
            }
        }
        
        // filenameの処理
        begin_pos = line.find("filename=\"");
        if( begin_pos!=string::npos ){
            filename.assign(src, begin_pos+10,
                        line.find('"', begin_pos+10)-(begin_pos+10));
        } else {
            begin_pos = line.find("filename=");
            if( begin_pos!=string::npos ){
                filename.assign(src, begin_pos+9,
                        line.find_first_of(" \t\r\n", begin_pos+9)-(begin_pos+9) );
            }
        }
        
        string::size_type start_pos;
        begin_pos = line.find("Content-Type:");
        if( begin_pos != string::npos ){
            start_pos = line.find_first_not_of(" \t\"", begin_pos+13);
            type.assign( line, start_pos,
                         line.find_first_of(" \t\"\n", start_pos)-start_pos-1);
        }
        
        cur=next_line;
    }
}


bool cgi::Form::ParseMultipart(const string& src, const string &boundary)
{
    string separator = "--" + boundary;
    
    if( src.substr(0, separator.length()) != separator )
        return false;
    if( src.substr(separator.length(), 2) == "--" )
        return true;
    
    string::size_type cur = next_line_begin(src, 0);
    while( 1 ){
        string::size_type separator_pos = src.find(separator, cur+1);
        if(separator_pos==string::npos)
            return false;
        
        string::size_type header_begin = cur;//= src.find('\n', cur+separator.length());
        
        if( src[separator_pos-1]!='\n' )
            return false;
        string::size_type content_end = src.find_last_not_of('\r',separator_pos-2)+1;
        if(content_end==string::npos)
            return false;
        
        string name, filename, type;
        string::size_type header_len =
            ParseMultipartHeader(name, filename, type, 
                                 src.substr(header_begin, content_end-header_begin) );
        if( header_len == string::npos )
            return false;
        string::size_type content_begin =
            next_line_begin(src, header_begin+header_len);
        if( content_begin == string::npos )
            return false;
        
        Pair.insert(make_pair(name,src.substr(content_begin, content_end-content_begin)));
        
        if( filename.length() > 0 ){
            string name_of_filename;
            
            name_of_filename = name + "_name";
            Pair.insert( make_pair(name_of_filename, filename) );
        }
        
        if( type.length() > 0 ){
            string name_of_type;
            
            name_of_type = name + "_type";
            Pair.insert( make_pair(name_of_type, type) );
        }
        
        //最後のcontentならbreak
        if( src.substr(separator_pos+separator.length(), 2) == "--" )
            break;
        //そうでなければcurを進めてもう一回
        cur = next_line_begin(src, separator_pos+separator.length());
    }
    return true;
}

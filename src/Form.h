/*
cgilib - a C++ library for CGI

Copyright (C) 2001-2004
  Kawazoe Tomonori, Ueda Satoshi, Hayase Yasuhiro, Harada Erimu
*/

/** @file Form.h
  * このファイルでは、cgi::Formクラスの宣言を行っています。
  */

#ifndef _FORM_H_
# define _FORM_H_

# include <string>
# include <map>
# include <iostream>
# include <utility>

using namespace std;

namespace cgi {
    

/** フォームのデコードを行うクラス。
  * 
  * このクラスは、POST・GETで渡されたフォームデータの解析を行う。
  * <br>
  * 解析は最初のインスタンスを生成する時のみ行うため、
  * 複数のインスタンスを生成した場合でも、
  * 性能上の大きなペナルティにはならない。
  */
  
class Form {
public:
   typedef multimap<string, string> associArray;
   typedef associArray::iterator iterator;
   typedef associArray::const_iterator const_iterator;
   
   Form();
   
   /** 指定したnameのフォーム値が存在するかどうかを調べる。
     * 
     * @param name 検索したいフォーム値のname
     * @retval true  値が存在
     * @retval false 値が存在しない
     */
   bool find(const string &name) const;
  
   /** 指定したnameのフォーム値を取得する。
     * 対応する値がない場合には、空文字列を返す。
     * 
     * @param name 取得したいフォーム値のname
     * @retval フォーム値
     */
    string operator [](const string& name) const;
    
    /** 指定したnameのフォーム値を取得し、valueに格納する。
      * 値が存在しない場合はvalueには何の変更も加えない。
      * 値が存在すればtrueを、存在しなければfalseを返す。
      * 
      * @param name 取得したいフォーム値のname
      * @param value 取得した値を格納する文字列
      * @retval true 値が存在
      * @retval false 値が存在しない
      */
    bool getValue(const string& name, string& value) const;
    
    /** 指定したnameのフォーム値をすべて取得する。
     * 
     * @param name 取得したいフォーム値のname
     * @param p 取得した値の始点と終点を表すiteratorのpair
     * @retval true 値が存在
     * @retval false 値が存在しない
     */
    bool getMultiValue(const string& name,
		      pair<iterator, iterator>& p) const;
   
   /// getFormData が失敗した場合に設定される、
   /// エラーメッセージを返す
   string getErrMesg() const;
   
   /// FORMで渡されたデータを返す.
   string getFormData() const;
   
private:
   
   static string formData;
   static bool decoded;
   static bool parseFlag;
   static associArray Pair;
   static string ErrMesg;
   
   bool parseFormData();
   
   bool ParseURLEncoded(const string&);
   bool ParseMultipart(const string& src, const string &boundary);
   
   void fatal(const string& str) {
       ErrMesg = str;
   }
};

}  // namespace cgi

#endif

/* created by: fanghui  on Monday 15 November 2010  */

#ifndef __CRYPT_H__
#define __CRYPT_H__

#include <cstdlib>
#include <iostream>
#include <string>

using namespace std;

int  md5HexToDec( const string md5Str );
void md5String( const char* md5Digit,  string &md5Str );

void encJavaFile( string fileName );
void decJavaFile( string fileName );
string fileDigest(const string& file);


#endif

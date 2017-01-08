/* created by: fanghui  on Monday 15 November 2010  */



#include "md5.h"
#include "crypt.h"



// PINid consists of 16 bytes, represented in hexdec len= 32
int md5HexToDec( const string md5Str )
{
	//char MD5[16];
	int L = md5Str.length();
	//if (L != 32)  return 0;
	
	int s = 0;
	for( int i=0;i< L;i++   ) {  
		char ch = md5Str[i];
		if     (ch >= '0' && ch <= '9')  s += ch - '0';
		else if(ch >= 'a' && ch <= 'f')  s += ch - 'a'+10;  
		else if(ch >= 'A' && ch <= 'F')  s += ch - 'A'+10;  
        //else return  0;      

        //  mid   <<=   ((len-i-1)<<2);  
        //  idec   |=   mid;      
	}
	
	return s;  
}



void md5String( const char* md5Digit,  string &md5Str )
{
	//char MD5[32];  // store string
	//sprintf( MD5, "%x", md5Digit );
	return;

}



void encJavaFile( string fileName )
{
}

void decJavaFile( string fileName )
{
}


string fileDigest(const string& file) 
{
	ifstream in(file.c_str(), ios::binary);
	if (!in) {
		return "";
	}

	MD5 md5;
	std::streamsize length;
	char buffer[1024];
	while (!in.eof()) {
		in.read(buffer, 1024);
		length = in.gcount();
		if (length > 0) {
			md5.update(buffer, length);
		}
	}
	in.close();
	return md5.toString();
}



void PrintMD5(const string& str, MD5& md5) 
{
	cout << "MD5(\"" << str << "\") = " << md5.toString() << endl;
}


int demoFileMD5() 
{
	//cout << MD5("abc").toString() << endl;
	//cout << MD5(ifstream("D:\\test.txt")).toString() << endl;
	//cout << MD5(ifstream("D:\\test.exe", ios::binary)).toString() << endl;
	//cout << FileDigest("D:\\test.exe") << endl;

	MD5 md5;
	md5.update("");
	PrintMD5("", md5);

	md5.update("a");
	PrintMD5("a", md5);

	md5.update("bc");
	PrintMD5("abc", md5);

	md5.update("defghijklmnopqrstuvwxyz");
	PrintMD5("abcdefghijklmnopqrstuvwxyz", md5);

	md5.reset();
	md5.update("message digest");
	PrintMD5("message digest", md5);

	//md5.reset();
	//md5.update(ifstream("D:\\test.txt"));
	//PrintMD5("D:\\test.txt", md5);
	return 0;
}

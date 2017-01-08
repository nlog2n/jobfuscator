/* created by: fanghui  on 23 December 2010  */

#include <windows.h>
#include <stdio.h>
//#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <string>
using namespace std;

extern "C" {
#include "..\filter\crypt.h"
}

#define FortyKB  40960L


long readFileBuffer(FILE *fp1, char *buf, long size)
{
	long len;
	char *p=buf;
	len=size;
	do {
		len=fread(buf,1,len,fp1);
		if(len>0) {
			p+=len;
			len=size-len;
		}
	} while(len>0);

	len=p-buf;
	return len;
}

int encryptJava(const char *inJavaFile, char* outJavaFile, int algo)
{
	char buf[FortyKB];
	long len, offset;
	FILE *fp1=NULL, *fp2=NULL;

	fp1=fopen(inJavaFile,"rb");
	if(fp1==NULL) {
		printf("%s can not be opened\n", inJavaFile);
		return 2;
	}

	fp2=fopen(outJavaFile,"wb");
	if(fp2==NULL) {
		printf("%s can not be opened\n", outJavaFile);
		return 3;
	}

	offset = 0;
    while(1) {
		len=readFileBuffer(fp1, buf, FortyKB);
		if(len<=0) break;

		EncryptBuffer( buf, len, offset);
		fwrite(buf,1,len,fp2);
		offset+=len;
	}

	fclose(fp1);
	fclose(fp2);
	return 0;
}



int main(int argc, char * const argv[])
{
   char *inJavaFile, *outJavaFile;
   int algo;
   if(argc != 4) {
      cout << "*************************************************************************\n";
      cout << "* Encrypt java files                                                    *\n";
      cout << "* Usage: vguardapp inJavaFile outJavaFile encryptAlgo                   *\n";
	  cout << "* Example: vguardapp HelloWorld.java HelloWorld_enc.java 1              *\n";
      cout << "*************************************************************************\n";
      return 0;
   }

   inJavaFile  = argv[1];
   outJavaFile = argv[2];
   algo = atoi(argv[3]);
   encryptJava( inJavaFile, outJavaFile, algo);

   return 1;
}


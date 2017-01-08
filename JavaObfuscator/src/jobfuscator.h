/* created by: fanghui  on Wednesday 3 November 2010  */

#ifndef __JOBFUSCATOR_H__
#define __JOBFUSCATOR_H__

#include <iostream>
#include <fstream>
#include <string>
using namespace std;

#include "bbconfig.h"



class JavaObfuscator{
	private:
		int     software_secret_key;
		string  m_PINid;

        string  m_outCodPath;
        string  m_outCodName;

		//ofstream errFile;
        FILE    *errFile ;

	
	public:
	
	private:
		int  getWrapperID();
		void createCheckCode();
        void setEnv();
	
	public:
		JavaObfuscator();
		JavaObfuscator(string outCodPath, string outCodName, string PINid);
	   ~JavaObfuscator();
   	    bool customizeJava();
        bool compileJarFile();
   	    bool callExternalObfuscator();
		bool preverifyJarFile();
		bool compileCODFile();
        bool signCODFile();
        void cleanTmpFiles();

   	    void obfuscateJava();   

};
#endif

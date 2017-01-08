/* created by: fanghui  on Wednesday 3 November 2010  */
/* program is located on Aksaas server, which customizes,
 * obfuscates jar file and turns it into cod file.
 *  1. customize by a software secret key and blackberry PIN
 *  2. obfuscate wrapper program
 *  3. convert to blackberry cod file
 */

/* step 1. dec&cust; 2.jar;  3. obfuscate; 4. preverify; 5. cod     
 * Examples: step 2 or 5    - create jar, cod files                 
 *           step 1,2,5     - create customized cod file            
 *           step 1,2,3,4,5 - create customized, obfed cod          
 */


#include <windows.h>
//#include <stdio.h>
//#include <stdlib.h>

#include "crypt.h"
#include "md5.h"
#include "pfile.h"
#include "jobfuscator.h"


JavaObfuscator::JavaObfuscator()
{
	
  software_secret_key = 10000;
  setEnv();

  m_outCodPath = BB_CODEPATH;
  m_outCodName = BB_CODENAME;
  m_PINid      = BB_PIN_MD5;

}


JavaObfuscator::JavaObfuscator(string outCodPath, string outCodName, string PINid)
{
  setEnv();

  cout << "\nstep 0: initiating parameters..." << endl;
  software_secret_key = 10000;
  m_outCodPath = outCodPath;
  m_outCodName = outCodName;
  m_PINid      = PINid;
  if ( PINid.length() != 32 ) {  // input is a PIN instead
	  string PIN = m_PINid;
	  MD5 md5;
	  md5.update( BB_SIGNATURE( PIN ) );
	  m_PINid = md5.toString();
	  cout  << "input is a PIN:" << PIN << "--> PINid:"  << m_PINid << endl;
  } else {
	  m_PINid = PINid;    // input is an MD5 of PIN
  }

}



JavaObfuscator::~JavaObfuscator()
{
	if (DEBUG_PRINT) {
		errFile = freopen("CON", "w", stdout); //printf("And now back to the console once again\n");
	}
}


// set Java run environment for using rapc to compile blackberry program
void JavaObfuscator::setEnv()
{
	// redirect stdout to a file
	if (DEBUG_PRINT) {
		string tmp2 = DEBUG_ERR_FILE;
		if((errFile = freopen(tmp2.c_str(), "w", stdout)) == NULL)
			cout << "error: redirect stdout to file " << tmp2 << endl;
		else
			printf("this is from stdout output:\n");
	}

    // set JAVA_HOME, CLASSPATH and modify PATH environment variables
	string cmdLine = " IF EXIST \"%java_home%\" GOTO LABEL  \n\
                          set java_home=\"C:\\Program Files\\Java\\jdk1.6.0_22\"  \n\
                          set classpath=.;%java_home%\\lib\\dt.jar;%java_home%\\lib\\tools.jar \n\
                          set path=%path%;%java_home%\\bin  \n\" ";
	/*
	string e1 = "blackberry_dir=" +BLACKBERRY_DIR;
	putenv(e1.c_str());
	string e2 = getenv("PATH");
	e2 =  "PATH=" + e2 + ";" + BLACKBERRY_DIR + "\\components\\bin";
	putenv(e2.c_str());
    */

	// change current directory to project dir
	/*
#define MAXPATHLEN 4096
	char curDir[MAXPATHLEN];
    _getcwd(curDir,MAXPATHLEN);
	string workDir = "D:\\smartphone\\BlackBerry\\JavaObfuscator\\src";
	_chdir(workDir.c_str());

	cout << "current dir:" << _getcwd(curDir,MAXPATHLEN) << endl;
	*/
	
}



// a function to integrate device PINid and software key
int JavaObfuscator::getWrapperID()
{
	//return software_secret_key ^ pin;  
	//return m_PINid % software_secret_key;
	int wID = md5HexToDec( m_PINid ) % software_secret_key;
	return wID;
}
	
void JavaObfuscator::createCheckCode()
{
	// TODO: move Java secure code here
}	




// Step 1: customize java wrapper
// Input:  fileName -the java file to be protected, pin - device code
// Output: fileNameNew - new java file name
bool JavaObfuscator::customizeJava()
{
	cout << "\nstep 1: customizing wrapper java files..." << endl;

   	// create a wrapper ID
   	int wid = getWrapperID(); 
	cout << "get new wrapper ID:" << wid << endl;
    
	// decrypt java files
    string changedFileName = BB_SECURE_JAVA;
	if ( !file_exists(changedFileName) ) {
		cout <<"ERROR: file not exist!" << changedFileName << endl;
		return false;
	}

	decJavaFile( changedFileName );
	cout << "decrypt java file(done by vguard driver):" << changedFileName << endl;


	string tmpFileName;
	get_filename(changedFileName, false, tmpFileName);
	tmpFileName = BB_CODEPATH + tmpFileName + ".aaa";
	cout << "write into new file..." << tmpFileName << endl;

	ifstream inFile(  changedFileName.c_str() );
	ofstream outFile( tmpFileName.c_str() ); // , ios::binary );
	if ( !inFile.is_open() ) {
		cout << "ERROR: cannot open inFile!" << endl;
		return false;
	}
	if ( !outFile.is_open() ) {
		cout << "ERROR: cannot open outFile!" << endl;
		return false;
	}

	bool found = false;
	while (1) {
	    string oldLine;
		getline(inFile,oldLine);
		//cout << oldLine << endl;

		if ( inFile.eof() ) break;

		// replace
		char temp[100];
		_itoa(wid,temp,10);
        string s1 = "my_wrapper_id =";
        string s2 = "my_wrapper_id = " + string(temp) + ";";

        //string newLine = oldLine.replace(s1, s2);
		string newLine = oldLine;
		if ( oldLine.find(s1) != string::npos ){     // found match
			found = true;
			newLine = s2;
			cout << "   replaced by: " << newLine << endl;
		}


        outFile << newLine << endl;
        //cout << newLine << endl;
    }


	inFile.close(); 
	outFile.close();
	if ( !found ) {
		cout << "ERROR: did not find my_wrapper_id in file " << changedFileName << endl;
		return false;
	}


    // note: should keep java file name and class name consistent!
	string bakFileName;
	get_filename(changedFileName, false, bakFileName);
	bakFileName += ".orig";
    bakFileName = BB_CODEPATH + bakFileName;


	cout << "back up original java file..." << endl;
	string cmdLine3 = "move \""+ changedFileName    + "\" \"" + bakFileName +"\""; 
	cout << cmdLine3 << endl;
	copy_file( changedFileName.c_str(), bakFileName.c_str() );
	//system(cmdLine3.c_str());

	cout << "overwrite original java files..." << endl;
	delete_file( changedFileName.c_str() );
	string cmdLine4 = "move \""+ tmpFileName + "\" \"" + changedFileName +"\""; 
	cout << cmdLine4 << endl;
	if ( !move_file( tmpFileName.c_str(), changedFileName.c_str() ) ){
		cout << "error code " << GetLastError() << ": move failed!" << endl;
		return false;
	}
	//system(cmdLine4.c_str());

	return true;
}
	

// step 2: compiling Blackberry jar file
// Input:   files Secure_PDF.rapc containing configuration, filelist.txt (java files)
// Output:  file Secure_PDF.rapc.jar 
bool JavaObfuscator::compileJarFile()
{
	cout << "\nstep 2: compiling Blackberry class files(jar)..." << endl;

    string bbSrcFiles[] = BB_SOURCE_FILE_LIST2; 
	string bbSrcFileString = " ";
	for (int i = 0; i< BB_SOURCE_FILE_NUM; i++) {
		string file1 = bbSrcFiles[i];
		// check
		if ( !file_exists( file1 ) ) {
			cout <<"Warning: file not exist!" << file1 << endl;
		    //return false;
		} else {
			bbSrcFileString += "\"" + file1 + "\" ";
		}
	}
 	string cmdLine = BB_RAPC_EXE  + " -quiet codename=" + BB_CODENAME           
                                  + " \"" + string(BB_PROJECT_FILE) + "\""       
							  /*  +	" -sourceroot=\"" + BB_RAPC_SOURCE_ROOT +"\""    */
								  +	" -import=\""+ BB_LIB_JAR +"\""
								  + bbSrcFileString;  
	cout << cmdLine << endl;
	cout << endl << endl;

	system(cmdLine.c_str());

//  obsolete: another way is to read java file list in a file 
/* 	
    string cmdLine = BB_RAPC_EXE  + " -quiet codename="+ BB_CODENAME 
	                              + " \"" + string(BB_PROJECT_FILE) + "\""
								  + " -import=\""+ BB_LIB_JAR +"\""
								  + " @" + BB_SOURCE_FILE_LIST;  
	cout << cmdLine << endl;
	system(cmdLine.c_str());
*/

	string outJarFile = BB_CODENAME + ".jar";
	cout << "output:" << outJarFile << endl;
	if ( !file_exists( outJarFile ) ) {
		cout <<"ERROR: file not exist!" << outJarFile << endl;
		return false;
	}

	return true;
}






// step 3: obfuscate using an external tool
//  *.jar -> *_obf.jar
// cannot let two jar filenames same, because proguard firstly writes to outJar, and then
// copy resources from inJar to outJar.
bool JavaObfuscator::callExternalObfuscator()
{
	//return;
	cout << "\nstep 3: obfuscating class files (jar)..." << endl;

    // command: java -jar "%PROGUARD_HOME%"\lib\proguard.jar %1 %2 %3 %4 %5 %6 %7 %8 %9

/*
    string bbSrcFiles[] = BB_SOURCE_FILE_LIST2; 
	string bbSrcFileString = " ";
	for (int i = 0; i< BB_SOURCE_FILE_NUM; i++) {
		string file1 = bbSrcFiles[i];
		// check
		if ( !file_exists( file1 ) ) {
			cout <<"ERROR: file not exist!" << file1 << endl;
		    //return false;
		}else {
			bbSrcFileString += " -injars \"" + file1 + "\" ";
		}
	}
	bbSrcFileString += PROGUARD_OPTION3;
*/


    string cmdLine1 = "java -jar " +PROGUARD_JAR_EXE + " " + PROGUARD_OPTION2;
	cout << cmdLine1 << endl;
	system(cmdLine1.c_str());

	if ( !file_exists( BB_SOURCE_FILE_OBF_JAR ) ) {
		cout <<"ERROR: obfuscation failed, file not exist!" << BB_SOURCE_FILE_OBF_JAR <<  endl;
		return false;
	}


	cout << "obfuscated jar created, going to replace original jar file..." << endl;	
    string cmdLine2 = "move " + BB_SOURCE_FILE_OBF_JAR + " " + BB_SOURCE_FILE_JAR;
	cout << cmdLine2 << endl;
	system(cmdLine2.c_str());

	return true;
}



// step 4: preverify must be done after external obfuscating and before turning jar into cod file
// preverify for BB JDE
bool JavaObfuscator::preverifyJarFile()
{
	cout << "\nstep 4: preverifying Blackberry jar file..." << endl;

	// change current directory to project dir (writtable), because preverify tool needs to create tmp dir and files
#define MAXPATHLEN 4096
	char curDir[MAXPATHLEN];
    _getcwd(curDir,MAXPATHLEN);
	string workDir = BB_CODEPATH;
	_chdir(workDir.c_str());
	cout<< "change to dir:" << workDir << endl;

	// preverify parameters
	string PREVERIFY_OUTPUT_DIR = BB_OUTPUT_DIR;          // no dash at path end!
	string PREVERIFY_OUTPUT_JAR = BB_SOURCE_FILE_JAR;     // output to the same jar file

	string cmdLine = BB_PREVERIFY_EXE + " -verbose"
		                              + " -d " + PREVERIFY_OUTPUT_DIR 
									  + " -classpath \"" + BB_LIB_JAR + "\" "
									  + BB_SOURCE_FILE_JAR;
	cout << cmdLine << endl;	
	system(cmdLine.c_str());
	if ( !file_exists( PREVERIFY_OUTPUT_JAR ) ) {
		cout <<"ERROR: preverification failed!" << PREVERIFY_OUTPUT_JAR <<  endl;
		return false;
	}


	// since I set output directory same as input jar for preverify, there is no need to move files then.
/*
	string cmdLine2 = "move " + PREVERIFY_OUTPUT_JAR + " " + BB_SOURCE_FILE_JAR;
	cout << "warning: going to replace jar file by preverified one..." << endl;
	cout << cmdLine2 << endl;
	system(cmdLine2.c_str());
*/


    // recover previous directory
	_chdir(curDir);
	cout << "return to previous dir:" << curDir << endl;

	cout << "preverify output:" << PREVERIFY_OUTPUT_JAR << endl;
	return true;
}




// step 5: generating cod file
// Input:  SecurePDF.rapc -config file, SecurePDF.rapc.jar  -wrapper
// Output: file SecurePDF.rapc.cod
bool JavaObfuscator::compileCODFile()
{
	cout << "\nstep 5: converting jar to cod file..." << endl;

	string bbOutCodName = BB_CODEPATH + m_outCodName;    // with path, tmp
 	string cmdLine = BB_RAPC_EXE  + " -quiet codename="+ bbOutCodName
		                          + " \"" + string(BB_PROJECT_FILE)+ "\""
								  + " -import=\""+ BB_LIB_JAR +"\" " 
								  + BB_SOURCE_FILE_JAR;  
	cout << cmdLine << endl;
	system(cmdLine.c_str());

	string bbOutCodFile = bbOutCodName + ".cod"; 
	if ( !file_exists( bbOutCodFile ) ) {
		cout <<"ERROR: Blackberry rapc failed compiling cod file!" << bbOutCodFile << endl;
		return false;
	}

	cout << "output:" << bbOutCodFile << endl;
	return true;
}



// step 6: sign the blackberry java program
// Input: need *.cod, *.csl, *.cso three files
//        need private key files in signaturetool directory
// Output: new cod file
bool JavaObfuscator::signCODFile()
{
    // Before code signing, three files sigtool.csk, sigtool.db, sigtool.set
	// should already be in directory: 
	// C:\Eclipse\plugins\net.rim.ejde.componentpack6.0.0_6.0.0.30\components\bin
	// where SignatureTool.jar exists.

	// command:
	//       java -jar "c:\Eclipse\plugins\net.rim...\components\bin\SignatureTool.jar -a -c -p caslab19s "D:\...\wrapper.cod"
	
	cout << "\nstep 6: signing blackberry cod file..." << endl;

	string bbOutCodName = BB_CODEPATH + m_outCodName;    
    string bbOutCodFile = bbOutCodName + ".cod";

    string cmdLine1 = "java -jar " + BB_SIGN_TOOL + " -a -c -p caslab19s \"" + bbOutCodFile + "\"";
	cout << cmdLine1 << endl;
	system(cmdLine1.c_str());


	return true;
}




void JavaObfuscator::cleanTmpFiles()
{
	cout << "\nstep 7: output result and clean up tmp files..." << endl;

	// move two files (cod and jad) to specified directory
 	string FILE_COD      =  BB_CODEPATH  + m_outCodName+ ".cod";
	string FILE_JAD      =  BB_CODEPATH  + m_outCodName+ ".jad";
	string FILE_COD_DEST =  m_outCodPath   + m_outCodName+ ".cod";
	string FILE_JAD_DEST =  m_outCodPath   + m_outCodName+ ".jad";

#define BUFFER_SIZE 4096
	char fullPathName[BUFFER_SIZE] = {0};
	GetFullPathName(FILE_COD_DEST.c_str(),BUFFER_SIZE,fullPathName,0);
	FILE_COD_DEST = fullPathName;

	char fullPathName2[BUFFER_SIZE] = {0};
	GetFullPathName(FILE_JAD_DEST.c_str(),BUFFER_SIZE,fullPathName2,0);
	FILE_JAD_DEST = fullPathName2;

	cout << "destination directory:" << FILE_COD_DEST << endl;
	string cmdLine1 = "move " + FILE_COD +" " + FILE_COD_DEST;
	string cmdLine2 = "move " + FILE_JAD +" " + FILE_JAD_DEST;
	cout << cmdLine1 << endl;
	system(cmdLine1.c_str());
	cout << cmdLine2 << endl;
	system(cmdLine2.c_str());

	if ( !file_exists( FILE_COD_DEST ) ) {
		cout <<"ERROR: failed moving cod file!" << endl;
		cout <<"ERROR: file not exist!" << FILE_COD_DEST << endl;
		return;
	}

	// clean up *.cso, *.csl, *.debug, *.map,  *.jad, *.jar, *.cod, *.orig
	cout << "delete tmp files: *.cso, *.csl, *.debug, *.map, *.jad, *.jar, *.cod, *.orig." << endl;
	/*
    string cmdLine3 = "del "+ BB_CODEPATH + "*.*"; 
	cout << "warning: " << cmdLine3 << endl;
	system(cmdLine3.c_str());
	*/
	string bbCleanUpFiles[] = BB_CLEANUP_FILE_LIST; 
	for (int i = 0; i< BB_CLEANUP_FILE_NUM; i++) {
		string file1 = bbCleanUpFiles[i];
		file1 = BB_CODEPATH + file1;
		// check
		if ( file_exists( file1 ) ) {
			delete_file( file1.c_str() );
			//cout << "delete file:" << file1 << endl;
		} 
	}
	

}








// 
void JavaObfuscator::obfuscateJava()
{
}




// Interface 
// Prerequisite: Java
// Input:  cod file path, cod file name w/o extension, and PINid string (len=32)
//         example:  jobfuscator.exe ./nonce/  wrapper  0123456789ABCDEF0123456789ABCDEF
// Output: two files cod and jad
// Return: erro code
int main(int argc, char * const argv[])
{
   if(argc != 4) {
      cout << "*************************************************************************\n";
      cout << "* Customize, obfuscate jar file and turn it into cod file               *\n";
      cout << "* Usage: jobfuscate codFilePath codFileName(w/o ext) PINid              *\n";
	  cout << "* Example: jobfuscate ./nonce/ wrapper 0123456789ABCDEF0123456789ABCDEF *\n";
      cout << "*************************************************************************\n";
      return 0;
   }

   //int pin = atoi(argv[3]);

   JavaObfuscator obf(argv[1], argv[2], argv[3]);

   if ( !obf.customizeJava() ) return 0;           // Step 1

   obf.obfuscateJava();           // (null)

   if ( !obf.compileJarFile() ) return 0;          // Step 2

   if ( !obf.callExternalObfuscator() ) return 0;  // Step 3
   if ( !obf.preverifyJarFile() ) return 0;        // Step 4

   if ( !obf.compileCODFile() ) return 0;          // Step 5

   if ( !obf.signCODFile() ) return 0;             // Step 6

   obf.cleanTmpFiles();           // Step 7

   return 1;
}


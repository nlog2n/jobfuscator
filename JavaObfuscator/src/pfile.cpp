/* created by: fanghui  on Thursday 18 November 2010  */


#include <windows.h>

#include "pfile.h"


int file_exists( const string fileName )
{
	return ( _access( fileName.c_str(), 0 ) != -1 );
}



// file name without dir path
void get_filename( const string fileName, bool hasExt, string &fileName2)
{
	int start = (int) fileName.find_last_of('\\');
	if (start ==string::npos) start = (int) fileName.find_last_of('/');
	if (start ==string::npos) start = 0;

	int n = fileName.length();
	if (!hasExt) {  // without extension
		int end = (int) fileName.find_last_of('.');
		if ( end != string::npos )
			n = end -1- start;
	}

    fileName2 = fileName.substr(start+1,n);
}


// file path  (without last "/")
void get_filepath( const string fileName, string &filePath)
{
	int start = 0;
	int end = (int) fileName.find_last_of('\\');
	if (end ==string::npos) end = (int) fileName.find_last_of('/');
	if (end ==string::npos) end = 0;
    int n = end - start;

    filePath = fileName.substr(start,n);
}


bool copy_file(const char *inFileName,const char *outFileName)
{
	return CopyFile(inFileName, outFileName, false) == 0; // overwrite if exists
}

// Return: True = success
bool move_file(const char *inFileName,const char *outFileName)
{
	return MoveFile(inFileName, outFileName) != 0 ;
}

bool delete_file(const char *inFileName)
{
	return DeleteFile(inFileName) != 0;
}


bool make_dir(char *dirName)
{
	/*
  BOOL CreateDirectory(
  LPCTSTR lpPathName, 
  LPSECURITY_ATTRIBUTES lpSecurityAttributes 
); 
*/
	return _mkdir(dirName)==0;
}

bool rm_dir(char *dirName)
{ 
	return _rmdir(dirName)==0;
}
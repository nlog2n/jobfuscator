/* created by: fanghui  on Thursday 18 November 2010  */

#ifndef __PFILE_H__
#define __PFILE_H__

#include <io.h>
#include <stdio.h>
#include <stdlib.h>
#include <direct.h>
#include <string>
using namespace std;

//#include <iostream>
//#include <fstream>



int  file_exists( const string fileName );
void get_filename( const string fileName, bool hasExt, string &fileName2);
void get_filepath( const string fileName, string &filePath);

bool copy_file(const char *inFileName,const char *outFileName);
bool move_file(const char *inFileName,const char *outFileName);
bool delete_file(const char *inFileName);
bool make_dir(char *dirName);
bool rm_dir(char *dirName);


#endif

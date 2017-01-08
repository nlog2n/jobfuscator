


#include "process.h"




LIST_ENTRY g_ProcessListHead ;
KSPIN_LOCK g_ProcessListLock ;



BOOLEAN FoundInFileStrings( PUNICODE_STRING pTestedFile )
{
    UNICODE_STRING templateFile;
    WCHAR *pfiles[] = PROTECTED_FILES;
	int i;
	for( i = 0; i < PROTECTED_FILES_NUM; i++ ) {
		RtlInitUnicodeString( &templateFile, pfiles[i] );
        if ( RtlCompareUnicodeString(pTestedFile, &templateFile, TRUE) == 0 ){ // match
			return TRUE;
        }
	}

	return FALSE;
}



BOOLEAN FoundInProcessStrings( PUNICODE_STRING pTestedProc )
{
    UNICODE_STRING templateProc;
    WCHAR *pprocs[] = AUTHORIZED_EXES;
	int i;
	for( i = 0; i < AUTHORIZED_EXES_NUM; i++ ) {
		RtlInitUnicodeString( &templateProc, pprocs[i] );
	    //    if (_stricmp( aaaa, "notepad.exe")==0) {	
        if ( RtlCompareUnicodeString(pTestedProc, &templateProc, TRUE) == 0 ){ // match
			return TRUE;
        }
	}

	return FALSE;
}






BOOLEAN GetCurrentFileName( __in  PFLT_CALLBACK_DATA Data,
                                __in  PCFLT_RELATED_OBJECTS FltObjects,
                                __inout PUNICODE_STRING pFileName )
{
		PVOLUME_CONTEXT volCtx = NULL;
		PFLT_FILE_NAME_INFORMATION pfni = NULL;
		NTSTATUS status;

		// get file full name
		status = FltGetFileNameInformation( Data, 
								   FLT_FILE_NAME_NORMALIZED | FLT_FILE_NAME_QUERY_DEFAULT,
								   &pfni);
		if (!NT_SUCCESS(status))  {
			LOG_PRINT( LOGFL_ERRORS,
					   ("Vguard!GetCurrentFileName:				Error getting file name, status=%x\n",
						status) );
			// status = c01c0005 STATUS_FLT_INVALID_NAME_REQUEST
			return FALSE;
		}
	
		// succeed to get UNICODE_STRING pfni -> Name, Volume, and Share
		// further to parse other info such as pfni-> Extension, Stream, FinalComponent, ParentDir
		status = FltParseFileNameInformation( pfni );
		if (!NT_SUCCESS(status))  {
			LOG_PRINT( LOGFL_ERRORS,
					   ("Vguard!GetCurrentFileName:				Error parsing file name, status=%x\n",
						status) );
			return FALSE;
		}
	
		// pfni ->
		//   Name:  "\Device\HarddiskVolume2\smartphone\BlackBerry\JavaObfuscator\vguard\weird_bird.java"
		//   Volume: "\Device\HarddiskVolume2"
		//   Share: (null)
		//   Extension: "java"
		//   Stream: (null) 
		//   FinalComponent: "weird_bird.java"
		//   ParentDir: "\smartphone\BlackBerry\JavaObfuscator\vguard\"
	
	
		//	Get our volume context so we can display our volume name in the debug output.
		// volCtx->Name :  "D:"
		status = FltGetVolumeContext( FltObjects->Filter, FltObjects->Volume, &volCtx );
		if (!NT_SUCCESS(status)) {
			LOG_PRINT( LOGFL_ERRORS,
				   ("Vguard!GetCurrentFileName:				Error getting volume context, status=%x\n",
					status) );
			return FALSE;
		}
	
		//UNICODE_STRING *pFileName = volCtx->Name + pfni->ParentDir + pfni->FinalComponent;
		RtlCopyUnicodeString( pFileName, &(volCtx->Name) );
		RtlAppendUnicodeStringToString( pFileName, &(pfni->ParentDir) );
		RtlAppendUnicodeStringToString( pFileName, &(pfni->FinalComponent) );	
	
		// free 		
		if (volCtx != NULL) {
			FltReleaseContext( volCtx );
		}
		if (pfni != NULL) {
			FltReleaseFileNameInformation( pfni );
		}
		
		return TRUE;
}
	
	


BOOLEAN GetCurrentFileExt( __in  PFLT_CALLBACK_DATA Data,
                                __in  PCFLT_RELATED_OBJECTS FltObjects,
                                __inout PUNICODE_STRING pFileName )
{
		PFLT_FILE_NAME_INFORMATION pfni = NULL;
		NTSTATUS status;

		// get file full name
		status = FltGetFileNameInformation( Data, 
								   FLT_FILE_NAME_NORMALIZED | FLT_FILE_NAME_QUERY_DEFAULT,
								   &pfni);
		if (!NT_SUCCESS(status))  {
			return FALSE;
		}
	
		// succeed to get UNICODE_STRING pfni -> Name, Volume, and Share
		// further to parse other info such as pfni-> Extension, Stream, FinalComponent, ParentDir
		status = FltParseFileNameInformation( pfni );
		if (!NT_SUCCESS(status))  {
			return FALSE;
		}
	
		//   Extension: "java"
		RtlCopyUnicodeString( pFileName, &(pfni->Extension) );
	
		// free 		
		if (pfni != NULL) {
			FltReleaseFileNameInformation( pfni );
		}
		
		return TRUE;
}
	
	





// Return:   0  file not matched, 1 matched 
BOOLEAN IsCurrentFileMonitored( __in PFLT_CALLBACK_DATA Data,
                       __in PCFLT_RELATED_OBJECTS FltObjects )
{
    BOOLEAN retVal = FALSE;
    UNICODE_STRING  tested_file;
    WCHAR tested_file_buffer[VGUARD_MAX_NANE_LENGTH];
    tested_file.Buffer = tested_file_buffer; 
	tested_file.MaximumLength = VGUARD_MAX_NANE_LENGTH* sizeof(WCHAR);
	tested_file.Length = 0;

	retVal = GetCurrentFileName( Data, FltObjects, &tested_file);
	if ( !retVal ) {  // error getting file name
		return FALSE;
	}

	LOG_PRINT( LOGFL_VGUARD,
			   ("Vguard!IsCurrentFileMonitored:				file %wZ\n",
				&tested_file ));

    // compare
    if ( FoundInFileStrings( &tested_file ) ) { // match
		LOG_PRINT( LOGFL_VGUARD,
				   ("Vguard!IsCurrentFileMonitored:				%wZ matched <-----------\n",
					&tested_file ));
		retVal = TRUE;
    }
    
			
    return retVal;
}



BOOLEAN IsCurrentFileExtMonitored( __in PFLT_CALLBACK_DATA Data,
                       __in PCFLT_RELATED_OBJECTS FltObjects )
{
    BOOLEAN retVal = FALSE;

    UNICODE_STRING  template_file, tested_file;
    WCHAR tested_file_buffer[VGUARD_MAX_NANE_LENGTH];
    tested_file.Buffer = tested_file_buffer; 
	tested_file.MaximumLength = VGUARD_MAX_NANE_LENGTH* sizeof(WCHAR);
	tested_file.Length = 0;

	retVal = GetCurrentFileExt( Data, FltObjects, &tested_file);
	if ( !retVal ) {  // error getting file name
		return FALSE;
	}

    RtlInitUnicodeString( &template_file, PROTECTED_FILE_EXT );

	LOG_PRINT( LOGFL_VGUARD,
			   ("Vguard!IsCurrentFileMonitored:				file %wZ\n",
				&tested_file ));

    // compare
    if ( RtlCompareUnicodeString(&tested_file, &template_file, TRUE) == 0 ){ // match
		LOG_PRINT( LOGFL_VGUARD,
				   ("Vguard!IsCurrentFileMonitored:				%wZ matched <-----------\n",
					&tested_file ));
		retVal = TRUE;
    }
    
			
    return retVal;
}


//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////

int GetProcessNameByHandle( HANDLE ProcessHandle, PUNICODE_STRING ProcessImageName) 
{ 
    NTSTATUS status = STATUS_ACCESS_DENIED; 
	PUNICODE_STRING imageName = NULL; 
	ULONG returnedLength = 0; 
	ULONG bufferLength = 0; 
	PVOID buffer = NULL; 

    int ret;

	// get the size we need 
	status = NtQueryInformationProcess(ProcessHandle, ProcessImageFileName /*27*/, NULL, 0, &returnedLength); 
	if(STATUS_INFO_LENGTH_MISMATCH != status) { 
		return status; 
	} 

	//check the buffer size 
	bufferLength = returnedLength - sizeof(UNICODE_STRING); 
	if(ProcessImageName->MaximumLength < bufferLength) { 
		ProcessImageName->Length = (USHORT) bufferLength; 
		//return STATUS_BUFFER_OVERFLOW; 
		return 6;
	} 

	buffer = ExAllocatePoolWithTag(PagedPool, returnedLength, 'ipgD'); 
	if(NULL == buffer) { 
		//return STATUS_INSUFFICIENT_RESOURCES; 
		return 7;
	} 

	status = NtQueryInformationProcess(ProcessHandle, ProcessImageFileName /*27*/, buffer, returnedLength, &returnedLength); 
	if(NT_SUCCESS(status)) { 
		imageName = (PUNICODE_STRING) buffer; 
		RtlCopyUnicodeString(ProcessImageName, imageName); 

		ret = 10;
	} 
	else
		ret = 8;

	ExFreePool(buffer); 
	//return status; 
	return ret;

} 


int GetProcessNameByID( HANDLE ProcessId, PUNICODE_STRING pusImageFileName )
{
    UNICODE_STRING ProcImgName = {0};
	HANDLE hProcessHandle = NULL;
	NTSTATUS status = STATUS_ACCESS_DENIED;
	PEPROCESS eProcess = NULL;
	int iEntryIndex = -1;

	int xxx;

	// get handle for given PID
	status = PsLookupProcessByProcessId( ProcessId, &eProcess );
	if ((!NT_SUCCESS(status)) || (!eProcess)) {
		return 1;
	}

	status = ObOpenObjectByPointer( eProcess, 0, NULL, 0,0,KernelMode, &hProcessHandle);
	if ((!NT_SUCCESS(status)) || (!hProcessHandle)) {
		ObDereferenceObject(eProcess);
		eProcess = NULL;
		return 2;
	}

    //Find out name of process
    ProcImgName.Length = 0; 
	ProcImgName.MaximumLength = 1024; 
	ProcImgName.Buffer = ExAllocatePoolWithTag(NonPagedPool, ProcImgName.MaximumLength, '2leN'); 
	if(ProcImgName.Buffer == NULL) { 
		ZwClose(hProcessHandle); 
		ObDereferenceObject(eProcess); 
		eProcess = NULL; 
		return 3; 
	} 

	RtlZeroMemory( ProcImgName.Buffer, ProcImgName.MaximumLength ) ; 
	xxx = GetProcessNameByHandle(hProcessHandle, &ProcImgName); 
	if( xxx != 10 ) { 
		ExFreePoolWithTag(ProcImgName.Buffer, '2leN'); 	
		ZwClose(hProcessHandle); 
		ObDereferenceObject(eProcess); 
		eProcess = NULL; 
		return xxx; 
	} 

	if(pusImageFileName) { 
		RtlCopyUnicodeString(pusImageFileName, &ProcImgName); 
	} 

	ExFreePoolWithTag(ProcImgName.Buffer, '2leN'); 
	ZwClose(hProcessHandle); 
	ObDereferenceObject(eProcess); 
	eProcess = NULL; 
	
	return 10; 

} 

	

// Func: In an effort to remain version-independent, rather than using a hard-coded into the 
//          KPEB (Kernel Process Environment Block), we scan the KPEB looking for the name, 
//          which should match that of the system process. This is because we are in the system 
//          process' context in DriverEntry, where this is called.
// Called by: DriverEntry()
ULONG GetProcessNameOffset()
{
    PEPROCESS       curproc;
    int             i;

//	return 0x174; // hard-coded for WinXP 32 SP2

    // Scan for 12KB, hoping the KPEB never grows that big!
    curproc = PsGetCurrentProcess();  // can only be called in DriverEntry to get "System"
    for( i = 0; i < 3*PAGE_SIZE; i++ ) {
        if( !strncmp( "System", (PCHAR) curproc + i, strlen("System") )) {
            return i;
        }
    }

    return 0;  // Name not found - oh, well
}




// this is for user mode, include "windows.h"
BOOLEAN ConvertUcharToUnicodeString_user( PUCHAR pchar, PUNICODE_STRING pun )
{
#ifdef _UNICODE
    ULONG nwLen;
    ULONG nLen = strlen(pchar);
	nwLen = MultiByteToWideChar (CP_ACP, 0, pchar, -1, NULL, 0); // get needed length for wchar
    if ( pun.MaximumLength < nwLen )
		return FALSE;

	MultiByteToWideChar (CP_ACP, 0, pchar, -1, pun.Buffer, nwLen);  // convert and copy to Buffer
	pun.Length = nwLen;
	return TRUE;
#else
    return FALSE;
#endif
	
}


// for kernel mode
BOOLEAN ConvertUcharToUnicodeString( PUCHAR pchar, PUNICODE_STRING pun)
{
    NTSTATUS status;

    ANSI_STRING ansiStr;
	ansiStr.Length = (USHORT) strlen(pchar);
	ansiStr.MaximumLength = ansiStr.Length;
	ansiStr.Buffer = pchar;
	
    status = RtlAnsiStringToUnicodeString( pun, &ansiStr, TRUE );
	if (!NT_SUCCESS(status)) {
		return FALSE;
	}

	//RtlFreeUnicodeString( pun );  ///////////// remember to free buffer!
	return TRUE;
}






// Uses undocumented data structure offsets to obtain the name of the currently executing process.
// should be called in pre/post op to get hooked exe file name
BOOLEAN GetCurrentProcessNameByOffset( PUNICODE_STRING punProcessName )
{
#define	NT_PROCNAMELEN  16

    PEPROCESS       curproc;
    char            *nameptr;

    // We only do this if we determined the process name offset
    if( ProcessNameOffset ) {
        curproc = PsGetCurrentProcess(); // Get a pointer to the current process block

        // Dig into it to extract the name. Make sure to leave enough room
        // in the buffer for the appended process ID.
        nameptr   = (PCHAR) curproc + ProcessNameOffset;
		//		  strncpy( ProcessName, nameptr, NT_PROCNAMELEN-1 );
		//		  ProcessName[NT_PROCNAMELEN-1] = 0;

		return ConvertUcharToUnicodeString( nameptr, punProcessName );
    } 

    return FALSE;
}


BOOLEAN GetCurrentProcessName( PUNICODE_STRING punProcessName )
{
/*
	HANDLE ProcessId;
       ProcessId = PsGetCurrentProcessId();     // get process ID
	if ( ProcessId == NULL ) return 0;
	return GetProcessNameByID( ProcessId, punProcessName);
*/	

	return GetCurrentProcessNameByOffset( punProcessName );
}



// Return:   0  file not matched, 1 matched
BOOLEAN IsCurrentProcessMonitored( __in PFLT_CALLBACK_DATA Data,
                          __in PCFLT_RELATED_OBJECTS FltObjects )
{
	// Get a pointer to the current process block
    //PEPROCESS curproc = PsGetCurrentProcess(); 
    //PEPROCESS curproc = FltGetRequestorProcess( Data );  // similar to PsGetCurrentProcess()

    BOOLEAN retVal = FALSE;
    UNICODE_STRING  procName;
	
	int status = GetCurrentProcessName( &procName );
	if ( !status ) { // failed
		LOG_PRINT( LOGFL_VGUARD,
				   ("Vguard!IsCurrentProcessMonitored:				did not get process name, status=%x\n",
					status) );
		return FALSE;
	}

    // show
	LOG_PRINT( LOGFL_VGUARD,
			   ("Vguard!IsCurrentProcessMonitored:				process %wZ\n",
				&procName ));


    // compare
    if ( FoundInProcessStrings( &procName ) )	{ // match
		LOG_PRINT( LOGFL_VGUARD,
				   ("Vguard!IsCurrentProcessMonitored:				process %wZ hooked<-----------\n",
					&procName ));
		retVal = TRUE;
    }

    // free the mem allocated by GetCurrentProcessName
	RtlFreeUnicodeString( &procName );
	
	return retVal;
}




// file filtering logic
BOOLEAN IsCurrentFileProcessMonitored(  __in PFLT_CALLBACK_DATA Data,
                          __in PCFLT_RELATED_OBJECTS FltObjects )
{
    BOOLEAN fileHooked, procHooked;
    fileHooked = IsCurrentFileMonitored( Data, FltObjects );
    procHooked = IsCurrentProcessMonitored( Data, FltObjects );

//    if (TRUE) {
    if ( procHooked  ) {
    	if ( fileHooked  ) {
	    	return TRUE;  // do encryption or decryption, and swap buffers
    	} else {
	    	return FALSE; // pass thru
    	}
    } else {
    	return FALSE;  // pass thru
    }
}


// Return:   0  file not matched, 1 matched 
BOOLEAN MonitorCurrentFileAccess( __in PFLT_CALLBACK_DATA Data,
                       __in PCFLT_RELATED_OBJECTS FltObjects,
                       int OpFlag )
{
    BOOLEAN retVal = TRUE;

    UNICODE_STRING  procName, tested_file;
    WCHAR tested_file_buffer[VGUARD_MAX_NANE_LENGTH];
    tested_file.Buffer = tested_file_buffer; 
	tested_file.MaximumLength = VGUARD_MAX_NANE_LENGTH* sizeof(WCHAR);
	tested_file.Length = 0;

	retVal = GetCurrentFileName( Data, FltObjects, &tested_file);
	if ( !retVal ) {  // error getting file name
		return FALSE;
	}

	retVal = GetCurrentProcessName( &procName );
	if ( !retVal ) { // failed
  	    RtlFreeUnicodeString( &procName );	    
		return FALSE;
	}

    // show current file access
    if ( OpFlag == LOGFL_READ )
    	LOG_PRINT( LOGFL_VGUARD,
			   ("Vguard!MonitorCurrentFileAccess:				%wZ read %wZ\n",
			    &procName,
				&tested_file ));
	else if ( OpFlag == LOGFL_WRITE )
		LOG_PRINT( LOGFL_VGUARD,
				   ("Vguard!MonitorCurrentFileAccess:				%wZ write %wZ\n",
					&procName,
					&tested_file ));
		

    // compare file name	
    if ( !FoundInFileStrings( &tested_file ) ) { // not match
        retVal = FALSE;
    }

    // compare proc name
    if ( !FoundInProcessStrings( &procName ) )	{ // not match
        retVal = FALSE;
    }

    // show
    if ( retVal ) {
		LOG_PRINT( LOGFL_VGUARD,
			   ("Vguard!MonitorCurrentFileAccess:<------------------------------------------hooked by %wZ\n",
			    &procName ));
    }


    // free the mem allocated by GetCurrentProcessName
	RtlFreeUnicodeString( &procName );

    return retVal;
}





/////////////////////////////////////////////////////////////////////////////////
//
//   maintain a process list in monitoring
//
/////////////////////////////////////////////////////////////////////////////////


/**
 * result of add process info 
 */
#define MGAPI_RESULT_SUCCESS        0x00000000
#define MGAPI_RESULT_ALREADY_EXIST  0x00000001
#define MGAPI_RESULT_INTERNEL_ERROR 0x00000002


/**
 * result of delete process info
 */
#define MGDPI_RESULT_SUCCESS MGAPI_RESULT_SUCCESS        
#define MGDPI_RESULT_NOT_EXIST MGAPI_RESULT_ALREADY_EXIST   
#define MGDPI_RESULT_INTERNEL_ERROR MGAPI_RESULT_INTERNEL_ERROR 




static BOOLEAN PsSearchProcessInList(PUCHAR pszProcessName, BOOLEAN bRemove)
{
	BOOLEAN bRet = TRUE ;
	KIRQL oldIrql ;
	PLIST_ENTRY TmpListEntryPtr = NULL ;
	PiPROCESS_INFO psProcessInfo = NULL ;

	try{

		TmpListEntryPtr = g_ProcessListHead.Flink ;
		while(&g_ProcessListHead != TmpListEntryPtr)
		{
			psProcessInfo = CONTAINING_RECORD(TmpListEntryPtr, iPROCESS_INFO, ProcessList) ;

			if (!_strnicmp(psProcessInfo->szProcessName, pszProcessName, strlen(pszProcessName)))
			{
				bRet = TRUE;
				if (bRemove)
				{
					KeAcquireSpinLock(&g_ProcessListLock, &oldIrql) ;
					RemoveEntryList(&psProcessInfo->ProcessList) ;
					KeReleaseSpinLock(&g_ProcessListLock, oldIrql) ;
					ExFreePool(psProcessInfo) ;
					psProcessInfo = NULL ;
				}
				leave ;
			}

			TmpListEntryPtr = TmpListEntryPtr->Flink ;
		}

		bRet = FALSE ;
	}
	finally{
		//Todo some post work here
	}

	return bRet ;
}




ULONG PsAddProcessInList(PUCHAR pszProcessName, BOOLEAN bMonitor)
{

	// to replace strcpy and wcscopy
#define szStrCopy( d, s )  RtlCopyMemory( d, s, strlen(s) )
#define unStrCopy( d, s )  RtlCopyMemory( d, s, wcslen(s)* sizeof(WCHAR) )  



	ULONG uRes = MGAPI_RESULT_SUCCESS ;
	PiPROCESS_INFO psProcInfo = NULL ;
	BOOLEAN bRet ;

	try{
		if (NULL == pszProcessName)
		{
			uRes = MGAPI_RESULT_INTERNEL_ERROR ;
			leave ;
		}

		/**
		* search for process name, if exists, donnot insert again
		*/
		bRet = PsSearchProcessInList(pszProcessName, FALSE) ;
		if (bRet)
		{
			uRes = MGAPI_RESULT_ALREADY_EXIST ;
			leave ;
		}

		/**
		* allocate process info structure
		*/
		psProcInfo = ExAllocatePoolWithTag(NonPagedPool, sizeof(iPROCESS_INFO), 'ipws') ;
		if (NULL == psProcInfo)
		{
			uRes = MGAPI_RESULT_INTERNEL_ERROR ;
			leave ;
		}

		RtlZeroMemory(psProcInfo, sizeof(iPROCESS_INFO)) ;

		/**
		* initialize process info and insert it into global process list
		*/
		if (!_strnicmp(pszProcessName, "WINWORD.EXE", strlen("WINWORD.EXE")))
		{
			unStrCopy(psProcInfo->wsszRelatedExt[0],  L".html") ;
			unStrCopy(psProcInfo->wsszRelatedExt[1],  L".txt") ;
			unStrCopy(psProcInfo->wsszRelatedExt[2], L".mh_") ; //relative to .mht and .mhtml extension
			unStrCopy(psProcInfo->wsszRelatedExt[3],  L".rtf") ;
			unStrCopy(psProcInfo->wsszRelatedExt[4], L".ht_") ; //relative to .htm and .html extension
			unStrCopy(psProcInfo->wsszRelatedExt[5],  L".xml") ;
			unStrCopy(psProcInfo->wsszRelatedExt[6],  L".mht") ;
			unStrCopy(psProcInfo->wsszRelatedExt[7],  L".mhtml") ;
			unStrCopy(psProcInfo->wsszRelatedExt[8],  L".htm") ;
			unStrCopy(psProcInfo->wsszRelatedExt[9],  L".dot") ;
			unStrCopy(psProcInfo->wsszRelatedExt[10],  L".tmp") ;
			unStrCopy(psProcInfo->wsszRelatedExt[11], L".docm") ;
			unStrCopy(psProcInfo->wsszRelatedExt[12], L".docx") ;
			unStrCopy(psProcInfo->wsszRelatedExt[13],  L".doc") ;
		}
		else if (!_strnicmp(pszProcessName, "notepad.exe", strlen("notepad.exe")))
		{
			unStrCopy(psProcInfo->wsszRelatedExt[0],  L".txt") ;
		}
		else if (!_strnicmp(pszProcessName, "explorer.exe", strlen("explorer.exe")))
		{
			unStrCopy(psProcInfo->wsszRelatedExt[0], L".mp3") ;
			unStrCopy(psProcInfo->wsszRelatedExt[1],L".mp2") ;	
			unStrCopy(psProcInfo->wsszRelatedExt[2], L".xml") ;
			unStrCopy(psProcInfo->wsszRelatedExt[3], L".mht") ;
			unStrCopy(psProcInfo->wsszRelatedExt[4], L".mhtml") ;
			unStrCopy(psProcInfo->wsszRelatedExt[5], L".htm") ;
			unStrCopy(psProcInfo->wsszRelatedExt[6], L".html") ;	
			unStrCopy(psProcInfo->wsszRelatedExt[7], L".xlt") ;
			unStrCopy(psProcInfo->wsszRelatedExt[8], L".mid") ;
			unStrCopy(psProcInfo->wsszRelatedExt[9], L".rmi") ;
			unStrCopy(psProcInfo->wsszRelatedExt[10],L".midi") ;
			unStrCopy(psProcInfo->wsszRelatedExt[11],L".asf") ;
			unStrCopy(psProcInfo->wsszRelatedExt[12],L".wm") ;
			unStrCopy(psProcInfo->wsszRelatedExt[13],L".wma") ;
			unStrCopy(psProcInfo->wsszRelatedExt[14],L".wmv") ;
			unStrCopy(psProcInfo->wsszRelatedExt[15],L".avi") ;
			unStrCopy(psProcInfo->wsszRelatedExt[16],L".wav") ;
			unStrCopy(psProcInfo->wsszRelatedExt[17],L".mpg") ;
			unStrCopy(psProcInfo->wsszRelatedExt[18],L".mpeg") ;
			unStrCopy(psProcInfo->wsszRelatedExt[19], L".xls") ;
			unStrCopy(psProcInfo->wsszRelatedExt[20], L".ppt") ;
		}
		else if (!_strnicmp(pszProcessName, "System", strlen("System")))
		{
			unStrCopy(psProcInfo->wsszRelatedExt[0],  L".doc") ;
			unStrCopy(psProcInfo->wsszRelatedExt[1],  L".xls") ;
			unStrCopy(psProcInfo->wsszRelatedExt[2],  L".ppt") ;
			unStrCopy(psProcInfo->wsszRelatedExt[3],  L".txt") ;
			unStrCopy(psProcInfo->wsszRelatedExt[4], L".mp2") ;
			unStrCopy(psProcInfo->wsszRelatedExt[5],  L".rtf") ;
			unStrCopy(psProcInfo->wsszRelatedExt[6], L".mp3") ;
			unStrCopy(psProcInfo->wsszRelatedExt[7],  L".xml") ;
			unStrCopy(psProcInfo->wsszRelatedExt[8],  L".mht") ;
			unStrCopy(psProcInfo->wsszRelatedExt[9],  L".mhtml") ;
			unStrCopy(psProcInfo->wsszRelatedExt[10], L".htm") ;
			unStrCopy(psProcInfo->wsszRelatedExt[11], L".html") ;
			unStrCopy(psProcInfo->wsszRelatedExt[12], L".docx") ;
			unStrCopy(psProcInfo->wsszRelatedExt[13], L".docm") ;	
			unStrCopy(psProcInfo->wsszRelatedExt[14], L".pps") ;
			unStrCopy(psProcInfo->wsszRelatedExt[15], L".ppa") ;
			unStrCopy(psProcInfo->wsszRelatedExt[16], L".pptx") ;
			unStrCopy(psProcInfo->wsszRelatedExt[17], L".pptm") ;
			unStrCopy(psProcInfo->wsszRelatedExt[18], L".potx") ;
			unStrCopy(psProcInfo->wsszRelatedExt[19], L".potm") ;
			unStrCopy(psProcInfo->wsszRelatedExt[20], L".ppsx") ;	
			unStrCopy(psProcInfo->wsszRelatedExt[21], L".pot") ;
			unStrCopy(psProcInfo->wsszRelatedExt[22], L".ppsm") ;	
			unStrCopy(psProcInfo->wsszRelatedExt[23], L".mh_") ; //relative to .mht extension
			unStrCopy(psProcInfo->wsszRelatedExt[24], L".ht_") ; //relative to .htm and .html extension
			unStrCopy(psProcInfo->wsszRelatedExt[25], L".xlt") ;
			unStrCopy(psProcInfo->wsszRelatedExt[26], L".mid") ;
			unStrCopy(psProcInfo->wsszRelatedExt[27], L".rmi") ;
			unStrCopy(psProcInfo->wsszRelatedExt[28], L".midi") ;
			unStrCopy(psProcInfo->wsszRelatedExt[29], L".asf") ;
			unStrCopy(psProcInfo->wsszRelatedExt[30], L".wm") ;
			unStrCopy(psProcInfo->wsszRelatedExt[31], L".wma") ;
			unStrCopy(psProcInfo->wsszRelatedExt[32], L".wmv") ;
			unStrCopy(psProcInfo->wsszRelatedExt[33], L".avi") ;
			unStrCopy(psProcInfo->wsszRelatedExt[34], L".wav") ;
			unStrCopy(psProcInfo->wsszRelatedExt[35], L".mpg") ;
			unStrCopy(psProcInfo->wsszRelatedExt[36], L".mpeg");
			
			
			unStrCopy(psProcInfo->wsszRelatedExt[37],  L".tmp") ;
			unStrCopy(psProcInfo->wsszRelatedExt[38],  L".dot") ;
		}
		
		psProcInfo->bMonitor = bMonitor ;
		RtlCopyMemory(psProcInfo->szProcessName, pszProcessName, strlen(pszProcessName)) ;
		ExInterlockedInsertTailList(&g_ProcessListHead, &psProcInfo->ProcessList, &g_ProcessListLock) ;
		
	}
	finally{
	}

	return uRes ;
}


ULONG PsDelProcessInList(PUCHAR pszProcessName, BOOLEAN bMonitor) 
{
	ULONG uRes = MGAPI_RESULT_SUCCESS ;
	BOOLEAN bRet ;

	try{
		if (NULL == pszProcessName)
		{
			uRes = MGDPI_RESULT_INTERNEL_ERROR ;
			leave ;
		}

		/**
		* search for process name, if exists, donnot insert again
		*/
		bRet = PsSearchProcessInList(pszProcessName, TRUE) ;
		if (!bRet)
		{
			uRes = MGDPI_RESULT_NOT_EXIST ;
			leave ;
		}
	}
	finally{
	}

	return uRes ;
}


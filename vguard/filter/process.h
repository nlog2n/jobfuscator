
#ifndef __PROCESS_H__
#define __PROCESS_H__

#include "vkernel.h"
#include "vlog.h"


#define VGUARD_MAX_NANE_LENGTH  1024

#define PROTECTED_FILE_EXT  L"sjava"

#define PROTECTED_DIRS_NUM    2
#define PROTECTED_DIRS        {  \
 L"D:\\smartphone\\BlackBerry\\JavaObfuscator\\src\\output\\",                 \
 L"D:\\smartphone\\BlackBerry\\Secure PDF\\src\\com\\i2r\\cas\\securepdf\\"    }




#define PROTECTED_FILES_NUM   52 
#define PROTECTED_FILES       {  \
    L"D:\\smartphone\\BlackBerry\\JavaObfuscator\\vguard\\WeirdBird.java",  \
    L"D:\\smartphone\\BlackBerry\\Secure PDF\\src\\com\\i2r\\cas\\securepdf\\HelloWorld.java",  \
    L"D:\\smartphone\\BlackBerry\\Secure PDF\\src\\com\\i2r\\cas\\securepdf\\SecurePDF.java",   \
    L"D:\\smartphone\\BlackBerry\\JavaObfuscator\\src\\output\\SecurePDF.aaa", \
L"D:\\SEDMBlackberryWeb\\apps\\sedm\\JavaObfuscator\\src\\output\\SecurePDF.aaa", \
L"D:\\SEDMBlackberryWeb\\apps\\sedm\\SEDMReader\\sbox\\com\\i2r\\sedm\\SecurePDF.java", \
L"D:\\SEDMBlackberryWeb\\apps\\sedm\\SEDMReader\\sbox\\com\\i2r\\sedm\\LoginScreen.java", \
L"D:\\SEDMBlackberryWeb\\apps\\sedm\\SEDMReader\\sbox\\com\\i2r\\sedm\\RecordManager.java", \
L"D:\\SEDMBlackberryWeb\\apps\\sedm\\SEDMReader\\sbox\\com\\i2r\\sedm\\SPDFReader.java", \
L"D:\\SEDMBlackberryWeb\\apps\\sedm\\SEDMReader\\sbox\\com\\i2r\\sedm\\SPDFReader.rrc", \
L"D:\\SEDMBlackberryWeb\\apps\\sedm\\SEDMReader\\sbox\\com\\i2r\\sedm\\SPDFReader.rrh", \
L"D:\\SEDMBlackberryWeb\\apps\\sedm\\SEDMReader\\sbox\\com\\i2r\\sedm\\SPDFReaderAppPermissions.java", \
L"D:\\SEDMBlackberryWeb\\apps\\sedm\\SEDMReader\\sbox\\com\\i2r\\sedm\\SPDFReaderCore.java", \
L"D:\\SEDMBlackberryWeb\\apps\\sedm\\SEDMReader\\sbox\\com\\i2r\\sedm\\SPDFReaderDAO.java", \
L"D:\\SEDMBlackberryWeb\\apps\\sedm\\SEDMReader\\sbox\\com\\i2r\\sedm\\SPDFReaderPref.java", \
L"D:\\SEDMBlackberryWeb\\apps\\sedm\\SEDMReader\\sbox\\com\\i2r\\sedm\\SPDFReaderScreen.java", \
L"D:\\SEDMBlackberryWeb\\apps\\sedm\\SEDMReader\\sbox\\com\\i2r\\utils\\CalendarUtils.java", \
L"D:\\SEDMBlackberryWeb\\apps\\sedm\\SEDMReader\\sbox\\com\\i2r\\utils\\PropertyUtils.java", \
L"D:\\SEDMBlackberryWeb\\apps\\sedm\\SEDMReader\\sbox\\com\\i2r\\utils\\SelectorPopupScreen.java", \
L"D:\\SEDMBlackberryWeb\\apps\\sedm\\SEDMReader\\sbox\\com\\i2r\\utils\\StringUtils.java", \
L"D:\\SEDMBlackberryWeb\\apps\\sedm\\SEDMReader\\sbox\\com\\i2r\\utils\\Tools.java", \
L"D:\\SEDMBlackberryWeb\\apps\\sedm\\SEDMReader\\sbox\\com\\i2r\\utils\\conn\\AbstractConfiguration.java", \
L"D:\\SEDMBlackberryWeb\\apps\\sedm\\SEDMReader\\sbox\\com\\i2r\\utils\\conn\\BESConfig.java", \
L"D:\\SEDMBlackberryWeb\\apps\\sedm\\SEDMReader\\sbox\\com\\i2r\\utils\\conn\\BISConfig.java", \
L"D:\\SEDMBlackberryWeb\\apps\\sedm\\SEDMReader\\sbox\\com\\i2r\\utils\\conn\\ConnectionManager.java", \
L"D:\\SEDMBlackberryWeb\\apps\\sedm\\SEDMReader\\sbox\\com\\i2r\\utils\\conn\\ConnectionUtils.java", \
L"D:\\SEDMBlackberryWeb\\apps\\sedm\\SEDMReader\\sbox\\com\\i2r\\utils\\conn\\Gateway.java", \
L"D:\\SEDMBlackberryWeb\\apps\\sedm\\SEDMReader\\sbox\\com\\i2r\\utils\\conn\\ServiceBookConfig.java", \
L"D:\\SEDMBlackberryWeb\\apps\\sedm\\SEDMReader\\sbox\\com\\i2r\\utils\\conn\\TcpConfig.java", \
L"D:\\SEDMBlackberryWeb\\apps\\sedm\\SEDMReader\\sbox\\com\\i2r\\utils\\conn\\WiFiConfig.java", \
L"D:\\SEDMBlackberryWeb\\apps\\sedm\\SEDMReader\\sbox\\com\\i2r\\utils\\File\\CustomHashtable.java", \
L"D:\\SEDMBlackberryWeb\\apps\\sedm\\SEDMReader\\sbox\\com\\i2r\\utils\\File\\FileUtils.java", \
L"D:\\SEDMBlackberryWeb\\apps\\sedm\\SEDMReader\\sbox\\com\\i2r\\utils\\File\\JSR75FileSystem.java", \
L"D:\\SEDMBlackberryWeb\\apps\\sedm\\SEDMReader\\sbox\\com\\i2r\\utils\\log\\Appender.java", \
L"D:\\SEDMBlackberryWeb\\apps\\sedm\\SEDMReader\\sbox\\com\\i2r\\utils\\log\\BlackberryEventLogAppender.java", \
L"D:\\SEDMBlackberryWeb\\apps\\sedm\\SEDMReader\\sbox\\com\\i2r\\utils\\log\\ConsoleAppender.java", \
L"D:\\SEDMBlackberryWeb\\apps\\sedm\\SEDMReader\\sbox\\com\\i2r\\utils\\log\\FileAppender.java", \
L"D:\\SEDMBlackberryWeb\\apps\\sedm\\SEDMReader\\sbox\\com\\i2r\\utils\\log\\Log.java",   \
L"D:\\SEDMBlackberryWeb\\apps\\sedm\\SEDMReader\\sbox\\com\\i2r\\sedminstaller\\core\\SEDMLicense.java", \
L"D:\\SEDMBlackberryWeb\\apps\\sedm\\SEDMReader\\sbox\\com\\i2r\\sedminstaller\\core\\SEDMPacket.java", \
L"D:\\SEDMBlackberryWeb\\apps\\sedm\\SEDMReader\\sbox\\com\\i2r\\sedminstaller\\core\\SEDMProcess.java", \
L"D:\\SEDMBlackberryWeb\\apps\\sedm\\SEDMReader\\sbox\\com\\i2r\\sedminstaller\\crypto\\AESCrypto.java", \
L"D:\\SEDMBlackberryWeb\\apps\\sedm\\SEDMReader\\sbox\\com\\i2r\\sedminstaller\\crypto\\PDFCrypto.java", \
L"D:\\SEDMBlackberryWeb\\apps\\sedm\\SEDMReader\\sbox\\com\\i2r\\sedminstaller\\crypto\\PDFMD5.java", \
L"D:\\SEDMBlackberryWeb\\apps\\sedm\\SEDMReader\\sbox\\com\\i2r\\sedminstaller\\listener\\Listener.java", \
L"D:\\SEDMBlackberryWeb\\apps\\sedm\\SEDMReader\\sbox\\com\\i2r\\sedminstaller\\network\\HttpPacket.java", \
L"D:\\SEDMBlackberryWeb\\apps\\sedm\\SEDMReader\\sbox\\com\\i2r\\sedminstaller\\network\\HttpUtils.java", \
L"D:\\SEDMBlackberryWeb\\apps\\sedm\\SEDMReader\\sbox\\com\\i2r\\sedminstaller\\network\\Server.java", \
L"D:\\SEDMBlackberryWeb\\apps\\sedm\\SEDMReader\\sbox\\com\\i2r\\sedminstaller\\util\\ByteBuffer.java", \
L"D:\\SEDMBlackberryWeb\\apps\\sedm\\SEDMReader\\sbox\\com\\i2r\\sedminstaller\\util\\Convert.java", \
L"D:\\SEDMBlackberryWeb\\apps\\sedm\\SEDMReader\\sbox\\com\\i2r\\sedminstaller\\util\\FileSystem.java", \
L"D:\\SEDMBlackberryWeb\\apps\\sedm\\SEDMReader\\sbox\\com\\i2r\\sedminstaller\\util\\LoaderScreen.java" }







// for example, notepad.exe, wordpad.exe, winword.exe, Uedit32.exe, javac.exe
#define AUTHORIZED_EXES_NUM   5
#define AUTHORIZED_EXES       {   \
 L"jobfuscator.exe",       \
 L"jobf.exe",              \
 L"jobfusca",              \
 /*L"explorer.exe", */        \
 L"java.exe",              \
 /* L"javac.exe", */          \
 L"Uedit32.exe"               }








//////////////////////////////////////////////////////////////////////////////////////////////////




extern ULONG ProcessNameOffset;  // process name offset in EPROCESS

ULONG   GetProcessNameOffset();
BOOLEAN IsCurrentFileMonitored( __in PFLT_CALLBACK_DATA Data,  __in PCFLT_RELATED_OBJECTS FltObjects );
BOOLEAN IsCurrentProcessMonitored( __in PFLT_CALLBACK_DATA Data,  __in PCFLT_RELATED_OBJECTS FltObjects );
BOOLEAN MonitorCurrentFileAccess( __in PFLT_CALLBACK_DATA Data, __in PCFLT_RELATED_OBJECTS FltObjects, int OpFlag );
BOOLEAN IsCurrentFileProcessMonitored( __in PFLT_CALLBACK_DATA Data, __in PCFLT_RELATED_OBJECTS FltObjects );


// monitored user process list
typedef struct _iPROCESS_INFO{
	CHAR    szProcessName[16] ;
	BOOLEAN bMonitor ;
	WCHAR   wsszRelatedExt[64][6] ; /*< related file extension, containing maximum 10 extensions and each length is 6 characters */
	LIST_ENTRY ProcessList ;
}iPROCESS_INFO,*PiPROCESS_INFO ;

extern LIST_ENTRY g_ProcessListHead ; /*< process info list */
extern KSPIN_LOCK g_ProcessListLock ; /*< process list operation lock */





#endif __PROCESS_H__









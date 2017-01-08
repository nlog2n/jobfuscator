/* created by: fanghui  on Monday 8 November 2010  */

#ifndef __BBCONFIG_H__
#define __BBCONFIG_H__

// Blackberry simulator setting
//#define BB_DIR                  string("C:\\Progra~1\\eclipse\\plugins\\net.rim.ejde.componentpack6.0.0_6.0.0.29")
//#define BB_DIR                  string("C:\\Eclipse\\plugins\\net.rim.ejde.componentpack6.0.0_6.0.0.30")
//#define BB_DIR                  string("D:\\SEDMBlackberryWeb\\apps\\sedm\\net.rim.ejde.componentpack6.0.0_6.0.0.29")      ////////////// LAB SETTING
#define BB_DIR                  string("D:\\SEDMBlackberryWeb\\apps\\sedm\\net.rim.ejde.componentpack6.0.0_6.0.0.30") 
#define BB_RAPC_EXE             BB_DIR + "\\components\\bin\\rapc.exe"
#define BB_PREVERIFY_EXE        BB_DIR + "\\components\\bin\\preverify.exe"
#define BB_LIB_JAR              BB_DIR + "\\components\\lib\\net_rim_api.jar"
#define BB_SIGN_TOOL     	    BB_DIR + "\\components\\bin\\SignatureTool.jar"


// Project setting
#define BB_PROJECT_ROOT         string("D:\\SEDMBlackberryWeb\\apps\\sedm\\")  ////////////////////////////////////////////////////////// LAB SETTING
#define BB_OUTPUT_DIR           BB_PROJECT_ROOT + "JavaObfuscator\\src\\output"  
#define BB_CODEPATH             BB_OUTPUT_DIR + "\\"  
#define	BB_CODENAME             BB_CODEPATH + "PDFreaderWrapper"
#define BB_PIN                  553648138
#define BB_PIN_MD5              "fce4597afd2005835c9f6c617884cff3"   // md5(PIN || "SEDM BLACKBERRY" || PIN)
#define BB_SIGNATURE(pin)       pin + "SEDM BLACKBERRY" + pin
#define BB_PROJECT_FILE         BB_PROJECT_ROOT + "JavaObfuscator\\src\\SecurePDF.rapc" //copied from eclipse project dir: "deliverables\\Standard\\5.0.0\\Secure_PDF.rapc"


// Java wrapper program setting
#define BB_WRAPPER_SRC_ROOT     BB_PROJECT_ROOT + "SEDMReader\\"   ////////////////////////////// follows directory struct
#define BB_SOURCE_FILE_LIST	    "filelist.txt"   // obsolete
#define	BB_RAPC_SOURCE_ROOT     BB_WRAPPER_SRC_ROOT + "src" + ";" + BB_WRAPPER_SRC_ROOT + "res"  // obsolete
#define BB_SECURE_JAVA  		BB_WRAPPER_SRC_ROOT + "sbox\\com\\i2r\\sedm\\SecurePDF.java"
#define BB_SOURCE_FILE_NUM      48                                     ////////////////////////////////////// remember to reset NUM!!!
#define BB_SOURCE_FILE_LIST2    {   \
      BB_WRAPPER_SRC_ROOT + "sbox\\com\\i2r\\sedm\\SecurePDF.java",   \
      BB_WRAPPER_SRC_ROOT + "sbox\\com\\i2r\\sedm\\LoginScreen.java",   \
      BB_WRAPPER_SRC_ROOT + "sbox\\com\\i2r\\sedm\\RecordManager.java", \
      BB_WRAPPER_SRC_ROOT + "sbox\\com\\i2r\\sedm\\SPDFReader.java",    \
      BB_WRAPPER_SRC_ROOT + "sbox\\com\\i2r\\sedm\\SPDFReader.rrc",     \
      BB_WRAPPER_SRC_ROOT + "sbox\\com\\i2r\\sedm\\SPDFReader.rrh",     \
      BB_WRAPPER_SRC_ROOT + "sbox\\com\\i2r\\sedm\\SPDFReaderAppPermissions.java", \
      BB_WRAPPER_SRC_ROOT + "sbox\\com\\i2r\\sedm\\SPDFReaderCore.java", \
      BB_WRAPPER_SRC_ROOT + "sbox\\com\\i2r\\sedm\\SPDFReaderDAO.java",  \
      BB_WRAPPER_SRC_ROOT + "sbox\\com\\i2r\\sedm\\SPDFReaderPref.java", \
      BB_WRAPPER_SRC_ROOT + "sbox\\com\\i2r\\sedm\\SPDFReaderScreen.java",  \
      BB_WRAPPER_SRC_ROOT + "sbox\\com\\i2r\\utils\\CalendarUtils.java", \
      BB_WRAPPER_SRC_ROOT + "sbox\\com\\i2r\\utils\\PropertyUtils.java", \
      BB_WRAPPER_SRC_ROOT + "sbox\\com\\i2r\\utils\\SelectorPopupScreen.java", \
      BB_WRAPPER_SRC_ROOT + "sbox\\com\\i2r\\utils\\StringUtils.java", \
      BB_WRAPPER_SRC_ROOT + "sbox\\com\\i2r\\utils\\Tools.java", \
      BB_WRAPPER_SRC_ROOT + "sbox\\com\\i2r\\utils\\conn\\AbstractConfiguration.java", \
      BB_WRAPPER_SRC_ROOT + "sbox\\com\\i2r\\utils\\conn\\BESConfig.java", \
      BB_WRAPPER_SRC_ROOT + "sbox\\com\\i2r\\utils\\conn\\BISConfig.java", \
      BB_WRAPPER_SRC_ROOT + "sbox\\com\\i2r\\utils\\conn\\ConnectionManager.java", \
      BB_WRAPPER_SRC_ROOT + "sbox\\com\\i2r\\utils\\conn\\ConnectionUtils.java", \
      BB_WRAPPER_SRC_ROOT + "sbox\\com\\i2r\\utils\\conn\\Gateway.java", \
      BB_WRAPPER_SRC_ROOT + "sbox\\com\\i2r\\utils\\conn\\ServiceBookConfig.java", \
      BB_WRAPPER_SRC_ROOT + "sbox\\com\\i2r\\utils\\conn\\TcpConfig.java", \
      BB_WRAPPER_SRC_ROOT + "sbox\\com\\i2r\\utils\\conn\\WiFiConfig.java", \
      BB_WRAPPER_SRC_ROOT + "sbox\\com\\i2r\\utils\\File\\CustomHashtable.java", \
      BB_WRAPPER_SRC_ROOT + "sbox\\com\\i2r\\utils\\File\\FileUtils.java", \
      BB_WRAPPER_SRC_ROOT + "sbox\\com\\i2r\\utils\\File\\JSR75FileSystem.java", \
      BB_WRAPPER_SRC_ROOT + "sbox\\com\\i2r\\utils\\log\\Appender.java", \
      BB_WRAPPER_SRC_ROOT + "sbox\\com\\i2r\\utils\\log\\BlackberryEventLogAppender.java", \
      BB_WRAPPER_SRC_ROOT + "sbox\\com\\i2r\\utils\\log\\ConsoleAppender.java", \
      BB_WRAPPER_SRC_ROOT + "sbox\\com\\i2r\\utils\\log\\FileAppender.java", \
      BB_WRAPPER_SRC_ROOT + "sbox\\com\\i2r\\utils\\log\\Log.java", \
      BB_WRAPPER_SRC_ROOT + "sbox\\com\\i2r\\sedminstaller\\core\\SEDMLicense.java", \
      BB_WRAPPER_SRC_ROOT + "sbox\\com\\i2r\\sedminstaller\\core\\SEDMPacket.java", \
      BB_WRAPPER_SRC_ROOT + "sbox\\com\\i2r\\sedminstaller\\core\\SEDMProcess.java", \
      BB_WRAPPER_SRC_ROOT + "sbox\\com\\i2r\\sedminstaller\\crypto\\AESCrypto.java", \
      BB_WRAPPER_SRC_ROOT + "sbox\\com\\i2r\\sedminstaller\\crypto\\PDFCrypto.java", \
      BB_WRAPPER_SRC_ROOT + "sbox\\com\\i2r\\sedminstaller\\crypto\\PDFMD5.java", \
      BB_WRAPPER_SRC_ROOT + "sbox\\com\\i2r\\sedminstaller\\listener\\Listener.java", \
      BB_WRAPPER_SRC_ROOT + "sbox\\com\\i2r\\sedminstaller\\network\\HttpPacket.java", \
      BB_WRAPPER_SRC_ROOT + "sbox\\com\\i2r\\sedminstaller\\network\\HttpUtils.java", \
      BB_WRAPPER_SRC_ROOT + "sbox\\com\\i2r\\sedminstaller\\network\\Server.java", \
      BB_WRAPPER_SRC_ROOT + "sbox\\com\\i2r\\sedminstaller\\util\\ByteBuffer.java", \
      BB_WRAPPER_SRC_ROOT + "sbox\\com\\i2r\\sedminstaller\\util\\Convert.java", \
      BB_WRAPPER_SRC_ROOT + "sbox\\com\\i2r\\sedminstaller\\util\\FileSystem.java", \
      BB_WRAPPER_SRC_ROOT + "sbox\\com\\i2r\\sedminstaller\\util\\LoaderScreen.java", \
      BB_WRAPPER_SRC_ROOT + "res\\spdf.jpg"                                \
	};


// Proguard configuration 
#define PROGUARD_JAR_EXE        BB_PROJECT_ROOT + "JavaObfuscator\\proguard4.5.1\\lib\\proguard.jar"
#define PROGUARD_LIBJARS_1      "<java.home>/lib/rt.jar"
#define PROGUARD_LIBJARS_2      BB_LIB_JAR   /* use RIM library instead for proguard obfuscation */
#define	PROGUARD_OPTION         "@" + BB_PROJECT_ROOT + "JavaObfuscator\\src\\obfrules.txt"     // obsolete
#define BB_SOURCE_FILE_JAR      BB_CODENAME + ".jar"
#define BB_SOURCE_FILE_OBF_JAR  BB_CODENAME + "_obf.jar"
#define BB_SOURCE_FILE_OBF_MAP  BB_CODENAME + ".map"
#define	PROGUARD_OPTION1        \
     " -injars       \"" + BB_SOURCE_FILE_JAR     + "\"  \
       -outjars      \"" + BB_SOURCE_FILE_OBF_JAR + "\"  \
       -libraryjars  \"" + BB_LIB_JAR             + "\"  \
       -printmapping \"" + BB_SOURCE_FILE_OBF_MAP + "\"  \
       -repackageclasses \'\' \
       -optimizations \"!code/simplification/arithmetic\"  \
       -keep \"public class com.i2r.sedm.SPDFReader {public static void main(java.lang.String[]);}\"  \
       -keep \"interface * extends net.rim.device.api.ui.FieldChangeListener{public void fieldChanged(net.rim.device.api.ui.Field, int);}\" \
       -verbose \
      ";

#define	PROGUARD_OPTION2        \
     " -injars       \"" + BB_SOURCE_FILE_JAR     + "\"  \
       -outjars      \"" + BB_SOURCE_FILE_OBF_JAR + "\"  \
       -libraryjars  \"" + BB_LIB_JAR             + "\"  \
       -printmapping \"" + BB_SOURCE_FILE_OBF_MAP + "\"  \
       -keep \"public class com.i2r.sedm.SPDFReader {public static void main(java.lang.String[]);}\"  \
       -verbose \
      ";



////////////////////////////////////////////////////////////////////////////////////\\\---> above define java main entry setting


#define DEBUG_PRINT             false
#define DEBUG_ERR_FILE          BB_CODEPATH + "bberr.txt"

#define BB_CLEANUP_FILE_NUM   13
#define BB_CLEANUP_FILE_LIST  {  \
	                            "PDFreaderWrapper.cod",   \
                                "PDFreaderWrapper.csl",   \
                                "PDFreaderWrapper.cso",   \
                                "PDFreaderWrapper.debug", \
                                "PDFreaderWrapper.jad",   \
                                "PDFreaderWrapper.jar",   \
                                "PDFreaderWrapper.map",   \
                                "SecurePDF.orig",         \
                                "wrapper.csl",            \
                                "wrapper.cso",            \
                                "wrapper.debug",\
                                "wrapper-1.debug",\
                                "wrapper.jar"}



#endif

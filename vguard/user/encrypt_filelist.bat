
set vguard_enc_app_dir="d:\smartphone\Blackberry\JavaObfuscator\vguard\user\Debug"

set sedm_file_dir="D:\smartphone\BlackBerry\SEDMReader\sbox\com\i2r"
set sedm_file_bak_dir="D:\smartphone\BlackBerry\SEDMReader\src\com\i2r"

cd %vguard_enc_app_dir%


vguardapp.exe %sedm_file_bak_dir%\sedm\SecurePDF.java %sedm_file_dir%\sedm\SecurePDF.java 1

vguardapp.exe %sedm_file_bak_dir%\sedm\LoginScreen.java %sedm_file_dir%\sedm\LoginScreen.java 1
vguardapp.exe %sedm_file_bak_dir%\sedm\RecordManager.java %sedm_file_dir%\sedm\RecordManager.java 1
vguardapp.exe %sedm_file_bak_dir%\sedm\SPDFReader.java %sedm_file_dir%\sedm\SPDFReader.java 1
vguardapp.exe %sedm_file_bak_dir%\sedm\SPDFReader.rrc %sedm_file_dir%\sedm\SPDFReader.rrc 1
vguardapp.exe %sedm_file_bak_dir%\sedm\SPDFReader.rrh %sedm_file_dir%\sedm\SPDFReader.rrh 1
vguardapp.exe %sedm_file_bak_dir%\sedm\SPDFReaderAppPermissions.java %sedm_file_dir%\sedm\SPDFReaderAppPermissions.java 1
vguardapp.exe %sedm_file_bak_dir%\sedm\SPDFReaderCore.java %sedm_file_dir%\sedm\SPDFReaderCore.java 1
vguardapp.exe %sedm_file_bak_dir%\sedm\SPDFReaderDAO.java %sedm_file_dir%\sedm\SPDFReaderDAO.java 1
vguardapp.exe %sedm_file_bak_dir%\sedm\SPDFReaderPref.java %sedm_file_dir%\sedm\SPDFReaderPref.java 1
vguardapp.exe %sedm_file_bak_dir%\sedm\SPDFReaderScreen.java %sedm_file_dir%\sedm\SPDFReaderScreen.java 1


vguardapp.exe %sedm_file_bak_dir%\utils\CalendarUtils.java %sedm_file_dir%\utils\CalendarUtils.java 1
vguardapp.exe %sedm_file_bak_dir%\utils\PropertyUtils.java %sedm_file_dir%\utils\PropertyUtils.java 1
vguardapp.exe %sedm_file_bak_dir%\utils\SelectorPopupScreen.java %sedm_file_dir%\utils\SelectorPopupScreen.java 1
vguardapp.exe %sedm_file_bak_dir%\utils\StringUtils.java %sedm_file_dir%\utils\StringUtils.java 1
vguardapp.exe %sedm_file_bak_dir%\utils\Tools.java %sedm_file_dir%\utils\Tools.java 1

vguardapp.exe %sedm_file_bak_dir%\utils\conn\AbstractConfiguration.java %sedm_file_dir%\utils\conn\AbstractConfiguration.java 1
vguardapp.exe %sedm_file_bak_dir%\utils\conn\BESConfig.java %sedm_file_dir%\utils\conn\BESConfig.java 1
vguardapp.exe %sedm_file_bak_dir%\utils\conn\BISConfig.java %sedm_file_dir%\utils\conn\BISConfig.java 1
vguardapp.exe %sedm_file_bak_dir%\utils\conn\ConnectionManager.java %sedm_file_dir%\utils\conn\ConnectionManager.java 1
vguardapp.exe %sedm_file_bak_dir%\utils\conn\ConnectionUtils.java %sedm_file_dir%\utils\conn\ConnectionUtils.java 1
vguardapp.exe %sedm_file_bak_dir%\utils\conn\Gateway.java %sedm_file_dir%\utils\conn\Gateway.java 1
vguardapp.exe %sedm_file_bak_dir%\utils\conn\ServiceBookConfig.java %sedm_file_dir%\utils\conn\ServiceBookConfig.java 1
vguardapp.exe %sedm_file_bak_dir%\utils\conn\TcpConfig.java %sedm_file_dir%\utils\conn\TcpConfig.java 1
vguardapp.exe %sedm_file_bak_dir%\utils\conn\WiFiConfig.java %sedm_file_dir%\utils\conn\WiFiConfig.java 1

vguardapp.exe %sedm_file_bak_dir%\utils\File\CustomHashtable.java %sedm_file_dir%\utils\File\CustomHashtable.java 1
vguardapp.exe %sedm_file_bak_dir%\utils\File\FileUtils.java %sedm_file_dir%\utils\File\FileUtils.java 1
vguardapp.exe %sedm_file_bak_dir%\utils\File\JSR75FileSystem.java %sedm_file_dir%\utils\File\JSR75FileSystem.java 1

vguardapp.exe %sedm_file_bak_dir%\utils\log\Appender.java %sedm_file_dir%\utils\log\Appender.java 1 
vguardapp.exe %sedm_file_bak_dir%\utils\log\BlackberryEventLogAppender.java %sedm_file_dir%\utils\log\BlackberryEventLogAppender.java 1
vguardapp.exe %sedm_file_bak_dir%\utils\log\ConsoleAppender.java %sedm_file_dir%\utils\log\ConsoleAppender.java 1
vguardapp.exe %sedm_file_bak_dir%\utils\log\FileAppender.java %sedm_file_dir%\utils\log\FileAppender.java 1
vguardapp.exe %sedm_file_bak_dir%\utils\log\Log.java %sedm_file_dir%\utils\log\Log.java 1

vguardapp.exe %sedm_file_bak_dir%\sedminstaller\core\SEDMLicense.java %sedm_file_dir%\sedminstaller\core\SEDMLicense.java 1
vguardapp.exe %sedm_file_bak_dir%\sedminstaller\core\SEDMPacket.java %sedm_file_dir%\sedminstaller\core\SEDMPacket.java 1
vguardapp.exe %sedm_file_bak_dir%\sedminstaller\core\SEDMProcess.java %sedm_file_dir%\sedminstaller\core\SEDMProcess.java 1


vguardapp.exe %sedm_file_bak_dir%\sedminstaller\crypto\AESCrypto.java %sedm_file_dir%\sedminstaller\crypto\AESCrypto.java 1
vguardapp.exe %sedm_file_bak_dir%\sedminstaller\crypto\PDFCrypto.java %sedm_file_dir%\sedminstaller\crypto\PDFCrypto.java 1
vguardapp.exe %sedm_file_bak_dir%\sedminstaller\crypto\PDFMD5.java %sedm_file_dir%\sedminstaller\crypto\PDFMD5.java 1


vguardapp.exe %sedm_file_bak_dir%\sedminstaller\listener\Listener.java %sedm_file_dir%\sedminstaller\listener\Listener.java 1


vguardapp.exe %sedm_file_bak_dir%\sedminstaller\network\HttpPacket.java %sedm_file_dir%\sedminstaller\network\HttpPacket.java 1
vguardapp.exe %sedm_file_bak_dir%\sedminstaller\network\HttpUtils.java %sedm_file_dir%\sedminstaller\network\HttpUtils.java 1
vguardapp.exe %sedm_file_bak_dir%\sedminstaller\network\Server.java %sedm_file_dir%\sedminstaller\network\Server.java 1


vguardapp.exe %sedm_file_bak_dir%\sedminstaller\util\ByteBuffer.java %sedm_file_dir%\sedminstaller\util\ByteBuffer.java 1
vguardapp.exe %sedm_file_bak_dir%\sedminstaller\util\Convert.java %sedm_file_dir%\sedminstaller\util\Convert.java 1
vguardapp.exe %sedm_file_bak_dir%\sedminstaller\util\FileSystem.java %sedm_file_dir%\sedminstaller\util\FileSystem.java 1
vguardapp.exe %sedm_file_bak_dir%\sedminstaller\util\LoaderScreen.java %sedm_file_dir%\sedminstaller\util\LoaderScreen.java 1





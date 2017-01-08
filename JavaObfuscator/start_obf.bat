
@echo off
echo ***********************************************************
echo * customize, obfuscate jar file and turn it into cod file *
echo * usage:                                                  *
echo * example:  step 2 or 5    - create jar, cod files        *
echo *           step 1,2,5     - create customized cod file   *
echo *           step 1,2,3,4,5 - create customized, obfed cod *
echo ***********************************************************

REM set java_home="C:\Program Files\Java\jdk1.6.0_22"
REM set classpath=.;%java_home%\lib\dt.jar;%java_home%\lib\tools.jar
REM set path=%path%;%java_home%\bin


set wrapper_dir="D:\smartphone\BlackBerry\Secure PDF"
set wrapper_file=%wrapper_dir%\src\com\i2r\cas\securepdf\HelloWorld.java
set wrapper_new_file=.\HelloWorld.java
set wrapper_pin=12345678

cd "D:\smartphone\BlackBerry\JavaObfuscator\src"

echo step 1: customizing wrapper java file...
REM javac javacustomizer.java
REM java JavaCustomizer %wrapper_file% %wrapper_new_file% %wrapper_pin%
echo this part was replaced by C++ program.


echo step 2: compressing java files to jar file...
set blackberry_dir="C:\Program Files\eclipse\plugins\net.rim.ejde.componentpack4.7.0_4.7.0.57"
set path=%path%;%blackberry_dir%\components\bin
C:\Progra~1\eclipse\plugins\net.rim.ejde.componentpack5.0.0_5.0.0.25\components\bin\rapc.exe -quiet codename=PDFreaderWrapper "D:\smartphone\Blackberry\Secure PDF\deliverables\Standard\5.0.0\Secure_PDF.rapc" -import="C:\Progra~1\eclipse\plugins\net.rim.ejde.componentpack5.0.0_5.0.0.25\components\lib\net_rim_api.jar" @filelist.txt


echo step 3: obfuscating jar file...
SET PROGUARD_HOME="D:\smartphone\proguard4.5.1"

java -jar D:\smartphone\proguard4.5.1\lib\proguard.jar @obfrules.txt
del PDFreaderWrapper.jar
move PDFreaderWrapper_obf.jar PDFreaderWrapper.jar

echo step 4: preverifying jar file...
"C:\Program Files\eclipse\plugins\net.rim.ejde.componentpack5.0.0_5.0.0.25\components\bin\preverify.exe" -classpath "c:\program files\eclipse\plugins\net.rim.ejde.componentpack5.0.0_5.0.0.25\components\lib\net_rim_api.jar" PDFreaderWrapper.jar


echo step 5: converting jar to cod file...
C:\Progra~1\eclipse\plugins\net.rim.ejde.componentpack5.0.0_5.0.0.25\components\bin\rapc.exe -quiet codename=PDFreaderWrapper "D:\smartphone\Blackberry\Secure PDF\deliverables\Standard\5.0.0\Secure_PDF.rapc" -import="C:\Progra~1\eclipse\plugins\net.rim.ejde.componentpack5.0.0_5.0.0.25\components\lib\net_rim_api.jar" PDFreaderWrapper.jar
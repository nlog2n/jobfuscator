package com.i2r.sedm;

import net.rim.device.api.system.DeviceInfo;          // using RIM personal info module
import net.rim.device.api.crypto.MD5Digest;           // using RIM cryptography module
import net.rim.device.api.crypto.DigestOutputStream;  // using RIM cryptography module


//////////////////////////////////////// store secrets here :-)
final class SecurePDF {
  
  int my_wrapper_id;             
  int software_secret_key;
  
  String errDebug;

  public SecurePDF()
  {
	  software_secret_key = 10000;   // internal
      my_wrapper_id = 260;
      errDebug = "";
  }

  
  public boolean checkDevice()   // call it wherever needed
  {
    int pin = DeviceInfo.getDeviceId();
    errDebug  += " Device PIN=" +Integer.toString(pin) ;
    
    String PINid = getPINid(pin);
    errDebug  += ",PINid=" + PINid;

    int h  = getWrapperID( pin );
    errDebug  += ",WrapperID=" +Integer.toString(h);
    
    if ( h== my_wrapper_id ){
      return true;
    }else{
      return false;
    }
  }

  public String getErrString()
  {
    return errDebug;
  }

  private int getWrapperID( int pin )    
  {
    String pinStr = Integer.toString( pin );
    String aaa = pinStr + "SEDM BLACKBERRY" + pinStr;

    int PINidSum = getMD5Sum ( aaa );
    return PINidSum%software_secret_key;   // same as in Java obfuscator
  }
  
  
  private String getPINid( int pin )
  {
    String pinStr = Integer.toString( pin );
    return getMD5( pinStr + "SEDM BLACKBERRY" + pinStr );
  }
  
  // called by getWrapperID()
  private int getMD5Sum( String aaa ) 
  {
        byte[] source;           // input
        source = aaa.getBytes();
        byte[] digestData= new byte[16];  // output
            
        int s = 0;
        try {
          MD5Digest digest = new MD5Digest();// Create an instance of MD5 digest
          // Create the digest output stream for easy use
          DigestOutputStream digestStream = new DigestOutputStream( digest, null );
            
          // Write the text to the stream
          digestStream.write( source );   // input
          digest.getDigest( digestData, 0 );  // output,is a 128-bit 16 bytes
          int  md5Len = digest.getDigestLength();    // output = 16
          if (md5Len != 16) 
        	  errDebug += ",strange: md5 len !=16";

          for (int i = 0; i < md5Len; i++) {
              byte byte0 = digestData[i];   
              //s = s + byte0;
              
              s += byte0 >>> 4 & 0xf;  // high 4 bits of byte. >>>: rshift
              s += byte0 & 0xf;        // low 4 bits of byte
              
          }
        } catch( Exception e ) {
          e.printStackTrace();
          errDebug +=  e.getMessage();
        }
        
        errDebug += ", MD5 sum:" + Integer.toString(s);
        return s;
  }

  
  
  
  
  private String getMD5( String aaa ) 
  {
	  byte[] source;      // input
      byte[] digestData = new byte[16];  // output
      source = aaa.getBytes();
      String s = null;
      char hexDigits[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd',  'e', 'f'}; 
      try {
    	  MD5Digest digest = new MD5Digest();// Create an instance of MD5 digest
          // Create the digest output stream for easy use
          DigestOutputStream digestStream = new DigestOutputStream( digest, null );
                
          // Write the text to the stream
          digestStream.write( source );   // input
          digest.getDigest( digestData, 0 );  // output,is a 128-bit 16 bytes
          int  md5Len = digest.getDigestLength();    // output = 16
        
          char str[] = new char[md5Len * 2];   // each bytes in 2 chars, so 32 chars
          int k = 0;               
          for (int i = 0; i < md5Len; i++) {
        	  byte byte0 = digestData[i];   
              str[k++] = hexDigits[byte0 >>> 4 & 0xf];  // high 4 bits of byte. >>>: rshift
              str[k++] = hexDigits[byte0 & 0xf];        // low 4 bits of byte
          }
          
          s = new String(str);        //  to string
      } catch( Exception e ) {
    	  e.printStackTrace();
          errDebug +=  e.getMessage();
      }
      
      errDebug += ", MD5 string:" + s;      
      return s;
  }
        
}


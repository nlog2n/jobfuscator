package com.i2r.cas.securepdf;

//import net.rim.device.api.system.DeviceInfo;  // using RIM personal info module
//import net.rim.device.api.crypto.MD5Digest;   // using RIM cryptography module
//import net.rim.device.api.crypto.DigestOutputStream;


//////////////////////////////////////// store secrets here :-)

final class SecurePDF {
	
	int my_wrapper_id;             
	int software_secret_key;
	
	String errDebug;

	public SecurePDF()
        {      	
    	        software_secret_key = 10000;   // internal
my_wrapper_id = 219;

                errDebug = "OK";
        }

	
	public boolean checkDevice()   // call it wherever needed
	{
		return true;
	}

	public String getErrString()
	{
		return errDebug;
	}

}




























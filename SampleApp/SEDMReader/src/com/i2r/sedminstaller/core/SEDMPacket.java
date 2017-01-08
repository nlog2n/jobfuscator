package com.i2r.sedminstaller.core;

import java.io.UnsupportedEncodingException;

import com.i2r.sedminstaller.util.Convert;

import net.rim.device.api.crypto.MD5Digest;
import net.rim.device.api.crypto.SHA1Digest;
import net.rim.device.api.system.DeviceInfo;

public class SEDMPacket {
	byte[] packet;
	
	public SEDMPacket()
	{
		
	}
	
	public void CreateDownloadPacket(byte[] nonce, byte[] password) {
		packet = new byte[64];
		
		SHA1Digest shaPassword = new SHA1Digest();
		shaPassword.reset();
		shaPassword.update(password, 0, password.length);
		byte[] pwd = shaPassword.getDigest();
		
		for (int i=0; i<28; i++)
			packet[i] = nonce[i];
		for (int i=28; i<48; i++)
			packet[i] = pwd[i-28];
		
		byte[] md5 = getPIN();
		
		for (int i=48; i<64; i++)
			packet[i] = md5[i-48];
	}
	
	public void CreateLicensePacket(byte[] nonce, byte[] password, int fileID) {
		packet = new byte[64 + 16];
	
		SHA1Digest shaPassword = new SHA1Digest();
		shaPassword.reset();
		shaPassword.update(password, 0, password.length);
		byte[] pwd = shaPassword.getDigest();
		
		for (int i=0; i<28; i++)
			packet[i] = nonce[i];
		for (int i=28; i<48; i++)
			packet[i] = pwd[i-28];
		
		byte[] md5 = getPIN();
		
		for (int i=48; i<64; i++)
			packet[i] = md5[i-48];
		
		byte[] id = Convert.intToByteArray(fileID);
		
		for (int i=64; i<64 + 16; i++)
		{
			if (i < 68)
				packet[i] = id[i-64];
			else
				packet[i] = (byte) i;
		}	
	}
	
	public byte[] getPacket() {
		return packet;
	}
	
	public byte[] getPIN() {
		String pin = Integer.toString(
       		 DeviceInfo.getDeviceId(),16).toUpperCase();
		
		String PINid = pin + "SEDM BLACKBERRY" + pin;
		MD5Digest digest = new MD5Digest();
	    try {
			digest.update(PINid.getBytes("UTF-8"), 0, PINid.getBytes("UTF-8").length);
		} catch (UnsupportedEncodingException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
	    int length = digest.getDigestLength();
	    byte[] md5 = new byte[length];
	    digest.getDigest(md5, 0, true);
	    
	    return md5;
	}
}

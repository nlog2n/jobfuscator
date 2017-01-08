package com.i2r.sedminstaller.core;

import com.i2r.sedminstaller.network.Server;
import com.i2r.sedminstaller.util.Convert;
import com.i2r.sedminstaller.util.FileSystem;

public class SEDMLicense {
	private byte[] softwareKc;
	private int    rights;
	
	public SEDMLicense() {
		softwareKc = new byte[32];
	}
	
	public SEDMLicense(byte[] lic) {
		softwareKc = new byte[32];
		process(lic);
	}
	public void setSoftwareKc(byte[] input) {
		System.arraycopy(input, 0, softwareKc, 0, 32);
	}
	
	public void setRights(int input) {
		rights = input;
	}
	
	public byte[] getSoftwareKey() {
		return softwareKc;
	}
	
	public int getRights() {
		return rights;
	}
	
	public void print() {
		try {
			Server.showToast("License: " + Convert.getHexString(softwareKc) + " Rights: " + Integer.toString(rights));
		} catch (Exception e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
	}
	
	public void writeToFile(String filename) {
		byte[] temp = new byte[32 + 4];
		System.arraycopy(softwareKc, 0, temp, 0, 32);
		byte[] temp_rights = Convert.intToByteArray(rights);
		System.arraycopy(temp_rights, 0, temp, 32, 4);
		
		FileSystem.WriteFile(filename, temp);
	}
	
	public void readFromFile(String filename) {
		byte[] temp = FileSystem.ReadFile(filename);
		process(temp);
	}
	
	private void process(byte[] lic) {
		byte[] bRights = new byte[4];
		
		System.arraycopy(lic, 0, softwareKc, 0, 32);
		System.arraycopy(lic, 32, bRights, 0, 4);
		rights = Convert.byteArrayToInt(bRights);
		
	}
}

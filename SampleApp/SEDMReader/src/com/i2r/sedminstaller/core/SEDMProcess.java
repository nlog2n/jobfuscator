package com.i2r.sedminstaller.core;

import net.rim.device.api.ui.component.Dialog;

import com.i2r.sedminstaller.crypto.AESCrypto;
import com.i2r.sedminstaller.crypto.PDFCrypto;
import com.i2r.sedminstaller.util.Convert;
import com.i2r.sedminstaller.util.FileSystem;

public class SEDMProcess {
	private int fileID;
	private byte[] input;
	
	private static byte[] softwareKey = new byte[] {
		 0x0A, 0x00, 0x0B, 0x01, 0x0C, 0x01, 0x01, 0x01, 
		 0x00, 0x00, 0x05, 0x00, 0x00, 0x0D, 0x00, 0x00,
		 0x00, 0x00, 0x03, 0x02, 0x00, (byte) 0xFF, (byte) 0xFF, (byte) 0xF5 
	};
	
	private static byte[] softwareIV = new byte[] { 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
			0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x00 };
	
	
	public void DecryptEncryptedPDF(SEDMLicense lic)
	{
		AESCrypto aesc = new AESCrypto(softwareKey, softwareIV, 192);
		byte[] key = new byte[32];
		aesc.decrypt(lic.getSoftwareKey(), key, 0);
		
		PDFCrypto pc = new PDFCrypto();
		
		pc.EncryptStream(key, 32, input, input.length, 0, 40960);
		
		boolean b = FileSystem.WriteFile("file:///SDCard/Blackberry/documents/result.pdf", input);
		
		if (b) {
			/* Call ReadPDF */
			Dialog.alert("Success");
			
		}
		else
			Dialog.alert("Error in reading the PDF file!");
		
	}

	public void DecryptEncryptedPDF2(SEDMLicense lic,String pdffilename)
	{
		AESCrypto aesc = new AESCrypto(softwareKey, softwareIV, 192);
		byte[] key = new byte[32];
		aesc.decrypt(lic.getSoftwareKey(), key, 0);
		
		PDFCrypto pc = new PDFCrypto();
		
		pc.EncryptStream(key, 32, input, input.length, 0, 40960);
		
		boolean b = FileSystem.WriteFile(pdffilename, input);
/*		
		if (b) {
			Dialog.alert("Success");
			
		}
		else
			Dialog.alert("Error in reading the PDF file!");
*/		
		
	}	
	public void ReadPlainPDF(String filename) 
	{
		
	}
	
	public void DeletePlainPDF(String filename)
	{
		
	}
	
	public void ReadEncryptedPDF(String filename) 
	{
		input = FileSystem.ReadFile(filename);
		byte[] temp = new byte[4];
		System.arraycopy(input, input.length-8, temp, 0, 4);
		fileID = Convert.byteArrayToIntLittleEndian(temp);
	}
	
	public int GetFileID()
	{
		return fileID;
	}
}

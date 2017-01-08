package com.i2r.sedminstaller.util;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;

import javax.microedition.io.Connector;
import javax.microedition.io.file.FileConnection;

import net.rim.device.api.ui.component.Dialog;

/*"file:///SDCard/test.gif"*/

public class FileSystem {
	
	public static byte[] ReadFile(String filename)  {
		try {
			FileConnection fc = (FileConnection)Connector.open(filename);
		    boolean bFileExists = fc.exists();
		    if (!bFileExists)
		    {
		      Dialog.alert("Cannot find specified file.");
		      System.exit(0);
		    }
	   
			InputStream in = fc.openInputStream();
			int size = (int) fc.fileSize();
			
			byte[] b = new byte[size];

			//reading data from a InputStream
			in.read(b,0,size);
			
			fc.close();
			return b;
		} catch (IOException e) {
			// TODO Auto-generated catch block
			System.out.println(e.getMessage() );
		}
		
		return null;
	}
	
	public static boolean WriteFile(String filename, byte[] output)  {
		try {
			FileConnection fc = (FileConnection)Connector.open(filename);
		    boolean bFileExists = fc.exists();
		    if (bFileExists)
		    {
		      fc.delete();
		      fc.close();
		      
		      fc = (FileConnection)Connector.open(filename);
		    }
	   
		    fc.create();
		    
			OutputStream out = fc.openOutputStream();
			out.write(output);
			out.close();
			
			fc.close();
			return true;
		} catch (IOException e) {
			// TODO Auto-generated catch block
			System.out.println(e.getMessage() );
		}
		return false;
	}
	
	public static void DeleteFile(String filename) {
		try {
			FileConnection fc = (FileConnection)Connector.open(filename);
		    boolean bFileExists = fc.exists();
		    if (bFileExists)
		    {
		      fc.delete();
		      fc.close();
		    }	
		} catch (IOException e) {
			// TODO Auto-generated catch block
			System.out.println(e.getMessage() );
		}
		
	}
}

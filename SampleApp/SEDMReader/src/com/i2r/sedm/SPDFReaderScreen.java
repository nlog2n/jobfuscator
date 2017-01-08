/**
 * 
 */
package com.i2r.sedm;

import java.io.ByteArrayInputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.util.Random;

import javax.microedition.content.ContentHandler;
import javax.microedition.content.Invocation;
import javax.microedition.content.Registry;
import javax.microedition.io.Connection;
import javax.microedition.io.Connector;
import javax.microedition.io.file.FileConnection;

import com.i2r.utils.File.JSR75FileSystem;
import com.i2r.utils.log.Log;
import com.i2r.sedminstaller.listener.*;
import com.i2r.sedminstaller.network.Server;

import net.rim.device.api.crypto.ARC4Key;
import net.rim.device.api.crypto.ARC4PseudoRandomSource;
import net.rim.device.api.crypto.CryptoException;
import net.rim.device.api.crypto.PRNGDecryptor;
import net.rim.device.api.crypto.PRNGEncryptor;
import net.rim.device.api.io.NoCopyByteArrayOutputStream;

import net.rim.device.api.system.ApplicationManager;
import net.rim.device.api.ui.UiApplication;
import net.rim.device.api.ui.component.Dialog;
import net.rim.device.api.ui.container.MainScreen;
import net.rim.device.api.ui.Field;
import net.rim.device.api.ui.MenuItem;
import net.rim.device.api.ui.component.Dialog;
import net.rim.device.api.ui.component.EditField;
import net.rim.device.api.ui.component.LabelField;
import net.rim.device.api.ui.component.Menu;
import net.rim.device.api.ui.component.SeparatorField;

/**
 * @author zzhao
 * 
 */
public class SPDFReaderScreen extends MainScreen {
	private static String ID = "com.i2r.sedm.SPDFReaderScreen";
	private static String CLASSNAME = "com.i2r.sedm.SPDFReaderScreen";

	/**
	 * 
	 */
	SPDFReader spdfapp;
	EditField _ef;
	private LabelField _instructions;
	Random random;
	Listener listener;
	String pdffilename = "";

	public SPDFReaderScreen() {
		// TODO Auto-generated constructor stub
		super(NO_SYSTEM_MENU_ITEMS);
		setTitle("SPDF Reader");

		spdfapp = (SPDFReader) UiApplication.getUiApplication();
		random = new Random();
		listener = new Listener();

		// Initialize UI components
		_instructions = new LabelField("", Field.NON_FOCUSABLE);

		// An EditField where a user can enter the path to the document to be
		// opened.
		// An example path is pre-populated.
		add(new LabelField("\n\n", Field.NON_FOCUSABLE));
		add(new SeparatorField());
		_ef = new EditField("SEDM File to open: ", "file:///");
		add(_ef);
		add(new LabelField("\n", Field.NON_FOCUSABLE));
		add(new LabelField("\n\n\n\n\n\n", Field.NON_FOCUSABLE));
		add(new SeparatorField());
		add(_instructions);

		/*
		 * MenuItem menu = new MenuItem("Open File", 40, 40) { public void run()
		 * { try { // Create the invocation request. Invocation invocation = new
		 * Invocation(_ef.getText());
		 * invocation.setAction(ContentHandler.ACTION_OPEN);
		 * invocation.setResponseRequired(true);
		 * 
		 * // Use the registry to perform the invocation. Registry registry =
		 * Registry.getRegistry("com.i2r.sedm.SPDFReader");
		 * registry.invoke(invocation);
		 * 
		 * Dialog.alert("Invoked " + _ef.getText()); } catch (Exception ex) {
		 * System.out.println("Exception: " + ex.toString());
		 * Dialog.alert("Exception: " + ex.toString()); }
		 * setInstructions("Open SEDM file " + _ef.getText() + " by menu"); } };
		 * addMenuItem(menu);
		 */
	}

	/*
	 * protected void makeMenu(Menu menu, int instance) { menu.deleteAll();
	 * 
	 * menu.add(MenuItem.separator(122));
	 * 
	 * menu.add(MenuItem.separator(155)); menu.addSeparator(); }
	 */
	public String getRandomStr() {
		String str = new String(
				"QAa0bcLdUK2eHfJgTP8XhiFj61DOklNm9nBoI5pGqYVrs3CtSuMZvwWx4yE7zR");
		StringBuffer sb = new StringBuffer();
		int te = 0;
		for (int i = 1; i <= 8; i++) {
			te = random.nextInt(62);
			sb.append(str.charAt(te));
		}
		return sb.toString();
	}

	/**
	 * If Java 1.4 is unavailable, the following technique may be used.
	 * 
	 * @param aInput
	 *            is the original String which may contain substring aOldPattern
	 * @param aOldPattern
	 *            is the non-empty substring which is to be replaced
	 * @param aNewPattern
	 *            is the replacement for aOldPattern
	 */
	public static String replaceOld(final String aInput,
			final String aOldPattern, final String aNewPattern) {
		if (aOldPattern.equals("")) {
			throw new IllegalArgumentException("Old pattern must have content.");
		}

		final StringBuffer result = new StringBuffer();
		// startIdx and idxOld delimit various chunks of aInput; these
		// chunks always end where aOldPattern begins
		int startIdx = 0;
		int idxOld = 0;
		while ((idxOld = aInput.indexOf(aOldPattern, startIdx)) >= 0) {
			// grab a part of aInput which does not include aOldPattern
			result.append(aInput.substring(startIdx, idxOld));
			// add aNewPattern to take place of aOldPattern
			result.append(aNewPattern);

			// reset the startIdx to just after the current match, to see
			// if there are any further matches
			startIdx = idxOld + aOldPattern.length();
		}
		// the final chunk will go to the end of aInput
		result.append(aInput.substring(startIdx));
		return result.toString();
	}

	public void InvokeSPDF() {
		String pdffilepath = "";
		//String pdffilename = "";
		if (spdfapp.currentSPDFFile != null) {
			Dialog.alert("invoked for: " + spdfapp.currentSPDFFile);
			_ef.setText(spdfapp.currentSPDFFile);

/*			pdffilename = spdfapp.currentSPDFFile.substring(0,
					spdfapp.currentSPDFFile.lastIndexOf('/') + 1)
					+ getRandomStr() + ".pdf";
*/

			try {
				if(JSR75FileSystem.isFileExist(SPDFReaderDAO.getBaseDirPath() +SPDFReaderDAO.TMP_DIR))
				{
					pdffilepath = SPDFReaderDAO.getBaseDirPath() +SPDFReaderDAO.TMP_DIR +getRandomStr()+"/";
				}
				else
				{
					pdffilepath = spdfapp.currentSPDFFile.substring(0,spdfapp.currentSPDFFile.lastIndexOf('/') + 1)
							+ getRandomStr()+"/";
				}
				JSR75FileSystem.createDir(pdffilepath);
				
				
				pdffilename = pdffilepath+ getRandomStr() + ".pdf";
				
				
				listener.SubmitLicenseProcess(spdfapp.userName,spdfapp.passWord,spdfapp.currentSPDFFile,pdffilename);


				
				
				UiApplication.getUiApplication().invokeLater(new Runnable()  {
        			public void run() {					
			                boolean keepGoing = true;
			                int trynum =0;
			                try {
			                while (keepGoing && trynum<50)
			                {
			                	trynum++;
			                    if (!listener.flag )
			                    {
			                        try { Thread.sleep(100); }
			                        catch (Exception ex) { }
			                        if (trynum>=50)
			                        {
			                        	Dialog.alert("Error on online processing for SPDF file"); 
			                        }
			                    }
			                    else
			                    {
			                    	keepGoing = false;
			                    	startRead();
			                   	
			                    }//end else
			    			}
			    			} catch (Exception ex) {
			    				Log.error("SPDFReaderScreen#InvokeSPDF() threw: Exception: "
			    								+ ex.toString());
			    				Dialog.alert("SPDFReaderScreen#InvokeSPDF() threw: Exception: "
			    						+ ex.toString());
			    			}

        			}
 			    });
				
			} catch (Exception ex) {
				Log.error("SPDFReaderScreen#InvokeSPDF() threw: Exception: "
								+ ex.toString());
				Dialog.alert("SPDFReaderScreen#InvokeSPDF() threw: Exception: "
						+ ex.toString());
			}

//			setInstructions("Open SEDM file: " + pdffilename);
//			spdfapp.currentSPDFFile = null;
		}
	}

	public void AddPDF(String pdffilename) {
		RecordManager rm = new RecordManager();
		String[] strs = rm.Read("PDFFileURL");
		int i = strs.length - 1;

		try {
			while (i >= 0) {
				if (strs[i] != null && strs[i].length() > 0) {
					spdfapp.DeleteFile(strs[i]);
				}
				i--;
			}
		} catch (Exception ex) {
			System.out.println("SPDFReaderScreen#AddPDF() threw "
					+ ex.toString());
		} finally {
			String[] strs2 = new String[17];
			int j = 0;
			int k = 0;
			while (j <= i && j < 16) {

				if (strs[j] != null && strs[j].length() > 0) {
					strs2[k] = strs[j];
					k++;
				}
				j++;
			}
			strs2[k] = pdffilename;
			rm.Save(strs2, "PDFFileURL");
		}
	}

	private void setInstructions(String instruction) {
		_instructions.setText(instruction);
	}

	void startRead()
	{
		try{
		AddPDF(pdffilename);

		// Create the invocation request.
		Invocation invocation = new Invocation(pdffilename);
		invocation.setAction(ContentHandler.ACTION_OPEN);
		invocation.setResponseRequired(true);

		// Use the registry to perform the invocation.
		Registry registry = Registry
				.getRegistry("com.i2r.sedm.SPDFReader");
		registry.setListener(spdfapp);
		registry.invoke(invocation);
		
		setInstructions("Open SEDM file: " + pdffilename);
		spdfapp.currentSPDFFile = null;	
	} catch (Exception ex) {
		Log.error("SPDFReaderScreen#InvokeSPDF() threw: Exception: "
						+ ex.toString());
		Dialog.alert("SPDFReaderScreen#InvokeSPDF() threw: Exception: "
				+ ex.toString());
	}
	}
	
	void decryptFile(String sfile, String tfile) {
		try {
			encryptFile(sfile, tfile, false);
		} catch (Exception ex) {
			System.out
					.println("SPDFReaderScreen#decryptFile() threw: Exception: "
							+ ex.toString());
			Dialog.alert("SPDFReaderScreen#decryptFile() threw: Exception: "
					+ ex.toString());
		}
	}

	public boolean onSavePrompt() {
		// Suppress the save dialog
		return true;
	}

	void encryptFile(String sfile, String tfile, boolean encrypt)
			throws IOException {
		String enckey = "123456";
		if (sfile == null) {
			throw new NullPointerException("sfile==null");
		} else if (tfile == null) {
			throw new NullPointerException("tfile==null");
		}

		Connection conIn;
		Connection conOut;

		FileConnection fconIn;
		FileConnection fconOut;

		try {
			conIn = Connector.open(sfile);
			fconIn = (FileConnection) conIn;
			if (!fconIn.exists()) {
				throw new IOException(
						"SPDFReaderScreen#encryptFile() threw: directory does not exist");
			}
		} catch (Exception e) {
			throw new IOException(
					"SPDFReaderScreen#encryptFile() threw: unable to open connection to "
							+ sfile + ": " + e);
		}

		try {
			conOut = Connector.open(tfile);
			fconOut = (FileConnection) conOut;
			if (!fconOut.exists()) {
				try {
					fconOut.create();
				} catch (Exception e) {
					throw new IOException(
							"SPDFReaderScreen#encryptFile() threw: unable to create "
									+ fconOut.getURL() + ": " + e);
				}
			}
		} catch (Exception e) {
			throw new IOException(
					"SPDFReaderScreen#encryptFile() threw: unable to open connection to "
							+ tfile + ": " + e);
		}

		InputStream in;
		try {
			in = fconIn.openInputStream();
		} catch (Exception e) {
			throw new IOException(
					"SPDFReaderScreen#encryptFile() threw: unable to open input stream to "
							+ fconIn.getURL() + ": " + e);
		}
		try {

			OutputStream out;
			try {
				out = fconOut.openOutputStream();
			} catch (Exception e) {
				throw new IOException(
						"SPDFReaderScreen#encryptFile() threw: unable to open output stream to "
								+ fconOut.getURL() + ": " + e);
			}

			try {
				byte[] buffer = new byte[2048];
				byte[] buffer2 = new byte[2048];
				int readCount = in.read(buffer);
				while (readCount >= 0) {
					if (encrypt)
						ARC4Encryption(enckey.getBytes(), buffer, buffer2,
								readCount);
					else
						ARC4Decryption(enckey.getBytes(), buffer, buffer2,
								readCount);
					out.write(buffer2, 0, readCount);
					readCount = in.read(buffer);
				}
			} catch (Exception e) {
				throw new IOException(
						"SPDFReaderScreen#encryptFile() threw: unable to copy data from "
								+ fconIn.getURL() + " to " + fconOut.getURL()
								+ ": " + e);
			} finally {
				try {
					out.close();
				} catch (Exception e) {
				}
			}
		} finally {
			try {
				in.close();
			} catch (Exception e) {
			}
		}
	}

	private static int ARC4Encryption(byte[] secretKey, byte[] plainText,
			byte[] cipherText, int dataLength) throws CryptoException,
			IOException {
		// Create a new ARC4 key based on the bytes in the secretKey array
		ARC4Key key = new ARC4Key(secretKey);

		// Create a new byte array output stream for use in encryption
		NoCopyByteArrayOutputStream out = new NoCopyByteArrayOutputStream();

		// Now create a new instance of the PRNGEncryptor (pseudorandom number
		// generator
		// encryptor) and pass in an ARC4 pseudorandom source along with the
		// output stream
		PRNGEncryptor cryptoStream = new PRNGEncryptor(
				new ARC4PseudoRandomSource(key), out);

		// Write dataLength bytes from plainText to the ARC4 encryptor stream
		cryptoStream.write(plainText, 0, dataLength);
		cryptoStream.close();

		// Now copy the encrypted bytes from out into cipherText and return the
		// length
		int finalLength = out.size();
		System.arraycopy(out.getByteArray(), 0, cipherText, 0, finalLength);
		return finalLength;
	}

	private static int ARC4Decryption(byte[] secretKey, byte[] cipherText,
			byte[] plainText, int dataLength) throws CryptoException,
			IOException {
		// Create a new ARC4 key based on the bytes in the secretKey array
		ARC4Key key = new ARC4Key(secretKey);

		// Create a new byte array input stream based on the first dataLength
		// bytes in cipherText
		ByteArrayInputStream in = new ByteArrayInputStream(cipherText, 0,
				dataLength);

		// Now create a new instance of the PRNGDecryptor (pseudorandom number
		// generator
		// decryptor) and pass in an ARC4 pseudorandom source along with the
		// input stream
		PRNGDecryptor cryptoStream = new PRNGDecryptor(
				new ARC4PseudoRandomSource(key), in);

		// Read from the decryptor stream and place the decrypted bytes in
		// plainText,
		// returning the actual number of bytes read
		return cryptoStream.read(plainText, 0, dataLength);
	}
/*
	public boolean onClose() {
		listener.EndConnection();
		return true;
	}
*/	
}

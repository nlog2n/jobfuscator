package com.i2r.sedminstaller.listener;

import java.io.IOException;

import net.rim.blackberry.api.browser.Browser;
import net.rim.blackberry.api.browser.BrowserSession;
import net.rim.device.api.system.Bitmap;
import net.rim.device.api.ui.Manager;
import net.rim.device.api.ui.UiApplication;
import net.rim.device.api.ui.component.Dialog;

import com.i2r.sedminstaller.core.SEDMLicense;
import com.i2r.sedminstaller.core.SEDMPacket;
import com.i2r.sedminstaller.core.SEDMProcess;
import com.i2r.sedminstaller.crypto.AESCrypto;
import com.i2r.sedminstaller.network.HttpPacket;
import com.i2r.sedminstaller.network.Server;
import com.i2r.sedminstaller.util.ByteBuffer;
import com.i2r.sedminstaller.util.LoaderScreen;


public class Listener {
	private static String downloadURL = "http://10.217.141.250/Blackberry/Download.aspx";
	private static String licenseURL = "http://10.217.141.250/Blackberry/License.aspx";
	
	byte[] key = { 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16};
	byte[] IV = { 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16};
	byte[] random;
	public boolean flag = false;
	public int userflag = 0;
	public Listener() {
		this.random = new byte[28];
	}
	
	public void ExitProcess() {
		EndConnection();
		System.exit(0);
	}
	
	public void SubmitDownloadProcess(final String username, final String password) {
		
		final Dialog linkDialog = new Dialog(Dialog.D_OK_CANCEL, "Download SEDM Application?", 1,
		          Bitmap.getPredefinedBitmap(Bitmap.EXCLAMATION), Manager.FOCUSABLE);
		
		if ((username.length()!=0) && (password.length()!=0))
		{	
			Runnable runnable = new Runnable() {
		        public void run() {
		          //Server.checkEDT();
		          //Server.checkCoverage();
		          	
		          try {	                
		        	final ByteBuffer permission = new ByteBuffer(Server.POST(downloadURL, new HttpPacket(null,HttpPacket.DOWNLOAD,null,null)));
		        	//Server.showToast(permission.getString());
		        	if ((permission.getBytes()[0] == 'O') && (permission.getBytes()[2] == 'K')) {
		        		EndConnection();
		        		final ByteBuffer randomNonce = new ByteBuffer(Server.POST(downloadURL, new HttpPacket(username.getBytes(),HttpPacket.USERNAME,null,null)));
		        		random = randomNonce.getBytes();
		        		//Server.showToast(randomNonce.getString());
		        		SEDMPacket data = new SEDMPacket();
		        		data.CreateDownloadPacket(randomNonce.getBytes(), 
		        				                  password.getBytes("UTF-8") 
		        				                 /*,(Integer.toString(
		        				                		 DeviceInfo.getDeviceId(),16)).toUpperCase()*/);
		        		final ByteBuffer downloadLink = new ByteBuffer(Server.POST(downloadURL,	new HttpPacket(data.getPacket(),HttpPacket.AES,key,IV)));
		        		
		        		if ((downloadLink.getBytes()[0] == 'h') && 
		        		    (downloadLink.getBytes()[1] == 't') &&
		        		    (downloadLink.getBytes()[2] == 't') &&
		        		    (downloadLink.getBytes()[3] == 'p')) {
		        		
		        			//Server.showToast(downloadLink.getString());

		        			UiApplication.getUiApplication().invokeLater(new Runnable() {
		        			public void run() {
		        				// TODO Auto-generated method stub
			        			//Dialog.alert(downloadLink.getString());		        			
		        				if (linkDialog.doModal() == Dialog.D_OK) {
		        					BrowserSession bSession = Browser.getDefaultSession();
		        					bSession.displayPage(downloadLink.getString());
		        				}
		        				}
		        			});
		        			
		        		} else
		        			Server.showToast(downloadLink.getString());
		        	} else
		        		Server.showToast("SEDM Blackberry Server Error (0x1001)");
		          }
		          catch (final Exception e) {
		            System.out.println(e.toString());
		            Server.showToast("SEDM Blackberry Server Exception (0x100A) : " + e.toString());
		          }
		          LoaderScreen screen = (LoaderScreen) UiApplication.getUiApplication().getActiveScreen();
		          screen.setShowLoader(false);
		        }
		      };

		      try {
		          new Thread(runnable).start();
		        }
		        catch (Exception e) {
		          System.out.println(e.toString());
		          Server.showToast("SEDM Blackberry Server Exception (0x100B): " + e.toString());
		        }
		} else {
			Dialog.alert("Username/Password must be valid");
			LoaderScreen screen = (LoaderScreen) UiApplication.getUiApplication().getActiveScreen();
	        screen.setShowLoader(false);
		}
	}

	/**
	 * Presents a dialog to the user with a given message
	 * 
	 * @param message
	 *            The text to display
	 */
	public static void errorDialog(final String message) {
		UiApplication.getUiApplication().invokeLater(new Runnable() {
			public void run() {
				Dialog.alert(message);
			}
		});
	}
	
	public void SubmitLicenseProcess(final String username, final String password, final String filename,final String PDFFilename) {
		
		if ((username.length()!=0) && (password.length()!=0))
		{	
			flag = false;
			Runnable runnable = new Runnable() {
		        public void run() {
		          //Server.checkEDT();
		          //Server.checkCoverage();
		          	
		          try {	                
		        	final ByteBuffer permission = new ByteBuffer(Server.POST(licenseURL, new HttpPacket(null,HttpPacket.LICENSE,null,null)));
		        	//Server.showToast(permission.getString());
		        	if ((permission.getBytes()[0] == 'O') && (permission.getBytes()[2] == 'K')) {
		        		//EndConnection();
		        		final ByteBuffer randomNonce = new ByteBuffer(Server.POST(licenseURL, new HttpPacket(username.getBytes(),HttpPacket.USERNAME,null,null)));
		        		random = randomNonce.getBytes();

		        		final SEDMProcess sp = new SEDMProcess();
		        		sp.ReadEncryptedPDF(filename);
		    
		        		//Server.showToast(Integer.toString(sp.GetFileID()));
		        		
		        		SEDMPacket data = new SEDMPacket();
		        		data.CreateLicensePacket(randomNonce.getBytes(), 
		        				                 password.getBytes("UTF-8")/*, 
		        				                 (Integer.toString(
		        				                		 DeviceInfo.getDeviceId(),16)).toUpperCase()*/,
		        				                 sp.GetFileID());
		        		
		        		final ByteBuffer downloadLink = new ByteBuffer(Server.POST(licenseURL,	new HttpPacket(data.getPacket(),HttpPacket.AES,key,IV)));
		        		
		        		if ((downloadLink.getBytes()[0] == (byte) '!') && 
		        		    (downloadLink.getBytes()[1] == (byte) '!') &&
		        		    (downloadLink.getBytes()[2] == (byte) '!')) {
		        		
		        			byte[] response = new byte[32 + 16];
		        			byte[] encResponse = new byte[32 + 16];
		        			System.arraycopy(downloadLink.getBytes(), 3, encResponse, 0, 32+16);
		        			
		        			AESCrypto decryptor = new AESCrypto(data.getPIN(), IV);
		        			decryptor.decrypt(encResponse, response, 0);
		        		    
		        			final SEDMLicense lic = new SEDMLicense(response);
		        			
		        			errorDialog("Get License!");
		        			
		        			sp.DecryptEncryptedPDF2(lic,PDFFilename);
		        			flag =true;
		        			
		        			
		        			errorDialog("Finish Decrypted!");
		        			
/*		        			
 	        			    UiApplication.getUiApplication().invokeLater(new Runnable() {
			        			public void run() {
			        				// TODO Auto-generated method stub
			        				//lic.print();
			        				
			        				sp.DecryptEncryptedPDF2(lic,PDFFilename);
				        			errorDialog("Finish Decrypted!");
				        			flag =true;
			        				}
			        			});
*/		        			
		        		} else
		        			Server.showToast(downloadLink.getString());
		        	} else
		        		Server.showToast("SEDM Blackberry Server Error (0x1001)");
		          }
		          catch (final Exception e) {
		            System.out.println(e.toString());
		            Server.showToast("SEDM Blackberry Server Exception (0x100A) : " + e.toString());
		          }
		          //LoaderScreen screen = (LoaderScreen) UiApplication.getUiApplication().getActiveScreen();
		          //screen.setShowLoader(false);
		        }
		      };

		      try {
		          new Thread(runnable).start();
		        }
		        catch (Exception e) {
		          System.out.println(e.toString());
		          Server.showToast("SEDM Blackberry Server Exception (0x100B): " + e.toString());
		        }
		} else {
			Dialog.alert("Username/Password must be valid");
			//LoaderScreen screen = (LoaderScreen) UiApplication.getUiApplication().getActiveScreen();
	        //screen.setShowLoader(false);
		}
		
	}
	public void LoginProcess(final String username, final String password)
    {
		    userflag = 0;
            if ((username.length()!=0) && (password.length()!=0))
            {      
                    Runnable runnable = new Runnable() {
                           
                    public void run() {
                      //Server.checkEDT();
                      //Server.checkCoverage();
                           
                      try {                
                            final ByteBuffer permission = new ByteBuffer(Server.POST(downloadURL, new HttpPacket(null,HttpPacket.DOWNLOAD,null,null)));
                            //Server.showToast(permission.getString());
                            if ((permission.getBytes()[0] == 'O') && (permission.getBytes()[2] == 'K')) {
                                    EndConnection();
                                    final ByteBuffer randomNonce = new ByteBuffer(Server.POST(downloadURL, new HttpPacket(username.getBytes(),HttpPacket.LOGIN_1,null,null)));
                                    random = randomNonce.getBytes();
                                    //Server.showToast(randomNonce.getString());
                                    SEDMPacket data = new SEDMPacket();
                                    data.CreateDownloadPacket(randomNonce.getBytes(),
                                                                      password.getBytes("UTF-8")
                                                                     /*,(Integer.toString(
                                                                                     DeviceInfo.getDeviceId(),16)).toUpperCase()*/);
                                    final ByteBuffer downloadLink = new ByteBuffer(Server.POST(downloadURL, new HttpPacket(data.getPacket(),HttpPacket.LOGIN_2,key,IV)));
                                   
                                    if ((downloadLink.getBytes()[0] == 'O') &&
                                        (downloadLink.getBytes()[1] == 'K')) {
                                    	userflag = 9;
                                    	//errorDialog("Username & Password correct!");
                                    }
                                    else
                                    {
                                    	userflag = 1;
                                    	//errorDialog("Username & Password not correct!");
                                    	
                                    }
                            } else
                            {
                                userflag = 2;    
                            	//errorDialog("SEDM Blackberry Server Error (0x1001)");
                            	Server.showToast("SEDM Blackberry Server Error (0x1001)");
                            }
                      }
                      catch (final Exception e) {
                    	userflag = 3;
                        System.out.println(e.toString());
                        Server.showToast("SEDM Blackberry Server Exception (0x100A) : " + e.toString());
                    }
                   }
                  };

                  try {
                      new Thread(runnable).start();
                    }
                    catch (Exception e) {
                      userflag = 4; 	
                      System.out.println(e.toString());
                      Server.showToast("SEDM Blackberry Server Exception (0x100B): " + e.toString());
                    }
            } else {
                    Dialog.alert("Username/Password must be valid");
            }
           
    }

	
	public void EndConnection() {
		try {
			/* send packet to delete the directory in the server */
			Server.POST(downloadURL, new HttpPacket(random, HttpPacket.DELETE,null,null));
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		} catch (Exception e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
	}
}

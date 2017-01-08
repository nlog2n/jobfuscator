package com.i2r.utils;


import java.io.IOException;
import java.util.Vector;

import javax.microedition.content.ContentHandler;
import javax.microedition.content.ContentHandlerException;
import javax.microedition.content.Invocation;
import javax.microedition.content.Registry;

import net.rim.blackberry.api.browser.Browser;
import net.rim.blackberry.api.browser.BrowserSession;
import net.rim.device.api.i18n.ResourceBundle;
import net.rim.device.api.synchronization.UIDGenerator;
import net.rim.device.api.system.DeviceInfo;

import com.i2r.utils.log.Log;

import com.i2r.sedm.*;

public class Tools {

	
	public static synchronized void openFileWithExternalApp(String fileURL){
		
		ResourceBundle resourceBundle = SPDFReaderCore.getInstance().getResourceBundle();
		
		// Create the Invocation with the file URL
		Invocation invoc = new Invocation(fileURL);
		invoc.setResponseRequired(false); // We don't require a response
		invoc.setAction(ContentHandler.ACTION_OPEN);
		
		if(!fileURL.startsWith("file://")){
			Tools.openNativeBrowser(fileURL);
			return;
		} 
		
		// Get access to the Registry and pass it the Invocation
		Registry registry = Registry.getRegistry("com.i2r.utils.Tools");
		try {
			ContentHandler[] candidates = new ContentHandler[0];
			candidates = registry.findHandler(invoc);
		
			if(candidates.length == 0) {
				Log.trace("there is no ext-app that could open this file, using browser");
				Tools.openNativeBrowser(fileURL);
				return;
			} else if(candidates.length == 1) { //there is 1 only ext-app that could open this file
    			Log.trace("there is 1 only ext-app that could open this file");
    			invoc.setID(candidates[0].getID());
    			registry.invoke(invoc);
    			return;
    		}

			String[] appNames = new String[candidates.length];
			for (int i = 0; i < candidates.length; i++) {
				appNames[i] = candidates[i].getAppName();
			}
			String title = resourceBundle.getString(SPDFReaderResource.MENUITEM_OPEN_IN);
			SelectorPopupScreen selScr = new SelectorPopupScreen(title, appNames);
			selScr.pickItem();
			int selection = selScr.getSelectedItem();
			if(selection != -1) {
				invoc.setID(candidates[selection].getID());
				registry.invoke(invoc);                
			}
		} catch (Exception e) {
			Log.error(e, "Error while finding a chapi endpoint");
			Tools.openNativeBrowser(fileURL);
			return;
		}
	}
			
	
	/**
	 * Groups numbers by inserting 'separator' after every group of 'size' digits,
	 * starting from the right.
	 */
	
	public static synchronized String groupDigits(String value, int size, char separator) {

		StringBuffer r = new StringBuffer(value.length() + 10);
		int ndx = 0;
		int len = value.length() - 1;
		int mod = len % size;
		while (ndx < len) {
			r.append(value.charAt(ndx));
			if (mod == 0) {
				r.append(separator);
				mod = size;
			}
			mod--;
			ndx++;
		}
		r.append(value.charAt(ndx));
		return r.toString();
	}
	
	
	/**
	 * Finds installed Browsers and asks user which one should be used to open the page
	 */
	public static synchronized void openURL(String url){
		
		ResourceBundle resourceBundle = /*SPDFReaderCore.getInstance().getResourceBundle();*/ResourceBundle.getBundle(SPDFReaderResource.BUNDLE_ID, SPDFReaderResource.BUNDLE_NAME);
		
		// Create the Invocation with the file URL
		Invocation invoc = new Invocation(url);
		invoc.setResponseRequired(false); // We don't require a response
		invoc.setType("text/html");   //FIXME: this is not correct
		invoc.setAction(ContentHandler.ACTION_OPEN);
		
		if(!url.startsWith("http")){
			Tools.openNativeBrowser(url);
			return;
		} 
		
		// Get access to the Registry and pass it the Invocation
		Registry registry = Registry.getRegistry("com.i2r.utils.Tools");		
		try {
			ContentHandler[] candidates = new ContentHandler[0];
			candidates = registry.findHandler(invoc);
			
			//check the #s of apps that can handle html file
			if(candidates.length == 0) {
				Tools.openNativeBrowser(url);
				return;
			}
					
			//remove the net.rim.bb.mediacontenthandler from content handler list
			Vector newCandidates = new Vector();
			for (int i = 0; i < candidates.length; i++) {
				if(candidates[i].getID().equalsIgnoreCase("net.rim.bb.mediacontenthandler")) {
					
				} else { 
					newCandidates.addElement(candidates[i]);
				}
			}
			
			candidates = new ContentHandler[newCandidates.size()];
			newCandidates.copyInto(candidates);

			if(candidates.length == 0) {
				Tools.openNativeBrowser(url);
				return;
			}

			//add the native browser at the end of the list	of available applications	
			String[] appNames = new String[candidates.length+1];
			for (int i = 0; i < appNames.length-1; i++) {
				appNames[i] = candidates[i].getAppName();
			}
			appNames[appNames.length-1] = resourceBundle.getString(SPDFReaderResource.NATIVE_BROWSER);
				
			String title = resourceBundle.getString(SPDFReaderResource.MENUITEM_OPEN_IN);
			SelectorPopupScreen selScr = new SelectorPopupScreen(title, appNames);
			selScr.pickItem();
			int selection = selScr.getSelectedItem();
			if(selection != -1) {
				if(selection < appNames.length-1) {
					invoc.setID(candidates[selection].getID());
					registry.invoke(invoc);
				} else {
					//native browser
					Tools.openNativeBrowser(url);
				}
			}
	
		} catch (Exception e) {
			Log.error(e, "Error while finding a chapi endpoint");
			Tools.openNativeBrowser(url);
			return;
		}
	}	
	
	
	public static synchronized BrowserSession openNativeBrowser(String URL) {
		// Get the default sessionBrowserSession
    	BrowserSession browserSession = Browser.getDefaultSession();
    	// now launch the URL
    	browserSession.displayPage(URL);
    	// The following line is a work around to the issue found in
    	// version 4.2.0
    	browserSession.showBrowser();
		return browserSession;
	}
	

	public static synchronized String getAppVersion() {
    	String version = PropertyUtils.getAppVersion(); //read from the alx files
        if(version == null || version.trim().equals("")) { //read value from jad file
        	//MIDlet-Version
        	version = PropertyUtils.getIstance().get("MIDlet-Version");
        	if(version == null)
        		version = "";
        }
    	Log.debug("App version: "+version);
        return version;
	}
	
	/**
	 * check the path of the file xmlrpc.php into the url string
	 */
	public static String checkURL(String url){
		Log.trace(">>> checkURL");
		Log.trace("in URL: "+url);
		if(url == null || url.trim().length() == 0 ) {
			return null;
		}
			
		if (url.endsWith("xmlrpc.php")){
			
		} else {
			if (!url.endsWith("/")){
				url+="/";
			}
			url+="xmlrpc.php";
		}
		Log.trace("out URL: "+url);	
		return url;
	}
	
  public static double round(final double num) {
	    final double floor = Math.floor(num);
	    if (num - floor >= 0.5) {
	      return Math.ceil(num);
	    } else {
	      return floor;
	    }
	  }
	
  /*
   * This method will return the PIN number of a device 
   * or the default value of 0x2100000a if connected to the simulator.
   * 
   * note: You can modify the default value associated with the simulator by using the PIN command line option. 
   * note2: Formatting the value as in the phone option screen: String uniquePart1 = Integer.toHexString(DeviceInfo.getDeviceId());
   * 
   */
	public static int generateDeviceUUID() {
		return DeviceInfo.getDeviceId();	 
		//return UIDGenerator.makeLUID(UIDGenerator.getUniqueScopingValue(), UIDGenerator.getUID());
	}
	
	 public static int decodeInt(Object input){	 
		 if (input instanceof String){
			 return Integer.parseInt((String)input);
		 } else if (input instanceof Integer){
			 return ((Integer)input).intValue();			 
		 } else if (input instanceof Boolean) {
			 if (((Boolean)input).booleanValue())
				 return 1;
			 else 
				 return 0;
		 }
		 return -1;		 
	 }
	 
	 public static String decodeString(Object input){
		 if(input == null ) return null;
		 if (input instanceof String)
			 return (String) input;
		  else 
			 return String.valueOf(input);			 			 
	 }
	
	  /**
	   * Convert a vector to a string array.
	   * 
	   * @param v
	   *          vector to convert
	   * @return the string array
	   */
	  public static String[] toStringArray(final Vector v) {
	    final String[] res = new String[v.size()];
	    v.copyInto(res);
	    return res;
	  }

}

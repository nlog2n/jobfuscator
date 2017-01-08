package com.i2r.sedm;

import java.io.IOException;
import java.util.Timer;
import java.util.Vector;

import javax.microedition.io.file.FileSystemListener;
import javax.microedition.rms.RecordStoreException;

import net.rim.device.api.i18n.ResourceBundle;
import net.rim.device.api.system.Bitmap;
import net.rim.device.api.ui.UiApplication;

import com.i2r.utils.log.FileAppender;
import com.i2r.utils.log.Log;

public class SPDFReaderCore {
	
	private FileAppender fileAppender = null;
	private MySDListener sdCardListener = null;
	private Timer timer = null;
	private Vector applicationBlogs = new Vector();
	
	private String lastFileBrowserPath = null; //store the last path opened in the file browser
		
	//create a variable to store the ResourceBundle for localization support
    private final ResourceBundle _resources;

	private static SPDFReaderCore instance;
	
	private SPDFReaderCore() {
		Log.debug("SPDFReaderCore initializated");
		sdCardListener = new MySDListener();
		UiApplication.getUiApplication().addFileSystemListener(sdCardListener);
		timer = new Timer();
		_resources = ResourceBundle.getBundle(SPDFReaderResource.BUNDLE_ID, SPDFReaderResource.BUNDLE_NAME);
	}
	
	public static SPDFReaderCore getInstance() {
		if (instance == null) {
			instance = new SPDFReaderCore();
		}
		return instance;
	}
	
	public ResourceBundle getResourceBundle() {
		return _resources;
	}

	private class MySDListener implements FileSystemListener {
		public void rootChanged(int state, String rootName) {
			if( state == ROOT_ADDED ) {
				if( rootName.equalsIgnoreCase("sdcard/") ) {
					Log.trace("microSD card inserted");
				}
			} else if( state == ROOT_REMOVED ) {
				Log.trace("microSD card removed");
				boolean needClose = true;
				try {
					//if storage is not set on SD card
					if(!SPDFReaderDAO.SD_STORE_PATH.equalsIgnoreCase(SPDFReaderDAO.getBaseDirPath())) {
						needClose = false;
					 }
				} catch (RecordStoreException e) {
					Log.error(e, "Error reading RMS");
				} catch (IOException e) {
					Log.error(e, "Error reading RMS");
				} finally {
					//close the app only if needed
					if(needClose)
						exitSPDFReader();
				}
			}
		}
	}
	
	public void exitSPDFReader() {	
		Log.debug("closing app...");
		UiApplication.getUiApplication().removeFileSystemListener(sdCardListener);
		timer.cancel(); //cancel the timer
		System.exit(0);
	}
	
	public FileAppender getFileAppender() {
		return fileAppender;
	}
	
	public void setFileAppender(FileAppender fileAppender) {
		this.fileAppender = fileAppender;
	}
	

	/**
	 * Screen dimensions
	 * 
	 * Pearl 8220 - 240 x 320 pixels
	 * Curve 8300 Series, 8800 Series, 8700 Series - 320 x 240 pixels
	 * Curve 8350i - 320 x 240 pixels
	 * Curve 8900 - 480 x 360 pixels
	 * Bold  9000 Series - 480 x 320 pixels
	 * Tour  9600 Series - 480 x 360 pixels
	 * Storm 9500 Series - portrait view: 360 x 480 pixels,  landscape view: 480 x 360 pixels
	 * 
	 */
	public Bitmap getBackgroundBitmap() {
		return Bitmap.getBitmapResource("bg.png");
/*		 int width = Display.getWidth(); 
		 int height = Display.getHeight();
		 
		 if(width == 240 && height == 320 ) {
			 
		 } else if(width == 320 && height == 240) {
			 
		 } else if(width == 480 && height == 320) { 
			 
		 } else if(width == 480 && height == 360) {
			 
		 } else if(width == 360 && height == 480) {
			 
		 } else {
			 
		 }

		return null;*/
	}

	public Timer getTimer() {
		return timer;
	}

	public String getLastFileBrowserPath() {
		return lastFileBrowserPath;
	}

	public void setLastFileBrowserPath(String lastFileBrowserPath) {
		this.lastFileBrowserPath = lastFileBrowserPath;
	}	

}

/**
 * 
 */
package com.i2r.sedm;

import java.io.*;

import javax.microedition.content.*;
import javax.microedition.io.*;
import javax.microedition.io.file.*;

import com.i2r.utils.File.JSR75FileSystem;
import com.i2r.utils.log.*;

import net.rim.device.api.content.DefaultContentHandlerRegistry;
import net.rim.device.api.i18n.ResourceBundle;
import net.rim.device.api.system.*;
import net.rim.device.api.ui.*;
import net.rim.device.api.ui.component.*;
import net.rim.device.api.ui.container.*;

/**
 * @author zzhao
 * 
 */
public class SPDFReader extends UiApplication implements RequestListener,
		ResponseListener, SPDFReaderResource {
    private static ResourceBundle _resources;
    private static Appender logAppender  = null;

    private boolean isSDCardNotFound = false;    
    
	static String CHAPI_ID = "com.i2r.sedm";
	static String CHAPI_CLASS_NAME = CHAPI_ID+".SPDFReader";
	
	static String SPDF_TYPE = "application/spd";
	static String SPDF_SUF = ".spd";
	static String SPDF_HANDLER = "SPDFHandler";
	
	
	/** System event GUID */
    public final static long APPLICATION_ID = 0xcc9ff4807528baa0L; //com.i2r.sedm.SPDFReader
	
	private static String[] TYPES = { SPDF_TYPE };
	private static String[] SUFFIXES = { SPDF_SUF };
	private static String[] ACTIONS = { ContentHandler.ACTION_OPEN };
	private static String HANDLERNAME = SPDF_HANDLER;

	boolean flagAccountExists = false;
	String userName;
	String passWord;
	LoginScreen loginScreen;
	SPDFReaderScreen readerScreen;
	String currentSPDFFile = null;
	SecurePDF my_wrapper; 

    static {
        //retrieve a reference to the ResourceBundle for localization support
        _resources = ResourceBundle.getBundle(SPDFReaderResource.BUNDLE_ID, SPDFReaderResource.BUNDLE_NAME);
    }	
	/**
	 * Entry point for application
	 * 
	 * @param args
	 *            Command line arguments
	 */
	public static void main(String[] args) {
   	try {
    		//Open the RuntimeStore.
    		RuntimeStore store = RuntimeStore.getRuntimeStore();
    		//Obtain the reference to SPDFReader for BlackBerry.
    		final Object obj = store.get(APPLICATION_ID);
    		//If obj is null, there is no current reference
    		//to SPDFReader for BlackBerry
    		if (obj == null) {
    			System.out.println("RecordStore is void, launching new app");
    			SPDFReader app = new SPDFReader(args);
    			app.enterEventDispatcher();
    		} else {
    			System.out.println("RecordStore is NOT void, foreground the app");
    			((SPDFReader)obj).requestForeground();
    			System.exit(0);
    		}
    	} catch (Exception e) {
    		SPDFReader app = new SPDFReader(args);
    		app.enterEventDispatcher();
    		System.err.println("error reading appstore: " + e.getMessage());
    	} finally {

    	}
/*
		if (args != null && args.length > 0) {
			if (args[0].equals("startup")) {

				SPDFReader app = new SPDFReader(args);
				app.RemovePDF();

				// Register this application as a content handler on startup
				register();
				System.exit(0);
			}
		} else {
			// Create a new instance of the application and make the currently
			// running thread the application's event dispatch thread.
			SPDFReader app = new SPDFReader(args);
			app.loadApp();
			app.enterEventDispatcher();
		}	
*/		
			
	}

	/**
	 * 
	 */
	public SPDFReader(String[] args) {
		// TODO Auto-generated constructor stub
		
		//When device is in startup check the startup variable
		
		ApplicationManager myApp = ApplicationManager.getApplicationManager();
		if (myApp.inStartup()) {
			doAutoStart();
		} else {
			initLog();
			Log.trace("==== User Start Mode ====");
			register();
        	loadApp();
		}
		
	}	
	/**
	 * Registers this application as a content handler for image, video, and
	 * audio files
	 */
	private static void register() {

		// Get access to the registry

		try {
			Registry registry = Registry.getRegistry(CHAPI_CLASS_NAME);
			ContentHandler registered = registry.forID(CHAPI_ID, true);
			if (registered != null)
			{
				return;
			}			
			// Register as a content handler
			registry.register(CHAPI_CLASS_NAME, TYPES, SUFFIXES, ACTIONS, null, CHAPI_ID,
					null);
		} catch (ContentHandlerException che) {
			Log.error("SPDFReader#register() threw " + che.toString());
		} catch (ClassNotFoundException cnfe) {
			System.out
					.println("SPDFReader#register() threw " + cnfe.toString());
		}
	}

	public void RemovePDF() {
		RecordManager rm = new RecordManager();
		String[] strs = rm.Read("PDFFileURL");
		int i = strs.length - 1;
		try {
			while (i >= 0) {
				if (strs[i] != null && strs[i].length() > 0) {
				//	Dialog.alert("delete: " + strs[i]);
					//DeleteFile(strs[i]);
					
					//remove directory with temp PDF file
					JSR75FileSystem.removeFile(strs[i].substring(0,strs[i].lastIndexOf('/') + 1));
				    Log.info("RemovePDF: " + strs[i]);
				}
				i--;
			}
		} catch (Exception ex) {
			Log.error("SPDFReader#RemovePDF() threw " + ex.toString());
		} finally {
			String[] strs2 = new String[16];
			int j = 0;
			int k = 0;
			while (j <= i && j < 16) {
				if (strs[j] != null && strs[j].length() > 0) {
					strs2[k] = strs[j];
					k++;
				}
				j++;
			}
			rm.Save(strs2, "PDFFileURL");
		}
	}

	public void InvokeSPDF() {
		if (!flagAccountExists) {
			popScreen(getActiveScreen());
			pushScreen(loginScreen);
		} else {
			popScreen(getActiveScreen());
			pushScreen(readerScreen);
			if (currentSPDFFile != null) {
				readerScreen.InvokeSPDF();
			}
		}
	}

	/**
	 * Presents a dialog to the user with a given message
	 * 
	 * @param message
	 *            The text to display
	 */
	public void errorDialog(final String message) {
		UiApplication.getUiApplication().invokeLater(new Runnable() {
			public void run() {
				Dialog.alert(message);
			}
		});
	}



	 /**
     * Method to execute in autostart mode.
     */
    private void doAutoStart() {
		
    	invokeLater(new Runnable()
        {
            public void run()
            {
                ApplicationManager myApp = ApplicationManager.getApplicationManager();
                boolean keepGoing = true;
                while (keepGoing)
                {
                    if (myApp.inStartup())
                    {
                        try { Thread.sleep(1000); }
                        catch (Exception ex) { }
                    }
                    else
                    {
                    	// The BlackBerry has finished its startup process
                    	initLog();
        				RemovePDF();

        				// Register this application as a content handler on startup
        				register();
        				keepGoing = false;
        				System.exit(0);
                   	
                    }//end else
                }//end while
            }
        });
    }
    
    //init the log system
	private void initLog() {
		Appender eventAppender = new BlackberryEventLogAppender("SPDFReader for BlackBerry");
		eventAppender.setLogLevel(Log.ERROR);
		eventAppender.open();
		Log.addAppender(eventAppender);
		
		//#ifdef LOG_CONSOLE
		Log.addAppender(new ConsoleAppender());
		//#endif
		
		Log.initLog(Log.TRACE);		
		Log.trace("==== SPDFReader for BlackBerry Startup ====");

	}
	
	public void loadApp() {
		
		SPDFReaderPref appPrefs = SPDFReaderPref.getIstance();
		//check application permission as first step
		SPDFReaderAppPermissions.getIstance().checkPermissions();

		try {
			String baseDirPath = SPDFReaderDAO.getBaseDirPath(); //read the base dir path
			//first startup
			if(baseDirPath == null) {
				appPrefs.isFirstStartupOrUpgrade = true; //set as first startup.
				if(JSR75FileSystem.supportMicroSD() && JSR75FileSystem.hasMicroSD()) {
					SPDFReaderDAO.setBaseDirPath(SPDFReaderDAO.SD_STORE_PATH);
				} else {
					SPDFReaderDAO.setBaseDirPath(SPDFReaderDAO.DEVICE_STORE_PATH); 
				}
			} else {
				//set as no first  startup.
				appPrefs.isFirstStartupOrUpgrade = false; 

				//checking if storage is set to SDcard, then verify the presence of sd card into phone
				if(baseDirPath.equals(SPDFReaderDAO.SD_STORE_PATH)) {
					if(JSR75FileSystem.supportMicroSD() && JSR75FileSystem.hasMicroSD()) {
						//ok
					} else {
						//microSD not present. set the storage to memory device
						isSDCardNotFound = true;
						SPDFReaderDAO.setBaseDirPath(SPDFReaderDAO.DEVICE_STORE_PATH); 
						baseDirPath = null;
					}
				}
			}

			SPDFReaderDAO.setUpFolderStructure(); //check for the folder existence, create it if not exist
			SPDFReaderDAO.readApplicationPreferecens(appPrefs); //load pref on startup
			
			//add the file log appender
			FileAppender fileAppender = new FileAppender(baseDirPath, SPDFReaderDAO.LOG_FILE);
			fileAppender.setLogLevel(Log.INFO); //if we set level to TRACE the file log size grows too fast
			fileAppender.open();
			Log.addAppender(fileAppender);
			SPDFReaderCore spCore = SPDFReaderCore.getInstance();
			spCore.setFileAppender(fileAppender); // add the file appender to the queue
						
		} catch (Exception e) {
			final String excMsg;
			
			if(e != null && e.getMessage()!= null ) {
				excMsg = "\n" + e.getMessage();
			} else {
				excMsg = "\n" + "Please configure application permissions and reboot the device by removing and reinserting the battery.";
			}
			}
	
		try {
			// Get access to the registry
			Registry registry = Registry.getRegistry(CHAPI_CLASS_NAME);

			// Get access to the ContentHandlerServer for this application and
			// register as a listener.
			ContentHandlerServer contentHandlerServer = Registry
					.getServer(CHAPI_CLASS_NAME);
			contentHandlerServer.setListener(this);

			// Set the name of the content handler by updating the
			// ApplicationDescriptor
			DefaultContentHandlerRegistry defaultRegistry = DefaultContentHandlerRegistry
					.getDefaultContentHandlerRegistry(registry);
			ApplicationDescriptor currentDescriptor = ApplicationDescriptor
					.currentApplicationDescriptor();

			ApplicationDescriptor descriptor = new ApplicationDescriptor(
					currentDescriptor, HANDLERNAME, null);
			defaultRegistry.setApplicationDescriptor(descriptor, CHAPI_ID);

			loginScreen = new LoginScreen();
			readerScreen = new SPDFReaderScreen();

			
			  if (!flagAccountExists) { 
				  pushScreen(loginScreen); 
				 } else {
			  pushScreen(readerScreen); 
			  }
			  requestForeground();
			 
				String aText;
				my_wrapper = new SecurePDF();
				if (my_wrapper.checkDevice()){       
					aText = " successfully verified.";
					//errorDialog(aText);
				}else{
					aText = " verification failed! Exit...";
					errorDialog(aText);
					//System.exit(0);
				}			
				
		} catch (ContentHandlerException che) {
			System.out.println("SPDFReader#SPDFReader() threw "
					+ che.toString());
			errorDialog("SPDFReader#SPDFReader() threw " + che.toString());
		}
	}

	/**
	 * RequestListener implementation
	 * 
	 * @param server
	 *            The content handler server from which to request Invocation
	 *            objects
	 * 
	 * @see javax.microedition.content.RequestListener#invocationRequestNotify(ContentHandlerServer)
	 */
	public void invocationRequestNotify(ContentHandlerServer server) {
		// Retrieve Invocation from the content handler server
		Invocation invoc = server.getRequest(false);

		if (invoc == null) {
			return; // Nothing to do
		}

		int invocationStatus = invoc.getStatus();
		
		currentSPDFFile = invoc.getURL();
		InvokeSPDF();
		invocationStatus = Invocation.OK;
		server.finish(invoc, invocationStatus);
		
		
/*		
		String type = invoc.getType();
		Log.info("invoc.getType():" + invoc.getType());
		if(type.equals(SPDF_TYPE))
		{
		currentSPDFFile = invoc.getURL();
		InvokeSPDF();
		invocationStatus = Invocation.OK;
		server.finish(invoc, invocationStatus);
		}
		else
		{
			invocationStatus = Invocation.ERROR;
			server.finish(invoc, invocationStatus);
		}
*/		
	}

	void DeleteFile(String filename) throws IOException {
		try {
			FileConnection fileToDelete = (FileConnection) Connector
					.open(filename);
			if (fileToDelete.exists()) {
				fileToDelete.delete();
				fileToDelete.close();
			} else {
				errorDialog("File " + fileToDelete.getName()
						+ " does not exists");
			}
		} catch (Exception ex) {
			errorDialog("SPDFReader#DeleteFile() threw " + filename + ": " + ex);
			throw new IOException("SPDFReader#DeleteFile() threw " + filename
					+ ": " + ex);
		}
	}

	public void invocationResponseNotify(Registry registry) {
		// TODO Auto-generated method stub
		String alertText = null;
		//Dialog.alert("invocationResponseNotify is called.");
		Log.trace("invocationResponseNotify is called.");

		try {
			Invocation invocation = registry.getResponse(false);

			switch (invocation.getStatus()) {
			case Invocation.ACTIVE:
				Log.trace("CHAPI response = Active");
				alertText = "ACTIVE";
				break;

			case Invocation.CANCELLED:
				Log.trace("CHAPI response = Canceled");
				byte[] data = invocation.getData();
				alertText = "Status:" + "CANCELLED";
				alertText += "Reason:" + "[0]" + data[0] + "[1]" + data[1];
				alertText += "URL:" + invocation.getURL();
				break;

			case Invocation.ERROR:
				Log.error("CHAPI response = Error");
				alertText = "ERROR";
				RemovePDF();
				System.exit(0);
				break;

			case Invocation.HOLD:
				Log.trace("CHAPI response = Hold");
				alertText = "HOLD";
				break;

			case Invocation.INIT:
				Log.trace("CHAPI response = Init");
				alertText = "INIT";
				break;

			case Invocation.INITIATED:
				Log.trace("CHAPI response = Initialized");
				alertText = "INITIATED";
				break;

			case Invocation.OK:
				Log.info("CHAPI response = Ok");
				alertText = "Status:" + "OK";
				alertText += "URL:" + invocation.getURL();
				//RemovePDF();
				Log.info("URL:" + invocation.getURL());
				System.exit(0);
				break;

			case Invocation.WAITING:
				Log.trace("CHAPI response = Waiting");
				alertText = "WAITING";
				break;

			default:
				Log.trace("CHAPI response = Unknown");
				break;
			}
		} catch (Exception e) {
			e.printStackTrace();
		}
		Log.info(alertText);

	//	Dialog.alert(alertText);
	}

}

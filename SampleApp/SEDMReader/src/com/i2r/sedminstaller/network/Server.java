package com.i2r.sedminstaller.network;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;

import javax.microedition.io.Connector;
import javax.microedition.io.HttpConnection;

import com.i2r.sedminstaller.util.ByteBuffer;

import net.rim.device.api.system.Application;
import net.rim.device.api.system.CoverageInfo;
import net.rim.device.api.system.RadioInfo;
import net.rim.device.api.ui.UiApplication;
import net.rim.device.api.ui.component.Status;

public class Server {
	private static ByteBuffer bb;
	/*private static HttpConnection _httpConnection;
	private static OutputStream _httpOutputStream;
	private static InputStream _httpInputStream;
	*/
	
	public static ByteBuffer POST(String uri, HttpPacket data) throws Exception {
		  // create the connection...
		  HttpConnection _connection = (HttpConnection) Connector.open(uri, Connector.READ_WRITE, true);

		  // Set the request method and headers
		  {
		    _connection.setRequestMethod(HttpConnection.POST);
		    _connection.setRequestProperty("If-Modified-Since",
		                                   "29 Oct 1999 19:43:31 GMT");
		    _connection.setRequestProperty("User-Agent",
		                                   "Profile/MIDP-2.0 Configuration/CLDC-1.0");
		    _connection.setRequestProperty("Content-Language", "en-US");
		  }

		  OutputStream _outputStream = _connection.openOutputStream();

		  _outputStream.write(data.getPacket());
		  _outputStream.flush(); // Optional, getResponseCode will flush

		  // Getting the response code will open the connection, send the request, and read the HTTP response headers.
		  // The headers are stored until requested.
		  {
		    int rc = _connection.getResponseCode();
		    if (rc != HttpConnection.HTTP_OK) {
		      throw new IOException("HTTP response code: " + rc);
		    }
		  }

		  // Getting the response code will open the connection, send the request, and read the HTTP response headers.
		  // The headers are stored until requested.
		  {
		    int rc = _connection.getResponseCode();
		    if (rc != HttpConnection.HTTP_OK) {
		      throw new IOException("HTTP response code: " + rc);
		    }
		  }

		  // get the data from the service
		  {
		    InputStream _inputStream = _connection.openInputStream();
		    bb = new ByteBuffer(_inputStream);

  	      // close everything out
		    {
		      if (_inputStream != null) try {_inputStream.close();}catch (Exception e) {}
		      if (_outputStream != null) try {_outputStream.close();}catch (Exception e) {}
		      if (_connection != null) try {_connection.close();}catch (Exception e) {}

		    }
		    
		    return bb;
		  }
		}
	
	
	/*public static void openConnection(String uri) throws IOException {
		_httpConnection = (HttpConnection) Connector.open(uri, Connector.READ_WRITE, true);

		  // Set the request method and headers
		  {
		    _httpConnection.setRequestMethod(HttpConnection.POST);
		    _httpConnection.setRequestProperty("If-Modified-Since",
		                                   "29 Oct 1999 19:43:31 GMT");
		    _httpConnection.setRequestProperty("User-Agent",
		                                   "Profile/MIDP-2.0 Configuration/CLDC-1.0");
		    _httpConnection.setRequestProperty("Content-Language", "en-US");
		  }
		  
		  _httpOutputStream = _httpConnection.openOutputStream();
		  _httpInputStream = _httpConnection.openInputStream();
	}
	
	public static void closeConnection() throws IOException {
		if (_httpConnection != null) try {_httpConnection.close();}catch (Exception e) {}
		if (_httpInputStream != null) try {_httpInputStream.close();}catch (Exception e) {}
	    if (_httpOutputStream != null) try {_httpOutputStream.close();}catch (Exception e) {}
	    
	}
	
	public static ByteBuffer POST(byte[] data) throws IOException {
		
		  _httpOutputStream.write(data);
		  _httpOutputStream.flush(); // Optional, getResponseCode will flush

		  // Getting the response code will open the connection, send the request, and read the HTTP response headers.
		  // The headers are stored until requested.
		  {
		    int rc = _httpConnection.getResponseCode();
		    if (rc != HttpConnection.HTTP_OK) {
		      throw new IOException("HTTP response code: " + rc);
		    }
		  }

		  // Getting the response code will open the connection, send the request, and read the HTTP response headers.
		  // The headers are stored until requested.
		  {
		    int rc = _httpConnection.getResponseCode();
		    if (rc != HttpConnection.HTTP_OK) {
		      throw new IOException("HTTP response code: " + rc);
		    }
		  }

		  // get the data from the service
		  {
		    
		    bb = new ByteBuffer(_httpInputStream);
		    
		    return bb;
		  }
	}*/
		  
		  
	public static  void checkEDT() {
		  if (Application.isEventDispatchThread())
		    throw new IllegalStateException("Can't call this method in the EDT.");
		}
	  
	public static void checkCoverage() {
		  boolean _outOfCoverage = false;

		  if (!CoverageInfo.isOutOfCoverage()) {
		    // check to see if the available coverage is of sufficient strength
		    _outOfCoverage = !(
		        CoverageInfo.isCoverageSufficient(RadioInfo.WAF_WLAN) ||
		        CoverageInfo.isCoverageSufficient(RadioInfo.WAF_3GPP) ||
		        CoverageInfo.isCoverageSufficient(RadioInfo.WAF_CDMA) ||
		        CoverageInfo.isCoverageSufficient(RadioInfo.WAF_IDEN)
		    );

		    if (_outOfCoverage) {
		      showToast("In network coverage, but insufficient strength");
		    }
		    else {
		      showToast("In network coverage");
		    }
		  }
		  else {
		    // definitely out of network coverage
		    _outOfCoverage = true;
		    showToast("Out of network coverage");
		  }
		  if (_outOfCoverage) throw new IllegalArgumentException("Out of network coverage");
		}
	  

	public static void showToast(final String msg) {
		  UiApplication.getApplication().invokeLater(new Runnable() {
		    public void run() {
		      Status.show(msg);
		    }
		  });
	  }

}

package com.i2r.sedminstaller.network;

import java.io.IOException;
import java.io.OutputStream;

import javax.microedition.io.Connector;
import javax.microedition.io.HttpConnection;
import javax.microedition.io.HttpsConnection;

import net.rim.device.api.io.http.HttpHeaders;
import net.rim.device.api.io.http.HttpProtocolConstants;
import net.rim.device.api.system.Application;
import net.rim.device.api.system.CoverageInfo;
import net.rim.device.api.system.RadioInfo;
import net.rim.device.api.ui.UiApplication;
import net.rim.device.api.ui.component.Dialog;
import net.rim.device.api.ui.component.Status;
import net.rim.device.api.util.StringUtilities;

public class HttpUtils {
	  public static final int CONNECTION_DEFAULT = 0;
	  public static final int CONNECTION_BIS = 1;
	  public static final int CONNECTION_BES = 2;
	  public static final int CONNECTION_TCPIP = 3;
	  public static final int CONNECTION_WIFI = 4;

	  /**
	   * This method opens a HTTP connection to the given url. The method used is
	   * GET or POST depending on whether postData is null or not. Only the
	   * provided connType is used. For example, if the connType is
	   * CONNECTION_BES, the connection is tried using the BES only.
	   * The only time provided connection type is not used is when the URL
	   * contains ";deviceside=".
	   *
	   * @param url            The url to connect to.
	   * @param requestHeaders The headers in the request. May be null or empty.
	   * @param postData       Data to be posted to the server. If null, the GET method used
	   *                       for the http connection.
	   * @param connType       The type of transport (BES / BIS / WIFI / Default) to be used
	   *                       for opening connection.
	   *
	   * @return Opened HttpConnection object or null if some error occurs.
	   */
	  public static HttpConnection makeHttpConnection(String url,
	                                                  HttpHeaders requestHeaders, byte[] postData, int connType)
	  {
	    HttpConnection conn = null;
	    OutputStream out = null;

	    if (StringUtilities.startsWithIgnoreCase(url, "www.")) {
	      url = "http://" + url;
	    }

	    try {
	      if (url.indexOf(";deviceside=") == -1) {
	        switch (connType) {
	          case CONNECTION_BES:
	            url = url + ";deviceside=false";
	            break;
	          case CONNECTION_BIS:
	            url = url + ";XXXXXXXXXXXXXXXX";
	            break;
	          case CONNECTION_TCPIP:
	            url = url + ";deviceside=true";
	            break;
	          case CONNECTION_WIFI:
	            url = url + ";interface=wifi";
	        }
	      }

	      conn = (HttpConnection) Connector.open(url);

	      if (requestHeaders != null) {
	        String referer = requestHeaders.getPropertyValue("referer");

	        boolean sendReferrer = true;

	        if (referer != null &&

	            StringUtilities.startsWithIgnoreCase(referer, "https:") &&

	            !StringUtilities.startsWithIgnoreCase(url, "https:"))
	        {
	          sendReferrer = false;
	        }

	        int size = requestHeaders.size();
	        for (int i = 0; i < size;) {
	          String header = requestHeaders.getPropertyKey(i);
	          // remove header if needed
	          if (!sendReferrer && header.equals("referer")) {
	            requestHeaders.removeProperty(i);
	            --size;
	            continue;
	          }

	          String value = requestHeaders.getPropertyValue(i++);
	          if (value != null) {
	            conn.setRequestProperty(header, value);
	          }
	        }
	      }

	      if (postData == null) {
	        conn.setRequestMethod(HttpConnection.GET);
	        conn.setRequestProperty("User-Agent",

	                                "Profile/MIDP-2.0 Configuration/CLDC-1.0");
	      }
	      else {
	        conn.setRequestMethod(HttpConnection.POST);
	        conn.setRequestProperty(

	            HttpProtocolConstants.HEADER_CONTENT_LENGTH,

	            String.valueOf(postData.length));
	        conn.setRequestProperty("Content-Type",
	                                "application/x-www-form-urlencoded");
	        conn.setRequestProperty("User-Agent",
	                                "Profile/MIDP-2.0 Configuration/CLDC-1.0");

	        out = conn.openOutputStream();
	        out.write(postData);
	        out.flush();
	      }
	    }
	    catch (IOException e1) {
	      Dialog.alert("UTIL.HTC " + e1);
	      close(conn, null); // Close the connection

	      conn = null;
	    }
	    finally {
	      close(null, out); // Close the output, but keep connection open
	    }

	    return conn;
	  }

	  private static void close(HttpConnection con, OutputStream out) {
	    if (out != null) {
	      try {
	        out.close();
	      }
	      catch (IOException e2) {
	      }
	    }
	    if (con != null) {
	      try {
	        con.close();
	      }
	      catch (IOException e) {
	      }
	    }
	  }
	  
	}
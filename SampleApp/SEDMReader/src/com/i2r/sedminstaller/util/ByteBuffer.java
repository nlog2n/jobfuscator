package com.i2r.sedminstaller.util;

import java.io.ByteArrayInputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.UnsupportedEncodingException;

public class ByteBuffer {

	/* constants */
	public static final int BUFFER_SIZE = 512;
	
	/* jvm constants for string encoding */
	public static final String ASCII = "US-ASCII";
	public static final String UTF8 = "UTF-8";
	public static final String DEFAULT_CHAR_ENCODING = UTF8;

	/* data */
	protected byte[] byteRay = null;
	protected String enc = DEFAULT_CHAR_ENCODING;

	/* constructors */
	public ByteBuffer() {
	}

	public ByteBuffer(byte[] srcBuf) {
	  append(srcBuf);
	}

	public ByteBuffer(ByteBuffer bb) {
	  append(bb);
	}

	/**
	 * this method does not close the InputStream. 
	 * Simply reads all data on the stream into a byte
	 * buffer.
	 */
	public ByteBuffer(InputStream is) throws IOException {
	  byte[] readBuf = new byte[BUFFER_SIZE];
	  while (true) {
	    int read = is.read(readBuf);
	    if (read == -1) break;
	    append(readBuf, 0, read);
	  }
	}
	public ByteBuffer(String s) throws IllegalArgumentException {
	  append(s.getBytes());
	}

	/* pure functionality of the class */
	public ByteBuffer append(
	    byte[] srcBuf, int srcStartIndex, int srcLength)
	{
	  if (byteRay == null) {
	    //create a new array
	    byteRay = new byte[srcLength];
	    //copy the src array to the new one
	    /*
	    System.out.println(
	      "byteRay.length="+byteRay.length +
	      ",srcBuf.length="+srcBuf.length +
	      ",srcStartIndex="+srcStartIndex +
	      ",srcLength="+srcLength );
	    */
	    arrayCopy(
	        srcBuf, srcStartIndex, byteRay, 0, srcLength);
	  }
	  else {
	    int currentSize = byteRay.length;
	    //create a new array (which is bigger than existing one)
	    byte[] newByteRay = new byte[currentSize + srcLength];
	    //copy the old (internal) array into the new one
	    arrayCopy(byteRay, 0, newByteRay, 0, currentSize);
	    //copy the src array into the new one
	    int newByteRayStartIndex = currentSize;
	    arrayCopy(
	        srcBuf, srcStartIndex,
	        newByteRay, newByteRayStartIndex,
	        srcLength);
	    //now blow away the old internal byte array with the bigger one
	    byteRay = newByteRay;
	  }

	  return this;
	}

	public byte[] toByteArray() {
	  return getBytes();
	}

	/**
	 * This method simply returns the size of this object in KB.
	 * This is <b>not</b> the same as {@link #getString()}, which should be used if you're trying to encode the bytes to
	 * string form.
	 */
	public String toString() {
	  if ((byteRay != null) && (byteRay.length > 0)) {
	    float sizeInKB = byteRay.length / 1024f;
	    return sizeInKB + " KB";
	  }
	  else {
	    return "0 KB";
	  }
	}

	/*
	public String toString() {
	  if (byteRay == null) {
	    return null;
	  }

	  try {
	    return new String(byteRay, enc);
	  }
	  catch (UnsupportedEncodingException e) {
	    //this will never happen, unless the DEFAULT_CHAR_ENCODING
	    //is invalid
	    return "support.ConstantsIF.DEFAULT_CHAR_ENCODING=" +
	           DEFAULT_CHAR_ENCODING + " is invalid!";
	  }
	}
	*/

	public void setEncoding(String enc) {
	  if (enc == null) {
	    return;
	  }
	  else {
	    //test this encoding string to be valid
	    try {
	      byte[] bytes = {(byte) '0', (byte) '1'};
	      new String(bytes, enc);
	      this.enc = enc;
	    }
	    catch (UnsupportedEncodingException e) {
	      //don't override the default encoding
	      System.out.println("unsupported encoding");
	    }
	  }
	}

	/** might return null if encoding fails; unlike {@link #getString()} this does not use system default encoding */
	public String getEncodedString() {
	  try {
	    return new String(byteRay, enc);
	  }
	  catch (Exception e) {
	    return null;
	  }
	}

	/** returns a string representation of the byte[] using system default encoding */
	public String getString() {
	  if (byteRay != null) return new String(byteRay);
	  else return "";
	}

	/* internal impl methods */
	protected final void arrayCopy(byte[] srcBuf, int srcStartIndex,
	                               byte[] destBuf, int destStartIndex,
	                               int numberOfBytesToCopy)
	{
	  System.arraycopy(srcBuf, srcStartIndex,
	                   destBuf, destStartIndex,
	                   numberOfBytesToCopy);
	  /*
	  System.out.println( "arrayCopy start" );
	  for( int i=0; i<numberOfBytesToCopy; i++) {
	    destBuf[ destStartIndex + i ] = srcBuf[ srcStartIndex + i ];
	    System.out.println( "\tindex="+i );
	  }
	  System.out.println( "arrayCopy end" );
	  */
	}

	/* accessors for internal state */
	public byte[] getBytes() {
	  if (byteRay == null) {
	    return new byte[0];
	  }
	  return byteRay;
	}

	public ByteArrayInputStream getInputStream() {
	  return new ByteArrayInputStream(getBytes());
	}

	public int getSize() {
	  if (byteRay != null) {
	    return byteRay.length;
	  }
	  else {
	    return 0;
	  }
	}

	/* convenience methods */
	public void append(byte[] srcBuf) {
	  append(srcBuf, 0, srcBuf.length);
	}

	public void append(ByteBuffer buf) {
	  append(buf.getBytes(), 0, buf.getSize());
	}

	public void clear() {
	  if (byteRay != null) {
	    byteRay = null;
	  }
	}

	/* self test method */
	public static void selftest() {
	  byte[] br1 = {(byte) '0', (byte) '1'};
	  byte[] br2 = {(byte) '<', (byte) 'T', (byte) '>'};

	  System.out.println("::bb1.append( br1 )");
	  ByteBuffer bb1 = new ByteBuffer().append(br1, 0, 2);
	  bb1.setEncoding(UTF8);
	  System.out.println();

	  System.out.println("::bb1.toString():" + bb1.toString());
	  System.out.println();

	  System.out.println("::bb1.append( br2 )");
	  bb1.append(br2, 0, 3);
	  System.out.println();

	  System.out.println("::bb1.toString():" + bb1.toString());
	  System.out.println();
	}

}
package com.i2r.utils;

import java.io.UnsupportedEncodingException;
import java.util.Hashtable;
import java.util.Vector;

public class StringUtils {

	//check if RIM devices supports the encoding
	public static boolean isDeviceSupportEncoding(String encoding){
		try{
			new String("Testing string".getBytes(), encoding);
			return true;
		} catch (UnsupportedEncodingException e) {
			return false;
		}
	}
	
    
	
	// Splits string 
	public static String[] split(final String string, final String splitBy) {
		final Vector tokens = new Vector();
		final int tokenLength = splitBy.length();

		int tokenStart = 0;
		int splitIndex;
		while ((splitIndex = string.indexOf(splitBy, tokenStart)) != -1) {
			tokens.addElement(string.substring(tokenStart, splitIndex));
			tokenStart = splitIndex + tokenLength;
		}

		tokens.addElement(string.substring(tokenStart));

		final String[] result = new String[tokens.size()];
		tokens.copyInto(result);
		return result;
	}

    public static boolean equalsIgnoreCase(String string1, String string2) {
        // Strings are both null, return true
        if (string1 == null && string2 == null) {
            return true;
        }
        // One of the two is null, return false
        if (string1 == null || string2 == null) {
            return false;
        }
        // Both are not null, compare the lowercase strings
        if ((string1.toLowerCase()).equals(string2.toLowerCase())) {
            return true;
        } else {
            return false;
        }
    }

	  // Split string in 2 strings 
	 public synchronized static String[] split2Strings( String in, char ch ){
	      String[] result = new String[2];
	      int      pos = in.indexOf( ch );

	      if( pos != -1 ){
	          result[0] = in.substring( 0, pos ).trim();
	          result[1] = in.substring( pos+1 ).trim();
	      } else {
	          result[0] = in.trim();
	      }

	      return result;
	  }

	
  public synchronized static int ensureInt(final String string, final int defaultValue) {
	    try {
	      return Integer.parseInt(string);
	    } catch (final NumberFormatException e) {
	      return defaultValue;
	    }
	  }

	  public synchronized static double ensureDouble(final String string, final double defaultValue) {
	    try {
	      return Double.parseDouble(string);
	    } catch (final NumberFormatException e) {
	      return defaultValue;
	    }
	  }
		 
	 
	  public static String replaceAll(final String original, final String tokenToBeReplaced, final String value) {
		    //TODO : optimize
		    final StringBuffer result = new StringBuffer();
		    final String[] originalSplit = split(original, tokenToBeReplaced);
		    for (int i = 0; i < originalSplit.length; i++) {
		      result.append(originalSplit[i]);
		      if (i != originalSplit.length - 1) {
		        result.append(value);
		      }
		    }
		    return result.toString();
		  }
	  
	  public static String replaceLast(final String original, final String tokenToBeReplaced, final String value) {
		    //TODO : optimize
		    final StringBuffer result = new StringBuffer();
		    final String[] originalSplit = split(original, tokenToBeReplaced);
		    for (int i = 0; i < originalSplit.length; i++) {
		      result.append(originalSplit[i]);
		      if(i == originalSplit.length - 2) {
		    	  result.append(value);
		      } else if (i != originalSplit.length - 1 ) {
		        result.append(tokenToBeReplaced);
		      }
		    }
		    return result.toString();
		  }
	  
		/**
		 * decode an escaped ASCII format string as defined by RFC 2396.
		 * Removes %nn encodings from a string.
		 */
		public static String decode(final String encodedStr)
				throws Exception
		{
			if (encodedStr == null)
			{
				return null;
			}
			if (encodedStr.indexOf('%') < 0)
			{
				return encodedStr;
			}
			final StringBuffer buffer = new StringBuffer(encodedStr);
			decode(buffer, 0, buffer.length());
			return buffer.toString();
		}

		/**
		 * decode an escaped ASCII format string as defined by RFC 2396.
		 * Removes %nn encodings from a string.
		 */
		public static void decode(final StringBuffer buffer, final int offset,
				final int length) throws Exception
		{
			int index = offset;
			int count = length;
			for (; count > 0; count--, index++)
			{
				final char ch = buffer.charAt(index);
				if (ch != '%')
				{
					continue;
				}
				if (count < 3)
				{
					throw new Exception("invalid encoded character found!");
				}

				// Decode
				int dig1 = Character.digit(buffer.charAt(index + 1), 16);
				int dig2 = Character.digit(buffer.charAt(index + 2), 16);
				if (dig1 == -1 || dig2 == -1)
				{
					throw new Exception("invalid encoded character found! ");
				}
				char value = (char) (dig1 << 4 | dig2);

				// Replace
				buffer.setCharAt(index, value);
				buffer.delete(index + 1, index + 3);
				count -= 2;
			}
		}
	  
	  
	  
}


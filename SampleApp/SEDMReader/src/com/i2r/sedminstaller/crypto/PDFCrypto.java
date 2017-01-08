package com.i2r.sedminstaller.crypto;

import java.io.UnsupportedEncodingException;

import net.rim.device.api.crypto.MD5Digest;
import net.rim.device.api.ui.component.Dialog;

import com.i2r.sedminstaller.util.Convert;

public class PDFCrypto {
	private static int KEY_SIZE = 32;
	private static int FRAME_KEY_SIZE = KEY_SIZE + 4;
	private byte[] frameKey;
	private byte[] SBox;
	
	public PDFCrypto() {
		frameKey = new byte[FRAME_KEY_SIZE];
		SBox = new byte[256];
	}
	
	public void EncryptStream(byte[] key, int key_len, 
			                  byte[] input, int input_len, 
			                  int offset, int block_size)
	{
		int blockNum = offset/block_size;
		byte c;
		byte iii, jjj, ttt;
		int i, rem;
		int input_offset = 0;
	
		if (key_len < FRAME_KEY_SIZE)
			System.arraycopy(key, 0, frameKey, 0, key_len);
		else
			System.arraycopy(key, 0, frameKey, 0, FRAME_KEY_SIZE - KEY_SIZE);
	
		System.arraycopy(Convert.intToByteArray(blockNum), 0, frameKey, KEY_SIZE, FRAME_KEY_SIZE - KEY_SIZE);
		
	    byte[] hashKey = PDFMD5.digest(frameKey, 0, 4);
	   
	    PDFCryptoInit(hashKey, 16);   
	    
	    iii=0; 
		jjj=0;
	    rem=offset%block_size;
		for(i=0;  i<rem; i++) {
			iii = (byte) (unsignedByteToInt(iii) + 1);
			jjj = (byte) (unsignedByteToInt(jjj) + unsignedByteToInt(SBox[unsignedByteToInt(iii)]));
			ttt = SBox[unsignedByteToInt(iii)];
			SBox[unsignedByteToInt(iii)] = SBox[unsignedByteToInt(jjj)];
			SBox[unsignedByteToInt(jjj)] = ttt;
			ttt = (byte) (unsignedByteToInt(SBox[unsignedByteToInt(iii)]) + unsignedByteToInt(SBox[unsignedByteToInt(jjj)]));
			c= SBox[unsignedByteToInt(ttt)];
		}
		
		offset=block_size;
		for(i=rem; i<rem+input_len; i++) 
		{
			if(i==offset)  // For each new block, refresh the key
			{
				blockNum++;
				memset(frameKey,0, FRAME_KEY_SIZE);
				if(key_len <= FRAME_KEY_SIZE) 
					System.arraycopy(key, 0, frameKey, 0, key_len);
				else 
					System.arraycopy(key, 0, frameKey, 0, FRAME_KEY_SIZE - KEY_SIZE);
				System.arraycopy(Convert.intToByteArray(blockNum), 0, frameKey, KEY_SIZE, FRAME_KEY_SIZE - KEY_SIZE);
				 
				hashKey = PDFMD5.digest(frameKey, 0, 4);
				PDFCryptoInit(hashKey, 16);    

				iii=0; 
				jjj=0;
				offset+=block_size;
			}
			{
				iii = (byte) (unsignedByteToInt(iii) + 1);
				jjj = (byte) (unsignedByteToInt(jjj) + unsignedByteToInt(SBox[unsignedByteToInt(iii)]));
				ttt = SBox[unsignedByteToInt(iii)];
				SBox[unsignedByteToInt(iii)] = SBox[unsignedByteToInt(jjj)];
				SBox[unsignedByteToInt(jjj)] = ttt;
				ttt = (byte) (unsignedByteToInt(SBox[unsignedByteToInt(iii)]) + unsignedByteToInt(SBox[unsignedByteToInt(jjj)]));
				c= SBox[unsignedByteToInt(ttt)];
			}
			input[input_offset]= (byte) (input[input_offset]^c);
			input_offset++;
		}
	}
	
	private void PDFCryptoInit(byte[] key, int key_len) 
	{
		byte jjj=0, ttt=0;
		int keypos = 0, x;
		byte[] K = new byte [256];

		for ( x=0; x<256; ++x )
		{
			SBox[x] = (byte) x;
			K[x] = key[keypos++];
			if ( keypos >= key_len ) keypos=0;
		}

		jjj = 0;
		for ( x=0; x<256; ++x )
		{
			jjj = (byte) (unsignedByteToInt(jjj) + (unsignedByteToInt(SBox[x]) + unsignedByteToInt(K[x])));
			ttt = (byte) unsignedByteToInt(SBox[x]);
			SBox[x] = SBox[unsignedByteToInt(jjj)];
			SBox[unsignedByteToInt(jjj)] = (byte) ttt;
		}
	}
	
	private void memset(byte[] p, int j, int length) {
		for (int i=0; i<length; i++)
			p[i] = (byte) j;
	}
	
	public static int unsignedByteToInt(byte b) {
	    return (int) b & 0xFF;
	    }
	
	/*public void example(byte[] key)
	{
		//MD5Digest digest = new MD5Digest();
		//digest.update(key, 0, 4);
				
		byte[] hashKey = PDFMD5.digest(key, 0, 4);
		//digest.getDigest(hashKey, 0, false);
		
	    try {
			Dialog.alert(Convert.getHexString(hashKey));
		} catch (Exception e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
	}*/
	
}

package com.i2r.sedminstaller.crypto;

import java.io.ByteArrayInputStream;
import java.io.ByteArrayOutputStream;
import java.io.IOException;

import net.rim.device.api.crypto.AESDecryptorEngine;
import net.rim.device.api.crypto.AESEncryptorEngine;
import net.rim.device.api.crypto.AESKey;
import net.rim.device.api.crypto.BlockDecryptor;
import net.rim.device.api.crypto.BlockEncryptor;
import net.rim.device.api.crypto.CBCDecryptorEngine;
import net.rim.device.api.crypto.CBCEncryptorEngine;
import net.rim.device.api.crypto.CryptoTokenException;
import net.rim.device.api.crypto.CryptoUnsupportedOperationException;
import net.rim.device.api.crypto.InitializationVector;


public class AESCrypto {
	ByteArrayOutputStream outputStream;
	ByteArrayInputStream inputStream;
	BlockEncryptor encryptor;
	BlockDecryptor decryptor;
	AESKey key;
	InitializationVector IV;
	
	public AESCrypto() {
		key = new AESKey();
		IV = new InitializationVector(16);
	}
	
	public AESCrypto(byte[] _key, byte[] _IV) {
		key = new AESKey(_key);
		IV  = new InitializationVector(_IV);
	}
	
	public AESCrypto(byte[] _key, byte[] _IV, int size) {
		key = new AESKey(_key, 0, size);
		IV  = new InitializationVector(_IV);
	}
	
	public AESCrypto(AESKey _key, InitializationVector _IV) {
		key = _key;
		IV = _IV;
	}
	
	public void encrypt(byte[] data, byte[] output, int offset)  {
		try {
			outputStream = new ByteArrayOutputStream();
			encryptor = new BlockEncryptor(
					//new PKCS5FormatterEngine(
				new CBCEncryptorEngine(
						new AESEncryptorEngine(key),IV 
				), outputStream
		      
			);
			encryptor.write(data, 0, data.length);
			encryptor.close();
			
			byte[] temp = outputStream.toByteArray();
			for (int i=0; i<data.length; i++)
				output[i + offset] = temp[i];
			
			outputStream.close();
			
		} catch (CryptoTokenException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		} catch (CryptoUnsupportedOperationException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
	}
	
	public void decrypt(byte[] data, byte[] output, int offset)  {
		try {
			inputStream = new ByteArrayInputStream(data);
			decryptor = new BlockDecryptor(
					//new PKCS5UnformatterEngine(
				new CBCDecryptorEngine(
						new AESDecryptorEngine(key),IV 
				), inputStream
	
			);
			byte[] temp = new  byte[data.length];
			int ret = decryptor.read(temp, 0, data.length );
			
			for (int i=0; i<data.length; i++)
				output[i + offset] = temp[i];
			
			decryptor.close();
			inputStream.close();
			
			
		} catch (CryptoTokenException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		} catch (CryptoUnsupportedOperationException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
	}
	
	
}

package com.i2r.sedminstaller.network;

import java.io.IOException;
import com.i2r.sedminstaller.crypto.AESCrypto;

public class HttpPacket {
	public static int DOWNLOAD = 0x00;
	public static int USERNAME = 0x01;
	public static int AES      = 0x02;
	public static int DELETE   = 0x03;
	public static int LICENSE  = 0x04;
	public static int LOGIN_1  = 0x05;
	public static int LOGIN_2  = 0x06;
	byte[] packet;
	
	public HttpPacket(byte[] data, int mode, byte[] key, byte[] IV) throws IOException {
		if (mode == HttpPacket.DOWNLOAD) {
			packet = new byte[2];
			packet[0] = 0x4A;
		} else
			if (mode == HttpPacket.USERNAME){
				packet = new byte[data.length + 2];
				packet[0] = '@';
				packet[1] = (byte) data.length;
				for (int i=0; i<data.length; i++)
					packet[i + 2] = data[i];
				
			} else
				if (mode == HttpPacket.AES)
					{
					    packet = new byte[data.length + 2];
					    packet[0] = '*';
						AESCrypto encryptor = new AESCrypto(key, IV);
						encryptor.encrypt(data, packet, 1);
					} else
						if (mode == HttpPacket.DELETE)
						{
							packet = new byte[29];
							packet[0] = '^';
							for (int i=0; i<data.length; i++)
								packet[i + 1] = data[i];
						} else
							if (mode == HttpPacket.LICENSE)
							{
								packet = new byte[2];
								packet[0] = '#';
							} else
								if (mode == HttpPacket.LOGIN_1)
								{
									packet = new byte[data.length + 2];
									packet[0] = (byte) 0xFF;
									packet[1] = (byte) data.length;
									for (int i=0; i<data.length; i++)
										packet[i + 2] = data[i];
									
								} else
									if (mode == HttpPacket.LOGIN_2)
									{
										 packet = new byte[data.length + 2];
										 packet[0] = (byte) 0xEE;
										 AESCrypto encryptor = new AESCrypto(key, IV);
										 encryptor.encrypt(data, packet, 1);
										
									} else
										packet = null;
	}
	
	public byte[] getPacket() {
		return packet;
	}
}

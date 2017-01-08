package com.i2r.sedm;

import java.io.ByteArrayInputStream;
import java.io.ByteArrayOutputStream;
import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.IOException;
import javax.microedition.rms.RecordEnumeration;
import javax.microedition.rms.RecordStore;
import javax.microedition.rms.RecordStoreException;
import javax.microedition.rms.RecordStoreNotFoundException;
import javax.microedition.rms.RecordStoreNotOpenException;

import net.rim.device.api.ui.component.Dialog;

/**
 * 
 * @author Administrator
 */
public class RecordManager {

	RecordStore rms;

	public byte[] StringtoBytes(String str) {
		byte[] data = null;
		try {
			ByteArrayOutputStream baos = new ByteArrayOutputStream();
			DataOutputStream dos = new DataOutputStream(baos);
			dos.writeUTF(str);
			data = baos.toByteArray();
			baos.close();
			dos.close();
		} catch (Exception ex) {
			System.out.println("RecordManager#StringtoBytes() threw "
					+ ex.toString());
			ex.printStackTrace();
		}
		return data;
	}

	public String BytesToString(byte[] data) {
		ByteArrayInputStream bais = new ByteArrayInputStream(data);
		DataInputStream dis = new DataInputStream(bais);
		String str = null;
		try {
			str = dis.readUTF();
			bais.close();
			dis.close();
		} catch (Exception ex) {
			System.out.println("RecordManager#BytesToString() threw "
					+ ex.toString());
			ex.printStackTrace();
		}
		return str;
	}

	public String[] Read(String fileName) {
		String records[] = null;
		try {
			rms = RecordStore.openRecordStore(fileName, true);
			int NumberTotal = rms.getNumRecords();
			records = new String[NumberTotal];
			System.out.println(NumberTotal);
			RecordEnumeration re = rms.enumerateRecords(null, null, false);
			int i = 0;
			while (re.hasNextElement()) {
				int rid = re.nextRecordId();
				String temp = BytesToString(rms.getRecord(rid));
				records[i] = temp;
				i++;
			}
			rms.closeRecordStore();
		} catch (Exception ex) {
			System.out.println("RecordManager#Read() threw " + ex.toString());
			ex.printStackTrace();
		}
		return records;
	}

	public String Save(String[] strs, String fileName) {
		String result = null;
		try {
			rms = RecordStore.openRecordStore(fileName, true);
			RecordEnumeration re = rms.enumerateRecords(null, null, false);
			while (re.hasNextElement()) {
				int rid = re.nextRecordId();
				rms.deleteRecord(rid);
			}

			for (int i = 0; i < strs.length; i++) {
				if (strs[i] != null && strs[i].length() > 0) {
					byte[] data = StringtoBytes(strs[i]);
					rms.addRecord(data, 0, data.length);
				}
			}
			rms.closeRecordStore();
			// System.out.println("Save: "+rms.getNumRecords());
		} catch (Exception ex) {
			System.out.println("RecordManager#Save() threw " + ex.toString());
			result = "store error!";
		}
		return result;
	}

}

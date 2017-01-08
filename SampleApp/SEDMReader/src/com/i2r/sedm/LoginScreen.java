package com.i2r.sedm;

import net.rim.device.api.ui.Color;
import net.rim.device.api.ui.Field;
import net.rim.device.api.ui.FieldChangeListener;
import net.rim.device.api.ui.Font;
import net.rim.device.api.ui.Graphics;
import net.rim.device.api.ui.UiApplication;
import net.rim.device.api.ui.component.ButtonField;
import net.rim.device.api.ui.component.Dialog;
import net.rim.device.api.ui.component.EditField;
import net.rim.device.api.ui.component.LabelField;
import net.rim.device.api.ui.component.PasswordEditField;
import net.rim.device.api.ui.component.SeparatorField;
import net.rim.device.api.ui.container.HorizontalFieldManager;
import net.rim.device.api.ui.container.MainScreen;
import net.rim.device.api.ui.container.VerticalFieldManager;
import com.i2r.sedminstaller.listener.*;
import com.i2r.utils.log.Log;

public class LoginScreen extends MainScreen implements FieldChangeListener {
	final int lblHeight = 14;
	LabelField lblTitle = new LabelField("Login Screen", FIELD_HCENTER);
	EditField edtName;
	PasswordEditField edtPass;
	ButtonField btnEnter, btnCancel;
	Font fontLblTitle = Font.getDefault().derive(Font.EXTRA_BOLD, lblHeight);
	Font fontLblStatus = Font.getDefault().derive(Font.ITALIC, lblHeight);
	SPDFReader spdfapp;

	public LoginScreen() {
		spdfapp = (SPDFReader)UiApplication.getUiApplication();
		lblTitle.setFont(fontLblTitle);
		setTitle(lblTitle);
		add(new SeparatorField());
		// MAIN MANAGER
		VerticalFieldManager vfmMain = new VerticalFieldManager();
		VerticalFieldManager vfmContentHolder = new VerticalFieldManager();
		// HORIZONTAL MANAGER FOR NAME FIELDS
		HorizontalFieldManager hfmName = new HorizontalFieldManager(FIELD_RIGHT);
		// LBL FIELD FOR NAME
		LabelField lblName = new LabelField("Username: ");
		hfmName.add(lblName);
		// EDIT FIELD FOR NAME
		edtName = new EditField("", "enter here", 256, EditField.NO_NEWLINE) {
			protected void onFocus(int direction) {
				if (edtName.getText().equalsIgnoreCase("enter here"))
					clear(256);
				super.onFocus(direction);
			}

			protected void paint(Graphics graphics) {
				graphics.setColor(Color.GRAY);
				super.paint(graphics);
			}
		};
		hfmName.add(edtName);
		// vfmMain.add(hfmName);
		vfmContentHolder.add(hfmName);
		HorizontalFieldManager hfmPass = new HorizontalFieldManager();
		// LBL FIELD FOR pass
		LabelField lblPass = new LabelField("Password: ");
		hfmPass.add(lblPass);
		// EDIT FIELD FOR pass
		edtPass = new PasswordEditField("", "", 256, EditField.NO_NEWLINE) {
			protected void onFocus(int direction) {
				if (edtPass.getText().equalsIgnoreCase(""))
					clear(256);
				super.onFocus(direction);
			}
		};
		hfmPass.add(edtPass);
		// vfmMain.add(hfmPass);
		vfmContentHolder.add(hfmPass);
		vfmContentHolder.setMargin(50, 0, 0, 50);
		vfmMain.add(vfmContentHolder);
		HorizontalFieldManager hfmStatus = new HorizontalFieldManager(
				FIELD_HCENTER);
		btnEnter = new ButtonField("Login", FIELD_HCENTER
				| ButtonField.CONSUME_CLICK);
		btnEnter.setChangeListener(this);
		hfmStatus.add(btnEnter);
		btnCancel = new ButtonField("Cancel", FIELD_HCENTER
				| ButtonField.CONSUME_CLICK);
		btnCancel.setChangeListener(this);
		hfmStatus.add(btnCancel);
		add(vfmMain);
		// hfmStatus.add(lblStatus);
		add(new SeparatorField());
		setStatus(hfmStatus);
		btnCancel.setFocus();
	}

	
	public void DoLogin(final String username,final String password)
	{
		spdfapp.readerScreen.listener.LoginProcess(username,password);	
		UiApplication.getUiApplication().invokeLater(new Runnable()  {
		public void run() {					
	                boolean keepGoing = true;
	                int trynum =0;
	                try {
	                while (keepGoing && trynum<50)
	                {
	                	trynum++;
	                    if (spdfapp.readerScreen.listener.userflag == 0 )
	                    {
	                        try { Thread.sleep(100); }
	                        catch (Exception ex) { }
	                        if (trynum>=50)
	                        {
		        				spdfapp.errorDialog("Timeout on online Login check");
	                        }
	                    }
	                    else
		                    if (spdfapp.readerScreen.listener.userflag == 1 )
		                    {
		        				spdfapp.errorDialog("Invalid Username or Password!");
		        				keepGoing = false;
		                    }//end else
		                else
	                    if (spdfapp.readerScreen.listener.userflag == 9 )
	                    {
	    					spdfapp.userName =username;
	    					spdfapp.passWord =password;
	    					spdfapp.flagAccountExists = true;
	    					spdfapp.InvokeSPDF();
	        				spdfapp.errorDialog("valid Username and Password!");
	                    	keepGoing = false;
	                   	
	                    }//end else
	                    else
	                    {
	        				spdfapp.errorDialog("Error on online Login check!");
	        				keepGoing = false;
	                    }
	    			}
	    			} catch (Exception ex) {
	    				Log.error("SPDFLoginScreen#DoLogin() threw: Exception: "
	    								+ ex.toString());
	    				Dialog.alert("SPDFLoginScreen#DoLogin() threw: Exception: "
	    						+ ex.toString());
	    			}

			}
		    });
		
		
	}
	
	public void fieldChanged(Field field, int context) {
		String name = edtName.getText().trim();
		String pass = edtPass.getText().trim();
		if (field == btnEnter) {
			if (name.equalsIgnoreCase("") || pass.equalsIgnoreCase("")
					|| name.equalsIgnoreCase("enter here")) {
				Dialog.inform("Please enter all fields");
			} else {
				
/*				if (name.equalsIgnoreCase("111")
						&& pass.equalsIgnoreCase("111")) {
					spdfapp.userName ="zhigang";
					spdfapp.passWord ="p@ssw0rd";
					spdfapp.flagAccountExists = true;
				}
*/
				DoLogin(name,pass);
			}
		}
	}
}


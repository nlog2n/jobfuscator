package com.i2r.cas.securepdf;

//import net.rim.device.api.system.DeviceInfo;
import net.rim.device.api.system.Bitmap;
import net.rim.device.api.ui.Field;
import net.rim.device.api.ui.FieldChangeListener;
import net.rim.device.api.ui.UiApplication;
import net.rim.device.api.ui.component.BitmapField;
import net.rim.device.api.ui.component.ButtonField;
import net.rim.device.api.ui.component.Dialog;
import net.rim.device.api.ui.component.LabelField;
import net.rim.device.api.ui.component.RichTextField;
import net.rim.device.api.ui.container.MainScreen;

import com.i2r.cas.securepdf.SecurePDF;  /////////////////////////////////////////////////////////////////////////// add this line!


public class HelloWorld extends UiApplication {
	public static void main (String[] args) {
		HelloWorld theApp = new HelloWorld();
		theApp.enterEventDispatcher();
	}
	
	public HelloWorld() {
		pushScreen(new HelloWorldScreen());
	}
}

final class HelloWorldScreen extends MainScreen {
	RichTextField text;
	BitmapField _bitmap;
	Bitmap pic;
	
	SecurePDF my_wrapper;              ////////////////////////////////////////////////////////// add this member!

	
	public HelloWorldScreen()
    {      	
		super();
		
		my_wrapper = new SecurePDF();    ////////////////////////////////////////////////////////// add this line!
		
		LabelField title = new LabelField("Secure PDF ReaderWrapper", 
            LabelField.ELLIPSIS | LabelField.USE_ALL_WIDTH);
		setTitle(title);
		 text = new RichTextField("Hello World!");
		add(text);
		
		FieldChangeListener listernerPIN_button = new FieldChangeListener() {
			public void fieldChanged(Field field, int context) {
				String a;
				if (my_wrapper.checkDevice()){                         /////////////////////////////// One example to do checking
					a = " successfully verified.";
				}else{
					a = " verification failed!";
				}
				
				a = a+ my_wrapper.getErrString(); 
				text.setText(a);
			}
		};
		
		ButtonField PIN_button = new ButtonField("Verify wrapper");
		PIN_button.setChangeListener(listernerPIN_button);
		
		FieldChangeListener listernerImage_button = new FieldChangeListener() {
			public void fieldChanged(Field field, int context) {
				if (my_wrapper.checkDevice()) {                     ///////////////////////////////// Another example to do checking
					pic = Bitmap.getBitmapResource("example3.jpg");
				}else{
					pic = Bitmap.getBitmapResource("example2.jpg");					
				}
				_bitmap.setBitmap(pic);
			}
		};
		ButtonField Image_button = new ButtonField("Show PDF Image");
		Image_button.setChangeListener(listernerImage_button);
		
		add(PIN_button);
		_bitmap = new BitmapField();
		add(_bitmap);
		add(Image_button);
		
		
    }
	
	public boolean onClose()
    {
        Dialog.alert("Goodbye!");
        System.exit(0);
        return true;
    }
}
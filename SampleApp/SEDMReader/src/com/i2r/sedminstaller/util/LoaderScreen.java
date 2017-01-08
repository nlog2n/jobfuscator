package com.i2r.sedminstaller.util;

import java.util.Timer;
import java.util.TimerTask;
import net.rim.device.api.system.Bitmap;
import net.rim.device.api.system.Display;
import net.rim.device.api.ui.Graphics;
import net.rim.device.api.ui.container.MainScreen;
 
public class LoaderScreen extends MainScreen {
 
    Timer loadingTimer = new Timer();
    TimerTask loadingTask;
    int imageIndex = 0;
    Bitmap loader1 = Bitmap.getBitmapResource("loader001.png");
    Bitmap loader2 = Bitmap.getBitmapResource("loader002.png");
    Bitmap loader3 = Bitmap.getBitmapResource("loader003.png");
    Bitmap loader4 = Bitmap.getBitmapResource("loader004.png");
    Bitmap loader5 = Bitmap.getBitmapResource("loader005.png");
    Bitmap loader6 = Bitmap.getBitmapResource("loader006.png");
    Bitmap loader7 = Bitmap.getBitmapResource("loader007.png");
    Bitmap loader8 = Bitmap.getBitmapResource("loader008.png");
    Bitmap loader9 = Bitmap.getBitmapResource("loader009.png");
    Bitmap loader10 = Bitmap.getBitmapResource("loader010.png");
    Bitmap loader11 = Bitmap.getBitmapResource("loader011.png");
    Bitmap loader12 = Bitmap.getBitmapResource("loader012.png");
    boolean showLoader = false;
    int screenWidth = Display.getWidth();
    int screenHeight = Display.getHeight();
    int xAnchor = (screenWidth-200)/2;
    int yAnchor = (screenHeight-40)/2;
    String loaderText = "Please wait...";
 
    public LoaderScreen() {
        super();
 
        loadingTask = new TimerTask() {
 
            public void run() {
 
                invalidate();
                imageIndex++;
                if(imageIndex == 11){
                    imageIndex = 0;
                }
            }
        };
 
        loadingTimer.scheduleAtFixedRate(loadingTask, 100, 100);
 
    }
 
    public void setShowLoader(boolean showLoader) {
        this.showLoader = showLoader;
    }
 
    public void setLoaderText(String str) {
    	this.loaderText = str;
    }
    
    public boolean isShowing(){
        return showLoader;
    }
 
    protected void paint(Graphics graphics) {
        super.paint(graphics);
        if (showLoader) {
            //draw background:
            graphics.setColor(0x363636);
            graphics.fillRect(xAnchor, yAnchor, 200, 40);
 
            //draw animation frame:
            switch (imageIndex) {
                case 0:
                    graphics.drawBitmap(xAnchor + 4, yAnchor + 4, 32, 32, loader1, 0, 0);
                    break;
                case 1:
                    graphics.drawBitmap(xAnchor + 4, yAnchor + 4, 32, 32, loader2, 0, 0);
                    break;
                case 2:
                    graphics.drawBitmap(xAnchor + 4, yAnchor + 4, 32, 32, loader3, 0, 0);
                    break;
                case 3:
                    graphics.drawBitmap(xAnchor + 4, yAnchor + 4, 32, 32, loader4, 0, 0);
                    break;
                case 4:
                    graphics.drawBitmap(xAnchor + 4, yAnchor + 4, 32, 32, loader5, 0, 0);
                    break;
                case 5:
                    graphics.drawBitmap(xAnchor + 4, yAnchor + 4, 32, 32, loader6, 0, 0);
                    break;
                case 6:
                    graphics.drawBitmap(xAnchor + 4, yAnchor + 4, 32, 32, loader7, 0, 0);
                    break;
                case 7:
                    graphics.drawBitmap(xAnchor + 4, yAnchor + 4, 32, 32, loader8, 0, 0);
                    break;
                case 8:
                    graphics.drawBitmap(xAnchor + 4, yAnchor + 4, 32, 32, loader9, 0, 0);
                    break;
                case 9:
                    graphics.drawBitmap(xAnchor + 4, yAnchor + 4, 32, 32, loader10, 0, 0);
                    break;
                case 10:
                    graphics.drawBitmap(xAnchor + 4, yAnchor + 4, 32, 32, loader11, 0, 0);
                    break;
                case 11:
                    graphics.drawBitmap(xAnchor + 4, yAnchor + 4, 32, 32, loader12, 0, 0);
                    break;
            }
 
            //draw text:
            graphics.setColor(0xefefef);
            graphics.drawText(loaderText, xAnchor + 44, yAnchor + 6);
 
            //draw border:
            graphics.setColor(0xcccccc);
            graphics.drawRect(xAnchor, yAnchor, 200, 40);
        }
    }
}

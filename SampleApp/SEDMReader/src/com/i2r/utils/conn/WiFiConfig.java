package com.i2r.utils.conn;

public class WiFiConfig extends AbstractConfiguration {
	
    protected static final String CONFIG_DESCRIPTION = "Wi-Fi Network";
  
	public WiFiConfig() {
		super();
		setUrlParameters(";interface=wifi");
		setDescription(CONFIG_DESCRIPTION);
	}
}

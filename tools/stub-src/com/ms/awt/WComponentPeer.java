package com.ms.awt;

/** Microsoft JVM AWT peer stub. getPeer() was removed from Component. */
public class WComponentPeer {
	public static WComponentPeer from(Object component) {
		return new WComponentPeer();
	}

	public int getHwnd() {
		return 0;
	}

	public int getTopHwnd() {
		return 0;
	}
}

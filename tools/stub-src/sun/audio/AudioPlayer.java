package sun.audio;

import java.io.InputStream;

/** Removed JDK internal; enough for RSC PcmPlayer to compile. */
public class AudioPlayer extends Thread {
	public static final AudioPlayer player = new AudioPlayer();

	private AudioPlayer() {
		setDaemon(true);
	}

	public synchronized void start(InputStream stream) {
	}

	public synchronized void stop(InputStream stream) {
	}
}

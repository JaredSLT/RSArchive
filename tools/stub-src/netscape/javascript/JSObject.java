package netscape.javascript;

/** LiveConnect stub so archived clients compile on modern JDKs. */
public abstract class JSObject {
	public static JSObject getWindow(Object target) {
		return new Dummy();
	}

	public abstract Object call(String methodName, Object... args) throws JSException;

	public abstract Object eval(String s) throws JSException;

	public abstract Object getMember(String name) throws JSException;

	public abstract void setMember(String name, Object value) throws JSException;

	public abstract void removeMember(String name) throws JSException;

	public abstract Object getSlot(int index) throws JSException;

	public abstract void setSlot(int index, Object value) throws JSException;

	private static final class Dummy extends JSObject {
		public Object call(String methodName, Object... args) {
			return null;
		}

		public Object eval(String s) {
			return null;
		}

		public Object getMember(String name) {
			return null;
		}

		public void setMember(String name, Object value) {
		}

		public void removeMember(String name) {
		}

		public Object getSlot(int index) {
			return null;
		}

		public void setSlot(int index, Object value) {
		}
	}
}

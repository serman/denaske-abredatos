package org.denaske.palcampito;

public interface ServerListener {

	public void onImageDraw(String fileName); 
	public void onBGChange(int r, int g, int b); 
	public void onTextShow(String text, int x, int y, int size, int r, int g, int b); 
	
}

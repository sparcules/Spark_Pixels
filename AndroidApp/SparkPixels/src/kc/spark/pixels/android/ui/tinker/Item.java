package kc.spark.pixels.android.ui.tinker;

public class Item {
    private String mTitle;
    private int mNumColors;
    private int mColor1;
    private int mColor2;
    private int mColor3;
    private int mColor4;
    private int mColor5;
    private int mColor6;
    
    public Item(String title, int numColors, int color1, int color2, int color3, int color4, int color5, int color6) {
        super();
        mTitle = title;
        mNumColors = numColors;
        mColor1 = color1;
        mColor2 = color2;
        mColor3 = color3;
        mColor4 = color4;
        mColor5 = color5;
        mColor6 = color6;
    }

    public String getTitle() {
        return mTitle;
    }

    public void setTitle(String title) {
    	mTitle = title;
    }
    
    public int getNumColors() {
        return mNumColors;
    }

    public void setNumColors(int numColors) {
    	mNumColors = numColors;
    }
    
    public int getColor1() {
        return mColor1;
    }

    public void setColor1(int color1) {
    	mColor1 = color1;
    }
    
    public int getColor2() {
        return mColor2;
    }

    public void setColor2(int color2) {
    	mColor2 = color2;
    }
    
    public int getColor3() {
        return mColor3;
    }

    public void setColor3(int color3) {
    	mColor3 = color3;
    }
    
    public int getColor4() {
        return mColor4;
    }

    public void setColor4(int color4) {
    	mColor4 = color4;
    }
    
    public int getColor5() {
        return mColor5;
    }

    public void setColor5(int color5) {
    	mColor4 = color5;
    }
    
    public int getColor6() {
        return mColor6;
    }

    public void setColor6(int color6) {
    	mColor4 = color6;
    }
    
    
    public int getColor(int colorNumber) {
    	switch(colorNumber){
      	case 1:
      		return mColor1;
       	case 2:
       		return mColor2;
       	case 3:
       		return mColor3;
       	case 4:
       		return mColor4;
       	case 5:
       		return mColor5;
       	case 6:
       		return mColor6;
    	}
        return mColor1;
    }
    
    public int setColor(int colorNumber, int color) {
    	switch(colorNumber){
      	case 1:
      		mColor1 = color; break;
       	case 2:
       		mColor2 = color; break;
       	case 3:
       		mColor3 = color; break;
       	case 4:
       		mColor4 = color; break;
       	case 5:
       		mColor5 = color; break;
       	case 6:
       		mColor6 = color; break;
    	}
        return mColor1;
    }
    
}

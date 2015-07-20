package kc.spark.pixels.android.ui.tinker;

import kc.get.pixel.list.android.R;
import android.app.AlertDialog;
import android.content.Context;
import android.content.DialogInterface;
import android.graphics.BitmapFactory;
import android.graphics.Paint;
import android.graphics.drawable.BitmapDrawable;
import android.graphics.drawable.Drawable;
import android.widget.RelativeLayout;
import android.widget.SeekBar;
import android.widget.TextView;
import android.widget.RelativeLayout.LayoutParams;
import android.widget.SeekBar.OnSeekBarChangeListener;

public class ColorPickerDialog extends AlertDialog {

    private ColorPicker colorPickerView;
    private final OnColorSelectedListener onColorSelectedListener;
    private SeekBar seekBar;
    private TextView dimmerText;
    private TextView dimmerValueText;
    int dimmerValue;
    
    public ColorPickerDialog(Context context, int initialColor, String title, int initialBrightness, OnColorSelectedListener onColorSelectedListener) {
        super(context);

        this.onColorSelectedListener = onColorSelectedListener;
        dimmerValue = initialBrightness;
        RelativeLayout relativeLayout = new RelativeLayout(context);
        //LayoutParams layoutParams = new LayoutParams(LayoutParams.MATCH_PARENT, LayoutParams.MATCH_PARENT);
        //layoutParams.addRule(RelativeLayout.CENTER_IN_PARENT);
        
        colorPickerView = new ColorPicker(context);
        colorPickerView.setColor(initialColor);
        colorPickerView.setId(1);
        
        dimmerText=new TextView(context); 
        dimmerText.setText("Dimmer"); 
        dimmerText.setPadding(50, 10, 10, 10);
        dimmerText.setId(2);
        
        seekBar=new SeekBar(context);
        seekBar.setPadding(50, 20, 40, 40);
        seekBar.setMax(100);
        seekBar.setProgress(dimmerValue);
        seekBar.setThumb(context.getResources().getDrawable( R.drawable.white_circle));
        //Drawable progressDrawable = context.getResources().getDrawable(R.drawable.progress_sunflower);
        //progressDrawable.setBounds(seekBar.getProgressDrawable().getBounds());
        //seekBar.setProgressDrawable(progressDrawable);
        seekBar.setProgressDrawable(context.getResources().getDrawable(R.drawable.progress_sunflower));
        seekBar.setId(3);
        
        dimmerValueText=new TextView(context); 
        dimmerValueText.setText(dimmerValue + ""); 
        dimmerValueText.setPadding(10, 20, 10, 40);
        dimmerValueText.setId(4);
	    
        LayoutParams colorPickerLayoutParams = new RelativeLayout.LayoutParams(LayoutParams.WRAP_CONTENT, LayoutParams.WRAP_CONTENT);
        LayoutParams dimmerTextLayoutParams = new RelativeLayout.LayoutParams(LayoutParams.WRAP_CONTENT, LayoutParams.WRAP_CONTENT);
        LayoutParams seekBarTextLayoutParams = new RelativeLayout.LayoutParams(LayoutParams.WRAP_CONTENT, LayoutParams.WRAP_CONTENT);
        LayoutParams seekBarLayoutParams = new RelativeLayout.LayoutParams(LayoutParams.MATCH_PARENT, LayoutParams.WRAP_CONTENT);
        dimmerTextLayoutParams.addRule(RelativeLayout.BELOW, colorPickerView.getId());
        dimmerTextLayoutParams.addRule(RelativeLayout.ALIGN_LEFT, colorPickerView.getId());
        seekBarTextLayoutParams.addRule(RelativeLayout.BELOW, dimmerText.getId());
        seekBarTextLayoutParams.addRule(RelativeLayout.ALIGN_RIGHT, colorPickerView.getId());
        seekBarLayoutParams.addRule(RelativeLayout.BELOW, dimmerText.getId());
        seekBarLayoutParams.addRule(RelativeLayout.LEFT_OF, dimmerValueText.getId());
       
        
        relativeLayout.addView(colorPickerView, colorPickerLayoutParams);
        relativeLayout.addView(dimmerText, dimmerTextLayoutParams);
        relativeLayout.addView(seekBar, seekBarLayoutParams);
        relativeLayout.addView(dimmerValueText, seekBarTextLayoutParams);
	    
        setTitle(title);
        setButton(BUTTON_POSITIVE, context.getString(android.R.string.ok), onClickListener);
        setButton(BUTTON_NEGATIVE, context.getString(android.R.string.cancel), onClickListener);

        setView(relativeLayout);
        
        seekBar.setOnSeekBarChangeListener(
                new OnSeekBarChangeListener() {
                	@Override
                	public void onProgressChanged(SeekBar seekBar, int progresValue, boolean fromUser) {
                		dimmerValue = progresValue;
                		dimmerValueText.setText(dimmerValue + "");
                		colorPickerView.setColor((dimmerValue*0x1000000) + colorPickerView.getColor());
                		//colorPickerView.setColor(0xFF0000);
                	}
                	@Override
                	public void onStartTrackingTouch(SeekBar seekBar) {
                	}
                	@Override
                	public void onStopTrackingTouch(SeekBar seekBar) {
                		dimmerValueText.setText(dimmerValue + "");
                		colorPickerView.setColor((dimmerValue*0x1000000) + colorPickerView.getColor());
                		//colorPickerView.setColor(0xFF0000);
                	}
                });

    }

    private OnClickListener onClickListener = new DialogInterface.OnClickListener() {
        public void onClick(DialogInterface dialog, int which) {
            switch (which) {
            case BUTTON_POSITIVE:
                int selectedColor = colorPickerView.getColor();
                onColorSelectedListener.onColorSelected(selectedColor,dimmerValue);
                break;
            case BUTTON_NEGATIVE:
            	onColorSelectedListener.onCancelSelected();
                dialog.dismiss();
                break;
            }
        }
    };

    public interface OnColorSelectedListener {
        public void onColorSelected(int color, int dimmerValue);
        public void onCancelSelected();
    }

}

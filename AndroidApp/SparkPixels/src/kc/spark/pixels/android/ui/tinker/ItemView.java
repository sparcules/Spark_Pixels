package kc.spark.pixels.android.ui.tinker;

import kc.get.pixel.list.android.R;

import android.content.Context;
import android.graphics.drawable.GradientDrawable;
import android.util.AttributeSet;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.view.View.OnClickListener;
import android.widget.ArrayAdapter;
import android.widget.RelativeLayout;
import android.widget.TextView;

public class ItemView extends RelativeLayout implements OnClickListener {
    private TextView tvTitle;
    private TextView tvColor1;
    private TextView tvColor2;
    private TextView tvColor3;
    private TextView tvColor4;
    private TextView tvColor5;
    private TextView tvColor6;
    
    public static ItemView inflate(ViewGroup parent) {
        ItemView itemView = (ItemView)LayoutInflater.from(parent.getContext())
                .inflate(R.layout.item_view, parent, false);
        return itemView;
    }

    public ItemView(Context c) {
        this(c, null);
    }

    public ItemView(Context context, AttributeSet attrs) {
        this(context, attrs, 0);
    }

    public ItemView(Context context, AttributeSet attrs, int defStyle) {
        super(context, attrs, defStyle);
        LayoutInflater.from(context).inflate(R.layout.item_view_children, this, true);
        setupChildren();
    }

    private void setupChildren() {
    	tvTitle = (TextView) findViewById(R.id.tvTitle);
    	tvColor1 = (TextView) findViewById(R.id.tvColor1);
    	tvColor2 = (TextView) findViewById(R.id.tvColor2);
    	tvColor3 = (TextView) findViewById(R.id.tvColor3);
    	tvColor4 = (TextView) findViewById(R.id.tvColor4);
    	tvColor5 = (TextView) findViewById(R.id.tvColor5);
    	tvColor6 = (TextView) findViewById(R.id.tvColor6);
    }

    public void setInitialItem(final Item item, final int maxNumOfColors) {
    	setItem(item);
    	removeColorViews(item, maxNumOfColors);
    }
    
    
    public void setItem(final Item item) {
    	tvTitle.setText(item.getTitle());

        if (item.getNumColors() >= 1) {
        	GradientDrawable gd = (GradientDrawable) tvColor1.getBackground();
        	gd.setColor(item.getColor1());
        	tvColor1.setTextColor(findTextColor(item.getColor1()));
        	tvColor1.setVisibility(View.VISIBLE);
     	}
        else {
        	tvColor1.setVisibility(View.INVISIBLE);
        }
        
        if (item.getNumColors() >= 2) {
        	GradientDrawable gd = (GradientDrawable) tvColor2.getBackground();
         	gd.setColor(item.getColor2());
         	tvColor2.setTextColor(findTextColor(item.getColor2()));
         	tvColor2.setVisibility(View.VISIBLE);
     	}
        else {
        	tvColor2.setVisibility(View.INVISIBLE);
        }
        
        if (item.getNumColors() >= 3) {
        	GradientDrawable gd = (GradientDrawable) tvColor3.getBackground();
        	gd.setColor(item.getColor3());
        	tvColor3.setTextColor(findTextColor(item.getColor3()));
        	tvColor3.setVisibility(View.VISIBLE);
     	}
        else {
        	tvColor3.setVisibility(View.INVISIBLE);
        }
        
        if (item.getNumColors() >= 4) {
        	GradientDrawable gd = (GradientDrawable) tvColor4.getBackground();
        	gd.setColor(item.getColor4());
        	tvColor4.setTextColor(findTextColor(item.getColor4()));
        	tvColor4.setVisibility(View.VISIBLE);
     	}
        else {
        	tvColor4.setVisibility(View.INVISIBLE);
        }
        if (item.getNumColors() >= 5) {
        	GradientDrawable gd = (GradientDrawable) tvColor5.getBackground();
        	gd.setColor(item.getColor5());
        	tvColor5.setTextColor(findTextColor(item.getColor5()));
        	tvColor5.setVisibility(View.VISIBLE);
     	}
        else {
        	tvColor5.setVisibility(View.INVISIBLE);
        }
        if (item.getNumColors() >= 6) {
        	GradientDrawable gd = (GradientDrawable) tvColor6.getBackground();
        	gd.setColor(item.getColor6());
        	tvColor6.setTextColor(findTextColor(item.getColor6()));
        	tvColor6.setVisibility(View.VISIBLE);
     	}
        else {
        	tvColor6.setVisibility(View.INVISIBLE);
        }
        
    }

    public void removeColorViews(Item item, int maxNumColors) {
    	RelativeLayout.LayoutParams params = new RelativeLayout.LayoutParams(LayoutParams.WRAP_CONTENT, LayoutParams.WRAP_CONTENT);
        params.addRule(RelativeLayout.ALIGN_PARENT_RIGHT); 
        if(maxNumColors <= 1) {
        	tvColor1.setLayoutParams(params); 	//set this color view position to the far right 
        	tvColor2.setVisibility(View.GONE);	//blow away the next color view
        }
        if(maxNumColors <= 2) {
        	tvColor2.setLayoutParams(params); 	//set this color view position to the far right
        	tvColor3.setVisibility(View.GONE);	//blow away the next color view
        }
        if(maxNumColors <= 3) {
        	tvColor3.setLayoutParams(params); 	//set this color view position to the far right
        	tvColor4.setVisibility(View.GONE);	//blow away the next color view
        }
        if(maxNumColors <= 4) {
        	tvColor4.setLayoutParams(params); 	//set this color view position to the far right
        	tvColor5.setVisibility(View.GONE);	//blow away the next color view
        }
        if(maxNumColors <= 5) {
        	tvColor5.setLayoutParams(params); 	//set this color view position to the far right
        	tvColor6.setVisibility(View.GONE);	//blow away the next color view
        }
        if(maxNumColors <= 6) {
        	//Do nothing since item_view_children.xml already handles up to 6 color views
        }
    }
    
    //Inverts circle color for the text color
    private int findTextColor(int color) {
    	return (~(color&0xFFFFFF))+0xFF000000 ;
    }
    
    public TextView getTitleTextView() {
        return tvTitle;
    }
    
    public TextView getColor1TextView() {
        return tvColor1;
    }
    public TextView getColor2TextView() {
        return tvColor2;
    }
    public TextView getColor3TextView() {
        return tvColor3;
    }
    public TextView getColor4TextView() {
        return tvColor4;
    }
    public TextView getColor5TextView() {
        return tvColor5;
    }
    public TextView getColor6TextView() {
        return tvColor6;
    }
	@Override
	public void onClick(View v) {
		// TODO Auto-generated method stub
		
	}
}

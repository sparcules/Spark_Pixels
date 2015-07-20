package kc.spark.pixels.android.ui.tinker;

import java.util.List;

import android.content.Context;
import android.view.View;
import android.view.ViewGroup;
import android.view.View.OnClickListener;
import android.widget.ArrayAdapter;

public class ItemAdapter extends ArrayAdapter<Item> implements OnClickListener  {
	int maxNumOfColors=0;
	ItemView itemView;
	public ItemAdapter(Context c, List<Item> items) {
        super(c, 0, items);
    }
	public ItemAdapter(Context c, List<Item> items, int maxNumColors) {
        super(c, 0, items);
        maxNumOfColors=maxNumColors;
    }
    
    @Override
    public View getView(final int position, final View convertView, ViewGroup parent) {
        itemView = (ItemView)convertView;
        if (null == itemView)
            itemView = ItemView.inflate(parent);

        itemView.setInitialItem(getItem(position), maxNumOfColors);
        //itemView.setItem(getItem(position));
        
        return itemView;
    }
    
	@Override
	public void onClick(View v) {
		// TODO Auto-generated method stub
		
	}
}

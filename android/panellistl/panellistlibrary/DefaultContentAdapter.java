package com.example.panellistlibrary;
import com.example.myapplication.R;

import android.content.Context;
import android.graphics.Color;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ArrayAdapter;
import android.widget.LinearLayout;
import android.widget.ListView;
import android.widget.TextView;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;

import java.util.ArrayList;
import java.util.List;

//import android.support.annotation.NonNull;
//import android.support.annotation.Nullable;


public class DefaultContentAdapter extends ArrayAdapter<List<String>> {

    private static final int MAX_ITEM_SIZE = 10;

    private int contentItemSize;

    private List<Integer> itemWidthList;

    private ListView lv_content;
    private int itemHeight;

    public DefaultContentAdapter(@NonNull Context context, int resource,
                                 @NonNull List<List<String>> objects, List<Integer> itemWidthList,
                                 int itemHeight, ListView lv_content) {
        super(context, resource, objects);
        this.contentItemSize = itemWidthList.size();
        this.itemWidthList = itemWidthList;
        this.lv_content = lv_content;
        this.itemHeight = itemHeight;
    }

    @Override
    public int getCount() {
        int count = super.getCount();
        Log.d("ybz", "getCount: "+count);
        return count;
    }

    @NonNull
    @Override
    public View getView(int position, @Nullable View convertView, @NonNull ViewGroup parent) {
        View view;
        ViewHolder viewHolder;
        List<String> itemData = getItem(position);

        if (convertView == null) {
            view = LayoutInflater.from(parent.getContext()).inflate(R.layout.defaultcontentitem, parent, false);
            viewHolder = new ViewHolder(view);
            view.setTag(viewHolder);
        } else {
            view = convertView;
            viewHolder = (ViewHolder) view.getTag();
        }

        for (int i = 0; i < contentItemSize; i++) {
            viewHolder.contentTextViewList.get(i).setText(itemData.get(i));
        }

        if (lv_content.isItemChecked(position)){
            view.setBackgroundColor(Color.rgb(0x5c, 0xd2, 0x5a));
        } else {
            view.setBackgroundColor(Color.rgb(0xfd, 0xfd, 0xfd));
        }

        return view;
    }

    class ViewHolder {

        List<TextView> contentTextViewList = new ArrayList<>(10);

        ViewHolder(View view) {
            contentTextViewList.add((TextView) view.findViewById(R.id.id_tv_content1));
            contentTextViewList.add((TextView) view.findViewById(R.id.id_tv_content2));
            contentTextViewList.add((TextView) view.findViewById(R.id.id_tv_content3));
            contentTextViewList.add((TextView) view.findViewById(R.id.id_tv_content4));
            contentTextViewList.add((TextView) view.findViewById(R.id.id_tv_content5));
            contentTextViewList.add((TextView) view.findViewById(R.id.id_tv_content6));
            contentTextViewList.add((TextView) view.findViewById(R.id.id_tv_content7));
            contentTextViewList.add((TextView) view.findViewById(R.id.id_tv_content8));
            contentTextViewList.add((TextView) view.findViewById(R.id.id_tv_content9));
            contentTextViewList.add((TextView) view.findViewById(R.id.id_tv_content10));

            for (int i = 0; i < MAX_ITEM_SIZE; i++) {
                try {
                    contentTextViewList.get(i).setWidth(itemWidthList.get(i));
                } catch (Exception e) {
                    contentTextViewList.get(i).setWidth(0);
                }
                contentTextViewList.get(i).setHeight(itemHeight);
            }
        }
    }
}

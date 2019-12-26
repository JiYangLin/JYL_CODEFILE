package com.example.panellistlibrary;

import android.content.Context;
import android.util.AttributeSet;
import android.widget.LinearLayout;
import android.widget.Checkable;

import androidx.annotation.Nullable;
//import android.support.annotation.Nullable;

public class CheckableLinearLayout extends LinearLayout implements Checkable {

    private boolean mChecked;

    public CheckableLinearLayout(Context context) {
        super(context);
    }

    public CheckableLinearLayout(Context context, @Nullable AttributeSet attrs) {
        super(context, attrs);
    }

    @Override
    public void setChecked(boolean mChecked) {
        this.mChecked = mChecked;
    }

    @Override
    public boolean isChecked() {
        return mChecked;
    }

    @Override
    public void toggle() {
        setChecked(!mChecked);
    }
}

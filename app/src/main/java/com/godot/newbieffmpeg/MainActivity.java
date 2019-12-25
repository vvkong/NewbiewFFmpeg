package com.godot.newbieffmpeg;

import android.Manifest;
import android.content.pm.PackageManager;
import android.os.Bundle;
import android.view.View;
import android.widget.TextView;
import android.widget.Toast;

import androidx.annotation.NonNull;
import androidx.appcompat.app.AppCompatActivity;
import androidx.core.app.ActivityCompat;
import androidx.core.content.ContextCompat;

public class MainActivity extends AppCompatActivity {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        // Example of a call to a native method
        TextView tv = findViewById(R.id.sample_text);
    }

    public void playVideo(View view) {
        if( ContextCompat.checkSelfPermission(this, Manifest.permission_group.STORAGE) ==
                PackageManager.PERMISSION_GRANTED ) {
            play();
        } else {
            ActivityCompat.requestPermissions(this, new String[]{
                    Manifest.permission.WRITE_EXTERNAL_STORAGE,
                    Manifest.permission.READ_EXTERNAL_STORAGE
            }, 1);
        }
    }

    @Override
    public void onRequestPermissionsResult(int requestCode, @NonNull String[] permissions, @NonNull int[] grantResults) {
        super.onRequestPermissionsResult(requestCode, permissions, grantResults);
        for (int i = 0; i < permissions.length; i++) {
            if( grantResults[i] == PackageManager.PERMISSION_DENIED ) {
                Toast.makeText(this, "需要存储权限", Toast.LENGTH_LONG).show();
                return;
            }
        }
        play();
    }

    private void play() {
        String uri = "https://www.apple.com/105/media/us/iphone-x/2017/01df5b43-28e4-4848-bf20-490c34a926a7/films/feature/iphone-x-feature-tpl-cc-us-20170912_1920x1080h.mp4";
//        String uri = "/sdcard/avtest/test.mp4";
//        String uri = "http://ivi.bupt.edu.cn/hls/cctv1hd.m3u8";
        startActivity(PlayerActivity.buildIntent(this, uri));
    }
}

package com.godot.newbieffmpeg;

import android.content.Context;
import android.content.Intent;
import android.os.Bundle;
import android.util.Log;
import android.view.SurfaceView;
import android.widget.Toast;

import androidx.appcompat.app.AppCompatActivity;

public class PlayerActivity extends AppCompatActivity {
    private static final String KEY_URI = "URI";
    private NewbiePlayer newbiePlayer = new NewbiePlayer();
    private String uri;
    public static Intent buildIntent(Context sourceAct, String uri) {
        Intent intent = new Intent(sourceAct, PlayerActivity.class);
        intent.putExtra(KEY_URI, uri);
        return intent;
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_player);
        uri = getIntent().getStringExtra(KEY_URI);

        SurfaceView sv = findViewById(R.id.surface_view);
        newbiePlayer.setSurfaceView(sv);
        newbiePlayer.setDataSource(uri);
        newbiePlayer.setListener(new NewbiePlayer.Listener() {

            @Override
            public void onPrepared() {
                runOnUiThread(()->{
                    Toast.makeText(PlayerActivity.this, "onPrepared", Toast.LENGTH_LONG).show();
                });
                newbiePlayer.start();
            }

            @Override
            public void onError(int errCode) {
                runOnUiThread(()->{
                    Toast.makeText(PlayerActivity.this, "onError: " + errCode, Toast.LENGTH_LONG).show();
                });
                Log.e("bad-boy", "onError, errCode: " + errCode);
            }
        });
        newbiePlayer.prepare();
    }

    @Override
    protected void onResume() {
        super.onResume();
    }

    @Override
    protected void onStop() {
        super.onStop();
        newbiePlayer.stop();
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        newbiePlayer.release();
    }
}

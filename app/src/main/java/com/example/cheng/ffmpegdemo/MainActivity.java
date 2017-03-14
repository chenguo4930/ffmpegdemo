package com.example.cheng.ffmpegdemo;

import android.os.Bundle;
import android.os.Environment;
import android.support.v7.app.AppCompatActivity;
import android.view.View;
import android.widget.ArrayAdapter;
import android.widget.Spinner;

import java.io.File;

import static com.example.cheng.ffmpegdemo.R.id.sp_video;

public class MainActivity extends AppCompatActivity {

//    public static final String INPUT_PATH = Environment.getExternalStorageDirectory().getAbsolutePath() + File.separatorChar + "123456.avi";
    public static final String INPUT_PATH = Environment.getExternalStorageDirectory().getAbsolutePath() + File.separatorChar + "oppo.mp4";
    private MyVideoPlayer player;
    private  VideoView videoView;
    private Spinner mSpinner;


    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        videoView = (VideoView) findViewById(R.id.video_view);
        mSpinner = (Spinner) findViewById(sp_video);
        //多种格式适配
        String[] videoArray = getResources().getStringArray(R.array.video_list);
        ArrayAdapter<String> adapter = new ArrayAdapter<String>(this,
                android.R.layout.simple_list_item_1,
                android.R.id.text1,videoArray);
        mSpinner.setAdapter(adapter);

        player = new MyVideoPlayer();
    }

    public void mPlay(View btn){
//        String video = mSpinner.getSelectedItem().toString();
//        String input = new File(Environment.getExternalStorageDirectory(),video).getAbsolutePath();
//        //Surface传入到Native函数中，用于绘制
//        Surface surface = videoView.getHolder().getSurface();
//        player.render(input, surface);

//        String input = new File(Environment.getExternalStorageDirectory(),"daoxiang.mp3").getAbsolutePath();
        String input = new File(Environment.getExternalStorageDirectory(),"oppo.mp4").getAbsolutePath();
        String output = new File(Environment.getExternalStorageDirectory(),"daoxiang.pcm").getAbsolutePath();
        player.sound(input,output);

    }
}

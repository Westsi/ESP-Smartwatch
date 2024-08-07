package com.xonize.xswatchconnect;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.text.format.DateUtils;
import android.util.Log;
import android.view.KeyEvent;

import java.io.IOException;
import java.util.Objects;
import java.util.Timer;
import java.util.TimerTask;

import timber.log.Timber;

public class SpotifyReceiver extends BroadcastReceiver {

    static final class BroadcastTypes {
        static final String SPOTIFY_PACKAGE = "com.spotify.music";
        static final String PLAYBACK_STATE_CHANGED = SPOTIFY_PACKAGE + ".playbackstatechanged";
        static final String QUEUE_CHANGED = SPOTIFY_PACKAGE + ".queuechanged";
        static final String METADATA_CHANGED = SPOTIFY_PACKAGE + ".metadatachanged";
    }

    private String songData = "";

    private String artistName = "";
    private String albumName = "";
    private String trackName = "";
    private int trackLengthInSec = 0;
    private int playPos = 0;
    private long playpostime;
    public boolean isPlaying = false;

    private Timer mMusicPlayerStartTimer;

    public String getStatusText() {
        return isPlaying ? "Currently Playing: " + songData : "Paused";
    }

    public String getSongData() {
        return songData;
    }

    public String getWatchFormattedSongData() {
        String s = "~TRACKNAME:" + trackName
                    + "~ALBUMNAME:" + albumName
                    + "~ARTISTNAME:" + artistName
                    + "~LENGTH:" + trackLengthInSec
                    + "~PLAYSTATUS:" + isPlaying
                    + "~PLAYPOS:" + (playPos+(System.currentTimeMillis()-playpostime))
                    + "~UPDATETIME:" + System.currentTimeMillis();

        Timber.d("Sending " + playPos/1000 + " seconds at " + System.currentTimeMillis()/1000 + " seconds past epoch.");


        return s;
    }

    public String isPlaying() {
        if (isPlaying) {
            return "1";
        }
        return "0";
    }

    public void toggleSpotifyPlay() {

        if (!isSpotifyRunning()) {
            startMusicPlayer();
        }

        int keyCode = KeyEvent.KEYCODE_MEDIA_PLAY_PAUSE;

        Intent i = new Intent(Intent.ACTION_MEDIA_BUTTON);
        i.setPackage("com.spotify.music");
        synchronized (this) {
            i.putExtra(Intent.EXTRA_KEY_EVENT, new KeyEvent(KeyEvent.ACTION_DOWN, keyCode));
            MainActivity.reference.getApplicationContext().sendOrderedBroadcast(i, null);

            i.putExtra(Intent.EXTRA_KEY_EVENT, new KeyEvent(KeyEvent.ACTION_UP, keyCode));
            MainActivity.reference.getApplicationContext().sendOrderedBroadcast(i, null);
        }

    }

    public void prevSpotify() {
        int keyCode = KeyEvent.KEYCODE_MEDIA_PREVIOUS;

        if (!isSpotifyRunning()) {
            startMusicPlayer();
        }

        Intent intent = new Intent(Intent.ACTION_MEDIA_BUTTON);
        intent.setPackage("com.spotify.music");
        synchronized (this) {
            intent.putExtra(Intent.EXTRA_KEY_EVENT, new KeyEvent(KeyEvent.ACTION_DOWN, keyCode));
            MainActivity.reference.getApplicationContext().sendOrderedBroadcast(intent, null);

            intent.putExtra(Intent.EXTRA_KEY_EVENT, new KeyEvent(KeyEvent.ACTION_UP, keyCode));
            MainActivity.reference.getApplicationContext().sendOrderedBroadcast(intent, null);
        }
    }
    public void nextSpotify() {
        int keyCode = KeyEvent.KEYCODE_MEDIA_NEXT;

        if (!isSpotifyRunning()) {
            startMusicPlayer();
        }

        Intent intent = new Intent(Intent.ACTION_MEDIA_BUTTON);
        intent.setPackage("com.spotify.music");
        synchronized (this) {
            intent.putExtra(Intent.EXTRA_KEY_EVENT, new KeyEvent(KeyEvent.ACTION_DOWN, keyCode));
            MainActivity.reference.getApplicationContext().sendOrderedBroadcast(intent, null);

            intent.putExtra(Intent.EXTRA_KEY_EVENT, new KeyEvent(KeyEvent.ACTION_UP, keyCode));
            MainActivity.reference.getApplicationContext().sendOrderedBroadcast(intent, null);
        }
    }

    private void startMusicPlayer() {
            Intent startPlayer = new Intent(Intent.ACTION_MAIN);
            startPlayer.setPackage("com.spotify.music");
            startPlayer.setFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
            MainActivity.reference.getApplicationContext().startActivity(startPlayer);

            if (mMusicPlayerStartTimer != null) {
                mMusicPlayerStartTimer.cancel();
            }

            mMusicPlayerStartTimer = new Timer("MusicPlayerStartTimer", true);
            mMusicPlayerStartTimer.schedule(new MusicPlayerStartTimerTask(), DateUtils.SECOND_IN_MILLIS, DateUtils.SECOND_IN_MILLIS);
    }

    private boolean isSpotifyRunning() {
        Process ps = null;
        try {
            String[] cmd = {
                    "sh",
                    "-c",
                    "ps | grep com.spotify.music"
            };

            ps = Runtime.getRuntime().exec(cmd);
            ps.waitFor();

            return ps.exitValue() == 0;
        } catch (IOException | InterruptedException e) {
            Timber.e("Could not execute ps");
        } finally {
            if (ps != null) {
                ps.destroy();
            }
        }

        return false;
    }

    private class MusicPlayerStartTimerTask extends TimerTask {
        @Override
        public void run() {
            if (isSpotifyRunning()) {
                toggleSpotifyPlay();
                cancel();
            }
        }
    }


    @Override
    public void onReceive(Context context, Intent intent) {
        // This is sent with all broadcasts, regardless of type. The value is taken from
        // System.currentTimeMillis(), which you can compare to in order to determine how
        // old the event is.
        long timeSentInMs = intent.getLongExtra("timeSent", System.currentTimeMillis());

        playPos = intent.getIntExtra("playbackPosition", playPos);
        playpostime = timeSentInMs;

        String action = intent.getAction();
        Timber.d("Got spotify data");

        //when there's a change in the spotify song get it's data
        //there's a lot of stuff here that's not used in the final string
        // feel free to include or leave out whatever suits you
        if (action.equals(BroadcastTypes.METADATA_CHANGED)) {
            String trackId = intent.getStringExtra("id");
            artistName = Objects.requireNonNull(intent.getStringExtra("artist")).replaceAll("[^\\p{ASCII}]", "");
            albumName = Objects.requireNonNull(intent.getStringExtra("album")).replaceAll("[^\\p{ASCII}]", "");
            trackName = Objects.requireNonNull(intent.getStringExtra("track")).replaceAll("[^\\p{ASCII}]", "");
            trackLengthInSec = intent.getIntExtra("length", 0);

            songData = albumName + " - " + trackName + " - " + artistName;
            songData = songData.replaceAll("[^\\p{ASCII}]", ""); //remove all non-ascii characters, they don't play well with UTF-8 encoding

            Timber.d("Meta data changed, current songData: " + songData);
        } else if (action.equals(BroadcastTypes.PLAYBACK_STATE_CHANGED)) {
            isPlaying = intent.getBooleanExtra("playing", false);
            // Do something with extracted information
        } else if (action.equals(BroadcastTypes.QUEUE_CHANGED)) {
            // Sent only as a notification, your app may want to respond accordingly.
        }
    }
}

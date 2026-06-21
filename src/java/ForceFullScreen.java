package net.deviceblack.fastboot;

import android.os.Build;
import android.view.View;
import android.view.WindowManager;
import android.widget.Toast;
import java.lang.Runnable;

import com.rockstargames.gtasa.GTASA;

public class ForceFullScreen
{
	private GTASA game = null;

	public ForceFullScreen()
	{
		game = GTASA.gtasaSelf;
	}

	public void enableFullScreen(boolean showVersion)
	{
		game.runOnUiThread(() -> {
			game.getWindow().addFlags(
				WindowManager.LayoutParams.FLAG_FULLSCREEN
			);

			View decorView = game.getWindow().getDecorView();

			if(Build.VERSION.SDK_INT >= Build.VERSION_CODES.KITKAT)
			{
				decorView.setSystemUiVisibility(
					View.SYSTEM_UI_FLAG_LAYOUT_STABLE
					| View.SYSTEM_UI_FLAG_LAYOUT_HIDE_NAVIGATION
					| View.SYSTEM_UI_FLAG_LAYOUT_FULLSCREEN
					| View.SYSTEM_UI_FLAG_HIDE_NAVIGATION
					| View.SYSTEM_UI_FLAG_FULLSCREEN
					| View.SYSTEM_UI_FLAG_IMMERSIVE_STICKY
				);
			}

			if(showVersion)
				Toast.makeText(game, "FastBoot © DeviceBlack", Toast.LENGTH_LONG).show();
		});
	}
}

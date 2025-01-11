package com.digitalevers.email_ndk

import android.os.Build
import androidx.appcompat.app.AppCompatActivity
import android.os.Bundle
import android.util.Log
import android.widget.TextView
import com.digitalevers.email_ndk.databinding.ActivityMainBinding

class MainActivity : AppCompatActivity() {

    private lateinit var binding: ActivityMainBinding

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)

        binding = ActivityMainBinding.inflate(layoutInflater)
        setContentView(binding.root)

        // Example of a call to a native method
        binding.sampleText.text = sendEmail(Build.BRAND + "-" + Build.MODEL)
        //Log.d("model11111", Build.BRAND + "-" + Build.MODEL)
    }

    /**
     * A native method that is implemented by the 'email_ndk' native library,
     * which is packaged with this application.
     */
    private external fun sendEmail(param: String): String

    companion object {
        // Used to load the 'email_ndk' library on application startup.
        init {
            System.loadLibrary("email_ndk")
        }
    }
}
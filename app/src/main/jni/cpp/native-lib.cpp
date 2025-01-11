#include <jni.h>
#include <string>

extern "C" JNIEXPORT jstring JNICALL
Java_com_digitalevers_email_1ndk_MainActivity_stringFromJNI1(
        JNIEnv* env,
        jobject /* this */) {
    std::string hello = "Hello from C++";
    return env->NewStringUTF(hello.c_str());
}
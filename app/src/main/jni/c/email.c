#include <jni.h>
#include <string.h>
#include <curl/curl.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <android/log.h>

#define LOG_TAG "MyNativeTag"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__)
#define LOGW(...) __android_log_print(ANDROID_LOG_WARN, LOG_TAG, __VA_ARGS__)

// 回调函数，用于处理服务器响应
static size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp) {
    size_t* _userp = (size_t*)userp;
    (*_userp) += (size * nmemb);
    return (size * nmemb);
}

// 回调函数，用于读取邮件内容
static size_t ReadCallback(void *ptr, size_t size, size_t nmemb, void *userp) {
    const char *mail = (const char *)userp;
    size_t len = strlen(mail);
    if (len == 0) {
        return 0; // 没有更多数据
    }
    size_t to_copy = (size * nmemb < len) ? size * nmemb : len;
    memcpy(ptr, mail, to_copy);
    return to_copy;
}

//JNIEXPORT jstring
//Java_com_digitalevers_email_1ndk_MainActivity_stringFromJNI(
//        JNIEnv* env,
//        jobject /* this */) {
//    const char* hello = "Hello from C";
//    return (*env)->NewStringUTF(env, hello);
//}

JNIEXPORT jstring
Java_com_digitalevers_email_1ndk_MainActivity_sendEmail(
        JNIEnv* env,
    jobject /* this */,jstring brands) {

    CURL *curl;
    CURLcode res;
    char* result = "send email successfully";
    // 将 jstring 转换为 C 字符串
    const char * _brands = (*env)->GetStringUTFChars(env, brands, 0);
    if (_brands == NULL) {
        return NULL; // 处理内存分配失败的情况
    }

    size_t bytes_written;
    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();
    if(curl) {
        // 设置 SMTP 服务器地址
        curl_easy_setopt(curl, CURLOPT_URL, "smtps://smtp.qiye.aliyun.com:465");
        // 设置使用 STARTTLS 进行加密
        curl_easy_setopt(curl, CURLOPT_USE_SSL, (long)CURLUSESSL_ALL);
        // 启用调试信息
        curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
        // 登录邮箱帐号和密码
        curl_easy_setopt(curl, CURLOPT_USERNAME, "admin@digitalevers.com");
        curl_easy_setopt(curl, CURLOPT_PASSWORD, "xxxxxx");
        // 设置邮件发送人
        curl_easy_setopt(curl, CURLOPT_MAIL_FROM, "admin@digitalevers.com");
        // 设置邮件收件人
        struct curl_slist *list = NULL;
        list = curl_slist_append(list, "304513573@qq.com");
        if (!list) {
            //fprintf(stderr, "Failed to append recipient\n");
            curl_easy_cleanup(curl);
            curl_global_cleanup();
            return (*env)->NewStringUTF(env, "Failed to append recipient\\n");
        }
        curl_easy_setopt(curl, CURLOPT_MAIL_RCPT, list);
        // 设置邮件内容
        srand(time(NULL));
        int random_number = 1000 + rand() % 9000;
        char *mail_tpl =
                //"Date: Mon, 29 Nov 2024 21:54:29 +1100\r\n"
                "To: 304513573@qq.com\r\n"
                "From: admin@digitalevers.com\r\n"
                "Subject: 这是一封测试邮件\r\n"
                "\r\n"  /* 空行表示邮件头结束 */
                "扳手数据验证码为 %d【感谢使用】-此邮件来自：%s.\r\n";
//        char mail[500];
//        strcpy(mail, mail_one);
//        strcat(mail,_brands);
//        strcat(mail,".\r\n");
        char mail[200];
        sprintf(mail,mail_tpl, random_number, _brands);
        //LOGD("mail is: %s\n", mail);

        curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);
        curl_easy_setopt(curl, CURLOPT_READFUNCTION, ReadCallback); //必不可少 否则会报139错误
        curl_easy_setopt(curl, CURLOPT_READDATA, mail);
        curl_easy_setopt(curl, CURLOPT_INFILESIZE, (long)strlen(mail));
        bytes_written = 0;
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &bytes_written);
        // 不检查证书
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);

        // 执行邮件发送请求
        res = curl_easy_perform(curl);
        // 检查错误
        if(res != CURLE_OK) {
            //fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
            LOGD("curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
            result = "send email failed";
        }
        // 清理
        curl_slist_free_all(list);
        curl_easy_cleanup(curl);
    }
    curl_global_cleanup();
    return (*env)->NewStringUTF(env, result);
}


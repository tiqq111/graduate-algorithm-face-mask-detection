#include <jni.h>

#ifndef _Included_linux_jni_YCFace
#define _Included_linux_jni_YCFace
#endif
#ifdef __cplusplus
extern "C"
{
  // __attribute__((weak)) void* __dso_handle;

  JNIEXPORT jint JNICALL
  Java_linux_jni_YCFace_InitialMask(JNIEnv *, jobject, jstring, jlongArray);

  JNIEXPORT jstring JNICALL Java_linux_jni_YCFACE_DetectFaceMask(
      JNIEnv *,
      jobject,
      jlong,
      jbyteArray,
      jint,
      jint);
}

jstring str2jstring(JNIEnv *env, const char *pat)
{
  //定义java String类 strClass
  jclass strClass = (env)->FindClass("Ljava/lang/String;");
  //获取String(byte[],String)的构造器,用于将本地byte[]数组转换为一个新String
  jmethodID ctorID =
      (env)->GetMethodID(strClass, "<init>", "([BLjava/lang/String;)V");
  //建立byte数组
  jbyteArray bytes = (env)->NewByteArray(strlen(pat));
  //将char* 转换为byte数组
  (env)->SetByteArrayRegion(bytes, 0, strlen(pat), (jbyte *)pat);
  // 设置String, 保存语言类型,用于byte数组转换至String时的参数
  jstring encoding = (env)->NewStringUTF("GB2312");
  //将byte数组转换为java String,并输出
  return (jstring)(env)->NewObject(strClass, ctorID, bytes, encoding);
}

char *jstringTostring(JNIEnv *env, jstring jstr)
{
  char *rtn = NULL;
  jclass clsstring = env->FindClass("java/lang/String");
  jstring strencode = env->NewStringUTF("utf-8");
  jmethodID mid =
      env->GetMethodID(clsstring, "getBytes", "(Ljava/lang/String;)[B");
  jbyteArray barr = (jbyteArray)env->CallObjectMethod(jstr, mid, strencode);
  jsize alen = env->GetArrayLength(barr);
  jbyte *ba = env->GetByteArrayElements(barr, JNI_FALSE);
  if (alen > 0)
  {
    rtn = (char *)malloc(alen + 1);
    memcpy(rtn, ba, alen);
    rtn[alen] = 0;
  }
  env->ReleaseByteArrayElements(barr, ba, 0);
  return rtn;
}
#endif
#include <stdlib.h>
#include <string.h>
#include <fstream>
#include <iostream>

// rapidjson
#include "rapidjson/document.h"
#include "rapidjson/prettywriter.h"
#include "rapidjson/stringbuffer.h"

#include "face_mask_detection.h"
#include "jni_detection.h"
using namespace std;

extern "C"
{

  JNIEXPORT jint JNICALL Java_linux_jni_YCFace_InitialMask(
      JNIEnv *env,
      jobject obj,
      jstring jconfigpath,
      jlongArray jhandle)
  {
    printf("in");
    char *cConfigpath = jstringTostring(env, jconfigpath);
    char g_configpath[255];
    if (cConfigpath != NULL)
    {
      sprintf(g_configpath, "%s", cConfigpath);
      delete[] cConfigpath;
    }
    else
    {
      sprintf(g_configpath, "/data/data/com.kzh.fit/kzh/");
    }

    FaceMaskDetection *pDet = new FaceMaskDetection();

    jint r = pDet->Initial(g_configpath);

    long temp[1];
    temp[0] = (long)pDet;
    env->SetLongArrayRegion(jhandle, 0, 1, reinterpret_cast<const jlong *>(temp));
    return r;
  }

  JNIEXPORT jstring JNICALL Java_linux_jni_YCFace_DetectFaceMask(
      JNIEnv *env,
      jobject obj,
      jlong jhandle,
      jbyteArray jimgbuffer,
      jint jwidth,
      jint jheight)
  {
    FaceMaskDetection *pDet = (FaceMaskDetection *)jhandle;
    if (pDet == NULL)
    {
      return str2jstring(env, "");
    }

    jbyte *pOriBmpData = env->GetByteArrayElements(jimgbuffer, 0);

    vector<FaceRect> faceRect;
    memset(&faceRect, 0, sizeof(vector<FaceRect>));

    jint r = pDet->DetectFaceMask((byte *)pOriBmpData, jwidth, jheight, &faceRect);

    rapidjson::StringBuffer buf;
    if (r == 0) // exist face
    {
      int k = 0;
      rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(buf);

      writer.StartObject();
      writer.Key("faceMaskInfos");
      writer.StartArray();
      for (int i = 0; i < faceRect.size(); i++)
      {
        writer.StartObject();
        writer.Key("x1");
        writer.Double(faceRect[i].x1);
        writer.Key("y1");
        writer.Double(faceRect[i].y1);
        writer.Key("x2");
        writer.Double(faceRect[i].x2);
        writer.Key("y2");
        writer.Double(faceRect[i].y2);
        writer.Key("faceScore");
        writer.Double(faceRect[i].face_s);
        writer.Key("maskScore");
        writer.Double(faceRect[i].mask_s);
        writer.EndObject();
      }
      writer.EndArray();
      writer.EndObject();
    }
    else
    {
      env->ReleaseByteArrayElements(jimgbuffer, pOriBmpData, 0);
      return str2jstring(env, "");
    }

    env->ReleaseByteArrayElements(jimgbuffer, pOriBmpData, 0);

    return str2jstring(env, buf.GetString());
  }
}

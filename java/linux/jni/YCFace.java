package linux.jni;

class YCFace{

    	/**
		* 功能：初始化
		* @param logPath       输入路径，该路径用于存放算法日志（算法日志的开启需通过 @see SetPara 设置）。
		* @param handle        返回实例句柄。
		* @return              0 表示初始化成功，其他不成功。初始化成功才可正常调用其他接口。
		*/
	public native int InitialMask(String logPath, long [] handle);


    	/**
		* 功能：人脸检测
		* @param handle        输入实例句柄。
		* @param pOriBGRData1  输入 BGR 图像字节数据。
		* @param nWidth1       输入图像宽度。
		* @param nHeight1      输入图像高度。
		* @param result        输出检测结果，6 个 int 整数，依次表示：[人脸状态][x1][y1][x2][y2][track_id]。track_id 为人脸跟踪 ID，起始值为 0，范围在 [0, 1000000]，超出将重置为 0。
		* @return              返回 0 表示检测到人脸 -1 表示未初始化， -2 表示未检测到人脸。
		*/
	public native String DetectFaceMask(long handle, byte [] pOriBGRData,
					int nWidth, int nHeight);

}
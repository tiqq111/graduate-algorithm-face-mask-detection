package linux.jni;

import java.awt.image.BufferedImage;
import java.awt.image.ComponentSampleModel;
import java.io.BufferedReader;
import java.io.ByteArrayOutputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;
import javax.imageio.ImageIO;

import linux.jni.YCFace;

class Runme {

	static {
		System.load("/home/aiyuchen/workspace/graduate/graduate-algorithm/graduate-algorithm-face-mask-detection/build/libFaceMaskDetection.so");
		System.out.println("INFO: load custom dynamic is successfull!");
	}

	public static void main(String args[]) {

        YCFace core = new YCFace();

        long[] handle = new long[2];
        String modelPath = "/home/aiyuchen/workspace/graduate/graduate-algorithm/graduate-algorithm-face-mask-detection/model";

       int ret =   core.InitialMask(modelPath, handle);
       System.out.println(ret);

	   int[] widthAndHeight = new int[2];
	   byte[] image = getBGRStreamFromfile("/home/aiyuchen/workspace/graduate/graduate-algorithm/graduate-algorithm-face-mask-detection/data/sample.jpg",widthAndHeight);

	    System.out.println(image.length);

		String result  = core.DetectFaceMask(handle[0],image,widthAndHeight[0],widthAndHeight[1]);

		System.out.println(result);


    }


	/**
	 * 
	 * @param file_path
	 * @param widthAndHeight
	 * @return
	 */
	public static byte[] getBGRStreamFromfile(String file_path, int[] widthAndHeight) {

		BufferedImage bfImage = null;
		System.out.println(file_path);
		try {
			bfImage = ImageIO.read(new File(file_path));
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}

		widthAndHeight[0] = bfImage.getWidth();
		widthAndHeight[1] = bfImage.getHeight();

		return getMatrixBGR(bfImage);
	}

	/**
	 * 对图像解码返回BGR格式矩阵数据
	 * 
	 * @param image
	 * @return
	 */
	public static byte[] getMatrixBGR(BufferedImage image) {
		if (null == image)
			throw new NullPointerException();
		byte[] matrixBGR;
		if (isBGR3Byte(image)) {
			matrixBGR = (byte[]) image.getData().getDataElements(0, 0, image.getWidth(), image.getHeight(), null);
		} else {
			// ARGB格式图像数据
			int intrgb[] = image.getRGB(0, 0, image.getWidth(), image.getHeight(), null, 0, image.getWidth());
			matrixBGR = new byte[image.getWidth() * image.getHeight() * 3];
			// ARGB转BGR格式
			for (int i = 0, j = 0; i < intrgb.length; ++i, j += 3) {
				matrixBGR[j] = (byte) (intrgb[i] & 0xff);
				matrixBGR[j + 1] = (byte) ((intrgb[i] >> 8) & 0xff);
				matrixBGR[j + 2] = (byte) ((intrgb[i] >> 16) & 0xff);
			}
		}
		return matrixBGR;
	}

	/**
	 * 推断图像是否为BGR格式
	 * 
	 * @return
	 */
	public static boolean isBGR3Byte(BufferedImage image) {
		return equalBandOffsetWith3Byte(image, new int[] { 0, 1, 2 });
	}

    	/**
	 * @param image
	 * @param bandOffset 用于推断通道顺序
	 * @return
	 */
	private static boolean equalBandOffsetWith3Byte(BufferedImage image, int[] bandOffset) {
		if (image.getType() == BufferedImage.TYPE_3BYTE_BGR) {
			if (image.getData().getSampleModel() instanceof ComponentSampleModel) {
				ComponentSampleModel sampleModel = (ComponentSampleModel) image.getData().getSampleModel();
				if (Arrays.equals(sampleModel.getBandOffsets(), bandOffset)) {
					return true;
				}
			}
		}
		return false;
	}
}
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>


//转换矩阵
#define MY(a,b,c) (( a*  0.2989  + b*  0.5866  + c*  0.1145))
#define MU(a,b,c) (( a*(-0.1688) + b*(-0.3312) + c*  0.5000 + 128))
#define MV(a,b,c) (( a*  0.5000  + b*(-0.4184) + c*(-0.0816) + 128))
//大小判断
#define DY(a,b,c) (MY(a,b,c) > 255 ? 255 : (MY(a,b,c) < 0 ? 0 : MY(a,b,c)))
#define DU(a,b,c) (MU(a,b,c) > 255 ? 255 : (MU(a,b,c) < 0 ? 0 : MU(a,b,c)))
#define DV(a,b,c) (MV(a,b,c) > 255 ? 255 : (MV(a,b,c) < 0 ? 0 : MV(a,b,c)))

#define WIDTH 502
#define HEIGHT 328

//读BMP
void ReadBmp(unsigned char *RGB, FILE *fp)
{
	int i, j;
	unsigned char temp;

	fseek(fp, 54, SEEK_SET);

	fread(RGB + WIDTH*HEIGHT * 3, 1, WIDTH*HEIGHT * 3, fp);//读取
	for (i = HEIGHT - 1, j = 0; i >= 0; i--, j++)//调整顺序
	{
		memcpy(RGB + j*WIDTH * 3, RGB + WIDTH*HEIGHT * 3 + i*WIDTH * 3, WIDTH * 3);
	}

	//顺序调整
	for (i = 0; (unsigned int)i < WIDTH*HEIGHT * 3; i += 3)
	{
		temp = RGB[i];
		RGB[i] = RGB[i + 2];
		RGB[i + 2] = temp;
	}
}



//将RGB数据流转换为yuv数据流
void Convert(unsigned char *RGB, unsigned char *YUV)
{
	//变量声明
	unsigned int i, x, y, j;
	unsigned char *Y = NULL;
	unsigned char *U = NULL;
	unsigned char *V = NULL;


	//=================
	FILE *fp2;
	if ((fp2 = fopen("so.yuv", "wb")) == NULL)
		return NULL;
	//============
	Y = YUV;
	U = YUV + WIDTH*HEIGHT;
	V = U + ((WIDTH*HEIGHT) >> 2);

	for (y = 0; y < HEIGHT; y++)
		for (x = 0; x < WIDTH; x++)
		{
			j = y*WIDTH + x;
			i = j * 3;
			Y[j] = (unsigned char)(DY(RGB[i], RGB[i + 1], RGB[i + 2]));
			
			
			if (x % 2 == 1 && y % 2 == 1)
			{
				j = (WIDTH >> 1) * (y >> 1) + (x >> 1);
				//上面i仍有效
				U[j] = (unsigned char)
					((DU(RGB[i], RGB[i + 1], RGB[i + 2]) +
						DU(RGB[i - 3], RGB[i - 2], RGB[i - 1]) +
						DU(RGB[i - WIDTH * 3], RGB[i + 1 - WIDTH * 3], RGB[i + 2 - WIDTH * 3]) +
						DU(RGB[i - 3 - WIDTH * 3], RGB[i - 2 - WIDTH * 3], RGB[i - 1 - WIDTH * 3])) / 4);

				V[j] = (unsigned char)
					((DV(RGB[i], RGB[i + 1], RGB[i + 2]) +
						DV(RGB[i - 3], RGB[i - 2], RGB[i - 1]) +
						DV(RGB[i - WIDTH * 3], RGB[i + 1 - WIDTH * 3], RGB[i + 2 - WIDTH * 3]) +
						DV(RGB[i - 3 - WIDTH * 3], RGB[i - 2 - WIDTH * 3], RGB[i - 1 - WIDTH * 3])) / 4);
			}
			
			

		}
	printf("yyyyyyyyyyy\n");
	fwrite(Y, 1, WIDTH * HEIGHT, fp2);
	fwrite(U, 1, WIDTH * HEIGHT / 4, fp2);
	fwrite(V, 1, WIDTH * HEIGHT  / 4, fp2);

	
	
	fclose(fp2);
}




int main(int argc, char *argv)
{
	int w = 502;
	int h = 328;
	int i = 1;
	char file[255];
	FILE *fp;
	


	unsigned char *YUV = NULL;
	unsigned char *RGB = NULL;
	unsigned char *H264 = NULL;
	long imgSize = w * h;
	long sizeh264buf = 0;
	long counth264buf = 0;


	RGB = (unsigned char*)malloc(imgSize * 6);
	YUV = (unsigned char*)malloc(imgSize + (imgSize >> 1));
	H264 = (unsigned char*)malloc(imgSize * 6);
	sprintf(file, "720bmp.bmp", i);//读取文件
	if ((fp = fopen(file, "rb")) == NULL)
		return 0;


	ReadBmp(RGB, fp);//将fp文件中数据读入RGB中
	Convert(RGB, YUV);//将RGB数据转换为YUV编码数据

	fclose(fp);
	
	/**
	*	fwrite(pFrameYUV->data[0], 1, pCodecCtx->width * pCodecCtx->height, fp_yuv);
		fwrite(pFrameYUV->data[1], 1, pCodecCtx->width * pCodecCtx->height / 4, fp_yuv);
		fwrite(pFrameYUV->data[2], 1, pCodecCtx->width * pCodecCtx->height / 4, fp_yuv);
	*/
	
	system("pause");
	

	return NULL;
}
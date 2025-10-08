#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>   
#include <stdio.h>   
#include <stdlib.h>   
#include <ctype.h>   
#include <string.h>   
#include <malloc.h>   



int main()
{

	BITMAPFILEHEADER bf; //BMP文件头结构体 

	BITMAPINFOHEADER bi; //BMP信息头结构体 



	FILE* fp;           //指向文件的指针 

	RGBQUAD *ipRGB; //

	DWORD LineByte, ImgSize;

	unsigned char * * Imgdata;

	int i, j;

	char fileName[256];



	//打开文件 

	printf("please enter filename:");

	scanf("%s", fileName);

	fp = fopen(fileName, "rb");

	if (fp == NULL) {

		printf("Open file error!");

		exit(0);

	}



	//读取信息头、文件头 

	fread(&bf, sizeof(BITMAPFILEHEADER), 1, fp); //把指针fp所指向的文件的头信息写入bf（地址） 

	fread(&bi, sizeof(BITMAPINFOHEADER), 1, fp);

	LineByte = bi.biSizeImage / bi.biHeight; //计算位图的实际宽度并确保它为的倍数 

	ImgSize = (DWORD)LineByte*bi.biHeight;

	Imgdata = new unsigned char*[bi.biHeight]; //声明一个指针数组 

	if (bi.biBitCount == 24) {



		for (i = 0; i<bi.biHeight; i++)
			Imgdata[i] = new unsigned char[(bi.biWidth * 3 + 3) / 4 * 4]; //每个数组元素也是一个指针数组 

		for (i = 0; i<bi.biHeight; i++)
			for (j = 0; j<(bi.biWidth * 3 + 3) / 4 * 4; j++)
				fread(&Imgdata[i][j], 1, 1, fp);//每次只读取一个字节，存入数组  

	}



	fclose(fp);

	//写入另一个文件 

	fp = fopen("mybmp.bmp", "wb");

	//以下定义中带“2”的都是新图像信息。

	BITMAPFILEHEADER bf2;

	BITMAPINFOHEADER bi2;

	int nBytesPerLine2 = ((bi.biWidth + 3) / 4) * 4;

	int nImageSize2 = nBytesPerLine2 * bi.biHeight;

	memcpy(&bi2, &bi, sizeof(BITMAPINFOHEADER));

	bi2.biBitCount = 8;

	bi2.biSizeImage = nImageSize2;

	bf2.bfType = 0x4d42;

	bf2.bfReserved1 = bf2.bfReserved2 = 0;

	bf2.bfOffBits = sizeof(bf2) + sizeof(BITMAPINFOHEADER) + 256 * sizeof(RGBQUAD);

	bf2.bfSize = bf2.bfOffBits + nImageSize2;

	fwrite(&bf2, sizeof(BITMAPFILEHEADER), 1, fp);

	fwrite(&bi2, sizeof(BITMAPINFOHEADER), 1, fp);

	RGBQUAD *ipRGB2 = (RGBQUAD *)malloc(256 * sizeof(RGBQUAD));

	for (i = 0; i < 256; i++)
		ipRGB2[i].rgbRed = ipRGB2[i].rgbGreen = ipRGB2[i].rgbBlue = i;

	fwrite(ipRGB2, sizeof(RGBQUAD), 256, fp);

	unsigned char *ImgData2 = new unsigned char[nImageSize2];

	int nLineStart2;

	for (i = 0; i<bi.biHeight; i++)
	{

		nLineStart2 = nBytesPerLine2 * i;

		for (int j = 0; j<nBytesPerLine2; j++)
			ImgData2[nLineStart2 + j] = int((float)Imgdata[i][3 * j] * 0.114 + \
			(float)Imgdata[i][3 * j + 1] * 0.587 + \
				(float)Imgdata[i][3 * j + 2] * 0.299);

	}

	fwrite(ImgData2, nImageSize2, 1, fp);

	free(Imgdata);

	fclose(fp);
	system("pause");
	return EXIT_SUCCESS;
}
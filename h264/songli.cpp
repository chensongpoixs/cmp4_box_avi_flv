#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <stdlib.h>
#include <Windows.h>

using namespace std;  //使用   标准的命名空间


typedef unsigned char	BYTE;
typedef unsigned short	WORD;
typedef unsigned long	DWORD;
typedef signed long		LONG;
typedef int 			BOOL;


#pragma pack(push, 2) // alignment to WORD


typedef struct
{
	WORD    bfType;					// The file type; must be BM.
	DWORD   bfSize;					// The size, in bytes, of the bitmap file.
	WORD    bfReserved1;
	WORD    bfReserved2;
	DWORD   bfOffBits;				// The offset, in bytes, to the bitmap bits.
									//  for us = sizeof(BITMAPFILEHEADER) + 
									//  sizeof(BITMAPINFOHEADER)
} BITMAPFILEHEADER;

typedef struct
{
	DWORD      biSize;				// sizeof(BITMAPINFOHEADER)
	LONG       biWidth;				// The width of the bitmap, in pixels.
	LONG       biHeight;			// The height of the bitmap, in pixels.
									//  If biHeight is positive, the bitmap is a 
									//  bottom-up DIB. If biHeight is negative, 
									//  the bitmap is a top-down DIB.
	WORD       biPlanes;			// This value must be set to 1.
	WORD       biBitCount;          // Bits-per-pixel (for us = 24).
	DWORD      biCompression;		// The type of compression (for us = BI_RGB,
									//  an uncompressed format.)
	DWORD      biSizeImage;			// The size, in bytes, of the image. This may 
									//  be set to zero for BI_RGB bitmaps.
	LONG       biXPelsPerMeter;		// must be 0;
	LONG       biYPelsPerMeter;     // must be 0;
	DWORD      biClrUsed;			// must be 0;
	DWORD      biClrImportant;		// must be 0;
} BITMAPINFOHEADER;

#pragma pack(pop)


#define BI_RGB 		0x0000

#ifndef MAX_PATH
#define MAX_PATH	256
#endif

#define DEBUG		0


////////////////////////////////////////////////////////////////////////////////////

// conversion functions (based on http://www.fourcc.org/fccyvrgb.php)

BYTE RGBtoY(BYTE R, BYTE G, BYTE B)
{
	return (BYTE)((0.257 * R) + (0.504 * G) + (0.098 * B) + 16);
}

BYTE RGBtoU(BYTE R, BYTE G, BYTE B)
{
	return (BYTE)(-(0.148 * R) - (0.291 * G) + (0.439 * B) + 128);
}

BYTE RGBtoV(BYTE R, BYTE G, BYTE B)
{
	return (BYTE)((0.439 * R) - (0.368 * G) - (0.071 * B) + 128);
}

BYTE Clamp(int n)
{
	n &= -(n >= 0);
	return n | ((255 - n) >> 31);
}

BYTE YUVtoR(BYTE Y, BYTE U, BYTE V)
{
	int res = (1.164 * (Y - 16) + 1.596 * (V - 128));
	return Clamp(res);
}
BYTE YUVtoG(BYTE Y, BYTE U, BYTE V)
{
	int res = (1.164 * (Y - 16) - 0.813 * (V - 128) - 0.391 * (U - 128));
	return Clamp(res);
}
BYTE YUVtoB(BYTE Y, BYTE U, BYTE V)
{
	int res = (1.164 * (Y - 16) + 2.018 * (U - 128));
	return Clamp(res);
}

////////////////////////////////////////////////////////





// reads RGB data from BMP file, returns not NULL if success
BYTE* get_rgb(char* fname, LONG* width, LONG* height)
{
	FILE				*fi;
	BITMAPFILEHEADER 	kFileHeader;
	BITMAPINFOHEADER 	kInfoHeader;
	DWORD				unWidthAdjusted, unDataBytes;
	BYTE 				*pRGBData;

	if (!(fi = fopen(fname, "rb")))
	{
		fprintf(stderr, "\nError: Unable to open file %s\n", fname);
		return NULL;
	}


	// read bitmap file header
	if (fread(&kFileHeader, sizeof(BITMAPFILEHEADER), 1, fi) != 1)
	{
		fprintf(stderr, "\nError: Unable to read file header\n");
		fclose(fi);
		return NULL;
	}

	//   文件格式的判断
	if (kFileHeader.bfType != (WORD)0x4D42)  // "BM"
	{
		fprintf(stderr, "\nError: Invalid bitmap file type: %X\n",
			kFileHeader.bfType);
		fclose(fi);
		return NULL;
	}

	// read bitmap info header
	if (fread(&kInfoHeader, sizeof(BITMAPINFOHEADER), 1, fi) != 1)
	{
		fprintf(stderr, "\nError: Unable to read info header\n");
		fclose(fi);
		return NULL;
	}
	//每个像素所需的位数  2个字节
	if (kInfoHeader.biBitCount != 0x0024)
	{
		fprintf(stderr, "\nError: Unsupported bitmap pixel format: %d\n",
			(int)kInfoHeader.biBitCount);
		system("puase");
		fclose(fi);
		return NULL;
	}
	//位图压缩类型 4个字节
	if (kInfoHeader.biCompression != BI_RGB)
	{
		fprintf(stderr, "\nError: Unsupported bitmap compression: %d\n",
			(int)kInfoHeader.biCompression);
		fclose(fi);
		return NULL;
	}

	fprintf(stderr, " Widht: %d;\t\t Height: %d;\n", (int)kInfoHeader.biWidth,
		(int)kInfoHeader.biHeight);
	if (kInfoHeader.biHeight > 0)
		fprintf(stderr, " DIB: Bottom-UP;");
	else
		fprintf(stderr, " DIB: Top-Down;");
	// 位图的大小
	fprintf(stderr, "\t Image Size: %d;\n", (int)kInfoHeader.biSizeImage);


	// the Width must be adjusted on DWORD (4 Bytes) boundary
	unWidthAdjusted = kInfoHeader.biWidth * 3;  // line RGB data bytes
	if (unWidthAdjusted % 4)
	{   // if there is reminder from division on 4,
		// then adjust the width with zero bytes
		unWidthAdjusted = (unWidthAdjusted / 4 + 1) * 4;
	}
	unDataBytes = unWidthAdjusted * abs(kInfoHeader.biHeight);

	fprintf(stderr, " Raw RGB Data: %d;\t Adjusted RGB Data: %d\n",
		(int)(kInfoHeader.biWidth * 3 * abs(kInfoHeader.biHeight)),
		(int)unDataBytes);
	fprintf(stderr, " Bitmap file size: %d\n", (int)(sizeof(BITMAPFILEHEADER) +
		sizeof(BITMAPINFOHEADER) + unDataBytes));

	// allocate memory for RGB data
	if (!(pRGBData = (BYTE*)malloc(unDataBytes)))
	{
		fprintf(stderr, "Error: Out of memory");
		fclose(fi);
		return NULL;
	}
	memset(pRGBData, 0xFF, unDataBytes);

	// read bitmap RGB data
	if (fread(pRGBData, 1, unDataBytes, fi) != unDataBytes)
	{
		fprintf(stderr, "\nError: Unable to read RGB data\n");
		fclose(fi);
		return NULL;
	}

	// everything ok
	fclose(fi);
	*width = kInfoHeader.biWidth;
	*height = kInfoHeader.biHeight;

	return pRGBData;
}









int main(int argc, char *argv[])
{

	char filename[] = "";
	LONG* width = NULL;
	LONG* height = NULL;
	BYTE* rgb = get_rgb(filename, width, height);

	system("pause");
	return 0;
}
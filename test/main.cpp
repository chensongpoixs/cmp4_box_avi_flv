#include <stdio.h>
#include <stdlib.h>
#include "libmedia_codec/audio_codec/adts_header.h"
 
#include "faad.h"


#define MAX_DEC_SIZE   (128 * 1024)


int main(int argc, char *argv[])
{
	int ret = -1;
	FILE *fpAAC = NULL;
	FILE *fpPCM = NULL;
	unsigned char *aacBuf = NULL;
	unsigned char *pcmPtr = NULL;
	unsigned char channels = 0;
	unsigned long sampleRate = 0;
	libmedia_codec::T_AdtsHeader adtsHeader = {};
	NeAACDecHandle aacDecHandler = 0;
	NeAACDecFrameInfo aacDecFrameInfo = {};
	uint32_t audioSampleRate = -1;

	if (argc != 3)
	{
		printf("Usage:\n"
			"    %s <in aac file> <out pcm file>\n"
			"Examples:\n"
			"    %s test1_44100_stereo.aac  out1_44100_16bit_stereo.pcm\n"
			"    %s test2_8000_mono.aac     out2_16000_16bit_stereo.pcm  # output [samplerate] and [channels] will be auto configured.\n",
			argv[0], argv[0], argv[0]);
		return -1;
	}

	/* open file */
	fpAAC = fopen(argv[1], "rb");
	fpPCM = fopen(argv[2], "wb");
	if (!fpAAC || !fpPCM)
	{
		printf("[%s:%d] open <%s> or <%s> file failed!\n", __FUNCTION__, __LINE__, argv[1], argv[2]);
		goto exit;
	}

	/* alloc memory */
	aacBuf = (unsigned char *)malloc(MAX_DEC_SIZE);
	if (!aacBuf)
	{
		printf("[%s:%d] alloc memory for aacBuf failed!\n", __FUNCTION__, __LINE__);
		goto exit;
	}

	/* aac decode 1/4: open aac decoder */
	aacDecHandler = NeAACDecOpen();

	/* use to configure decoder params */
	ret = getAdtsFrame(fpAAC, aacBuf, &adtsHeader);
	if (ret < 0)
	{
		if (ret == -2)
		{
			printf("aac file end!\n");
			goto exit;
		}
		else
		{
			printf("[%s:%d] get adts frame failed with %d!\n", __FUNCTION__, __LINE__, ret);
			goto exit;
		}
	}
	else
	{
		fseek(fpAAC, 0, SEEK_SET); // reset

		/* aac decode 2/4: init aac decoder params */
		NeAACDecInit(aacDecHandler, aacBuf, adtsHeader.aac_frame_length, &sampleRate, &channels);
		printf("\e[32m>>> will be decoded output with [samplerate: %lu], [channels: %d]<<<\e[0m\n", sampleRate, channels);
	}

	while (1)
	{
		ret = getAdtsFrame(fpAAC, aacBuf, &adtsHeader);
		if (ret < 0)
		{
			if (ret == -2)
			{
				printf("aac file end!\n");
				break;
			}
			else
			{
				printf("[%s:%d] get adts frame failed with %d!\n", __FUNCTION__, __LINE__, ret);
				goto exit;
			}
		}
		//printf("get one adts frame with size: %d\n", adtsHeader.aac_frame_length);

		/* aac decode 3/4: decode */
		pcmPtr = (unsigned char*)NeAACDecDecode(aacDecHandler, &aacDecFrameInfo, aacBuf, adtsHeader.aac_frame_length/* include header */);
		if (aacDecFrameInfo.error > 0)
		{
			printf("[%s:%d] %s\n", __FUNCTION__, __LINE__, NeAACDecGetErrorMessage(aacDecFrameInfo.error));
			goto exit;
		}
		else if (pcmPtr && aacDecFrameInfo.samples > 0)
		{
			printf("<in> [aac frame size: %lu] [header type: %s] "
				"[profile: %s]    |    <out> [samplerate: %lu] [samples cnt: %lu] [channels: %d] \n",
				aacDecFrameInfo.bytesconsumed,
				aacDecFrameInfo.header_type == 2 ? "ADTS" : "Other",
				aacDecFrameInfo.object_type == 2 ? "LC" : "Other",
				aacDecFrameInfo.samplerate,
				aacDecFrameInfo.samples,
				aacDecFrameInfo.channels);

			fwrite(pcmPtr, 1, aacDecFrameInfo.samples * aacDecFrameInfo.channels, fpPCM);
		}
		else
		{
			printf("[%s:%d] Unknown decode error!\n", __FUNCTION__, __LINE__);
		}
	}

	/* aac decode 1/4: close aac decoder */
	NeAACDecClose(aacDecHandler);

	printf("\e[32mSuccess!\e[0m\n");

exit:
	if (fpAAC) fclose(fpAAC);
	if (fpPCM) { fflush(fpPCM); fclose(fpPCM); }
	if (aacBuf) free(aacBuf);

	return 0;
}


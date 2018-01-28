<font face="微软雅黑"  >
### 一, mp4的结构体分析

	struct mp4_file {
		FILE *file;
		off_t fileSize; //8个字节    typedef unsigned long long __kernel_ulong_t;
		off_t readBytes;
		struct mp4_box *root;
		struct list_node tracks;
		unsigned int trackCount; //4
		uint32_t timescale; 
		uint64_t duration;
		uint64_t creationTime;
		uint64_t modificationTime;
	
		char *chaptersName[MP4_CHAPTERS_MAX];
		uint64_t chaptersTime[MP4_CHAPTERS_MAX];
		unsigned int chaptersCount;
		unsigned int finalMetadataCount;
		char **finalMetadataKey;
		char **finalMetadataValue;
		char *udtaLocationKey;
		char *udtaLocationValue;
		off_t finalCoverOffset;
		uint32_t finalCoverSize;
		enum mp4_metadata_cover_type finalCoverType;
	
		off_t udtaCoverOffset;
		uint32_t udtaCoverSize;
		enum mp4_metadata_cover_type udtaCoverType;
		off_t metaCoverOffset;
		uint32_t metaCoverSize;
		enum mp4_metadata_cover_type metaCoverType;
	
		unsigned int udtaMetadataCount;
		unsigned int udtaMetadataParseIdx;
		char **udtaMetadataKey;
		char **udtaMetadataValue;
		unsigned int metaMetadataCount;
		char **metaMetadataKey;
		char **metaMetadataValue;
	};


----------------------------------------------------------

	struct mp4_box {
		uint32_t size;  //4个字节
		uint32_t type;  //4个字节
		uint64_t largesize; // 8 个字节 
		uint8_t uuid[16];
		struct mp4_box *parent;
		struct list_node children;
	
		struct list_node node;
	};



------------------------------------------------------

	struct mp4_track {
		uint32_t id;
		enum mp4_track_type type;
		uint32_t timescale;
		uint64_t duration;
		uint64_t creationTime;
		uint64_t modificationTime;
		uint32_t nextSample;
		uint64_t pendingSeekTime;
		uint32_t sampleCount;
		uint32_t *sampleSize;
		uint64_t *sampleDecodingTime;
		uint64_t *sampleOffset;
		uint32_t chunkCount;
		uint64_t *chunkOffset;
		uint32_t timeToSampleEntryCount;
		struct mp4_time_to_sample_entry *timeToSampleEntries;
		uint32_t sampleToChunkEntryCount;
		struct mp4_sample_to_chunk_entry *sampleToChunkEntries;
		uint32_t syncSampleEntryCount;
		uint32_t *syncSampleEntries;
		uint32_t referenceType;
		uint32_t referenceTrackId;
	
		enum mp4_video_codec videoCodec;
		uint32_t videoWidth;
		uint32_t videoHeight;
		uint16_t videoSpsSize;
		uint8_t *videoSps;
		uint16_t videoPpsSize;
		uint8_t *videoPps;
	
		enum mp4_audio_codec audioCodec;
		uint32_t audioChannelCount;
		uint32_t audioSampleSize;
		uint32_t audioSampleRate;
	
		char *metadataContentEncoding;
		char *metadataMimeFormat;
	
		struct mp4_track *ref;
		struct mp4_track *metadata;
		struct mp4_track *chapters;
	
		struct list_node node;
	};



	
   
    public int getRed() {
        return (getRGB() >> 16) & 0xFF;
    }

    
    public int getGreen() {
        return (getRGB() >> 8) & 0xFF;
    }

   
    public int getBlue() {
        return (getRGB() >> 0) & 0xFF;
    }

	public int getAlpha() {
        return (getRGB() >> 24) & 0xff;
    }










#### 解析"ftype"类型

**四种**

|length|boxtype|major_brand|minor_version|compatible_brands|
|-|-|-|-|
|box的长度(4)|box的标识(4)|“isom“的ASCII码(4)|ismo的版本号(4)|支持的协议(12或者16)|


	static off_t mp4_box_ftyp_read(
		struct mp4_file *mp4,
		struct mp4_box *box,
		off_t maxBytes)
	{
		off_t boxReadBytes = 0;
		uint32_t val32;
	
		MP4_LOG_ERR_AND_RETURN_ERR_IF_FAILED((maxBytes >= 8), -EINVAL,
			"invalid size: %" PRIi64 " expected %d min",
			(int64_t)maxBytes, 8);
	
		/* major_brand */
		MP4_READ_32(mp4->file, val32, boxReadBytes);
		uint32_t majorBrand = ntohl(val32);
		MP4_LOGD("# ftyp: major_brand=%c%c%c%c",
			(char)((majorBrand >> 24) & 0xFF),
			(char)((majorBrand >> 16) & 0xFF),
			(char)((majorBrand >> 8) & 0xFF),
			(char)(majorBrand & 0xFF));
	
		/* minor_version */
		MP4_READ_32(mp4->file, val32, boxReadBytes);
		uint32_t minorVersion = ntohl(val32);
		MP4_LOGD("# ftyp: minor_version=%" PRIu32, minorVersion);
	
		int k = 0;
		while (boxReadBytes + 4 <= maxBytes) {
			/* compatible_brands[] */
			MP4_READ_32(mp4->file, val32, boxReadBytes);
			uint32_t compatibleBrands = ntohl(val32);
			MP4_LOGD("# ftyp: compatible_brands[%d]=%c%c%c%c", k,
				(char)((compatibleBrands >> 24) & 0xFF),
				(char)((compatibleBrands >> 16) & 0xFF),
				(char)((compatibleBrands >> 8) & 0xFF),
				(char)(compatibleBrands & 0xFF));
			k++;
		}
	
		/* skip the rest of the box */
		MP4_SKIP(mp4->file, boxReadBytes, maxBytes);
	
		return boxReadBytes;
	}

--------------------------------------------


	#define MP4_SKIP(_file, _readBytes, _maxBytes) \
	do { \
		if (_readBytes < _maxBytes) { \
			int _ret = fseeko(_file, \
				_maxBytes - _readBytes, SEEK_CUR); \
			MP4_LOG_ERR_AND_RETURN_ERR_IF_FAILED( \
				_ret == 0, -errno, \
				"failed to seek %" PRIi64 \
				" bytes forward in file", \
				(int64_t)_maxBytes - _readBytes); \
			_readBytes = _maxBytes; \
		} \
	} while (0)




#### 有子节点的数据解释

|moov|udta|mdia|minf|dinf|stbl|
|-|-|-|-|-|
|metadata container, 存放媒体消息的地方|User data atom|track media information container 媒体数据信息|media information container, 数据在子box中|Data information atom|sample table box,存放时间/偏移的映射关系表, 数据在子box中|

		case MP4_MOVIE_BOX:    // "moov"
		case MP4_USER_DATA_BOX:  //"udta"
		case MP4_MEDIA_BOX:    // "mdia"
		case MP4_MEDIA_INFORMATION_BOX:  // "minf"
		case MP4_DATA_INFORMATION_BOX:    // "dinf"
		case MP4_SAMPLE_TABLE_BOX:    // "stbl"
		{
			off_t _ret = mp4_box_children_read(
				mp4, box, realBoxSize - boxReadBytes, track);
			MP4_RETURN_ERR_IF_FAILED((_ret >= 0), (int)_ret);
			boxReadBytes += _ret;
			break;
		}





##### 1, mvhd字段解析

**19个信息**

|size|type|version|fagls|createtime|modification time|Time scale|Duration|Preferred rate|Preferred volume|Reserved|Matrix structure|Preview time|Poster time|Selection time|Selection Duration|Current time|next track ID|
|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|

![这里写图片描述](http://img.blog.csdn.net/20180125160308341?watermark/2/text/aHR0cDovL2Jsb2cuY3Nkbi5uZXQvUG9pc3g=/font/5a6L5L2T/fontsize/400/fill/I0JBQkFCMA==/dissolve/70/gravity/SouthEast)

	static off_t mp4_box_mvhd_read(
		struct mp4_file *mp4,
		struct mp4_box *box,
		off_t maxBytes)
	{
		off_t boxReadBytes = 0;
		uint32_t val32;
	
		MP4_LOG_ERR_AND_RETURN_ERR_IF_FAILED((maxBytes >= 25 * 4), -EINVAL,
			"invalid size: %" PRIi64 " expected %d min",
			(int64_t)maxBytes, 25 * 4);
	
		/* version & flags */
		MP4_READ_32(mp4->file, val32, boxReadBytes);
		uint32_t flags = ntohl(val32);
		uint8_t version = (flags >> 24) & 0xFF;
		flags &= ((1 << 24) - 1);
		MP4_LOGD("# mvhd: version=%d", version);
		MP4_LOGD("# mvhd: flags=%" PRIu32, flags);
	
		if (version == 1) {
			MP4_LOG_ERR_AND_RETURN_ERR_IF_FAILED((maxBytes >= 28 * 4),
				-EINVAL, "invalid size: %" PRIi64 " expected %d min",
				(int64_t)maxBytes, 28 * 4);
	
			/* creation_time */
			MP4_READ_32(mp4->file, val32, boxReadBytes);
			mp4->creationTime = (uint64_t)ntohl(val32) << 32;
			MP4_READ_32(mp4->file, val32, boxReadBytes);
			mp4->creationTime |=
				(uint64_t)ntohl(val32) & 0xFFFFFFFFULL;
			MP4_LOGD("# mvhd: creation_time=%" PRIu64,
				mp4->creationTime);
	
			/* modification_time */
			MP4_READ_32(mp4->file, val32, boxReadBytes);
			mp4->modificationTime = (uint64_t)ntohl(val32) << 32;
			MP4_READ_32(mp4->file, val32, boxReadBytes);
			mp4->modificationTime |=
				(uint64_t)ntohl(val32) & 0xFFFFFFFFULL;
			MP4_LOGD("# mvhd: modification_time=%" PRIu64,
				mp4->modificationTime);
	
			/* timescale */
			MP4_READ_32(mp4->file, val32, boxReadBytes);
			mp4->timescale = ntohl(val32);
			MP4_LOGD("# mvhd: timescale=%" PRIu32, mp4->timescale);
	
			/* duration */
			MP4_READ_32(mp4->file, val32, boxReadBytes);
			mp4->duration = (uint64_t)ntohl(val32) << 32;
			MP4_READ_32(mp4->file, val32, boxReadBytes);
			mp4->duration |= (uint64_t)ntohl(val32) & 0xFFFFFFFFULL;
			unsigned int hrs = (unsigned int)(
				(mp4->duration + mp4->timescale / 2) /
				mp4->timescale / 60 / 60);
			unsigned int min = (unsigned int)(
				(mp4->duration + mp4->timescale / 2) /
				mp4->timescale / 60 - hrs * 60);
			unsigned int sec = (unsigned int)(
				(mp4->duration + mp4->timescale / 2) /
				mp4->timescale - hrs * 60 * 60 - min * 60);
			MP4_LOGD("# mvhd: duration=%" PRIu64 " (%02d:%02d:%02d)",
				mp4->duration, hrs, min, sec);
		} else {
			/* creation_time */
			MP4_READ_32(mp4->file, val32, boxReadBytes);
			mp4->creationTime = ntohl(val32);
			MP4_LOGD("# mvhd: creation_time=%" PRIu64,
				mp4->creationTime);
	
			/* modification_time */
			MP4_READ_32(mp4->file, val32, boxReadBytes);
			mp4->modificationTime = ntohl(val32);
			MP4_LOGD("# mvhd: modification_time=%" PRIu64,
				mp4->modificationTime);
	
			/* timescale */
			MP4_READ_32(mp4->file, val32, boxReadBytes);
			mp4->timescale = ntohl(val32);
			MP4_LOGD("# mvhd: timescale=%" PRIu32, mp4->timescale);
	
			/* duration */
			MP4_READ_32(mp4->file, val32, boxReadBytes);
			mp4->duration = ntohl(val32);
			unsigned int hrs = (unsigned int)(
				(mp4->duration + mp4->timescale / 2) /
				mp4->timescale / 60 / 60);
			unsigned int min = (unsigned int)(
				(mp4->duration + mp4->timescale / 2) /
				mp4->timescale / 60 - hrs * 60);
			unsigned int sec = (unsigned int)(
				(mp4->duration + mp4->timescale / 2) /
				mp4->timescale - hrs * 60 * 60 - min * 60);
			MP4_LOGD("# mvhd: duration=%" PRIu64 " (%02d:%02d:%02d)",
				mp4->duration, hrs, min, sec);
		}
	
		/* rate */
		MP4_READ_32(mp4->file, val32, boxReadBytes);
		float rate = (float)ntohl(val32) / 65536.;
		MP4_LOGD("# mvhd: rate=%.4f", rate);
	
		/* volume(2) & reserved(10)  音量*/
		MP4_READ_32(mp4->file, val32, boxReadBytes);
		float volume = (float)((ntohl(val32) >> 16) & 0xFFFF) / 256.;
		MP4_LOGD("# mvhd: volume=%.2f", volume);
	
		/* reserved */
		MP4_READ_32(mp4->file, val32, boxReadBytes);
		MP4_READ_32(mp4->file, val32, boxReadBytes);
	
		/* matrix (36)*/
		int k;
		for (k = 0; k < 9; k++)
			MP4_READ_32(mp4->file, val32, boxReadBytes);
	
		/* pre_defined (Preview time, Preview duration, Poster time, Selection time, Selection duration, Current time*/
		for (k = 0; k < 6; k++)
			MP4_READ_32(mp4->file, val32, boxReadBytes);
	
		/* next_track_ID */
		MP4_READ_32(mp4->file, val32, boxReadBytes);
		uint32_t next_track_ID = ntohl(val32);
		MP4_LOGD("# mvhd: next_track_ID=%" PRIu32, next_track_ID);
	
		/* skip the rest of the box */
		MP4_SKIP(mp4->file, boxReadBytes, maxBytes);
	
		return boxReadBytes;
	}




##### 2, trak 字段分析



![这里写图片描述](http://img.blog.csdn.net/20180125172840026?watermark/2/text/aHR0cDovL2Jsb2cuY3Nkbi5uZXQvUG9pc3g=/font/5a6L5L2T/fontsize/400/fill/I0JBQkFCMA==/dissolve/70/gravity/SouthEast)



###### 2.1, trak中"tkhd"字段分析


**"tkhd"结构如下表**

|字段(tkhd)|字节数|意义|
|-|-|-|
|box size|4|box 大小|
|box type|4|box 类型|
|version|1|box版本, 0或者1, 一般为0. (以下字节数均按version=0)|
|flags|3|按位或处置结果值, 预定义如下: <a color = red>0x000001</a> track_enabled, 否则该track 不被播放; <a color = red>0x000002</a> track_in_movie, 表示该track在播放中被引用; <a color = red>0x000004</a> track_in_preview, 表示该track在预览时被引用. 一般该值为7, 如果一个馒头所有track均未设置track_in_movie和track_in_preview, 将被理解为所有track均设置了这两项; 对于hint track, 该值为0|
|creation|4|创建时间(相对于UTC时间1904-01-01 0点的秒数|
|modification time|4|修改时间|
|track id|4|id 号, 不能重复且不能为0|
|reserved|4|保留位|
|duration|4|track的时间长度|
|reserved|8|保留位|
|layer|2|视频层, 默认为0, 值小的在上层|
|alternate group|2|track 分组信息, 默认为0表示该track未与其他track有群组关系|
|volume|2|[8, 8]格式, 如果为音频track, 1.0(0x0100)表示最大音量:否则为0|
|reserved|2|保留位|
|matrix|36|视频变换矩阵|
|width|4|宽|
|height|4|高, 均为[16, 16] 格式值, 与sample描述中的实际画面大小比值, 用于播放时的展示宽高|
||||


![这里写图片描述](http://img.blog.csdn.net/20180125173235818?watermark/2/text/aHR0cDovL2Jsb2cuY3Nkbi5uZXQvUG9pc3g=/font/5a6L5L2T/fontsize/400/fill/I0JBQkFCMA==/dissolve/70/gravity/SouthEast)




	static off_t mp4_box_tkhd_read(
		struct mp4_file *mp4,
		struct mp4_box *box,
		off_t maxBytes,
		struct mp4_track *track)
	{
		off_t boxReadBytes = 0;
		uint32_t val32;
	
		MP4_LOG_ERR_AND_RETURN_ERR_IF_FAILED((track != NULL), -EINVAL,
			"invalid track");
	
		MP4_LOG_ERR_AND_RETURN_ERR_IF_FAILED((maxBytes >= 21 * 4), -EINVAL,
			"invalid size: %" PRIi64 " expected %d min",
			(int64_t)maxBytes, 21 * 4);
	
		/* version & flags */
		MP4_READ_32(mp4->file, val32, boxReadBytes);
		uint32_t flags = ntohl(val32);
		uint8_t version = (flags >> 24) & 0xFF;
		flags &= ((1 << 24) - 1);
		MP4_LOGD("# tkhd: version=%d", version);
		MP4_LOGD("# tkhd: flags=%" PRIu32, flags);
	
		if (version == 1) {
			MP4_LOG_ERR_AND_RETURN_ERR_IF_FAILED((maxBytes >= 24 * 4),
				-EINVAL, "invalid size: %" PRIi64 " expected %d min",
				(int64_t)maxBytes, 24 * 4);
	
			/* creation_time */
			MP4_READ_32(mp4->file, val32, boxReadBytes);
			uint64_t creationTime = (uint64_t)ntohl(val32) << 32;
			MP4_READ_32(mp4->file, val32, boxReadBytes);
			creationTime |= (uint64_t)ntohl(val32) & 0xFFFFFFFFULL;
			MP4_LOGD("# tkhd: creation_time=%" PRIu64,
				creationTime);
	
			/* modification_time */
			MP4_READ_32(mp4->file, val32, boxReadBytes);
			uint64_t modificationTime = (uint64_t)ntohl(val32) << 32;
			MP4_READ_32(mp4->file, val32, boxReadBytes);
			modificationTime |= (uint64_t)ntohl(val32) & 0xFFFFFFFFULL;
			MP4_LOGD("# tkhd: modification_time=%" PRIu64,
				modificationTime);
	
			/* track_ID */
			MP4_READ_32(mp4->file, val32, boxReadBytes);
			track->id = ntohl(val32);
			MP4_LOGD("# tkhd: track_ID=%" PRIu32, track->id);
	
			/* reserved */
			MP4_READ_32(mp4->file, val32, boxReadBytes);
	
			/* duration */
			MP4_READ_32(mp4->file, val32, boxReadBytes);
			uint64_t duration = (uint64_t)ntohl(val32) << 32;
			MP4_READ_32(mp4->file, val32, boxReadBytes);
			duration |= (uint64_t)ntohl(val32) & 0xFFFFFFFFULL;
			unsigned int hrs = (unsigned int)(
				(duration + mp4->timescale / 2) /
				mp4->timescale / 60 / 60);
			unsigned int min = (unsigned int)(
				(duration + mp4->timescale / 2) /
				mp4->timescale / 60 - hrs * 60);
			unsigned int sec = (unsigned int)(
				(duration + mp4->timescale / 2) /
				mp4->timescale - hrs * 60 * 60 - min * 60);
			MP4_LOGD("# tkhd: duration=%" PRIu64 " (%02d:%02d:%02d)",
				duration, hrs, min, sec);
		} else {
			/* creation_time */
			MP4_READ_32(mp4->file, val32, boxReadBytes);
			uint32_t creationTime = ntohl(val32);
			MP4_LOGD("# tkhd: creation_time=%" PRIu32,
				creationTime);
	
			/* modification_time */
			MP4_READ_32(mp4->file, val32, boxReadBytes);
			uint32_t modificationTime = ntohl(val32);
			MP4_LOGD("# tkhd: modification_time=%" PRIu32,
				modificationTime);
	
			/* track_ID */
			MP4_READ_32(mp4->file, val32, boxReadBytes);
			track->id = ntohl(val32);
			MP4_LOGD("# tkhd: track_ID=%" PRIu32, track->id);
	
			/* reserved */
			MP4_READ_32(mp4->file, val32, boxReadBytes);
	
			/* duration */
			MP4_READ_32(mp4->file, val32, boxReadBytes);
			uint32_t duration = ntohl(val32);
			unsigned int hrs = (unsigned int)(
				(duration + mp4->timescale / 2) /
				mp4->timescale / 60 / 60);
			unsigned int min = (unsigned int)(
				(duration + mp4->timescale / 2) /
				mp4->timescale / 60 - hrs * 60);
			unsigned int sec = (unsigned int)(
				(duration + mp4->timescale / 2) /
				mp4->timescale - hrs * 60 * 60 - min * 60);
			MP4_LOGD("# tkhd: duration=%" PRIu32 " (%02d:%02d:%02d)",
				duration, hrs, min, sec);
		}
	
		/* reserved */
		MP4_READ_32(mp4->file, val32, boxReadBytes);
		MP4_READ_32(mp4->file, val32, boxReadBytes);
	
		/* layer & alternate_group */
		MP4_READ_32(mp4->file, val32, boxReadBytes);
		int16_t layer = (int16_t)(ntohl(val32) >> 16);
		int16_t alternateGroup = (int16_t)(ntohl(val32) & 0xFFFF);
		MP4_LOGD("# tkhd: layer=%i", layer);
		MP4_LOGD("# tkhd: alternate_group=%i", alternateGroup);
	
		/* volume & reserved */
		MP4_READ_32(mp4->file, val32, boxReadBytes);
		float volume = (float)((ntohl(val32) >> 16) & 0xFFFF) / 256.;
		MP4_LOGD("# tkhd: volume=%.2f", volume);
	
		/* matrix */
		int k;
		for (k = 0; k < 9; k++)
			MP4_READ_32(mp4->file, val32, boxReadBytes);
	
		/* width */
		MP4_READ_32(mp4->file, val32, boxReadBytes);
		float width = (float)ntohl(val32) / 65536.;
		MP4_LOGD("# tkhd: width=%.2f", width);
	
		/* height */
		MP4_READ_32(mp4->file, val32, boxReadBytes);
		float height = (float)ntohl(val32) / 65536.;
		MP4_LOGD("# tkhd: height=%.2f", height);
	
		/* skip the rest of the box */
		MP4_SKIP(mp4->file, boxReadBytes, maxBytes);
	
		return boxReadBytes;
	}


###### 2.2 trak 中'edts'字段分析

|box size (4)|box type(4)|
|-|-|
|box 的大小|box 的'edts'类型|

![这里写图片描述](http://img.blog.csdn.net/20180127132031539?watermark/2/text/aHR0cDovL2Jsb2cuY3Nkbi5uZXQvUG9pc3g=/font/5a6L5L2T/fontsize/400/fill/I0JBQkFCMA==/dissolve/70/gravity/SouthEast)




![这里写图片描述](http://img.blog.csdn.net/20180127131741869?watermark/2/text/aHR0cDovL2Jsb2cuY3Nkbi5uZXQvUG9pc3g=/font/5a6L5L2T/fontsize/400/fill/I0JBQkFCMA==/dissolve/70/gravity/SouthEast)


####### 2.2.1 trak中'edts'中的'elts'字段的分析

|box size(4)|box type(4)|box version(1)|box flags(3)|number of entries(4)|Edit list table (array )|
|-|-|-|-|
|||||A 32-bit integer that specifies the number of entries in the edit list atom that follows.(有几个数组)|An array of 32-bit values, grouped into entries containing 3 values(有时间信息, 视频速度信息, 视频) each|

![这里写图片描述](http://img.blog.csdn.net/20180127133919335?watermark/2/text/aHR0cDovL2Jsb2cuY3Nkbi5uZXQvUG9pc3g=/font/5a6L5L2T/fontsize/400/fill/I0JBQkFCMA==/dissolve/70/gravity/SouthEast)


![这里写图片描述](http://img.blog.csdn.net/20180127134244256?watermark/2/text/aHR0cDovL2Jsb2cuY3Nkbi5uZXQvUG9pc3g=/font/5a6L5L2T/fontsize/400/fill/I0JBQkFCMA==/dissolve/70/gravity/SouthEast)


00 03 1D 08, 00 00 00 00, 00 01 00 00

1. 00 03 1D 08:视频的长度信息->> 204040(duration)  视频长度= duration / TimeScale
2. 00 00 00 00: 视频的信息开始时间
3. 00 01 00 00: 视频的播放速度 -->65536 要除以 65536 = 1



###### 2.3, 'mdia' 字段分析

|box size(4)|box type(4)|media Header|Extended language tag atom|Handler reference atom|Media information atom|User data atom|
|-|-|-|-|-|-|
|||header 说明||


![这里写图片描述](http://img.blog.csdn.net/20180127140954280?watermark/2/text/aHR0cDovL2Jsb2cuY3Nkbi5uZXQvUG9pc3g=/font/5a6L5L2T/fontsize/400/fill/I0JBQkFCMA==/dissolve/70/gravity/SouthEast)

####### 2.3.1 mdia字段中的'mdhd'分析

|box size(4)|box type(4)|box version(1)|box flags(3)|cration time(4)|modification time(4)|time scale(4)|duration(4)|language(2)|Quality(2)|
|-|-|-|-|-|-|-|-|-|-|

![这里写图片描述](http://img.blog.csdn.net/20180127142937536?watermark/2/text/aHR0cDovL2Jsb2cuY3Nkbi5uZXQvUG9pc3g=/font/5a6L5L2T/fontsize/400/fill/I0JBQkFCMA==/dissolve/70/gravity/SouthEast)


视频的长度 = timescale / duration;

####### 2.3.2 mdia字段中的'hdlr'分析

![这里写图片描述](http://img.blog.csdn.net/20180127145201708?watermark/2/text/aHR0cDovL2Jsb2cuY3Nkbi5uZXQvUG9pc3g=/font/5a6L5L2T/fontsize/400/fill/I0JBQkFCMA==/dissolve/70/gravity/SouthEast)

|box size(4)|box type(4)|version(1)|Flags(3)|Component type(4)|Coponent subtype(4)|Component manufacturer(4)|Component flags(4)|Component flags mask(4)| Component name|
|-|-|-|-|-|-|-|-|-|-|
|||||A four-character code that identifies the type of the handler. Only two values are valid for this field:'mhlr' for media handlers and 'dhlr' for data handlers.|A four-character code that identifies the type of the media handler or data handler. For media handlers,this field defines the type of data—for example, 'vide' for video data, 'soun' for sound data or‘meta’ for metadata. For data handlers, this field defines the data reference type—for example, a component subtypevalue of 'alis' identifies a file alias.|Reserved. Set to 0.|Reserved. Set to 0.|Reserved. Set to 0.|A (counted) string that specifies the name of the omponent—that is, the media handler used whenthis media was created. This field may contain a zero-length (empty) string.(媒体名称)|





![这里写图片描述](http://img.blog.csdn.net/20180127151412295?watermark/2/text/aHR0cDovL2Jsb2cuY3Nkbi5uZXQvUG9pc3g=/font/5a6L5L2T/fontsize/400/fill/I0JBQkFCMA==/dissolve/70/gravity/SouthEast)




####### 2.3.3  mdia字段中的'minf'分析('vmhd'和'smhd')

![这里写图片描述](http://img.blog.csdn.net/20180127152409612?watermark/2/text/aHR0cDovL2Jsb2cuY3Nkbi5uZXQvUG9pc3g=/font/5a6L5L2T/fontsize/400/fill/I0JBQkFCMA==/dissolve/70/gravity/SouthEast)

|box size(4)|box type(4)|Video media information atom|Handler reference atom|Data information atom|Sample table atom|
|-|-|-|-|-|

######## 2.3.3.1 minf中的"vmhd'分析(Video Media Information Header Atoms)

![这里写图片描述](http://img.blog.csdn.net/20180127153315908?watermark/2/text/aHR0cDovL2Jsb2cuY3Nkbi5uZXQvUG9pc3g=/font/5a6L5L2T/fontsize/400/fill/I0JBQkFCMA==/dissolve/70/gravity/SouthEast)

|box size(4)|box type(4)|version(1)|flags(3)|Graphics mode(2)|Opcolor(6)|
|-|-|-|-|-|-|
|||||Copy the source image over the destination.|Three 16-bit values that specify the red, green, and blue colors for the transfer mode operationindicated in the graphics mode field(三个16位值，指定在图形模式字段中指示的传输模式操作的红色、绿色和蓝色。)|


![这里写图片描述](http://img.blog.csdn.net/20180127154006611?watermark/2/text/aHR0cDovL2Jsb2cuY3Nkbi5uZXQvUG9pc3g=/font/5a6L5L2T/fontsize/400/fill/I0JBQkFCMA==/dissolve/70/gravity/SouthEast)


######## 2.3.3.1 minf中的 'dinf'的字段分析(Handler reference atom)

![这里写图片描述](http://img.blog.csdn.net/20180127155526828?watermark/2/text/aHR0cDovL2Jsb2cuY3Nkbi5uZXQvUG9pc3g=/font/5a6L5L2T/fontsize/400/fill/I0JBQkFCMA==/dissolve/70/gravity/SouthEast)

|box size(4)|box type(4)|Data reference atom(dref )|
|-|-|-|

![这里写图片描述](http://img.blog.csdn.net/20180127160403586?watermark/2/text/aHR0cDovL2Jsb2cuY3Nkbi5uZXQvUG9pc3g=/font/5a6L5L2T/fontsize/400/fill/I0JBQkFCMA==/dissolve/70/gravity/SouthEast)


######### 2.3.3.1.1 minf中的 'dinf'中的'dref'的字段分析

|box size(4)|box type(4)|version(1)|flags(3)|number ofentries(4)|Data reference(array)||
|-|-|-|-|-|-|-|

![这里写图片描述](http://img.blog.csdn.net/20180127161219079?watermark/2/text/aHR0cDovL2Jsb2cuY3Nkbi5uZXQvUG9pc3g=/font/5a6L5L2T/fontsize/400/fill/I0JBQkFCMA==/dissolve/70/gravity/SouthEast)


########## 2.3.3.1.1 minf中的 'dinf'中的'dref'的 'url'字段分析

|box size(4)|box type(4)|version(1)|flags(3)|Data|


![这里写图片描述](http://img.blog.csdn.net/20180127161648605?watermark/2/text/aHR0cDovL2Jsb2cuY3Nkbi5uZXQvUG9pc3g=/font/5a6L5L2T/fontsize/400/fill/I0JBQkFCMA==/dissolve/70/gravity/SouthEast)


**url中的data数据的说明**

![这里写图片描述](http://img.blog.csdn.net/20180127161907546?watermark/2/text/aHR0cDovL2Jsb2cuY3Nkbi5uZXQvUG9pc3g=/font/5a6L5L2T/fontsize/400/fill/I0JBQkFCMA==/dissolve/70/gravity/SouthEast)



####### 2.3.3  stbl字段分析

![这里写图片描述](http://img.blog.csdn.net/20180127162746319?watermark/2/text/aHR0cDovL2Jsb2cuY3Nkbi5uZXQvUG9pc3g=/font/5a6L5L2T/fontsize/400/fill/I0JBQkFCMA==/dissolve/70/gravity/SouthEast)

|box size(4)|box type(4)|Sample description(stsd)|Time-to-Sample(stts)|Composition offset(ctts)|Composition Shift Least Greates (cslg)|Sync sample (stss)|partial sysn sample(stps)|Sample-to-chunk(stsc)|Sample size(stsz)|Chunk offset(stco)|Sample Dependency Flags(sdtp)|Shadow sync(stsh)|
|-|-|-|-|-|-|


########  2.3.3.1 stbl 中的stsd 的段分析

![这里写图片描述](http://img.blog.csdn.net/20180126154157023?watermark/2/text/aHR0cDovL2Jsb2cuY3Nkbi5uZXQvUG9pc3g=/font/5a6L5L2T/fontsize/400/fill/I0JBQkFCMA==/dissolve/70/gravity/SouthEast)


########  2.3.3.1.1 **stsd中avc1段分析**

![这里写图片描述](http://img.blog.csdn.net/20180126154449838?watermark/2/text/aHR0cDovL2Jsb2cuY3Nkbi5uZXQvUG9pc3g=/font/5a6L5L2T/fontsize/400/fill/I0JBQkFCMA==/dissolve/70/gravity/SouthEast)

|字段|字节数|意义|
|-|-|-|
|size|4|size|
|Data format|4|封装格式|
|Reserved|6|Six bytes that must be set to 0.|
|Data reference index|2|有说明数据的参数个数|
||||


**avc1中的编码视频的宽度高度, 压缩编码32分析**

![这里写图片描述](http://img.blog.csdn.net/20180126163527462?watermark/2/text/aHR0cDovL2Jsb2cuY3Nkbi5uZXQvUG9pc3g=/font/5a6L5L2T/fontsize/400/fill/I0JBQkFCMA==/dissolve/70/gravity/SouthEast)

|size(4)|type(4)|version(2)|Revision level(2)|Vendor(4)|Temporal quality(4)|Spatial quality(4)|width(2)|Height(2)|Horizontal resolution(4)|Vertical resolution(4)|Data size(4)|Frame count(2)|Compressor name(4)|Depth(2)|Color table ID(2)|
|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|

|字段|字节数|意义|
|-|-|-|
|box size|4|size|
|box type|4|type|
|version|2|box 版本, 0或1, 一般为0, (以下字节数均按version=0)|
|Revision level|2|must be set to 0.|
|Vendor|4||
|Temporal quality|4|时间的压缩|
|Spatial quality|4|视频的质量|
|Width|2||
|Height|2||
|Horizontal resolution|4|垂直分辨率|
|Vertical resolution|4|水平分辨率|
|Data size|4|A 32-bit integer that must be set to 0|
|Frame count|2|A 16-bit integer that indicates how many frames of compressed data are stored in each sample. Usually set to 1.|
|Compressor name|4|A 32-byte Pascal string containing the name of the compressor that created the image, such as "jpeg"|
|Depth|2|表示压缩图像的像素深度的16位整数。1, 2, 4，8, 16, 24的值，32表示彩色图像的深度。只有在图像包含时才使用值32。阿尔法通道。灰度值分别为34, 36、40和表示2、4和8位灰度值。图像.|
|Color table ID|2|标识要使用的颜色表的16位整数。如果这个字段被设置为- 1，默认颜色表应用于指定深度。对于每像素16位以下的深度，这表示一个标准。指定深度的Macintosh颜色表。深度为16, 24，32没有颜色表。如果颜色表ID设置为0，则颜色表包含在示例描述本身中。颜色|
||||


这个地方特殊说明,这个地方需要32个8bit位置,0x00 *32第一个8bit来表明字符长度,后面31个8bit来表明压缩的内容,



	aligned(8) abstract class SampleEntry (unsigned int(32) format) extends Box(format){
	    const unsigned int(8)[6] reserved = 0;    ////首先6个字节的保留位  值都是0
	    unsigned int(16) data_reference_index;  ///一个2个字节来描述的 数据索引
	}
	
	///如果是一个空的entry,则追加一个字节的空数据
	class HintSampleEntry() extends SampleEntry (protocol) { 
	    unsigned int(8) data [];
	}
	// Visual Sequences    视频entry
	
	class VisualSampleEntry(codingname) extends SampleEntry (codingname){ 
	    unsigned int(16) pre_defined = 0;     //2个字节的保留位
	    const unsigned int(16) reserved = 0;    //2个字节的保留位
	    unsigned int(32)[3] pre_defined = 0;    //3*4个字节的保留位
	    unsigned int(16) width;             //2个字节的宽度
	    unsigned int(16) height;                //2个字节的高度
	    template unsigned int(32) horizresolution = 0x00480000; // 72 dpi    //纵向dpi,4字节
	    template unsigned int(32) vertresolution = 0x00480000; // 72 dpi    //横向dpi 4字节
	    const unsigned int(32) reserved = 0;                                //4字节保留位
	    template unsigned int(16) frame_count = 1;                      //2字节的frame_count
	    string[32] compressorname;                                      //32字节的compressorname
	    //这个地方特殊说明,这个地方需要32个8bit位置,0x00 *32
	    第一个8bit来表明字符长度,后面31个8bit来表明压缩的内容,
	    例子:
	    0x04,                       //  strlen compressorname: 32 bytes         String[32]
	                                                    //32个8 bit    第一个8bit表示长度,剩下31个8bit表示内容
	        0x67, 0x31, 0x31, 0x31,  // compressorname: 32 bytes    翻译过来是g111
	        0x00, 0x00, 0x00, 0x00,//
	        0x00, 0x00, 0x00, 0x00,//
	        0x00, 0x00, 0x00, 0x00,
	        0x00, 0x00, 0x00, 0x00,
	        0x00, 0x00, 0x00, 0x00,
	        0x00, 0x00, 0x00, 0x00,
	        0x00, 0x00, 0x00,
	
	
	    template unsigned int(16) depth = 0x0018;                       //2字节的色彩深度
	    int(16) pre_defined = -1;                                           //2字节的pre_defined
	}
	   // Audio Sequences   音频entry
	
	class AudioSampleEntry(codingname) extends SampleEntry (codingname){ 
	    const unsigned int(32)[2] reserved = 0;                             //2*4字节保留位
	    template unsigned int(16) channelcount = 2;                         //2字节的channelcount
	    template unsigned int(16) samplesize = 16;                          //2字节的 samplesize
	    unsigned int(16) pre_defined = 0;                                       //2字节的pre_defined
	    const unsigned int(16) reserved = 0 ;                                   //2字节保留位
	    template unsigned int(32) samplerate = {timescale of media}<<16;    //4字节声音赫兹
	}





**avc1中avcC字段的分析(H.264)**

|size(4)|Type(4)|AVC Decoder Configuration Record|
|-|-|-|

![这里写图片描述](http://img.blog.csdn.net/20180126192746337?watermark/2/text/aHR0cDovL2Jsb2cuY3Nkbi5uZXQvUG9pc3g=/font/5a6L5L2T/fontsize/400/fill/I0JBQkFCMA==/dissolve/70/gravity/SouthEast)

**数据有H.264分析header**

00 00 00 31, 61 76 63 43, 01 42 C0 15, FF E1 00 19

67 42 C0 15, D9 01 B1 FE, 4F 01 10 00, 00 03 00 10

00 00 03 03, 20 F1 62 E4, 80 01 00 05, 68 CB 82 CB 20

**mp4中分析h.264**

　AVC sequence header就是AVCDecoderConfigurationRecord结构，该结构在标准文档“ISO-14496-15 AVC file format”中有详细说明。

|长度|字段|说明|
|-|-|-|
|8 bit|configuration Version|版本号, 1|
|8 bit|AVCProFileIndication|sps[1]|
|8 bit|profile_compatibility|sps[2]|
|8 bit|AVC_LevelIndication|sps[3]|
|6 bit|reserved||
|2 bit|lengthSizeMinusOne|NALUnitLength的长度-1|
|3 bit|reserved|111|
|5 bit|numOfSequencePaarameterSets|sps个数, 一般为1|
||sequenceParameterSetNALUnits|sps_size + size)的数组|
|8 bit|numOfPictureParameterSets|pps个数, 一般为1|
||pictureParameterSetNALUnit|(pps_size + pps)的数组|

根据 AVCDecoderConfigurationRecord 结构的定义：

1. 00 00 00 31: box size :  'avcC' 大小 49个字节
2. 61 76 63 43: box type : 'avcC' 类型   
3. 01 : box 版本: 
4. 42 C0 15 : box ProFile sps[1]
5. FF :  非常重要，是 H.264 视频中 NALU 的长度，计算方法是 1 + (lengthSizeMinusOne & 3)，实际计算结果一直是4
6. E1 :   SPS 的个数，计算方法是 numOfSequenceParameterSets & 0x1F，实际计算结果一直为1
7. 00 19 : SPS的长度 ->25个字节
8. 67 42 C0 15, D9 01 B1 FE, 4F 01 10 00, 00 03 00 10, 00 00 03 03, 20 F1 62 E4, 80 --> SPS
9. 01 : PPS的个数 
10. 00 05 : PPS的长度
11. 68 CB 82 CB 20 : PPS数据


发送NALU包
	
	void add_264_sequence_header(unsigned char *pps, unsigned char *sps,
	                             int pps_len, int sps_len) {
	    int body_size = 13 + sps_len + 3 + pps_len;
	    RTMPPacket *packet = (RTMPPacket *) malloc(sizeof(RTMPPacket));
	    RTMPPacket_Alloc(packet, body_size);
	    RTMPPacket_Reset(packet);
	    char *body = packet->m_body;
	    int i = 0;
	    body[i++] = 0x17;
	    body[i++] = 0x00;
	    //composition time 0x000000
	    body[i++] = 0x00;
	    body[i++] = 0x00;
	    body[i++] = 0x00;
	
	    /*AVCDecoderConfigurationRecord*/
	    body[i++] = 0x01;
	    body[i++] = sps[1];
	    body[i++] = sps[2];
	    body[i++] = sps[3];
	    body[i++] = 0xFF;
	
	    /*sps*/
	    body[i++] = 0xE1;
	    body[i++] = (sps_len >> 8) & 0xff;
	    body[i++] = sps_len & 0xff;
	    memcpy(&body[i], sps, sps_len);
	    i += sps_len;
	
	    /*pps*/
	    body[i++] = 0x01;
	    body[i++] = (pps_len >> 8) & 0xff;
	    body[i++] = (pps_len) & 0xff;
	    memcpy(&body[i], pps, pps_len);
	    i += pps_len;
	
	    packet->m_packetType = RTMP_PACKET_TYPE_VIDEO;
	    packet->m_nBodySize = body_size;
	    packet->m_nChannel = 0x04;
	    packet->m_nTimeStamp = 0;
	    packet->m_hasAbsTimestamp = 0;
	    packet->m_headerType = RTMP_PACKET_SIZE_MEDIUM;
	    add_rtmp_packet(packet);
	}
	
	void add_264_body(unsigned char *buf, int len) {
	    /*去掉帧界定符 *00 00 00 01*/
	    if (buf[2] == 0x00) { //
	        buf += 4;
	        len -= 4;
	    } else if (buf[2] == 0x01) { //00 00 01
	        buf += 3;
	        len -= 3;
	    }
	    int body_size = len + 9;
	    RTMPPacket *packet = (RTMPPacket *) malloc(sizeof(RTMPPacket));
	    RTMPPacket_Alloc(packet, len + 9);
	    char *body = packet->m_body;
	    int type = buf[0] & 0x1f;
	    /*key frame*/
	    body[0] = 0x27;
	    if (type == NAL_SLICE_IDR) {
	        body[0] = 0x17;
	    }
	    body[1] = 0x01; /*nal unit*/
	    body[2] = 0x00;
	    body[3] = 0x00;
	    body[4] = 0x00;
	
	    body[5] = (len >> 24) & 0xff;
	    body[6] = (len >> 16) & 0xff;
	    body[7] = (len >> 8) & 0xff;
	    body[8] = (len) & 0xff;
	
	    /*copy data*/
	    memcpy(&body[9], buf, len);
	
	    packet->m_hasAbsTimestamp = 0;
	    packet->m_nBodySize = body_size;
	    packet->m_packetType = RTMP_PACKET_TYPE_VIDEO;
	    packet->m_nChannel = 0x04;
	    packet->m_headerType = RTMP_PACKET_SIZE_LARGE;
	//	packet->m_nTimeStamp = -1;
	    packet->m_nTimeStamp = RTMP_GetTime() - start_time;
	    add_rtmp_packet(packet);
	}
	
	void add_aac_body(unsigned char *buf, int len) {
	    //outputformat = 1 ADTS头 7个，写入文件
	    //	outputformat = 0  直接为原始数据 不需要去掉头7个
	//		buf += 7;
	//		len -= 7;
	    int body_size = len + 2;
	    RTMPPacket *packet = (RTMPPacket *) malloc(sizeof(RTMPPacket));
	    RTMPPacket_Alloc(packet, body_size);
	    char *body = packet->m_body;
	    /*AF 01 + AAC RAW data*/
	    body[0] = 0xAF;
	    body[1] = 0x01;
	    memcpy(&body[2], buf, len);
	    packet->m_packetType = RTMP_PACKET_TYPE_AUDIO;
	    packet->m_nBodySize = body_size;
	    packet->m_nChannel = 0x04;
	    packet->m_hasAbsTimestamp = 0;
	    packet->m_headerType = RTMP_PACKET_SIZE_MEDIUM;
	//	packet->m_nTimeStamp = -1;
	    packet->m_nTimeStamp = RTMP_GetTime() - start_time;
	    add_rtmp_packet(packet);
	}




**音频数据分析**

　　AAC sequence header存放的是AudioSpecificConfig结构，该结构则在“ISO-14496-3 Audio”中描述。AudioSpecificConfig结构的描述非常复杂，这里我做一下简化，事先设定要将要编码的音频格式，其中，选择"AAC-LC"为音频编码，音频采样率为44100，于是AudioSpecificConfig简化为下表：


|长度|字段|说明|
|-|-|-|
|5 bit|audio ObjectType|编码结构类型, AAC-LC为2|
|4 bit|samplingFrequencyIndex|音频采样率索引值, 44100对应值4|
|4 bit|channelConfiguration|音频输出声道, 2|
||GASpecific|该结构包含以下三项|
|1 bit|frameLengthFlag|标志位, 用于表明IMDCT窗口长度, 0|
|1 bit|dependsOnCoreCoder|标志位, 表明是否依赖于corecoder, 0|
|1 bit|extensionFlag| 选择了AAC-LC, 这里必须为0|


 ![这里写图片描述](http://img.blog.csdn.net/20180126204854139?watermark/2/text/aHR0cDovL2Jsb2cuY3Nkbi5uZXQvUG9pc3g=/font/5a6L5L2T/fontsize/400/fill/I0JBQkFCMA==/dissolve/70/gravity/SouthEast)



########  2.3.3.2 stbl 中的'stts'的段分析

![这里写图片描述](http://img.blog.csdn.net/20180127165511209?watermark/2/text/aHR0cDovL2Jsb2cuY3Nkbi5uZXQvUG9pc3g=/font/5a6L5L2T/fontsize/400/fill/I0JBQkFCMA==/dissolve/70/gravity/SouthEast)

|box size(4)|box type(4)|Version(1)|Flags(3)|Number of entries(4)|Time-to-sample table|
|-|-|-|-|-|
||||||A table that defines the duration of each sample in the media. Each table entry contains a count field and a duration field. The structure of the time-to-sample table is shown in (定义媒体中每个样本持续时间的表。每个表条目都包含一个计数字段。和持续时间字段。时间的结构示例表在图2-34所示（86页）。)|

![这里写图片描述](http://img.blog.csdn.net/20180127170410511?watermark/2/text/aHR0cDovL2Jsb2cuY3Nkbi5uZXQvUG9pc3g=/font/5a6L5L2T/fontsize/400/fill/I0JBQkFCMA==/dissolve/70/gravity/SouthEast)



表中的条目根据媒体中的顺序和它们的持续时间来描述样本。如果连续

示例具有相同的持续时间，可以使用一个表条目定义多个示例。在这些

实例中，计数字段指示具有相同持续时间的连续样本的数目。例如,

如果视频媒体具有恒定的帧速率，则该表将有一个条目，计数将相等。

样本数。

图2-35（87页）提出了一个例子，一个样品的时间表，基于分块的媒体

图2-30所示的数据（81页）。该数据流包含九个对应的样本。

这里显示的表条目的计数和持续时间。即使样本4, 5和6是相同的

块，示例4的持续时间为3，示例5和6的持续时间为2。

########  2.3.3.3 stbl 中的'stss'的段分析(关键帧)


![这里写图片描述](http://img.blog.csdn.net/20180127171636386?watermark/2/text/aHR0cDovL2Jsb2cuY3Nkbi5uZXQvUG9pc3g=/font/5a6L5L2T/fontsize/400/fill/I0JBQkFCMA==/dissolve/70/gravity/SouthEast)

|box size(4)|box type(4)|Version(1)|Flags(3)|Number of entries(4)|Time-to-sample table|
|-|-|-|-|-|
|||||包含同步示例表中条目计数的32位整数.|A table of sample numbers; each sample number corresponds to a key frame. shows the layout of the sync sample table.(一个样本数表；每个样本数对应于一个关键帧。图2-40(第91页)显示了同步示例表的布局。)|


![这里写图片描述](http://img.blog.csdn.net/20180127172014263?watermark/2/text/aHR0cDovL2Jsb2cuY3Nkbi5uZXQvUG9pc3g=/font/5a6L5L2T/fontsize/400/fill/I0JBQkFCMA==/dissolve/70/gravity/SouthEast)

</font>
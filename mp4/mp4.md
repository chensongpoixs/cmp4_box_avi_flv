<font face="微软雅黑"  >

### 一, mp4格式分析

<font size = 3> <a color = red>MP4</a>(MPEG-4 Part 14)是一种常见的多媒体容器格式，它是在“ISO/IEC 14496-14”标准文件中定义的，属于MPEG-4的一部分，是“ISO/IEC 14496-12(MPEG-4 Part 12 ISO base media file format)”标准中所定义的媒体格式的一种实现，后者定义了一种通用的媒体文件结构标准。MP4是一种描述较为全面的容器格式，被认为可以在其中嵌入任何形式的数据，各种编码的视频、音频等都不在话下，不过我们常见的大部分的MP4文件存放的AVC(H.264)或MPEG-4(Part 2)编码的视频和AAC编码的音频。MP4格式的官方文件后缀名是“.mp4”，还有其他的以mp4为基础进行的扩展或者是缩水版本的格式，包括：M4V,  3GP, F4V等。</font>


mp4是由一个个“box”组成的，大box中存放小box，一级嵌套一级来存放媒体信息


#### 1,解析box

一般来说，解析媒体文件，最关心的部分是视频文件的宽高、时长、码率、编码格式、帧列表、关键帧列表，以及所对应的时戳和在文件中的位置，这些信息，在mp4中，是以特定的算法分开存放在stbl box下属的几个box中的，需要解析stbl下面所有的box，来还原媒体信息。下表是对于以上几个重要的box存放信息的说明：

![这里写图片描述](http://img.blog.csdn.net/20180122131854808?watermark/2/text/aHR0cDovL2Jsb2cuY3Nkbi5uZXQvUG9pc3g=/font/5a6L5L2T/fontsize/400/fill/I0JBQkFCMA==/dissolve/70/gravity/SouthEast)



|box类型|box类型|box类型|box类型|box类型|box类型|说明|
|-|-|-|-|-|-|-|
|ftyp||||||file type , 表明文件类型|
|moov||||||metadata container, 存放媒体消息的地方|
||mvhd|||||movie header, 文件的总体消息, 如时长, 创建时间等|
||trak|||||track or stream container, 存放视频/音频流的容器|
|||tkhd||||track header, track的总体消息, 如时长, 宽高等|
|||mdia||||track media information container, 不解释|
||||mdhd|||media header, 定义TimeScale, trak需要通过TimeScale换算成真实的时间|
||||hdlr|||header, 表明本trak类型, 指明是video/audio/还是hint|
||||minf|||media information container, 数据在子box中|
|||||stbl||sample table box,存放时间/偏移的映射关系表, 数据在子box中|
||||||stsd|sample descriptions|
||||||stts|(decodeing)time-to-sample, "时间戳-sample序号"的映射表|
||||||stsc|sample-to-chunk, sample和chunk的映射表, 这里的算法比较巧妙|
||||||stsz|sample size, 每个sample的大小|
||||||stz2|sample size, 另一个sample size的存储算法, 更节省空间|
||||||stss|sync sample table, 可随机访问的sample列表(关键帧列表)|
||||||stco|chunk offset, 每个chunk的偏移, sample的偏移可关键其他box推算出来|
||||||co64|64-bit chunk offset|
|mdat||||||media data container, 具体的媒体数据|
||||||||



**box**



MP4文件中的所有数据都装在box（QuickTime中为atom）中，也就是说MP4文件由若干个box组成，每个box有类型和长度，可以将box理解为一个数据对象块。box中可以包含另一个box，这种box称为container box。一个MP4文件首先会有且只有一个“ftyp”类型的box，作为MP4格式的标志并包含关于文件的一些信息；之后会有且只有一个“moov”类型的box（Movie Box），它是一种container box，子box包含了媒体的metadata信息；MP4文件的媒体数据包含在“mdat”类型的box（Midia Data Box）中，该类型的box也是container box，可以有多个，也可以没有（当媒体数据全部引用其他文件时），媒体数据的结构由metadata进行描述。




**下面是一些概念：**

 >  track  表示一些sample的集合，对于媒体数据来说，track表示一个视频或音频序列。

  > hint track  这个特殊的track并不包含媒体数据，而是包含了一些将其他数据track打包成流媒体的指示信息。

  > sample  对于非hint track来说，video sample即为一帧视频，或一组连续视频帧，audio sample即为一段连续的压缩音频，它们统称sample。对于hint track，sample定义一个或多个流媒体包的格式。

  > sample table  指明sampe时序和物理布局的表。

  > chunk 一个track的几个sample组成的单元。


![这里写图片描述](http://img.blog.csdn.net/20180122145941740?watermark/2/text/aHR0cDovL2Jsb2cuY3Nkbi5uZXQvUG9pc3g=/font/5a6L5L2T/fontsize/400/fill/I0JBQkFCMA==/dissolve/70/gravity/SouthEast)





1. 首先需要说明的是，box中的字节序为网络字节序，也就是大端字节序（Big-Endian），简单的说，就是一个32位的4字节整数存储方式为高位字节在内存的低端。Box由header和body组成，其中header统一指明box的大小和类型，body根据类型有不同的意义和格式。
2. 标准的box开头的4个字节（32位）为box size，该大小包括box header和box body整个box的大小，这样我们就可以在文件中定位各个box。如果size为1，则表示这个box的大小为large size，真正的size值要在largesize域上得到。（实际上只有“mdat”类型的box才有可能用到large size。）如果size为0，表示该box为文件的最后一个box，文件结尾即为该box结尾。（同样只存在于“mdat”类型的box中。）
3. size后面紧跟的32位为box type，一般是4个字符，如“ftyp”、“moov”等，这些box type都是已经预定义好的，分别表示固定的意义。如果是“uuid”，表示该box为用户扩展类型。如果box type是未定义的，应该将其忽略




(1、 mp4文件由许多Box和FullBox组成。

(2、 Box，每个Box由Header和Data组成。

(3、 FullBox，是Box的扩展，Box结构的基础上在Header中增加8bits version和24bits flags。

(4、 Header，包含了整个Box的长度size和类型type。当size==0时，代表这是文件中最后一个Box；当size==1时，意味着Box长度需要更多bits来描述，在后面会定义一个64bits的largesize描述Box的长度；当type是uuid时，代表Box中的数据是用户自定义扩展类型。

(5、 Data，是Box的实际数据，可以是纯数据也可以是更多的子Boxes。

(6、 当一个Box的Data中是一系列子Box时，这个Box又可成为Container Box。当一个Box里面不包含子Box时，这个Box称为leaf Box，而FullBox是mp4格式协议规定的某些特殊的Box。


##### ① FILE Type Box (ftyp) 分析

>该box只有一个, 并且只能被包含在文件层, 而且不能被其它box包含. 该box应该被放在文件的最开始, 指示该MP4文件应用的相关信息


>"ftyp" body 依次包含1个32位的major brand(4个字节), 1个32的minor version(整数)和1个以32(4个字节) 为单位元素的数组 compatible brands. 这些都是用来指示文件应用级别的信息.该 box的字节实例如下:

![这里写图片描述](http://img.blog.csdn.net/20180122145536304?watermark/2/text/aHR0cDovL2Jsb2cuY3Nkbi5uZXQvUG9pc3g=/font/5a6L5L2T/fontsize/400/fill/I0JBQkFCMA==/dissolve/70/gravity/SouthEast)

|length|boxtype|major_brand|minor_version|compatible_brands|
|-|-|-|-|-|
|box的长度(4)|box的标识(4)|“isom“的ASCII码(4)|ismo的版本号(4)|支持的协议(12或者16)|
1. length（4字节）：0x00000020：box的长度是32字节(有的是24字节说明支持3种协议)；
2. boxtype（4字节）：0x66747970：“ftyp”的ASCII码，box的标识；
3. major_brand（4字节）：0x69736f6d：“isom“的ASCII码；
4. minor_version（4字节）：0x00000200：ismo的版本号；
5. compatible_brands（16字节）：说明本文件遵从（或称兼容）ismo,iso2,avc1,mp41四种协议。(可变参数 文件支持的协议看length长度)

##### ② Movie Box(moov)

>    该box包含了文件媒体的metadata信息, "moov"是一个container box, 具有内容信息由子box诠释. 同FILE Type Box一样, 该box有且只有一个, 且只被包含在文件层. 一般情况下, "moov"会紧跟"ftyp"出现.

>    一般情况下(限于mp4文件结构), "moov"中会包含1个"mvhd"和若干个"trak". 其中"mvhd"为header box, 一般作为"moov"的第一子box出现(对于其他container box 来说, header box 都应作为首个子box出现). "trak" 包含了一个track的相关的信息, 是一个container box. 下图为部分"moov"的字节实例, 


###### ②.1moov中mvhd结构box分析

![这里写图片描述](http://img.blog.csdn.net/20180122171032825?watermark/2/text/aHR0cDovL2Jsb2cuY3Nkbi5uZXQvUG9pc3g=/font/5a6L5L2T/fontsize/400/fill/I0JBQkFCMA==/dissolve/70/gravity/SouthEast)

**"mvhd"结构体如下表**

|字段|字节数|意义|
|-|-|-|
|box size|4|box大小|
|box type|4|box类型|
|version|1|box版本, 0或者1, 一般为0(以下字节数均按照version=0)|
|flags|3||
|creation|4|创建时间(相对于UTC时间1904-01-01 0点秒数|
|modification|4|修改时间|
|time scale|4|文件媒体在1秒时间内的刻度值, 可以理解为1秒长度的时间单元数|
|duration|4|该tack的时间长度, 用duration和time scale值可以计算track时长, 比如audio track的time scale = 8000, duration = 560128, 时长为70.016, video track 的time scale = 600, duration = 42000, 时长为70|
|rate|4|推荐播放速率, 高16位和低16位分别为小数点整数播放和小数部分, 即[16, 16]  格式, 该值为1.0(0x00010000) 表示正常前向播放|
|volume|2|与rate类型, [8, 8]格式, 1.0 (0x0100) 表示最大音量|
|reserved|10|保留位|
|matrix|36|视频转换矩阵|
|pre-defined|24||
|next track id|4|下一个track使用的id号|
||||
||||
||||

###### ②.2 Track Box(trak)

>"trak"也是一个 container box, 其子box包含了该trak的媒体数据引用和描述(hint trak 除外). 一个mp4文件中的媒体可以包含多个trak, 且至少有一个trak, 这些track之间披此独立, 有自己的时间和空间信息. "trak" 必须包含一个"tkhd"和一个"mdia", 披外还有很多可选的box. 其中"tkhd"为track header box, "mdia" 为media box , 该box是一个包含以下track媒体数据信息 box的container box

![这里写图片描述](http://img.blog.csdn.net/20180122191740978?watermark/2/text/aHR0cDovL2Jsb2cuY3Nkbi5uZXQvUG9pc3g=/font/5a6L5L2T/fontsize/400/fill/I0JBQkFCMA==/dissolve/70/gravity/SouthEast)


####### ②2.1 Track Header Box(tkhd)
![这里写图片描述](http://img.blog.csdn.net/20180122200657941?watermark/2/text/aHR0cDovL2Jsb2cuY3Nkbi5uZXQvUG9pc3g=/font/5a6L5L2T/fontsize/400/fill/I0JBQkFCMA==/dissolve/70/gravity/SouthEast)

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


####### ②2.2 Media Box (mdia)

>"mdia"也是个 container box, 其子box 的结构和种类型还是比较复杂的. 

>先来看一个"mdia"的实例结构树图

![这里写图片描述](http://img.blog.csdn.net/20180123133558408?watermark/2/text/aHR0cDovL2Jsb2cuY3Nkbi5uZXQvUG9pc3g=/font/5a6L5L2T/fontsize/400/fill/I0JBQkFCMA==/dissolve/70/gravity/SouthEast)


>总体来说, "mdia"定义了track媒体类型以及sample 数据, 描述sample信息. 一般"mdia"包含一个"mdhd", 一个"hdlr"和一个"minf", 其中"mdhd"为media header box, "hdlr"为handler reference box, "minf"为media information box. 

|mdhd|hdlr|minf|
|-|-|-|
|media header, 定义TimeScale, trak需要通过TimeScale换算成真实的时间|header, 表明本trak类型, 指明是video/audio/还是hint|mediainformationcontainer, 数据在子box中|

![这里写图片描述](http://img.blog.csdn.net/20180123135114654?watermark/2/text/aHR0cDovL2Jsb2cuY3Nkbi5uZXQvUG9pc3g=/font/5a6L5L2T/fontsize/400/fill/I0JBQkFCMA==/dissolve/70/gravity/SouthEast)

下面依次看一下这几个box的结构.
######### ②2.2.1 Media Header Box(mdhd)
![这里写图片描述](http://img.blog.csdn.net/20180123140856140?watermark/2/text/aHR0cDovL2Jsb2cuY3Nkbi5uZXQvUG9pc3g=/font/5a6L5L2T/fontsize/400/fill/I0JBQkFCMA==/dissolve/70/gravity/SouthEast)

**"mdhd"结构如下表**

|字段|字节数|意义|
|-|-|-| 
|box size|4|box 大小|
|box type|4|box 类型|
|version|1|box 版本, 0或1, 一般为0, (以下字节数均按version=0)|
|flags|3||
|creation|4|创建时间(相对于UTC时间1904-01-01 0点的秒数|
|modification time|4|修改时间|
|time scale|4|同前表|
|duration|4|track 的时间长度|
|language|2|媒体语言码. 最高位为0, 后面15位为3个字符(见 ISO 639-2/T 标准中定义)|
|pre-defined|2||
||||


######### ②2.2.2 Handler Reference Box(hdlr)

>"hdlr"解释了媒体的播放过程的信息, 该box 也可以被包含在metabox(meta)中. 

![这里写图片描述](http://img.blog.csdn.net/20180123141417375?watermark/2/text/aHR0cDovL2Jsb2cuY3Nkbi5uZXQvUG9pc3g=/font/5a6L5L2T/fontsize/400/fill/I0JBQkFCMA==/dissolve/70/gravity/SouthEast)

**"hdlr" 结构如下表.**


|字段|字节数|意义|
|-|-|-|
|box size|4|box 大小|
|box type|4|box 类型|
|version|1|box版本, 0或1, 一般为0.(以下字节数均按version=0)|
|flags|3||
|pre-defined|4||
|handler type|4|在media box中, 该值为4个字符: "vide" - video track "soun" - audio track "hint" - hint track|
|reserved|12||
|name|不定|track type name, 以'\0'结尾的字符串|
||||

![这里写图片描述](http://img.blog.csdn.net/20180123142846891?watermark/2/text/aHR0cDovL2Jsb2cuY3Nkbi5uZXQvUG9pc3g=/font/5a6L5L2T/fontsize/400/fill/I0JBQkFCMA==/dissolve/70/gravity/SouthEast)



上图最后000A不是name字段中的

######### ②2.2.3 Media information Box (minf)

第一个字段(可能有下面的)

|vmhd|smhd|hmhd|nmhd|
|-|-|-|-|




|smhd(vmhd, hmhd, nmhd)|dinf|stbl|
|-|-|-|
||||

>"minf" 存储了解释track媒体数据的 handler-specific 信息. media handler 用这些信息将媒体时间映射到媒体数据并进行处理. "minf"中的信息格式和内容与媒体类型以及解释媒体数据的media handler 密切相关, 其他media handler 不知道如何解释这些信息."minf"是一个container box, 其实际内容由子box说明.

>一般情况下, "minf"包含一个header box, 一个"dinf"和一个"stbl", 其中, header box 根据 track type (即 media handler type) 分为"vmhd", "smhd", "hmhd"和"nmhd", "dinf" 为data information box, "stbl" 为sample table box. 




![这里写图片描述](http://img.blog.csdn.net/20180123151212445?watermark/2/text/aHR0cDovL2Jsb2cuY3Nkbi5uZXQvUG9pc3g=/font/5a6L5L2T/fontsize/400/fill/I0JBQkFCMA==/dissolve/70/gravity/SouthEast)


########## <font color = red size = 4>②2.2.3.1 Media Information Header Box (vmhd, smhd, hmhd, nmhd)</font>


**1, Video Media Header Box(vmhd)****视频分析**



|字段|字节数|意义|
|-|-|-|
|box size|4|box 大小|
|box type|4|box 类型|
|version|1|box 版本, 0或1, 一般为0, (以下字节数均按version=0) |
|flags|3||
|graphics|4|视频合成模式, 为0时拷贝原始图像, 否则与opcolor进行合成|
|opcolor|2 * 3|{red, green, blue}|
||||

**2, Sound Media Header Box(smhd) 音频分析**

|字段|字节数|意义|
|-|-|-|
|box size|4|box 大小|
|box type|4|box 类型|
|version|1|box 版本, 0或1, 一般为0, (以下字节数均按version=0)|
|flags|3||
|balance|2|立体声平衡, [8, 8] 格式值, 一般为0, -1.0 表示全部左声道, 1.0表示全部右声道|
|reserved|2||
||||

**3, Hint Media Header Box(hmhd)**

**4, Null Media Header Box(nmhd)**
非音视频媒体使用该box, 


########## ②2.2.3.2 Data Information Box(dinf)

>"dinf"解释如何定位媒体信息, 是一个container box. "dinf" 一般包含一个"dref", 即 data reference box; "dref"下会包含若干个"url"或"urn", 这些box组成一个表, 用来定位track数据. 简单的说, track可以被分成若干段, 每一段都可以根据"url"或"urn"指向的地址来换取数据, sample描述中会用这些片段的序号将这些片段组成一个完整的track. 一般情况下, 当数据被完全包含在文件中是, "url"或"urn"中的定位字符串是空的.

![这里写图片描述](http://img.blog.csdn.net/20180123161051741?watermark/2/text/aHR0cDovL2Jsb2cuY3Nkbi5uZXQvUG9pc3g=/font/5a6L5L2T/fontsize/400/fill/I0JBQkFCMA==/dissolve/70/gravity/SouthEast)

|字段|字节数|意义|
|-|-|-|
|box size|4|box 大小|
|box type|4|box 类型|
|version|1|box 版本, 0或1, 一般为0, (以下字节数均按version=0)|
|flags|3||
|entry count|4|"url"或"urn"表的元素个数|
|"url"或"urn"列表|不定||



<font color = red size = 3>**"url"或"urn" 都是box**, "url"的内容为字符串(location string), "urn"的内容为一对字符串(name string and location string). 当"url"或 "urn"的box flags 为1时, 字符串均为空</font>

![这里写图片描述](http://img.blog.csdn.net/20180123162436102?watermark/2/text/aHR0cDovL2Jsb2cuY3Nkbi5uZXQvUG9pc3g=/font/5a6L5L2T/fontsize/400/fill/I0JBQkFCMA==/dissolve/70/gravity/SouthEast)


########## ②2.2.3.3 Sample Table Box(stbl)

>"stbl" 几乎是普通的MP4文件中最复杂的一个box了, 首先需要回忆一个sample的概念.sample是媒体数据存储的单位, 存储在media的chunk中,chunk和sample的长度平均可互不相同, 

**如下图所示:**

![这里写图片描述](http://img.blog.csdn.net/20180123163441316?watermark/2/text/aHR0cDovL2Jsb2cuY3Nkbi5uZXQvUG9pc3g=/font/5a6L5L2T/fontsize/400/fill/I0JBQkFCMA==/dissolve/70/gravity/SouthEast)


**1, "stbl"分析**

|stsd|stts|stsz,stz2|stsc|stco,co64|ctts|stss|
|-|-|-|-|-|-|-|


>"stbl"包含了关于track中sample所有时间和位置的信息,以及sample的编解码等信息. 利用这个表, 可以解释sample 的时序, 类型, 大小以及在各自存储容器中的位置. "stbl"是一个container box, 其子box包含:sample description box(stsd), time to sample box(stts), sample size box(stsz或者stz2), sample 头chunk box(stsc), chunk offset box (stco 或 co64), composition time 头sample box(ctts), sync sample box(stss)等.


>"stsd"必不可少, 且至少包含一个条目, 该box包含了data reference box进行 sample数据检索的信息, 没有"stsd"就无法计算media sample的存储位置. "stsd"包含了编码的信息, 其存储的信息随媒体类型不同而不同.

**Sample Description Box(stsd)**

>box header 和version字段后会有一个entry count字段, 工具entry的个数, 每个entry会有type信息, 如"vide", "sund"等, 根据type 不同sample description会提供不同的信息, 例如对于video track, 会有"VisualSampleEntry"类型的信息, 对于 audio track会有"AudioSampleEntry"类型信息.

>视频的编码类型, 宽高, 长度, 音频的声道, 采样等信息都会出现在这个box中.


**Time To Sample Box(stts)**

>"stts"存储了 sample的duration, 描述了smaple 时序的映射方法, 我们通过它可以找到任何时间的sample. "stts"可以包含一个压缩的表来映射时间和sample序号, 用拳头的表来提供每个sample的长度和指针. 表中每个条目提供了在同一个时间偏移量里面连续的sample序号, 以及samples的偏移量. 递增这些偏移量, 就可以建立一个完整的ime to sample表.



**Sample Size Box (stsz)**


>"stsz"定义了每个sample大小, 包含了媒体中全部sample的数目和一张给出每个sample大小的表. 这个box想点来说体积是比较大的.

**Sample To Chunk Box(stsc)**

>用chunk组织smaple可以方便优化数据获取, 一个thunk包含一个或多个sample. "stsc"中用一个表描述了sample与chunk的映射关系, 查看这张表就可以找到包含指定sample的thunk,从而找到这个sample


**Sync Sample Box(stss)**

>"stss"确定media中的关键帧. 对于压缩媒体数据, 关键帧是一系列压缩序列的开始帧,其解压缩是不依赖以前的帧, 而后续帧的解压缩将依赖于这个关键帧. "stss"可以非常紧挤的标记媒体内的随机存取点, 它包含一个sample序号表, 表内的每一项严格按照sample的序号排列, 说明了媒体中哪个sample是关键帧. 如果此表不存在, 说明每一个sample都是一个关键帧, 是一个随机存取点.

**Chunk Offset Box(stco)**

>"stco"定义了每个thunk在媒体流中的位置. 位置有两种可能. 32位的和64位的, 后者对非常大的电影很有用. 在一个表中只会有一种可能, 这个位置是在整个文件中的, 而不是在任何box中的, 这样做就可以直接在文件中找到媒体数据, 而不用解释box. 需要注意的是一旦前面的box有了任何改变, 这张表都要重新建立, 因为位置信息已经改变了.
>
metadata Structure(medi)

###### ② 3.1 User Data Atoms (udta) 

|字段|字节数|意义|
|-|-|-|
|box size |4| size|
|box type|4|类型|
|User data list(ilst)|未知||
|box size|4||
|type|4|type|
|User data list|不定|数据的列表|


##### ③ Free Space Box (free  或 skip)

>"free"中的内容是无关紧要的, 可以被忽略. 该box表删除后, 不会对播放产生任何影响.
>


##### ④ Media Data Box (mdat)

>该box包含于文件层, 可以有多个, 也可以没有(当媒体数据全部为外部文件引用时), 用来存储媒体数据. 数据直接跟在box type 字段后面, 具体数据结构的意义需要参考metadata(主要在(smpale table中描述).














**MP4结构图**

![这里写图片描述](http://img.blog.csdn.net/20180123175004214?watermark/2/text/aHR0cDovL2Jsb2cuY3Nkbi5uZXQvUG9pc3g=/font/5a6L5L2T/fontsize/400/fill/I0JBQkFCMA==/dissolve/70/gravity/SouthEast)





</font>
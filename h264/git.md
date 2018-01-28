<font face="微软雅黑"  >

### 一, gif格式图片分析

GIF文件内部是按块划分的，包括控制块（ Control Block ）和数据块（Data Sub-blocks）两种。控制块是控制数据块行为的，根据不同的控制块包含一些不同的控制参数；数据块只包含一些8-bit的字符流，由它前面的控制块来决定它的功能，每个数据块大小从0到255个字节，数据块的第一个字节指出这个数据块大小（字节数），计算数据块的大小时不包括这个字节，所以一个空的数据块有一个字节，那就是数据块的大小0x00



一个GIF文件的结构可分为文件头(File Header)、GIF数据流(GIF Data Stream)和文件终结器(Trailer)三个部分。文件头包含GIF文件署名(Signature)和版本号(Version)；GIF数据流由控制标识符、图象块(Image Block)和其他的一些扩展块组成；文件终结器只有一个值为0x3B的字符（’;’）表示文件结束

|文件头部分(Header)|GIF数据流部分(GIF Data Stream)|文件终结器(Trailer)|
|-|-|-|

</font>
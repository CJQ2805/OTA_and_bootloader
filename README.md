
用正点原子的IAP例程写的 boot程序， 分4个区, BOOT区， APP1区，固件信息区，APP2区
在应用层开辟巨大缓存存储外界数据（这里并不好，可以改为边存边读，减少缓存），数据接收完成写到APP2，然后给固件信息区写数据长度，标志位等，重启进入BOOT程序
BOOT程序中检测升级标志位，将数据一点一点的写到APP1中

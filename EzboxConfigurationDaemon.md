# 简介 #

ezbox配置管理服务进程（ezbox configuration daemon，简称ezcd）用于管理各个软件包的配置及启动、停止等运行操作。


# 详细说明 #

## 设计目的 ##
  * ezbox核心软件系统使用squashfs文件系统存储，用于存放软件包配置信息及系统启动配置信息的/etc目录默认是只读的，因此不能按照普通Linux系统对配置文件直接进行修改的方式来操作。为此，我们引入ezcd作为一个桥梁，自动完成用户配置存储与系统配置文件的相互映射。

## 实现细节 ##
  * ezcd使用unix域套结字（unix domain socket）进行进程间通信
  * ezcd使用监控者-工作者设计模式（monitor-worker model），在ezcd启动时初始化一个monitor线程，并监听/var/run/ezcd.socket套结字
  * ezcd内建《信息设备资源共享协同服务》协议(?3)，默认支持ISO/IEC 14543-5标准

## 实现参考 ##
  * ezcd的monitor-worker模型参考了udevd(?1)的实现，ezcd的多线程实现参考了mongoose(?2)的实现

# 参考资料 #

  * ?1 : udev相关信息可查阅http://www.kernel.org/pub/linux/utils/kernel/hotplug/udev.html
  * ?2 : mongoose的主页<a href='http://code.google.com/p/mongoose/'><a href='http://code.google.com/p/mongoose/'>http://code.google.com/p/mongoose/</a></a>
  * ?3 : 《信息设备资源共享协同服务》协议相关信息可参阅
  1. <a href='http://hes-standards.org/doc/SC25_WG1_N1253.pdf'> IGRS Part 5-1:Core Protocol </a>
  1. <a href='http://hes-standards.org/doc/SC25_WG1_N1294.pdf'> IGRS Part 5-21:Application Profile – AV Profile </a>
  1. <a href='http://hes-standards.org/doc/SC25_WG1_N1295.pdf'> IGRS Part 5-22:Application Profile – File Profile </a>
  1. <a href='http://hes-standards.org/doc/SC25_WG1_N1255.pdf'> IGRS Part 5-4:Device Validation </a>
  1. <a href='http://hes-standards.org/doc/SC25_WG1_N1296.pdf'> IGRS Part 5-5:Device type </a>
  1. <a href='http://hes-standards.org/doc/SC25_WG1_N1297.pdf'> IGRS Part 5-6:Service type </a>

  * ?4 ISO/IEC 14543-5 标准相关信息可参阅<a href='http://www.iso.org/iso/iso_catalogue/catalogue_tc/catalogue_detail.htm?csnumber=44391'> ISO/IEC 14543-5-1:2010 </a>
  * ?5 IGRS 协议相关的 IANA 分配资源
  1. 端口号 3880/tcp 及 3880/udp
  1. 多播地址 224.0.0.110
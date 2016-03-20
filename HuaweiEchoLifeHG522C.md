# 简介 #

本页介绍如何移植 ezbox 到 EchoLife HG522-C 华为电信宽带相框猫-科胜讯(Conexant)版本-硬件平台

# 详细说明 #

## 产品信息 ##
  * 技术标准 Annex A ANSI T1.413 [Issue 2](https://code.google.com/p/ezbox/issues/detail?id=2)，ITU-T G.992.1（G.DMT），G.992.3（ADSL2）和G.992.5（ADSL2+），802.11b/802.11g
  * 数据传输速率 G.992.5(ADSL2+): 下行最大24M bit/s，上行最大1M bit/s
  * Wi-Fi速率: 54，48，36，24，18，12，11，9，6，5.5，2和1M bit/s
  * 外部接口WAN: 一个ADSL/ADSL2+端口（RJ-11）
  * LAN: 四个自适应10/100 Base-T以太网端口（RJ-45）
  * WLAN: 一个天线，用于进行802.11b/g无线连接
  * USB: 一个USB接口，可以连接打印机和存储设备


## 设备基本信息 ##

  * 设备型号 : EchoLife HG522
  * 设备标识号 : 001E10-44200001E103C2B30
  * 硬件版本 : HG52MAGS
  * 软件版本 : HG522V100R001C02B023\_GDCT
  * 批次编号 : CWC2P0.023.E254142\_GDCT

## 硬件平台信息 ##

  * CPU : Conexant CX94610-11Z
  * SDRAM : Hynix HY57V281620FTP-6 X2 (32MB)
  * FLASH : Spansion S29GL128N10T (16MB)
  * SWITCH : Realtek RTL8306SD
  * WLAN : CX50221
  * RF-FEM: CX3510
  * USB : CX20552

## 串口针脚定义 ##
  * 板上预留 UART 口，丝印 J5 留有接口(插针)，其中第 4 PIN 空脚。上面还有个J6的焊盘（未知）。
  * 针脚从左到右分别是 : Txd Gnd Vcc 空脚 Rxd
  * 串口速率 : 38400 bps

## 固件升级方法 ##
  * 本地升级 :
  1. 将 PC 连接到 HGW 的任意一个以太网口；
  1. HGW 重新上电；
  1. 在上电后的 5 秒钟内，按住 reset 键 1 秒以上， POWER 起依次往左的三个灯亮后，表示可以进行本地升级。如果长时间三个灯都没有同时亮，系统可以已经跳过了本地升级的过程，请重新上电；
  1. 打开 IE ，键入地址 http://192.168.1.1/upload.html , 打开本地升级的页面。选择需要升级的 .y 文件，开始升级，如果需要升级 Bootloader ，则选择 .w 文件；
  1. 整个升级过程需要 3 分钟左右，升级后网关会自动重启。当启动升级后， PC 右下角的网络连接标志标识出再次可用时，表示升级完成。

## 升级文件的格式描述 ##
### .bin 文件 ###
  * 此文件仅包含主机程序相关资料，即只包含 Linux 内核，squashfs根文件系统
  1. (0x00000000 - 0x000003ff) .bin 文件开始的 1kB 部分包含系统的结构信息，从中可以得到 linux kernel 分区信息，squashfs 根文件系统信息以及启动时 bootloader 传递给 linux 内核的启动参数。
  1. (0x00000400 - 0x000bfff) .bin 文件这一段 767kB 部分包含 linux 内核
  1. (0x000c0000 - 0x003xxxx) .bin 文件这一段 包含 squashfs 根文件系统

### .y 文件 ###
  * 此文件包含固件相关资料，即包含 bootloader，Linux 内核，squashfs根文件系统
  1. (0x00000000 - 0x0001ffff) .y 文件开始 128kB 部分是系统的bootloader，其中在(0x00018000-0x0001ffff)用于存放产品相关信息，例如网卡的MAC地址，此MAC地址仅在bootloader被使用，默认是002233445566
  1. (0x00020000 - 0x000203ff) .y 文件此部分与 .bin 文件开始 1kB 部分内容相同
  1. (0x00020400 - 0x000dffff) .y 文件此部分与 .bin 文件的linux 内核部分内容相同
  1. (0x000e0000 - 0x0040xxxx) .y 文件此部分与 .bin 文件的squashfs根文件系统部分内容相同


## 登录帐号密码 ##
  * 网页电信管理员登录
  1. 登陆地址: http//192.168.1.1/
  1. 用户名: telecomadmin
  1. 密码: nE7jA%5m

  * telnet登录
  1. 登录地址: telnet 192.168.1.1
  1. 用户名: !!Huawei
  1. 密码: @HuaweiHgw

  * 串口登录
  1. 登录地址: 连接好串口线后，输入回车
  1. 用户名: !!Huawei
  1. 密码: @HuaweiHgw

  * 进入Linux环境
  1. 登录地址: 使用 telnet 或串口登录后，在 CLI 环境下输入 start
  1. 即： ATP> start

## 系统信息 ##
基于 V100R001C02B029SP01\_GDCT 版本固件

### CPU 信息 ###
  * Processor	: ARM1026EJ-Sid(wb)B rev 2 (v5l)
  * BogoMIPS	: 351.43
  * Features	: swp half thumb fastmult
  * CPU implementer	: 0x41
  * CPU architecture: 5TEJ
  * CPU variant	: 0x0
  * CPU part	: 0xa26
  * CPU revision	: 2
  * Cache type	: VIPT write-back
  * Cache clean	: cp15 c7 ops
  * Cache lockdown	: format C
  * Cache format	: Harvard
  * I size		: 16384
  * I assoc		: 4
  * I line length	: 32
  * I sets		: 128
  * D size		: 8192
  * D assoc		: 4
  * D line length	: 32
  * D sets		: 64
  * Hardware	: Solos CX4615
  * Revision	: 0000
  * Serial		: 0000002233445566

### FLASH 分区信息 ###
  * dev:    size   erasesize  name
  * mtd0: 00020000 00020000 "boot"
  * mtd1: 000c0000 00020000 "kernel"
  * mtd2: 00480000 00020000 "SQUASHFS"
  * mtd3: 000c0000 00020000 "none"
  * mtd4: 00400000 00020000 "JFFS2"

  * major minor  #blocks  name
  * 31     0        128 mtdblock0
  * 31     1        768 mtdblock1
  * 31     2       4608 mtdblock2
  * 31     3        768 mtdblock3
  * 31     4       4096 mtdblock4

### Linux 版本 ###
  * Linux version 2.6.8.1 (root@localhost.localdomain) (gcc version 4.0.1) #17 Fri Jun 5 10:41:20 CST 2009

### Linux 启动命令 ###
  * console=ttyS0 root=31:2 mem=31M r mtdparts=phys\_mapped\_flash:128k(boot),768k(kernel),4608k(SQUASHFS),768k(none),4m(JFFS2) rootfstype=squashfs

## 相似产品 ##
### Linksys WAG54G2 v1.0 ###
  * OpenWRT上的介绍 <a href='http://oldwiki.openwrt.org/OpenWrtDocs(2f)Hardware(2f)Linksys(2f)WAG54G2.html'><a href='http://oldwiki.openwrt.org/OpenWrtDocs(2f)Hardware(2f)Linksys(2f)WAG54G2.html'>http://oldwiki.openwrt.org/OpenWrtDocs(2f)Hardware(2f)Linksys(2f)WAG54G2.html</a></a>
  * Linksys 提供 GPL 源代码 <a href='http://homesupport.cisco.com/en-us/gplcodecenter'><a href='http://homesupport.cisco.com/en-us/gplcodecenter'>http://homesupport.cisco.com/en-us/gplcodecenter</a></a> 选择 WAG54G2 AnnexA 的 GPL 源代码

### Netgear DG834 v5 ###
  * Wiki 百科上的介绍 <a href='http://en.wikipedia.org/wiki/Netgear_DG834'><a href='http://en.wikipedia.org/wiki/Netgear_DG834'>http://en.wikipedia.org/wiki/Netgear_DG834</a></a>
  * Netgear GPL 源代码下载地址 [ftp://downloads.netgear.com/files/GPL/](ftp://downloads.netgear.com/files/GPL/)

### Conexant Solos SoC Linux BSP ###
  * OpenWRT 邮件列表的讨论 <a href='http://comments.gmane.org/gmane.comp.embedded.openwrt.devel/6586'><a href='http://comments.gmane.org/gmane.comp.embedded.openwrt.devel/6586'>http://comments.gmane.org/gmane.comp.embedded.openwrt.devel/6586</a></a>
  * conexant Solos SoC Linux BSP 下载地址 <a href='http://broadband.gennetsa.com/gpl/tellas-20090712/'><a href='http://broadband.gennetsa.com/gpl/tellas-20090712/'>http://broadband.gennetsa.com/gpl/tellas-20090712/</a></a>


# 常见问题 #

## 我的猫不能用本地升级方式升级固件 ##
  1. 请确认你进入到了boot固件升级模式
  1. 如果可以请连接你的串口线，观察升级时的串口输出信息，已知在boot中的网卡驱动与某些PC配置的网卡存在兼容性问题，如果你在串口中看到如下信息，请换一台PC使用其他类型的网卡重新尝试
```
Booting from Webserver

Boot from Ethernet Port
..TCP timeout retryCount = 0, state = 3
...TCP timeout retryCount = 0, state = 3
```

# 参考资料 #
  1. EchoLife HG522-c 华为电信宽带猫 <a><a href='http://hi.baidu.com/haitun198/blog/item/b50877b3a486e1acd8335a63.html'>http://hi.baidu.com/haitun198/blog/item/b50877b3a486e1acd8335a63.html</a></a>
  1. 无线宽带猫－－中国电信定制无线家庭网关产品大全 e8-B篇 <a href='http://hi.baidu.com/broadcom/blog/item/57a19c08d360cda62eddd4d4.html'><a href='http://hi.baidu.com/broadcom/blog/item/57a19c08d360cda62eddd4d4.html'>http://hi.baidu.com/broadcom/blog/item/57a19c08d360cda62eddd4d4.html</a></a>
  1. [CONEXANT系列芯片] 【华为Huawei】EchoLife HG522-C V100R001C03B016 <a href='http://www.chinaqos.net/viewthread.php?tid=39007'><a href='http://www.chinaqos.net/viewthread.php?tid=39007'>http://www.chinaqos.net/viewthread.php?tid=39007</a></a>
  1. [CONEXANT系列芯片] 【华为Huawei】EchoLife HG522-C V100R001C02B029SP01\_GDCT <a href='http://www.chinadsl.net/viewthread.php?tid=39799'><a href='http://www.chinadsl.net/viewthread.php?tid=39799'>http://www.chinadsl.net/viewthread.php?tid=39799</a></a>
  1. 解除华为 hg522-c ADSL无线路由Modem限制 <a href='http://hi.baidu.com/es1120/blog/item/79111b1a2841cdf3ae513305.html'><a href='http://hi.baidu.com/es1120/blog/item/79111b1a2841cdf3ae513305.html'>http://hi.baidu.com/es1120/blog/item/79111b1a2841cdf3ae513305.html</a></a>
  1. e8-b维修前期指导 <a href='http://www.chinadsl.net/viewthread.php?tid=28352'><a href='http://www.chinadsl.net/viewthread.php?tid=28352'>http://www.chinadsl.net/viewthread.php?tid=28352</a></a>
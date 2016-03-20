# 简介 #

本页介绍如何移植 ezbox 到 TP-Link TL-WR740N v4.0 无线路由器硬件平台

# 详细说明 #

## 产品信息 ##
  * 产品官方主页 <a href='http://www.tp-link.com.cn/pages/product-detail.asp?d=144'><a href='http://www.tp-link.com.cn/pages/product-detail.asp?d=144'>http://www.tp-link.com.cn/pages/product-detail.asp?d=144</a></a>

## 设备基本信息 ##
  * 当前软件版本： 3.10.8 Build 100708 Rel.55371n
  * 当前硬件版本： WR740N v3/v4 00000000

## 硬件平台信息 ##
  * CPU: Atheros AR7240 (400MHz)
  * SDRAM: Zentel A3S56D40FTP-G5 (32MB)
  * FLASH: Spansion FL032PIF (4MB)

## 串口针脚定义 ##
  * CPU pins : AR7240的pin 86为UART RX，pin 87位UART TX
  * PCB板上TP4与TP5焊孔与串口TX/RX针脚对应
  * 串口速率 : 115200 bps

## 固件升级方法 ##
  * u-boot升级
  1. 连接串口线，在设备启动时看到 Autobooting in 1 seconds 时快速输入"tpl"，进入u-boot环境
  1. 设置 tftp 服务器IP地址 ar7240> setenv serverip 192.168.1.253
  1. 从 tftp 服务器上获取固件 ar7240> tftpboot 0x80000000 fw.bin
  1. 擦除原有固件信息 ar7240> erase 0x9f020000 +0x3c0000
  1. 将下载到内存的固件写入flash中 ar7240> cp.b 0x80000000 0x9f020000 0x3c0000

  * 管理页面web升级

## 登录帐号密码 ##
  * 串口登录
  1. 登录地址: 连接好串口线后，输入回车
  1. 用户名: root
  1. 密码: 5up

## 系统信息 ##

## 相似产品 ##
### 水星 MW150R ###
  * 产品官方网页 <a href='http://www.mercurycom.com.cn/product-detail.asp?pd=10'><a href='http://www.mercurycom.com.cn/product-detail.asp?pd=10'>http://www.mercurycom.com.cn/product-detail.asp?pd=10</a></a>

# 常见问题 #

# 参考资料 #
  1. TP-LINK GPL Code Center <a href='http://www.tp-link.com/support/gpl.asp'><a href='http://www.tp-link.com/support/gpl.asp'>http://www.tp-link.com/support/gpl.asp</a></a>
  1. Is tp-link WR741ND v1 supported? <a href='https://forum.openwrt.org/viewtopic.php?pid=102069'><a href='https://forum.openwrt.org/viewtopic.php?pid=102069'>https://forum.openwrt.org/viewtopic.php?pid=102069</a></a>
  1. 资料收集，请有机的兄弟贡献下！ <a href='http://www.right.com.cn/forum/viewthread.php?tid=38224'><a href='http://www.right.com.cn/forum/viewthread.php?tid=38224'>http://www.right.com.cn/forum/viewthread.php?tid=38224</a></a>
  1. 全球首发--tplink 941 （ar9132） usb 成功驱动 <a href='http://www.right.com.cn/forum/viewthread.php?tid=38742'><a href='http://www.right.com.cn/forum/viewthread.php?tid=38742'>http://www.right.com.cn/forum/viewthread.php?tid=38742</a></a>
  1. tp link系列usb电路芯片适合wr941n wr841n wr741n <a href='http://www.wifi.com.cn/forum/viewthread.php?tid=38214'><a href='http://www.wifi.com.cn/forum/viewthread.php?tid=38214'>http://www.wifi.com.cn/forum/viewthread.php?tid=38214</a></a>
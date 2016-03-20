# 简介 #

ezbox 使用 buildbot 进行自动编译测试，本页介绍如何设定 buildbot 参与测试 ezbox 的不同发布版本。

# 详细说明 #
## 安装 buildbot ##
  * debian/ubuntu 用户可以使用 apt-get install buildbot

## 配置 buildbot-slave ##
  * master：buildbot.ezidc.net:9989
  * 用户名：发布版本的名字，目前支持 kuafu , huangdi
  * 密码：与用户名相同
  * 使用 buildbot create-slave 命令创建你的 buildbot-slave 实例
  * debian/ubuntu 用户请修改 /etc/default/buildbot 的配置，然后运行 /etc/init.d/buildbot restart

## 设定编译环境参数 ##
  * ezbox 自动编译时会检查编译环境参数设定文件，并根据文件中的配置编译对应目标系统的发布版本
  * BASEDIR/BUILDDIR/default.mk 为编译环境参数设定文件，例如你希望编译ar71xx目标系统的kuafu发布版本，请在 BASEDIR/ezbox-kuafu/目录下编辑如下的default.mk
```
#
# default setting for kuafu buildbot
#

DISTRO=kuafu
TARGET=ar71xx
DEVICE_TYPE=ezbox
ARCH=mips
```

## 设定编译环境信息 ##
> 为了方便定位编译出错的根源，请参与自动编译测试的主机管理员设定好一下反馈信息
  * BASEDIR/info/admin 请填入你的名字（或者昵称）以及可以直接与你联系的方法，如电子邮件或QQ等
  * BASEDIR/info/admin 的例子： zetalabs <zetalabs@gmail.com>
  * BASEDIR/info/host 请填入测试使用的主机环境，包括操作系统类型及版本，内存大小，CPU型号及频率，所安装的与编译有关的库，所使用的 buildbot 版本
  * BASEDIR/info/host 的例子： GNU/Linux 2.6.30-2-amd64, 2GB RAM, AMD Athlon(tm) 64 Processor 3000+ 2025MHz, Debian squeeze, Buildbot version: 0.7.12, Twisted version: 10.1.0
  * Linux下获取操作系统类型可以使用 uname -o
  * Linux下获取操作系统内核版本可以使用 uname -r
  * Linux下获取操作系统内存大小可以使用 free，此处请使用实际物理内存大小
  * Linux下获取CPU型号及频率可以使用 cat /proc/cpuinfo
  * Linux下所安装的与编译有关的库可以使用发布版本来代替
  * buildbot版本可以使用 buildbot --version 获取
  * 如果使用虚拟机进行编译测试，请将虚拟机名字和版本，以及虚拟机运行的宿主系统信息一并描述

## 编译结果网址 ##
  * <a href='http://buildbot.ezidc.net:8010/'><a href='http://buildbot.ezidc.net:8010/'>http://buildbot.ezidc.net:8010/</a></a>

## 申请测试账号 ##
  * 如果你希望设定单独的自动编译测试帐号，请与 zetalabs@gmail.com 联系
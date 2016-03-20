# 简介 #

本页指导用户如何搭建 ezbox 的开发环境。

# 详细说明 #

ezbox 的开发在ubuntu/Linux发行版本中完成，工作机器使用MS Windows操作系统或Mac OS操作系统的用户可以使用虚拟机来搭建 ezbox 的开发环境。

## MS Windows 用户 ##
  * 到 <a href='http://www.virtualbox.org/'> www.virtualbox.org </a>下载最新的 virtualbox 安装程序
  * 安装 virtualbox
  * 在virtualbox 中新建一个虚拟电脑，操作系统选择 Linux，版本选择 Ubuntu，内存大小选择512M或更多，创建新的虚拟硬盘，容量在8G或以上
  * 网络配置2张网卡，网卡1使用NAT模式，透过主机系统的网络连接与互联网相连，网卡2使用host-only模式，用于与主机系统进行网络通信
  * 在新建的虚拟电脑中使用.iso文件安装 Ubuntu/Linux 操作系统
  * 配置虚拟电脑中的Ubuntu系统的网络连接，使虚拟电脑的Ubuntu系统可以上网以及与主机Windows系统进行网络通信
  * 后续步骤参考 Ubuntu/Linux 用户的操作步骤

## Ubuntu/Linux 用户 ##
> 所有操作在 Ubuntu 10.04 netbook edition (Lucid) 上完成，Ubuntu 其他版本操作类似

  * 安装开发使用的工具软件
```
ezbox@ezbox-laptop:~$ sudo apt-get install build-essential bison flex zlib1g-dev libncurses5-dev subversion quilt git-core texinfo intltool
```

  * 修正 /bin/sh 链接到 /bin/bash
```
ezbox@ezbox-laptop:~$ sudo rm -f /bin/sh
ezbox@ezbox-laptop:~$ sudo ln -sf bash /bin/sh
```

  * 在用户目录中建立开发使用的工作目录
```
ezbox@ezbox-laptop:~$ mkdir -p workplace/ezbox
ezbox@ezbox-laptop:~$ cd workplace/ezbox/
```

  * 下载 ezbox 开发代码，开发代码将保存在工作目录的trunk目录下
```
ezbox@ezbox-laptop:~/workplace/ezbox$ svn checkout http://ezbox.googlecode.com/svn/trunk/
```

  * 编辑 default.mk ，设定 ezbox 的目标系统和目标发行版本
```
ezbox@ezbox-laptop:~/workplace/ezbox$ cp trunk/default.mk.sample ./default.mk
ezbox@ezbox-laptop:~/workplace/ezbox$ vi default.mk
```

  * default.mk 的示例，示例为构建 ar71xx 系统的 kuafu 发行版本，该版本可在 TP-Link 的 TL-WR740N 路由器上运行
```
#
# Sample default.mk for ezbox quick start
#

# DISTRO set the ezbox distro to be build
# support [kuafu|huangdi|jingwei]
DISTRO := kuafu

# TARGET set the ezbox target running board
# support [x86|ar71xx|brcm47xx|s3c24xx]
TARGET := ar71xx

# DEVICE_TYPE set the openwrt device type
# Should always be ezbox
DEVICE_TYPE := ezbox

# ARCH set the CPU arch, please see README to set correct TARGET/ARCH
# support [i386|mips|mipsel|arm]
ARCH := mips

# LOG_FILE set default build log file name
# default name is at $(BASE_DIR)/$(shell date --iso=seconds)-$(DISTRO)-$(TARGET)-build.log
# LOG_FILE :=

# LOG_LEVEL set openwrt build log verbose level
# Set it to 99 for debugging
LOG_LEVEL := 99
```

  * 编译目标 ezbox 系统固件
```
ezbox@ezbox-laptop:~/workplace/ezbox$ cd trunk/
ezbox@ezbox-laptop:~/workplace/ezbox/trunk$ make
```

# 参考资料 #
# 简介 #

本页介绍ezus在MCS51系列单片机上的移植，移植测试在EdSim51模拟器上完成

# 详细说明 #
## 基本概念 ##
  * 宇宙（universe）：系统所有资源的总和，硬件可用资源是系统运行的基础，软件规则是系统运行的轨迹描述。
  * 大爆炸（big bang）：系统启动的过程。大爆炸理论认为宇宙诞生于一次大爆炸，在大爆炸的瞬间宇宙的运行规则就确定了。
  * 黑洞（black hole）：系统终止的过程。事实上，黑洞并非系统运行的终止时点，当黑洞不断吞噬周围的物质时，自身质量也在增大，而体积却在缩小，当物质密度达到某个临界值时，又一次的大爆炸将发生，于是一个新的宇宙诞生了。
  * 道（Tao）：老子的“道”可以指称天地万物及其运行规律，它不以人的意志而改变。
  * 德（Teh）：老子的“德”可以理解为“顺势而为”，即人主观上顺应“道”的规律行事。
  * 混沌（Chaos）：传说中指宇宙形成前模糊一团的景象。
  * 世界（World）：宇宙在人脑中的主观影像。

## 文件及目录说明 ##
  * board/edsim51/edsim51/ezus.s 包含系统启动时执行的第一条指令，另外，依据8051的中断向量定义，在此文件中同时构建了8051的中断向量表（0x0003 ~ 0x0023）
  * board/edsim51/edsim51/chaos.s 包含系统初始状态检测代码，在此文件中系统运行轨迹将分为两支：系统运行状态未准备好，运行轨迹将进入黑洞状态（bh.c文件中的black\_hole()函数）；系统运行状态已准备好，运行轨迹将进入大爆炸状态（bb.c文件中的big\_bang()函数）
  * board/edsim51/edsim51/bh.c 包含系统运行状态没有准备好时，或系统退出时的运行代码（bh 是 Black Hole 的缩写）
  * board/edsim51/edsim51/bb.c 包含系统大爆炸时的运行代码（bb 是 Big Bang 的缩写)，此处将初始化宇宙的时空度量
  * board/edsim51/edsim51/universe.c 包含宇宙形成时的运行代码，此处将规定宇宙运行的规则
  * board/edsim51/edsim51/world.c 包含世界形成的运行代码
  * board/edsim51/edsim51/init.c 包含用户初始线程的运行代码

## 构建步骤 ##
  * ezus 在 EdSim51 上的移植使用 SDCC 编译器，以下操作在 Ubuntu/Linux 操作系统下完成

### 编译 SDCC v3.0.0 ###
  * 从 <a href='http://sdcc.sourceforge.net/'><a href='http://sdcc.sourceforge.net/'>http://sdcc.sourceforge.net/</a></a> 处获取最新的 SDCC 源码
  * 配置 sdcc，只选择mcs51的支持，并且没有使用 SDCC 提供的设备支持库
```
./configure --disable-gbz80-port --disable-z80-port --disable-ds390-port --disable-ds400-port --disable-pic-port --disable-pic16-port --disable-hc08-port --disable-ucsim --disable-device-lib --enable-libgc
```
  * 编译 sdcc
```
make
```
  * 安装 sdcc，默认安装到 /usr/local 目录下
```
sudo make install
```


## 实现细节 ##
### 8051单片机的复位状态 ###
复位就是指通过某种手段使单片机内部某些资源一种固定的初始状态，以确保单片机每次复位后都能在某一固定的环境中从某一固定的入口地址处开始运行，8051复位后片内各特殊功能寄存器状态如下：
  * PC内容为0000H，使单片机从起始地址0000H开始执行程序。所以单片机运行出错或进入死循环，可以按复位键重启
  * ACC内容为00H。
  * PSW内容为00H，意义为复位后，上次运算没有产生进位或借位，也没有产生半进位或半借位，工作寄存器组R0—R7定位在内部RAM的00-07单元，上次运算没有溢出，累加器中“1”的个数为偶数。
  * SP内容为07H，意义为堆栈指针SP指向内部数据存储器的07单元，即堆栈从08单元开始，
  * DPTR为0000H
  * PO-P3内容为FFH
  * IP的第六位为0，高六位的值不变
  * IE次高位即位6的值不定，其于位全为0
  * TMDO内容为00
  * TCON内容为00
  * TLO内容为00
  * THO内容为00
  * TL1内容为00
  * TH1内容为00
  * SCON内容为00
  * PCON的位六位五位四位内容不变，其余位都为0
  * 除以上寄存器外，其余的数据寄存器内容都为随机数。

### 8051单片机的中断系统 ###
  * 中断源      -- 中断标志 -- 中断向量
  * 
---

  * 上电复位    -- RST      -- 0000H
  * 外部中断0   -- IE0      -- 0003H
  * 定时器0溢出 -- TF0      -- 000BH
  * 外部中断1   -- IE1      -- 0013H
  * 定时器1溢出 -- TF1      -- 001BH
  * 串行口中断  -- RI/TI    -- 0023H
  * 
---


### 系统启动过程 ###
  * 硬件上电后，8051从内部ROM的0x0000地址开始执行指令，系统的第一条指令在ezus.s的system\_reset:标签位置，这是一条长跳转指令，8051将转到stay\_at\_chaos:标签处的指令
  * stay\_at\_chaos:标签表示系统还处在混沌状态，此时需要校验各种寄存器的状态，如果有寄存器状态不符合8051复位状态，表示系统还没有准备好，此时将跳转到fall\_into\_black\_hole\_1:或者fall\_into\_black\_hole\_2:标签
  * 跳入fall\_into\_black\_hole\_1:标签的运行轨迹表示，在chaos状态的中断寄存器中的中断使能标志位EA的状态没有符合8051复位状态的中断使能标志位状态，即当前EA=1而不是EA=0
  * 跳入fall\_into\_black\_hole\_2:标签的运行轨迹，下一步将跳转到black\_hole()函数执行，此处不用lcall而是使用ljmp表示black\_hole()函数是一个不需要返回的执行轨迹
  * black\_hole()函数在bh.c文件中，通常black\_hole()表示系统进入了一个对用户而言不可用的状态，此时需要外界给与一定的指令，系统才会继续运行下去，在此函数中可以加入向上位机传输系统当前状态的信息，并等待上位机给与下一步执行的指令，需要注意的是在black\_hole()中应尽量小的破坏当前系统状态，以保证向上位机传输的系统当前状态信息的准确性
  * 如果在stay\_at\_chaos:标签中的状态检测符合8051复位状态，此时系统将跳入start\_big\_bang:标签
  * 跳入start\_big\_bang:标签的运行轨迹表示，系统已经准备好了，即将跳入big\_bang()函数，此时混沌初开，宇宙即将诞生
  * big\_bang()函数在bb.c文件中，big\_bang()函数初始化好当前宇宙运行环境，包括可用空间，计时机制，然后调用universe()函数，开始宇宙的运行。需要注意的是big\_bang()仅规定宇宙运行的时空维度，其他宇宙内部结构将在universe()函数中规定。
  * universe()函数在universe.c文件中，universe 层相当于传统计算机体系结构中的硬件抽象层 HAL，为更高层 world 层提供必要的硬件访问接口，也可以用来构建hypervisor，实现虚拟硬件系统
  * universe()函数将规定universe内部运行机制，包括I/O操作接口，可用的调度策略的，完成初始化工作后，universe()函数调用world()函数，系统运行轨迹进入世界(world)层面
  * world()函数在world.c文件中，world 层相当于传统计算机体系结构中的操作系统层(OS)，为用户的应用程序提供必要的访问接口，在edsim51的移植示例中，我们在这一层实现一个可在8051单片机上使用的RTOS
  * world()函数初始化好本层的空间使用，时钟计时以及用户层线程管理需要的信息后调用init()函数
  * init()函数在init.c文件中，init()函数相当于传统计算机系统结构中的用户层初始进程，即PID=1的init进程
  * init()函数实现用户层初始线程，通常是一个空闲线程

### 系统存储空间分配 ###
  * chaos, black hole及big bang相关的运行代码使用bank 0
  * universe层相关的运行代码使用bank 1
  * world层相关的运行代码使用bank 2
  * 用户层线程运行代码使用bank 3

### 重要的宏 ###
  * W\_SP\_BOTTOM : world层栈底位置，init线程用户栈起始位置，定义为0x30
  * W\_SP\_TOP : world层栈顶位置，实际指向data区起始位置，即可用栈空间+1的位置，定义为(W\_IRAM\_SIZE - W\_DATA\_SIZE)

### 线程切换 ###
  * 线程切换发生在两种情况，第一：线程调用 thread\_wait() 函数主动放弃控制进行线程调度；第二：中断发生后，如时钟中断发生时，有高优先级的线程准备就绪，在时钟中断处理结束，准备中断返回时，会调用thread\_context\_switch()进行线程切换
  * 线程切换时需要保存当前运行线程的运行现场，这就要求保存通用寄存器 [R0](https://code.google.com/p/ezbox/source/detail?r=0)-[R7](https://code.google.com/p/ezbox/source/detail?r=7)，数据指针寄存器 DPL 和 DPH，寄存器 B，累加器 ACC，程序状态字 PSW
  * 此外，在SDCC中，线程切换还需要保存用\_bp，（ _**bp is the stack frame pointer and is used to compute the offset into the stack for parameters and local variables.** ）_


# 参考 #
  * Intel MCS-51 <a href='http://en.wikipedia.org/wiki/Intel_MCS-51'><a href='http://en.wikipedia.org/wiki/Intel_MCS-51'>http://en.wikipedia.org/wiki/Intel_MCS-51</a></a>
  * EdSim51 - The 8051 Simulator for Teachers and Students <a href='http://www.edsim51.com/'><a href='http://www.edsim51.com/'>http://www.edsim51.com/</a></a>
  * JMCE - Java Multiple Computer Emulator <a href='http://www.viara.cn/en/jmce/'><a href='http://www.viara.cn/en/jmce/'>http://www.viara.cn/en/jmce/</a></a>
  * CMON51 - CMON51 is a monitor program for the 8051 family of microcontrollers.<a href='http://cmon51.sourceforge.net/'><a href='http://cmon51.sourceforge.net/'>http://cmon51.sourceforge.net/</a></a>
  * 嵌入式实时操作系统small RTOS51原理及应用 (陈明记 周立功 等编著)
  * Operating Systems Design and Implementation, Third Edition (By Andrew S. Tanenbaum -  Vrije Universiteit Amsterdam, The Netherlands, Albert S. Woodhull -  Amherst, Massachusetts)
  * 源码开放的嵌入式实时操作系统T-Kernel （日）坂村健 等著，周立功 等译
  * SDCC Compiler User Guide <a href='http://sdcc.sourceforge.net/doc/sdccman.html/'> <a href='http://sdcc.sourceforge.net/doc/sdccman.html/'>http://sdcc.sourceforge.net/doc/sdccman.html/</a> </a>
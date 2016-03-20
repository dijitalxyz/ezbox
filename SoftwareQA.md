# 简介 #

> 本页将引导ezbox的使用者利用各种开源工具进行ezbox的软件代码查错及软件健壮性检查，并建立实际产品开发时所需的软件质量保证概念。

# 详细说明 #

> 为达到企业级软件质量保证，在ezbox的构建发布这个循环学习过程中，我们同样关注ezbox的代码质量。在软件工程实践中，软件的质量问题从软件的设计阶段起一直持续到产品部署及后期维护。以下给出ezbox在开发各阶段所使用到的工具。

## 软件设计阶段 ##
  * 测试驱动的开发方法使用 xxx 在编写代码前进行测试用例的编写

## 软件实现阶段 ##
  * 代码评审(code review)使用 Clang Static Analyzer (?2) 进行源代码分析

## 测试阶段 ##
  * 黑盒测试
  * 白盒测试
  * 回归测试
  * 性能测试
  * 压力测试

## 产品部署 ##
  * 缺陷跟踪管理
  * 现场压力测试

# 工具介绍 #
## 源代码分析工具 ##
  * splint (?3)
  * Clang Static Analyzer

# 参考资料 #
  * ?1 "Finding Bugs in Open Source Kernels using Parfait" <a href='http://labs.oracle.com/projects/downunder/publications/kca09-slides.pdf'> <a href='http://labs.oracle.com/projects/downunder/publications/kca09-slides.pdf'>http://labs.oracle.com/projects/downunder/publications/kca09-slides.pdf</a> </a>
  * ?2 Clang Static Analyzer 主页 <a href='http://clang-analyzer.llvm.org/'> <a href='http://clang-analyzer.llvm.org/'>http://clang-analyzer.llvm.org/</a> </a>
  * ?3 splint 主页 <a href='http://www.splint.org/'> <a href='http://www.splint.org/'>http://www.splint.org/</a> </a>
  * ?4 开源测试工具 <a href='http://www.opensourcetesting.org/'> <a href='http://www.opensourcetesting.org/'>http://www.opensourcetesting.org/</a> </a>
  * ?5 Check: A unit testing framework for C <a href='http://check.sourceforge.net/'> <a href='http://check.sourceforge.net/'>http://check.sourceforge.net/</a> </a>
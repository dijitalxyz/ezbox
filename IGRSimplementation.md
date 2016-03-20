# 简介 #

本页指导应用程序开发者如何实现闪联协议栈，为应用程序增加闪联协议支持。


# 详细说明 #

  * 闪联协议基于 HTTP/1.1 ， SOAP/1.2 及 WSDL/2.0 协议标准。
  * 从实现的角度讲可以遵循从底层协议逐层往上实现整个协议栈，即先实现网络传输层的工作，包括 UDP 的广播包， TCP 的点对点传输；然后实现 HTTP/1.1 的协议内容，再实现封装在 HTTP 协议数据单元中的 SOAP/1.2 的协议内容。
  * 也可以遵循自顶向下逐层细化的方法实现整个协议栈，即先根据协议编写对应的 WSDL/2.0 描述文件，在描述文件中指明使用 SOAP/1.2 绑定，并且传输协议使用 HTTP/1.1 ；然后使用自动转换工具，将 WSDL/2.0 描述文件转换为编程语言框架；最后扩充框架内容支持完整的闪联协议。


## 自底向上的方法 ##



## 自顶向下的方法 ##


# 常见问题 #

  1. 

# 相关协议文档 #
  1. <a href='http://hes-standards.org/doc/SC25_WG1_N1253.pdf'> IGRS Part 5-1:Core Protocol (CD) </a>
  1. <a href='http://hes-standards.org/doc/SC25_WG1_N1314.doc'> IGRS Part 5-1:Core Protocol (FCD) </a>
  1. <a href='http://hes-standards.org/doc/SC25_WG1_N1294.pdf'> IGRS Part 5-21:Application Profile – AV Profile </a>
  1. <a href='http://hes-standards.org/doc/SC25_WG1_N1295.pdf'> IGRS Part 5-22:Application Profile – File Profile </a>
  1. <a href='http://hes-standards.org/doc/SC25_WG1_N1255.pdf'> IGRS Part 5-4:Device Validation </a>
  1. <a href='http://hes-standards.org/doc/SC25_WG1_N1296.pdf'> IGRS Part 5-5:Device type </a>
  1. <a href='http://hes-standards.org/doc/SC25_WG1_N1297.pdf'> IGRS Part 5-6:Service type </a>
  1. <a href='http://www.iso.org/iso/iso_catalogue/catalogue_tc/catalogue_detail.htm?csnumber=44391'> ISO/IEC 14543-5-1:2010 Information technology -- Home electronic system (HES) architecture -- Part 5-1: Intelligent grouping and resource sharing for Class 2 and Class 3 -- Core protocol </a>
  1. <a href='http://www.iso.org/iso/catalogue_detail.htm?csnumber=51469'> ISO/IEC 14543-5-22:2010 Information technology -- Home electronic system (HES) architecture -- Part 5-22: Intelligent grouping and resource sharing for HES Class 2 and Class 3 -- Application profile -- File profile </a>
  1. <a href='http://www.faqs.org/rfcs/rfc2616.html'> RFC2616 - Hypertext Transfer Protocol -- HTTP/1.1 </a>
  1. <a href='http://www.faqs.org/rfcs/rfc2774.html'> RFC2774 - An HTTP Extension Framework </a>
  1. <a href='http://www.faqs.org/rfcs/rfc4122.html'> RFC4122 - A Universally Unique IDentifier (UUID) URN Namespace </a>
  1. <a href='http://www.faqs.org/rfcs/rfc1750.html'> RFC1750 - Randomness Recommendations for Security </a>
  1. <a href='http://www.w3.org/TR/wsdl20/'> Web Services Description Language (WSDL) Version 2.0 Part 1: Core Language </a>
  1. <a href='http://www.w3.org/TR/2007/REC-soap12-part1-20070427/'> SOAP Version 1.2 Part 1: Messaging Framework (Second Edition) </a>
  1. <a href='http://www.w3.org/TR/2006/REC-xml-20060816/'> Extensible Markup Language (XML) 1.0 (Fourth Edition) </a>
  1. <a href='http://www.w3.org/TR/2004/REC-xml-infoset-20040204/'> XML Information Set (Second Edition) </a>

# 参考资料 #
  1. IGRS Media Server on Linux <a><a href='http://code.google.com/p/igrs-media-server/'>http://code.google.com/p/igrs-media-server/</a></a>
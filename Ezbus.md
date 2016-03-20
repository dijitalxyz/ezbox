# 简介 #

本页介绍用于ezbox 应用服务间通信的 ezbus 嵌入式服务总线架构，设计思想及实现细节。

# 详细说明 #

ezbox 作为一个完整的嵌入式系统，其所提供的各种应用服务间相互通信借助于 ezbus 嵌入式服务总线来完成。

## 设计目的 ##
  * 嵌入式系统中各个应用服务间的通信接口
  * 粘合内核层与应用层不同的应用服务
  * 为独立第三方应用服务管理提供统一的接口

## 实现考量 ##
  * ezbus 嵌入式服务总线是一种中间件(?1)，位于内核层与应用层之间，起到粘合不同应用服务的作用
  * ezbus 的语义描述使用WSDL 2.0
  * ezbus 的本地实现参考 D-bus
  * ezbus 的远程实现通过 WSDL 2.0 到 HTTP 1.1 的绑定完成

# 参考资料 #
  * ?1: 中间件的 wiki 请参考 http://en.wikipedia.org/wiki/Middleware
  * ?2: D-bus项目主页 http://www.freedesktop.org/wiki/Software/dbus
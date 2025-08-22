<!--
 * @encode: utf-8
 * @Date: 2025-08-22 21:17:17
 * @LastEditTime: 2025-08-22 21:37:46
 * @FilePath: /TDSetting/Qt/setting/README.md
-->
# 极简设置

这是一个基于`Qt`项目的程序

![Qt Version](https://img.shields.io/badge/Qt-6.9.1-green.svg)
![License](https://img.shields.io/badge/License-MIT-blue.svg)
![Platform](https://img.shields.io/badge/Platform-Windows%20%7C%20Linux%20%7C%20macOS-lightgrey.svg)

使用:

使用yaml格式的配置文件

格式如下:

需要图形化配置的,在`#`注释中使用//$ title: 标题; detail: 详细描述; type: 类型

标题与详细描述可以填写除了`;`以外的任意字符
`type`字段为可选,`bool`或者`string`
`bool`类型适用于`n: false`这种
`string`则适用以`"`或者`'`包裹字符串

使用示例:
```
name: Setting
version: false      # //$ title: version; detail: 版本信息; type: bool
version: false      # //$ title: version2; detail: 版本信息2; type: bool
version: true      # //$ title: version3; detail: 版本信息3; type: bool

window:
  width: false    # //$ title: width; detail: 宽; type: bool

wge: "lsdagghrhgewuy"   # //$ title: wge; detail: wget; type: string
wgw: "和agrwh4g好人" # //$ title: wge; detail: wget; type: string
```
图形化显示:
![setup](./READMEimg/em.png)
好像有点丑,hh
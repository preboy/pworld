
在开发libCore的时候， 注释掉stdafx.h中的如下行
#include "libcore.hpp"
某个模块若需要其它模块的头文件，则包括在当前开发模块的头文件中include其它模块的头文件
如若不然，则每次更新都要重新编译libcore, it's to slow.

libcore发布时：
还愿stdafx.h中的注释行，
在libcore.hpp中包括各个模块的头文件，
去掉各模块头文件中依赖其它模块的头文件《可选》 To keep code tidy.

# 准备

requirements

- vs code
- waf
- ns3
- gdb

# 环境构建

## 硬件开发环境
最好的是通过windows的笔记本, 还有一个ubuntu的工作站, 两个机器协同工作.两个机器都分别具有有线无线网卡, 有线网卡两台机器互联,文件互相访问的时候接近同一台机器的感觉; 无线各自连接. 

具体实现如下


- 利用 ubuntu、 `samba`、 端到端局域网、 共享文件夹, 实现windows访问ubuntu机器的文件、双向传输文件**透明**.
- vncViewer(windows端) 和RealVnc(ubuntu端) 进行局域网内远程桌面, 实现一个键盘(笔记本的键盘)无缝跳跃在两台机器上, 切粘贴板共用.

## 软件开发环境

主要在 `.vscode/tasks.json`中编写常用task, 例如 waf build, waf --run 等

//todo

## 软件开发环境调试

非实时调试包括logs(包括print调试大法)和测试(要编写匹配的测试代码);
实时调试有两种方法, 一个是**gdb命令行**, 一个是vscode利用gdb进行**ui调试**. 不嫌麻烦gdb命令行也可以， 如下

```
./waf --run scratch/test.cc --command-template="gdb %s"
```
- 注意, gdb命令行调试需要环境变量配好,后文有详细.
  
这里主要讲通过vscode界面、gdb调试器进行**UI**调试。



1. ns3 的头文件以及共享库文件
   
   将ns3编译出的共享库文件`.so`文件放到合适的地方,并添加到环境变量`LD_LIBRARY_PATH` 以及 `C_INCLUDE_PATH`中. 虽然waf中是集成环境, 里面的开发等不需要更改、添加任何环境变量，但是为了使用vscode的调试，需要将共享库文件放到检索lib下，否则调试会报错，类似如下：

    ` libns3.34-aodv-optimized.so: cannot open shared object file: No such file or directory`

    上面的报错是第一个共享库没找到。
    这里`~.profile`文件最后添加

    ```bash
    NS3_WORK_DIR=/home/roit/programs_pro/ns-allinone-3.34/ns-3.34
    export C_INCLUDE_PATH=$NS3_WORK_DIR/build/debug/ns3
    export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$NS3_WORK_DIR/build/debug/lib
    export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$NS3_WORK_DIR/build/optimized/lib
    ```
   注意
    
    - 编写后执行`source .profile`来启用. 记得执行后需要关闭vscode再打开才能正式启用.
    
    - 添加后, 可以使用`ldd my-first`来测试是否链接到正确的共享库中

    - `ns-3.xx/src`和`ns-3.xx/contrib`的module文件会被编译成`.so`, 并放置到`ns-3.xx/lib`下, 如果build的模式不同, `.so`文件的名字也不同,lib中到底是`*-debug.so`还是`*-optimized.so`, 需要注意. 而且gdb命令行调试也需要这一步

2. 编写 `.vscode/lauch.json`文件, 如下
    
    ```json
        
        {
        // Use IntelliSense to learn about possible attributes.
        // Hover to view descriptions of existing attributes.
        // For more information, visit: https://go.microsoft.com/fwlink/?linkid=830387
        "version": "0.2.0",
        "configurations": [
            
            {
                "name": "waf debug",
                "type": "cppdbg",
                "request": "launch",
                "cwd": "/home/roit/programs_pro/ns-allinone-3.34/ns-3.34",
                "program": "${workspaceFolder}/build/optimized/${relativeFileDirname}/${fileBasenameNoExtension}",
                "args": ["--run_dir='ns3ws/data/one_tcp_flow'"],
                "stopAtEntry": false,
                "environment": [],
                "externalConsole": false,
                "MIMode": "gdb",
                "setupCommands": [
                    {   
                        "description": "Enable pretty-printing for gdb",
                        "text": "-enable-pretty-printing",
                        "ignoreFailures": true
                    }
                ],
                "preLaunchTask": "waf build",
                "miDebuggerPath": "/usr/bin/gdb"
                // "miDebuggerPath": "/home/roit/programs_pro/ns-allinone-3.34/ns-3.34"
            }
        ]
        }

    ```

    `program`举例：
    
    main.cc路径：
        `/home/roit/programs_pro/ns-allinone-3.34/ns-3.34/ns3ws/scripts/my-main`

    则program路径：
        `/home/roit/programs_pro/ns-allinone-3.34/ns-3.34/build/optimized/ns3ws/scripts/my-main/my-main`

    主要是根据源码，映射到`build`的程序里去

3. 对着脚本按`f5`, 测试能否调试


# 调试常见错误:

1. 共享库文件找不到
   
    ` libns3.34-aodv-optimized.so: cannot open shared object file: No such file or directory`
    首先 `ldd 可执行文件` 看看是否链接到了, 如果没有, 检查环境变量是否写错.


2. 符号未定义

    `symbol lookup error undefined symbol`

    执行`nm -g *.so |grep XXXX` 看看是否有这个符号, 有的话有没有定义, 如果标识符是U就是没定义仅声明.
    例如头文件中声明,没定义, 在`.cc`直接用了就会这样, 这种情况可能是`include`不匹配造成的.



3. 可视化调试的时候乱跳

    代码和实际链接的`.so`并不对应!
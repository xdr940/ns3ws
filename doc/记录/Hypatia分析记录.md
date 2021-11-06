# `Hypatia`源码记录



- 重载构造函数, 可以通过复杂的, 弄出一个简化版的.

- simulation -> accessor() 来访问config变量, 虽然安全, 但是感觉并不方便.

- `arbiter-ecmp-helper`这里， `global_ecmp_state` 弄成个三维链表。



# 编译报错

## `main_satnetwork.cc`相关报错

例如 `pingmesh.h` 或者`tcp-optimizer.h`等缺失情况.

因为Hypatia 仓库构建是依赖了一个submodule `basic-sim`仓库, 而且版本是`basic-sim-3b32597c183e1039be7f0bede17d36d354696776`,并不是最新的版本. 上述缺失的文件都是这个旧版本的module里的内容. 作者在21年1月份对`basic-sim`继续更新,导致其不适用与最新的`Hypatia`. 这个通过在Hypatia issue里面提到了,需要版本正确, 通过指令`git submodule update --init --recursive`更新时候会很慢, 直接到[对应分支页面下载](https://github.com/snkas/basic-sim/tree/3b32597c183e1039be7f0bede17d36d354696776)放到对应目录即可



## `satellite-network` 编译报错
 - 在build`satellite-network`这个module中, `sgi-hashmap.h`会缺失, 这个只有3.31版本有,后续的就删除了,报错如下

```bash
In file included from ../../contrib/satellite-network/helper/gsl-helper.cc:31:0:
./ns3/gsl-channel.h:30:10: fatal error: ns3/sgi-hashmap.h: No such file or directory
 #include "ns3/sgi-hashmap.h"
```

- 成员函数未定义

```bash
../../contrib/satellite-network/model/topology-satellite-network.cc: In member function ‘void ns3::TopologySatelliteNetwork::ReadSatellites()’:
../../contrib/satellite-network/model/topology-satellite-network.cc:158:25: error: ‘SatellitePositionHelperValue’ was not declared in this scope
                         SatellitePositionHelperValue(SatellitePositionHelper(satellite))
                         ^~~~~~~~~~~~~~~~~~~~~~~~~~~~
../../contrib/satellite-network/model/topology-satellite-network.cc:158:25: note: suggested alternative: ‘SatellitePositionHelper’
                         SatellitePositionHelperValue(SatellitePositionHelper(satellite))
                         ^~~~~~~~~~~~~~~~~~~~~~~~~~~~
                         SatellitePositionHelper

```



# 预备更新

## `exp-util`部分

这个部分主要是一些通用操作, 并被整个项目中模组频繁调用. 具有以下小毛病:
- [ ] 里面文件全部集中在一个`.cc`中, 文件太长.
- 有些函数写的不大泛化.



## 文件操作部分改进


- [x] 由于文件系统具有较强的操作系统相关性, basic-sim项目没有使用boost库而是通过std库中进行文件相关处理.
由于本项目文件相关操作比较复杂, 而且涉及文件操作频率很大. 为了更好的处理, 这里采用第三方wjakob的[filesystem](https://github.com/wjakob/filesystem)进行补充, 减少文件操作的复杂以及相关代码的冗长. 其通过仅头文件方式集成 .


## 参数读取部分

- [ ] 参数读取这里有很多冗余, 典型的就是读取任意都需要assert一下, 使得代码冗余过多,如下

```cpp
    m_num_nodes = parse_positive_int64(get_param_or_fail("num_nodes", m_topology_config));
    m_num_undirected_edges = parse_positive_int64(get_param_or_fail("num_undirected_edges", m_topology_config));

    // Node types
    std::string tmp;
    tmp = get_param_or_fail("switches", m_topology_config);
    m_switches = parse_set_positive_int64(tmp);
    all_items_are_less_than(m_switches, m_num_nodes);
    tmp = get_param_or_fail("switches_which_are_tors", m_topology_config);
    m_switches_which_are_tors = parse_set_positive_int64(tmp);
    all_items_are_less_than(m_switches_which_are_tors, m_num_nodes);
    tmp = get_param_or_fail("servers", m_topology_config);
    m_servers = parse_set_positive_int64(tmp);
    all_items_are_less_than(m_servers, m_num_nodes);
```

如果在载入`.properties`文件之前已经统一assert, 后面就可以如下, 代码明显少很多.

```cpp
assert(m_topology_config);
m_num_nodes = m_topology_config["num_nodes"];
m_num_undirected_edges = m_topology_config["num_undirected_edges"];
...

```


- [x] 把keys弄成静态全局变量, 放到了`exp-util.h`里面了, 但是暂时弄的是`std::vector<std::string> xx_keys`,后面考虑换成`set`?



- [x] 通过宏减少一些麻烦?





/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */





#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include "ns3/netanim-module.h"
// #include <boost/filesystem.hpp>

// Default Network Topology
//
//       10.1.1.0
// n0 -------------- n1
//    point-to-point
//
 
using namespace ns3;
using namespace std;
NS_LOG_COMPONENT_DEFINE ("FirstScriptExample");//定义日志组件

int
main (int argc, char *argv[])
{

    // boost::filesystem::path path = "/usr/share/cmake/modules";
    // if(path.is_relative())
    // {
    //     std::cout << "Path is relative" << std::endl;
    // }
    // else
    // {
    //     std::cout << "Path is not relative" << std::endl;
    // }

  cout<<"first example"<<endl;
  CommandLine cmd (__FILE__);// /home/roit/..../myfirst.cc
  cmd.Parse (argc, argv);
  
  Time::SetResolution (Time::NS);
  LogComponentEnable ("UdpEchoClientApplication", LOG_LEVEL_INFO);
  LogComponentEnable ("UdpEchoServerApplication", LOG_LEVEL_INFO);


//1. nodes 构建网络拓扑

  //构建两个拓扑节点
  NodeContainer nodes;
  nodes.Create (2);

  
//2. channel & devices 
  //2.1 创建信道
  PointToPointHelper pointToPoint;
  //配置信道属性
  pointToPoint.SetDeviceAttribute ("DataRate", StringValue ("5Mbps"));//传播速率定义
  pointToPoint.SetChannelAttribute ("Delay", StringValue ("2ms"));//传播延时设定

  //2.2 channle in stall nodes, get devices
  NetDeviceContainer devices;//创建设备
  devices = pointToPoint.Install (nodes);//将设备装载到信道中



//3. protocol stack安装协议族
  InternetStackHelper stack;
  stack.Install (nodes);//为所有node节点安装协议栈

//4. address
  //4.1 ip define
  Ipv4AddressHelper address;//地址分配助手
  address.SetBase ("10.1.1.0", "255.255.255.0");//起始地址 和 子网掩码
  //4.2 ip distribution
  //interface 将一个设备和一个地址关联起来
  Ipv4InterfaceContainer interfaces = address.Assign (devices);//两个设备分别被分给了10.1.1.1 以及10.1.1.2,不用挨个操作


//5. application 应用层安装
  //5.1 nodes1 app
    //5.1.1
    //在节点1中创建了一个服务器端回显服务应用echoSever。
  UdpEchoServerHelper echoServer (19);//服务端口号设置
    //5.1.2
    //节点1中安装 服务端 应用程序
  ApplicationContainer serverApps = echoServer.Install (nodes.Get (1));
  serverApps.Start (Seconds (1.0));
  serverApps.Stop (Seconds (10.0));
  //echoSever在模拟自启动后1.0s时开始监听并接收9号端口的数据在接收到数据包后向echoClient返回一个相同大小的UDP数据包，并在10.0s停止。


  //5.2 nodes 2 app
    //5.2.1创建客户端
  cout<<interfaces.GetAddress (1)<<endl;//get 10.1.1.2
  UdpEchoClientHelper echoClient (interfaces.GetAddress (1), 19);
    //5.2.2配置客户端属性
  echoClient.SetAttribute ("MaxPackets", UintegerValue (1));
  echoClient.SetAttribute ("Interval", TimeValue (Seconds (1.0)));
  echoClient.SetAttribute ("PacketSize", UintegerValue (1024));

  //客户端 安装 到节点0
  ApplicationContainer clientApps = echoClient.Install (nodes.Get (0));
  clientApps.Start (Seconds (2.0));
  clientApps.Stop (Seconds (10.0));

  
  //visualization
  AnimationInterface anim("./build/ns3ws/src/myfirst/myfirst.xml");
  anim.SetConstantPosition(nodes.Get(0),1.0,2.0);
  anim.SetConstantPosition(nodes.Get(1),20.0,20.0);




  //6.run
  Simulator::Run ();
  //7.del
  Simulator::Destroy ();
  return 0;
}

//这里可以看看容器的概念,本次多次用到XXcontainer, 其实就是一次性安置多个类,但又比较数组等具有一些一般性以及其他属性.

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
  cout<<"git src"<<endl;
  CommandLine cmd (__FILE__);// /home/roit/..../myfirst.cc
  cmd.Parse (argc, argv);
  
  Time::SetResolution (Time::NS);
  LogComponentEnable ("UdpEchoClientApplication", LOG_LEVEL_INFO);
  LogComponentEnable ("UdpEchoServerApplication", LOG_LEVEL_INFO);


//构建网络拓扑

  //构建两个拓扑节点
  NodeContainer nodes;
  nodes.Create (2);

  
  //创建信道
  PointToPointHelper pointToPoint;
  //配置信道属性
  pointToPoint.SetDeviceAttribute ("DataRate", StringValue ("5Mbps"));//传播速率定义
  pointToPoint.SetChannelAttribute ("Delay", StringValue ("2ms"));//传播延时设定


  NetDeviceContainer devices;//创建设备
  devices = pointToPoint.Install (nodes);//将设备装载到信道中



//安装协议族
  InternetStackHelper stack;
  stack.Install (nodes);//为所有node节点安装协议栈


  Ipv4AddressHelper address;//地址分配助手
  address.SetBase ("10.1.1.0", "255.255.255.0");//起始地址 和 子网掩码

  Ipv4InterfaceContainer interfaces = address.Assign (devices);//两个设备分别被分给了10.1.1.1 以及10.1.1.2
  //interface 将一个设备和一个地址关联起来

//应用层安装
  
  //在节点1中创建了一个服务器端回显服务应用echoSever。
  UdpEchoServerHelper echoServer (9);//服务端口号设置
  //节点1中安装 服务端 应用程序
  ApplicationContainer serverApps = echoServer.Install (nodes.Get (1));
  serverApps.Start (Seconds (1.0));
  serverApps.Stop (Seconds (10.0));
  //echoSever在模拟自启动后1.0s时开始监听并接收9号端口的数据在接收到数据包后向echoClient返回一个相同大小的UDP数据包，并在10.0s停止。





  //配置客户端属性
  UdpEchoClientHelper echoClient (interfaces.GetAddress (1), 9);
  echoClient.SetAttribute ("MaxPackets", UintegerValue (1));
  echoClient.SetAttribute ("Interval", TimeValue (Seconds (1.0)));
  echoClient.SetAttribute ("PacketSize", UintegerValue (1024));


  //节点0中安装客户端属性
  ApplicationContainer clientApps = echoClient.Install (nodes.Get (0));
  clientApps.Start (Seconds (2.0));
  clientApps.Stop (Seconds (10.0));

  
  Simulator::Run ();
  Simulator::Destroy ();
  return 0;
}

//这里可以看看容器的概念,本次多次用到XXcontainer, 其实就是一次性安置多个类,但又比较数组等具有一些一般性以及其他属性.


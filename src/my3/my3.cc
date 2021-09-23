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
#include "ns3/point-to-point-module.h"
#include "ns3/network-module.h"
#include "ns3/applications-module.h"
#include "ns3/mobility-module.h"
#include "ns3/csma-module.h"
#include "ns3/internet-module.h"
#include "ns3/yans-wifi-helper.h"
#include "ns3/ssid.h"
#include "ns3/netanim-module.h"


// Default Network Topology
//
//   Wifi 10.1.3.0
//                 AP
//  *    *    *    *
//  |    |    |    |    10.1.1.0
// n5   n6   n7   n0 -------------- n1   n2   n3   n4
//                   point-to-point  |    |    |    |
//                                   ================ //csma channel
//                                     LAN 10.1.2.0

using namespace ns3;
using namespace std;
NS_LOG_COMPONENT_DEFINE ("ThirdScriptExample");

int 
main (int argc, char *argv[])
{



//0. 拓扑准备
  cout<<"-> at my third"<<endl;
  bool verbose = true;
  uint32_t nCsma = 3;
  uint32_t nWifi = 3;
  bool tracing = true;

  CommandLine cmd (__FILE__);
  cmd.AddValue ("nCsma", "Number of \"extra\" CSMA nodes/devices", nCsma);
  cmd.AddValue ("nWifi", "Number of wifi STA devices", nWifi);
  cmd.AddValue ("verbose", "Tell echo applications to log if true", verbose);
  cmd.AddValue ("tracing", "Enable pcap tracing", tracing);

  cmd.Parse (argc,argv);
  // The underlying restriction of 18 is due to the grid position
  // allocator's configuration; the grid layout will exceed the
  // bounding box if more than 18 nodes are provided.
  if (nWifi > 18)
    {
      std::cout << "nWifi should be 18 or less; otherwise grid layout exceeds the bounding box" << std::endl;
      return 1;
    }

  if (verbose)
    {
      LogComponentEnable ("UdpEchoClientApplication", LOG_LEVEL_INFO);
      LogComponentEnable ("UdpEchoServerApplication", LOG_LEVEL_INFO);
    }

//1.p2p topology
  //1.1两个点对点网络拓扑节点
  NodeContainer p2pNodes;
  p2pNodes.Create (2);

  //1.2信道设置
  PointToPointHelper pointToPoint;//link-layer ppp protocol
  pointToPoint.SetDeviceAttribute ("DataRate", StringValue ("5Mbps"));
  pointToPoint.SetChannelAttribute ("Delay", StringValue ("2ms"));

  //1.2 信道安装节点,get devices
  NetDeviceContainer p2pDevices;//创建设备
  p2pDevices = pointToPoint.Install (p2pNodes);//设备装载到信道两端


//2. csma topology
  //2.1 csma node
  NodeContainer csmaNodes;
  csmaNodes.Add (p2pNodes.Get (1));//get 访问n1号节点并装载?
  csmaNodes.Create (nCsma);

  //2.2 csma channel 设置CSMA信道属性
  CsmaHelper csma; //link-layer csma protocol
  csma.SetChannelAttribute ("DataRate", StringValue ("100Mbps"));
  csma.SetChannelAttribute ("Delay", TimeValue (NanoSeconds (6560)));

  //2.3 channel install nodes, get devices 设备节点
  NetDeviceContainer csmaDevices;
  csmaDevices = csma.Install (csmaNodes);


//----------------------
//3 wifi topology
//----------------------


  //3.1 wifi sta nodes and ap nodes
  NodeContainer wifiStaNodes;
  wifiStaNodes.Create (nWifi);//建立站设备数量
  NodeContainer wifiApNode = p2pNodes.Get (0);//



  WifiHelper wifi;
  wifi.SetRemoteStationManager ("ns3::AarfWifiManager");//告诉助手类使用那种速率控制算法,这里为aarf


  //3.3 channel setting

    //(1) physical layer setting
  YansWifiChannelHelper channel = YansWifiChannelHelper::Default ();
  YansWifiPhyHelper phy;
  phy.SetChannel (channel.Create ());//使用默认的物理层配置和信道模型


 
    //(2) link-layer setting
  Ssid ssid = Ssid ("ns-3-ssid");//创建 IEEE 802.11 服务集标识符 对象,用来设置MAC层的SSID属性值

    //stas
  WifiMacHelper mac;//mac config
  mac.SetType ("ns3::StaWifiMac",
               "Ssid", SsidValue (ssid),
               "ActiveProbing", BooleanValue (false));
  NetDeviceContainer staDevices;
  staDevices = wifi.Install (phy, mac, wifiStaNodes);



    //aps
  mac.SetType ("ns3::ApWifiMac",
               "Ssid", SsidValue (ssid));
  NetDeviceContainer apDevices;
  apDevices = wifi.Install (phy, mac, wifiApNode);






  //3.7 wifi 移动性构建(卫星网络轨道运行)
  MobilityHelper mobility;
  mobility.SetPositionAllocator ("ns3::GridPositionAllocator",
                                 "MinX", DoubleValue (0.0),
                                 "MinY", DoubleValue (0.0),
                                 "DeltaX", DoubleValue (5.0),
                                 "DeltaY", DoubleValue (10.0),
                                 "GridWidth", UintegerValue (3),
                                 "LayoutType", StringValue ("RowFirst"));

  
  mobility.SetMobilityModel ("ns3::RandomWalk2dMobilityModel",
                             "Bounds", RectangleValue (Rectangle (-50, 50, -50, 50)));
  mobility.Install (wifiStaNodes);

  mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
  mobility.Install (wifiApNode);
  std::cout<<"test2"<<std::endl;


//--------------
//4 协议安装
//----------------

  InternetStackHelper stack;
  stack.Install (csmaNodes);
  stack.Install (wifiApNode);
  stack.Install (wifiStaNodes);

//-------------
//5. address assign to devices, get interfaces
//----------------
  Ipv4AddressHelper address;


  address.SetBase ("10.1.1.0", "255.255.255.0");//x.x.x.0地址不会分配哦,特殊地址
  Ipv4InterfaceContainer p2pInterfaces;
  p2pInterfaces = address.Assign (p2pDevices);


  address.SetBase ("10.1.2.0", "255.255.255.0");
  Ipv4InterfaceContainer csmaInterfaces;
  csmaInterfaces = address.Assign (csmaDevices);


  address.SetBase ("10.1.3.0", "255.255.255.0");
  // Ipv4InterfaceContainer  interface not in
  address.Assign (staDevices);//.1, .2, .3 --> n5 n6 n7 
  address.Assign (apDevices);//.4 --> n0


//---------------
// applications install
//----------------

  //服务端init
  UdpEchoServerHelper echoServer (9);// 监听来自9号端口的? the port the server will wait on for incoming packets


  //服务端 start
  ApplicationContainer serverApps = echoServer.Install (csmaNodes.Get (nCsma));//把csma 信道上的最后一个节点,即n4装上echoSever,
  serverApps.Start (Seconds (1.0));
  serverApps.Stop (Seconds (10.0));

  //客户端init
  Ipv4Address addresses = csmaInterfaces.GetAddress (nCsma);//csma 链路中最后一个结点n4, 为发送到的目的方, 得到其IP地址,
  UdpEchoClientHelper echoClient (addresses, 9); //发往 这些 ip, 通过9号口发
  echoClient.SetAttribute ("MaxPackets", UintegerValue (1));
  echoClient.SetAttribute ("Interval", TimeValue (Seconds (1.0)));
  echoClient.SetAttribute ("PacketSize", UintegerValue (2024));

  //n7 sta 装上客户端
  ApplicationContainer clientApps = echoClient.Install (wifiStaNodes.Get (nWifi - 1));//n7
  //客户端start
  clientApps.Start (Seconds (2.0));
  clientApps.Stop (Seconds (10.0));


//8. 路由设置
  Ipv4GlobalRoutingHelper::PopulateRoutingTables ();

  Simulator::Stop (Seconds (10.0));

  if (tracing == true)
    {
      pointToPoint.EnablePcapAll ("./my3-p2p");
      phy.EnablePcap ("./my3-phy", apDevices.Get (0));
      csma.EnablePcap ("./my3-csma", csmaDevices.Get (0), true);
    }



  AnimationInterface anim("./build/ns3ws/src/my3/my3.xml");
  anim.SetConstantPosition(csmaNodes.Get(0),10.0,10.0);
  anim.SetConstantPosition(csmaNodes.Get(1),10.0,20.0);
  anim.SetConstantPosition(csmaNodes.Get(2),20.0,20.0);
  anim.SetConstantPosition(csmaNodes.Get(3),30.0,20.0);



// start toplogic
  Simulator::Run ();
  Simulator::Destroy ();
  return 0;
}
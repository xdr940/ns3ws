#include <ns3/command-line.h>
// #include "ns3/basic-simulation.h"
// #include "ns3/topology-ptop.h"



#include "ns3/ipv4-arbiter-routing-helper.h"
#include "ns3/arbiter-ecmp-helper.h"
#include "ns3/tcp-config-helper.h"


#include "ns3/ptop-link-net-device-utilization-tracking.h"//PtopLinkNetDeviceUtilizationTracking
#include "ns3/ptop-link-net-device-queue-tracking.h"
#include "ns3/ptop-link-interface-tc-qdisc-queue-tracking.h"

#include "ns3/tcp-flow-scheduler.h"//TcpFlowScheduler,tcp
#include "ns3/udp-burst-scheduler.h"
#include "ns3/udp-ping-scheduler.h"

using namespace ns3;

int main(int argc, char *argv[]) {

    // No buffering of printf
    setbuf(stdout, nullptr);

    // Retrieve run directory
    CommandLine cmd;
    std::string run_dir = "";
    cmd.Usage("Usage: ./waf --run=\"my-main --run_dir='<path/to/run/directory>'\"");
    cmd.AddValue("run_dir",  "Run directory", run_dir);
    cmd.Parse(argc, argv);
    if (run_dir.compare("") == 0) {
        printf("Usage: ./waf --run=\"my-main --run_dir='<path/to/run/directory>'\"");
        return 1;
    }

    // Load basic simulation environment
    Ptr<BasicSimulation> basicSimulation = CreateObject<BasicSimulation>(run_dir);

    // Read point-to-point topology, and install routing arbiters
    Ptr<TopologyPtop> topology = CreateObject<TopologyPtop>(basicSimulation, Ipv4ArbiterRoutingHelper());
    




    ArbiterEcmpHelper::InstallArbiters(basicSimulation, topology);




// trackers

    // Install link net-device utilization trackers
    PtopLinkNetDeviceUtilizationTracking 
        netDeviceUtilizationTracking = PtopLinkNetDeviceUtilizationTracking(basicSimulation, topology); 
    // Requires enable_link_net_device_utilization_tracking=true

    // Install link net-device queue trackers
    PtopLinkNetDeviceQueueTracking netDeviceQueueTracking = PtopLinkNetDeviceQueueTracking(basicSimulation, topology); 
    // Requires enable_link_net_device_queue_tracking=true

    // Install link interface traffic-control qdisc queue trackers
    PtopLinkInterfaceTcQdiscQueueTracking tcQdiscQueueTracking = PtopLinkInterfaceTcQdiscQueueTracking(basicSimulation, topology); 
    // Requires enable_link_interface_tc_qdisc_queue_tracking=true

    // Configure TCP
    TcpConfigHelper::Configure(basicSimulation);



// schedule

    // Schedule TCP flows
    TcpFlowScheduler tcpFlowScheduler(basicSimulation, topology); // Requires enable_tcp_flow_scheduler=true

    // Schedule UDP bursts
    UdpBurstScheduler udpBurstScheduler(basicSimulation, topology); // Requires enable_udp_burst_scheduler=true

    // Schedule UDP pings
    UdpPingScheduler udpPingScheduler(basicSimulation, topology); // Requires enable_udp_ping_scheduler=true



//run
    // Run simulation
    basicSimulation->Run();



// output results

    // Write TCP flow results
    tcpFlowScheduler.WriteResults();

    // Write UDP burst results
    udpBurstScheduler.WriteResults();

    // Write UDP ping results
    udpPingScheduler.WriteResults();

    // Write link net-device utilization results
    netDeviceUtilizationTracking.WriteResults();

    // Write link net-device queue results
    netDeviceQueueTracking.WriteResults();

    // Write link interface traffic-control qdisc queue results
    tcQdiscQueueTracking.WriteResults();


// end the sim 
    // Finalize the simulation
    basicSimulation->Finalize();

    return 0;

}
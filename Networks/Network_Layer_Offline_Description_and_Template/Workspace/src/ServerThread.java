

import sun.rmi.runtime.NewThreadAction;

import java.util.ArrayList;
import java.util.Random;

public class ServerThread implements Runnable {

    NetworkUtility networkUtility;
    EndDevice endDevice;

    ArrayList<Router> routing_path;

    ServerThread(NetworkUtility networkUtility, EndDevice endDevice) {
        this.networkUtility = networkUtility;
        this.endDevice = endDevice;
        System.out.println("Server Ready for client " + NetworkLayerServer.clientCount);
        NetworkLayerServer.clientCount++;
        routing_path = new ArrayList<>();
        new Thread(this).start();
    }

    @Override
    public void run() {
        /**
         * Synchronize actions with client.
         */
        /*
        Tasks:
        1. Upon receiving a packet and recipient, call deliverPacket(packet)
        2. If the packet contains "SHOW_ROUTE" request, then fetch the required information
                and send back to client
        3. Either send acknowledgement with number of hops or send failure message back to client
        */

        networkUtility.write(endDevice); // Sending endDevice to corresponding client
        networkUtility.write(NetworkLayerServer.endDevices);

        while(true) {
            Packet packet = (Packet) networkUtility.read();
            System.out.println(packet.getMessage() + "," + packet.getSpecialMessage());
            if(packet.getMessage().equals("exit")) break;
            boolean sent = deliverPacket(packet);
            if (packet.getSpecialMessage().equals("SHOW_ROUTE")) {
                System.out.println("show route");
                Packet status;
                if(sent){
                    System.out.println("successful");
                    status = new Packet("Packet sent successfully", "", packet.getDestinationIP(), packet.getSourceIP());
//                    send_to_client.setRouting_path(routing_path);
                    networkUtility.write(status);
                    networkUtility.write(routing_path);
                }else{
                    System.out.println("dropped");
                    status = new Packet("Packet Dropped", "", packet.getDestinationIP(), packet.getSourceIP());
//                    send_to_client.setRouting_path(routing_path);
                    networkUtility.write(status);
                }

            }else {
                String acknowledgement_msg;
                if (sent) {
                    acknowledgement_msg = "Packet sent successfully";
                } else acknowledgement_msg = "Packet Dropped";
                Packet send_to_client = new Packet(acknowledgement_msg, "", packet.getDestinationIP(), packet.getSourceIP());
                networkUtility.write(send_to_client);
            }
        }

    }

    public String get_last_24_bits(IPAddress ipAddress){
        String [] temp = ipAddress.getString().split("\\.");
        return temp[0] + temp[1] + temp[2];
    }


    public Boolean deliverPacket(Packet p) {

        
        /*
        1. Find the router s which has an interface
                such that the interface and source end device have same network address.
        2. Find the router d which has an interface
                such that the interface and destination end device have same network address.
        3. Implement forwarding, i.e., s forwards to its gateway router x considering d as the destination.
                similarly, x forwards to the next gateway router y considering d as the destination,
                and eventually the packet reaches to destination router d.

            3(a) If, while forwarding, any gateway x, found from routingTable of router r is in down state[x.state==FALSE]
                    (i) Drop packet
                    (ii) Update the entry with distance Constants.INFTY
                    (iii) Block NetworkLayerServer.stateChanger.t
                    (iv) Apply DVR starting from router r.
                    (v) Resume NetworkLayerServer.stateChanger.t

            3(b) If, while forwarding, a router x receives the packet from router y,
                    but routingTableEntry shows Constants.INFTY distance from x to y,
                    (i) Update the entry with distance 1
                    (ii) Block NetworkLayerServer.stateChanger.t
                    (iii) Apply DVR starting from router x.
                    (iv) Resume NetworkLayerServer.stateChanger.t

        4. If 3(a) occurs at any stage, packet will be dropped,
            otherwise successfully sent to the destination router
        */
        Router s = new Router();
        Router d = new Router();
        /***************        1         ****************/
        for(Router r : NetworkLayerServer.routers){
            IPAddress router_interface_ip = r.getInterfaceAddresses().get(0);
            String last_24_bits = get_last_24_bits(router_interface_ip);
            if(last_24_bits.equals(get_last_24_bits(p.getSourceIP()))){
                s = r;
                break;
            }
        }

        /***************        2         ****************/
        for(Router r : NetworkLayerServer.routers){
            IPAddress router_interface_ip = r.getInterfaceAddresses().get(0);
            String last_24_bits = get_last_24_bits(router_interface_ip);
            if(last_24_bits.equals(get_last_24_bits(p.getDestinationIP()))){
                d = r;
                break;
            }
        }
        System.out.println("Router " + s.getRouterId() + " to " + d.getRouterId());
        if(!s.getState()){
            System.out.println("source router " + s.getRouterId() + " is down.");
            return false;
        }
        Router current_router = s;
        routing_path.clear();
        routing_path.add(current_router);
//        routing_path_ips = current_router.getInterfaceAddresses().get(0).getString();
//        for(Router r : NetworkLayerServer.routers){
//            System.out.println(r.getRouterId() + " is " + r.getState());
//        }
        boolean drop = false;
        while(current_router.getRouterId() != d.getRouterId()){
            ArrayList<RoutingTableEntry> routing_table = current_router.getRoutingTable();

            /*********************     3(a)      **********************/

            drop = false;
            for(RoutingTableEntry rte : routing_table){
                if(rte.getRouterId() == d.getRouterId()){
                    Router gateway = NetworkLayerServer.routerMap.get(rte.getGatewayRouterId());
                    System.out.println(current_router.getRouterId() + " -> " + gateway.getRouterId());

                    if(!gateway.getState() || (gateway.getRouterId() == current_router.getRouterId())){
                        System.out.println("Packet dropped");
                        rte.setDistance(Constants.INFINITY); // check whether its really it.
//                        RouterStateChanger.islocked = true;
                        NetworkLayerServer.DVR(current_router.getRouterId());
//                        RouterStateChanger.islocked = false;
                        drop = true;
                    }else {
                        current_router = gateway;
                        routing_path.add(current_router);
//                        routing_path_ips += "," + current_router.getInterfaceAddresses().get(0).getString();
                        /*********************     3(b)      **********************/
                        ArrayList<RoutingTableEntry> gateway_routing_table =  gateway.getRoutingTable();
                        for(RoutingTableEntry rte_g : gateway_routing_table){
                            if(rte_g.getRouterId() == current_router.getRouterId()){
                                if(rte_g.getDistance() == Constants.INFINITY){
                                    rte_g.setDistance(1);
//                                    RouterStateChanger.islocked = true;
                                    NetworkLayerServer.DVR(gateway.getRouterId());
//                                    RouterStateChanger.islocked = false;
                                }
                                break;
                            }
                        }
                    }
                    break;
                }
            }
            if(drop) break;
            System.out.println("visiting " + current_router.getRouterId());
//            if(lim == 0) break;
        }

        return !drop;
    }

    @Override
    public boolean equals(Object obj) {
        return super.equals(obj); //To change body of generated methods, choose Tools | Templates.
    }
}

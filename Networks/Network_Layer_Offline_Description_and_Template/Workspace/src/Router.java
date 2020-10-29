//Work needed
import com.sun.xml.internal.bind.v2.runtime.reflect.opt.Const;

import java.util.*;

public class Router {
    private int routerId;
    private int numberOfInterfaces;
    private ArrayList<IPAddress> interfaceAddresses;//list of IP address of all interfaces of the router
    private ArrayList<RoutingTableEntry> routingTable;//used to implement DVR
    private ArrayList<Integer> neighborRouterIDs;//Contains both "UP" and "DOWN" state routers
    private Boolean state;//true represents "UP" state and false is for "DOWN" state
    private Map<Integer, IPAddress> gatewayIDtoIP;
    public Router() {
        interfaceAddresses = new ArrayList<>();
        routingTable = new ArrayList<>();
        neighborRouterIDs = new ArrayList<>();

        /**
         * 80% Probability that the router is up
         */
        Random random = new Random();
        double p = random.nextDouble();
        if(p < 0.80) state = true;
        else state = false;

        numberOfInterfaces = 0;
    }

    public Router(int routerId, ArrayList<Integer> neighborRouters, ArrayList<IPAddress> interfaceAddresses, Map<Integer, IPAddress> gatewayIDtoIP) {
        this.routerId = routerId;
        this.interfaceAddresses = interfaceAddresses;
        this.neighborRouterIDs = neighborRouters;
        this.gatewayIDtoIP = gatewayIDtoIP;
        routingTable = new ArrayList<>();



        /**
         * 80% Probability that the router is up
         */
        Random random = new Random();
        double p = random.nextDouble();
        if(p < 0.80) state = true;
        else state = false;

        numberOfInterfaces = interfaceAddresses.size();
    }

    @Override
    public String toString() {
        String string = "";
        string += "Router ID: " + routerId + "\n" + "Interfaces: \n";
        for (int i = 0; i < numberOfInterfaces; i++) {
            string += interfaceAddresses.get(i).getString() + "\t";
        }
        string += "\n" + "Neighbors: \n";
        for(int i = 0; i < neighborRouterIDs.size(); i++) {
            string += neighborRouterIDs.get(i) + "\t";
        }
        return string;
    }



    /**
     * Initialize the distance(hop count) for each router.
     * for itself, distance=0; for any connected router with state=true, distance=1; otherwise distance=Constants.INFTY;
     */
    public void initiateRoutingTable() {
        int router_cnt = NetworkLayerServer.routers.size();

        // marks the neighbor router which is in UP state
        ArrayList<Integer> mark = new ArrayList<>(Collections.nCopies(router_cnt + 1, 0));

//        Debug.printv(mark);

        for(Integer i : neighborRouterIDs){
            Router r = NetworkLayerServer.routerMap.get(i);
            if(r.state) mark.set(i, 1);
        }

        for(Router r : NetworkLayerServer.routers){
            RoutingTableEntry routingTableEntry = new RoutingTableEntry(r.getRouterId(), Constants.INFINITY, routerId);
            if(mark.get(r.routerId) == 1){
                routingTableEntry.setDistance(1);
                routingTableEntry.setGatewayRouterId(r.routerId);
            }
            if(r.routerId == routerId){
                routingTableEntry.setDistance(0);
            }
            routingTable.add(routingTableEntry);
        }

//        if(routerId == 1) {
//            Debug.printv(mark);
//            for (RoutingTableEntry r : routingTable) {
//                System.out.println(r.getRouterId() + " " + r.getDistance() + " " + r.getGatewayRouterId());
//            }
//        }
    }

    /**
     * Delete all the routingTableEntry
     */
    public void clearRoutingTable() {
        routingTable.clear();
    }

    /**
     * Update the routing table for this router using the entries of Router neighbor
     * @param neighbor
     */
    public boolean updateRoutingTable(Router neighbor) {
        int router_cnt = NetworkLayerServer.routers.size();
        // dist[i] is the distance of the router i from neighbor router
        ArrayList<Double> dist = new ArrayList<Double>(Collections.nCopies(router_cnt + 1, 0.0));
        for(RoutingTableEntry rte : neighbor.routingTable){
            dist.set(rte.getRouterId(), rte.getDistance());
        }
        for(RoutingTableEntry rte : routingTable){
            int to = rte.getRouterId();
            if(dist.get(to) < rte.getDistance()){
                rte.setDistance(1.0 + dist.get(to));
                rte.setGatewayRouterId(neighbor.getRouterId());
            }
        }
        return false;
    }

    public boolean sfupdateRoutingTable(Router neighbor) {
        int router_cnt = NetworkLayerServer.routers.size();
        // dist[i] is the distance of the router i from neighbor router
        ArrayList<Double> dist = new ArrayList<Double>(Collections.nCopies(router_cnt + 1, 0.0));
        for(RoutingTableEntry rte : neighbor.routingTable){
            dist.set(rte.getRouterId(), rte.getDistance());
        }
        for(RoutingTableEntry rte : routingTable){
            int to = rte.getRouterId();
            int next_hop = rte.getGatewayRouterId();
            int from = routerId;
            if(next_hop == neighbor.getRouterId() || (dist.get(to) < rte.getDistance() && next_hop != from)){
                rte.setDistance(1.0 + dist.get(to));
                rte.setGatewayRouterId(neighbor.getRouterId());
            }
        }
        return false;
    }

    /**
     * If the state was up, down it; if state was down, up it
     */
    public void revertState() {
        state = !state;
        if(state) { initiateRoutingTable(); }
        else { clearRoutingTable(); }
    }

    public int getRouterId() {
        return routerId;
    }

    public void setRouterId(int routerId) {
        this.routerId = routerId;
    }

    public int getNumberOfInterfaces() {
        return numberOfInterfaces;
    }

    public void setNumberOfInterfaces(int numberOfInterfaces) {
        this.numberOfInterfaces = numberOfInterfaces;
    }

    public ArrayList<IPAddress> getInterfaceAddresses() {
        return interfaceAddresses;
    }

    public void setInterfaceAddresses(ArrayList<IPAddress> interfaceAddresses) {
        this.interfaceAddresses = interfaceAddresses;
        numberOfInterfaces = interfaceAddresses.size();
    }

    public ArrayList<RoutingTableEntry> getRoutingTable() {
        return routingTable;
    }

    public void addRoutingTableEntry(RoutingTableEntry entry) {
        this.routingTable.add(entry);
    }

    public ArrayList<Integer> getNeighborRouterIDs() {
        return neighborRouterIDs;
    }

    public void setNeighborRouterIDs(ArrayList<Integer> neighborRouterIDs) { this.neighborRouterIDs = neighborRouterIDs; }

    public Boolean getState() {
        return state;
    }

    public void setState(Boolean state) {
        this.state = state;
    }

    public Map<Integer, IPAddress> getGatewayIDtoIP() { return gatewayIDtoIP; }

    public void printRoutingTable() {
        System.out.println("Router " + routerId);
        System.out.println("DestID Distance Nexthop");
        for (RoutingTableEntry routingTableEntry : routingTable) {
            System.out.println(routingTableEntry.getRouterId() + " " + routingTableEntry.getDistance() + " " + routingTableEntry.getGatewayRouterId());
        }
        System.out.println("-----------------------");
    }
    public String strRoutingTable() {
        String string = "Router" + routerId + "\n";
        string += "DestID Distance Nexthop\n";
        for (RoutingTableEntry routingTableEntry : routingTable) {
            string += routingTableEntry.getRouterId() + " " + routingTableEntry.getDistance() + " " + routingTableEntry.getGatewayRouterId() + "\n";
        }

        string += "-----------------------\n";
        return string;
    }

}

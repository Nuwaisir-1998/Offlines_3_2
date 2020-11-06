import sun.nio.cs.ext.MacArabic;

import java.io.*;
import java.lang.reflect.Array;
import java.net.ServerSocket;
import java.net.Socket;
import java.util.*;
import java.util.logging.Level;
import java.util.logging.Logger;
import java.io.File;  // Import the File class
import java.io.IOException;  // Import the IOException class to handle errors
//Work needed
public class NetworkLayerServer {

    static int clientCount = 0;
    static ArrayList<Router> routers = new ArrayList<>();
    static RouterStateChanger stateChanger = null;
    static Map<IPAddress,Integer> clientInterfaces = new HashMap<>(); //Each map entry represents number of client end devices connected to the interface
    static Map<IPAddress, EndDevice> endDeviceMap = new HashMap<>();
    static ArrayList<EndDevice> endDevices = new ArrayList<>();
    static Map<Integer, Integer> deviceIDtoRouterID = new HashMap<>();
    static Map<IPAddress, Integer> interfacetoRouterID = new HashMap<>();
    static Map<Integer, Router> routerMap = new HashMap<>();

    public static void main(String[] args) throws IOException {

        //Task: Maintain an active client list

        ServerSocket serverSocket = null;
        try {
            serverSocket = new ServerSocket(4444);
        } catch (IOException ex) {
            Logger.getLogger(NetworkLayerServer.class.getName()).log(Level.SEVERE, null, ex);
        }

        System.out.println("Server Ready: " + serverSocket.getInetAddress().getHostAddress());
        System.out.println("Creating router topology");

        readTopology();
        printRouters();

        initRoutingTables(); //Initialize routing tables for all routers



//        for (Map.Entry<IPAddress, Integer> entry : interfacetoRouterID.entrySet()) {
//            IPAddress key = entry.getKey();
//            int value = entry.getValue();
//            System.out.println(key + " -> " + value);
//        }



        simpleDVR(7); //Update routing table using distance vector routing until convergence
//        DVR(7); //Update routing table using distance vector routing until convergence



        BufferedWriter myWriter = new BufferedWriter(new FileWriter("filename.txt", false));
        for(Router r : routers){
            myWriter.write(r.getRouterId() + " is " + r.getState() + "\n");
        }
        for(Router r : routers){
            myWriter.write("Router ID: " + r.getRouterId());
            myWriter.newLine();
            System.out.println("Dfsfs : " + r.getRouterId());
            for(RoutingTableEntry rte : r.getRoutingTable()){
                System.out.println(rte.getRouterId() + " " + rte.getDistance() + " " + rte.getGatewayRouterId());
                myWriter.write(rte.getRouterId() + " " + rte.getDistance() + " " + rte.getGatewayRouterId());
                myWriter.newLine();
            }
        }
        myWriter.close();



        for(Router r : routers){
            r.printRoutingTable();
        }
//        simpleDVR(1);
        // printing initial state of each router
        for(Router router : routers) System.out.println("Router " + router.getRouterId() + " is " + router.getState());
        stateChanger = new RouterStateChanger();//Starts a new thread which turns on/off routers randomly depending on parameter Constants.LAMBDA

        while(true) {
            try {

//                for(int i=0;i<15;i++){
//                    add_end_device();
//                }

                Socket socket = serverSocket.accept();
                System.out.println("Client" + (clientCount + 1) + " attempted to connect");
//                System.out.println("db: " + clientInterfaces.size());
                EndDevice endDevice = getClientDeviceSetup();
                clientCount++;
//                System.out.println("Client count : " + clientCount);
                endDevices.add(endDevice);
                System.out.println("end device ip : " + endDevice.getIpAddress());
                endDeviceMap.put(endDevice.getIpAddress(),endDevice);
                new ServerThread(new NetworkUtility(socket), endDevice);
            } catch (IOException ex) {
                Logger.getLogger(NetworkLayerServer.class.getName()).log(Level.SEVERE, null, ex);
            }
        }
    }

    public static void add_end_device(){
//        System.out.println("db2: " + clientInterfaces.size());
        EndDevice endDevice = getClientDeviceSetup();
        clientCount++;
//                System.out.println("Client count : " + clientCount);
        endDevices.add(endDevice);
        System.out.println("end device ip : " + endDevice.getIpAddress());
        endDeviceMap.put(endDevice.getIpAddress(),endDevice);
        clientCount++;
    }

    public static void initRoutingTables() {
        for (Router router : routers) {
            router.initiateRoutingTable();
        }
    }

    public static synchronized void DVR(int startingRouterId) {
        /**
         * pseudocode
         */

        /*
        while(convergence)
        {
            //convergence means no change in any routingTable before and after executing the following for loop
            for each router r <starting from the router with routerId = startingRouterId, in any order>
            {
                1. T <- getRoutingTable of the router r
                2. N <- find routers which are the active neighbors of the current router r
                3. Update routingTable of each router t in N using the
                   routing table of r [Hint: Use t.updateRoutingTable(r)]
            }
        }
        */


        int convergence = 0;

        // lock statechanger
        RouterStateChanger.islocked = true;

        // swapping startingRouterId th router with 0th router.
        while(true) {
            for (int i = 0; i < routers.size(); i++) {
                if (routers.get(i).getRouterId() == startingRouterId) {
                    Router temp = routers.get(0);
                    routers.set(0, routers.get(i));
                    routers.set(i, temp);
                    break;
                }
            }

            convergence = 0;
            for (Router r : routers) {
                if(!r.getState()) continue;
                ArrayList<RoutingTableEntry> T = r.getRoutingTable();
                ArrayList<Integer> all_neighbors = r.getNeighborRouterIDs();
                ArrayList<Integer> all_active_neighbors = new ArrayList<>();

                for (Integer i : all_neighbors) {
                    Router neighbor = routerMap.get(i);
                    if (neighbor.getState()) {
                        convergence += neighbor.sfupdateRoutingTable(r) ? 1 : 0;
                    }
                }
            }
            System.out.println("convergence: " + convergence);
            if(convergence == 0) break;
        }

        System.out.println("DVR ended");
        // unlock statechanger
        RouterStateChanger.islocked = false;
//        synchronized (RouterStateChanger.msg) {
//            RouterStateChanger.msg.notify();
//            System.out.println("unlocked : " + msg);
//        }
//        for(Router router : routers){
//            System.out.println("-------------------------");
//            router.printRoutingTable();
//        }

    }

    public static synchronized void simpleDVR(int startingRouterId) {

        int convergence = 0;


        // lock statechanger
        RouterStateChanger.islocked = true;

        // swapping startingRouterId th router with 0th router.
        while(true) {
            for (int i = 0; i < routers.size(); i++) {
                if (routers.get(i).getRouterId() == startingRouterId) {
                    Router temp = routers.get(0);
                    routers.set(0, routers.get(i));
                    routers.set(i, temp);
                    break;
                }
            }

            convergence = 0;
            for (Router r : routers) {
                if(!r.getState()) continue;
                ArrayList<RoutingTableEntry> T = r.getRoutingTable();
                ArrayList<Integer> all_neighbors = r.getNeighborRouterIDs();
                ArrayList<Integer> all_active_neighbors = new ArrayList<>();

                for (Integer i : all_neighbors) {
                    Router neighbor = routerMap.get(i);
                    if (neighbor.getState()) {
                        convergence += neighbor.updateRoutingTable(r) ? 1 : 0;
                    }
                }
            }
            System.out.println(convergence);
            if(convergence == 0) break;
        }

        System.out.println("simpleDVR ended");
        // unlock statechanger
        RouterStateChanger.islocked = false;
//        synchronized (RouterStateChanger.msg) {
//            RouterStateChanger.msg.notify();
//            System.out.println("unlocked : " + msg);
//        }
//        for(Router router : routers){
//            System.out.println("-------------------------");
//            router.printRoutingTable();
//        }

    }

    public static EndDevice getClientDeviceSetup() {
        Random random = new Random(System.currentTimeMillis());
        int r = Math.abs(random.nextInt(clientInterfaces.size()));

        System.out.println("Size: " + clientInterfaces.size() + "\n" + r);

        IPAddress ip = null;
        IPAddress gateway = null;

        int i = 0;
        for (Map.Entry<IPAddress, Integer> entry : clientInterfaces.entrySet()) {
            IPAddress key = entry.getKey();
            Integer value = entry.getValue();
            if(i == r) {
                gateway = key;
                ip = new IPAddress(gateway.getBytes()[0] + "." + gateway.getBytes()[1] + "." + gateway.getBytes()[2] + "." + (value+2));
                value++;
                clientInterfaces.put(key, value);
                deviceIDtoRouterID.put(endDevices.size(), interfacetoRouterID.get(key));
                break;
            }
            i++;
        }

        EndDevice device = new EndDevice(ip, gateway, endDevices.size());

        System.out.println("Device : " + ip + "::::" + gateway);
        return device;
    }

    public static void printRouters() {
        for(int i = 0; i < routers.size(); i++) {
            System.out.println("------------------\n" + routers.get(i));
        }
    }

    public static String strrouters() {
        String string = "";
        for (int i = 0; i < routers.size(); i++) {
            string += "\n------------------\n" + routers.get(i).strRoutingTable();
        }
        string += "\n\n";
        return string;
    }

    public static void readTopology() {
        Scanner inputFile = null;
        try {
            inputFile = new Scanner(new File("topology.txt"));
            //skip first 27 lines
            int skipLines = 27;
            for(int i = 0; i < skipLines; i++) {
                inputFile.nextLine();
            }

            //start reading contents
            while(inputFile.hasNext()) {
                inputFile.nextLine();
                int routerId;
                ArrayList<Integer> neighborRouters = new ArrayList<>();
                ArrayList<IPAddress> interfaceAddrs = new ArrayList<>();
                Map<Integer, IPAddress> interfaceIDtoIP = new HashMap<>();

                routerId = inputFile.nextInt();

                int count = inputFile.nextInt();
                for(int i = 0; i < count; i++) {
                    neighborRouters.add(inputFile.nextInt());
                }
                count = inputFile.nextInt();
                inputFile.nextLine();

                for(int i = 0; i < count; i++) {
                    String string = inputFile.nextLine();
                    IPAddress ipAddress = new IPAddress(string);
                    interfaceAddrs.add(ipAddress);
                    interfacetoRouterID.put(ipAddress, routerId);

                    /**
                     * First interface is always client interface
                     */
                    if(i == 0) {
                        //client interface is not connected to any end device yet
                        clientInterfaces.put(ipAddress, 0);
                    }
                    else {
                        interfaceIDtoIP.put(neighborRouters.get(i - 1), ipAddress);
                    }
                }
                Router router = new Router(routerId, neighborRouters, interfaceAddrs, interfaceIDtoIP);
                routers.add(router);
                routerMap.put(routerId, router);
            }


        } catch (FileNotFoundException ex) {
            Logger.getLogger(NetworkLayerServer.class.getName()).log(Level.SEVERE, null, ex);
        }
    }

}

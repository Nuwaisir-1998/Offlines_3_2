import javax.lang.model.type.ArrayType;
import java.lang.reflect.Array;
import java.util.ArrayList;
import java.util.Map;
import java.util.Random;
import java.util.Scanner;

//Work needed
public class Client {
    public static void main(String[] args) throws InterruptedException {
        NetworkUtility networkUtility = new NetworkUtility("127.0.0.1", 4444);
        System.out.println("Connected to server");
        /**
         * Tasks
         */
        
        /*
        1. Receive EndDevice configuration from server
        2. Receive active client list from server
        3. for(int i=0;i<100;i++)
        4. {
        5.      Generate a random message
        6.      Assign a random receiver from active client list
        7.      if(i==20)
        8.      {
        9.            Send the message and recipient IP address to server and a special request "SHOW_ROUTE"
        10.           Display routing path, hop count and routing table of each router [You need to receive
                            all the required info from the server in response to "SHOW_ROUTE" request]
        11.     }
        12.     else
        13.     {
        14.           Simply send the message and recipient IP address to server.
        15.     }
        16.     If server can successfully send the message, client will get an acknowledgement along with hop count
                    Otherwise, client will get a failure message [dropped packet]
        17. }
        18. Report average number of hops and drop rate
        */

        EndDevice endDevice = (EndDevice)networkUtility.read();
        System.out.println("My device is : " + endDevice.getDeviceID() + ", " + endDevice.getIpAddress() + ", " + endDevice.getGateway());
        ArrayList<EndDevice> endDevices = (ArrayList<EndDevice>) networkUtility.read();
        System.out.println("List of available end devices");
        int cnt = 0;
        for(EndDevice e : endDevices){
            System.out.println(cnt + ": " + e.getDeviceID() + ", " + e.getIpAddress() + ", " + e.getGateway());
            cnt++;
        }
//        System.out.println("To whom?");
//        Scanner sc = new Scanner(System.in);
//        int to = sc.nextInt();
        Random random = new Random(System.currentTimeMillis());

        int total_dropped = 0;
        int total_hop_count = 0;

        for(int i=0;i<100;i++){
            int rnd = Math.abs(random.nextInt(endDevices.size()));
            System.out.println("End device " + endDevice.getDeviceID() + " sending packet to " + endDevices.get(rnd).getDeviceID());
            Packet packet = new Packet("I am client " + endDevice.getDeviceID(), "", endDevice.getIpAddress(), endDevices.get(rnd).getIpAddress());
            if(i == 20){
//                System.out.println(i);
                packet.setSpecialMessage("SHOW_ROUTE");
//                System.out.println(packet.getMessage() + "," + packet.getSpecialMessage());
                networkUtility.write(packet);
                Packet status = (Packet)networkUtility.read();
//                System.out.println(from_server.getMessage());
                if(status.getMessage().equals("Packet Dropped")) {
                    System.out.println(" dropped ");
                    total_dropped++;

                }else{
                    ArrayList<Router> routing_path = (ArrayList<Router>) networkUtility.read();
                    if(routing_path == null) System.out.println("null routing path");
                    else {
                        int hop_count = routing_path.size() - 1;
                        total_hop_count += hop_count;
                        System.out.println("Hop count : " + hop_count);
                        System.out.println("Routing Path");
                        for (Router r : routing_path) {
                            System.out.print(r.getRouterId() + " ");
                        }
                        System.out.println();
                        for(Router r : routing_path){
                            r.printRoutingTable();
                        }
                    }
                }

            }else{
                networkUtility.write(packet);
                Packet from_server = (Packet)networkUtility.read();
                System.out.println(from_server.getMessage() + ", Hop count : " + from_server.getHopcount());
                if(from_server.getMessage().equals("Packet Dropped")){
                    total_dropped++;
                }else total_hop_count += from_server.getHopcount();
            }
        }
//        System.out.println("Turn me off? Any key: YES");
//        Scanner sc = new Scanner(System.in);
//        int n = sc.nextInt();
        System.out.println("Exiting");
        Packet exit_packet = new Packet("exit", "", new IPAddress("0.0.0.0"), new IPAddress("0.0.0.0"));
        networkUtility.write(exit_packet);

        System.out.println("Avg hop count : " + total_hop_count / 100.0);
        System.out.println("Avg packet drops : " + total_dropped / 100.0);

        Thread.sleep(300000);

    }
}

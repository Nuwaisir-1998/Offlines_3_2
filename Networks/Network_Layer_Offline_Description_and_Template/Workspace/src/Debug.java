import java.lang.reflect.Array;
import java.util.*;

public class Debug {
    public static <T> void printv(ArrayList<T> a){
        System.out.println(Arrays.toString(a.toArray()));
    }
    public static void main(String[] args) {
        ArrayList<Integer> a = new ArrayList<>(Collections.nCopies(5 + 1, 0));
        ArrayList<IPAddress> i = new ArrayList<IPAddress>();
        IPAddress ip = new IPAddress("1.1.1.1");
        i.add(ip);
        HashMap<Integer, IPAddress> h = new HashMap<>();
        h.put(1, ip);
        Router r = new Router(1, a, i, h);
        Router d = new Router(1, a, i, h);
//        System.out.println(d.toString());
//        System.out.println(r.toString());

//        System.out.println(d == r);
//        System.out.println("Printing routing table");
//        for(RoutingTableEntry rte : r.getRoutingTable()){
//            System.out.println(rte.getRouterId() + ", " + rte.getDistance() + ", " + rte.getGatewayRouterId());
//        }


        int cnt = 10;
        while(cnt > 0) {
            Random random = new Random(System.currentTimeMillis());
            int rnd = Math.abs(random.nextInt(4));
            System.out.println(rnd);
            cnt--;
        }

    }
}

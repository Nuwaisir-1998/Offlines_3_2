import java.lang.reflect.Array;
import java.util.ArrayList;
import java.util.Arrays;

public class Debug {
    public static <T> void printv(ArrayList<T> a){
        System.out.println(Arrays.toString(a.toArray()));
    }
    public static void main(String[] args) {
//        Router r = new Router();
//        r.initiateRoutingTable();
        boolean t = true;
        int a = 0;
        a = a + (t ? 1 : 0);
        a = a + (t ? 1 : 0);
        System.out.println(a);
    }
}

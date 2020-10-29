import java.lang.reflect.Array;
import java.util.ArrayList;
import java.util.Arrays;

public class Debug {
    public static <T> void printv(ArrayList<T> a){
        System.out.println(Arrays.toString(a.toArray()));
    }
    public static void main(String[] args) {
        Router r = new Router();
        r.initiateRoutingTable();
    }
}

import java.lang.reflect.Array;
import java.util.*;

public class Debug {
    public static <T> void printv(ArrayList<T> a){
        System.out.println(Arrays.toString(a.toArray()));
    }
    public static void main(String[] args) {
        for(int i=0;i<15;i++)
            NetworkLayerServer.add_end_device();
//        System.out.println(NetworkLayerServer.clientInterfaces.size());
    }
}

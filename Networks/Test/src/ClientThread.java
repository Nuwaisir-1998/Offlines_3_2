import java.io.*;
import java.net.Socket;
import java.nio.charset.StandardCharsets;

public class ClientThread extends Thread{
    Socket socket;
    String fileName;
    BufferedOutputStream bufOut;
    ClientThread(Socket socket, String fileName, BufferedOutputStream bufOut){
        this.socket = socket;
        this.fileName = fileName;
        this.bufOut = bufOut;
    }

    int p(String b){
        for(int i=0;i<b.length();i++){
            if(b.charAt(i) == '\0'){
                return i;
            }
        }
        return b.length();
    }

    public void run(){
        try {
            //System.out.println("eof: " + p("abc") + "abc\0\0".substring(0, p("abc")));
            FileInputStream fis = null;
            System.out.println(fileName);
            File file = new File(fileName);
            //System.out.println(file.exists());
            if(!file.exists()){
                System.out.println("file not exists");
            }else {
                fis = new FileInputStream(file);
                int n ;
                byte[] byteArray = new byte[1024];
                int cnt = 1;
                while (fis.read(byteArray) != -1) {
                    socket.getOutputStream().write(byteArray);
                    socket.getOutputStream().flush();
                    byteArray = new byte[1024];
                }
            }
            socket.close();
        } catch (FileNotFoundException e) {
            e.printStackTrace();
        } catch (IOException e) {
            e.printStackTrace();
        }
    }
}

import java.io.BufferedOutputStream;
import java.io.File;
import java.io.IOException;
import java.net.Socket;
import java.util.Scanner;

public class Client {
    public static void main(String[] args) throws IOException {
        while(true) {
            Socket socket = new Socket("localhost", 6790);
            BufferedOutputStream bufOut = new BufferedOutputStream(socket.getOutputStream());
            String str = "UPLOAD\n";

            byte[] init = new byte[7];
            init = str.getBytes();
//        socket.getOutputStream().write(init);
            //System.out.println(init.length);
            bufOut.write(init);
            bufOut.flush();
            Scanner sc = new Scanner(System.in);

            String fileName;
            fileName = sc.nextLine();
            //socket.getOutputStream().flush();
            File file = new File(fileName);
            if(file.exists()) {
                String fileNameSend = fileName + "\n";
                bufOut.write(fileNameSend.getBytes());
                bufOut.flush();
                Thread clientThread = new ClientThread(socket, fileName, bufOut);
                clientThread.start();
            }else {
                System.out.println("File doesn't exist.");
                socket.close();
            }
        }
    }
}

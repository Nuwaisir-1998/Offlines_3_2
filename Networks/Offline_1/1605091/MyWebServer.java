import java.io.File;
import java.io.IOException;
import java.net.ServerSocket;
import java.net.Socket;

public class MyWebServer {
    public static void main(String[] args) throws IOException {
        ServerSocket serverSocket = new ServerSocket(6790);
        System.out.println("Server running.");
        File logFile = new File("log.txt");
        if(logFile.exists())logFile.delete();
        logFile.createNewFile();

        while(true){
            Socket socket = serverSocket.accept();
            Thread serverThread = new ServerThread(socket);
            serverThread.start();
        }


    }
}

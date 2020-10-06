import java.io.*;
import java.net.Socket;
import java.nio.charset.StandardCharsets;
import java.nio.file.Files;
import java.util.Date;
import java.util.StringTokenizer;

public class ServerThread extends Thread{
    Socket socket;
    ServerThread(Socket socket){
        this.socket = socket;
    }

    public static String readFileData(File file, int fileLength) throws IOException {
        FileInputStream fileIn = null;
        byte[] fileData = new byte[fileLength];

        try {
            fileIn = new FileInputStream(file);
            fileIn.read(fileData);
        } finally {
            if (fileIn != null)
                fileIn.close();
        }
        String s = String.valueOf(fileData);
        System.out.println("dsfas : " + s);
        return String.valueOf(fileData);
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
            File logFile = new File("log.txt");
            //System.out.println(logFile.exists());
            InputStream is = socket.getInputStream();
            BufferedWriter bw = new BufferedWriter(new FileWriter(logFile, true));
            BufferedReader in = new BufferedReader(new InputStreamReader(is));
            byte [] init = null;
            BufferedInputStream bis = new BufferedInputStream(is);
            FileInputStream fin = null;
            PrintWriter pr = new PrintWriter(socket.getOutputStream());
            String input = in.readLine();
            if(input == null) return;
            System.out.println("request : " + input);

            String path = null;
            File dir = null;
            File[] files = null;
            StringBuilder stringBuilder = new StringBuilder();
            if(input == null) return;
            StringTokenizer stringTokenizer = new StringTokenizer(input);
            String method = stringTokenizer.nextToken();
            if(method.equals("GET")) {
                path = stringTokenizer.nextToken();
                dir = new File("." + path);
                files = dir.listFiles();
                if (input != null) bw.write(input + "\n");
                bw.flush();
            }
            if(files != null){
                for(File file : files){
//                    if(path.length() > 0 && path.charAt(0) == '/'){
//                        path = path.substring(1, path.length());
//                    }
                    if(file.isDirectory()){
//                        System.out.println("name : " + file.getName());
//                        System.out.println(file.getParent() + "/" + file.getName());
                        stringBuilder.append("<b><a href = " + path + "/" + file.getName() + ">" + file.getName() + "</a></b><br>" );
                    }else
                        stringBuilder.append("<a href = " + path + "/" + file.getName() + ">" + file.getName() + "</a><br>" );

                    stringBuilder.append("\n");
                }
            }

            String content = stringBuilder.toString();
            if(input.length() > 0) {
                if(input.startsWith("GET"))
                {
//                    System.out.println(path);
                    //                    System.out.println(file.exists());
                    String contentType = Files.probeContentType(dir.toPath());
                    String disposition = null;
                    int contentLength;
                    if(contentType == null){
                        contentType = "text/html";
                    }
                    if(dir.isDirectory()){
                        disposition = "inline";
                        contentLength = content.length();
                    }else {
                        disposition = "attachment";
                        contentLength = (int)dir.length();
                    }

                    if(!dir.exists()){
//                        System.out.println("404");
                        pr.write("HTTP/1.1 404 Not Found\r\n");
                        pr.write("Server: Java HTTP Server: 1.0\r\n");
                        pr.write("Date: " + new Date() + "\r\n");
                        pr.write("\r\n");
                        pr.flush();

                        bw.write("HTTP/1.1 404 Not Found\r\n");
                        bw.write("Server: Java HTTP Server: 1.0\r\n");
                        bw.write("Date: " + new Date() + "\r\n");
                        bw.write("\r\n");
                        bw.flush();
                    }else {
                        pr.write("HTTP/1.1 200 OK\r\n");
                        pr.write("Server: Java HTTP Server: 1.0\r\n");
                        pr.write("Date: " + new Date() + "\r\n");
                        pr.write("Content-Disposition: " + disposition + "\r\n");
                        pr.write("Content-Type: " + contentType + "\r\n");
                        pr.write("Content-Length: " + contentLength + "\r\n");
                        pr.write("\r\n");
                        pr.write(content);
                        pr.flush();

                        bw.write("HTTP/1.1 200 OK\r\n");
                        bw.write("Server: Java HTTP Server: 1.0\r\n");
                        bw.write("Date: " + new Date() + "\r\n");
                        bw.write("Content-Disposition: " + disposition + "\r\n");
                        bw.write("Content-Type: " + contentType + "\r\n");
                        bw.write("Content-Length: " + contentLength + "\r\n");
                        bw.write("\r\n");
                        bw.flush();


                        if (!dir.isDirectory() && !dir.getName().equals("favicon.ico")) {
                            //System.out.println(dir.getName() + " " + dir.isDirectory());
                            fin = new FileInputStream(dir);
                            byte[] byteArray = new byte[1024];
                            while (fin.read(byteArray) > 0) {
                                socket.getOutputStream().write(byteArray);
                                socket.getOutputStream().flush();
                            }
                            // pr.write("<p> " + s + "</p>");
                        }
                    }
                }

                else
                {
                    //System.out.println("else e");
                    byte [] bytes = new byte[1024];
                    int n;
                    //System.out.println();
                    int cnt = 50;
                    byte [] fileName = new byte[1024];
//                    bis.read(fileName);
//                    String s = new String(fileName);
//                    String pathOfTarget = "root/" + s;
                    String pathOfTarget = "root/" + in.readLine();
                    System.out.println("path: "+pathOfTarget);
                    File targetFile = new File(pathOfTarget);
                    if(targetFile.exists()){
                        targetFile.delete();
                    }else
                        targetFile.createNewFile();
                    BufferedWriter bw2 = new BufferedWriter(new FileWriter(targetFile, true));
                    StringBuilder sb = new StringBuilder();

                    while((n=socket.getInputStream().read(bytes)) > 0) {
                        String str = new String(bytes);
//                        int idx = p(str);
                        str = str.substring(0, p(str));
                        //System.out.println("str : " + str);
                        //sb.append(str);
                        bw2.write(str);
                        bw2.flush();
                    }
                }
            }

            socket.close();
            //System.out.println("socket closed");
        } catch (IOException e) {
            e.printStackTrace();
        }

    }
}

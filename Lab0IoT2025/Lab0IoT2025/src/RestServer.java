import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.net.ServerSocket;
import java.net.Socket;

public class RestServer {

    boolean runServer = true;
    ServerSocket ss;

    int sensor = 42;

    public RestServer(){
        try {
            ss = new ServerSocket(80);
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    public void start() {
        try {

            while(runServer){
                Socket s = ss.accept();
                System.out.println("Client connected");

                InputStream is = s.getInputStream();
                OutputStream os = s.getOutputStream();

                byte[] buffer = new byte[4048];
                int readBytes = is.read(buffer);

                String requestMsg = new String(buffer).trim();
                System.out.println("Request Message: " + requestMsg);

                String[] split = requestMsg.split("\r\n");

                String rqLine = split[0];
                String[] rqSplit = rqLine.split(" ");

                String method = rqSplit[0];
                String path = rqSplit[1];
                String protocol = rqSplit[2];
                System.out.println("Method: " + method + " Path: " + path + " Protocol: " + protocol);

                String responseHeader = "";
                String responseBody = "";

                if(method.equalsIgnoreCase("GET")){

                    if(path.equalsIgnoreCase("/sensor")){

                        responseHeader = "HTTP/1.1 200 OK\r\n" +
                                "Content-Type: text/html\r\n" +
                                "Content-Length: ";
                        responseBody = "<h1>Senso Value</h1> sensor = " + sensor;

                    } else {
                        responseHeader = "HTTP/1.1 404 File not Found\r\n" +
                                "Content-Type: text/html\r\n" +
                                "Content-Length: ";
                        responseBody = "<h1>File not found</h1>";
                    }

                } else if (method.equalsIgnoreCase("POST")){

                    if(path.equalsIgnoreCase("/sensor")){

                        String[] split1 = requestMsg.split("\r\n\r\n");

                        String sensorValue = split1[1];
                        sensor = Integer.parseInt(sensorValue);

                        responseHeader = "HTTP/1.1 200 OK\r\n" +
                                "Content-Type: text/html\r\n" +
                                "Content-Length: ";
                        responseBody = "<h1>Senso Updated!</h1>";
                    }
                }

                //responseBody = "<h1>Hello World!</h1> Hej!!" + System.currentTimeMillis();

                responseHeader = responseHeader + responseBody.length() + "\r\n\r\n";

                String completeResponse = responseHeader + responseBody;

                os.write(completeResponse.getBytes());
                os.close();
                s.close();

            }

        } catch (Exception e) {
            e.printStackTrace();
        }


    }
}

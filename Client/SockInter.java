package Client;

import java.net.*;
import java.io.*;
import java.util.LinkedList;

public class SockInter {

    private Socket socket;
    private PrintWriter out;
    private DataInputStream in;

    SockInter() {
        try {
            this.socket = new Socket("127.0.0.1", 4200);
            this.out = new PrintWriter(socket.getOutputStream(), true);
            this.in = new DataInputStream(new BufferedInputStream(socket.getInputStream()));
        }

        catch(Exception e) {
            System.out.println("Error when connecting: " + e.getMessage());
        }
    }

    private void sendMessage(String msg) {
        out.println(msg);
    }

    private String receiveMessage() {
        try { 
            int len = in.readInt();
            byte[] bytes = new byte[len];
            in.readFully(bytes);
            String text = new String(bytes, "ISO-8859-1").trim();

            return text;
        }
        catch(Exception e) {
            System.out.println("Error receiving message: " + e.getMessage());
            return null;
        }
    }    

    public void createTable(String csv_path) {
        sendMessage("1 " + csv_path);
        receiveMessage();
    }

    public LinkedList<Register> searchAll() {
        sendMessage("2");
        String response = receiveMessage();
        
        response = response.replace("\\n", "\n");
        return Register.readMultipleRegisters(response);
    }

    public LinkedList<Register> searchAllFilter(Register filter) {
        sendMessage("3 " + filter.toFilter());
        String response = receiveMessage();

        response = response.replace("\\n", "\n");
        response = response.substring(response.indexOf("\n")+2);
        return Register.readMultipleRegisters(response);
    }

    public void remove(Register filter) {
        sendMessage("5 " + filter.toFilter());
        receiveMessage();
    }

    public void insert(Register reg) {
        sendMessage("6 " + reg.toRegInput());
        receiveMessage();
    }

    public void close() {
        try {
            this.socket.close();
        }
        catch (Exception e) {
            System.out.println("Trouble closing socket: " + e.getMessage());
        }
    }

}

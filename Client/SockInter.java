package Client;

import java.net.*;
import java.io.*;
import java.util.LinkedList;

public class SockInter {

    private Socket socket;
    private PrintWriter out;
    private DataInputStream in;

    public static void main(String[] args) {
        SockInter sock = new SockInter();
        sock.createTable("dado1.csv");    
        Register filter = new Register();
        filter.name = "K. DE BRUYNElal";

        sock.searchAllFilter(filter);
    }

    public SockInter() {
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

    public boolean createTable(String csv_path) {
        sendMessage("1 " + csv_path);
        String text = receiveMessage();
        try {
            float _ = Float.parseFloat(text);
            return true;
        }
        catch (Exception e){
            return false;
        }
    }

    public LinkedList<Register> searchAll() {
        sendMessage("2");
        String response = receiveMessage();
        
        response = response.replace("\\n", "\n");
        try {
            return Register.readMultipleRegisters(response);
        }
        catch (Exception e) {
            return null;
        }
    }

    public LinkedList<Register> searchAllFilter(Register filter) {
        sendMessage("3 " + filter.toFilter());
        String response = receiveMessage();

        response = response.replace("\\n", "\n");
        response = response.substring(response.indexOf("\n")+2);
        try {
            return Register.readMultipleRegisters(response);
        }
        catch (Exception e) {
            return null;
        }
    }

    public boolean remove(Register filter) {
        sendMessage("5 " + filter.toFilter());
        String text = receiveMessage();
        try {
            text = text.replace("\\n", "\n");
            String[] splitted = text.split("\n");
            float _ = Float.parseFloat(splitted[0]);
            return true;
        }
        catch (Exception e){
            return false;
        }
    }

    public boolean insert(Register reg) {
        sendMessage("6 " + reg.toRegInput());
        String text = receiveMessage();
        try {
            text = text.replace("\\n", "\n");
            String[] splitted = text.split("\n");
            float _ = Float.parseFloat(splitted[0]);
            return true;
        }
        catch (Exception e){
            return false;
        }
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

import javax.swing.*;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.io.*;
import java.net.*;

public class GUI {
    public static void main(String[] args) {
        SimpleClient client = new SimpleClient();//"127.0.0.1", 4200
        // Initialize the GUI
        SwingUtilities.invokeLater(new Runnable() {
            public void run() {
                new GUI().new YearMenu();
            }
        });
    }

    private static class SimpleClient {
        private Socket socket;
        private PrintWriter writer;
        private BufferedReader reader;

        public void setupConnection(String hostname, int port) {
            try {
                socket = new Socket(hostname, port);
                OutputStream output = socket.getOutputStream();
                writer = new PrintWriter(output, true);

                InputStream input = socket.getInputStream();
                reader = new BufferedReader(new InputStreamReader(input));
            } catch (UnknownHostException ex) {
                System.out.println("Server not found: " + ex.getMessage());
            } catch (IOException ex) {
                System.out.println("I/O error: " + ex.getMessage());
            }
        }

        public void sendMessage(String message) {
            if (socket != null && writer != null && reader != null) {
                writer.println(message);
                try {
                    String response = reader.readLine();
                    System.out.println("Server response: " + response);
                } catch (IOException ex) {
                    System.out.println("Error reading response: " + ex.getMessage());
                }
            }
        }
        
        public void closeConnection() {
            try {
                if (socket != null) {
                    socket.close();
                }
            } catch (IOException ex) {
                System.out.println("Error closing socket: " + ex.getMessage());
            }
        }
    }

    private class YearMenu {
        public YearMenu() {
            // Create the main frame
            JFrame frame = new JFrame("Menu Anos");
            frame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
            frame.setSize(400, 300);

            // Create the menu bar
            JMenuBar menuBar = new JMenuBar();

            // Create menu items for each year from 2017 to 2023 and add them to the menu bar
            for (int year = 2017; year <= 2023; year++) {
                JMenuItem yearItem = new JMenuItem(String.valueOf(year));

                // Add action listener to each menu item
                yearItem.addActionListener(new ActionListener() {
                    public void actionPerformed(ActionEvent e) {
                        //inserir comando pra loadar os dados de um ano
                        //provavelmente criar um bool de "dados_carregados" pra nÃ£o deixar procurar jogadores ou outras coisas sem ter carregado os dados
                    }
                });

                // Add the menu item to the menu bar
                menuBar.add(yearItem);
            }

            // Add the menu bar to the frame
            frame.setJMenuBar(menuBar);

            

            // Set the frame visibility to true
            frame.setVisible(true);
        }
    }
}
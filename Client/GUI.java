package Client;
import javax.swing.*;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;

public class GUI {
    public static void main(String[] args) {
        SockInter sock = new SockInter();
        // Initialize the GUI
        SwingUtilities.invokeLater(new Runnable() {
            public void run() {
                new GUI().new YearMenu();
            }
        });
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
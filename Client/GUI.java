package Client;
import javax.swing.*;
import java.awt.event.*;
import java.awt.*;
import java.io.*;
import java.net.*;
import java.util.LinkedList;
import java.util.Iterator;

//Classe geral que engloba tudo
public class GUI {
    private JFrame frame; //janela principal
    private InputPanel inputPanel;
    public JPanel cardPanel; //objeto pra alternar entre visualizacao de um player e a tabela scrollable com varios

    
    public boolean dataBaseLoaded = false;
   
    public static final char PESQUISA = 0;
    public static final char EDICAO = 1;
    public static final char DELECAO = 2;

    public SockInter interactiveSocket;

    public GUI() {
        interactiveSocket = new SockInter();
        reset();
    }

    //funcao que povoa o GUI e fecha outras janelas
    public void reset(){
        if(dataBaseLoaded){
            this.frame.dispose();
            
        }
        //cria e seta a janela
        frame = new JFrame("Input Panel");
        frame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        frame.setSize(1000, 600);
        //adiciona a variavel de card à janela
        cardPanel = new JPanel(new CardLayout());

        YearMenu yearMenu = new YearMenu();
        frame.setJMenuBar(yearMenu.getMenuBar());

        String[] empty_strings = {"", "", "", "", ""};
        InputPanel inputPanel = new InputPanel(empty_strings, PESQUISA);
        cardPanel.add(inputPanel, "input panel");

        frame.add(cardPanel);
        frame.setVisible(true);
    }

    public static void main(String[] args) {
        new GUI();
    }

    //objeto pro menu, responsavel por carregar dados tambem
    private class YearMenu {
        private JMenuBar menuBar;
        public YearMenu() {
            menuBar = new JMenuBar();
            for (int year = 2017; year <= 2023; year++) {
                JMenuItem yearItem = new JMenuItem(String.valueOf(year));
                yearItem.addActionListener(new ActionListener() {
                    public void actionPerformed(ActionEvent e) {
                        JOptionPane.showMessageDialog(null, "Ano " + yearItem.getText() + " selecionado. Carregando dados");
                        if(dataBaseLoaded)
                            reset();
                        
                        dataBaseLoaded = loadData("dado1.csv");
                    }
                });
                menuBar.add(yearItem);
            }
            JMenuItem show_txt = new JMenuItem("print");
            show_txt.addActionListener(new ActionListener() {
                public void actionPerformed(ActionEvent e) {
                    dataBaseLoaded = loadData("dado1.csv");
                    StringBuilder massiveFuckingString = new StringBuilder();
                    LinkedList<Register> allPlayersData = interactiveSocket.searchAll();
                    Iterator<Register> it = allPlayersData.iterator();
                    Register temp;
                    while (it.hasNext()) {
                        temp = it.next();
                        massiveFuckingString.append(temp.toString()).append("\n");
                    }

                    JTextArea textArea = new JTextArea(massiveFuckingString.toString());
                    textArea.setEditable(false);  // torna a area de texto nao editável
                    textArea.setLineWrap(true);   // permite line wrapping
                    textArea.setWrapStyleWord(true);  // Wrap no limite das palavras

                    JScrollPane scrollPane = new JScrollPane(textArea);

                    JFrame frame = new JFrame("Text Display");
                    frame.setSize(900, 700);
                    frame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
                    frame.add(scrollPane, BorderLayout.CENTER);
                    frame.setVisible(true);
                }
            });

            menuBar.add(show_txt);
        }

        public JMenuBar getMenuBar() {
            return menuBar;
        }

        private boolean loadData(String path_to_csv){
            
            interactiveSocket.createTable(path_to_csv);
            return true;
        }
    }

    //classe responsavel por mostrar a lista de multiplos jogadores (inclusive quando ha todos os jogadores)
    private class PlayerDisplay extends JPanel{
        private boolean hasWindow;
        private JFrame auxiliarWindow;
        //quando os jogadores sao carregados, eles salvos aqui em "memoria primaria" para uso
        LinkedList<Register> players;

        private class PlayerButton extends JButton{
            private int playerId;
            private int linkedListIndex;

            public PlayerButton(int linkedListIndex, int playerId, String buttonText){
                super(buttonText);
                this.playerId = playerId;
                this.linkedListIndex = linkedListIndex;
            }
            public int getLLIndex(){
                return this.linkedListIndex;
            }
        }

        private Register displayGetRegister(int id){
            return (Register) this.players.get(id);
        }

        public PlayerDisplay(LinkedList<Register> players, Timer timer){
            auxiliarWindow = new JFrame();
           
            render(players, timer);
        }

        private void render(LinkedList<Register> players, Timer timer){
            this.players = players;
            JPanel scrollable = new JPanel();
            scrollable.setLayout(new BoxLayout(scrollable, BoxLayout.Y_AXIS));
            Iterator it = this.players.iterator();
            int i = 0;
            Register temp = new Register();
            while(it.hasNext()){
                temp = (Register) it.next();
                PlayerButton tempPlayer = new PlayerButton(i, temp.id, temp.toString());
                i++;
                tempPlayer.addActionListener(new ActionListener() {
                @Override
                public void actionPerformed(ActionEvent e){
                    if(!auxiliarWindow.isShowing()){
                        auxiliarWindow.setSize(900, 540);
                       
                        auxiliarWindow.addWindowListener(new WindowAdapter() {
                            @Override
                            public void windowClosing(WindowEvent e) {
                                
                                System.out.println("Child frame is closing. Perform action in parent frame.");
                                
                            }
                        });
                        int playerLLId = ((PlayerButton) e.getSource()).getLLIndex();
                        Register playerTemp = displayGetRegister(playerLLId);
                        
                        String[] stringAux = {String.valueOf(playerTemp.id), String.valueOf(playerTemp.age), playerTemp.name, playerTemp.country, playerTemp.team};
                        InputPanel localInputPanel = new InputPanel(stringAux, EDICAO);
                        auxiliarWindow.add(localInputPanel);
                        auxiliarWindow.setVisible(true);
                    }
                }
            });
                scrollable.add(tempPlayer);
            }
            JScrollPane scrollFrame = new JScrollPane(scrollable);
            scrollFrame.setPreferredSize(new Dimension(1000, 400));
            setAutoscrolls(true);
            scrollFrame.setVerticalScrollBarPolicy(JScrollPane.VERTICAL_SCROLLBAR_ALWAYS);
            scrollFrame.setHorizontalScrollBarPolicy(JScrollPane.HORIZONTAL_SCROLLBAR_AS_NEEDED);
            
            JButton goBackButton = new JButton("Voltar para pesquisa");
            goBackButton.setBackground(Color.CYAN);
            goBackButton.addActionListener(new ActionListener() {
                @Override
                public void actionPerformed(ActionEvent e){
                    timer.stop();
                    CardLayout cl = (CardLayout) (cardPanel.getLayout());
                    cl.show(cardPanel, "input panel");
                }
            });
            add(scrollFrame);
            add(goBackButton);
        }
        
        
    }

    //tabela com os dados de um jogador. Vazia para buscas, preenchida parcial ou totalmente quando associada a um jogador presente na lista
    private class InputPanel extends JPanel {
        private JTextField campoId;
        private JTextField campoIdade;
        private JTextField campoNome;
        private JTextField campoNacionalidade;
        private JTextField campoClube;
        LinkedList<Register> filteredPlayersData = null;
        Register filter = null;
        LinkedList<Register> allPlayersData = null;

        private JLabel typeLabel;

        private PlayerDisplay playerDisplay = null;

        private int actualOperation;

        //TODO: mudar de receber um array de strings pra receber um Register vazio
        public InputPanel(String[] values, char operation) {
            actualOperation = operation;
            setLayout(new BorderLayout());

            JPanel formPanel = new JPanel(new GridLayout(6, 2)); // 6 fileiras, 2 colunas

            JLabel idLabel = new JLabel("Id:");
            campoId = new JTextField(15);
            campoId.setText(values[0]);

            JLabel idadeLabel = new JLabel("Idade:");
            campoIdade = new JTextField(15);
            campoIdade.setText(values[1]);

            JLabel nomeLabel = new JLabel("Nome:");
            campoNome = new JTextField(15);
            campoNome.setText(values[2]);

            JLabel nacionLabel = new JLabel("Nacionalidade:");
            campoNacionalidade = new JTextField(15);
            campoNacionalidade.setText(values[3]);

            JLabel clubeLabel = new JLabel("Clube:");
            campoClube = new JTextField(15);
            campoClube.setText(values[4]);

            JButton leftButton = new JButton(" ");
            JButton rightButton = new JButton(" ");

            String typeIndicator = " ";
            Color labelColor = Color.WHITE;

            switch(actualOperation){
                case PESQUISA:
                    leftButton.setText("Submeter Pesquisa");
                    leftButton.setBackground(Color.BLUE);
                    
                    Timer timer = new Timer(100, null);
                    ActionListener taskPerformer = new ActionListener() {
                        public void actionPerformed(ActionEvent evt) {
                            values[0] = campoId.getText();
                            values[1] = campoIdade.getText();
                            values[2] = campoNome.getText();
                            values[3] = campoNacionalidade.getText();
                            values[4] = campoClube.getText();
                            Register filter = new Register(values);
                            // Faz as atividades periodicas aqui
                            filteredPlayersData = interactiveSocket.searchAllFilter(filter);
                            System.out.println("Received filtered players data");

                            // Cria um novo PlayerDisplay com dados filtrados
                            playerDisplay = new PlayerDisplay(filteredPlayersData, timer);
                            System.out.println("Created PlayerDisplay for filtered players");

                            // coloca o PlayerDisplay no cardPanel
                            cardPanel.add(playerDisplay, "filtered players");

                            // mostra o card "jogadores filtrados" no cardPanel
                            CardLayout cl = (CardLayout) cardPanel.getLayout();
                            cl.show(cardPanel, "filtered players");
                        }
                    };
                    timer.addActionListener(taskPerformer);
                   
                    leftButton.addActionListener(new ActionListener() {
                        @Override
                        public void actionPerformed(ActionEvent e) {
                            System.out.println("botao de busca apertado.");
                            if(dataBaseLoaded){
                                values[0] = campoId.getText();
                                values[1] = campoIdade.getText();
                                values[2] = campoNome.getText();
                                values[3] = campoNacionalidade.getText();
                                values[4] = campoClube.getText();
                                Register filter = new Register(values);
                                
                                timer.start();
                            }
                        }
                    });

                    rightButton.setText("Mostrar Todos");
                    rightButton.setBackground(Color.CYAN);
                   
                    Timer timer2 = new Timer(100, null);
                    ActionListener taskPerformer2 = new ActionListener() {
                        public void actionPerformed(ActionEvent evt) {
                            // Faz as atividades periódicas
                            allPlayersData = interactiveSocket.searchAll();
                            playerDisplay = new PlayerDisplay(allPlayersData, timer2);

                            // coloca o PlayerDisplay no cardPanel
                            cardPanel.add(playerDisplay, "filtered players");

                            // mostra o card "jogadores filtrados" no cardPanel
                            CardLayout cl = (CardLayout) cardPanel.getLayout();
                            cl.show(cardPanel, "filtered players");
                        }
                    };
                    timer2.addActionListener(taskPerformer2);
                  
                    rightButton.addActionListener(new ActionListener() {
                        @Override
                        public void actionPerformed(ActionEvent e) {
                            System.out.println("botao de mostrar todos apertado.");
                            if(dataBaseLoaded){
                                
                                timer2.start();
                            }
                        }
                    });

                    typeIndicator = "Pesquisa";
                    labelColor = Color.BLUE;
                    break;
                case EDICAO:
                    leftButton.setText("Alterar Dados");
                    leftButton.setBackground(Color.YELLOW);
                    leftButton.addActionListener(new ActionListener() {
                        @Override
                        public void actionPerformed(ActionEvent e) {
                            System.out.println("botao de alterar dados apertado. Ainda nao faz nada");
            
                            int result = JOptionPane.showConfirmDialog(frame,"Tem certeza que quer editar o registro do jogador?", "Confirmar edição",
                               JOptionPane.YES_NO_OPTION, JOptionPane.QUESTION_MESSAGE);
                            if(result == JOptionPane.YES_OPTION){
                                Register toEdit = new Register(values);
                                boolean funfouRemEd = interactiveSocket.remove(toEdit);
                                System.out.println("passou de deletar um jogador");
                                if(funfouRemEd){
                                    JOptionPane.showMessageDialog(null, "Jogador apagado com sucesso");
                                    values[0] = campoId.getText();
                                    values[1] = campoIdade.getText();
                                    values[2] = campoNome.getText();
                                    values[3] = campoNacionalidade.getText();
                                    values[4] = campoClube.getText();
                                    toEdit = new Register(values);
                                    boolean funfouEd = interactiveSocket.insert(toEdit);
                                    if(funfouEd){
                                        JOptionPane.showMessageDialog(null, "Jogador editado com sucesso");
                                        JFrame frameToClose = (JFrame) SwingUtilities.getWindowAncestor((JButton) e.getSource());
                                        frameToClose.dispose();
                                        
                                    }
                                }
                            }
                        }
                    });

                    rightButton.setText("Deletar Jogador");
                    rightButton.setBackground(Color.RED);
                    rightButton.addActionListener(new ActionListener() {
                        @Override
                        public void actionPerformed(ActionEvent e) {
                            System.out.println("botao de deletar jogador apertado. Ainda nao faz nada");

                            int result = JOptionPane.showConfirmDialog(frame,"Tem certeza que quer apagar o registro do jogador?", "Confirmar deleção",
                               JOptionPane.YES_NO_OPTION, JOptionPane.QUESTION_MESSAGE);
                            if(result == JOptionPane.YES_OPTION){
                               Register toRemove = new Register(values);
                               boolean funfouRem = interactiveSocket.remove(toRemove);
                               System.out.println("passou de deletar um jogador");
                               if(funfouRem){
                                System.out.println("Jogador apagado com sucesso");
                               
                                JFrame frameToClose = (JFrame) SwingUtilities.getWindowAncestor((JButton) e.getSource());
                                frameToClose.dispose();
                                
                               }
                            }
                        }
                    });

                    typeIndicator = "Edição ou Deleção";
                    labelColor = Color.YELLOW;
                    break;
            }

            typeLabel = new JLabel(typeIndicator, SwingConstants.CENTER);
            typeLabel.setOpaque(true);
            typeLabel.setBackground(labelColor);
            typeLabel.setForeground(Color.WHITE);
            typeLabel.setPreferredSize(new Dimension(60, 30));
            add(typeLabel, BorderLayout.NORTH);

            formPanel.add(idLabel);
            formPanel.add(campoId);
            formPanel.add(idadeLabel);
            formPanel.add(campoIdade);
            formPanel.add(nomeLabel);
            formPanel.add(campoNome);
            formPanel.add(nacionLabel);
            formPanel.add(campoNacionalidade);
            formPanel.add(clubeLabel);
            formPanel.add(campoClube);
            formPanel.add(leftButton);
            formPanel.add(rightButton);

            add(formPanel, BorderLayout.CENTER);
        }

        private void closePanel(){
            Window win = SwingUtilities.getWindowAncestor(this);
            win.dispose();
        }
    }
}
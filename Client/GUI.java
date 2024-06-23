package Client;
import javax.swing.*;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.*;
import java.io.*;
import java.net.*;
import java.util.LinkedList;
import java.util.Iterator;

//Classe geral que engloba tudo
public class GUI {
    private JFrame frame; //janela principal
    private InputPanel inputPanel;
    public JPanel cardPanel; //objeto pra alternar entre visualizavao de um player e a tabela scrollable com varios

    //polish esses booleano devia ser private e com getter
    public boolean dataBaseLoaded = false;
    public static final char PESQUISA = 0;
    public static final char EDICAO = 1;


    public SockInter interactiveSocket;

    public GUI() {
        interactiveSocket = new SockInter();
        reset();
    }

    //funcao que povoa o GUI e, em teoria, fecha outras janelas
    public void reset(){
        if(dataBaseLoaded){
            this.frame.dispose();
            //polish resolver o problema que eu mostrei na call
            //if(this.inputPanel.playerDisplay != null && this.inputPanel.playerDisplay.auxiliarWindow.isShowing())
            //    this.inputPanel.playerDisplay.auxiliarWindow.dispose();
        }
        //cria e seta a janela
        frame = new JFrame("Input Panel");
        frame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        frame.setSize(1000, 600);
        //adiciona a variavel la de card à janela
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
                        //polish fingir que tem mais de um ano, talvez mudar o arquivo usado
                        dataBaseLoaded = loadData("dado1.csv");
                    }
                });
                menuBar.add(yearItem);
            }
        }

        public JMenuBar getMenuBar() {
            return menuBar;
        }

        private boolean loadData(String path_to_csv){
            //polish verificar se deu certo, de ALGUMA MANEIRA que eu nao sei como
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

        public PlayerDisplay(LinkedList<Register> players){
            this.players = players;
            auxiliarWindow = new JFrame();
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
                        auxiliarWindow.setDefaultCloseOperation(JFrame.DISPOSE_ON_CLOSE);

                        int playerLLId = ((PlayerButton) e.getSource()).getLLIndex();
                        Register playerTemp = displayGetRegister(playerLLId);
                        //System.out.println(playerTemp.toString());
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
                    CardLayout cl = (CardLayout) (cardPanel.getLayout());
                    cl.show(cardPanel, "input panel");
                }
            });
            add(scrollFrame);
            add(goBackButton);
        }
        
        //apen criar uma funcao para editar o conteudo de um jogador em players ou pra apagar ele, se nao a copia local dos dados fica diferente da copia em "memoria secundaria"
    }

    //tabela com os dados de um jogador. Vazia para buscas, preenchida parcial ou totalmente quando associada a um jogador presente na lista
    private class InputPanel extends JPanel {
        private JTextField campoId;
        private JTextField campoIdade;
        private JTextField campoNome;
        private JTextField campoNacionalidade;
        private JTextField campoClube;

        private JLabel typeLabel;

        private PlayerDisplay playerDisplay = null;

        private int actualOperation;//porque java nao me deixa fazer uma maquina de estados simples sem ser um pe no saco

        public int isInteger(String str) {
            if (str == null) {
                return -1;
            }
            int length = str.length();
            if (length == 0) {
                return -1;
            }
            int i = 0;
            if (str.charAt(0) == '-') {
                if (length == 1) {
                    return -1;
                }
                i = 1;
            }
            for (; i < length; i++) {
                char c = str.charAt(i);
                if (c < '0' || c > '9') {
                    return -1;
                }
            }
            return Integer.valueOf(str);
        }

        public InputPanel(String[] values, char operation) {
            actualOperation = operation;
            setLayout(new BorderLayout());

            JPanel formPanel = new JPanel(new GridLayout(6, 2)); // 6 rows, 2 columns

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
                    leftButton.addActionListener(new ActionListener() {
                        @Override
                        public void actionPerformed(ActionEvent e) {
                            System.out.println("botao de busca apertado.");
                            if(dataBaseLoaded){
                                Register filter = new Register();
                                filter.id = isInteger(campoId.getText());
                                filter.age = isInteger(campoIdade.getText());
                                String str = "";
                                str = campoNome.getText();
                                if(str.length() == 0)
                                    filter.name = null;
                                else
                                   filter.name = str;
                                //------------------------------
                                str = campoNacionalidade.getText();
                                if(str.length() == 0)
                                    filter.country = null;
                                else
                                   filter.country = str;
                                //----------------------
                                str = campoClube.getText();
                                if(str.length() == 0)
                                    filter.team = null;
                                else
                                   filter.team = str;
                                System.out.println("passou de criar o filter");
                                //System.out.println(filter.toString());
                                //System.out.println(filter.toFilter());
                                
                                //apen fazer a busca de jogadores funcionar
                                LinkedList<Register> filteredPlayersData = interactiveSocket.searchAllFilter(filter);
                                System.out.println("passou de receber os dados da pesquisa");
                                playerDisplay = new PlayerDisplay(filteredPlayersData);
                                System.out.println("passou de criar o filtered players");
                                cardPanel.add(playerDisplay, "filtered players");
                                CardLayout cl = (CardLayout) (cardPanel.getLayout());
                                cl.show(cardPanel, "filtered players");
                            }
                        }
                    });

                    rightButton.setText("Mostrar Todos");
                    rightButton.setBackground(Color.CYAN);
                    rightButton.addActionListener(new ActionListener() {
                        @Override
                        public void actionPerformed(ActionEvent e) {
                            System.out.println("botao de mostrar todos apertado.");
                            if(dataBaseLoaded){
                                LinkedList<Register> allPlayersData = interactiveSocket.searchAll();
                                playerDisplay = new PlayerDisplay(allPlayersData);
                                cardPanel.add(playerDisplay, "all players");
                                CardLayout cl = (CardLayout) (cardPanel.getLayout());
                                cl.show(cardPanel, "all players");
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
                            //apen mandar comando de edicao de dados se ele tiver funcionando. Talvez mostrar uma janelinha antes perguntando se tem certeza
                        }
                    });

                    rightButton.setText("Deletar Jogador");
                    rightButton.setBackground(Color.RED);
                    rightButton.addActionListener(new ActionListener() {
                        @Override
                        public void actionPerformed(ActionEvent e) {
                            System.out.println("botao de deletar jogador apertado. Ainda nao faz nada");
                            //apen mandar comando de delecao de dados se ele tiver funcionando. Talvez mostrar uma janelinha antes perguntando se tem certeza
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

        //private boolean searchFile(){}
    }
}
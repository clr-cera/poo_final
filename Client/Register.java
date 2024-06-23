package Client;

import java.util.LinkedList;

public class Register {
    public int id;
    public int age;
    public String name;
    public String country;
    public String team;

    Register() {
        this.id = -1;
        this.age = -1;
        this.name = null;
        this.country = null;
        this.team = null;
    }

    public static LinkedList<Register> readMultipleRegisters(String string) {
        LinkedList<Register> list = new LinkedList<Register>();

        for (String stringReg : string.split("\n\n")) {
            Register reg = new Register();
            String[] lines = stringReg.split("\n");

            reg.id = Integer.parseInt(lines[0]);
            reg.age = Integer.parseInt(lines[1]);
            reg.name = lines[2];
            reg.country = lines[3];
            reg.team = lines[4];

            list.addLast(reg);
            //System.out.println(reg.toRegInput());
        }

        return list;
    }

    public String toString() {
        String string = "";

        string += id      + " | ";
        string += age     + " | ";
        string += name    + " | ";
        string += country + " | ";
        string += team;

        return string;
    }

    public String toRegInput() {
        String string = "";

        string += id  +  " ";
        string += age + " ";
        string += "\"" + name + "\" ";
        string += "\"" + country + "\" ";
        string += "\"" + team + "\"";

        return string;

    }

    public String toFilter() {
        String string = "";
        int counter = 0;

        if (id != -1) {
            counter++;
            string += "id " + id + " ";
        }
        if (age != -1) {
            counter++;
            string += "idade " + age + " ";
        }
        if (name != null) {
            counter++;
            string += "nomeJogador \"" + name + "\" "; 
        }
        if (country != null) {
            counter++;
            string += "nacionalidade \"" + country + "\" "; 
        }
        if (team != null) {
            counter++;
            string += "nomeClube \"" + team + "\" "; 
        }
        return counter + " " + string;
    }
    
    public void setTest() {
        this.id = -50;
        this.age = 27;
        this.name = "Gabrielzinho";
        this.country = "Paraiba";
        this.team = "Jovem";
    }

    public void setTestFilter() {
        this.age = 27;
        this.country = "Paraiba";
        this.team = "Jovem";
    }

}
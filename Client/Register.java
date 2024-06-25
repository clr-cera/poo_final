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

    Register(String[] values) {
        this.id = isInteger(values[0]);
        this.age = isInteger(values[1]);
        String str = "";
        str = values[2];
        if(str.length() == 0)
            this.name = null;
        else
           this.name = str;
        //------------------------------
        str = values[3];
        if(str.length() == 0)
            this.country = null;
        else
           this.country = str;
        //----------------------
        str = values[4];
        if(str.length() == 0)
            this.team = null;
        else
           this.team = str;
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
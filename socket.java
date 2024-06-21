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
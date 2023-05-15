/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package UI;

import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.OutputStream;
import java.io.PrintWriter;
import java.net.InetAddress;
import java.net.Socket;
import java.net.URL;
import java.net.UnknownHostException;
import java.util.ResourceBundle;

import org.json.simple.JSONObject;
import org.json.simple.parser.JSONParser;
import org.json.simple.parser.ParseException;

import javafx.event.ActionEvent;
import javafx.fxml.FXML;
import javafx.fxml.Initializable;
import javafx.scene.control.Button;
import javafx.scene.control.Label;
import javafx.scene.control.TextField;
/**
 * FXML Controller class
 *
 */
public class MainController implements Initializable {

    /**
     * Initializes the controller class.
     */
    
    @FXML private Button btnPlay;
    @FXML private Label  lblResult;
    @FXML private TextField txtGroupName;

    private String user;

    private final static int AUTHPORT = 5001;
    private Socket socket;
    private OutputStream outputStream;
    private InputStream inputStream;
    private PrintWriter out;

    @Override
    public void initialize(URL url, ResourceBundle rb) {
        lblResult.setVisible(false);
        txtGroupName.setText("Group");
    }    

    public void initData(String username) throws IOException{
        user = username;
    }
    
    @FXML
    private void createGroup(ActionEvent event) throws IOException, ParseException{
        String host = InetAddress.getLocalHost().getHostAddress();
        int key = 8;

        // Connect with the server
        try {
            socket = new Socket(host, AUTHPORT);
            outputStream = socket.getOutputStream();
            out = new PrintWriter(outputStream, true);
            inputStream = socket.getInputStream();
        } catch (UnknownHostException e) {
            System.err.println("Error: Unknown host " + host);
        } catch (IOException e) {
            System.err.println("Error: I/O error with server " + host);
        }

        char response = '0';
        String groupname = txtGroupName.getText();

        // Create a JSONObject instance
        JSONObject jsonObject = new JSONObject();
        jsonObject.put("service", "group");
        jsonObject.put("username", user);
        jsonObject.put("groupname", groupname);

        // Convert the JSONObject to a JSON string:
        String jsonString = jsonObject.toJSONString();

        // Encrypt JSON string
        String encryptedJson = caesarEncrypt(key, jsonString);

        // Send JSON string to server
        out.println(encryptedJson);
        out.flush();




        // Read the encrypted response from the server
        byte[] data = new byte[1024];
        int bytesRead = inputStream.read(data);
        String encryptedResp = new String(data, 0, bytesRead);

        // Decrypt the JSON string
        String decryptedJson = caesarDecrypt(key, encryptedResp);

        // Parse the decrypted JSON string
        JSONParser parser = new JSONParser();
        JSONObject responseJson = (JSONObject) parser.parse(decryptedJson);
        response = ((String) responseJson.get("result")).charAt(0);

        lblResult.setVisible(true);
        if (response == '1') {
            lblResult.setText("Group created!");
        } else if (response == '0') {
            lblResult.setText("Group already exists");
        } else {
            lblResult.setText("Server not available in this moment");
        }
    }

    public static String caesarEncrypt(int shift, String plaintext) {
        StringBuilder ciphertext = new StringBuilder();
        for (int i = 0; i < plaintext.length(); i++) {
            char c = plaintext.charAt(i);
            if (Character.isLetter(c)) {
                int base = Character.isLowerCase(c) ? 'a' : 'A';
                c = (char)(((int)c - base + shift) % 26 + base);
            }
            ciphertext.append(c);
        }
        return ciphertext.toString();
    }

    public static String caesarDecrypt(int shift, String ciphertext) {
        return caesarEncrypt(26 - shift,ciphertext);
    }
}

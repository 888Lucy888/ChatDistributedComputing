package UI;
/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

import java.io.IOException;
import java.net.URL;
import java.util.ResourceBundle;
import java.util.logging.Level;
import java.util.logging.Logger;

import javafx.event.ActionEvent;
import javafx.fxml.FXML;
import javafx.fxml.FXMLLoader;
import javafx.fxml.Initializable;
import javafx.scene.Parent;
import javafx.scene.Scene;
import javafx.scene.control.Button;
import javafx.scene.control.Label;
import javafx.scene.control.PasswordField;
import javafx.scene.control.TextField;
import javafx.scene.image.Image;
import javafx.stage.Stage;

import java.net.*;
import java.io.*;

import org.json.simple.JSONObject;
import org.json.simple.JSONValue;
import org.json.simple.parser.JSONParser;
import org.json.simple.parser.ParseException;

/**
 * FXML Controller class
 *
 */
public class LoginController implements Initializable {

    /**
     * Initializes the controller class.
     */

    @FXML
    private TextField txtUser;
    @FXML
    private PasswordField txtPass;
    @FXML
    private Button btnLogin;
    @FXML
    private Label lblError;

    private final static int AUTHPORT = 5001;
    private Socket socket;
    private OutputStream outputStream;
    private InputStream inputStream;
    private PrintWriter out;

    @Override
    public void initialize(URL url, ResourceBundle rb) {
        lblError.setVisible(false);
        txtPass.setText("pass1");
        txtUser.setText("user1");
    }

    @FXML
    private void login(ActionEvent event) throws IOException, ParseException {
        FXMLLoader loader = new FXMLLoader();

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
        String username = txtUser.getText();
        String password = txtPass.getText();

        // Create a JSONObject instance
        JSONObject jsonObject = new JSONObject();
        jsonObject.put("service", "auth");
        jsonObject.put("username", username);
        jsonObject.put("password", password);

        // Convert the JSONObject to a JSON string:
        String jsonString = jsonObject.toJSONString();
        
        // Encrypt JSON string
        String encryptedJson = caesarEncrypt(key, jsonString);
        System.out.println("encryptedJson: " + encryptedJson);

        // Send JSON string to server
        out.println(encryptedJson);
        out.flush();




        // Read the encrypted response from the server
        byte[] data = new byte[1024];
        int bytesRead = inputStream.read(data);
        String encryptedResp = new String(data, 0, bytesRead);
        System.out.println("encryptedResp: " + encryptedResp);

        // Decrypt the JSON string
        String decryptedJson = caesarDecrypt(key, encryptedResp);
        System.out.println("decryptedJson: " + decryptedJson);

        // Parse the decrypted JSON string
        JSONParser parser = new JSONParser();
        JSONObject responseJson = (JSONObject) parser.parse(decryptedJson);

        response = ((String) responseJson.get("result")).charAt(0);

        if (response == '1') {
            String token = (String) responseJson.get("token");
            System.out.println("Token: " + token);

            Stage loginScreen = (Stage) txtUser.getScene().getWindow();
            loginScreen.hide();

            loader.setLocation(getClass().getResource("Main.fxml"));

            Stage gameStage = new Stage();
            Parent root = null;
            try {
                root = loader.load();
                MainController controller = loader.getController();
                controller.initData(username);

                Scene sceneInstructions = new Scene(root);
                gameStage.setTitle("Talkie");
                gameStage.getIcons().add(new Image("file:./img/Icon.png"));
                gameStage.setScene(sceneInstructions);
                gameStage.show();
            } catch (IOException ex) {
                Logger.getLogger(SplashController.class.getName()).log(Level.SEVERE, null, ex);
            }
        } else if (response == '0') {
            lblError.setVisible(true);
            lblError.setText("User or passsword incorrect, try again");
        } else {
            lblError.setVisible(true);
            lblError.setText("Server not available in this moment");
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
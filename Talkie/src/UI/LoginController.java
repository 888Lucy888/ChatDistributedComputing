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
    private boolean firstTry;

    @Override
    public void initialize(URL url, ResourceBundle rb) {
        lblError.setVisible(false);
        txtPass.setText("pass1");
        txtUser.setText("user1");
        firstTry = true;
    }

    @FXML
    private void login(ActionEvent event) throws IOException, ParseException {
        FXMLLoader loader = new FXMLLoader();

        String host = InetAddress.getLocalHost().getHostAddress();

        // Checks if it's the first time the user tries to login to connect with the
        // server just once
        if (firstTry) {
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
        }

        char response = '0';
        String username = txtUser.getText();
        String password = txtPass.getText();

        /*
         * String message = username + ":" + password;
         * out.println(message);
         * out.flush();
         * response = (char) inputStream.read();
         */

        // Create a JSONObject instance
        JSONObject jsonObject = new JSONObject();
        jsonObject.put("username", username);
        jsonObject.put("password", password);

        // Convert the JSONObject to a JSON string:
        String jsonString = jsonObject.toJSONString();

        // Send JSON string to server
        out.println(jsonString);
        out.flush();

        // JSON
        // Receive JSON response from server
        JSONParser parser = new JSONParser();
        JSONObject responseJson = (JSONObject) parser.parse(new InputStreamReader(inputStream));
        System.out.println(responseJson);
        response = ((String) responseJson.get("result")).charAt(0);
        System.out.println(response);
        // TEST
        // response = (char) inputStream.read();

        if (response == '1') {

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
            // firstTry = false;
            lblError.setVisible(true);
            lblError.setText("User or passsword incorrect, try again");
        } else {
            lblError.setVisible(true);
            lblError.setText("Server not available in this moment");
        }
    }
}
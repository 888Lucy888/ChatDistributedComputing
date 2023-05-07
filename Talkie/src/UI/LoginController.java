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
    
    @FXML private TextField txtUser;
    @FXML private PasswordField txtPass;
    @FXML private Button btnLogin;
    @FXML private Label lblError;

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
    private void login(ActionEvent event) throws IOException{
        System.out.println("Login");
    }
}

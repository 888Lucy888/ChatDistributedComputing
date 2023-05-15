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

        /*
         * String message = username + ":" + password;
         * out.println(message);
         * out.flush();
         * response = (char) inputStream.read();
         */

        // Create a JSONObject instance
        JSONObject jsonObject = new JSONObject();
        jsonObject.put("service", "group");
        jsonObject.put("username", user);
        jsonObject.put("groupname", groupname);

        // Convert the JSONObject to a JSON string:
        String jsonString = jsonObject.toJSONString();

        out.println(jsonString);
        out.flush();

        JSONParser parser = new JSONParser();
        JSONObject responseJson = (JSONObject) parser.parse(new InputStreamReader(inputStream));
        System.out.println(responseJson);
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
}

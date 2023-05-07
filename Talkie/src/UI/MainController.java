/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package UI;

import java.io.IOException;
import java.net.URL;
import java.util.ResourceBundle;
import javafx.event.ActionEvent;
import javafx.fxml.FXML;
import javafx.fxml.Initializable;
import javafx.scene.control.Button;
/**
 * FXML Controller class
 *
 */
public class MainController implements Initializable {

    /**
     * Initializes the controller class.
     */
    
    @FXML private Button btnPlay;

    private String user;

    @Override
    public void initialize(URL url, ResourceBundle rb) {
    }    

    public void initData(String username) throws IOException{
        user = username;
    }
    
    @FXML
    private void play(ActionEvent event) throws IOException{
        System.out.println("Hello "+ user);
    }
}

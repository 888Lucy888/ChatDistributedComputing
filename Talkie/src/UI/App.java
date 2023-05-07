package UI;

import javafx.application.Application;
import javafx.fxml.FXMLLoader;
import javafx.scene.Parent;
import javafx.scene.Scene;
import javafx.scene.image.Image;
import javafx.stage.Stage;
import javafx.stage.StageStyle;

public class App extends Application {
    public static void main(String[] args) {
        launch(args);
    }

    @Override
    public void start(Stage splashScreen) throws Exception {
        Parent rootSS = FXMLLoader.load(getClass().getResource("Splash.fxml"));
        
        Scene sceneSS = new Scene(rootSS);
        
        splashScreen.setTitle("Talkie");
        splashScreen.getIcons().add(new Image("file:./img/Icon.png"));
        splashScreen.setResizable(false);
        splashScreen.setScene(sceneSS);
        splashScreen.initStyle(StageStyle.TRANSPARENT);
        splashScreen.show();
    }
}

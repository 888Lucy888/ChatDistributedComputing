#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cjson/cJSON.h>


#define PORT 5001

typedef struct {
    char * username;
    char * password;
} User;

int authenticate(int new_socket, const char * user, const char * pass) {
    User user1 = {"user1", "pass1"};
    User user2 = {"user2", "pass2"};
    int login_successful = 0;

    printf("\n%s:%s\n", user, pass);

    // Check if username and password match hardcoded users
    if (strcmp(user, user1.username) == 0 && strcmp(pass, user1.password) == 0) {
        login_successful = 1;
    } else if (strcmp(user, user2.username) == 0 && strcmp(pass, user2.password) == 0) {
        login_successful = 1;
    }

    return login_successful;
}

int main(int argc, char *argv[]) {
    int server_fd, actual_socket;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    char buffer[1024] = {0};
    int authenticated_players = 0; 

    // Creating socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // Forcefully attaching socket to the port 5001
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // Forcefully attaching socket to the port 5001
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 3) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    printf("Server listening on port %d\n", PORT);
    
    while (1) {
        // Accept incoming client connection
        if ((actual_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
            perror("accept");
            exit(EXIT_FAILURE);
        }

        // Fork a new process to handle the client connection
        pid_t pid = fork();
        if (pid == -1) {
            perror("fork failed");
            exit(EXIT_FAILURE);
        } else if (pid == 0) {
            // Child process
            close(server_fd);

            int authenticated = 0;
        
            while (authenticated == 0) {
                /*read(actual_socket, buffer, 1024);

                int len = strlen(buffer);
                if (len > 0 && buffer[len-1] == '\n') {
                    buffer[len-1] = '\0';
                    if (buffer[len-2] == '\r') {
                        buffer[len-2] = '\0';
                    }
                }

                char *user = strtok(buffer, ":");
                char *pswrd = strtok(NULL, ":");

                authenticated = authenticate(actual_socket, user, pswrd);
                printf("Login result=%s\n",  authenticated ? "success" : "failure");

                // Send authentication result to client
                if (send(actual_socket, authenticated == 1 ? "1" : "0", 1, 0) <= 0) {
                    perror("Error sending authentication result to client");
                    exit(EXIT_FAILURE);
                }
                */

                read(actual_socket, buffer, 1024);
                cJSON *json = cJSON_Parse(buffer);
                const char *username = cJSON_GetObjectItem(json, "username")->valuestring;
                const char *password = cJSON_GetObjectItem(json, "password")->valuestring;

                authenticated = authenticate(actual_socket, username, password);
                printf("Login result=%s\n",  authenticated ? "success" : "failure");

                cJSON *response = cJSON_CreateObject();
                if (authenticated == 1 ) {
                    cJSON_AddStringToObject(response, "result", "1");
                } else {
                    cJSON_AddStringToObject(response, "result", "0");
                }
                char *json_str = cJSON_Print(response);
                cJSON_Delete(response);

                printf("Json String=%s\n\n",  json_str);

                // Send authentication result to client
                if (send(actual_socket, json_str, strlen(json_str), 0) <= 0) {
                    perror("Error sending authentication result to client");
                    exit(EXIT_FAILURE);
                }
                

                // Send authentication result to client
                /*if (send(actual_socket, authenticated == 1 ? "1" : "0", 1, 0) <= 0) {
                    perror("Error sending authentication result to client");
                    exit(EXIT_FAILURE);
                }*/

                printf("Authentication result sent to client\n");

                if (authenticated == 1) {
                    break;
                }
            }

             printf("Login successful\n");
             close(actual_socket);
             exit(0);
        } else {
            // Parent process
            close(actual_socket);
        }
    }

    return 0;
}
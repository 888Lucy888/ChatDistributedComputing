#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cjson/cJSON.h>

#define PORT 5000

typedef struct
{
    char *username;
    char *password;
} User;

int xor (int a, int b) {
    int q;
    q = a ^ b;
    return q;
}

    void xor_crypt(char *key, char *data, int data_len)
{
    int key_len = strlen(key);
    for (int i = 0; i < data_len; i++)
    {
        data[i] = data[i] ^ key[i % key_len];
    }
}

int authenticate(const char *user, const char *pass)
{
    FILE *fp;
    char buffer[1024];
    int login_successful = 0;

    fp = fopen("database.txt", "r");
    if (fp == NULL)
    {
        printf("Failed to open file");
        return 0;
    }

    while (fgets(buffer, sizeof(buffer), fp))
    {
        cJSON *root, *item;
        const char *coin;

        root = cJSON_Parse(buffer);
        if (root == NULL)
        {
            printf("Failed to parse JSON\n");
            continue;
        }

        item = cJSON_GetObjectItem(root, user);
        if (cJSON_IsString(item))
        {
            coin = item->valuestring;
            printf("Coin: %s\n", coin);
            login_successful = 1;
        }

        cJSON_Delete(root);
    }

    fclose(fp);
    return login_successful;

    /*     User user1 = {"user1", "pass1"};
        User user2 = {"user2", "pass2"};
        int login_successful = 0;

        printf("\n%s:%s\n", user, pass);

        // Check if username and password match hardcoded users
        if (strcmp(user, user1.username) == 0 && strcmp(pass, user1.password) == 0)
        {
            login_successful = 1;
        }
        else if (strcmp(user, user2.username) == 0 && strcmp(pass, user2.password) == 0)
        {
            login_successful = 1;
        }

        return login_successful; */
}

int main(int argc, char *argv[])
{
    /* // ENCRYPTION TEST
    char jsonMsg[] = "{\"username\":\"user1\",\"password\":\"pass1\"}";
    int json_len = strlen(jsonMsg);
    char key[] = "secret_key";

    xor_crypt(key, jsonMsg, json_len);
    printf("Encrypted JSON: %s\n", jsonMsg);
    xor_crypt(key, jsonMsg, json_len);
    printf("Decrypted JSON: %s\n", jsonMsg);

    // Parse decrypted JSON string
    cJSON *root = cJSON_Parse(jsonMsg);
    if (root == NULL)
    {
        printf("Error parsing JSON: %s\n", cJSON_GetErrorPtr());
        return 1;
    }

    char *name = cJSON_GetObjectItem(root, "username")->valuestring;
    char *pswd = cJSON_GetObjectItem(root, "password")->valuestring;

    printf("Name: %s\nPswrd: %s\n", name, pswd);

    int authenticated = 0;
    authenticated = authenticate(name, pswd);
    printf("Login result=%s\n", authenticated ? "success" : "failure");

    cJSON *response = cJSON_CreateObject();
    if (authenticated == 1)
    {
        cJSON_AddStringToObject(response, "result", "1");
    }
    else
    {
        cJSON_AddStringToObject(response, "result", "0");
    }
    char *json_str = cJSON_Print(response);
    cJSON_Delete(response);

    printf("Json String=%s\n\n", json_str);

    int jsonR_len = strlen(json_str);
    xor_crypt(key, json_str, jsonR_len);
    printf("Encrypted JSON Response: %s\n", json_str);
    xor_crypt(key, json_str, jsonR_len);
    printf("Dencrypted JSON Response: %s\n", json_str);
    // END TEST */

    int server_fd,
        actual_socket;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    char buffer[1024] = {0};
    int authenticated_players = 0;
    char key[] = "secret_key";

    // Creating socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // Forcefully attaching socket to the port 5001
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)))
    {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // Forcefully attaching socket to the port 5001
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 3) < 0)
    {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    printf("Server listening on port %d\n", PORT);

    while (1)
    {
        // Accept incoming client connection
        if ((actual_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0)
        {
            perror("accept");
            exit(EXIT_FAILURE);
        }

        // Fork a new process to handle the client connection
        pid_t pid = fork();
        if (pid == -1)
        {
            perror("fork failed");
            exit(EXIT_FAILURE);
        }
        else if (pid == 0)
        {
            // Child process
            close(server_fd);

            int authenticated = 0;

            while (authenticated == 0)
            {
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
                // DECIFRAR MENSAJE
                // SI NO FUNCIONA: - CHECAR LENGTH DEL BUFFER, - PASAR MENSAJE DEL BUFFER A OTRA VARIABLE (USAR VARIABLE PARA LO DE ABAJO EN VEZ DE buffer) strcmpy
                int json_len = strlen(buffer);
                xor_crypt(key, buffer, json_len);

                // PARSING FOR AUTH
                cJSON *root = cJSON_Parse(buffer);
                if (root == NULL)
                {
                    printf("Error parsing JSON: %s\n", cJSON_GetErrorPtr());
                    return 1;
                }

                char *name = cJSON_GetObjectItem(root, "username")->valuestring;
                char *pswd = cJSON_GetObjectItem(root, "password")->valuestring;

                printf("Name: %s\nPswrd: %s\n", name, pswd);

                int authenticated = 0;
                authenticated = authenticate(name, pswd);
                printf("Login result=%s\n", authenticated ? "success" : "failure");

                cJSON *response = cJSON_CreateObject();
                if (authenticated == 1)
                {
                    cJSON_AddStringToObject(response, "result", "1");
                }
                else
                {
                    cJSON_AddStringToObject(response, "result", "0");
                }
                char *json_str = cJSON_Print(response);
                cJSON_Delete(response);

                printf("Json String=%s\n\n", json_str);

                // CIFRAR MENSAJE
                int jsonR_len = strlen(json_str);
                xor_crypt(key, json_str, jsonR_len);
                printf("Encrypted JSON Response: %s\n", json_str);

                // MANDAR MENSAJE JSON CIFRADO
                // Send authentication result to client
                if (send(actual_socket, json_str, strlen(json_str), 0) <= 0)
                {
                    perror("Error sending authentication result to client");
                    exit(EXIT_FAILURE);
                }

                // TEST
                // Send authentication result to client
                /*    if (send(actual_socket, authenticated == 1 ? "1" : "0", 1, 0) <= 0)
                   {
                       perror("Error sending authentication result to client");
                       exit(EXIT_FAILURE);
                   } */

                printf("Authentication result sent to client\n");

                if (authenticated == 1)
                {
                    break;
                }
                break;
            }

            printf("Login successful\n");
            close(actual_socket);
            exit(0);
        }
        else
        {
            // Parent process
            close(actual_socket);
        }
    }

    return 0;
}
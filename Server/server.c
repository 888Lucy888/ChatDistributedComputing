
#include "./functions/functions.h"

#define PORT 5000

int main(int argc, char *argv[])
{

    int server_fd,
        actual_socket;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    char buffer[1024] = {0};
    int authenticated_players = 0;
    int key = 8;

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

            printf("\n\n----------------------- New Request -----------------------\n\n");

            int encrypted_data_len = read(actual_socket, buffer, 1024);

            // Decrypt the data
            printf("encrypted_data = %s\n", buffer);

            caesarDecrypt(key, buffer);
            printf("decrypted_data = %s\n", buffer);

            int len = strlen(buffer);
            for (int i = 0; i < len; i++)
            {
                if (buffer[i] == '\r' || buffer[i] == '\n')
                {
                    buffer[i] = '\0';
                    break;
                }
            }

            // Parse the decrypted data
            cJSON *json = cJSON_Parse(buffer);
            const char *service = cJSON_GetObjectItem(json, "service")->valuestring;

            char *json_str;
            if (strcmp(service, "auth") == 0)
            {
                printf("\n********* Auth *********\n");
                json_str = microAuth(json);
            }
            else if (strcmp(service, "group") == 0)
            {
                printf("\n********* Group *********\n");
                json_str = microCrteGrp(json);
            }
            else if (strcmp(service, "addUser") == 0)
            {
                printf("\n********* Add User *********\n");
                json_str = microAddU(json);
            }
            else if (strcmp(service, "chat") == 0)
            {
                printf("\n********* Chat *********\n");
                json_str = microChat(json);
            }
            else if (strcmp(service, "msg") == 0)
            {
                printf("\n********* Message *********\n");
                json_str = microMsg(json);
            }

            printf("Json String=%s\n", json_str);
            caesarEncrypt(key, json_str);
            printf("Json String Encrypted=%s\n", json_str);

            // Send authentication result to client
            if (send(actual_socket, json_str, strlen(json_str), 0) <= 0)
            {
                perror("Error sending authentication result to client");
                exit(EXIT_FAILURE);
            }

            printf("Request successful\n");
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
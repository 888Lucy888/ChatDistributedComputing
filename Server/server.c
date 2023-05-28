
#include "./functions/functions.h"

#define PORT 5001

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
            if (strcmp(service, "connecting") == 0)
            {
                printf("\n********* Returns 1 If User Connection *********\n");
                json_str = microConnect(json);
            }
            if (strcmp(service, "auth") == 0)
            {
                printf("\n********* Authenticate User *********\n");
                json_str = microAuth(json);
            }
            if (strcmp(service, "group") == 0)
            {
                printf("\n********* Create Group *********\n");
                json_str = microCrteGrp(json);
            }
            if (strcmp(service, "addUser") == 0)
            {
                printf("\n********* Add User To Group *********\n");
                json_str = microAddU(json);
            }
            if (strcmp(service, "chat") == 0)
            {
                printf("\n********* Read Chat *********\n");
                json_str = microChat(json);
            }
            if (strcmp(service, "msg") == 0)
            {
                printf("\n********* Send Message *********\n");
                json_str = microMsg(json);
            }
            if (strcmp(service, "users") == 0)
            {
                printf("\n********* Get Users From Group *********\n");
                json_str = microUsers(json);
            }
            if (strcmp(service, "usersAdmin") == 0)
            {
                printf("\n********* Get User Data From Group If Admin*********\n");
                json_str = microUsersAdmin(json);
            }
            if (strcmp(service, "groups") == 0)
            {
                printf("\n********* Get Groups *********\n");
                json_str = microGroups(json);
            }
            if (strcmp(service, "groupsNo") == 0)
            {
                printf("\n********* Get Groups You Are Not In *********\n");
                json_str = microGroupsNo(json);
            }
            if (strcmp(service, "deleteUser") == 0)
            {
                printf("\n********* Delete User From Group *********\n");
                json_str = microDeleteU(json);
            }
            if (strcmp(service, "deleteReq") == 0)
            {
                printf("\n********* Delete Request From User To Group *********\n");
                json_str = microDeleteReq(json);
            }
            if (strcmp(service, "deleteChat") == 0)
            {
                printf("\n********* Delete Chat Group *********\n");
                json_str = microDeleteChat(json);
            }
            if (strcmp(service, "addReq") == 0)
            {
                printf("\n********* Add Request To Join Group Chat *********\n");
                json_str = microAddReq(json);
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
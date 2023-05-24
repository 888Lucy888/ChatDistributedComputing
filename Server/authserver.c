#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cjson/cJSON.h>
#include <time.h>
#include <sys/stat.h>
#include <errno.h>
#include <ctype.h>

#define PORT 5000

typedef struct
{
    char *username;
    char *password;
} User;

<<<<<<< HEAD
void caesarEncrypt(int shift, char *plaintext) {
    int i;
    for (i = 0; plaintext[i] != '\0'; i++) {
        char c = plaintext[i];
        if (isalpha(c) || isdigit(c)) {
            int base;
            if (islower(c)) {
                base = 'a';
            } else if (isupper(c)) {
                base = 'A';
            } else {
                if((int)c>52){
                    plaintext[i] = c-5;
                }else{
                    plaintext[i] = c+5;
=======
void caesarEncrypt(int shift, char *plaintext)
{
    int i;
    for (i = 0; plaintext[i] != '\0'; i++)
    {
        char c = plaintext[i];
        if (isalpha(c) || isdigit(c))
        {
            int base;
            if (islower(c))
            {
                base = 'a';
            }
            else if (isupper(c))
            {
                base = 'A';
            }
            else
            {
                if ((int)c > 52)
                {
                    plaintext[i] = c - 5;
                }
                else
                {
                    plaintext[i] = c + 5;
>>>>>>> af14d9567d76a8c0d86b897edfad2d9c40310769
                }
                continue;
            }
            c = (char)(((int)c - base + shift) % 26 + base);
        }
        plaintext[i] = c;
    }
}

<<<<<<< HEAD
void caesarDecrypt(int shift, char *ciphertext) {
    int i;
    for (i = 0; ciphertext[i] != '\0'; i++) {
        char c = ciphertext[i];
        if (isalpha(c) || isdigit(c)) {
            int base;
            if (islower(c)) {
                base = 'a';
            } else if (isupper(c)) {
                base = 'A';
            } else {
                if((int)c>52){
                    ciphertext[i] = c-5;
                }else{
                    ciphertext[i] = c+5;
=======
void caesarDecrypt(int shift, char *ciphertext)
{
    int i;
    for (i = 0; ciphertext[i] != '\0'; i++)
    {
        char c = ciphertext[i];
        if (isalpha(c) || isdigit(c))
        {
            int base;
            if (islower(c))
            {
                base = 'a';
            }
            else if (isupper(c))
            {
                base = 'A';
            }
            else
            {
                if ((int)c > 52)
                {
                    ciphertext[i] = c - 5;
                }
                else
                {
                    ciphertext[i] = c + 5;
>>>>>>> af14d9567d76a8c0d86b897edfad2d9c40310769
                }
                continue;
            }
            c = (char)(((int)c - base - shift + 26) % 26 + base);
        }
        ciphertext[i] = c;
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
<<<<<<< HEAD
            if (strcmp(coin, pass) == 0) {
=======
            if (strcmp(coin, pass) == 0)
            {
>>>>>>> af14d9567d76a8c0d86b897edfad2d9c40310769
                login_successful = 1;
            }
        }

        cJSON_Delete(root);
    }

    fclose(fp);
    return login_successful;
}

<<<<<<< HEAD
char * microAuth(cJSON *json) {
=======
char *microAuth(cJSON *json)
{
>>>>>>> af14d9567d76a8c0d86b897edfad2d9c40310769
    const char *username = cJSON_GetObjectItem(json, "username")->valuestring;
    const char *password = cJSON_GetObjectItem(json, "password")->valuestring;

    int authenticated = authenticate(username, password);
    printf("Login result=%s\n", authenticated ? "success" : "failure");

    cJSON *response = cJSON_CreateObject();
    if (authenticated == 1)
    {
        cJSON_AddStringToObject(response, "result", "1");
        time_t currentTime = time(NULL);                // get current time
        struct tm *localTime = localtime(&currentTime); // convert to local time
        char timeString[80];
        strftime(timeString, sizeof(timeString), "%Y%m%d%H%M%S", localTime); // format time as string
        printf("Current time: %s\n", timeString);
        cJSON_AddStringToObject(response, "token", timeString);
    }
    else
    {
        cJSON_AddStringToObject(response, "result", "0");
    }
    char *json_str = cJSON_PrintUnformatted(response);
    cJSON_Delete(response);
    return json_str;
}

int createGroupFiles(const char *groupname)
{
    char group_dir[30];
    char conv_file[100];
    char users_file[100];
    struct stat st;
    snprintf(group_dir, sizeof(group_dir), "./groups/%s", groupname);
    if (stat(group_dir, &st) == 0 && S_ISDIR(st.st_mode))
    {
        printf("Group directory already exists: %s\n", group_dir);
        return 0;
    }
    else
    {
        if (mkdir(group_dir, 0777) != 0)
        {
            printf("Failed to create group directory: %s\n", strerror(errno));
            exit(EXIT_FAILURE);
        }
        else
        {
            printf("Group directory created successfully: %s\n", group_dir);
        }
    }

    snprintf(conv_file, sizeof(conv_file), "%s/%s.conv", group_dir, groupname);
    snprintf(users_file, sizeof(users_file), "%s/%s.users", group_dir, groupname);
    FILE *conv_fp = fopen(conv_file, "w");
    FILE *users_fp = fopen(users_file, "w");

    if (conv_fp == NULL || users_fp == NULL)
    {
        printf("Failed to create group files: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
    else
    {
        printf("Group files created successfully: %s, %s\n", conv_file, users_file);
        fclose(conv_fp);
        fclose(users_fp);
        return 1;
    }
}

char *microGrp(cJSON *json)
{
    const char *username = cJSON_GetObjectItem(json, "username")->valuestring;
    const char *groupname = cJSON_GetObjectItem(json, "groupname")->valuestring;

    int createSuccesfully = createGroupFiles(groupname);
    printf("Create files = %s\n", createSuccesfully ? "success" : "failure");

    cJSON *response = cJSON_CreateObject();
    cJSON_AddStringToObject(response, "username", username);
    cJSON_AddStringToObject(response, "groupname", groupname);

<<<<<<< HEAD
    if (createSuccesfully == 1) {
        cJSON_AddStringToObject(response, "result", "1");
    } else {
=======
    if (createSuccesfully == 1)
    {
        cJSON_AddStringToObject(response, "result", "1");
    }
    else
    {
>>>>>>> af14d9567d76a8c0d86b897edfad2d9c40310769
        cJSON_AddStringToObject(response, "result", "0");
    }
    char *json_str = cJSON_PrintUnformatted(response);
    cJSON_Delete(response);
    return json_str;
}

<<<<<<< HEAD
=======
int getGroupFile(const char *groupname, const char *username)
{
    char group_dir[30];
    char users_dir[40];
    snprintf(group_dir, sizeof(group_dir), "./groups/%s", groupname);
    DIR *dir = opendir(group_dir);
    if (dir)
    {
        snprintf(users_dir, sizeof(users_dir), "./groups/%s/%s.users", groupname, groupname);
        FILE *file = fopen(users_dir, "r");
        if (file == NULL)
        {
            printf("Failed to open the users file.\n");
            return 0;
        }

        char line[256];
        while (fgets(line, sizeof(line), file))
        {
            if (strstr(line, username) != NULL)
            {
                printf("User '%s' found in the file.\n", username);
                fclose(file);
                return 1;
            }
        }
        closedir(dir);
    }
    else
    {
        printf("Group does not exist.\n");
        return 0;
    }
}

char *microChat(cJSON *json)
{
    char filePath[256];
    char line[256];

    const char *username = cJSON_GetObjectItem(json, "username")->valuestring;
    const char *groupname = cJSON_GetObjectItem(json, "groupname")->valuestring;

    int accessedSuccesfully = getGroupFile(groupname, username);
    printf("Obtained group chat = %s\n", accessedSuccesfully ? "success" : "failure");

    cJSON *response = cJSON_CreateObject();

    if (accessedSuccesfully == 1)
    {
        // READ CHAT FILE AND ADD TO JSON
        snprintf(filePath, sizeof(filePath), "./groups/%s/%s.conv", groupname, groupname);
        FILE *file = fopen(filePath, "r");
        if (file == NULL)
        {
            printf("Failed to open the conv file.\n");
            cJSON_AddStringToObject(response, "result", "0");
        }
        else
        {
            cJSON_AddStringToObject(response, "result", "1");
            while (fgets(line, sizeof(line), file))
            {
                // Remove trailing newline character if present
                if (line[strlen(line) - 1] == '\n')
                    line[strlen(line) - 1] = '\0';

                // Find the position of the colon character
                char *colonPosition = strchr(line, ':');
                if (colonPosition == NULL)
                {
                    printf("Invalid line format: %s\n", line);
                    continue;
                }

                // Split the line into key and value
                *colonPosition = '\0'; // Replace colon with null terminator
                char *key = line;
                char *value = colonPosition + 1;

                // Remove leading/trailing whitespace from key and value
                while (*key == ' ' || *key == '\t')
                    key++;
                while (*value == ' ' || *value == '\t')
                    value++;
                char *keyEnd = key + strlen(key) - 1;
                char *valueEnd = value + strlen(value) - 1;
                while (keyEnd >= key && (*keyEnd == ' ' || *keyEnd == '\t'))
                    *keyEnd-- = '\0';
                while (valueEnd >= value && (*valueEnd == ' ' || *valueEnd == '\t'))
                    *valueEnd-- = '\0';

                // Create cJSON string object with the key and value
                cJSON_AddItemToObject(response, key, cJSON_CreateString(value));
            }
        }
    }
    else
    {
        cJSON_AddStringToObject(response, "result", "0");
    }

    char *json_str = cJSON_PrintUnformatted(response);
    cJSON_Delete(response);
    return json_str;
}

char *microMsg(cJSON *json)
{
    char filePath[256];
    char line[256];

    const char *username = cJSON_GetObjectItem(json, "username")->valuestring;
    const char *groupname = cJSON_GetObjectItem(json, "groupname")->valuestring;
    const char *message = cJSON_GetObjectItem(json, "message")->valuestring;

    int accessedSuccesfully = getGroupFile(groupname, username);
    printf("Obtained group chat = %s\n", accessedSuccesfully ? "success" : "failure");

    cJSON *response = cJSON_CreateObject();

    if (accessedSuccesfully == 1)
    {
        // ADD TO CHAT FILE
        snprintf(filePath, sizeof(filePath), "./groups/%s/%s.conv", groupname, groupname);
        FILE *file = fopen(filePath, "a");
        cJSON_AddStringToObject(response, "result", "1");
        fprintf(file, "%s: %s\n", username, message);
        fclose(file);
    }
    else
    {
        cJSON_AddStringToObject(response, "result", "0");
    }

    char *json_str = cJSON_PrintUnformatted(response);
    cJSON_Delete(response);
    return json_str;
}

>>>>>>> af14d9567d76a8c0d86b897edfad2d9c40310769
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

            printf("\n\nNew....\n");

            int encrypted_data_len = read(actual_socket, buffer, 1024);
<<<<<<< HEAD
            
=======

>>>>>>> af14d9567d76a8c0d86b897edfad2d9c40310769
            // Decrypt the data
            printf("encrypted_data = %s\n", buffer);

            caesarDecrypt(key, buffer);
            printf("decrypted_data = %s\n", buffer);
<<<<<<< HEAD
            
            int len = strlen(buffer);
            for (int i = 0; i < len; i++) {
                if (buffer[i] == '\r' || buffer[i] == '\n') {
=======

            int len = strlen(buffer);
            for (int i = 0; i < len; i++)
            {
                if (buffer[i] == '\r' || buffer[i] == '\n')
                {
>>>>>>> af14d9567d76a8c0d86b897edfad2d9c40310769
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
                printf("\n\n********* Auth *********\n");
                json_str = microAuth(json);
            }
            else if (strcmp(service, "group") == 0)
            {
                printf("\n\n********* Group *********\n");
                json_str = microGrp(json);
            }
<<<<<<< HEAD
            
            
            printf("Json String=%s\n\n", json_str);
            caesarEncrypt(key, json_str);
            printf("Json String Encrypted=%s\n\n", json_str);
            
=======
            else if (strcmp(service, "chat") == 0)
            {
                printf("\n\n********* Chat *********\n");
                json_str = microChat(json);
            }
            else if (strcmp(service, "msg") == 0)
            {
                printf("\n\n********* Message *********\n");
                json_str = microMsg(json);
            }

            printf("Json String=%s\n\n", json_str);
            caesarEncrypt(key, json_str);
            printf("Json String Encrypted=%s\n\n", json_str);
>>>>>>> af14d9567d76a8c0d86b897edfad2d9c40310769

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
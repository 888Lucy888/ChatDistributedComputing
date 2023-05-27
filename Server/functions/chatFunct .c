#include "functions.h"

int getGroupFile(const char *groupname, const char *username)
{
    char group_dir[30];
    char users_dir[40];
    snprintf(group_dir, sizeof(group_dir), "./db/groups/%s", groupname);
    DIR *dir = opendir(group_dir);
    if (dir)
    {
        snprintf(users_dir, sizeof(users_dir), "./db/groups/%s/%s.users", groupname, groupname);
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
                closedir(dir);
                return 1;
            }
        }
        fclose(file);
        closedir(dir);
    }
    else
    {
        printf("Group does not exist.\n");
        return 0;
    }
    return 0;
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
    cJSON *chatArray = cJSON_CreateArray();

    if (accessedSuccesfully == 1)
    {
        // READ CHAT FILE AND ADD TO JSON
        snprintf(filePath, sizeof(filePath), "./db/groups/%s/%s.conv", groupname, groupname);
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
                char *user = line;
                char *message = colonPosition + 1;

                // Remove leading/trailing whitespace from user and message
                while (*user == ' ' || *user == '\t')
                    user++;
                while (*message == ' ' || *message == '\t')
                    message++;
                char *userEnd = user + strlen(user) - 1;
                char *messageEnd = message + strlen(message) - 1;
                while (userEnd >= user && (*userEnd == ' ' || *userEnd == '\t'))
                    *userEnd-- = '\0';
                while (messageEnd >= message && (*messageEnd == ' ' || *messageEnd == '\t'))
                    *messageEnd-- = '\0';

                // Create cJSON object for the chat entry
                cJSON *chatEntry = cJSON_CreateObject();
                cJSON_AddStringToObject(chatEntry, "user", user);
                cJSON_AddStringToObject(chatEntry, "message", message);

                // Add the chat entry to the array
                cJSON_AddItemToArray(chatArray, chatEntry);
            }
            cJSON_AddItemToObject(response, "chats", chatArray);
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
        snprintf(filePath, sizeof(filePath), "./db/groups/%s/%s.conv", groupname, groupname);
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

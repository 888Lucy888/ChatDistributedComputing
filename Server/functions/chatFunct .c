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

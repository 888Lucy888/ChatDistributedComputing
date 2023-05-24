#include "functions.h"

int createGroupFiles(const char *groupname)
{
    char group_dir[30];
    char conv_file[100];
    char users_file[100];
    struct stat st;
    snprintf(group_dir, sizeof(group_dir), "./db/groups/%s", groupname);
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

char *microCrteGrp(cJSON *json)
{
    const char *username = cJSON_GetObjectItem(json, "username")->valuestring;
    const char *groupname = cJSON_GetObjectItem(json, "groupname")->valuestring;

    int createSuccesfully = createGroupFiles(groupname);
    printf("Create files = %s\n", createSuccesfully ? "success" : "failure");

    cJSON *response = cJSON_CreateObject();
    cJSON_AddStringToObject(response, "username", username);
    cJSON_AddStringToObject(response, "groupname", groupname);

    if (createSuccesfully == 1)
    {
        cJSON_AddStringToObject(response, "result", "1");
    }
    else
    {
        cJSON_AddStringToObject(response, "result", "0");
    }
    char *json_str = cJSON_PrintUnformatted(response);
    cJSON_Delete(response);
    return json_str;
}

char *microAddU(cJSON *json)
{
    char filePath[256];
    char line[256];

    const char *username = cJSON_GetObjectItem(json, "username")->valuestring;
    const char *groupname = cJSON_GetObjectItem(json, "groupname")->valuestring;
    const char *userToAdd = cJSON_GetObjectItem(json, "add")->valuestring;

    int accessedSuccesfully = getGroupFile(groupname, username);
    printf("Obtained group chat = %s\n", accessedSuccesfully ? "success" : "failure");

    cJSON *response = cJSON_CreateObject();

    if (accessedSuccesfully == 1)
    {
        // ADD TO USER FILE
        snprintf(filePath, sizeof(filePath), "./groups/%s/%s.users", groupname, groupname);
        FILE *file = fopen(filePath, "a");
        cJSON_AddStringToObject(response, "result", "1");
        fprintf(file, "%s\n", userToAdd);
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

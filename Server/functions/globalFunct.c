#include "functions.h"

char *microGroups(cJSON *json)
{
    const char group_dir[] = "./db/groups/";
    cJSON *response = cJSON_CreateObject();
    const char *username = cJSON_GetObjectItem(json, "username")->valuestring;

    cJSON *groupArray = cJSON_CreateArray();

    DIR *dir = opendir(group_dir);
    if (dir)
    {
        struct dirent *entry;
        while ((entry = readdir(dir)) != NULL)
        {
            if (entry->d_type == DT_DIR)
            {
                if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
                    continue;

                char users_file_path[1024];
                snprintf(users_file_path, sizeof(users_file_path), "%s/%s/%s.users", group_dir, entry->d_name, entry->d_name);
                FILE *users_file = fopen(users_file_path, "r");
                if (users_file != NULL)
                {
                    // Check if the username is in the .users file
                    int isAdmin = 0;
                    char line[256];
                    if (fgets(line, sizeof(line), users_file))
                    {
                        line[strcspn(line, "\n")] = '\0'; // Remove trailing newline character
                        if (strcmp(line, username) == 0)
                        {
                            isAdmin = 1;
                        }
                    }
                    fclose(users_file);

                    // Create cJSON object for the chat entry
                    cJSON *chatEntry = cJSON_CreateObject();
                    cJSON_AddStringToObject(chatEntry, "groupName", entry->d_name);

                    // Count the number of lines in the .users file
                    int totUsers = 0;
                    users_file = fopen(users_file_path, "r");
                    while (fgets(line, sizeof(line), users_file))
                    {
                        totUsers++;
                    }
                    fclose(users_file);

                    cJSON_AddNumberToObject(chatEntry, "totUsers", totUsers);
                    cJSON_AddNumberToObject(chatEntry, "isAdmin", isAdmin);

                    char conv_file_path[1024];
                    snprintf(conv_file_path, sizeof(conv_file_path), "%s/%s/%s.conv", group_dir, entry->d_name, entry->d_name);
                    FILE *conv_file = fopen(conv_file_path, "r");
                    if (conv_file != NULL)
                    {
                        // Count the number of lines in the .conv file
                        int totMsg = 0;
                        while (fgets(line, sizeof(line), conv_file))
                        {
                            totMsg++;
                        }
                        fclose(conv_file);

                        cJSON_AddNumberToObject(chatEntry, "totMsg", totMsg);
                    }
                    else
                    {
                        cJSON_AddNumberToObject(chatEntry, "totMsg", 0);
                    }

                    cJSON_AddItemToArray(groupArray, chatEntry);
                }
            }
        }
        closedir(dir);
    }

    cJSON_AddItemToObject(response, "groups", groupArray);

    char *json_str = cJSON_PrintUnformatted(response);
    cJSON_Delete(response);
    return json_str;
}

char *microGroupsNo(cJSON *json)
{
    const char group_dir[] = "./db/groups/";
    cJSON *response = cJSON_CreateObject();
    const char *username = cJSON_GetObjectItem(json, "username")->valuestring;

    cJSON *groupArray = cJSON_CreateArray();

    DIR *dir = opendir(group_dir);
    if (dir)
    {
        struct dirent *entry;
        while ((entry = readdir(dir)) != NULL)
        {
            if (entry->d_type == DT_DIR)
            {
                if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
                    continue;

                char users_file_path[1024];
                snprintf(users_file_path, sizeof(users_file_path), "%s/%s/%s.users", group_dir, entry->d_name, entry->d_name);
                FILE *users_file = fopen(users_file_path, "r");
                if (users_file != NULL)
                {
                    int found = 0;
                    char line[256];
                    while (fgets(line, sizeof(line), users_file))
                    {
                        if (line[strlen(line) - 1] == '\n')
                            line[strlen(line) - 1] = '\0';

                        if (strcmp(line, username) == 0)
                        {
                            found = 1;
                        }
                    }
                    if (!found)
                    {
                        char reqs_file_path[1024];
                        snprintf(reqs_file_path, sizeof(reqs_file_path), "%s/%s/%s.reqs", group_dir, entry->d_name, entry->d_name);
                        FILE *reqs_file = fopen(reqs_file_path, "r");
                        if (reqs_file != NULL)
                        {
                            fseek(reqs_file, 0L, SEEK_END);
                            long reqs_size = ftell(reqs_file);
                            fclose(reqs_file);
                            printf("Hi");

                            if (reqs_size > 0)
                            {
                                cJSON *chatEntry = cJSON_CreateObject();
                                cJSON_AddStringToObject(chatEntry, "groupName", entry->d_name);
                                cJSON_AddNumberToObject(chatEntry, "userWaiting", 1);
                                cJSON_AddItemToArray(groupArray, chatEntry);
                            }
                            else
                            {
                                cJSON *chatEntry = cJSON_CreateObject();
                                cJSON_AddStringToObject(chatEntry, "groupName", entry->d_name);
                                cJSON_AddNumberToObject(chatEntry, "userWaiting", 0);
                                cJSON_AddItemToArray(groupArray, chatEntry);
                            }
                        }
                    }
                }

                fclose(users_file);
            }
        }
    }
    closedir(dir);

    cJSON_AddItemToObject(response, "groups", groupArray);

    char *json_str = cJSON_PrintUnformatted(response);
    cJSON_Delete(response);
    return json_str;
}

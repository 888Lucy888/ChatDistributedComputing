#include "functions.h"

char *microUsers(cJSON *json)
{
    cJSON *response = cJSON_CreateObject();
    const char *username = cJSON_GetObjectItem(json, "username")->valuestring;
    FILE *fp;
    char buffer[1024];

    fp = fopen("./db/loginDB.txt", "r");
    if (fp == NULL)
    {
        printf("Failed to open file");
        cJSON_AddStringToObject(response, "result", "0");
    }
    else
    {
        cJSON *userArray = cJSON_CreateArray();
        cJSON_AddStringToObject(response, "result", "1");
        while (fgets(buffer, sizeof(buffer), fp))
        {
            cJSON *root = cJSON_Parse(buffer);
            if (root == NULL)
            {
                printf("Failed to parse JSON\n");
                continue;
            }

            cJSON *currentKey = root->child;
            while (currentKey != NULL)
            {
                if (strcmp(currentKey->string, username) != 0)
                {
                    cJSON_AddItemToArray(userArray, cJSON_CreateString(currentKey->string));
                }
                currentKey = currentKey->next;
            }

            cJSON_Delete(root);
        }
        cJSON_AddItemToObject(response, "users", userArray);
    }

    fclose(fp);

    char *json_str = cJSON_PrintUnformatted(response);
    cJSON_Delete(response);
    return json_str;
}

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
                    char line[256];
                    while (fgets(line, sizeof(line), users_file))
                    {
                        if (line[strlen(line) - 1] == '\n')
                            line[strlen(line) - 1] = '\0';

                        if (strcmp(line, username) == 0)
                        {
                            cJSON_AddItemToArray(groupArray, cJSON_CreateString(entry->d_name));
                            break;
                        }
                    }
                    fclose(users_file);
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

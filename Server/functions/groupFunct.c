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
    snprintf(users_file, sizeof(users_file), "%s/%s.reqs", group_dir, groupname);

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

int remove_directory(const char *path)
{
    DIR *dir = opendir(path);
    size_t path_len = strlen(path);
    int error = -1;

    if (dir)
    {
        struct dirent *entry;
        error = 0;

        while (!error && (entry = readdir(dir)))
        {
            char *buf;
            size_t len;

            if (!strcmp(entry->d_name, ".") || !strcmp(entry->d_name, ".."))
            {
                continue;
            }

            len = path_len + strlen(entry->d_name) + 2;
            buf = (char *)malloc(len);

            if (buf)
            {
                struct stat statbuf;

                snprintf(buf, len, "%s/%s", path, entry->d_name);

                if (!stat(buf, &statbuf))
                {
                    if (S_ISDIR(statbuf.st_mode))
                    {
                        error = remove_directory(buf);
                    }
                    else
                    {
                        error = remove(buf);
                    }
                }
                free(buf);
            }
            else
            {
                error = -1;
            }
        }
        closedir(dir);
    }

    if (!error)
    {
        error = remove(path);
    }

    return error;
}

char *microCrteGrp(cJSON *json)
{
    char filePath[256];
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
        // ADD TO USER FILE
        snprintf(filePath, sizeof(filePath), "./db/groups/%s/%s.users", groupname, groupname);
        FILE *file = fopen(filePath, "a");
        fprintf(file, "%s\n", username);
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

char *microDeleteChat(cJSON *json)
{
    const char *groupname = cJSON_GetObjectItem(json, "groupname")->valuestring;
    char filePath[256];
    snprintf(filePath, sizeof(filePath), "./db/groups/%s", groupname);

    cJSON *response = cJSON_CreateObject();

    int deletedSuccessfully = remove_directory(filePath);

    if (deletedSuccessfully == 0)
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
        snprintf(filePath, sizeof(filePath), "./db/groups/%s/%s.users", groupname, groupname);
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

char *microAddReq(cJSON *json)
{
    char filePath[256];
    char line[256];

    const char *groupname = cJSON_GetObjectItem(json, "groupname")->valuestring;
    const char *userToAdd = cJSON_GetObjectItem(json, "add")->valuestring;

    cJSON *response = cJSON_CreateObject();
    // ADD TO USER FILE
    snprintf(filePath, sizeof(filePath), "./db/groups/%s/%s.reqs", groupname, groupname);
    FILE *file = fopen(filePath, "a");
    if (file != NULL)
    {
        cJSON_AddStringToObject(response, "result", "1");
        fprintf(file, "%s\n", userToAdd);
    }
    else
    {
        cJSON_AddStringToObject(response, "result", "0");
    }
    fclose(file);

    char *json_str = cJSON_PrintUnformatted(response);
    cJSON_Delete(response);
    return json_str;
}

char *microDeleteU(cJSON *json)
{
    const char *userToDelete = cJSON_GetObjectItem(json, "deleteUser")->valuestring;
    const char *groupname = cJSON_GetObjectItem(json, "groupname")->valuestring;
    char filePath[256];
    char tempFilePath[256];

    snprintf(filePath, sizeof(filePath), "./db/groups/%s/%s.users", groupname, groupname);
    snprintf(tempFilePath, sizeof(tempFilePath), "./db/groups/%s/%s.users.tmp", groupname, groupname);

    FILE *file = fopen(filePath, "r");
    FILE *tempFile = fopen(tempFilePath, "w");

    cJSON *response = cJSON_CreateObject();

    if (file != NULL && tempFile != NULL)
    {
        char line[256];
        int userDeleted = 0;

        while (fgets(line, sizeof(line), file))
        {
            if (line[strlen(line) - 1] == '\n')
                line[strlen(line) - 1] = '\0';

            if (strcmp(line, userToDelete) == 0)
            {
                userDeleted = 1;
                continue;
            }

            fprintf(tempFile, "%s\n", line);
        }

        fclose(file);
        fclose(tempFile);

        // Replace the original file with the modified temporary file
        remove(filePath);
        rename(tempFilePath, filePath);

        if (userDeleted)
            cJSON_AddStringToObject(response, "result", "1");
        else
            cJSON_AddStringToObject(response, "result", "0");
    }
    else
    {
        cJSON_AddStringToObject(response, "result", "0");
    }

    char *json_str = cJSON_PrintUnformatted(response);
    cJSON_Delete(response);
    return json_str;
}

char *microDeleteReq(cJSON *json)
{
    const char *userToDelete = cJSON_GetObjectItem(json, "deleteUser")->valuestring;
    const char *groupname = cJSON_GetObjectItem(json, "groupname")->valuestring;
    char filePath[256];
    char tempFilePath[256];

    snprintf(filePath, sizeof(filePath), "./db/groups/%s/%s.reqs", groupname, groupname);
    snprintf(tempFilePath, sizeof(tempFilePath), "./db/groups/%s/%s.reqs.tmp", groupname, groupname);

    FILE *file = fopen(filePath, "r");
    FILE *tempFile = fopen(tempFilePath, "w");

    cJSON *response = cJSON_CreateObject();

    if (file != NULL && tempFile != NULL)
    {
        char line[256];
        int userDeleted = 0;

        while (fgets(line, sizeof(line), file))
        {
            if (line[strlen(line) - 1] == '\n')
                line[strlen(line) - 1] = '\0';

            if (strcmp(line, userToDelete) == 0)
            {
                userDeleted = 1;
                continue;
            }

            fprintf(tempFile, "%s\n", line);
        }

        fclose(file);
        fclose(tempFile);

        // Replace the original file with the modified temporary file
        remove(filePath);
        rename(tempFilePath, filePath);

        if (userDeleted)
            cJSON_AddStringToObject(response, "result", "1");
        else
            cJSON_AddStringToObject(response, "result", "0");
    }
    else
    {
        cJSON_AddStringToObject(response, "result", "0");
    }

    char *json_str = cJSON_PrintUnformatted(response);
    cJSON_Delete(response);
    return json_str;
}

char *microUsers(cJSON *json)
{
    cJSON *response = cJSON_CreateObject();
    const char *username = cJSON_GetObjectItem(json, "username")->valuestring;
    const char *groupname = cJSON_GetObjectItem(json, "groupname")->valuestring;
    char filePath[256];
    snprintf(filePath, sizeof(filePath), "./db/groups/%s/%s.users", groupname, groupname);

    FILE *file = fopen(filePath, "r");
    if (file != NULL)
    {
        cJSON *userArray = cJSON_CreateArray();
        char line[256];

        while (fgets(line, sizeof(line), file))
        {
            if (line[strlen(line) - 1] == '\n')
                line[strlen(line) - 1] = '\0';

            if (strcmp(line, username) == 0)
                continue;

            cJSON *userObj = cJSON_CreateObject();
            cJSON_AddStringToObject(userObj, "username", line);
            cJSON_AddItemToArray(userArray, userObj);
        }

        fclose(file);

        cJSON_AddItemToObject(response, "users", userArray);
    }

    char *json_str = cJSON_PrintUnformatted(response);
    cJSON_Delete(response);
    return json_str;
}

char *microUsersAdmin(cJSON *json)
{
    cJSON *response = cJSON_CreateObject();
    const char *username = cJSON_GetObjectItem(json, "username")->valuestring;
    const char *groupname = cJSON_GetObjectItem(json, "groupname")->valuestring;
    const char usersPath[] = "./db/loginDB.txt";
    char filePath[256];
    snprintf(filePath, sizeof(filePath), "./db/groups/%s/%s.users", groupname, groupname);

    FILE *fp;
    char buffer[1024];

    fp = fopen("./db/loginDB.txt", "r");
    if (fp == NULL)
    {
        printf("Failed to open file");
        return NULL;
    }

    cJSON *userArray = cJSON_CreateArray();

    while (fgets(buffer, sizeof(buffer), fp))
    {
        cJSON *root;
        root = cJSON_Parse(buffer);
        if (root == NULL)
        {
            printf("Failed to parse JSON\n");
            continue;
        }

        // Iterate over each key in the JSON object
        cJSON *item = NULL;
        cJSON_ArrayForEach(item, root)
        {
            if (cJSON_IsString(item))
            {
                const char *user = item->string;
                cJSON *userObj = cJSON_CreateObject();
                cJSON_AddStringToObject(userObj, "username", user);

                // Check if user is in the .users file
                FILE *usersFile = fopen(filePath, "r");
                int belongGroup = 0;
                if (usersFile != NULL)
                {
                    char userLine[256];
                    while (fgets(userLine, sizeof(userLine), usersFile))
                    {
                        if (userLine[strlen(userLine) - 1] == '\n')
                            userLine[strlen(userLine) - 1] = '\0';

                        if (strcmp(userLine, user) == 0)
                        {
                            belongGroup = 1;
                            break;
                        }
                    }
                    fclose(usersFile);
                }
                cJSON_AddNumberToObject(userObj, "belongGroup", belongGroup);

                // Check if user is in the .reqs file
                char reqsPath[1024];
                snprintf(reqsPath, sizeof(reqsPath), "./db/groups/%s/%s.reqs", groupname, groupname);
                FILE *reqsFile = fopen(reqsPath, "r");
                int requestEnter = 0;
                if (reqsFile != NULL)
                {
                    char reqsLine[256];
                    while (fgets(reqsLine, sizeof(reqsLine), reqsFile))
                    {
                        if (reqsLine[strlen(reqsLine) - 1] == '\n')
                            reqsLine[strlen(reqsLine) - 1] = '\0';

                        if (strcmp(reqsLine, user) == 0)
                        {
                            requestEnter = 1;
                            break;
                        }
                    }
                    fclose(reqsFile);
                }
                cJSON_AddNumberToObject(userObj, "requestEnter", requestEnter);

                cJSON_AddItemToArray(userArray, userObj);
            }
        }

        cJSON_Delete(root);
    }

    fclose(fp);

    cJSON_AddItemToObject(response, "users", userArray);

    char *json_str = cJSON_PrintUnformatted(response);
    cJSON_Delete(response);
    return json_str;
}

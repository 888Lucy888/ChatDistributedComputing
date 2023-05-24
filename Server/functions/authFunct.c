#include "functions.h"

int authenticate(const char *user, const char *pass)
{
    FILE *fp;
    char buffer[1024];
    int login_successful = 0;

    fp = fopen("./db/loginDB.txt", "r");
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
            if (strcmp(coin, pass) == 0)
            {
                login_successful = 1;
            }
        }

        cJSON_Delete(root);
    }

    fclose(fp);
    return login_successful;
}


char *microAuth(cJSON *json)
{
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

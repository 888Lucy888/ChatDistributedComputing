#ifndef FUNCTIONS_H
#define FUNCTIONS_H

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

typedef struct
{
    char *username;
    char *password;
} User;

// Ecnrypt Functions
void caesarEncrypt(int shift, char *plaintext);
void caesarDecrypt(int shift, char *ciphertext);

// Auth Functions
int authenticate(const char *user, const char *pass);
char *microAuth(cJSON *json);

// Group Functions
int createGroupFiles(const char *groupname);
int getGroupFile(const char *groupname, const char *username);
char *microCrteGrp(cJSON *json);
char *microAddU(cJSON *json);
char *microAddReq(cJSON *json);
char *microDeleteU(cJSON *json);
char *microDeleteReq(cJSON *json);
char *microDeleteChat(cJSON *json);

// Chat Functions
char *microChat(cJSON *json);
char *microMsg(cJSON *json);

// Global Functions
char *microUsers(cJSON *json);
char *microUsersAdmin(cJSON *json);
char *microGroups(cJSON *json);
char *microGroupsNo(cJSON *json);
char *microConnect(cJSON *json);

#endif
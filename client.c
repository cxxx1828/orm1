/* 
2.Na strani klijenta(3.0):
	*-Kreirajte klijentski program koji se povezuje na server.
	*-Primate slucajan broj od servera.
	*-Dozvolite korisniku da pogadja da li je broj paran ili neparan.
	*-Posaljite pretpostavku serveru.
	*-Primate povratnu informaciju od servera koja ukazuje da li je pretpostavka tacna ili ne.
	-Prikazuje rezultate i pitajte korisnika zeli li da igra jos jedan krug.
*/

#include<stdio.h>      //printf
#include<string.h>     //strlen
#include<sys/socket.h> //socket
#include<arpa/inet.h>  //inet_addr
#include <fcntl.h>     //for open
#include <unistd.h>    //for close

#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT   27015

int main(int argc , char *argv[])
{
    int sock;
    struct sockaddr_in server;

    //Create socket
    sock = socket(AF_INET , SOCK_STREAM , 0);
    if (sock == -1)
    {
        printf("Could not create socket");
    }
    puts("Socket created");

    server.sin_addr.s_addr = inet_addr("127.0.0.1");
    server.sin_family = AF_INET;
    server.sin_port = htons(DEFAULT_PORT);

    //Connect to remote server
    //Kreirajte klijentski program koji se povezuje na server.

    if (connect(sock , (struct sockaddr *)&server , sizeof(server)) < 0)
    {
        perror("connect failed. Error");
        return 1;
    }

    puts("Connected\n");

    int read_size;

    char server_message[DEFAULT_BUFLEN];
    char server_message2[DEFAULT_BUFLEN];
    char server_message3[DEFAULT_BUFLEN];

    read_size = recv(sock, server_message, DEFAULT_BUFLEN, 0);
    if (read_size > 0) {
        server_message[read_size] = '\0'; // Završni karakter
        printf("Poruka od servera: %s\n", server_message);
    }

    //Dozvolite korisniku da pogadja da li je broj paran ili neparan.
    int a;
    printf("Unesite 1 ako mislite da je broj paran i 2 ako mislite da je broj neparan: \n");
    scanf("%d", &a);

    char str[512];

    sprintf(str, "%d", a);

    //Posaljite pretpostavku serveru.   
    //Send some data
    if( send(sock , str , strlen(str), 0) < 0)
    {
        puts("Send failed");
        return 1;
    }

    //Primate povratnu informaciju od servera koja ukazuje da li je pretpostavka tacna ili ne.
    while( (read_size = recv(sock , server_message2 , DEFAULT_BUFLEN , 0)) > 0 )
    {
        //perror("Tajni broj: %c",server_message[0]);
        printf("Vasa uneta pretpostavka je: \n");
        printf("%s",server_message2);
    }

    //Prikazuje rezultate i pitajte korisnika zeli li da igra jos jedan krug.
    while( (read_size = recv(sock , server_message3 , DEFAULT_BUFLEN , 0)) > 0 )
    {
        //perror("Tajni broj: %c",server_message[0]);
        printf("Ukupan broj tacnih pretpostavki je: \n");
        printf("%s",server_message3);
    }

    int b;
    printf("Unesite 1 ako zelite da nastavite da igrate jos jedan krug: \n");
    scanf("%d", &b);

    char str2[512];

    sprintf(str2, "%d", b);

    close(sock);

    return 0;
}


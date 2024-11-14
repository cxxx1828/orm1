/* 
  1.Na strani servera (3.0):
	*-Kreirajte server program koji slusa dolazne klijentske konekcije.
	*-Generisite slucajan broj (izmedju odredjenog opsega, na primer, od 1 do 100) na serveru.
	*-Prihvatajte klijentske konekcije i komunicirajte sa povezanim klijentima (server interno cuva generisani broj za svakog klijenta ponaosob kao tajnu).
	*-Primate klijentovu pretpostavku (paran ili neparan) i proverite da li je ta pretpostavka tacna.
	*-Vodite evidenciju o broju tacnih pretpostavki.
	*-Dozvolite vise klijenata da igraju istovremeno i cuvajte rezultate za svakog klijenta.
	
*/

#include<stdio.h>
#include<string.h>    //strlen
#include<sys/socket.h>
#include<arpa/inet.h> //inet_addr
#include<unistd.h>    //write
#include<pthread.h>
#include<stdlib.h>

#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT   27015



void* obrada(void* param){
    int read_size;
    char *client_message=malloc(512*sizeof(char));
    int client_sock=*(int*)param;
    int tajni_broj=rand()% 100 + 1;
    int broj_tacnih=0;
    char *message1=("Tacno! \n");
    char *message2=("Netacno! \n");
    
    //client_message[read_size] = '\0'; // Zatvaranje stringa
    char secret_msg[100];
    sprintf(secret_msg, "Vas tajni broj je: %d\n", tajni_broj);
    send(client_sock, secret_msg, strlen(secret_msg), 0);

    //primio je broj koji je klijent pogodio
    while( (read_size = recv(client_sock , client_message , DEFAULT_BUFLEN , 0)) > 0 )
    {
        int pretpostavka = atoi(client_message);
        //Primate klijentovu pretpostavku (paran ili neparan) i proverite da li je ta pretpostavka tacna.
        if((pretpostavka==1 && tajni_broj%2==0) || (pretpostavka==2 && tajni_broj%2!=0)){
            //paran
            //Vodite evidenciju o broju tacnih pretpostavki.
            broj_tacnih++;
            if( send(client_sock , message1 , strlen(message1), 0) < 0)
            {
                puts("Send failed");
                break;
            }
        }else{
            //neparan
            if( send(client_sock , message2 , strlen(message2), 0) < 0)
            {
                puts("Send failed");
                break;
            }
        }

    }

    if(read_size == 0)
    {
        char *goodbye_message = "Veza je prekinuta. Zatvaranje veze.\n";
        
        // Posaljite klijentu poruku o prekidu veze
        if (send(client_sock, goodbye_message, strlen(goodbye_message), 0) < 0)
        {
            puts("Failed to send closure message to client.");
        }
        
        printf("Klijent se diskonektovao i ukupan broj tacnih pretpostavki je %d \n", broj_tacnih);
    }
    else if(read_size == -1)
    {
        perror("recv failed");
    }
    
    char str[512];

    sprintf(str, "%d", broj_tacnih);
     
    if( send(client_sock , str , strlen(str), 0) < 0)
    {
        puts("Send failed");
        
    }

    close(client_sock);
    free(client_message);
    return 0;
}

int main(int argc , char *argv[])
{
    srand(time(NULL));
    int socket_desc , client_sock , c;
    struct sockaddr_in server , client;
    //char client_message[DEFAULT_BUFLEN];
    int broj_klijenata=0;
    pthread_t zahtevi[5];

    //Create socket
    socket_desc = socket(AF_INET , SOCK_STREAM , 0);
    if (socket_desc == -1)
    {
        printf("Could not create socket");
    }
    puts("Socket created");

    //Prepare the sockaddr_in structure
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(DEFAULT_PORT);

    //Bind
    if( bind(socket_desc,(struct sockaddr *)&server , sizeof(server)) < 0)
    {
        //print the error message
        perror("bind failed. Error");
        return 1;
    }
    puts("bind done");

    //Listen
    //Kreirajte server program koji slusa dolazne klijentske konekcije.
    listen(socket_desc , 5);

    //Accept and incoming connection
    puts("Waiting for incoming connections...");
    c = sizeof(struct sockaddr_in);

    //-Generisite slucajan broj (izmedju odredjenog opsega, na primer, od 1 do 100) na serveru.
    //int a;
    //a=rand()% 100 + 1;

    //Prihvatajte klijentske konekcije i komunicirajte sa povezanim klijentima (server interno cuva generisani broj za svakog klijenta ponaosob kao tajnu).
    //accept connection from an incoming client
    while(broj_klijenata<5){
        client_sock = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c);
        if (client_sock < 0)
        {
            perror("accept failed");
            return 1;
        }
        puts("Connection accepted");

        int* new_sock = malloc(sizeof(int));
        *new_sock = client_sock;

        if (new_sock == NULL) {
            perror("Memory allocation failed");
            close(client_sock);
            continue;
        }

        pthread_create(&zahtevi[broj_klijenata],NULL,obrada,(void*)&new_sock);
        broj_klijenata++;

        pthread_detach(zahtevi[broj_klijenata-1]);
    }

    for(int i=0;i<5;i++){
        pthread_join(zahtevi[i],NULL);
    }
    
    close(socket_desc);
    return 0;
}


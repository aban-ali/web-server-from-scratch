#include <sys/socket.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>

#define PORT 8008
#define BUFFER_SIZE 8192

struct formData {
    char name[64];
    char phone[20];
};

char *Employees[] = {"Virat Kohli", "Lamine Yamal", "Tiger Woods", "Mike Tyson", "Roger Federer"};

void get_mimetype(const char* file_path, char* mimetype){
    const char* ext = strrchr(file_path, '.');
        if(ext == NULL){
            strcpy(mimetype, "application/octet-stream");
            return;
        }
        if(strcmp(ext, ".html")==0 || strcmp(ext, ".htm")==0)
            strcpy(mimetype, "text/html");
        else if(strcmp(ext, ".jpg")==0 || strcmp(ext, ".jpeg")==0)
            strcpy(mimetype, "image/jpeg");
        else if(strcmp(ext, ".css")==0)
            strcpy(mimetype, "text/css");
        else if(strcmp(ext, ".js")==0)
            strcpy(mimetype, "text/javascript");
        else
            strcpy(mimetype, "application/octet-stream");
}

void handle_get_request(int c_socket, const char* request){
    char file_path[128];

    sscanf(request, "GET /%s", &file_path[4]);
    if(strncmp(&file_path[4], "HTTP", 4)==0)
        strcpy(&file_path[4], "index.html");
    memcpy(file_path, "www/", 4);

    FILE* file = fopen(file_path, "rb");
    char response[BUFFER_SIZE];

    if(file != NULL){
        char mimetype[128];

        fseek(file, 0, SEEK_END);
        long file_size = ftell(file);
        fseek(file, 0, SEEK_SET);
        get_mimetype(file_path, mimetype);

        sprintf(response, "HTTP/1.1 200 OK\r\nContent-Type: %s\r\nContent-Length: %ld\r\n\r\n", mimetype, file_size);
        send(c_socket, response, strlen(response), 0);

        while(!feof(file)){
            size_t bytes_read = fread(response, 1, BUFFER_SIZE, file);
            send(c_socket, response, bytes_read, 0);
        }

        fclose(file);
    } else {
        FILE* file = fopen("www/error.html", "rb");

        char* not_found_response = "HTTP/1.1 404 Not Found\r\nContent-Type: text/html\r\n\r\n";
        send(c_socket, not_found_response, strlen(not_found_response), 0);

        while(!feof(file)){
            size_t bytes_read = fread(response, 1, BUFFER_SIZE, file);
            send(c_socket, response, bytes_read, 0);
        }
        fclose(file);
    }
}

void get_formData(struct formData* d, char* body) {
    char *name_start = strstr(body, "\"name\":\"");
    char *phone_start = strstr(body, "\"phone\":\"");

    if (!name_start || !phone_start) return;

    name_start += 8;
    char *name_end = strchr(name_start, '\"');
    phone_start += 9;
    char *phone_end = strchr(phone_start, '\"');

    int name_len = name_end - name_start;
    int phone_len = phone_end - phone_start;

    memcpy(d->name, name_start, name_len);
    d->name[name_len] = '\0';

    memcpy(d->phone, phone_start, phone_len);
    d->phone[phone_len] = '\0';
}

void handle_post_request(int c_socket, const char* request){
    char *body = strstr(request, "\r\n\r\n");
    if (body != NULL) {
        body += 4;
    }
    struct formData data;
    get_formData(&data, body);

    char response[BUFFER_SIZE], payload[BUFFER_SIZE];
    int token = rand();

    snprintf(payload, BUFFER_SIZE,
    "<div>\n"
    "\t<h3><u>Enquiry Details</u></h3>\n"
    "\t<p>Name of Customer: %s</p>\n"
    "\t<p>Phone number of Customer: %s</p>\n"
    "\t<p>Token Number: %ld</p>\n"
    "\t<p>Enquiry Assigned to: %s</p>\n"
    "\t<hr><br>Dear Customer your query will be resolved before 24 hours.<br>Thank you for your Patient.\n"
    "</div>\n\0", data.name, data.phone, token, Employees[token%5]);

    snprintf(response, BUFFER_SIZE, "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Length: %zu\r\n\r\n%s\0", strlen(payload), payload);
    send(c_socket, response, strlen(response), 0);
}

void* handle_thread(void* arg){
    int client_sockfd = (int)(intptr_t)arg;
    char buffer[BUFFER_SIZE];
    ssize_t bytes_received = recv(client_sockfd, buffer, BUFFER_SIZE, 0);
    buffer[bytes_received]='\0';

    printf("Received request : \n%s\n", buffer);
    printf("--------------------------------------------------------------------------------------\n");
    if(bytes_received>0){
        if(strncmp(buffer, "GET", 3)==0)
            handle_get_request(client_sockfd, buffer);
        else if(strncmp(buffer, "POST", 4)==0)
            handle_post_request(client_sockfd, buffer);
    }
    close(client_sockfd);
    return NULL;
}


int main(){
    srand(time(NULL));
    struct sockaddr_in server_addr;

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sockfd==-1){
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }
    if(bind(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr))==-1){
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    // start listening to traffic.
    if(listen(sockfd, 10)==-1){
        perror("listen failed");
        exit(EXIT_FAILURE);
    }
    printf("Server started listening on port : %d\n", PORT);

    while(1){
        struct sockaddr_in client_addr;
        char buffer[1024];
        socklen_t client_addr_len = sizeof(client_addr);

        memset(&client_addr, 0, sizeof(client_addr));
        int client_sockfd = accept(sockfd, (struct sockaddr *)&client_addr, &client_addr_len);

        printf("New client with address : %s and port : %d connected with socket fd : %d\n",\
            inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port), client_sockfd);

        
        pthread_t thread_id;
        pthread_create(&thread_id, NULL, handle_thread, (void*)(intptr_t)client_sockfd);
        pthread_detach(thread_id);
    }

    return 0;
}
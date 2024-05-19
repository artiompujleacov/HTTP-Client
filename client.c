#include <stdio.h>      /* printf, sprintf */
#include <stdlib.h>     /* exit, atoi, malloc, free */
#include <unistd.h>     /* read, write, close */
#include <string.h>     /* memcpy, memset */
#include <sys/socket.h> /* socket, connect */
#include <netinet/in.h> /* struct sockaddr_in, struct sockaddr */
#include <netdb.h>      /* struct hostent, gethostbyname */
#include <arpa/inet.h>
#include <stdbool.h>
#include "helpers.h"
#include "requests.h"
#include "buffer.h"
#include "parson.h"

#define SERVER_IP "34.246.184.49"
#define SERVER_PORT 8080


//functia de extragere a corpului json din raspuns
char* extract_json_body(const char* response) {
    const char *body_start = strstr(response, "\r\n\r\n");
    if (body_start) {
        body_start += 4;
        return strdup(body_start);
    }
    return NULL;
}

int main(void)
{
    char *message;
    char *response;
    int sockfd, Logged_in = 0;
    char command[20];
    JSON_Value *root_value;
    JSON_Object *root_object;
    char *token, *serialized_string = NULL;
    char *cookie = malloc(LINELEN * sizeof(char));
    while (1)
    {
        fgets(command, 20, stdin);

        if (strcmp(command, "register\n") == 0)
        {
            if (Logged_in == 1)
            {
                printf("Esti deja logat!\n");
                continue;
            }
            char **data = malloc(1 * sizeof(char *));
            data[0] = malloc(LINELEN * sizeof(char));

            root_value = json_value_init_object();
            root_object = json_value_get_object(root_value);

            char username[LINELEN];
            char password[LINELEN];

            printf("username=");
            scanf("%s", username);
            printf("password=");
            scanf("%s", password);

            sockfd = open_connection(SERVER_IP, SERVER_PORT, AF_INET, SOCK_STREAM, 0);

            json_object_set_string(root_object, "username", username);
            json_object_set_string(root_object, "password", password);

            serialized_string = json_serialize_to_string_pretty(root_value);
            memcpy(data[0], serialized_string, strlen(serialized_string));

            message = compute_post_request(SERVER_IP, "/api/v1/tema/auth/register", "application/json", data, 1, NULL, 0, NULL, 0);

            send_to_server(sockfd, message);
            response = receive_from_server(sockfd);

            if (strstr(response, "error") != NULL)
            {
                printf("%s\n", "Username-ul e deja folosit!");
            }
            else
            {
                printf("%s\n", "Utilizator inregistrat cu succes!");
            }

            json_free_serialized_string(serialized_string);
            json_value_free(root_value);

            root_value = NULL;
            root_object = NULL;

            free(data[0]);
            free(data);

            close_connection(sockfd);
        }
        else if (strcmp(command, "login\n") == 0)
        {
            if (Logged_in == 1)
            {
                printf("Esti deja logat!\n");
                continue;
            }

            char **data = malloc(1 * sizeof(char *));
            data[0] = malloc(LINELEN * sizeof(char));

            char username[LINELEN];
            char password[LINELEN];

            serialized_string = NULL;
            root_value = json_value_init_object();
            root_object = json_value_get_object(root_value);

            printf("username=");
            scanf("%s", username);
            printf("password=");
            scanf("%s", password);

            sockfd = open_connection(SERVER_IP, SERVER_PORT, AF_INET, SOCK_STREAM, 0);

            json_object_set_string(root_object, "username", username);
            json_object_set_string(root_object, "password", password);

            serialized_string = json_serialize_to_string_pretty(root_value);

            memcpy(data[0], serialized_string, strlen(serialized_string));
            message = compute_post_request(SERVER_IP, "/api/v1/tema/auth/login", "application/json", data, 1, NULL, 0, NULL, 0);

            send_to_server(sockfd, message);
            response = receive_from_server(sockfd);

            if (strstr(response, "error") != NULL)
            {
                if (strstr(response, "account") != NULL)
                {
                    printf("%s\n", "Acest username nu exista!");
                }
                else
                {
                    printf("%s\n", "Credentialele nu sunt bune!");
                }
            }
            else
            {
                printf("%s\n", "Utilizatorul a fost logat cu succes!");
                Logged_in = 1;
                char *cookieHeader = strstr(response, "Set-Cookie:");
                if (cookieHeader != NULL)
                {
                    cookieHeader += strlen("Set-Cookie:");
                    char *endOfCookie = strchr(cookieHeader, ';');
                    if (endOfCookie != NULL)
                    {
                        size_t cookieLength = endOfCookie - cookieHeader;
                        cookie = malloc((cookieLength + 1) * sizeof(char));
                        strncpy(cookie, cookieHeader, cookieLength);
                        cookie[cookieLength] = '\0';
                    }
                }
            }

            free(data[0]);
            free(data);
            json_free_serialized_string(serialized_string);
            json_value_free(root_value);

            close_connection(sockfd);
        }
        else if (strcmp(command, "enter_library\n") == 0)
        {
            if (Logged_in == 0)
            {
                printf("Nu esti logat!\n");
                continue;
            }

            sockfd = open_connection(SERVER_IP, SERVER_PORT, AF_INET, SOCK_STREAM, 0);

            char **data = malloc(1 * sizeof(char *));
            data[0] = malloc(LINELEN * sizeof(char));
            memcpy(data[0], "enter_library", 14);

            char **cookies = calloc(1, sizeof(char *));
            cookies[0] = calloc(LINELEN, sizeof(char));
            memcpy(cookies[0], cookie, strlen(cookie));

            message = compute_get_request(SERVER_IP, "/api/v1/tema/library/access", NULL, cookies, 1, NULL, 0);
            send_to_server(sockfd, message);

            response = receive_from_server(sockfd);

            token = strstr(response, "token");
            token += strlen("token") + 3;
            char *endOfToken = strchr(token, '"');
            size_t tokenLength = endOfToken - token;
            char *tokenValue = malloc((tokenLength + 1) * sizeof(char));
            strncpy(tokenValue, token, tokenLength);
            tokenValue[tokenLength] = '\0';
            strcpy(token, tokenValue);

            char *err = strstr(response, "error");
            if (err != NULL)
            {
                printf("%s\n", "EROARE!");
            }
            else
            {
                printf("%s\n", "Succes!Utilizatorul a intrat in biblioteca");
            }

            free(tokenValue);
            free(data[0]);
            free(data);
            free(cookies[0]);
            free(cookies);

            close_connection(sockfd);
        }
        else if (strcmp(command, "get_books\n") == 0)
        {
            if (Logged_in == 0)
            {
                printf("Nu esti logat!\n");
                continue;
            }

            sockfd = open_connection(SERVER_IP, SERVER_PORT, AF_INET, SOCK_STREAM, 0);

            char **data = malloc(1 * sizeof(char *));
            data[0] = malloc(LINELEN * sizeof(char));
            memcpy(data[0], "enter_library", 14);

            char **cookies = calloc(1, sizeof(char *));
            cookies[0] = calloc(LINELEN, sizeof(char));
            memcpy(cookies[0], cookie, strlen(cookie));

            char **tokens = calloc(1, sizeof(char *));
            tokens[0] = calloc(LINELEN, sizeof(char));
            memcpy(tokens[0], "Authorization: Bearer ", 22);
            strcat(tokens[0], token);

            message = compute_get_request(SERVER_IP, "/api/v1/tema/library/books", NULL, cookies, 1, tokens, 1);
            send_to_server(sockfd, message);
            response = receive_from_server(sockfd);

            char *err = strstr(response, "error");
            if (err != NULL)
            {
                printf("%s\n", "You are not logged in!");
            }
            else
            {
                char *jsonBody = extract_json_body(response);
                printf("%s\n", jsonBody);
            }

            free(data[0]);
            free(data);

            close_connection(sockfd);
        }
        else if (strcmp(command, "get_book\n") == 0)
        {
            if (Logged_in == 0)
            {
                printf("Nu esti logat!\n");
                continue;
            }

            sockfd = open_connection(SERVER_IP, SERVER_PORT, AF_INET, SOCK_STREAM, 0);

            printf("id=");
            char id[LINELEN];
            fgets(id, LINELEN, stdin);
            id[strlen(id) - 1] = '\0';
            char **data = malloc(1 * sizeof(char *));
            data[0] = malloc(LINELEN * sizeof(char));
            memcpy(data[0], "enter_library", 14);
            char **cookies = calloc(1, sizeof(char *));
            cookies[0] = calloc(LINELEN, sizeof(char));
            memcpy(cookies[0], cookie, strlen(cookie));
            char **tokens = calloc(1, sizeof(char *));
            tokens[0] = calloc(LINELEN, sizeof(char));
            memcpy(tokens[0], "Authorization: Bearer ", 22);

            strcat(tokens[0], token);

            char *url = malloc(LINELEN * sizeof(char));
            strcpy(url, "/api/v1/tema/library/books/");
            strcat(url, id);

            message = compute_get_request(SERVER_IP, url, NULL, cookies, 1, tokens, 1);
            send_to_server(sockfd, message);
            response = receive_from_server(sockfd);

            char *err = strstr(response, "error");
            if (err != NULL)
            {
                printf("%s\n", "Eroare!");
            }
            else
            {
                char *jsonBody = extract_json_body(response);
                printf("%s\n", jsonBody);
            }

            free(data[0]);
            free(data);
            free(cookies[0]);
            free(cookies);
            free(tokens[0]);
            free(tokens);

            close_connection(sockfd);
        }
        else if (strcmp(command, "add_book\n") == 0)
        {
            if (Logged_in == 0)
            {
                printf("Nu esti logat!\n");
                continue;
            }

            sockfd = open_connection(SERVER_IP, SERVER_PORT, AF_INET, SOCK_STREAM, 0);

            char author[LINELEN];
            char genre[LINELEN];
            char publisher[LINELEN];
            char title[LINELEN];
            char page_count[LINELEN];

            printf("title=");
            fgets(title, LINELEN, stdin);
            title[strlen(title) - 1] = '\0';
            printf("author=");
            fgets(author, LINELEN, stdin);
            author[strlen(author) - 1] = '\0';
            printf("genre=");
            fgets(genre, LINELEN, stdin);
            genre[strlen(genre) - 1] = '\0';
            printf("publisher=");
            fgets(publisher, LINELEN, stdin);
            publisher[strlen(publisher) - 1] = '\0';
            printf("page_count=");
            fgets(page_count, LINELEN, stdin);

            if (atoi(page_count) == 0)
            {
                printf("Invalid page count!\n");
                continue;
            }

            JSON_Value *new_value = json_value_init_object();
            JSON_Object *new_object = json_value_get_object(new_value);
            char *serialized_string = NULL;

            json_object_set_string(new_object, "title", title);
            json_object_set_string(new_object, "author", author);
            json_object_set_string(new_object, "genre", genre);
            json_object_set_number(new_object, "page_count", atoi(page_count));
            json_object_set_string(new_object, "publisher", publisher);
            serialized_string = json_serialize_to_string_pretty(new_value);

            char **data = malloc(1 * sizeof(char *));
            data[0] = malloc(LINELEN * sizeof(char));
            memcpy(data[0], serialized_string, strlen(serialized_string));

            char **cookies = calloc(1, sizeof(char *));
            cookies[0] = calloc(LINELEN, sizeof(char));
            memcpy(cookies[0], cookie, strlen(cookie));

            char **tokens = calloc(1, sizeof(char *));
            tokens[0] = calloc(LINELEN, sizeof(char));
            memcpy(tokens[0], "Authorization: Bearer ", 22);
            strcat(tokens[0], token);

            message = compute_post_request(SERVER_IP, "/api/v1/tema/library/books", "application/json", data, 1, cookies, 1, tokens, 1);

            send_to_server(sockfd, message);
            response = receive_from_server(sockfd);

            char *err = strstr(response, "error");

            if (err != NULL)
            {
                printf("%s\n", "Eroare!");
            }
            else
            {
                printf("%s\n", "Cartea a fost adaugata cu succes!");
            }

            json_free_serialized_string(serialized_string);
            json_value_free(new_value);
            free(data[0]);
            free(data);

            close_connection(sockfd);
        }
        else if (strcmp(command, "logout\n") == 0)
        {
            if (Logged_in == 0)
            {
                printf("Nu esti logat!\n");
                continue;
            }

            sockfd = open_connection(SERVER_IP, SERVER_PORT, AF_INET, SOCK_STREAM, 0);

            char **data = malloc(2 * sizeof(char *));
            data[0] = malloc(LINELEN * sizeof(char));
            memcpy(data[0], "enter_library", 14);

            char **cookies = calloc(1, sizeof(char *));
            cookies[0] = calloc(LINELEN, sizeof(char));
            memcpy(cookies[0], cookie, strlen(cookie));

            message = compute_get_request(SERVER_IP, "/api/v1/tema/auth/logout", NULL, cookies, 1, NULL, 0);
            send_to_server(sockfd, message);

            response = receive_from_server(sockfd);

            char *err = strstr(response, "error");

            if (err != NULL)
            {
                printf("%s\n", "Eroare!");
            }
            else
            {
                printf("%s\n", "Utilizatorul s-a delogat cu succes!");
            }

            free(data[0]);
            free(data);

            Logged_in = 0;
            close_connection(sockfd);
        }
        else if (strcmp(command, "delete_book\n") == 0)
        {
            if (Logged_in == 0)
            {
                printf("You are not logged in!\n");
                continue;
            }

            sockfd = open_connection(SERVER_IP, SERVER_PORT, AF_INET, SOCK_STREAM, 0);

            printf("id=");
            char id[LINELEN];
            fgets(id, LINELEN, stdin);
            id[strlen(id) - 1] = '\0';

            char **data = malloc(1 * sizeof(char *));
            data[0] = malloc(LINELEN * sizeof(char));
            memcpy(data[0], "enter_library", 14);

            char **cookies = calloc(1, sizeof(char *));
            cookies[0] = calloc(LINELEN, sizeof(char));
            memcpy(cookies[0], cookie, strlen(cookie));

            char **tokens = calloc(1, sizeof(char *));
            tokens[0] = calloc(LINELEN, sizeof(char));
            memcpy(tokens[0], "Authorization: Bearer ", 22);
            strcat(tokens[0], token);

            char *url = malloc(LINELEN * sizeof(char));
            strcpy(url, "/api/v1/tema/library/books/");
            strcat(url, id);

            message = compute_delete_request(SERVER_IP, url, NULL, cookies, 1, tokens, 1);

            send_to_server(sockfd, message);
            response = receive_from_server(sockfd);

            char *err = strstr(response, "error");
            if (err != NULL)
            {
                printf("%s\n", "Eroare la stergere!");
            }
            else
            {
                printf("Cartea cu id %d a fost stearsa cu succes!", atoi(id));
            }

            close_connection(sockfd);
            free(data[0]);
            free(data);
        }
        else if (strcmp(command, "exit\n") == 0)
        {
            close_connection(sockfd);
            free(cookie);
            break;
        }
    }
    return 0;
}

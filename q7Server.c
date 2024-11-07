#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>

#define PORT 8080
#define BUF_SIZE 1024

int main() {
    int sockfd;
    struct sockaddr_in serv_addr, client_addr;
    socklen_t addr_len = sizeof(client_addr);
    char buffer[BUF_SIZE];
    int rows, cols;

    // Create socket
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("Socket creation error");
        exit(EXIT_FAILURE);
    }

    // Bind the socket to the port
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(PORT);

    if (bind(sockfd, (const struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }

    printf("Server is listening on port %d...\n", PORT);

    // Receive matrix dimensions
    recvfrom(sockfd, buffer, BUF_SIZE, 0, (struct sockaddr *) &client_addr, &addr_len);
    sscanf(buffer, "%d %d", &rows, &cols);
    printf("Matrix dimensions received: %d x %d\n", rows, cols);

    // Allocate memory for the matrix
    int matrix[rows][cols];
    memset(matrix, 0, sizeof(matrix));  // Initialize matrix with zeros

    // Receive each row and store it in the matrix
    for (int i = 0; i < rows; i++) {
        int bytes_received = recvfrom(sockfd, buffer, BUF_SIZE, 0, (struct sockaddr *) &client_addr, &addr_len);
        buffer[bytes_received] = '\0';  // Null-terminate the received string

        // Parse row data and store it in the matrix
        int value, j = 0;
        char *token = strtok(buffer, " ");
        while (token != NULL && j < cols) {
            value = atoi(token);
            matrix[i][j++] = value;
            token = strtok(NULL, " ");
        }
        printf("Row %d received.\n", i + 1);
    }

    // Display the complete matrix
    printf("Complete matrix received:\n");
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            printf("%d ", matrix[i][j]);
        }
        printf("\n");
    }

    close(sockfd);
    return 0;
}

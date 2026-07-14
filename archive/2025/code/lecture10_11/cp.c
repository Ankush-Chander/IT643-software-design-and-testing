#include <fcntl.h>   // for open
#include <unistd.h>  // for read, write, close
#include <stdlib.h>  // for exit
#include <stdio.h>   // for perror

#define BUF_SIZE 1024

int main(int argc, char *argv[]) {
    if (argc != 3) {
        write(STDERR_FILENO, "Usage: copy <source> <destination>\n", 35);
        exit(1);
    }

    // Open source file (read-only)
    int src = open(argv[1], O_RDONLY);
    if (src < 0) {
        perror("open source");
        exit(1);
    }

    // Open destination file (write-only, create if not exist, truncate if exist)
    int dest = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (dest < 0) {
        perror("open destination");
        close(src);
        exit(1);
    }

    char buffer[BUF_SIZE];
    ssize_t n;

    // Copy loop
    while ((n = read(src, buffer, BUF_SIZE)) > 0) {
        if (write(dest, buffer, n) != n) {
            perror("write");
            close(src);
            close(dest);
            exit(1);
        }
    }

    if (n < 0) perror("read");

    close(src);
    close(dest);
    return 0;
}


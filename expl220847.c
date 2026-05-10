//gcc -Wall -Wextra expl220847.c -o expl220847
#define _GNU_SOURCE
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/mman.h>


int create_pipe(int fd[2]) { 
    // fd[0] - для чтения fd[1] - для записи
    if (pipe(fd) == -1){
        perror("pipe");
        return -1;
    }
    return 0;
}

int sz_pipe(int write_fd) {
    int size = fcntl(write_fd, F_GETPIPE_SZ);

    if (size == -1){
        perror("fcntl");
        return -1;
    }
    return size;
}

ssize_t fill_pipe(int write_fd, int size) {
    char *buffer = malloc(size);
    if (!buffer){
        perror("malloc");
        return -1;
    }
    memset(buffer, 'A', size);

    ssize_t written_bytes = write(write_fd, buffer, size);
    if (written_bytes == -1){
        perror("write");
        free(buffer);
        return -1;
    }
    free(buffer);
    return written_bytes;
}

ssize_t read_pipe(int read_fd, int size) {
    char *buffer = malloc(size);
    if (!buffer){
        perror("malloc");
        return -1;
    }
    
    ssize_t read_bytes = read(read_fd, buffer, size);
    if (read_bytes == -1){
        perror("read");
        free(buffer);
        return -1;
    }
    free(buffer);
    return read_bytes;
}

int open_ro(const char *path) {
    int file = open(path, O_RDONLY);
    if (file == -1){
        perror("open");
        return -1;
    }
    return file;
}

int splice_to_pipe(int file_fd, int pipe_write_fd, loff_t offset) {
    loff_t off = offset;
    ssize_t n = splice(file_fd, &off, pipe_write_fd, NULL, 1, 0);
    if (n == -1){
        perror("splice");
        return -1;
    }
    return 0;
}

ssize_t write_to_pipe(int pipe_write_fd, const char *data, size_t len) {
    ssize_t written = write(pipe_write_fd, data, len);
    if (written == -1){
        perror("write");
        return -1;
    }
    return written;
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        fprintf(stderr, "Usage: %s <filename> <data>\n", argv[0]);
        return 1;
    }
    int pipe_fd[2];
    if (create_pipe(pipe_fd) == -1) return 1;

    int pipe_size = sz_pipe(pipe_fd[1]);
    if (pipe_size == -1) return 1;

    ssize_t filled = fill_pipe(pipe_fd[1], pipe_size);
    if (filled == -1) return 1;

    ssize_t read_bytes = read_pipe(pipe_fd[0], pipe_size - 1);
    if (read_bytes == -1) return 1;

    const char *filename = argv[1];
    int file = open_ro(filename);
    if (file == -1) return 1;

    loff_t offset = 1;
    if (splice_to_pipe(file, pipe_fd[1], offset) == -1) return 1;

    const char *data = argv[2];
    if (write_to_pipe(pipe_fd[1], data, strlen(data)) == -1) return 1;

    close(file);
    close(pipe_fd[0]);
    close(pipe_fd[1]);

    return 0;
}
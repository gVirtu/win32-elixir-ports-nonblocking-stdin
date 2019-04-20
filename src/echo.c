#include <stdio.h>
#include <windows.h>  

#define MAX_READ 1023
#define H_STDIN GetStdHandle(STD_INPUT_HANDLE)

// Reads a single byte, advancing the `stdin_idx` pointer.
int read_byte(char* buffer, int* stdin_idx) {
    char read_char = getchar();
    fprintf(stderr, "Just read %c (%d)\n", read_char, (int) read_char);
    buffer[*stdin_idx] = read_char;
    *stdin_idx = *stdin_idx + 1;
    return 1;
}

// Reads `count` bytes to the buffer.
int read_fixed(char* buffer, int count) {
    int read_count = 0;

    while(read_count < count) {
        read_byte(buffer, &read_count);
        fprintf(stderr, "Read count %d, aiming for %d\n", read_count, count);
    }

    buffer[read_count] = 0;
    fprintf(stderr, "Done reading.\n");
    return read_count;
}

// Writes the first `count` bytes of buffer to the stdout.
int write_fixed(char* buffer, int count) {
    int write_count = 0;

    while(write_count < count) {
        char write_char = buffer[write_count++];
        fprintf(stderr, "Just wrote %c (%d)\n", write_char, (int) write_char);
        putchar(write_char);
    }

    return write_count;
}

// Writes the zero-terminated content of a buffer back to stdout, including header
void write_back(char* buffer) {
    unsigned long len = strlen(buffer);
    char size_header[2] = {(len >> 8 & 0xff), (len & 0xff)};

    write_fixed(size_header, 2);
    write_fixed(buffer, len);

    fflush(stdout);
}

// Decodes 2-byte message header containing the payload size (little endian)
int to_read_length() {
  unsigned char size_header[3];
  int r = read_fixed((char*) size_header, 2);

  if(r < 0) {
    return -1;
  }

  return (size_header[0] << 8) | size_header[1];
}

// Polls stdin every 16ms. When a message is received, it is handled immediately.
int main(char argc, char* argv[]) {
    char buffer[MAX_READ + 1];
    int read_idx;
    int pollInterval = 16;
    DWORD bytesAvailable = 0;
    int read_len = 0;

    //Poll STDIN to check how much there is to read
    while(PeekNamedPipe(H_STDIN, NULL, 0, NULL, &bytesAvailable,NULL)) {
        if(bytesAvailable > 0)
        {
            read_len = to_read_length();
            fprintf(stderr, "Should read %d\n", read_len);

            read_fixed(buffer, read_len);
            write_back(buffer);
        }
        else
        {
            // Nothing to read, stdin empty
            Sleep(pollInterval);
        }
    }

    fprintf(stderr, "Closed\n");
}
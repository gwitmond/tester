/* Tester, a bare bones lxip tester
 * Public domain.
 */

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/socket.h>
#include <errno.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <netinet/in.h>


// the main server socket.
int s;


int main(int argc, char **argv) {

  struct sockaddr_in sa;
  memset(&sa, 0, sizeof(sa));
  sa.sin_port = htons(80);
  sa.sin_family = AF_INET;
  s = socket(AF_INET, SOCK_STREAM, 0);
  if (s == -1) {
    printf("failed to open socket: errno: %d: %s\n", errno, strerror(errno));
    exit(1);
  }

  int b = bind(s, (struct sockaddr *)&sa, sizeof(sa));
  if (b == -1) {
    printf("failed to bind socket: errno: %d: %s\n", errno, strerror(errno));
    close(s);
    exit(2);
  }

  int l = listen(s, 5);
  if (l == -1) {
    printf("failed to listen: errno: %d: %s\n", errno, strerror(errno));
    close(s);
    exit(3);
  }

  while (1) {
    int fd;
    char buffer[1024];
    int length;
    int flags;
next:
    flags = fcntl(s, F_GETFL, 0);
    printf("listen socket is in NONBLOCK mode: %d\n", flags && O_NONBLOCK);
    //fcntl(s, F_SETFL, flags | O_NONBLOCK);
    flags = fcntl(s, F_GETFL, 0);
    printf("listen socket is in NONBLOCK mode: %d\n", flags && O_NONBLOCK);

    printf("waiting for connection...\n");

    struct sockaddr caddr;
    memset (&caddr, 0, sizeof(caddr));
    socklen_t l_caddr = (socklen_t)sizeof(caddr);
    fd = accept(s, &caddr, &l_caddr);
    if (fd == -1) {
      printf("failed to accept: errno: %d: %s\n", errno, strerror(errno));
      printf("NOTE: EAGAIN is %d\n", EAGAIN);
      printf("NOTE: EWOULDBLOCK is %d\n", EWOULDBLOCK);
      close(s);
      exit(4);
    }

    printf("received connection\n");

    memset(&buffer, 0, sizeof(buffer));
    length = read(fd, buffer, sizeof(buffer) -1); // leave last char \0 in case of too large request.
    if (length == -1) {
      printf("error reading client socket: %s\n", strerror(errno));
      close(fd);
      goto next;
    }

    printf("read %d bytes from socket.\n", length);
    char *message = "Got it\n";
    write(fd, message, strlen(message));

    close(fd);
    goto next;
  }
}

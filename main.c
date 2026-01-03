#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define MAX_BUF_SIZE 100

uint8_t get_leading_1s(uint8_t num) {
  uint8_t count = 0;

  for (int i = 7; i >= 0; i--) {
    if ((num >> i) & 1) {
      count++;
    } else {
      break;
    }
  }

  return count;
}

void print_bits(uint32_t num, int n) {
  for (int i = n - 1; i >= 0; i--) {
    printf("%d", (num >> i) & 1);
  }
  printf("\n");
}

uint8_t reverse_bits(uint8_t num) {
  uint8_t res = 0;

  for (int i = 0; i < 8; i++) {
    res <<= 1;
    res |= (num & 1);
    num >>= 1;
  }

  return res;
}

int main() {
  int fd = open("msg.txt", O_RDONLY);
  uint8_t buf[MAX_BUF_SIZE + 1] = {0};

  int n = read(fd, buf, MAX_BUF_SIZE);
  printf("Read: %d bytes\n", n);

  uint8_t *new_line = memchr(buf, '\n', n);

  if (new_line) {
    printf("New line at: %ld\n", (new_line - buf));
    int index = new_line - buf;
    buf[index] = '\0';
    n--;
  } else {
    buf[n] = '\0';
  }

  printf("New Buffer Length: %d\n", n);

  int i = 0;

  while (i < n) {
    uint8_t ld_byte = buf[i];
    uint8_t num_bytes = get_leading_1s(ld_byte);
    if (num_bytes == 0) {
      // Its an ascii character
      printf("U+%x\n", ld_byte);
      i += 1;
      continue;
    }

    uint8_t mask_for_ld = 0;
    for (int i = 0; i < num_bytes; i++) {
      mask_for_ld |= (1 << (7 - i));
    }

    uint8_t arr[num_bytes];
    arr[0] = ld_byte ^ mask_for_ld;

    for (int j = 1; j < num_bytes; j++) {
      uint8_t cont_byte = buf[j + i];
      arr[j] = cont_byte ^ reverse_bits(1);
    }

    uint32_t unicode = 0;

    for (int j = num_bytes - 1; j >= 0; j--) {
      uint8_t byte = arr[j];
      unicode |= byte << ((num_bytes - j - 1) * 6);
    }

    printf("U+%x\n", unicode);
    i += num_bytes;
  }

  printf("%s\n", buf);

  close(fd);
  return 0;
}

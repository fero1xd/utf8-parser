#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#define MAX_BUF_SIZE 2400
#define MASK_FOR_CD (0x3F)
#define GET_LEADING_1s(num)                                                    \
  ((uint8_t)(((num) == 0xFF) ? 8 : __builtin_clz((uint32_t)(~(num)) << 24)))

static inline void print_bits(uint32_t num, int n) {
  for (int i = n - 1; i >= 0; i--) {
    printf("%d", (num >> i) & 1);
  }
  printf("\n");
}

static inline uint8_t reverse_bits(uint8_t num) {
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

  // if (new_line) {
  //   printf("New line at: %ld\n", (new_line - buf));
  //   int index = new_line - buf;
  //   buf[index] = '\0';
  //   n--;
  // } else {
  buf[n] = '\0';

  printf("New Buffer Length: %d\n", n);

  int i = 0;
  clock_t start = clock();

  while (i < n) {
    uint8_t ld_byte = buf[i];
    if ((ld_byte & (1 << 7)) == 0) {
      // Its an ascii character
      printf("U+%x ", ld_byte);
      i += 1;
      continue;
    }

    uint8_t num_bytes = GET_LEADING_1s(ld_byte);
    uint8_t lead_payload = ld_byte & ((1 << (7 - num_bytes)) - 1);

    uint32_t unicode = 0;
    unicode = lead_payload << ((num_bytes - 1) * 6);

    for (int j = 1; j < num_bytes; j++) {
      uint8_t cont_payload = buf[i + j] & MASK_FOR_CD;
      unicode |= cont_payload << ((num_bytes - j - 1) * 6);
    }

    printf("U+%x ", unicode);
    i += num_bytes;
  }

  printf("\n");
  clock_t end = clock();
  double time_spent = (double)(end - start) * 1e6 / CLOCKS_PER_SEC;

  printf("Time taken: %.2f micro\n", time_spent);
  printf("%s\n", buf);

  close(fd);
  return 0;
}

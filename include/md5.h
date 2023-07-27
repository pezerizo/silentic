#ifndef _S_MD5_
#define _S_MD5_

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

#define ROTATE_LEFT(x, n) (((x) << (n)) | ((x) >> (32 - (n))))
#define MD5_DIGEST_LENGTH 16

// MD5 Constants
static const uint32_t S[] = { 7, 12, 17, 22, 5, 9, 14, 20, 4, 11, 16, 23, 6, 10, 15, 21 };
static const uint32_t K[] = {
    0xd76aa478, 0xe8c7b756, 0x242070db, 0xc1bdceee,
    0xf57c0faf, 0x4787c62a, 0xa8304613, 0xfd469501,
    0x698098d8, 0x8b44f7af, 0xffff5bb1, 0x895cd7be,
    0x6b901122, 0xfd987193, 0xa679438e, 0x49b40821
};

// Function to compute MD5 hash
void generateMD5(const char* input, size_t inputLen, char* output) {
    uint32_t a0 = 0x67452301;
    uint32_t b0 = 0xefcdab89;
    uint32_t c0 = 0x98badcfe;
    uint32_t d0 = 0x10325476;

    // Pre-processing: adding padding and length
    size_t initialLen = inputLen;
    size_t extraLen = 56 - (inputLen % 64);
    if (extraLen < 1) {
        extraLen += 64;
    }
    inputLen += extraLen + 8;

    // Check for potential overflow
    if (inputLen < initialLen) {
        return; // Handle error gracefully
    }

    uint8_t* paddedInput = (uint8_t*)malloc(inputLen);
    if (!paddedInput) {
        return; // Handle allocation error
    }

    memcpy(paddedInput, input, initialLen);
    paddedInput[initialLen] = 0x80;

    uint64_t bitLen = initialLen * 8;
    memcpy(paddedInput + inputLen - 8, &bitLen, sizeof(uint64_t));

    // Process the message in 512-bit chunks
    for (size_t i = 0; i < inputLen; i += 64) {
        uint32_t* M = (uint32_t*)(paddedInput + i);

        uint32_t A = a0;
        uint32_t B = b0;
        uint32_t C = c0;
        uint32_t D = d0;

        // Main MD5 algorithm
        for (int j = 0; j < 64; j++) {
            uint32_t F, g;
            if (j < 16) {
                F = (B & C) | ((~B) & D);
                g = j;
            } else if (j < 32) {
                F = (D & B) | ((~D) & C);
                g = (5 * j + 1) % 16;
            } else if (j < 48) {
                F = B ^ C ^ D;
                g = (3 * j + 5) % 16;
            } else {
                F = C ^ (B | (~D));
                g = (7 * j) % 16;
            }

            F = F + A + K[j] + M[g];
            A = D;
            D = C;
            C = B;
            B = B + ROTATE_LEFT(F, S[j / 16]);
        }

        // Update the state
        a0 += A;
        b0 += B;
        c0 += C;
        d0 += D;
    }

    // Output the MD5 hash
    snprintf(output, 33, "%08x%08x%08x%08x", a0, b0, c0, d0);

    free(paddedInput);
}

#endif
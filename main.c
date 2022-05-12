#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define OWN_PRINTF_DEBUG

#define TJE_IMPLEMENTATION
#include "tiny_jpeg.h"

#include "qrcodegen.h"

int main(void)
{
    const char *text = "{\n    \"message\":\"Hello, world!\",\n    \"value\":45,\n    \"type\":\"test\"\n}";
	enum qrcodegen_Ecc errCorLvl = qrcodegen_Ecc_MEDIUM;  // Error correction level
	
    int preferred_code_size = 1024;

	// Make and print the QR Code symbol
	uint8_t qrcode[qrcodegen_BUFFER_LEN_MAX];
	uint8_t tempBuffer[qrcodegen_BUFFER_LEN_MAX];
	if (!qrcodegen_encodeText(text, tempBuffer, qrcode, errCorLvl, qrcodegen_VERSION_MIN, qrcodegen_VERSION_MAX, qrcodegen_Mask_AUTO, true))
    {
        printf("ERROR: Failed to create QR code\n");
        return EXIT_FAILURE;
    }

#ifdef OWN_PRINTF_DEBUG
    printf("\nQR code successfully generated\n");
#endif
    
    int generated_code_size = qrcodegen_getSize(qrcode);
    int scalar = preferred_code_size / generated_code_size;
    int actual_code_size = preferred_code_size - (preferred_code_size % generated_code_size);

#ifdef OWN_PRINTF_DEBUG
    printf("\nQR STATS\n* Generated QR Code size: %4d\n* Desired size:           %4d\n* Scalar:                 %4d\n* Actual size:            %4d\n\n", generated_code_size, preferred_code_size, scalar, actual_code_size);
#endif

    uint8_t *out_buffer = malloc(actual_code_size * actual_code_size * 3);

    long index = 0;

    for (int y = 0; y < generated_code_size; y++)
    {
        for (int y_scalar = 0; y_scalar < scalar; y_scalar++)
        {
            for (int x = 0; x < generated_code_size; x++)
            {
                int pixel_value = qrcodegen_getModule(qrcode, x,y) ? 0 : 255;

                for (int x_scalar = 0; x_scalar < (scalar * 3); x_scalar++)
                {
                    out_buffer[index++] = pixel_value;
                }
            }
        }
    }

    const char *filepath = "./img2.jpeg";
    int res = tje_encode_to_file_at_quality(filepath, 3, actual_code_size, actual_code_size, 3, out_buffer);
    printf("QR code write to file: %s\n\n", res ? "success" : "failure");

    free(out_buffer);

    return EXIT_SUCCESS;
}

#include <stdio.h>
#include <stdlib.h>
#include <teo.h>

extern char TEO_ERROR_MESSAGE[];

static void rgb2gray(TEOIMAGE *src, TEOIMAGE *dst) {
    int x, y;
    TEO_UINT8 r, g, b, gray;

    for (y = TeoYstart(src); y <= TeoYend(src); y++) {
        for (x = TeoXstart(src); x <= TeoXend(src); x++) {
            r = TeoGetPixel(src, x, y, 0, TEO_UINT8);
            g = TeoGetPixel(src, x, y, 1, TEO_UINT8);
            b = TeoGetPixel(src, x, y, 2, TEO_UINT8);

            gray = (TEO_UINT8)(0.299 * r + 0.587 * g + 0.114 * b);
            TeoPutPixel(dst, x, y, 0, TEO_UINT8, gray);
        }
    }
}

static void binary(TEOIMAGE *src, TEO_UINT8 threshold) {
    int x, y;
    TEO_UINT8 val;
    printf("%d\n", threshold);
    for (y = TeoYstart(src); y <= TeoYend(src); y++) {
        for (x = TeoXstart(src); x <= TeoXend(src); x++) {
            val = TeoGetPixel(src, x, y, 0, TEO_UINT8);
            // printf("%d\n", val);
            val = (val > threshold) ? 255 : 0;
            // printf("%d\n", val);
            TeoPutPixel(src, x, y, 0, TEO_UINT8, val);
        }
    }
}

int main(int argc, char *argv[]) {
    TEOFILE *src_teofp, *dst_teofp;
    TEOIMAGE *src_img, *dst_img;
    TEO_UINT8 threshold;
    int n;

    if (argc != 3) {
        fprintf(stderr, "Usage: %s in.teo #threshold\n", argv[0]);
        exit(1);
    }

    // input file reader
    {
        src_teofp = TeoOpenFile(argv[1]);
        if (!src_teofp) {
            fprintf(stderr, "%s (error code:%d)\n", TEO_ERROR_MESSAGE,
                    TEO_ERROR_CODE);
            goto settle;
        }
        if (!TeoIsUINT8(src_teofp)) {
            fprintf(stderr,
                    "Pixcel type UINT8 is only supported in this program\n");
            goto settle;
        }

        src_img = TeoAllocSimilarImage(src_teofp);
        if (!src_img) {
            fprintf(stderr, "%s (error code:%d)\n", TEO_ERROR_MESSAGE,
                    TEO_ERROR_CODE);
            goto settle;
        }

        threshold = (TEO_UINT8)atoi(argv[2]);
    }

    // output file
    {
        dst_teofp = TeoCreateFile("multi_binary.teo",
                              TeoWidth(src_img),
                              TeoHeight(src_img),
                              TeoXoffset(src_img),
                              TeoYoffset(src_img),
                              TEO_UNSIGNED,
                              8,
                              1,
                              TeoFrame(src_teofp));
    }

    dst_img = TeoAllocSimilarImage(dst_teofp);
    if (!dst_img) {
        fprintf(stderr, "%s (error code:%d)\n", TEO_ERROR_MESSAGE,
                TEO_ERROR_CODE);
        goto settle;
    }
    
    for (n = 0; n < TeoFrame(src_teofp); n++) {
        TeoReadFrame(src_teofp, src_img);
        printf("%d\n", TeoPlane(src_img));
        if (TeoPlane(src_img) != 1) {
            puts("gray");
            rgb2gray(src_img, dst_img);
        }
        binary(dst_img, threshold);

        TeoWriteFrame(dst_teofp, dst_img);
    }

settle:
    if (src_teofp) TeoCloseFile(src_teofp);
    if (dst_teofp) TeoCloseFile(dst_teofp);

    if (src_img) TeoFreeImage(src_img);
    if (dst_img) TeoFreeImage(dst_img);

    return 0;
}
# include<stdio.h>
# include <stdlib.h>
# include<teo.h>

extern char TEO_ERROR_MESSAGE[];

static TEOIMAGE* rgb2gray(TEOIMAGE *src) {
    TEOIMAGE *dst;
    int x, y;
    TEO_UINT8 r, g, b, gray;

    dst = TeoAllocImage(TeoWidth(src),
                        TeoHeight(src),
                        TeoXoffset(src),
                        TeoYoffset(src),
                        TeoType(src),
                        TeoBit(src),
                        1);
    
    for(y = TeoYstart(src); y <= TeoYend(src); y++) {
        for(x = TeoXstart(src); x <= TeoXend(src); x++) {
            r = TeoGetPixel(src, x, y, 0, TEO_UINT8);
            g = TeoGetPixel(src, x, y, 1, TEO_UINT8);
            b = TeoGetPixel(src, x, y, 2, TEO_UINT8);

            gray = (TEO_UINT8) (0.299 * r + 0.587 * g + 0.114 * b);
            TeoPutPixel(dst, x, y, 0, TEO_UINT8, gray);
        }
    }

    return dst;
}

int main(int argc, char* argv[]){
    TEOFILE *src_teofp, *dst_teofp;
    TEOIMAGE *src_img, *dst_img;

    if (argc != 2) {
        fprintf(stderr, "Usage: %s in.teo\n", argv[0]);
        exit(1);
    }

    // input file reader
    {
        src_teofp = TeoOpenFile(argv[1]);
        if(!src_teofp) {
            fprintf(stderr, "%s (error code:%d)\n", TEO_ERROR_MESSAGE, TEO_ERROR_CODE);
            goto settle;
        }
        if(!TeoIsUINT8(src_teofp)) {
            fprintf(stderr, "Pixcel type UINT8 is only supported in this program\n");
            goto settle;
        }

        src_img = TeoAllocSimilarImage(src_teofp);
        if(!src_img) {
            fprintf(stderr, "%s (error code:%d)\n", TEO_ERROR_MESSAGE, TEO_ERROR_CODE);
            goto settle;
        }

        TeoReadFrame(src_teofp, src_img);
        if(TeoPlane(src_img) < 3 || !TeoIsUINT8(src_img)) {
            fprintf(stderr, "Wrong image type\n");
            goto settle;
        }
    }
    
    // output file
    {
        dst_teofp = TeoCreateFile("gray.teo",
                              TeoWidth(src_img),
                              TeoHeight(src_img),
                              TeoXoffset(src_img),
                              TeoYoffset(src_img),
                              TeoType(src_img),
                              TeoBit(src_img),
                              1,
                              1);
    }
    
    dst_img = rgb2gray(src_img);
    if(!dst_img) {
        fprintf(stderr, "%s (error code:%d)\n", TEO_ERROR_MESSAGE, TEO_ERROR_CODE);
        goto settle;
    }
    
    TeoWriteFrame(dst_teofp, dst_img);
    
    settle:
    if(src_teofp) TeoCloseFile(src_teofp);
    if(dst_teofp) TeoCloseFile(dst_teofp);
    
    if(src_img) TeoFreeImage(src_img);
    if(dst_img) TeoFreeImage(dst_img);

    return 0;
}
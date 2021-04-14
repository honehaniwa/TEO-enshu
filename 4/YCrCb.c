# include<stdio.h>
# include<stdlib.h>
# include<teo.h>

extern char TEO_ERROR_MESSAGE[];

static TEOIMAGE* YCrCb(TEOIMAGE *src) {
    TEOIMAGE *dst;
    int x, y;
    TEO_UINT8 r, g, b;
    TEO_FLOAT64 Y, Cr, Cb;

    dst = TeoAllocImage(TeoWidth(src),
                        TeoHeight(src),
                        TeoXoffset(src),
                        TeoYoffset(src),
                        TEO_FLOAT,
                        64,
                        3);
    
    for(y = TeoYstart(src); y <= TeoYend(src); y++) {
        for(x = TeoXstart(src); x <= TeoXend(src); x++) {
            r = TeoGetPixel(src, x, y, 0, TEO_UINT8);
            g = TeoGetPixel(src, x, y, 1, TEO_UINT8);
            b = TeoGetPixel(src, x, y, 2, TEO_UINT8);

            Y  = 0.2989 * r + 0.5866 * g + 0.1145 * b;
            Cr = 0.5000 * r - 0.4183 * g - 0.0816 * b;
            Cb =-0.1687 * r - 0.3312 * g + 0.5000 * b;

            TeoPutPixel(dst, x, y, 0, TEO_FLOAT64, Y);
            TeoPutPixel(dst, x, y, 1, TEO_FLOAT64, Cr);
            TeoPutPixel(dst, x, y, 2, TEO_FLOAT64, Cb);
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
        dst_teofp = TeoCreateFile("YCrCb.teo",
                              TeoWidth(src_img),
                              TeoHeight(src_img),
                              TeoXoffset(src_img),
                              TeoYoffset(src_img),
                              TEO_FLOAT,
                              64,
                              3,
                              1);
    }
    
    dst_img = YCrCb(src_img);
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
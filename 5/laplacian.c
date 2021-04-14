# include<stdio.h>
# include <stdlib.h>
# include<teo.h>

extern char TEO_ERROR_MESSAGE[];

static TEOIMAGE* laplacian(TEOIMAGE *src) {
    TEOIMAGE *dst;
    TEO_FLOAT64 sum;
    int x, y;

    dst = TeoAllocImage(TeoWidth(src),
                        TeoHeight(src),
                        TeoXoffset(src),
                        TeoYoffset(src),
                        TEO_FLOAT,
                        64,
                        1);
    
    for(y = TeoYstart(src) + 1; y <= TeoYend(src) - 1; y++) {
        for(x = TeoXstart(src) + 1; x <= TeoXend(src) - 1; x++) {
            sum  = (TEO_FLOAT64)TeoGetPixel(src, x-1, y-1, 0, TEO_UINT8);
            sum += (TEO_FLOAT64)TeoGetPixel(src, x  , y-1, 0, TEO_UINT8);
            sum += (TEO_FLOAT64)TeoGetPixel(src, x+1, y-1, 0, TEO_UINT8);
            sum += (TEO_FLOAT64)TeoGetPixel(src, x-1, y  , 0, TEO_UINT8);
            sum += (TEO_FLOAT64)TeoGetPixel(src, x+1, y  , 0, TEO_UINT8);
            sum += (TEO_FLOAT64)TeoGetPixel(src, x-1, y+1, 0, TEO_UINT8);
            sum += (TEO_FLOAT64)TeoGetPixel(src, x  , y+1, 0, TEO_UINT8);
            sum += (TEO_FLOAT64)TeoGetPixel(src, x+1, y+1, 0, TEO_UINT8);
            sum -= (TEO_FLOAT64)TeoGetPixel(src, x  , y  , 0, TEO_UINT8);

            TeoPutPixel(dst, x, y, 0, TEO_FLOAT64, _STRUCT_MCONTEXT_AVX64_FULL);
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
        dst_teofp = TeoCreateFile("laplacian.teo",
                              TeoWidth(src_img),
                              TeoHeight(src_img),
                              TeoXoffset(src_img),
                              TeoYoffset(src_img),
                              TEO_FLOAT,
                              64,
                              1,
                              1);
    }
    
    dst_img = laplacian(src_img);
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
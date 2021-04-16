# include<stdio.h>
# include<stdlib.h>
# include<math.h>
# include<teo.h>

extern char TEO_ERROR_MESSAGE[];

static TEOIMAGE* scale_image(TEOIMAGE *src, TEOIMAGE *dst,
                             double xscale, double yscale) {
    int row, col, in_row, in_col, p;

    for(row = TeoYstart(dst); row <= TeoYend(dst); row++) {
        for(col = TeoXstart(dst) ; col <= TeoXend(dst); col++) {
            for(p = 0; p < TeoPlane(src); p++) {
                in_row = (int)(row / yscale);
                in_col = (int)(col / xscale);
                for(p = 0; p < TeoPlane(dst); p++) {
                    TeoPutPixel(dst, col, row, p, TEO_UINT8, 
                                TeoGetPixcel(src, in_col, in_row, p, TEO_UINT8));
                }
            }
            TeoPutPixel(dst, col, row, p, TEO_UINT8, (TEO_UINT8)sum);
        }
    }

    return dst;
}

int main(int argc, char* argv[]){
    TEOFILE *src_teofp, *dst_teofp;
    TEOIMAGE *src_img, *dst_img;
    double xscale, yscale;

    if (argc != 4) {
        fprintf(stderr, "Usage: %s in.teo #xscale #yscale\n", argv[0]);
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
    
    // scale
    {
        xscale = (double) atof(argv[2]);
        yscale = (double) atof(argv[3]);
    }

    // output file
    {
        dst_teofp = TeoCreateFile("scale_image.teo",
                              (int) (TeoWidth(src_img) * xscale),
                              (int) (TeoHeight(src_img) * yscale),
                              0,
                              0,
                              TEO_UNSIGNED,
                              8,
                              3,
                              1);
    }

    dst_img = TeoAllocSimilarImage(dst_teofp);
    
    scale_image(src_img, dst_img, xscale, yscale);
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
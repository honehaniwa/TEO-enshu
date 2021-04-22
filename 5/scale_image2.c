# include<stdio.h>
# include<stdlib.h>
# include<math.h>
# include<teo.h>

extern char TEO_ERROR_MESSAGE[];

static TEO_UINT8 TeoGetLinerPixel(TEOIMAGE *img, double x, double y, int plane) {
    TEO_UINT8 pix[4];
    int ix, iy;
    double a, b;

    ix = (int)x;
    iy = (int)y;

    a = x - ix;
    b = y - iy;

    pix[0] = TeoGetPixel(img, ix, iy, plane, TEO_UINT8);
    pix[1] = TeoGetPixel(img, (ix>=TeoYend(img))? ix : ix + 1, iy,
                                      plane, TEO_UINT8);
    pix[2] = TeoGetPixel(img, ix, (iy>=TeoYend(img))? iy : iy + 1,
                                      plane, TEO_UINT8);
    pix[3] = TeoGetPixel(img, (ix>=TeoYend(img))? ix : ix + 1,
                              (iy>=TeoYend(img))? iy : iy + 1,
                                      plane, TEO_UINT8);
    
    return (TEO_UINT8) ((1.0 - b) * (1.0 - a) * pix[0] +
                        (1.0 - b) * a         * pix[1] +
                        b         * (1.0 - a) * pix[2] +
                        b         * a         * pix[3]);
}

static TEOIMAGE* scale_image2(TEOIMAGE *src, TEOIMAGE *dst,
                             double xscale, double yscale) {
    int row, col, p;
    double in_row, in_col;

    for(row = TeoYstart(dst); row <= TeoYend(dst); row++) {
        for(col = TeoXstart(dst) ; col <= TeoXend(dst); col++) {
            for(p = 0; p < TeoPlane(src); p++) {
                in_row = ((double)row / yscale);
                in_col = ((double)col / xscale);
                for(p = 0; p < TeoPlane(dst); p++) {
                    TeoPutPixel(dst, col, row, p, TEO_UINT8, 
                                TeoGetLinerPixel(src, in_col, in_row, p));
                }
            }
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
        dst_teofp = TeoCreateFile("scale_image2.teo",
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
    
    scale_image2(src_img, dst_img, xscale, yscale);
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
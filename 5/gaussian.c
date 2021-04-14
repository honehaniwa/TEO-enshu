# include<stdio.h>
# include<stdlib.h>
# include<math.h>
# include<teo.h>

extern char TEO_ERROR_MESSAGE[];

static TEOIMAGE* gaussian(TEOIMAGE *src, int sigma) {
    TEOIMAGE *dst;
    double **gauss;
    double sum = 0.0;
    int kernel_size, x, y, row, col, p;

    dst = TeoAllocImage(TeoWidth(src),
                        TeoHeight(src),
                        TeoXoffset(src),
                        TeoYoffset(src),
                        TEO_UNSIGNED,
                        8,
                        TeoPlane(src));
    
    kernel_size = sigma * 2 + 1;

    gauss = (double **)malloc(sizeof(double *) * kernel_size) + sigma;

    for(y = -sigma; y <= sigma; y++) {
        gauss[y] = (double *)malloc(sizeof(double) * kernel_size) + sigma;
        for(x = -sigma; x <= sigma; x++) {
            gauss[y][x] = exp(-(x*x + y*y) / (2.0 * sigma * sigma));
            sum += gauss[y][x];
        }
    }

    for(y = -sigma; y <= sigma; y++) {
        for(x = -sigma; x <= sigma; x++) {
            gauss[y][x] /= sum;
        }
    }

    for(row = TeoYstart(src) + sigma; row <= TeoYend(src) - sigma; row++) {
        for(col = TeoXstart(src) + sigma; col <= TeoXend(src) - sigma; col++) {
            for(p = 0; p < TeoPlane(src); p++) {
                sum = 0.0;
                for(y = -sigma; y <= sigma; y++) {
                    for(x = -sigma; x <= sigma; x++) {
                        sum += gauss[y][x] * TeoGetPixel(src, col+x, row+y, p, TEO_UINT8);
                    }
                }
            }
            TeoPutPixel(dst, col, row, p, TEO_UINT8, (TEO_UINT8)sum);
        }
    }

    for(y = -sigma; y <= sigma; y++) {
        gauss[y] -= sigma;
        free(gauss[y]);
    }

    gauss -= sigma;
    free(gauss);

    return dst;
}

int main(int argc, char* argv[]){
    TEOFILE *src_teofp, *dst_teofp;
    TEOIMAGE *src_img, *dst_img;
    int sigma

    if (argc != 2) {
        fprintf(stderr, "Usage: %s in.teo #sigma\n", argv[0]);
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
        dst_teofp = TeoCreateFile("gaussian.teo",
                              TeoWidth(src_img),//src_teofp?
                              TeoHeight(src_img),
                              TeoXoffset(src_img),
                              TeoYoffset(src_img),
                              TEO_UNSIGNED,
                              8,
                              3,
                              1);
    }
    
    sigma = atoi(argv[2]);

    dst_img = gaussian(src_img, sigma);
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
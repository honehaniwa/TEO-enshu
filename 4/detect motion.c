# include<stdio.h>
# include<stdlib.h>
# include<teo.h>

extern char TEO_ERROR_MESSAGE[];

static void f_and_image(TEOIMAGE *src1, TEOIMAGE *src2
                      TEOIMAGE *color,TEOIMAGE *dst) {
    TEO_UINT8 val1, val2;
    int x, y, p;

    for(y = TeoYstart(src1); y <= TeoYend(src1); y++) {
        for(x = TeoXstart(src1); x <= TeoXend(src1); x++) {
            val1 = TeoGetPixel(src1, x, y, 0, TEO_UINT8);
            val2 = TeoGetPixel(src2, x, y, 0, TEO_UINT8);

            if(val1 && vsl2) {
                for(p = 0; p < TeoPlane(xor); p++) {
                    TeoPutPixel(dst, x, y, p, TEO_UINT8, 
                                TeoGetPixel(xor, x, y, p, TEO_UINT8));
                }
            }
            else {
                for(p = 0; p < TeoPlane(xor); p++) {
                    TeoPutPixel(dst, x, y, p, TEO_UINT8, 256);
                }
            }
            TeoPutPixel(dst, x, y, 0, TEO_FLOAT64, Y);
            TeoPutPixel(dst, x, y, 1, TEO_FLOAT64, Cr);
            TeoPutPixel(dst, x, y, 2, TEO_FLOAT64, Cb);
        }
    }
}

static void f_sub_image(TEOIMAGE *src1, TEOIMAGE *src2, TEOIMAGE *dst) {
    TEO_UINT8 val1, val2, threshold = 8;
    int x, y;

    for(y = TeoYstart(src1); y <= TeoYend(src1); y++) {
        for(x = TeoXstart(src1); x <= TeoXend(src1); x++) {
            val1 = TeoGetPixel(src1, x, y, 0, TEO_UINT8);
            val2 = TeoGetPixel(src2, x, y, 0, TEO_UINT8);

            if(abs(val1 - vsl2) > threshold) {
                TeoPutPixel(dst, x, y, 0, TEO_UINT8, 1);
            }
            else {
                TeoPutPixel(dst, x, y, 0, TEO_UINT8, 0);
            }
        }
    }
}

int main(int argc, char* argv[]){
    TEOFILE *src_teofp, *dst_teofp;
    TEOIMAGE **src_img, **sub_img, *dst_img;
    int i, frame;
    char name[1024];

    if (argc != 4) {
        fprintf(stderr, "Usage: %s in_header #frame out_header\n", argv[0]);
        exit(1);
    }

    frame = atoi(argv[2]);

    // input file reader
    {
        src_img = (TEOIMAGE **)malloc(sizeof(TEOIMAGE *) * frame);

        for (i = 0; i < frame; i++) {
            sprintf(name, argv[1], i);
            src_teofp = TeoOpenFile(name);
            src_img[i] = TepAllocSimilarImage(src_teofp);
            TeoReadFrame(src_teofp, src_img[i]);
            TeoCloseFile(src_teofp);
        }
    }
    
    // output file
    {
        dst_img = TeoAllocSimilarImage(src_img[0]);
    }
    // Sub image
    {
        sub_img = (TEOIMAGE **)malloc(sizeof(TEOIMAGE *) * (frame-1));
        for (i = 0;i < frame-1; i++) {
            f_sub_image(src_img[i+1], src_img[i], sub_img[i]);
        }
    }
    // And sub_image
    {
        for(i = 1; i< frame - 1; i++) {
            f_and_image(sub_img[i-1], sub_img[i], src_img[i], dst_img);
            sprintf(name, argv[3], i);
            dst_teofp = TeoCreateFile(name,
                                      TeoWidth(dst_img),
                                      TeoHeight(dst_img),
                                      TeoXoffset(dst_img),
                                      TeoYoffset(dst_img),
                                      TeoType(dst_img),
                                      TeoBit(dst_img),
                                      TeoPlane(dst_img), 1);
            TeoWriteFrame(dst_teofp, dst_img);
            TeoCloseFile(dst_teofp);
        }
    }
    
    for(i = 0; i < frame; i++) TeoFreeImage(src_img[i]);
    for(i = 0; i < frame-1; i++) TeoFreeImage(sub_img[i]);
    free(src_img);
    free(sub_img);

    TeoFreeImage(dst_img);
    return 0;
}
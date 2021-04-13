# include<stdio.h>
# include <stdlib.h>
# include<teo.h>

extern char TEO_ERROR_MESSAGE[];

int main(int argc, char* argv[]){
    TEOFILE *src_teofp, *dst_teofp;
    TEOIMAGE *src_img, *dst_img;
    int x, y, p;
    TEO_UINT8 val;

    if (argc != 3) {
        fprintf(stderr, "Usage: %s in.teo out.teo\n", argv[0]);
        exit(1);
    }

    src_teofp = TeoOpenFile(argv[1]);
    if(!src_teofp) {
        fprintf(stderr, "%s (error code:%d)\n", TEO_ERROR_MESSAGE, TEO_ERROR_CODE);
        goto settle;
    }
    if(!TeoIsUINT8(src_teofp)) {
        fprintf(stderr, "Pixcel type UINT8 is only supported in this program\n");
        goto settle;
    }
    dst_teofp = TeoCreateSimilarFile(argv[2], src_teofp);
    if(!dst_teofp) {
        fprintf(stderr, "%s (error code:%d)\n", TEO_ERROR_MESSAGE, TEO_ERROR_CODE);
        goto settle;
    }
    src_img = TeoAllocSimilarImage(src_teofp);
    if(!src_img) {
        fprintf(stderr, "%s (error code:%d)\n", TEO_ERROR_MESSAGE, TEO_ERROR_CODE);
        goto settle;
    }
    dst_img = TeoAllocSimilarImage(dst_teofp);
    if(!dst_img) {
        fprintf(stderr, "%s (error code:%d)\n", TEO_ERROR_MESSAGE, TEO_ERROR_CODE);
        goto settle;
    }
    
    TeoReadFrame(src_teofp, src_img);
    
    for(y = TeoYstart(src_img); y <= TeoYend(src_img); y++) {
        for(x = TeoXstart(src_img); x <= TeoXend(src_img); x++) {
            for(p = 0; p < TeoPlane(src_img); p++) {
                // printf("%d %d %d\n", x, y, p);
                val = TeoGetPixel(src_img, x, y, p, TEO_UINT8);

                TeoPutPixel(dst_img, x, y, p, TEO_UINT8, val);
            }
        }
    }
    
    TeoWriteFrame(dst_teofp, dst_img);
    
    settle:
    if(src_teofp) TeoCloseFile(src_teofp);
    if(dst_teofp) TeoCloseFile(dst_teofp);
    
    if(src_img) TeoFreeImage(src_img);
    if(dst_img) TeoFreeImage(dst_img);

    return 0;
}
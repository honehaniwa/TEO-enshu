# include<stdio.h>
# include <stdlib.h>
# include<teo.h>

int main(int argc, char* argv[]){
    TEOFILE *src1_teofp, *src2_teofp, *dst_teofp;
    TEOIMAGE *src1_img, *src2_img, *dst_img;

    if (argc != 3) {
        fprintf(stderr, "Usage: %s in1.teo in2.teo\n", argv[0]);
        exit(1);
    }

    // input file reader
    {
        src1_teofp = TeoOpenFile(argv[1]);
        src2_teofp = TeoOpenFile(argv[2]);
        src1_img = TeoAllocSimilarImage(src1_teofp);
        src2_img = TeoAllocSimilarImage(src2_teofp);

        TeoReadFrame(src1_teofp, src1_img);
        TeoReadFrame(src2_teofp, src2_img);
    }
    
    // output file
    {
        dst_teofp = TeoCreateFile("panorama.teo",
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
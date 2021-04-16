#define TeoGetLinerPixel(image, index_x, index_y, index_p, ETYPE) \
    (((index_x) - ((int) (index_x))) * \
    (((index_y)-((int) (index_y))) * \
    (double) TeoGetPixel(image, (((int) (index_x)) == (teo_xend(image))) ? \
                                 ((int) (index_x)) : (((int) (index_x))+1), \
                                 (((int) (index_y)) == (teo_yend(image))) ?\
                                 ((int) (index_y)) : (((int) (index_y)) + 1),\
                                 (index_p), ETYPE)  + \
    (1.0-((index_y)-((int) (index_y)))) * \
    (((index_y)-((int) (index_y))) * \
    (double) TeoGetPixel(image, (((int) (index_x)) == (teo_yend(image))) ? \
                                 ((int) (index_x)) : ((int) (index_x)) + 1), \
                                 ((int) (index_y)), \
                                 (index_p), ETYPE)) + \
    (1.0-((index_x)-((int) (index_x)))) * \
    (((index_y)-((int) (index_y))) * \
    (double) TeoGetPixel(image, (((int) (index_x)), \
                                 (((int) (index_y)) == (teo_yend(image))) ?\
                                 ((int) (index_y)) : (((int) (index_y)) + 1),\
                                 (index_p), ETYPE)) + \
    (1.0-((index_y)-((int) (index_y)))) * \
    (double) TeoGetPixel(image, ((int) (index_x)), ((int) (index_y)), \
                                 (index_p), ETYPE)))
#include <teo.h>
#include <Eigen/Core>
#include <Eigen/LU>
#include <iostream>
#include <vector>

using namespace std;
using namespace Eigen;

struct Points {
    double x;
    double y;
};

extern char TEO_ERROR_MESSAGE[];

int main(int argc, char* argv[]) {
    TEOFILE *src1_teofp, *src2_teofp, *dst_teofp;
    TEOIMAGE *src1_img, *src2_img, *dst_img;

    if (argc != 3) {
        fprintf(stderr, "Usage: %s in1.teo in2.teo\n", argv[0]);
        exit(1);
    }

    // input files
    int N;  // 対応する点の数
    cin >> N;
    if (N < 4) {
        cerr << "small input. You must have more than 4 points.";
        return 1;
    }
    vector<pair<Points, Points>> pair_points(N);
    for (auto& p : pair_points) {
        cin >> p.first.x >> p.first.y >> p.second.x >> p.second.y;
    }

    // calc
    MatrixXd A(2 * N, 8), b(2 * N, 1);
    // calc A and b
    for (int i = 0; i < N; i++) {
        A(i, 0) = pair_points[i].first.x;
        A(i, 1) = pair_points[i].first.y;
        A(i, 2) = 1;
        A(i, 3) = 0;
        A(i, 4) = 0;
        A(i, 5) = 0;
        A(i, 6) = -pair_points[i].first.x * pair_points[i].second.x;
        A(i, 7) = -pair_points[i].first.y * pair_points[i].second.x;
        b(i, 0) = pair_points[i].second.x;

        A(i + N, 0) = 0;
        A(i + N, 1) = 0;
        A(i + N, 2) = 0;
        A(i + N, 3) = pair_points[i].first.x;
        A(i + N, 4) = pair_points[i].first.y;
        A(i + N, 5) = 1;
        A(i + N, 6) = -pair_points[i].first.x * pair_points[i].second.y;
        A(i + N, 7) = -pair_points[i].first.y * pair_points[i].second.y;
        b(i + N, 0) = pair_points[i].second.y;
    }
    // cout << "A is:\n" << A << endl;
    // cout << "b is:\n" << b << endl;

    // calc h
    MatrixXd A2 = A.transpose() * A;

    // cout << "A2 is:\n" << A2 << endl;

    MatrixXd h = MatrixXd::Ones(9, 1);
    h.topRows(8) = A2.inverse() * A.transpose() * b;

    cout << "h is:\n" << h << endl;
    h.resize(3, 3);
    MatrixXd H = h.transpose();
    cout << "H is:\n" << H << endl;

    // input file reader
    {
        // cout << argv[1] << ' ' << argv[2] << endl;
        src1_teofp = TeoOpenFile(argv[1]);
        src1_img = TeoAllocSimilarImage(src1_teofp);
        TeoReadFrame(src1_teofp, src1_img);

        src2_teofp = TeoOpenFile(argv[2]);
        src2_img = TeoAllocSimilarImage(src2_teofp);
        TeoReadFrame(src2_teofp, src2_img);
    }

    // calc image size
    vector<Points> img2_pos = {{TeoXoffset(src2_img), TeoYoffset(src2_img)},
                               {TeoWidth(src2_img), TeoYoffset(src2_img)},
                               {TeoXoffset(src2_img), TeoHeight(src2_img)},
                               {TeoWidth(src2_img), TeoHeight(src2_img)}};
    Points min_pos = {0., 0.}, max_pos = {0., 0.};

    MatrixXd H_inv = H.inverse();
    cout << "H_inv: \n" << H_inv << endl;
    for (int i = 0; i < 4; i++) {
        MatrixXd pos = MatrixXd::Ones(3, 1);
        pos(0, 0) = img2_pos[i].x;
        pos(1, 0) = img2_pos[i].y;
        // cout << img2_pos[i].x << ' ' << img2_pos[i].y << endl;
        // cout<<pos<<endl;
        MatrixXd newpos = H_inv * pos;
        // cout<<"newpos: \n"<<newpos<<endl;
        double nx = newpos(0, 0), ny = newpos(1, 0), z = newpos(2, 0);
        nx /= z;
        ny /= z;
        // cout << "nx, ny: " << nx << ' ' << ny << endl;
        min_pos = {min({nx, (double)TeoXoffset(src1_img), min_pos.x}),
                   min({ny, (double)TeoYoffset(src1_img), min_pos.y})};
        max_pos = {max({nx, (double)TeoWidth(src1_img), max_pos.x}),
                   max({ny, (double)TeoHeight(src1_img), max_pos.y})};
    }

    cout << "min_pos: " << min_pos.x << ' ' << min_pos.y << endl;
    cout << "max_pos: " << max_pos.x << ' ' << max_pos.y << endl;

    // output file
    {
        dst_teofp = TeoCreateFile(
            "panorama_out.teo", (int)max_pos.x - (int)min_pos.x, (int)max_pos.y - (int)min_pos.y,
            (int)min_pos.x, (int)min_pos.y, TEO_UNSIGNED, 8, TeoPlane(src1_img), 1);
        dst_img = TeoAllocSimilarImage(dst_teofp);
    }
    puts("write src1");
    // write
    for (int y = TeoYstart(src1_img); y <= TeoYend(src1_img); y++) {
        for (int x = TeoXstart(src1_img); x <= TeoXend(src1_img); x++) {
            for (int p = 0; p < TeoPlane(src1_img); p++) {
                int val = TeoGetPixel(src1_img, x, y, p, TEO_UINT8);

                TeoPutPixel(dst_img, x, y, p, TEO_UINT8, val);
            }
        }
    }
    puts("write src2");
    for (int y = min_pos.y; y <= max_pos.y; y++) {
        for (int x = min_pos.x; x <= max_pos.x; x++) {
            MatrixXd pos = MatrixXd::Ones(3, 1);
            pos(0, 0) = x;
            pos(1, 0) = y;
            MatrixXd newpos = H * pos;
            double nx = newpos(0, 0), ny = newpos(1, 0), z = newpos(2, 0);
            // cout << newpos << endl;
            // cout << "x, y: " << x << ' ' << y << endl;
            // cout << "nx, ny: " << (int)(nx/z) << ' ' << (int)(ny/z) <<
            // endl;
            if ((int)(nx / z) > TeoXoffset(src2_img) && (int)(nx / z) < (double)TeoWidth(src2_img) && (int)(ny / z) > TeoYoffset(src2_img) && (int)(ny / z) < (double)TeoHeight(src2_img)) {
                for (int p = 0; p < TeoPlane(src2_img); p++) {
                    int val = TeoGetPixel(src2_img, (int)(nx / z), (int)(ny / z), p, TEO_UINT8);
                    TeoPutPixel(dst_img, x, y, p, TEO_UINT8,
                            val);
                }
            }
        }
    }
    
    puts("write frame");
    TeoWriteFrame(dst_teofp, dst_img);

    puts("free");
    if (src1_img) TeoFreeImage(src1_img);
    if (src2_img) TeoFreeImage(src2_img);
    if (dst_img) TeoFreeImage(dst_img);
    if (src1_teofp) TeoCloseFile(src1_teofp);
    if (src2_teofp) TeoCloseFile(src2_teofp);
    if (dst_teofp) TeoCloseFile(dst_teofp);

    return 0;
}
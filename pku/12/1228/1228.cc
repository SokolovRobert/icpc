/**
 * @file
 * ��:��b
 */

#include <complex>
#include <cmath>
#include <vector>

using namespace std;


/// ���e�덷
#define EP 1.0e-8

/// �덷�������֐�
inline int SGN(double a) { return abs(a) < EP ? 0 : a > 0 ? 1 : -1; }
#define EQ(a,b) (SGN((a)-(b)) == 0)    ///< equal
#define NE(a,b) (SGN((a)-(b)) != 0)    ///< not equal
#define GE(a,b) (SGN((a)-(b)) >= 0)    ///< greater or equal
#define LE(a,b) (SGN((a)-(b)) <= 0)    ///< less or equal
#define GR(a,b) (SGN((a)-(b)) > 0)     ///< greater
#define LS(a,b) (SGN((a)-(b)) < 0)     ///< less
#define PS(a)   (SGN(a) > 0)           ///< positive
#define NG(a)   (SGN(a) < 0)           ///< negative
#define ZR(a)   (SGN(a) == 0)          ///< zero
#define NZ(a)   (SGN(a) != 0)          ///< nonzero



/// �_
typedef complex<double> Point;

/// Point�̔�r���Z�q
namespace std {
    inline bool operator<(const Point& a, const Point& b) {
        if (NE(a.real(), b.real()))
            return LS(a.real(), b.real());
        return LS(a.imag(), b.imag());
    }
}


/// ���p�`
typedef vector<Point> Polygon;

/// �����P��
const Point I(0, 1);



/// �����������͐���
/**
 * pos��pos+dir�����Ԑ����A�܂��͂������������������\���B
 */
struct Line {
    Point pos, dir;

    Line() {
    }
    Line(const Point& pos, const Point& dir)
        : pos(pos), dir(dir) {
    }


    /// ax+by+c = 0�Ȃ鎮���\�������𐶐�����
    static Line make_line_eq(double a, double b, double c) {
        return Line((NZ(a) ? Point(-c/a, 0) : Point(0, -c/b)), Point(b, -a));
    }
};


/// �����`
struct Rect {
    Point lb, rt; /* LeftBottom, RightTop */

    Rect()
        : lb(0, 0), rt(0, 0) {
        validate();
    }
    Rect(const Point& lb, const Point& rt)
        : lb(lb), rt(rt) {
        validate();
    }
    Rect(double left, double bottom, double right, double top)
        : lb(left, bottom), rt(right, top) {
        validate();
    }
    void validate() {
        Point sz = rt - lb;
        assert(sz.real() >= 0 && sz.imag() >= 0);
    }
};


/// �O�p�`
struct Triangle {
    Point a, b, c;

    Triangle() {
    }
    Triangle(const Point& a, const Point& b, const Point& c)
        : a(a), b(b), c(c) {
    }
};




/// ���ς����߂�
inline double inp(const Point& a, const Point& b) {
    return (conj(a)*b).real();
}

/// �O�ς����߂�
inline double outp(const Point& a, const Point& b) {
    return (conj(a)*b).imag();
}


/// �ˉe�x�N�g�������߂�
/**
 * \param[in] p �ˉe���̃x�N�g��
 * \param[in] b �ˉe��̃x�N�g��
 */
inline Point proj(const Point& p, const Point& b) {
    return b*inp(p,b)/norm(b);
}

/// �_���璼���ɑ΂��Ĉ����������̑������߂�
inline Point perf(const Line& l, const Point& p) {
    const Line m(l.pos - p, l.dir);
    return (p + (m.pos - proj(m.pos, m.dir)));
}

/// �����𒼐��Ɏˉe�������������߂�
/**
 * \param[in] s �ˉe���̐���
 * \param[in] b �ˉe��̒���
 */
inline Line proj(const Line& s, const Line& b) {
    return Line(perf(b, s.pos), proj(s.dir, b.dir));
}



/// ��]�̕����𒲂ׂ�(Sedgewick�̂�����������)
inline int ccw(const Point& p, const Point& r, const Point& s) {
    Point a(r-p), b(s-p);
    double op = outp(a, b);
    if (SGN(op) != 0)
        return SGN(op);
    if (NG(a.real()*b.real()) || NG(a.imag()*b.imag()))
        return -1;
    if (LS(norm(a), norm(b)))
        return 1;
    return 0;
}

/// ��]�̕����𒲂ׂ�
inline int rot(const Point& p, const Point& r, const Point& s) {
    return SGN(outp(r-p, s-p));
}



/// �����\���̐��K�����s��
Line normalize_line(const Line& l) {
    return Line(perf(l, Point(0, 0)),
                (l.dir.imag() < 0 ? -l.dir : l.dir)/abs(l.dir));
}


/**
 * @file
 * ��:��������ƒ���,�����̋���
 */



/// �������m���������邩�ǂ������ׂ�
bool ll_intersects(const Line& l, const Line& m) {
    Line a = normalize_line(l);
    Line b = normalize_line(m);
    return NZ(abs(a.dir-b.dir));
}

/// �����Ɛ������������邩���ׂ�
bool ls_intersects(const Line& l, const Line& s) {
    return ( ccw(l.pos, l.pos+l.dir, s.pos)
             *ccw(l.pos, l.pos+l.dir, s.pos+s.dir) <= 0 );
}

/// �������m���������邩���ׂ�
bool ss_intersects(const Line& s, const Line& t) {
    return (ls_intersects(s, t) &&
            ls_intersects(t, s));
}


/// �������m�����s���ǂ������ׂ�
bool line_parallel(const Line& l, const Line& m) {
    return !ll_intersects(l, m);
}

/// �������m����v���邩���ׂ�
bool line_equals(const Line& l, const Line& m) {
    Line a = normalize_line(l);
    Line b = normalize_line(m);
    return (EQ(abs(a.pos-b.pos), 0) && EQ(abs(a.dir-b.dir), 0));
}


/// �����`���m���������邩���ׂ�
bool rect_intersects(const Rect& r, const Rect& s) {
    Point a = s.rt - r.lb;
    Point b = r.rt - s.lb;
    return !(NG(a.real()) || NG(a.imag()) ||
             NG(b.real()) || NG(b.imag()) );
}



/// �_�Ɠ_�̋���
double pp_distance(const Point& a, const Point& b) {
    return abs(a-b);
}

/// �����Ɠ_�̋���
double lp_distance(const Line& l, const Point& p) {
    return pp_distance(perf(l, p), p);
}

/// �����Ɠ_�̋���
double sp_distance(const Line& s, const Point& p) {
    const Point r = perf(s, p);
    const double pos = ((r-s.pos)/s.dir).real();
    if (LE(0, pos) && LE(pos, 1))
        return lp_distance(s, p);
    return min(pp_distance(s.pos, p),
               pp_distance(s.pos+s.dir, p));
}

/// �����Ɛ����̋���
double ss_distance(const Line& s, const Line& t) {
    if (ss_intersects(s, t))
        return 0;
    return min(sp_distance(s, t.pos),
           min(sp_distance(s, t.pos+t.dir),
           min(sp_distance(t, s.pos),
               sp_distance(t, s.pos+s.dir))));
}

/// �����Ɛ����̋���
double ls_distance(const Line& l, const Line& s) {
    if (ls_intersects(l, s))
        return 0;
    return min(lp_distance(l, s.pos),
               lp_distance(l, s.pos+s.dir));
}

///�����ƒ����̋���
double ll_distance(const Line& l, const Line& m) {
    Line a = normalize_line(l);
    Line b = normalize_line(m);
    if (NZ(abs(a.dir-b.dir)))
        return 0;
    return pp_distance(a.pos/a.dir, b.pos/b.dir);
}


/**
 * @file
 * ��:�ʕ�:Graham Scan
 */

/// Graham Scan�ɗp����comparator
// Thanks to IHI
struct polar_less {
    polar_less(const Point& pivot)
        : pivot(pivot) {
    }
    bool operator()(const Point& a, const Point& b) {
        Point z = (a - pivot)/(b - pivot);
        if (NZ(arg(z)))
            return (arg(z) < 0);
        return (abs(z) < 1);
    }
    Point pivot;
};

/// �ʕ�����߂�
/**
 * \param[out] polygon �ʕ�ł��鑽�p�`
 * \param[in] vertices �_�̔z��
 */
// Thanks to IHI
void convex_hull(Polygon& polygon, const Polygon& vertices) {
    const int n = vertices.size();
    assert(n >= 3);

    polygon = vertices;
    swap(polygon[0], *min_element(polygon.begin(), polygon.end()));
    Point pivot = polygon[0];

    sort(polygon.begin()+1, polygon.end(), polar_less(pivot));

    {
        int tail = n - 1;
        while(tail > 1 &&
              EQ(arg(polygon[tail-1] - pivot),
                 arg(polygon[n-1] - pivot)))
            tail--;
        reverse(polygon.begin()+tail, polygon.end());
    }

    int m = 3;
    for(int i = 3; i < n; i++) {
        Point pt = polygon[i];
        while(ccw(polygon[m-2], polygon[m-1], pt) <= 0)
            m--;
        polygon[m++] = pt;
    }
    polygon.resize(m);
}


#include <iostream>

int main() {
    int nCases;
    cin >> nCases;
    for(int iCase = 0; iCase < nCases; iCase++) {
        int n;
        cin >> n;
        vector<Point> v;
        for(int i = 0; i < n; i++) {
            double x, y;
            cin >> x >> y;
            v.push_back(Point(x, y));
        }
        struct No {};
        try {
            if (n <= 3)
                throw No();
            Polygon convex;
            convex_hull(convex, v);
            if (convex.size() != v.size()) {
                throw No();
            }
            else {
                convex.resize(n+3);
                convex[n] = convex[0];
                convex[n+1] = convex[1];
                convex[n+2] = convex[2];
                for(int i = 0; i < n; i++) {
                    Line a(convex[i], convex[i+1]-convex[i]);
                    Line b(convex[i+1], convex[i+2]-convex[i+1]);
                    Line c(convex[i+2], convex[i+3]-convex[i+2]);
                    if (!line_parallel(a, b) && !line_parallel(b, c))
                        throw No();
                }
            }
            cout << "YES" << endl;
        }
        catch(No) {
            cout << "NO" << endl;
        }
    }
    return 0;
}


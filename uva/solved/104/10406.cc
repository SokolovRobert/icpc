/*
 * 幾何基礎
 */

#include <complex>
#include <cmath>
#include <vector>

using namespace std;


// 許容誤差
#define EP 1.0e-8

// 誤差つき符号関数
inline int SGN(double a) { return abs(a) < EP ? 0 : a > 0 ? 1 : -1; }
#define EQ(a,b) (SGN((a)-(b)) == 0)    ///< equal
#define NE(a,b) (SGN((a)-(b)) != 0)    ///< not equal
#define LE(a,b) (SGN((a)-(b)) <= 0)    ///< less or equal
#define LT(a,b) (SGN((a)-(b)) < 0)     ///< less



// 点
typedef complex<double> P;

// Ptの比較演算子
namespace std {
    inline bool operator<(const P& a, const P& b) {
        if (NE(a.real(), b.real()))
            return LT(a.real(), b.real());
        return LT(a.imag(), b.imag());
    }
}


// 多角形
typedef vector<P> G;



/*
 * 線分、半直線、直線
 *  posとpos+dirを結ぶ線分、またはそれを延長した直線、半直線を表す。
 */
struct L {
    P pos, dir;

    L() {}
    L(const P& pos, const P& dir) : pos(pos), dir(dir) {}

};

// ax+by+c = 0なる方程式が表す直線を生成する
L make_line_eq(double a, double b, double c) {
    assert(NE(a, 0) || NE(b, 0));
    return L((NE(a, 0) ? P(-c/a, 0) : P(0, -c/b)), P(b, -a));
}


// 円
struct C {
    
};


// 長方形
struct R {
    P lb, rt; /* LeftBottom, RightTop */

    R(const P& lb, const P& rt) : lb(lb), rt(rt) {}
    R(double left, double bottom, double right, double top)
        : lb(left, bottom), rt(right, top) {}
};


// 三角形
struct T {
    P a, b, c;

    T(const P& a, const P& b, const P& c) : a(a), b(b), c(c) {}
};




// 内積を求める
inline double inp(const P& a, const P& b) {
    return (conj(a)*b).real();
}

// 外積を求める
inline double outp(const P& a, const P& b) {
    return (conj(a)*b).imag();
}



/* 射影ベクトルを求める
 *  p 射影元のベクトル
 *  b 射影先のベクトル
 */
inline P proj(const P& p, const P& b) {
    return b*inp(p,b)/norm(b);
}

// 点から直線に対して引いた垂線の足を求める
inline P perf(const L& l, const P& p) {
    L m(l.pos - p, l.dir);
    return (p + (m.pos - proj(m.pos, m.dir)));
}


/* 線分を直線に射影した線分を求める
 *  s 射影元の線分
 *  b 射影先の直線
 */
inline L proj(const L& s, const L& b) {
    return L(perf(b, s.pos), proj(s.dir, b.dir));
}



// 回転の方向を調べる(Sedgewickのおせっかい版)
inline int ccw(const P& p, const P& r, const P& s) {
    P a(r-p), b(s-p);
    int sgn = SGN(outp(a, b));
    if (sgn != 0)
        return sgn;
    if (LT(a.real()*b.real(), 0) || LT(a.imag()*b.imag(), 0))
        return -1;
    if (LT(norm(a), norm(b)))
        return 1;
    return 0;
}

// 回転の方向を調べる
inline int rot(const P& p, const P& r, const P& s) {
    return SGN(outp(r-p, s-p));
}


// 直線表現の正規化を行う
L normalize_line(const L& l) {
    return L(perf(l, P(0, 0)),
             ( (LT(l.dir.imag(), 0) ||
                (EQ(l.dir.imag(), 0) && LT(l.dir.real(), 0)) )
               ? -l.dir : l.dir ) / abs(l.dir) );
}


/*
 * 点、線分、半直線、直線に関する交差判定と距離
 */


using namespace std;


// 直線同士が交差するかどうか調べる(OK)
bool ll_intersects(const L& l, const L& m) {
    return NE(abs(normalize_line(l).dir - normalize_line(m).dir), 0);
}

// 直線と線分が交差するか調べる(OK)
bool ls_intersects(const L& l, const L& s) {
    return (ccw(l.pos, l.pos+l.dir, s.pos)
            *ccw(l.pos, l.pos+l.dir, s.pos+s.dir) <= 0);
}

// 線分同士が交差するか調べる(OK)
bool ss_intersects(const L& s, const L& t) {
    return (ls_intersects(s, t) &&
            ls_intersects(t, s));
}


// 直線同士が平行かどうか調べる(OK)
bool line_parallel(const L& l, const L& m) {
    return !ll_intersects(l, m);
}

// 直線同士が一致するか調べる(OK)
bool line_equals(const L& l, const L& m) {
    L a = normalize_line(l);
    L b = normalize_line(m);
    return (EQ(abs(a.pos-b.pos), 0) && EQ(abs(a.dir-b.dir), 0));
}

// 直線同士の交点を求める
P line_cross(const L& l, const L& m) {
    double num = outp(m.dir, m.pos-l.pos);
    double denom = outp(m.dir, l.dir);
    if (EQ(denom, 0))
        throw 0; // 共有点が一つではない
    return P(l.pos + l.dir*num/denom);
}



// 点と点の距離(OK)
double pp_distance(const P& a, const P& b) {
    return abs(a - b);
}

// 直線と点の距離(OK)
double lp_distance(const L& l, const P& p) {
    return abs(perf(l, p) - p);
}

// 線分と点の距離(OK)
double sp_distance(const L& s, const P& p) {
    const P r = perf(s, p);
    const double pos = ((r-s.pos)/s.dir).real();
    if (LE(0, pos) && LE(pos, 1))
        return abs(r - p);
    return min(abs(s.pos - p),
               abs(s.pos+s.dir - p));
}

// 線分と線分の距離(OK)
double ss_distance(const L& s, const L& t) {
    if (ss_intersects(s, t))
        return 0;
    return min(sp_distance(s, t.pos),
           min(sp_distance(s, t.pos+t.dir),
           min(sp_distance(t, s.pos),
               sp_distance(t, s.pos+s.dir))));
}

// 直線と線分の距離(OK)
double ls_distance(const L& l, const L& s) {
    if (ls_intersects(l, s))
        return 0;
    return min(lp_distance(l, s.pos),
               lp_distance(l, s.pos+s.dir));
}

//直線と直線の距離
double ll_distance(const L& l, const L& m) {
    L a = normalize_line(l);
    L b = normalize_line(m);
    if (NE(abs(a.dir-b.dir), 0))
        return 0;
    return abs(a.pos - b.pos);
}

/*
 * 面積
 */

#include <cmath>

using namespace std;


// 多角形の符号付面積を求める
double signed_area(G& g) {
    int n = g.size();
    double s = 0.0;
    for(int i = 0; i < n; i++)
        s += outp(g[i], g[(i+1)%n])/2;
    return s;
}

// 多角形の面積を求める
double area(G& g) {
    return abs(signed_area(g));
}

// 多角形の回転方向を調べる(CCWならtrue, CWならfalse)
bool ccw(G& g) {
    return LT(0, signed_area(g));
}

#include <iostream>
#include <cstdio>

int main() {
    double d;
    int n;
    while(cin >> d >> n && !(d == 0 && n == 0)) {
        G v(n);
        for(int i = 0; i < n; i++) {
            double x, y;
            cin >> x >> y;
            v[i] = P(x, y);
        }
        vector<L> lines(n);
        for(int i = 0; i < n; i++) {
            int j = (i+1)%n;
            L baseline(v[i], v[j]-v[i]);
            lines[i] = L(baseline.pos + baseline.dir / abs(baseline.dir) * d * P(0, -1),
                         baseline.dir);
            /*
            printf("line[%d] = (%.2f,%.2f)->(%.2f,%.2f)\n",
                   i,
                   lines[i].pos.real(), lines[i].pos.imag(),
                   lines[i].dir.real(), lines[i].dir.imag());
            */
        }
        G u(n);
        for(int i = 0; i < n; i++) {
            int j = (i+1)%n;
            u[i] = line_cross(lines[i], lines[j]);
            //printf("u[%d] = (%.2f,%.2f)\n", i, u[i].real(), u[i].imag());
        }
        printf("%.3f\n", area(u));
    }
    return 0;
}

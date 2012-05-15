/*
 * �􉽊�b
 */

#include <complex>
#include <cmath>
#include <vector>

using namespace std;


// ���e�덷
#define EP 1.0e-8

// �덷�������֐�
inline int SGN(double a) { return abs(a) < EP ? 0 : a > 0 ? 1 : -1; }
#define EQ(a,b) (SGN((a)-(b)) == 0)    ///< equal
#define NE(a,b) (SGN((a)-(b)) != 0)    ///< not equal
#define LE(a,b) (SGN((a)-(b)) <= 0)    ///< less or equal
#define LT(a,b) (SGN((a)-(b)) < 0)     ///< less



// �_
typedef complex<double> P;

// Pt�̔�r���Z�q
namespace std {
    inline bool operator<(const P& a, const P& b) {
        if (NE(a.real(), b.real()))
            return LT(a.real(), b.real());
        return LT(a.imag(), b.imag());
    }
}


// ���p�`
typedef vector<P> G;



/*
 * �����A�������A����
 *  pos��pos+dir�����Ԑ����A�܂��͂�����������������A��������\���B
 */
struct L {
    P pos, dir;

    L(const P& pos, const P& dir) : pos(pos), dir(dir) {}

};

// ax+by+c = 0�Ȃ���������\�������𐶐�����
L make_line_eq(double a, double b, double c) {
    assert(NE(a, 0) || NE(b, 0));
    return L((NE(a, 0) ? P(-c/a, 0) : P(0, -c/b)), P(b, -a));
}


// �~
struct C {
    
};


// �����`
struct R {
    P lb, rt; /* LeftBottom, RightTop */

    R(const P& lb, const P& rt) : lb(lb), rt(rt) {}
    R(double left, double bottom, double right, double top)
        : lb(left, bottom), rt(right, top) {}
};


// �O�p�`
struct T {
    P a, b, c;

    T(const P& a, const P& b, const P& c) : a(a), b(b), c(c) {}
};




// ���ς����߂�
inline double inp(const P& a, const P& b) {
    return (conj(a)*b).real();
}

// �O�ς����߂�
inline double outp(const P& a, const P& b) {
    return (conj(a)*b).imag();
}



/* �ˉe�x�N�g�������߂�
 *  p �ˉe���̃x�N�g��
 *  b �ˉe��̃x�N�g��
 */
inline P proj(const P& p, const P& b) {
    return b*inp(p,b)/norm(b);
}

// �_���璼���ɑ΂��Ĉ����������̑������߂�
inline P perf(const L& l, const P& p) {
    L m(l.pos - p, l.dir);
    return (p + (m.pos - proj(m.pos, m.dir)));
}


/* �����𒼐��Ɏˉe�������������߂�
 *  s �ˉe���̐���
 *  b �ˉe��̒���
 */
inline L proj(const L& s, const L& b) {
    return L(perf(b, s.pos), proj(s.dir, b.dir));
}



// ��]�̕����𒲂ׂ�(Sedgewick�̂�����������)
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

// ��]�̕����𒲂ׂ�
inline int rot(const P& p, const P& r, const P& s) {
    return SGN(outp(r-p, s-p));
}


// �����\���̐��K�����s��
L normalize_line(const L& l) {
    return L(perf(l, P(0, 0)),
             ( (LT(l.dir.imag(), 0) ||
                (EQ(l.dir.imag(), 0) && LT(l.dir.real(), 0)) )
               ? -l.dir : l.dir ) / abs(l.dir) );
}


/*
 * �_�A�����A�������A�����Ɋւ����������Ƌ���
 */


using namespace std;


// �������m���������邩�ǂ������ׂ�
bool ll_intersects(const L& l, const L& m) {
    return NE(abs(normalize_line(l).dir - normalize_line(m).dir), 0);
}

// �����Ɛ������������邩���ׂ�
bool ls_intersects(const L& l, const L& s) {
    return (ccw(l.pos, l.pos+l.dir, s.pos)
            *ccw(l.pos, l.pos+l.dir, s.pos+s.dir) <= 0);
}

// �������m���������邩���ׂ�
bool ss_intersects(const L& s, const L& t) {
    return (ls_intersects(s, t) &&
            ls_intersects(t, s));
}


// �������m�����s���ǂ������ׂ�
bool line_parallel(const L& l, const L& m) {
    return !ll_intersects(l, m);
}

// �������m����v���邩���ׂ�
bool line_equals(const L& l, const L& m) {
    L a = normalize_line(l);
    L b = normalize_line(m);
    return (EQ(abs(a.pos-b.pos), 0) && EQ(abs(a.dir-b.dir), 0));
}

// �������m�̌�_�����߂�
P line_cross(const L& l, const L& m) {
    double num = outp(m.dir, m.pos-l.pos);
    double denom = outp(m.dir, l.dir);
    if (EQ(denom, 0))
        throw 0; // ���L�_����ł͂Ȃ�
    return P(l.pos + l.dir*num/denom);
}



// �_�Ɠ_�̋���
double pp_distance(const P& a, const P& b) {
    return abs(a - b);
}

// �����Ɠ_�̋���
double lp_distance(const L& l, const P& p) {
    return abs(perf(l, p) - p);
}

// �����Ɠ_�̋���
double sp_distance(const L& s, const P& p) {
    const P r = perf(s, p);
    const double pos = ((r-s.pos)/s.dir).real();
    if (LE(0, pos) && LE(pos, 1))
        return abs(r - p);
    return min(abs(s.pos - p),
               abs(s.pos+s.dir - p));
}

// �����Ɛ����̋���
double ss_distance(const L& s, const L& t) {
    if (ss_intersects(s, t))
        return 0;
    return min(sp_distance(s, t.pos),
           min(sp_distance(s, t.pos+t.dir),
           min(sp_distance(t, s.pos),
               sp_distance(t, s.pos+s.dir))));
}

// �����Ɛ����̋���
double ls_distance(const L& l, const L& s) {
    if (ls_intersects(l, s))
        return 0;
    return min(lp_distance(l, s.pos),
               lp_distance(l, s.pos+s.dir));
}

//�����ƒ����̋���
double ll_distance(const L& l, const L& m) {
    L a = normalize_line(l);
    L b = normalize_line(m);
    if (NE(abs(a.dir-b.dir), 0))
        return 0;
    return abs(a.pos/a.dir - b.pos/b.dir);
}


#include <iostream>

P readPoint() {
    double x, y;
    cin >> x >> y;
    return P(x, y);
}

int main() {
    int n;
    cin >> n;
    while(n-- > 0) {
        P p1 = readPoint();
        P p2 = readPoint();
        P lt = readPoint();
        P rb = readPoint();
        P lb(lt.real(), rb.imag());
        P rt(rb.real(), lt.imag());
        L l0(p1, p2-p1);
        L l1(lb, lt-lb), l2(lb, rb-lb), l3(rt, lt-rt), l4(rt, rb-rt);
        if (ss_intersects(l0, l1) ||
            ss_intersects(l0, l2) || 
            ss_intersects(l0, l3) || 
            ss_intersects(l0, l4))
        {
            cout << 'T' << endl;
        }
        else {
            cout << 'F' << endl;
        }
    }
    return 0;
}

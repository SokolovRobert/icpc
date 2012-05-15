#include <cstdio>
#include <cstring>

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


/*
 * �����A�������A����
 *  pos��pos+dir�����Ԑ����A�܂��͂�����������������A��������\���B
 */
struct L {
    P pos, dir;

    L() {}
    L(const P& pos, const P& dir) : pos(pos), dir(dir) {}
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








P readPoint() {
    double x, y;
    scanf("%lf%lf", &x, &y);
    return P(x, y);
}

L readLine() {
    P a, b;
    a = readPoint();
    b = readPoint();
    return L(a, b-a);
}

int main() {
    while(true) {
        char s[2][10];
        L l1 = readLine();
        scanf("%s", s[0]);
        L l2 = readLine();
        scanf("%s", s[1]);
        if (strcmp(s[0], "END") == 0)
            break;
        double d;
        if (strcmp(s[0], "L") == 0 && strcmp(s[1], "L") == 0)
            d = ll_distance(l1, l2);
        if (strcmp(s[0], "LS") == 0 && strcmp(s[1], "LS") == 0)
            d = ss_distance(l1, l2);
        if (strcmp(s[0], "L") == 0 && strcmp(s[1], "LS") == 0)
            d = ls_distance(l1, l2);
        if (strcmp(s[0], "LS") == 0 && strcmp(s[1], "L") == 0)
            d = ls_distance(l2, l1);
        printf("%.5f\n", d);
    }
    return 0;
}

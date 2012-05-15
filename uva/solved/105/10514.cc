#include <iostream>
#include <complex>
#include <cmath>
#include <vector>
#include <cstdio>
#include <queue>

using namespace std;



typedef double Weight;

const Weight WEIGHT_INFTY = 1.0e100;//std::numeric_limits<Weight>::max();

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

typedef vector<P> G;


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

double nya_distance(const G& a, const G& b) {
    int n = a.size();
    int m = b.size();
    double d = WEIGHT_INFTY;
    for(int i = 0; i < n-1; i++) {
        for(int j = 0; j < m-1; j++) {
            d = min(d,
                    ss_distance(L(a[i], a[i+1]-a[i]),
                                L(b[j], b[j+1]-b[j])));
        }
    }
    return d;
}




Weight dijkstra(vector< vector<Weight> >& adj, int start, int goal)
{
    const int n = adj.size();

    vector<bool> visited(n, false);

    typedef pair<Weight, int> Edge;
    priority_queue<Edge, vector<Edge>, greater<Edge> > q;
    q.push(Edge(0, start));

    while(! q.empty()) {
        Edge e = q.top();
        q.pop();

        if (visited[e.second])
            continue;

        //printf("visiting %d with cost %.2f\n", e.second, e.first);
        visited[e.second] = true;

        if (e.second == goal)
            return e.first;

        for(int i = 0; i < n; i++) {
            if (!visited[i]) {
                q.push(Edge(e.first+adj[e.second][i], i));
            }
        }
    }

    return -1;
}



P readPoint() {
    double x, y;
    cin >> x >> y;
    return P(x, y);
}

L readLine() {
    P a, b;
    a = readPoint();
    b = readPoint();
    return L(a, b-a);
}

int main() {
    int nCases;
    cin >> nCases;
    while(nCases-- > 0) {
        int r1, r2, n;
        cin >> r1 >> r2 >> n;
        n+=2;
        vector<G> v(n);
        for(int i = 0; i < r1; i++)
            v[0].push_back(readPoint());
        for(int i = 0; i < r2; i++)
            v[1].push_back(readPoint());
        for(int i = 2; i < n; i++) {
            int m;
            cin >> m;
            for(int j = 0; j < m; j++)
                v[i].push_back(readPoint());
            P p = v[i].front();
            v[i].push_back(p);
        }

        vector< vector<double> > adj(n, vector<double>(n, WEIGHT_INFTY));

        for(int a = 0; a < n; a++) {
            adj[a][a] = 0;
            for(int b = a+1; b < n; b++) {
                double d = nya_distance(v[a], v[b]);
                adj[a][b] = adj[b][a] = d;
            }
        }

        /*
        for(int i = 0; i < n; i++) {
            for(int j = 0; j < n; j++) {
                printf("%.2f\t", adj[i][j]);
            }
            printf("\n");
        }
        */

        double result = dijkstra(adj, 0, 1);

        printf("%.3f\n", result);

    }
    return 0;
}

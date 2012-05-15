/**
 * @file
 * ����:�ݏ��@�Ȃ�
 */

#include <utility>

using namespace std;

typedef long long int integer;

/// �g��Euclid�ݏ��@
/**
 * @return a*pair.first + b*pair.second == gcd(a, b)
 */
pair<integer,integer> xgcd(integer a, integer b) {
    if (b == 0)
        return make_pair(1, 0);
    pair<integer,integer> p = xgcd(b, a%b);
    return make_pair(p.second, p.first - (a/b)*p.second);
}

/// �ő����
integer gcd(integer a, integer b) {
    while(b != 0) {
        integer r = a%b;
        a = b;
        b = r;
    }
    return a;
}



struct no_solution {};

/// a*n = b (mod m) �ƂȂ�A�񕉂ł��ŏ���n�����߂�
/**
 * @note
 * �����Ȃ��ꍇ��struct no_solution��throw����
 */
integer divide(integer a, integer b, integer m) {
    integer g = gcd(a, m);
    if (b%g != 0)
        throw no_solution();
    pair<integer,integer> p = xgcd(a, m);
    assert(p.first*a+p.second*m == gcd(a,m));
    integer n = p.first*b/g;
    integer dn = m/g;
    n -= n/dn*dn;
    if (n < 0)
        n += dn;
    return n;
}

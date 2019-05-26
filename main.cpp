#include <set>
#include <random>
#include <cassert>
#include <cmath>

class UniqCounter {

    static unsigned int hash_function(int data) {
        unsigned int key = data;
        key += ~(key << 16);
        key ^= (key >> 5);
        key += (key << 3);
        key ^= (key >> 13);
        key += ~(key << 9);
        key ^= (key >> 17);
        return key;
    }

    static int p(unsigned int w) {
        int i = 1;
        for (; i <= 32 && ((w & 1) == 0); i++) {
            w = (unsigned int) (w >> 1);
        }
        return i;
    }

    int m = (1 << 12);
    int M[(1 << 12)];

public:

    UniqCounter() {
        for (int i = 0; i < m; i++) {
            M[i] = 0;
        }
    }

    void add(int x) {
        unsigned int h = hash_function(x);
        int j = (h >> 20);
        int w = ((unsigned int) (h << 12)) >> 12;
        M[j] = std::max(M[j], p(w));
    }

    int get_uniq_num() const {
        double z = 0.0;
        for (int i = 0; i < m; i++) {
            z += pow(2.0, -M[i] * 1.0);
        }

        int e = (int) (0.72111003961 * m * m / z);

        if (e < 5.0 / 2.0 * m) {
            int v = 0;
            for (int i = 0; i < m; i++) {
                v += M[i] == 0 ? 1 : 0;
            }
            return (int) (m * log((m * 1.0) / v));
        } else if (e > (1 << 30) / 7.5) {
            return (int) (-((1 << 30) / 7.5) * log(1 - e / pow(2.0, 32.0)));
        }

        return e;
    }
};

double relative_error(int expected, int got) {
    return abs(got - expected) / (double) expected;
}

int main() {
    std::random_device rd;
    std::mt19937 gen(rd());

    const int N = (int) 1e6;
    for (int k : {1, 10, 1000, 10000, N / 10, N, N * 10}) {
        std::uniform_int_distribution<> dis(1, k);
        std::set<int> all;
        UniqCounter counter;
        for (int i = 0; i < N; i++) {
            int value = dis(gen);
            all.insert(value);
            counter.add(value);
        }
        int expected = (int) all.size();
        int counter_result = counter.get_uniq_num();
        double error = relative_error(expected, counter_result);
        printf("%d numbers in range [1 .. %d], %d uniq, %d result, %.5f relative error\n", N, k, expected,
               counter_result, error);
        assert(error <= 0.1);
    }

    return 0;
}
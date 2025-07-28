#include <bits/stdc++.h>
#include "json.hpp"  // nlohmann JSON library (https://github.com/nlohmann/json)

using namespace std;
using json = nlohmann::json;
typedef __int128_t int128;

// Function to convert base-b string to decimal integer (supporting large numbers)
int128 baseToDecimal(string numStr, int base) {
    int128 result = 0;
    for (char ch : numStr) {
        int digit;
        if (isdigit(ch)) digit = ch - '0';
        else digit = tolower(ch) - 'a' + 10;
        result = result * base + digit;
    }
    return result;
}

// Function to print __int128 (since cout doesn't support it)
void printInt128(int128 n) {
    if (n == 0) { cout << "0"; return; }
    string s;
    while (n > 0) {
        s += (n % 10) + '0';
        n /= 10;
    }
    reverse(s.begin(), s.end());
    cout << s;
}

// Modular Inverse (Fermat's Little Theorem)
int128 modInverse(int128 a, int128 mod) {
    int128 m0 = mod, t, q;
    int128 x0 = 0, x1 = 1;
    if (mod == 1) return 0;
    while (a > 1) {
        q = a / mod;
        t = mod;
        mod = a % mod, a = t;
        t = x0;
        x0 = x1 - q * x0;
        x1 = t;
    }
    if (x1 < 0) x1 += m0;
    return x1;
}

// Lagrange Interpolation to find f(0)
int128 lagrangeInterpolation(vector<pair<int128, int128>>& points, int128 mod) {
    int k = points.size();
    int128 secret = 0;
    for (int i = 0; i < k; ++i) {
        int128 xi = points[i].first;
        int128 yi = points[i].second;
        int128 numerator = 1, denominator = 1;
        for (int j = 0; j < k; ++j) {
            if (i != j) {
                int128 xj = points[j].first;
                numerator = (numerator * (-xj + mod)) % mod;
                denominator = (denominator * (xi - xj + mod)) % mod;
            }
        }
        int128 inv = modInverse(denominator, mod);
        int128 term = yi * numerator % mod * inv % mod;
        secret = (secret + term + mod) % mod;
    }
    return secret;
}

// Read JSON and process a test case
void processTestCase(string filePath) {
    ifstream inFile(filePath);
    json data;
    inFile >> data;

    int n = data["keys"]["n"];
    int k = data["keys"]["k"];

    vector<pair<int128, int128>> shares;
    for (auto& [key, val] : data.items()) {
        if (key == "keys") continue;
        int128 x = stoll(key);
        int base = stoi(val["base"].get<string>());
        string encodedY = val["value"];
        int128 y = baseToDecimal(encodedY, base);
        shares.push_back({x, y});
    }

    // Select first k shares for interpolation
    vector<pair<int128, int128>> selectedShares(shares.begin(), shares.begin() + k);

    // Use a large prime modulus (> 256-bit limit)
    int128 MOD = (__int128_t(1) << 127) - 1; // A large Mersenne prime

    int128 secret = lagrangeInterpolation(selectedShares, MOD);
    printInt128(secret);
    cout << endl;
}

int main() {
    processTestCase("input1.json");
    processTestCase("input2.json");
    return 0;
}

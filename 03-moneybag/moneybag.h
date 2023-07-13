#ifndef MONEYBAG_H
#define MONEYBAG_H

#include <iostream>
#include <compare>
#include <cstdint>

using std::partial_ordering;
using std::strong_ordering;
using std::string;
using std::to_string;

class Moneybag {
public:
    using coin_number_t = uint64_t;

    constexpr Moneybag(coin_number_t livre,
                       coin_number_t solidus,
                       coin_number_t denier) : livre(livre),
                                               solidus(solidus),
                                               denier(denier) {};

    constexpr coin_number_t
    livre_number() const { return livre; }

    constexpr coin_number_t
    solidus_number() const { return solidus; }

    constexpr coin_number_t
    denier_number() const { return denier; }

    constexpr Moneybag operator+=(const Moneybag &snd);

    constexpr Moneybag operator-=(const Moneybag &snd);

    constexpr Moneybag operator*=(coin_number_t multiplier);

    constexpr Moneybag operator+(const Moneybag &snd) const;

    constexpr Moneybag operator-(const Moneybag &snd) const;

    constexpr Moneybag operator*(coin_number_t multiplier) const;

    constexpr bool operator==(const Moneybag &snd) const;

    constexpr bool operator!=(const Moneybag &snd) const;

    constexpr partial_ordering
    operator<=>(Moneybag const &snd) const;

    constexpr explicit operator bool() const;

    string toString() const;

private:
    coin_number_t livre;
    coin_number_t solidus;
    coin_number_t denier;

    constexpr void
    isSubtractionCorrect(const Moneybag &snd) const;

    constexpr void
    isAdditionCorrect(const Moneybag &snd) const;

    constexpr void
    isMultiplicationCorrect(coin_number_t multiplier) const;
};

constexpr static Moneybag Solidus = Moneybag{0, 1, 0};
constexpr static Moneybag Livre = Moneybag{1, 0, 0};
constexpr static Moneybag Denier = Moneybag{0, 0, 1};

constexpr Moneybag
Moneybag::operator+=(const Moneybag &snd) {
    isAdditionCorrect(snd);
    livre += snd.livre_number();
    solidus += snd.solidus_number();
    denier += snd.denier_number();
    return *this;
}

constexpr Moneybag
Moneybag::operator-=(const Moneybag &snd) {
    isSubtractionCorrect(snd);
    livre -= snd.livre_number();
    solidus -= snd.solidus_number();
    denier -= snd.denier_number();
    return *this;
}

constexpr Moneybag
Moneybag::operator*=(const coin_number_t multiplier) {
    isMultiplicationCorrect(multiplier);
    livre *= multiplier;
    solidus *= multiplier;
    denier *= multiplier;
    return *this;
}

constexpr Moneybag
Moneybag::operator-(const Moneybag &snd) const {
    return Moneybag(*this) -= snd;
}

constexpr Moneybag
Moneybag::operator+(const Moneybag &snd) const {
    return Moneybag(*this) += snd;
}

constexpr Moneybag
Moneybag::operator*(const coin_number_t multiplier) const {
    return Moneybag(*this) *= multiplier;
}

constexpr Moneybag operator*(const Moneybag::coin_number_t multiplier,
                             const Moneybag &moneybag) {
    return Moneybag(moneybag) *= multiplier;
}

constexpr bool
Moneybag::operator==(const Moneybag &snd) const {
    if (solidus == snd.solidus_number() &&
        denier == snd.denier_number() &&
        livre == snd.livre_number()) {
        return true;
    }
    return false;
}

constexpr bool
Moneybag::operator!=(const Moneybag &snd) const {
    if (!(*this == snd)) {
        return true;
    }
    return false;
}

constexpr partial_ordering
Moneybag::operator<=>(Moneybag const &snd) const {
    if (*this == snd) {
        return partial_ordering::equivalent;
    } else if (solidus <= snd.solidus_number() &&
               denier <= snd.denier_number() &&
               livre <= snd.livre_number()) {
        return partial_ordering::less;
    } else if (solidus >= snd.solidus_number() &&
               denier >= snd.denier_number() &&
               livre >= snd.livre_number()) {
        return partial_ordering::greater;
    } else {
        return partial_ordering::unordered;
    }
}

constexpr void
Moneybag::isSubtractionCorrect(const Moneybag &snd) const {
    if (snd.livre_number() > livre ||
        snd.solidus_number() > solidus ||
        snd.denier_number() > denier) {
        throw std::out_of_range(
                "Subtraction would result in negative number of coins.");
    }
}

constexpr void
Moneybag::isAdditionCorrect(const Moneybag &snd) const {
    if (UINT64_MAX - snd.livre_number() < livre ||
        UINT64_MAX - snd.solidus_number() < solidus ||
        UINT64_MAX - snd.denier_number() < denier) {
        throw std::out_of_range(
                "Addition would result in integer overflow.");
    }
}

constexpr void Moneybag::isMultiplicationCorrect(
        const coin_number_t multiplier) const {
    if (UINT64_MAX / multiplier < livre ||
        UINT64_MAX / multiplier < solidus ||
        UINT64_MAX / multiplier < denier) {
        throw std::out_of_range(
                "Multiplication would result in integer overflow.");
    }
}

string Moneybag::toString() const {
    string s1 = "(" + to_string(livre);
    if (livre == 1) {
        s1 += " livr";
    } else {
        s1 += " livres";
    }

    string s2 = ", " + to_string(solidus);
    if (solidus == 1) {
        s2 += " solidus";
    } else {
        s2 += " soliduses";
    }

    string s3 = ", " + to_string(denier);
    if (denier == 1) {
        s3 += " denier";
    } else {
        s3 += " deniers";
    }
    s3 += ")";
    return s1 + s2 + s3;
}

constexpr Moneybag::operator bool() const {
    if (denier == 0 && livre == 0 && solidus == 0) {
        return false;
    }
    return true;
}

std::ostream &
operator<<(std::ostream &stream, const Moneybag &moneybag) {
    return stream << moneybag.toString();
}

class Value {
private:
    __uint128_t amount;

public:
    constexpr Value() : amount(0) {};

    constexpr explicit Value(const Moneybag &moneybag);

    constexpr explicit Value(const Moneybag::coin_number_t denier)
            : amount(denier) {}

    constexpr Value &
    operator=(const Value &value) = default;

    constexpr bool operator==(const Value &value) const;

    constexpr bool operator!=(const Value &value) const;

    constexpr bool operator==(const Moneybag::coin_number_t denier) const;

    constexpr bool operator!=(const Moneybag::coin_number_t denier) const;

    constexpr strong_ordering
    operator<=>(const Value &value) const = default;

    constexpr strong_ordering
    operator<=>(Moneybag::coin_number_t denier) const;

    explicit operator std::string() const;
};

constexpr Value::Value(const Moneybag &moneybag) {
    amount = (__uint128_t) moneybag.livre_number() * 240 +
             (__uint128_t) moneybag.solidus_number() * 12 +
             (__uint128_t) moneybag.denier_number();
}

constexpr bool Value::operator==(const Value &value) const {
    return amount == value.amount;
}

constexpr bool Value::operator!=(const Value &value) const {
    return amount != value.amount;
}

constexpr bool Value::operator==(const Moneybag::coin_number_t denier) const {
    return amount == (__uint128_t) denier;
}

constexpr bool Value::operator!=(const Moneybag::coin_number_t denier) const {
    return amount != (__uint128_t) denier;
}

constexpr strong_ordering
Value::operator<=>(const Moneybag::coin_number_t denier) const {
    if (amount == (__uint128_t) denier) {
        return strong_ordering::equivalent;
    } else if (amount < (__uint128_t) denier) {
        return strong_ordering::less;
    } else {
        return strong_ordering::greater;
    }
}

Value::operator std::string() const {
    string result;
    __uint128_t tmp = amount;
    do {
        result += (char) (tmp % 10 + '0');
        tmp /= 10;
    } while (tmp > 0);
    std::reverse(result.begin(), result.end());
    return result;
}

#endif //MONEYBAG_H

// Copyright © 2017 Pieter Wuille
// Copyright © 2019 Binance.
//
// This file is part of the Binance Chain SDK. The full Binance Chain SDK
// copyright notice, including terms governing use, modification, and
// redistribution, is contained in the file LICENSE at the root of the source
// code distribution tree.

#include "Data.h"

#include <stdint.h>
#include <vector>
#include <string>

namespace Binance {
namespace Bech32 {

/// Encodes a Bech32 string.
///
/// \returns the encoded string, or an empty string in case of failure.
std::string encode(const std::string& hrp, const Data& values);

/// Decodes a Bech32 string.
///
/// \returns a pair with the human-readable part and the data, or a pair or empty collections on failure.
std::pair<std::string, Data> decode(const std::string& str);

/// Converts from one power-of-2 number base to another.
template<int frombits, int tobits, bool pad>
inline bool convertBits(Data& out, const Data& in) {
    int acc = 0;
    int bits = 0;
    const int maxv = (1 << tobits) - 1;
    const int max_acc = (1 << (frombits + tobits - 1)) - 1;
    for (size_t i = 0; i < in.size(); ++i) {
        int value = in[i];
        acc = ((acc << frombits) | value) & max_acc;
        bits += frombits;
        while (bits >= tobits) {
            bits -= tobits;
            out.push_back((acc >> bits) & maxv);
        }
    }
    if (pad) {
        if (bits) out.push_back((acc << (tobits - bits)) & maxv);
    } else if (bits >= frombits || ((acc << (tobits - bits)) & maxv)) {
        return false;
    }
    return true;
}

}} // namespace

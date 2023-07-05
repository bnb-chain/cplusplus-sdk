// Copyright © 2019 All BNB Chain Developers.
//
// This file is part of the BNB Chain SDK. The full BNB Chain SDK
// copyright notice, including terms governing use, modification, and
// redistribution, is contained in the file LICENSE at the root of the source
// code distribution tree.

#pragma once

#include "dex.pb.h"
#include "Data.h"

#include <stdint.h>
#include <string>
#include <vector>

namespace Binance {

/// Helper class that performs BNB transaction signing.
class Signer {
public:
    /// Chain identifier.
    std::string chainId;

    /// Signer's account number.
    int64_t accountNumber;

    /// Sequence number for the next transaction.
    int64_t sequence;

    /// Source identifier, set to zero if unwilling to disclose.
    int64_t source;

    /// A short remark on the transaction.
    ///
    /// Only `Transfer` transaction allows 'memo' input, and other transactions with non-empty `memo` would be rejected.
    std::string memo;

    /// Private signing key.
    Data privateKey;

    /// Order to sign.
    const ::google::protobuf::Message& order;

    /// Initializes a transaction signer.
    Signer(const ::google::protobuf::Message& order) : chainId("chain-bnb"), accountNumber(), sequence(), source(), memo(), privateKey(), order(order) {}

    /// Builds a signed transaction.
    ///
    /// \returns the signed transaction data or an empty vector if there is an error.
    Data build() const;

    /// Signs the transaction.
    ///
    /// \returns the transaction signature or an empty vector if there is an error.
    Data sign() const;

private:
    Data encodeTransaction(const Data& signature) const;
    Data encodeOrder() const;
    Data encodeSignature(const Data& signature) const;
    Data aminoWrap(const std::string& raw, const Data& typePrefix, bool isPrefixLength) const;
};

} // namespace

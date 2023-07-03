// Copyright © 2019 Binance.
//
// This file is part of the BNB Chain SDK. The full BNB Chain SDK
// copyright notice, including terms governing use, modification, and
// redistribution, is contained in the file LICENSE at the root of the source
// code distribution tree.

#include "Signer.h"
#include "Serialization.h"

#include "crypto/ecdsa.h"
#include "crypto/secp256k1.h"
#include "crypto/sha2.h"

#include <nlohmann/json.hpp>
#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/io/zero_copy_stream_impl_lite.h>
#include <string>

using namespace Binance;

// Message prefixes
static const auto sendOrderPrefix = Data{ 0x2A, 0x2C, 0x87, 0xFA };
static const auto tradeOrderPrefix = Data{ 0xCE, 0x6D, 0xC0, 0x43 };
static const auto cancelTradeOrderPrefix = Data{ 0x16, 0x6E, 0x68, 0x1B };
static const auto tokenFreezeOrderPrefix = Data{ 0xE7, 0x74, 0xB3, 0x2D };
static const auto tokenUnfreezeOrderPrefix = Data{ 0x65, 0x15, 0xFF, 0x0D };
static const auto pubKeyPrefix = Data{ 0xEB, 0x5A, 0xE9, 0x87 };
static const auto transactionPrefix = Data{ 0xF0, 0x62, 0x5D, 0xEE };

Data Signer::build() const {
    auto signature = sign();
    if (signature.empty()) {
        return {};
    }

    auto encoded = encodeSignature(signature);
    return encodeTransaction(encoded);
}

Data Signer::sign() const {
    const auto preImage = signaturePreimage(*this);

    byte hash[SHA256_DIGEST_LENGTH];
    sha256_Raw(reinterpret_cast<const byte*>(preImage.data()), preImage.size(), hash);

    byte sig[64];
    if (-1 == ecdsa_sign_digest(&secp256k1, privateKey.data(), hash, sig, nullptr, nullptr)) {
        return {};
    }

    return Data(sig, sig + 64);
}

Data Signer::encodeTransaction(const Data& signature) const {
    auto msg = encodeOrder();
    auto transaction = Binance::Transaction();
    transaction.add_msgs(msg.data(), msg.size());
    transaction.add_signatures(signature.data(), signature.size());
    transaction.set_memo(memo);
    transaction.set_source(source);
    
    auto data = transaction.SerializeAsString();
    return aminoWrap(data, transactionPrefix, true);
}

Data Signer::encodeOrder() const {
    std::string data;
    Data prefix;
    const auto typeName = order.GetTypeName();
    if (typeName == NewOrder::descriptor()->full_name()) {
        data = order.SerializeAsString();
        prefix = tradeOrderPrefix;
    } else if (typeName == CancelOrder::descriptor()->full_name()) {
        data = order.SerializeAsString();
        prefix = cancelTradeOrderPrefix;
    } else if (typeName == Send::descriptor()->full_name()) {
        data = order.SerializeAsString();
        prefix = sendOrderPrefix;
    } else if (typeName == TokenFreeze::descriptor()->full_name()) {
        data = order.SerializeAsString();
        prefix = tokenFreezeOrderPrefix;
    } else if (typeName == TokenUnfreeze::descriptor()->full_name()) {
        data = order.SerializeAsString();
        prefix = tokenUnfreezeOrderPrefix;
    } else {
        return {};
    }
    return aminoWrap(data, prefix, false);
}

Data Signer::encodeSignature(const Data& signature) const {
    Data publicKey;
    publicKey.resize(33);
    ecdsa_get_public_key33(&secp256k1, privateKey.data(), publicKey.data());

    auto encodedPublicKey = pubKeyPrefix;
    encodedPublicKey.insert(encodedPublicKey.end(), static_cast<uint8_t>(publicKey.size()));
    encodedPublicKey.insert(encodedPublicKey.end(), publicKey.begin(), publicKey.end());

    auto object = Binance::Signature();
    object.set_pub_key(encodedPublicKey.data(), encodedPublicKey.size());
    object.set_signature(signature.data(), signature.size());
    object.set_account_number(accountNumber);
    object.set_sequence(sequence);

    return aminoWrap(object.SerializeAsString(), {}, false);
}

Data Signer::aminoWrap(const std::string& raw, const Data& typePrefix, bool prefixWithSize) const {
    const auto contentsSize = raw.size() + typePrefix.size();
    auto size = contentsSize;
    if (prefixWithSize) {
        size += 10;
    }

    std::string msg;
    msg.reserve(size);
    {
        google::protobuf::io::StringOutputStream output(&msg);
        google::protobuf::io::CodedOutputStream cos(&output);
        if (prefixWithSize) {
            cos.WriteVarint64(contentsSize);
        }
        cos.WriteRaw(typePrefix.data(), typePrefix.size());
        cos.WriteRaw(raw.data(), raw.size());
    }

    return Data(msg.begin(), msg.end());
}

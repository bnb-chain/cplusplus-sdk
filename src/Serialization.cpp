// Copyright © 2019 All BNB Chain Developers.
//
// This file is part of the BNB Chain SDK. The full BNB Chain SDK
// copyright notice, including terms governing use, modification, and
// redistribution, is contained in the file LICENSE at the root of the source
// code distribution tree.

#include "Serialization.h"

#include "Address.h"
#include "Signer.h"

using namespace Binance;
using json = nlohmann::json;

static inline std::string addressString(const std::string& bytes) {
    auto data = std::vector<uint8_t>(bytes.begin(), bytes.end());
    auto address = Address(Address::binanceHRP, data);
    return address.encode();
}

std::string Binance::signaturePreimage(const Signer& signer) {
    json j;
    j["account_number"] = std::to_string(signer.accountNumber);
    j["chain_id"] = signer.chainId;
    j["data"] = nullptr;
    j["memo"] = signer.memo;
    j["msgs"] = json::array({ orderJSON(signer.order) });
    j["sequence"] = std::to_string(signer.sequence);
    j["source"] = std::to_string(signer.source);
    return j.dump();
}

json Binance::orderJSON(const ::google::protobuf::Message& order) {
    json j;
    const auto typeName = order.GetTypeName();
    if (typeName == NewOrder::descriptor()->full_name()) {
        auto tradeOrder = reinterpret_cast<const NewOrder&>(order);
        j["id"] = tradeOrder.id();
        j["ordertype"] = 2;
        j["price"] = tradeOrder.price();
        j["quantity"] = tradeOrder.quantity();
        j["sender"] = addressString(tradeOrder.sender());
        j["side"] = tradeOrder.side();
        j["symbol"] = tradeOrder.symbol();
        j["timeinforce"] = tradeOrder.timeinforce();
    } else if (typeName == CancelOrder::descriptor()->full_name()) {
        auto cancelOrder = reinterpret_cast<const CancelOrder&>(order);
        j["refid"] = cancelOrder.refid();
        j["sender"] = cancelOrder.sender();
        j["symbol"] = cancelOrder.symbol();
    } else if (typeName == Send::descriptor()->full_name()) {
        auto send = reinterpret_cast<const Send&>(order);
        j["inputs"] = inputsJSON(send);
        j["outputs"] = outputsJSON(send);
    } else if (typeName == TokenFreeze::descriptor()->full_name()) {
        auto freeze = reinterpret_cast<const TokenFreeze&>(order);
        j["from"] = addressString(freeze.from());
        j["symbol"] = freeze.symbol();
        j["amount"] = freeze.amount();
    } else if (typeName == TokenUnfreeze::descriptor()->full_name()) {
        auto unfreeze = reinterpret_cast<const TokenUnfreeze&>(order);
        j["from"] = addressString(unfreeze.from());
        j["symbol"] = unfreeze.symbol();
        j["amount"] = unfreeze.amount();
    } else {
        throw std::invalid_argument("Invalid order type");
    }
    return j;
}

json Binance::inputsJSON(const Binance::Send& order) {
    json j = json::array();
    for (auto& input : order.inputs()) {
        json sj;
        sj["address"] = addressString(input.address());
        sj["coins"] = tokensJSON(input.coins());
        j.push_back(sj);
    }
    return j;
}

json Binance::outputsJSON(const Binance::Send& order) {
    json j = json::array();
    for (auto& output : order.outputs()) {
        json sj;
        sj["address"] = addressString(output.address());
        sj["coins"] = tokensJSON(output.coins());
        j.push_back(sj);
    }
    return j;
}

json Binance::tokensJSON(const ::google::protobuf::RepeatedPtrField<Binance::Send_Token>& tokens) {
    json j = json::array();
    for (auto& token : tokens) {
        json sj;
        sj["denom"] = token.denom();
        sj["amount"] = token.amount();
        j.push_back(sj);
    }
    return j;
}

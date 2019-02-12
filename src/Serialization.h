// Copyright © 2019 Binance.
//
// This file is part of the Binance Chain SDK. The full Binance Chain SDK
// copyright notice, including terms governing use, modification, and
// redistribution, is contained in the file LICENSE at the root of the source
// code distribution tree.

#pragma once

#include "dex.pb.h"
#include <nlohmann/json.hpp>

namespace Binance {

class Signer;

std::string signaturePreimage(const Signer& signer);
nlohmann::json orderJSON(const ::google::protobuf::Message& order);
nlohmann::json inputsJSON(const Binance::Send& order);
nlohmann::json outputsJSON(const Binance::Send& order);
nlohmann::json tokensJSON(const ::google::protobuf::RepeatedPtrField<Binance::Send_Token>& tokens);

} // namespace

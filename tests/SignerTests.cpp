// Copyright © 2019 All BNB Chain Developers.
//
// This file is part of the BNB Chain SDK. The full BNB Chain SDK
// copyright notice, including terms governing use, modification, and
// redistribution, is contained in the file LICENSE at the root of the source
// code distribution tree.

#include "Address.h"
#include "HexCoding.h"
#include "Signer.h"

#include "dex.pb.h"

#include <gtest/gtest.h>

namespace Binance {

TEST(BinanceSigner, Sign) {
    auto order = NewOrder();

    auto result = Address::decode("bnb1hgm0p7khfk85zpz5v0j8wnej3a90w709vhkdfu");
    ASSERT_TRUE(result.second);
    auto keyhash = result.first.keyHash;
    order.set_sender(keyhash.data(), keyhash.size());

    order.set_id("BA36F0FAD74D8F41045463E4774F328F4AF779E5-36");
    order.set_symbol("NNB-338_BNB");
    order.set_ordertype(2);
    order.set_side(1);
    order.set_price(136350000);
    order.set_quantity(100000000);
    order.set_timeinforce(1);

    auto signer = Signer(order);
    signer.chainId = "chain-bnb";
    signer.accountNumber = 12;
    signer.sequence = 35;
    signer.source = 1;

    signer.privateKey = parse_hex("90335b9d2153ad1a9799a3ccc070bd64b4164e9642ee1dd48053c33f9a3a05e9");

    auto signature = signer.sign();

    ASSERT_EQ(hex(signature.begin(), signature.end()), "9123cb6906bb20aeb753f4a121d4d88ff0e9750ba75b0c4e10d76caee1e7d2481290fa3b9887a6225d6997f5f939ef834ea61d596a314237c48e560da9e17b5a");
}

TEST(BinanceSigner, Build) {
    auto order = NewOrder();
    auto address = Address(Address::binanceHRP, parse_hex("b6561dcc104130059a7c08f48c64610c1f6f9064"));
    auto keyhash = address.keyHash;
    order.set_sender(keyhash.data(), keyhash.size());
    order.set_id("B6561DCC104130059A7C08F48C64610C1F6F9064-11");
    order.set_symbol("BTC-5C4_BNB");
    order.set_ordertype(2);
    order.set_side(1);
    order.set_price(100000000);
    order.set_quantity(1200000000);
    order.set_timeinforce(1);
    
    auto signer = Binance::Signer(order);
    signer.chainId = "chain-bnb";
    signer.accountNumber = 1;
    signer.sequence = 10;
    signer.privateKey = parse_hex("90335b9d2153ad1a9799a3ccc070bd64b4164e9642ee1dd48053c33f9a3a05e9");

    auto result = signer.build();

    ASSERT_EQ(hex(result.begin(), result.end()), "db01"
        "f0625dee"
        "0a65"
            "ce6dc043"
            "0a14""b6561dcc104130059a7c08f48c64610c1f6f9064"
            "122b""423635363144434331303431333030353941374330384634384336343631304331463646393036342d3131"
            "1a0b""4254432d3543345f424e42"
            "2002"
            "2801"
            "3080c2d72f"
            "3880989abc04"
            "4001"
        "126e"
            "0a26"
            "eb5ae987"
            "21029729a52e4e3c2b4a4e52aa74033eedaf8ba1df5ab6d1f518fd69e67bbd309b0e"
            "1240""2a78b6d9a108eb9440221802b626e24d80179395ac984f016db012ef1a0c16d71b4d7053e05366ae3ea2681fc8052398fda20551c965d74c5970bbc66b94b48e"
            "1801"
            "200a"
    );
}

TEST(BinanceSigner, BuildSend) {
    auto order = Send();

    auto fromKeyhash = parse_hex("40c2979694bbc961023d1d27be6fc4d21a9febe6");
    auto fromAddress = Address(Address::binanceHRP, fromKeyhash);

    auto toKeyhash = parse_hex("88b37d5e05f3699e2a1406468e5d87cb9dcceb95");
    auto toAddress = Address(Address::binanceHRP, toKeyhash);

    auto input = order.add_inputs();
    input->set_address(fromKeyhash.data(), fromKeyhash.size());
    auto inputCoin = input->add_coins();
    inputCoin->set_denom("BNB");
    inputCoin->set_amount(1'001'000'000);

    auto output = order.add_outputs();
    output->set_address(toKeyhash.data(), toKeyhash.size());
    auto outputCoin = output->add_coins();
    outputCoin->set_denom("BNB");
    outputCoin->set_amount(1'001'000'000);

    auto signer = Binance::Signer(order);
    signer.chainId = "chain-bnb";
    signer.accountNumber = 19;
    signer.sequence = 23;
    signer.memo = "test";
    signer.source = 1;
    signer.privateKey = parse_hex("95949f757db1f57ca94a5dff23314accbe7abee89597bf6a3c7382c84d7eb832");

    auto result = signer.build();

    ASSERT_EQ(hex(result.begin(), result.end()), "cc01"
        "f0625dee"
        "0a4e"
            "2a2c87fa"
            "0a23""0a1440c2979694bbc961023d1d27be6fc4d21a9febe6120b0a03424e4210c098a8dd03"
            "1223""0a1488b37d5e05f3699e2a1406468e5d87cb9dcceb95120b0a03424e4210c098a8dd03"
        "126e"
            "0a26"
            "eb5ae987"
            "21026a35920088d98c3888ca68c53dfc93f4564602606cbb87f0fe5ee533db38e502"
            "1240""c65a13440f18a155bd971ee40b9e0dd58586f5bf344e12ec4c76c439aebca8c7789bab7bfbfb4ce89aadc4a02df225b6b6efc861c13bbeb5f7a3eea2d7ffc80f"
            "1813"
            "2017"
        "1a04""74657374"
        "2001"
    );
}

} // namespace

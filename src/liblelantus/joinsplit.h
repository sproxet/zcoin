#ifndef ZCOIN_LIBLELANTUS_JOINSPLIT_H
#define ZCOIN_LIBLELANTUS_JOINSPLIT_H

#include "coin.h"
#include "lelantus_proof.h"
#include "spend_metadata.h"

namespace lelantus {

class JoinSplit {
public:
    template<typename Stream>
    JoinSplit(const Params* p,  Stream& strm):
            params(p) {
        strm >> *this;
    }

    JoinSplit(const Params* p,
              const std::vector<std::pair<PrivateCoin, uint32_t>>& Cin,
              const std::map<uint32_t, std::vector<PublicCoin>>& anonymity_sets,
              const Scalar& Vout,
              const std::vector<PrivateCoin>& Cout,
              uint64_t fee,
              const std::vector<uint256>& groupBlockHashes,
              const uint256& txHash);

    ~JoinSplit();

    bool Verify(const std::map<uint32_t, std::vector<PublicCoin>>& anonymity_sets,
                const std::vector<PublicCoin>& Cout,
                const Scalar& Vout,
                const uint256& txHash) const;

    void signMetaData(const std::vector<std::pair<PrivateCoin, uint32_t>>& Cin, const SpendMetaData& m, size_t coutSize);

    uint256 signatureHash(const SpendMetaData& m, size_t coutSize) const;

    void setVersion(unsigned int nVersion) {
        version = nVersion;
    }

    const std::vector<Scalar>& getCoinSerialNumbers();

    uint64_t getFee();

    const std::vector<uint32_t>& getCoinGroupIds();

    const std::vector<std::pair<uint32_t, uint256>>& getIdAndBlockHashes();

    int getVersion() const {
        return version;
    }

    bool getIndex(const PublicCoin& coin, const std::vector<PublicCoin>& anonymity_set, size_t& index);

    bool HasValidSerials() const;

    ADD_SERIALIZE_METHODS;
    template <typename Stream, typename Operation>
    void SerializationOp(Stream& s, Operation ser_action, int nType, int nVersion)
    {
        READWRITE(lelantusProof);
        READWRITE(coinNum);

        if (ser_action.ForRead())
        {
            groupIds.resize(coinNum);
            ecdsaSignatures.resize(coinNum);
            ecdsaPubkeys.resize(coinNum);
            for (uint8_t i = 0; i < coinNum; ++i) {
                ecdsaSignatures[i] = new unsigned char[64];
                ecdsaPubkeys[i] = new unsigned char[33];
            }
        }

        for(uint8_t i = 0; i < coinNum; i++)
        {
            READWRITE(groupIds[i]);
            READWRITE(FLATDATA(ecdsaSignatures[i]));
            READWRITE(FLATDATA(ecdsaPubkeys[i]));
        }

        READWRITE(coinGroupIdAndBlockHash);
        READWRITE(fee);
        READWRITE(version);

        if (ser_action.ForRead()) {
            serialNumbers.resize(coinNum);
            for(size_t i = 0; i < coinNum; i++) {
                secp256k1_pubkey pubkey;
                if (!secp256k1_ec_pubkey_parse(OpenSSLContext::get_context(), &pubkey, ecdsaPubkeys[i], 33)) {
                    throw std::invalid_argument("Lelantus joinsplit unserialize failed due to unable to parse ecdsaPubkey.");
                }

                serialNumbers[i] = PrivateCoin::serialNumberFromSerializedPublicKey(OpenSSLContext::get_context(), &pubkey);
            }
        }
    }

private:
    const Params* params;
    unsigned int version = 0;
    LelantusProof lelantusProof;
    uint8_t coinNum;
    std::vector<Scalar> serialNumbers;
    std::vector<uint32_t> groupIds;
    std::vector<unsigned char*> ecdsaSignatures;
    std::vector<unsigned char*> ecdsaPubkeys;
    std::vector<std::pair<uint32_t, uint256>> coinGroupIdAndBlockHash;
    uint64_t fee;

};

} //namespace lelantus

#endif //ZCOIN_LIBLELANTUS_JOINSPLIT_H

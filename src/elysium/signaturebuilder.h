// Copyright (c) 2020 The Zcoin Core Developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef ZCOIN_ELYSIUM_SIGNATUREBUILDER_H
#define ZCOIN_ELYSIUM_SIGNATUREBUILDER_H

#include "sigmaprimitives.h"
#include "ecdsa_signature.h"

#include "../base58.h"
#include "../key.h"

#include <secp256k1/include/Scalar.h>

namespace elysium {

class SignatureBuilder
{
public:
    virtual ECDSASignature Sign(CKey &signer) = 0;
    virtual bool Verify(ECDSASignature const &signature) = 0;
};

class SigmaV1SignatureBuilder : SignatureBuilder
{
protected:
    CHashWriter hasher;
    CPubKey publicKey;

public:
    SigmaV1SignatureBuilder(
        CBitcoinAddress const &receiver,
        int64_t referenceAmount,
        SigmaProof const &proof,
        CPubKey const &publicKey);

public:
    ECDSASignature Sign(CKey &key);
    bool Verify(ECDSASignature const &signature);

    CPubKey const& PublicKey();
};

}

#endif // ZCOIN_ELYSIUM_SIGNATUREBUILDER_H
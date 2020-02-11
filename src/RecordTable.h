/*
 * Souffle - A Datalog Compiler
 * Copyright (c) 2020, The Souffle Developers. All rights reserved.
 * Licensed under the Universal Permissive License v 1.0 as shown at:
 * - https://opensource.org/licenses/UPL
 * - <souffle root>/licenses/SOUFFLE-UPL.txt
 */

/************************************************************************
 *
 * @file RecordTable.h
 *
 * Data container to store Records of the Datalog program.
 *
 ***********************************************************************/

#pragma once

#include "CompiledTuple.h"
#include "ParallelUtils.h"
#include "RamTypes.h"
#include <cassert>
#include <iostream>
#include <limits>
#include <map>
#include <unordered_map>
#include <vector>

namespace souffle {

/**
 * A bidirectional mapping between tuples and reference indices.
 */
class RecordMap {
    /** The arity of the stored tuples */
    const size_t arity;

    /** The mapping from tuples to references/indices */
    std::map<std::vector<RamDomain>, RamDomain> recordToIndex;

    /** The mapping from indices to tuples */
    std::vector<std::vector<RamDomain>> indexToRecord;

public:
    explicit RecordMap(size_t arity) : arity(arity), indexToRecord(1) {}  // note: index 0 element left free

    /**
     * Packs the given tuple -- and may create a new reference if necessary.
     */
    RamDomain pack(const RamDomain* tuple) {
        std::vector<RamDomain> tmp(arity);
        for (size_t i = 0; i < arity; i++) {
            tmp[i] = tuple[i];
        }

        RamDomain index;
#pragma omp critical(record_pack)
        {
            auto pos = recordToIndex.find(tmp);
            if (pos != recordToIndex.end()) {
                index = pos->second;
            } else {
#pragma omp critical(record_unpack)
                {
                    indexToRecord.push_back(tmp);
                    index = indexToRecord.size() - 1;
                    recordToIndex[tmp] = index;

                    // assert that new index is smaller than the range
                    assert(index != std::numeric_limits<RamDomain>::max());
                }
            }
        }

        return index;
    }

    /**
     * Obtains a pointer to the tuple addressed by the given index.
     */
    RamDomain* unpack(RamDomain index) {
        RamDomain* res;

#pragma omp critical(record_unpack)
        res = &(indexToRecord[index][0]);

        return res;
    }
};

class RecordTable {
public:
    RecordTable() = default;
    virtual ~RecordTable() = default;

    /**
     * A function packing a tuple of the given arity into a reference.
     */
    RamDomain pack(RamDomain* tuple, size_t arity) {
        return getForArity(arity).pack(tuple);
    }

    /**
     * A function packing a tuple of the given arity into a reference.
     */
    template <typename Domain, std::size_t _arity>
    RamDomain pack(ram::Tuple<Domain, _arity> tuple) {
        return getForArity(_arity).pack(static_cast<RamDomain*>(tuple.data));
    }

    /**
     * A function obtaining a pointer to the tuple addressed by the given reference.
     */
    RamDomain* unpack(RamDomain ref, size_t arity) {
        return getForArity(arity).unpack(ref);
    }

    /**
     * A function obtaining a pointer to the tuple addressed by the given reference.
     */
    template <typename Domain, std::size_t _arity>
    ram::Tuple<Domain, _arity> unpackTuple(RamDomain ref) {
        ram::Tuple<RamDomain, _arity> tuple;
        RamDomain* data = getForArity(_arity).unpack(ref);

        for (size_t i = 0; i < _arity; ++i) {
            tuple.data[i] = data[i];
        }
        return tuple;
    }

    /**
     * Determines whether the given reference is the nil reference encoding
     * the absence of any nested record.
     */
    bool isNil(RamDomain ref) {
        return ref == 0;
    }

private:
    std::unordered_map<size_t, RecordMap> maps;

    RecordMap& getForArity(size_t arity) {
        std::unordered_map<size_t, RecordMap>::iterator mapsIterator;
#pragma omp critical(RecordTableGetForArity)
        {
            // This will create a new map if it doesn't exist yet.
            mapsIterator = maps.emplace(arity, arity).first;
        }
        return mapsIterator->second;
    }
};

}  // namespace souffle

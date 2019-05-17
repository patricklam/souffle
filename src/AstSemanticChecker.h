/*
 * Souffle - A Datalog Compiler
 * Copyright (c) 2015, Oracle and/or its affiliates. All rights reserved
 * Licensed under the Universal Permissive License v 1.0 as shown at:
 * - https://opensource.org/licenses/UPL
 * - <souffle root>/licenses/SOUFFLE-UPL.txt
 */

/************************************************************************
 *
 * @file AstSemanticChecker.h
 *
 * Defines the semantic checker pass.
 *
 ***********************************************************************/

#pragma once

#include "AstArgument.h"
#include "AstTransformer.h"
#include <string>

namespace souffle {

class AstAtom;
class AstClause;
class AstLiteral;
class AstProgram;
class AstRecordType;
class AstRelation;
class AstTranslationUnit;
class AstType;
class AstUnionType;
class IOType;
class ErrorReport;
class PrecedenceGraph;
class RecursiveClauses;
class TypeAnalysis;
class TypeEnvironment;

class AstSemanticChecker : public AstTransformer {
public:
    ~AstSemanticChecker() override = default;

    std::string getName() const override {
        return "AstSemanticChecker";
    }

private:
    bool transform(AstTranslationUnit& translationUnit) override;

    static void checkProgram(ErrorReport& report, const AstProgram& program, const TypeEnvironment& typeEnv,
            const TypeAnalysis& typeAnalysis, const PrecedenceGraph& precedenceGraph,
            const RecursiveClauses& recursiveClauses, const IOType& ioTypes);

    static void checkAtom(ErrorReport& report, const AstProgram& program, const AstAtom& atom);
    static void checkLiteral(ErrorReport& report, const AstProgram& program, const AstLiteral& literal);
    static void checkAggregator(
            ErrorReport& report, const AstProgram& program, const AstAggregator& aggregator);
    static void checkArgument(ErrorReport& report, const AstProgram& program, const AstArgument& arg);
    static void checkConstant(ErrorReport& report, const AstArgument& argument);
    static void checkFact(ErrorReport& report, const AstProgram& program, const AstClause& fact);
    static void checkClause(ErrorReport& report, const AstProgram& program, const AstClause& clause,
            const RecursiveClauses& recursiveClauses);
    static void checkRelationDeclaration(ErrorReport& report, const TypeEnvironment& typeEnv,
            const AstProgram& program, const AstRelation& relation, const IOType& ioTypes);
    static void checkRelation(ErrorReport& report, const TypeEnvironment& typeEnv, const AstProgram& program,
            const AstRelation& relation, const RecursiveClauses& recursiveClauses, const IOType& ioTypes);
    static void checkRules(ErrorReport& report, const TypeEnvironment& typeEnv, const AstProgram& program,
            const RecursiveClauses& recursiveClauses, const IOType& ioTypes);

    static void checkUnionType(ErrorReport& report, const AstProgram& program, const AstUnionType& type);
    static void checkRecordType(ErrorReport& report, const AstProgram& program, const AstRecordType& type);
    static void checkType(ErrorReport& report, const AstProgram& program, const AstType& type);
    static void checkTypes(ErrorReport& report, const AstProgram& program);

    static void checkNamespaces(ErrorReport& report, const AstProgram& program);
    static void checkIODirectives(ErrorReport& report, const AstProgram& program);
    static void checkWitnessProblem(ErrorReport& report, const AstProgram& program);
    static void checkInlining(ErrorReport& report, const AstProgram& program,
            const PrecedenceGraph& precedenceGraph, const IOType& ioTypes);
    static void checkGroundedness(ErrorReport& report, const AstProgram& program);
    static void checkTypeUsage(
            ErrorReport& report, const TypeEnvironment& typeEnv, const AstProgram& program);
    static void checkTypeCorrectness(
            ErrorReport& report, const TypeAnalysis& typeAnalysis, const AstProgram& program);
    static void checkStratification(
            ErrorReport& report, const AstProgram& program, const PrecedenceGraph& precedenceGraph);
};

class AstExecutionPlanChecker : public AstTransformer {
public:
    std::string getName() const override {
        return "AstExecutionPlanChecker";
    }

private:
    bool transform(AstTranslationUnit& translationUnit) override;
};

}  // end of namespace souffle

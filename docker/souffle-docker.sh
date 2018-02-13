#!/bin/sh
#
# Souffle - A Datalog Compiler
# Copyright (c) 2018, The Souffle Developers. All rights reserved.
# Licensed under the Universal Permissive License v 1.0 as shown at:
# - https://opensource.org/licenses/UPL
# - <souffle root>/licenses/SOUFFLE-UPL.txt
###
### souffle-docker.sh
###
###     Use Souffle with Docker.
###
### Usage:
###
###     ./souffle.sh <command> <options>
###
### Commands:
###
###     build   Build Souffle in a Docker container.
###     exec    Call the Souffle executable.
###     test    Run the testsuite for Souffle.
###     list    List configuration information.
###     run     Enter the given Docker container.
###
### Options:
###
###    --git-user           -u    Github username.
###    --git-branch         -b    Git branch.
###    --git-revision       -r    Git revision.
###    --docker-image       -i    Docker image.
###    --docker-tag         -t    Docker tag.
###    --souffle-options    -o    Additional options.
###    --help               -h    Show this help text.
###
### Notes:
###
###    The behaviour of the --souffle-options/-o flag is
###    dependant on the provided command. For `build`,
###    the value of the flag is passed to the configure
###    script. For `exec`, the value of the flag is passed
###    to the Souffle executable. For test, the value of
###    the flag is passed to the testsuite. For list, this
###    flag has no effect. For run, this flag has no effect.
###
### Example:
###
###     To set up Souffle in a Docker for CentOS, using the
###     master branch of upstream at the latest commit, do
###
###     $ ./docker/souffle-docker.sh build -i centos -t centos7
###
###     Alternatively, to do the same but on your own master
###     branch.
###
###     $ ./docker/souffle-docker.sh build -i centos -t centos7 -u "${USER}"
###
###     Where ${USER} is your Github username.
###
###     Replacing 'build' with 'run' puts you in a shell environment
###     of the Docker container where Souffle has been built.
###
###     This is useful as one can debug their latest changes against the
###     current upstream by running multiple containers at the same time.
###

set -e -u -o pipefail

function Help {
    cat $0 | \
        grep "^##" | \
        sed 's/^##*//g'
}

if [ $# == 0 ]
then
    Help
    exit 1
fi

GIT_USER="souffle-lang"
GIT_BRANCH="master"
GIT_REVISION="HEAD"
DOCKER_IMAGE="souffle"
DOCKER_TAG="latest"
SOUFFLE_OPTIONS=""
COMMAND="${1}"
shift

while [ $# != 0 ]
do
    case "${1}" in
    --*)
        case "${1}" in
        --git-user)
            shift
            GIT_USER="${1}"
            ;;
        --git-branch)
            shift
            GIT_BRANCH="${1}"
            ;;
        --git-revision)
            shift
            GIT_REVISION="${1}"
            ;;
        --docker-image)
            shift
            DOCKER_IMAGE="${1}"
            ;;
        --docker-tag)
            shift
            DOCKER_TAG="${1}"
            ;;
        --souffle-options)
            shift
            SOUFFLE_OPTIONS="${1}"
            ;;
        --help)
            shift
            DOCKER_TAG="${1}"
            ;;
        esac
        ;;
    -*)
        case "${1}" in
        -u)
            shift
            GIT_USER="${1}"
            ;;
        -b)
            shift
            GIT_BRANCH="${1}"
            ;;
        -r)
            shift
            GIT_REVISION="${1}"
            ;;
        -i)
            shift
            DOCKER_IMAGE="${1}"
            ;;
        -t)
            shift
            DOCKER_TAG="${1}"
            ;;
        -o)
            shift
            SOUFFLE_OPTIONS="${1}"
            ;;
        -h)
            Help
            exit 0
            ;;
        esac
        ;;
    *)
        Help
        exit 1
        ;;
    esac
    shift
done

case "${COMMAND}" in
build)
    docker build \
        --rm \
        -t "$(echo souffle_${DOCKER_IMAGE}_${DOCKER_TAG}_${GIT_USER}_${GIT_BRANCH}_${GIT_REVISION} | tr [A-Z] [a-z])" \
        -f docker/Dockerfile.${DOCKER_IMAGE}_${DOCKER_TAG} \
        --build-arg GIT_USER=${GIT_USER} \
        --build-arg GIT_BRANCH=${GIT_BRANCH} \
        --build-arg GIT_REVISION=${GIT_REVISION} \
        --build-arg SOUFFLE_OPTIONS=${SOUFFLE_OPTIONS} \
        ${PWD}
    ;;
exec)
    docker run \
        --rm \
        -t "$(echo souffle_${DOCKER_IMAGE}_${DOCKER_TAG}_${GIT_USER}_${GIT_BRANCH}_${GIT_REVISION} | tr [A-Z] [a-z])" \
        -i \
        /bin/bash -c "/souffle/src/souffle ${SOUFFLE_OPTIONS}"
    ;;
test)
    docker run \
        --rm \
        -t "$(echo souffle_${DOCKER_IMAGE}_${DOCKER_TAG}_${GIT_USER}_${GIT_BRANCH}_${GIT_REVISION} | tr [A-Z] [a-z])" \
        -i \
        /bin/bash -c "make check ${SOUFFLE_OPTIONS}"
    ;;
list)
    message=""
    message+="docker-image_docker-tag\\n"
    message+="$(ls -a docker/Dockerfile* | sed 's/docker\/Dockerfile\.//')\\n"
    echo "${message}" | column -t -s $'_'
    ;;
run)
    docker run \
        --rm \
        -t "$(echo souffle_${DOCKER_IMAGE}_${DOCKER_TAG}_${GIT_USER}_${GIT_BRANCH}_${GIT_REVISION} | tr [A-Z] [a-z])" \
        -i \
        /bin/bash
    ;;
esac

exit 0

FROM ubuntu:22.04

LABEL maintainer="Hebe contributors"

ENV DEBIAN_FRONTEND=noninteractive
SHELL ["/bin/bash", "-o", "pipefail", "-c"]

ARG BUILD_ESSENTIAL_PKG=build-essential
ARG CMAKE_PKG=cmake
ARG NINJA_PKG=ninja-build
ARG FLEX_PKG=flex
ARG BISON_PKG=bison
ARG LLVM_PKG=llvm
ARG LLVM_DEV_PKG=llvm-dev
ARG CLANG_PKG=clang
ARG CA_CERTIFICATES_PKG=ca-certificates
ARG GIT_PKG=git
ARG WGET_PKG=wget
ARG CLANGD_PKG=clangd
ARG CLANGD17_PKG=clangd-17
ARG CLANG_FORMAT_PKG=clang-format
ARG GDB_PKG=gdb

RUN set -eux; \
    apt-get update; \
    apt-get install -y --no-install-recommends \
        ca-certificates gnupg wget lsb-release; \
    \
    # Add apt.llvm.org repo key (modern keyring approach)
    wget -qO- https://apt.llvm.org/llvm-snapshot.gpg.key | gpg --dearmor > /usr/share/keyrings/llvm.gpg; \
    \
    # Add LLVM 17 repo for Ubuntu 22.04 (jammy)
    echo "deb [signed-by=/usr/share/keyrings/llvm.gpg] http://apt.llvm.org/jammy/ llvm-toolchain-jammy-17 main" \
      > /etc/apt/sources.list.d/llvm17.list; \
    \
    apt-get update; \
    apt-get install -y --no-install-recommends \
        ${BUILD_ESSENTIAL_PKG:-build-essential} \
        ${CMAKE_PKG:-cmake} \
        ${NINJA_PKG:-ninja-build} \
        ${FLEX_PKG:-flex} \
        ${BISON_PKG:-bison} \
        ${LLVM_PKG:-llvm} \
        ${LLVM_DEV_PKG:-llvm-dev} \
        ${CLANG_PKG:-clang} \
        ${GIT_PKG:-git} \
        ${WGET_PKG:-wget} \
        ${CLANG_FORMAT_PKG:-clang-format} \
        ${GDB_PKG:-gdb} \
        clangd-17; \
    \
    # Make clangd default (optional, but convenient)
    update-alternatives --install /usr/bin/clangd clangd /usr/bin/clangd-17 100; \
    \
    rm -rf /var/lib/apt/lists/*

# Make llvm-config available for CMake's execute_process calls
ENV LLVM_CONFIG=/usr/bin/llvm-config

WORKDIR /workspace

# Default to bash so the container can be used interactively for development.
CMD ["/bin/bash"]

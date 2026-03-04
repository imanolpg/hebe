FROM ubuntu:22.04

LABEL maintainer="Hebe contributors"

ENV DEBIAN_FRONTEND=noninteractive

SHELL ["/bin/bash", "-o", "pipefail", "-c"]

ARG BUILD_ESSENTIAL_PKG=build-essential
ARG CMAKE_PKG=cmake
ARG NINJA_PKG=ninja-build
ARG FLEX_PKG=flex
ARG BISON_PKG=bison
ARG LLVM_21=llvm-21
ARG LLVM_21_DEV=llvm-21-dev
ARG CLANG_21=clang-21
ARG CLANGD_21=clangd-21
ARG CLANG_FORMAT_21=clang-format-21
ARG LIBCPP_21=libc++-21-dev
ARG LIBCPP_ABI_21_DEV=libc++abi-21-dev
ARG LIBPATHREAD_STUBS0_DEV=libpthread-stubs0-dev
ARG CA_CERTIFICATES_PKG=ca-certificates
ARG GIT_PKG=git
ARG WGET_PKG=wget
ARG GDB_PKG=gdb

RUN set -eux; \
    apt-get update; \
    apt-get install -y --no-install-recommends \
        ca-certificates gnupg wget lsb-release; \
    \
# Add apt.llvm.org repo key (modern keyring approach)
    wget -qO- https://apt.llvm.org/llvm-snapshot.gpg.key | gpg --dearmor > /usr/share/keyrings/llvm.gpg; \
    \
# Add LLVM 21 repo for Ubuntu 22.04 (jammy)
    echo "deb [signed-by=/usr/share/keyrings/llvm.gpg] http://apt.llvm.org/jammy/ llvm-toolchain-jammy-21 main" \
      > /etc/apt/sources.list.d/llvm21.list; \
    \
    apt-get update; \
    apt-get install -y --no-install-recommends \
        ${BUILD_ESSENTIAL_PKG:-build-essential} \
        ${CMAKE_PKG:-cmake} \
        ${NINJA_PKG:-ninja-build} \
        ${FLEX_PKG:-flex} \
        ${BISON_PKG:-bison} \
        ${LLVM_21:-llvm-21}\
        ${LLVM_21_DEV:-llvm-21-dev}\
        ${CLANG_21:-clang-21}\
        ${CLANGD_21:-clangd-21}\
        ${CLANG_FORMAT_21:-clang-format-21}\
        ${LIBCPP_21_DEV:-libc++-21-dev}\
        ${LIBCPP_ABI_21_DEV:-libc++abi-21-dev}\
        ${LIBPATHREAD_STUBS0_DEV:-libpthread-stubs0-dev}\
        ${GIT_PKG:-git} \
        ${WGET_PKG:-wget} \
        ${GDB_PKG:-gdb}; \
    \
# Make clang-21 the default clang
    update-alternatives --install /usr/bin/clang   clang   /usr/bin/clang-21   100; \
    update-alternatives --install /usr/bin/clang++ clang++ /usr/bin/clang++-21 100; \
    update-alternatives --install /usr/bin/clangd  clangd  /usr/bin/clangd-21  100; \
    update-alternatives --install /usr/bin/clang-format clang-format /usr/bin/clang-format-21 100; \
    update-alternatives --install /usr/bin/llvm-config llvm-config /usr/bin/llvm-config-21 100; \
    \
    rm -rf /var/lib/apt/lists/*


# Add Kitware repo for latest CMake
RUN set -eux; \
    wget -qO- https://apt.kitware.com/keys/kitware-archive-latest.asc | gpg --dearmor > /usr/share/keyrings/kitware.gpg; \
    echo "deb [signed-by=/usr/share/keyrings/kitware.gpg] https://apt.kitware.com/ubuntu/ jammy main" \
      > /etc/apt/sources.list.d/kitware.list; \
    apt-get update; \
    apt-get install -y --no-install-recommends cmake; \
    rm -rf /var/lib/apt/lists/*

ENV LLVM_CONFIG=/usr/bin/llvm-config

WORKDIR /workspace

CMD ["/bin/bash"]
FROM ubuntu:22.04
LABEL maintainer="Hebe contributors"
ENV DEBIAN_FRONTEND=noninteractive
SHELL ["/bin/bash", "-o", "pipefail", "-c"]
ARG BUILD_ESSENTIAL_PKG=build-essential
ARG CMAKE_PKG=cmake
ARG NINJA_PKG=ninja-build
ARG FLEX_PKG=flex
ARG BISON_PKG=bison
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
        llvm-21 \
        llvm-21-dev \
        clang-21 \
        clangd-21 \
        clang-format-21 \
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

ENV LLVM_CONFIG=/usr/bin/llvm-config
WORKDIR /workspace
CMD ["/bin/bash"]
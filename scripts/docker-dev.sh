#!/usr/bin/env bash
set -euo pipefail

# Determine repository root relative to this script.
ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
IMAGE_NAME="${IMAGE_NAME:-hebe-dev}"

BUILD_ARGS=()
if [[ -n "${DOCKER_BUILD_ARGS:-}" ]]; then
  # Intentionally rely on word splitting so callers can provide multiple args.
  # shellcheck disable=SC2206
  BUILD_ARGS=(${DOCKER_BUILD_ARGS})
fi

docker build "${BUILD_ARGS[@]}" -t "${IMAGE_NAME}" "${ROOT_DIR}"

if [[ $# -eq 0 ]]; then
  set -- /bin/bash
fi

docker run --rm -it \
  -v "${ROOT_DIR}":/workspace \
  -w /workspace \
  "${IMAGE_NAME}" \
  "$@"

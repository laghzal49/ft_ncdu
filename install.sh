#!/usr/bin/env bash

set -eu

SCRIPT_DIR=""
if [ -n "${BASH_SOURCE[0]:-}" ]; then
	SCRIPT_DIR="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd -P)"
fi
if [ -n "$SCRIPT_DIR" ] && [ -x "$SCRIPT_DIR/ft_ncdu/install.sh" ]; then
	exec "$SCRIPT_DIR/ft_ncdu/install.sh"
fi

TEMP_SCRIPT="$(mktemp "${TMPDIR:-/tmp}/ft_ncdu_installer.XXXXXX")"
cleanup()
{
	rm -f "$TEMP_SCRIPT"
}
trap cleanup EXIT HUP INT TERM

curl -fsSL \
	"https://raw.githubusercontent.com/laghzal49/ft_ncdu/main/ft_ncdu/install.sh" \
	-o "$TEMP_SCRIPT"
bash "$TEMP_SCRIPT"

#!/bin/bash
BRIDGE_BIN="/home/jildert/of_v0.12.0-rc1_linux64gcc6_release/apps/myApps/espnowBridge/bin/espnowBridge"
BRIDGE_DIR="$(dirname "$BRIDGE_BIN")"

if ! pgrep -s 0 '^sudo$' > /dev/null ; then
	echo "Run as sudo"
	exit 1
fi

cleanup() {
	echo ""
	echo "Stopping monitor mode..."
	monitorMode stop
}
trap cleanup EXIT

monitorMode start "$@"

# Verify monitor mode is active
if ! iwconfig wlp166s0 2>/dev/null | grep -q "Mode:Monitor"; then
	echo "Failed to enter monitor mode"
	exit 1
fi

echo "Starting espnowBridge..."
cd "$BRIDGE_DIR" && ./espnowBridge

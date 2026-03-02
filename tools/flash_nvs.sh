#!/usr/bin/env bash
# flash_nvs.sh — generate and flash the NVS partition from nvs_config.csv
#
# Usage:
#   ./tools/flash_nvs.sh [PORT]
#
# PORT defaults to /dev/ttyUSB0. Edit NVS_CSV to point to a per-device copy
# of nvs_config.csv before running.
#
# NVS partition layout (from partitions_singleapp.csv):
#   offset = 0x9000, size = 0x6000 (24 KiB)

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
REPO_ROOT="$(cd "$SCRIPT_DIR/.." && pwd)"

NVS_CSV="${SCRIPT_DIR}/nvs_config.csv"
NVS_BIN="${SCRIPT_DIR}/nvs.bin"
NVS_SIZE="0x6000"   # 24 KiB — must match partition table
NVS_OFFSET="0x9000" # default singleapp NVS offset
PORT="${1:-/dev/ttyUSB0}"

# Source ESP-IDF if idf.py is not already on PATH
if ! command -v idf.py &>/dev/null; then
    source "$HOME/esp-idf/export.sh"
fi

NVS_GEN="$IDF_PATH/components/nvs_flash/nvs_partition_generator/nvs_partition_gen.py"

echo "=== Generating NVS partition image ==="
echo "  CSV    : $NVS_CSV"
echo "  Output : $NVS_BIN"
echo "  Size   : $NVS_SIZE"
python3 "$NVS_GEN" generate "$NVS_CSV" "$NVS_BIN" "$NVS_SIZE"

echo ""
echo "=== Flashing NVS to $PORT at $NVS_OFFSET ==="
python3 -m esptool --chip esp32s3 -p "$PORT" -b 460800 \
    --before default_reset --after hard_reset \
    write_flash "$NVS_OFFSET" "$NVS_BIN"

echo ""
echo "Done. The device NVS has been written."
echo "Power-cycle or reset the board to apply."

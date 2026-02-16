import serial
import simple_pb2
import sys
import struct

# Configure your UART port here
SERIAL_PORT = '/dev/ttyUSB0' # Update this to your actual port
BAUD_RATE = 115200

FRAME_START = 0xAA
FRAME_END = 0xBB

def crc16_ccitt(data):
    crc = 0xFFFF
    for byte in data:
        crc ^= byte << 8
        for _ in range(8):
            if crc & 0x8000:
                crc = (crc << 1) ^ 0x1021
            else:
                crc <<= 1
            crc &= 0xFFFF
    return crc

def send_command(ser, cmd_obj):
    payload = cmd_obj.SerializeToString()
    length = len(payload)
    
    if length > 255:
        print("Error: Payload too large for 1-byte length field.")
        return

    crc = crc16_ccitt(payload)
    
    # Construct frame: START | LEN | DATA | CRC_H | CRC_L | END
    frame = bytearray()
    frame.append(FRAME_START)
    frame.append(length)
    frame.extend(payload)
    frame.append((crc >> 8) & 0xFF)
    frame.append(crc & 0xFF)
    frame.append(FRAME_END)
    
    ser.write(frame)
    print(f"Sent {len(frame)} byte frame (payload: {length} bytes, CRC: 0x{crc:04X})")

def main():
    try:
        ser = serial.Serial(SERIAL_PORT, BAUD_RATE, timeout=1)
    except Exception as e:
        print(f"Error opening serial port: {e}")
        return

    print("Robust UART Controller Ready.")
    print("Commands:")
    print("  setbrightness:[id],<val>")
    print("  setchannel:[id],<ch>,<val>")
    print("  setwifi:[id],<ssid>,<pass>")
    print("  setid:[id],<new_id>")
    print("  Note: [id] is optional. If omitted, id=0 (broadcast) is used.")
    print("  Example: setbrightness:10,0.5  (set brightness of device 10 to 0.5)")
    print("  Example: setbrightness:0.5     (set brightness of ALL devices to 0.5)")
    print("  exit")

    while True:
        try:
            line = input("> ").strip()
            if not line: continue
            if line == "exit": break

            cmd = simple_pb2.Command()
            cmd.id = 0 # Default broadcast

            if ":" in line:
                cmd_name, args_str = line.split(":", 1)
                args = args_str.split(",")
                
                # Check if first arg is likely an ID (integer) or a value
                # Simple heuristic: if it's setid, or if there's more than the required args
                has_id = False
                if cmd_name == "setbrightness" and len(args) == 2: has_id = True
                if cmd_name == "setchannel" and len(args) == 3: has_id = True
                if cmd_name == "setwifi" and len(args) == 3: has_id = True
                if cmd_name == "setid" and len(args) == 2: has_id = True

                arg_idx = 0
                if has_id:
                    cmd.id = int(args[0])
                    arg_idx = 1

                if cmd_name == "setbrightness":
                    cmd.led.brightness = float(args[arg_idx])
                    send_command(ser, cmd)
                elif cmd_name == "setchannel":
                    cmd.channel.channel = int(args[arg_idx])
                    cmd.channel.value = float(args[arg_idx+1])
                    send_command(ser, cmd)
                elif cmd_name == "setwifi":
                    cmd.wifi.ssid = args[arg_idx]
                    cmd.wifi.password = args[arg_idx+1] if len(args) > arg_idx+1 else ""
                    send_command(ser, cmd)
                elif cmd_name == "setid":
                    cmd.set_id.id = int(args[arg_idx])
                    send_command(ser, cmd)
                else:
                    print(f"Unknown command: {cmd_name}")
            else:
                print("Unknown command format. Use name:args")

        except ValueError as e:
            print(f"Invalid value: {e}")
        except IndexError:
            print("Missing arguments.")
        except KeyboardInterrupt:
            break
        except Exception as e:
            print(f"Error: {e}")

    ser.close()

if __name__ == "__main__":
    main()

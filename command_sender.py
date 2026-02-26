import serial
import simple_pb2
import sys
import struct
import argparse
from pythonosc import udp_client

# Constants for UART framing
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

def send_uart(ser, cmd_obj):
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
    print(f"Sent UART {len(frame)} byte frame (payload: {length} bytes, CRC: 0x{crc:04X})")

def send_osc(client, cmd_obj):
    payload = cmd_obj.SerializeToString()
    # Send as a blob (bytes) to /espnow
    client.send_message("/espnow", payload)
    print(f"Sent OSC blob to /espnow ({len(payload)} bytes)")

def main():
    parser = argparse.ArgumentParser(description="Command Sender for JFixLib")
    parser.add_argument("-m", "--mode", choices=["uart", "osc"], default="uart", help="Output mode: uart or osc")
    parser.add_argument("-port", "--serial_port", default="/dev/ttyUSB0", help="Serial port for UART mode")
    parser.add_argument("-ip", "--ip", default="127.0.0.1", help="Target IP for OSC mode")
    parser.add_argument("-p", "--osc_port", type=int, default=7997, help="Target port for OSC mode")
    parser.add_argument("-b", "--baud", type=int, default=115200, help="Baud rate for UART mode")
    
    args = parser.parse_args()

    sender_func = None
    output_obj = None

    if args.mode == "uart":
        try:
            output_obj = serial.Serial(args.serial_port, args.baud, timeout=1)
            sender_func = send_uart
            print(f"Mode: UART on {args.serial_port} ({args.baud} baud)")
        except Exception as e:
            print(f"Error opening serial port: {e}")
            return
    else:
        try:
            output_obj = udp_client.SimpleUDPClient(args.ip, args.osc_port)
            sender_func = send_osc
            print(f"Mode: OSC to {args.ip}:{args.osc_port}")
        except Exception as e:
            print(f"Error initializing OSC client: {e}")
            return

    print("\nCommands:")
    print("  setbrightness:[id],<val>")
    print("  setchannel:[id],<ch>,<val>")
    print("  setwifi:[id],<ssid>,<pass>")
    print("  setid:[id],<new_id>")
    print("  setmotor:[id],<steps>,<speed>,[relative=1]")
    print("  setblink:[id],<on_ms>,<off_ms>")
    print("  Note: [id] is optional. If omitted, id=0 (broadcast) is used.")
    print("  Example: setbrightness:10,0.5")
    print("  exit")

    while True:
        try:
            line = input("> ").strip()
        except EOFError:
            break
            
        if not line: continue
        if line == "exit": break

        try:
            cmd = simple_pb2.Command()
            cmd.id = 0 # Default broadcast

            if ":" in line:
                # Remove trailing semicolon
                line = line.rstrip(";")
                cmd_name, args_str = line.split(":", 1)
                cmd_args = [a.strip() for a in args_str.split(",")]
                
                # Heuristic to check if first arg is an ID
                has_id = False
                if cmd_name == "setbrightness" and len(cmd_args) == 2: has_id = True
                if cmd_name == "setchannel" and len(cmd_args) == 3: has_id = True
                if cmd_name == "setwifi" and len(cmd_args) == 3: has_id = True
                if cmd_name == "setid" and len(cmd_args) == 2: has_id = True
                if cmd_name == "setmotor" and len(cmd_args) >= 3:
                    if len(cmd_args) == 4: has_id = True
                    elif len(cmd_args) == 3 and cmd_args[0].isdigit() and int(cmd_args[0]) < 100: has_id = True
                if cmd_name == "setblink" and len(cmd_args) == 3: has_id = True

                arg_idx = 0
                if has_id:
                    cmd.id = int(float(cmd_args[0]))
                    arg_idx = 1

                if cmd_name == "setbrightness":
                    cmd.led.brightness = float(cmd_args[arg_idx])
                    sender_func(output_obj, cmd)
                elif cmd_name == "setchannel":
                    cmd.channel.channel = int(float(cmd_args[arg_idx]))
                    cmd.channel.value = float(cmd_args[arg_idx+1])
                    sender_func(output_obj, cmd)
                elif cmd_name == "setwifi":
                    cmd.wifi.ssid = cmd_args[arg_idx]
                    cmd.wifi.password = cmd_args[arg_idx+1] if len(cmd_args) > arg_idx+1 else ""
                    sender_func(output_obj, cmd)
                elif cmd_name == "setid":
                    cmd.set_id.id = int(float(cmd_args[arg_idx]))
                    sender_func(output_obj, cmd)
                elif cmd_name == "setmotor":
                    cmd.motor.steps = int(float(cmd_args[arg_idx]))
                    cmd.motor.speed = float(cmd_args[arg_idx+1])
                    cmd.motor.relative = bool(int(float(cmd_args[arg_idx+2]))) if len(cmd_args) > arg_idx+2 else True
                    sender_func(output_obj, cmd)
                elif cmd_name == "setblink":
                    cmd.blink.on_ms = int(float(cmd_args[arg_idx]))
                    cmd.blink.off_ms = int(float(cmd_args[arg_idx+1]))
                    sender_func(output_obj, cmd)
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

    if args.mode == "uart":
        output_obj.close()

if __name__ == "__main__":
    main()

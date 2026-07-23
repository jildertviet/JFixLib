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
    # Prepend 2-byte big-endian length prefix (matches processIncomingBuffer)
    pb_len = len(payload)
    prefixed = bytes([(pb_len >> 8) & 0xFF, pb_len & 0xFF]) + payload
    length = len(prefixed)

    if length > 255:
        print("Error: Payload too large for 1-byte length field.")
        return

    crc = crc16_ccitt(prefixed)

    # Construct frame: START | LEN | DATA | CRC_H | CRC_L | END
    frame = bytearray()
    frame.append(FRAME_START)
    frame.append(length)
    frame.extend(prefixed)
    frame.append((crc >> 8) & 0xFF)
    frame.append(crc & 0xFF)
    frame.append(FRAME_END)

    ser.write(frame)
    print(f"Sent UART {len(frame)} byte frame (payload: {pb_len} bytes, CRC: 0x{crc:04X})")

def send_osc(client, cmd_obj):
    payload = cmd_obj.SerializeToString()
    # Prepend 2-byte big-endian length prefix (matches JPb.command / processIncomingBuffer)
    length = len(payload)
    prefixed = bytes([(length >> 8) & 0xFF, length & 0xFF]) + payload
    client.send_message("/espnow", prefixed)
    print(f"Sent OSC blob to /espnow ({len(prefixed)} bytes, payload: {length})")

def parse_var(var_str):
    """Convert a var argument to its int32 value.
    Accepts a single ASCII character (e.g. 'b', 'x') or a decimal integer."""
    if len(var_str) == 1 and not var_str.isdigit():
        return ord(var_str)
    return int(float(var_str))

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

    print("\nCommands ([id] optional, omit for broadcast; 255 also broadcasts):")
    print("  setbrightness:[id,]<val>")
    print("  setchannel:[id,]<ch>,<val>")
    print("  setwifi:[id,]<ssid>,<pass>")
    print("  setid:<mac>,<new_id>  (mac as aa:bb:cc:dd:ee:ff)")
    print("  setmotor:[id,]<steps>,<speed>[,<relative=1>]")
    print("  setblink:[id,]<on_ms>,<off_ms>")
    print("  sleep:[id,]<duration_ms>  (0 = indefinite deep-sleep)")
    print("  lag:[id,]<lagger_id>,<lag_time_ms>")
    print("  deleteevents:[id]")
    print("  sync:[id,]<event_id>")
    print("  reboot:[id]")
    print("  setotaurl:[id,]<url>")
    print("  setparambus:[id,]<bus_index>,<value>")
    print("  setbackground:[id,]<r>,<g>,<b>,<a>")
    print("  setbootstate:[id,]<r>,<g>,<b>,<w>,<brightness>")
    print("  -- Animation (ID required; use 255 for broadcast) --")
    print("  addevent:<id>,<event_id>,<type>,<loc_x>,<loc_y>,<size_x>,<size_y>,<r>,<g>,<b>,<a>[,<wait>]")
    print("    type: 1=perlin  2=rect  3=osc")
    print("  addenv:<id>,<event_id>,<var>,<attack_ms>,<sustain_ms>,<release_ms>,<target>[,<kill>]")
    print("  setval:<id>,<event_id>,<var>,<value>")
    print("  setval:<id>,<event_id>,c,<r>,<g>,<b>,<a>  (full colour)")
    print("  setvaln:<id>,<event_id>,<var>,<val0>[,<val1>,...]")
    print("  setcustom:<id>,<event_id>,<arg_id>,<value>")
    print("  linkbus:<id>,<event_id>,<var>,<bus_index>")
    print("  var chars: b=brightness x,y=loc w,h=size r,g,B=colour f=osc-freq R=osc-range o=offset q=wavetable")
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
            cmd.id = 255  # Default broadcast

            if ":" in line:
                # Remove trailing semicolon
                line = line.rstrip(";")
                cmd_name, args_str = line.split(":", 1)
                cmd_args = [a.strip() for a in args_str.split(",")]
                args_empty = args_str.strip() == ""

                # Determine whether the first arg is a device ID (based on arg count).
                # Animation commands always require the ID as the first argument.
                has_id = False
                if cmd_name == "setbrightness" and len(cmd_args) == 2: has_id = True
                if cmd_name == "setchannel"    and len(cmd_args) == 3: has_id = True
                if cmd_name == "setwifi"       and len(cmd_args) == 3: has_id = True
                # setid never uses the [id,...] prefix — MAC is the identifier
                if cmd_name == "setmotor" and len(cmd_args) >= 3:
                    if len(cmd_args) == 4: has_id = True
                    elif len(cmd_args) == 3 and cmd_args[0].isdigit() and int(cmd_args[0]) < 100: has_id = True
                if cmd_name == "setblink"    and len(cmd_args) == 3: has_id = True
                if cmd_name == "sleep"       and len(cmd_args) == 2: has_id = True
                if cmd_name == "lag"         and len(cmd_args) == 3: has_id = True
                if cmd_name == "deleteevents" and not args_empty and len(cmd_args) == 1: has_id = True
                if cmd_name == "reboot"       and not args_empty and len(cmd_args) == 1: has_id = True
                if cmd_name == "sync"        and len(cmd_args) == 2: has_id = True
                if cmd_name == "setotaurl"   and len(cmd_args) == 2: has_id = True
                if cmd_name == "setparambus"   and len(cmd_args) == 3: has_id = True
                if cmd_name == "setbackground" and len(cmd_args) == 5: has_id = True
                if cmd_name == "setbootstate" and len(cmd_args) == 6: has_id = True
                if cmd_name in ("addevent", "addenv", "setval", "setvaln", "setcustom", "linkbus"):
                    has_id = True  # ID always required for animation commands

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
                    # setid:<mac>,<new_id>  e.g. setid:aa:bb:cc:dd:ee:ff,5
                    mac_str = cmd_args[arg_idx]
                    cmd.set_id.mac = bytes(int(b, 16) for b in mac_str.split(":"))
                    cmd.set_id.id = int(float(cmd_args[arg_idx+1]))
                    cmd.id = 255  # broadcast so it reaches the target regardless of current ID
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
                elif cmd_name == "sleep":
                    cmd.sleep.duration_ms = int(float(cmd_args[arg_idx]))
                    sender_func(output_obj, cmd)
                elif cmd_name == "lag":
                    cmd.lag.lagger_id = int(float(cmd_args[arg_idx]))
                    cmd.lag.lag_time_ms = float(cmd_args[arg_idx+1])
                    sender_func(output_obj, cmd)
                elif cmd_name == "deleteevents":
                    cmd.delete_events.CopyFrom(simple_pb2.DeleteEventsCmd())
                    sender_func(output_obj, cmd)
                elif cmd_name == "sync":
                    cmd.sync.event_id = int(float(cmd_args[arg_idx]))
                    sender_func(output_obj, cmd)
                elif cmd_name == "reboot":
                    cmd.reboot.CopyFrom(simple_pb2.RebootCmd())
                    sender_func(output_obj, cmd)
                elif cmd_name == "setotaurl":
                    cmd.set_ota_url.url = cmd_args[arg_idx]
                    sender_func(output_obj, cmd)
                elif cmd_name == "setparambus":
                    cmd.set_param_bus.bus_index = int(float(cmd_args[arg_idx]))
                    cmd.set_param_bus.value = float(cmd_args[arg_idx+1])
                    sender_func(output_obj, cmd)
                elif cmd_name == "setbackground":
                    cmd.set_background.r = float(cmd_args[arg_idx])
                    cmd.set_background.g = float(cmd_args[arg_idx+1])
                    cmd.set_background.b = float(cmd_args[arg_idx+2])
                    cmd.set_background.a = float(cmd_args[arg_idx+3])
                    sender_func(output_obj, cmd)
                elif cmd_name == "setbootstate":
                    cmd.set_boot_state.r = float(cmd_args[arg_idx])
                    cmd.set_boot_state.g = float(cmd_args[arg_idx+1])
                    cmd.set_boot_state.b = float(cmd_args[arg_idx+2])
                    cmd.set_boot_state.w = float(cmd_args[arg_idx+3])
                    cmd.set_boot_state.brightness = float(cmd_args[arg_idx+4])
                    sender_func(output_obj, cmd)
                elif cmd_name == "addevent":
                    # addevent:<id>,<event_id>,<type>,<loc_x>,<loc_y>,<size_x>,<size_y>,<r>,<g>,<b>,<a>[,<wait>]
                    cmd.add_event.event_id = int(float(cmd_args[arg_idx]))
                    cmd.add_event.type = int(float(cmd_args[arg_idx+1]))
                    cmd.add_event.loc_x  = float(cmd_args[arg_idx+2])
                    cmd.add_event.loc_y  = float(cmd_args[arg_idx+3])
                    cmd.add_event.size_x = float(cmd_args[arg_idx+4])
                    cmd.add_event.size_y = float(cmd_args[arg_idx+5])
                    cmd.add_event.r = float(cmd_args[arg_idx+6])
                    cmd.add_event.g = float(cmd_args[arg_idx+7])
                    cmd.add_event.b = float(cmd_args[arg_idx+8])
                    cmd.add_event.a = float(cmd_args[arg_idx+9])
                    if len(cmd_args) > arg_idx+10:
                        cmd.add_event.wait_for_env = bool(int(float(cmd_args[arg_idx+10])))
                    sender_func(output_obj, cmd)
                elif cmd_name == "addenv":
                    # addenv:<id>,<event_id>,<var>,<attack_ms>,<sustain_ms>,<release_ms>,<target>[,<kill>]
                    cmd.add_env.event_id   = int(float(cmd_args[arg_idx]))
                    cmd.add_env.var        = parse_var(cmd_args[arg_idx+1])
                    cmd.add_env.attack_ms  = int(float(cmd_args[arg_idx+2]))
                    cmd.add_env.sustain_ms = int(float(cmd_args[arg_idx+3]))
                    cmd.add_env.release_ms = int(float(cmd_args[arg_idx+4]))
                    cmd.add_env.target     = float(cmd_args[arg_idx+5])
                    if len(cmd_args) > arg_idx+6:
                        cmd.add_env.kill = bool(int(float(cmd_args[arg_idx+6])))
                    sender_func(output_obj, cmd)
                elif cmd_name == "setval":
                    # setval:<id>,<event_id>,<var>,<value>
                    # setval:<id>,<event_id>,c,<r>,<g>,<b>,<a>
                    cmd.set_val.event_id = int(float(cmd_args[arg_idx]))
                    var_str = cmd_args[arg_idx+1]
                    cmd.set_val.var = parse_var(var_str)
                    if var_str == 'c':
                        cmd.set_val.r = float(cmd_args[arg_idx+2])
                        cmd.set_val.g = float(cmd_args[arg_idx+3])
                        cmd.set_val.b = float(cmd_args[arg_idx+4])
                        cmd.set_val.a = float(cmd_args[arg_idx+5])
                    else:
                        cmd.set_val.value = float(cmd_args[arg_idx+2])
                    sender_func(output_obj, cmd)
                elif cmd_name == "setvaln":
                    # setvaln:<id>,<event_id>,<var>,<val0>[,<val1>,...]
                    cmd.set_val_n.event_id = int(float(cmd_args[arg_idx]))
                    cmd.set_val_n.var      = parse_var(cmd_args[arg_idx+1])
                    for v in cmd_args[arg_idx+2:]:
                        cmd.set_val_n.values.append(float(v))
                    sender_func(output_obj, cmd)
                elif cmd_name == "setcustom":
                    # setcustom:<id>,<event_id>,<arg_id>,<value>
                    cmd.set_custom.event_id = int(float(cmd_args[arg_idx]))
                    cmd.set_custom.arg_id   = int(float(cmd_args[arg_idx+1]))
                    cmd.set_custom.value    = float(cmd_args[arg_idx+2])
                    sender_func(output_obj, cmd)
                elif cmd_name == "linkbus":
                    # linkbus:<id>,<event_id>,<var>,<bus_index>
                    cmd.link_bus.event_id  = int(float(cmd_args[arg_idx]))
                    cmd.link_bus.var       = parse_var(cmd_args[arg_idx+1])
                    cmd.link_bus.bus_index = int(float(cmd_args[arg_idx+2]))
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

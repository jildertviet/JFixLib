// ProtoBuf binary wire-format encoder for JFixLib.
// Builds Command messages matching simple.proto (v2 ESP-IDF firmware).
//
// Wire types used here:
//   0 = varint  (int32, bool, enum)
//   2 = length-delimited  (string, bytes, embedded message, packed repeated)
//   5 = 32-bit  (float)
JPb {

    // Encode a non-negative integer as a varint byte array.
    *varint { |value|
        var bytes = [];
        value = value.asInteger;
        while({ value > 127 }, {
            bytes = bytes ++ [(value & 127) | 128];
            value = value >> 7;
        });
        ^bytes ++ [value];
    }

    // Field tag: (field_number << 3) | wire_type, encoded as varint.
    *tag { |fieldNum, wireType|
        ^JPb.varint((fieldNum << 3) | wireType);
    }

    // Float field (wire type 5 = 32-bit little-endian).
    *float32 { |fieldNum, value|
        ^JPb.tag(fieldNum, 5) ++ value.asFloat.asBytes32;
    }

    // Int32 / enum field (wire type 0 = varint).
    *int32 { |fieldNum, value|
        ^JPb.tag(fieldNum, 0) ++ JPb.varint(value);
    }

    // Bool field (wire type 0 = varint, 0 or 1).
    *bool { |fieldNum, value|
        ^JPb.tag(fieldNum, 0) ++ [if(value, 1, 0)];
    }

    // Bytes field (wire type 2 = length-delimited).
    *bytes { |fieldNum, value|
        ^JPb.tag(fieldNum, 2) ++ JPb.varint(value.size) ++ value;
    }

    // String field (wire type 2 = length-delimited).
    *string { |fieldNum, value|
        var bytes = value.ascii;
        ^JPb.tag(fieldNum, 2) ++ JPb.varint(bytes.size) ++ bytes;
    }

    // Embedded message field (wire type 2).
    *message { |fieldNum, payload|
        ^JPb.tag(fieldNum, 2) ++ JPb.varint(payload.size) ++ payload;
    }

    // Packed repeated float field (wire type 2).
    *repeatedFloat { |fieldNum, values|
        var bytes = values.collect({ |v| v.asFloat.asBytes32 }).flat;
        ^JPb.tag(fieldNum, 2) ++ JPb.varint(bytes.size) ++ bytes;
    }

    // Build a length-prefixed Command { id, <payloadTag>: <payloadBytes> }.
    // Returns: [2-byte big-endian length] ++ [protobuf bytes].
    // id = 255 = broadcast.
    *command { |id = 255, payloadTag, payloadBytes|
        var pb = JPb.int32(1, id) ++ JPb.message(payloadTag, payloadBytes);
        ^[(pb.size >> 8) & 0xFF, pb.size & 0xFF] ++ pb;
    }
}

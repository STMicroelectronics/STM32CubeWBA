#!/usr/bin/env python3
"""
Zigbee 3 OTA Builder - V1.0
Version without ECDSA
"""

import struct
import hashlib
import hmac
import os
import sys
import argparse
from typing import Tuple, Optional, List


# ============================================================================
# OTA Header
# ============================================================================

class OTAHeader:
    def __init__(self, manufacturer_id: int, image_type: int, image_version: int, header_string: str = "", total_size: int = 0):
        self.magic_number = 0x0BEEF11E
        self.header_version = 0x0100
        self.header_length = 56
        self.field_control = 0x0000
        self.manufacturer_id = manufacturer_id
        self.image_type = image_type
        self.image_version = image_version
        self.zigbee_stack_version = 0x0002
        if header_string == "":
            self.header_string = "ST OTA File"
        else:
            if len(header_string) > 31:
                self.header_string = header_string[0:30]
            else:
                self.header_string = header_string
        self.total_image_size = total_size
    
    def to_bytes(self) -> bytes:
        hdr_str = self.header_string.encode('utf-8')[:32]
        hdr_str += b'\x00' * (32 - len(hdr_str))
        return struct.pack('<IHHHHHIH32sI', self.magic_number, self.header_version,
                          self.header_length, self.field_control, self.manufacturer_id,
                          self.image_type, self.image_version, self.zigbee_stack_version,
                          hdr_str, self.total_image_size)


# ============================================================================
# Generic TAG
# ============================================================================

def make_tag(tag_id: int, data: bytes) -> bytes:
    return struct.pack('<HI', tag_id, len(data)) + data


# ============================================================================
# MAIN
# ============================================================================

def main():
    parser = argparse.ArgumentParser(description='Zigbee 3 OTA Builder')
    
    group = parser.add_mutually_exclusive_group(required=True)
    group.add_argument('--create', help='Create OTA')
        
    parser.add_argument('--output', '-o')
    parser.add_argument('--manuf-id', type=lambda x: int(x, 0))
    parser.add_argument('--image-type', type=lambda x: int(x, 0))
    parser.add_argument('--version', type=lambda x: int(x, 0))
    parser.add_argument('--header-string', '--string', default='')
    parser.add_argument('--input-hex')
    
    args = parser.parse_args()
    
    try:
        if args.create:
            if not os.path.exists(args.create):
                print(f"Error: File {args.create} not exist.")
                return 1

            with open(args.create, 'rb') as fFileIn:
                cFirmwareData = fFileIn.read()
            
            if not all( [args.output, args.manuf_id is not None, args.image_type is not None, args.version is not None]) :
                print("Error: Need --output, --manuf-id, --image-type, --version")
                return 1

            # Make Header with User values
            print(f"Name: {args.header_string}")
            sHeader = OTAHeader( args.manuf_id, args.image_type, args.version, args.header_string )
            
            # Make Firmware Tag
            sFirmwareTag = make_tag( 0x0000, cFirmwareData )
            
            # Update Header with 'final' values
            sHeader.total_image_size = len( sHeader.to_bytes() ) + len( sFirmwareTag )
            
            # Make Header with final values
            sFinalOta = sHeader.to_bytes() + sFirmwareTag
            
            with open(args.output, 'wb') as fFileOut:
                fFileOut.write( sFinalOta )
            
            print(f"✓ Created: {args.output} ({len(sFinalOta)} bytes)")
    
    except Exception as e:
        print(f"Error: {e}")
        import traceback
        traceback.print_exc()
        return 1
    
    return 0

if __name__ == '__main__':
    sys.exit(main())
 
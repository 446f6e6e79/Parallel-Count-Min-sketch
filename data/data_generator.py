#!/usr/bin/env python3
import sys
import random
import socket

def rand_ipv4():
    first = random.randint(1, 254)
    return "{}.{}.{}.{}".format(first, random.randint(0,255), random.randint(0,255), random.randint(0,255))

def ipv4_to_packed(ip_str):
    """
        Convert dotted-quad string to 4-byte packed binary format.
        This way, each IP address takes exactly 4 bytes in binary file, making
        it fixed-width and easier to implement efficient reading.
    """
    return socket.inet_aton(ip_str)  

def main():
    """
        - Usage: python data_generator.py <num_samples> <output_file> [format]
        - format: text (default) | bin
    """
    if len(sys.argv) < 3 or len(sys.argv) > 4:
        print("Usage: python data_generator.py <num_samples> <output_file> [text|bin]")
        sys.exit(1)
    # Parse the arguments passed as parameters
    num_samples = int(sys.argv[1])
    output_file = sys.argv[2]
    # Optional format argument
    fmt = sys.argv[3] if len(sys.argv) == 4 else "text"

    if fmt not in ("text", "bin"):
        print("Format must be 'text' or 'bin'")
        sys.exit(1)

    if fmt == "text":
        with open(output_file, 'w') as f:
            for _ in range(num_samples):
                ip = rand_ipv4()
                f.write(f"{ip}\n")
    else:  # bin
        with open(output_file, 'wb') as f:
            for _ in range(num_samples):
                ip = rand_ipv4()
                f.write(ipv4_to_packed(ip))

if __name__ == "__main__":
    main()
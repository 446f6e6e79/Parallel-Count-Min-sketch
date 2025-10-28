#!/usr/bin/env python3
import sys
import random
import socket
import time

def rand_ipv4():
    """
        Generate a random IPv4 address in dotted-quad string format.
    """
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
        - Usage: python data_generator.py <num_samples> <output_file> [text|bin] [duration_seconds]
          If duration_seconds is given (integer), the script ignores num_samples and runs
          appending generated records until duration_seconds have elapsed.
    """
    if len(sys.argv) < 3 or len(sys.argv) > 5:
        print("Usage: python data_generator.py <num_samples> <output_file> [text|bin] [duration_seconds]")
        sys.exit(1)

    # Get the number of samples to generate
    try:
        num_samples = int(sys.argv[1])
    except ValueError:
        print("First argument must be an integer (num_samples)")
        sys.exit(1)
    # Get the output file and format
    output_file = sys.argv[2]
    # By default, use text format
    file_format = sys.argv[3] if len(sys.argv) >= 4 else "text"
    
    # Get duration if provided
    duration = None
    if len(sys.argv) == 5:
        try:
            duration = int(sys.argv[4])
            if duration <= 0:
                raise ValueError()
        except ValueError:
            print("duration_seconds must be a positive integer")
            sys.exit(1)

    print("Generating {} IPv4 addresses in {} format to {}...".format(
          ("up to "+str(num_samples)) if duration is None else "for {} seconds".format(duration),
          file_format, output_file))
    
    # Check if a duration is specified
    if duration:
        # duration mode: append until time expires
        endtime = time.time() + duration
        if file_format == "text":
            with open(output_file, 'w') as f:
                while time.time() < endtime:
                    ip = rand_ipv4()
                    f.write("{}\n".format(ip))
        else:
            with open(output_file, 'wb') as f:
                while time.time() < endtime:
                    ip = rand_ipv4()
                    f.write(ipv4_to_packed(ip))
    else:
        # fixed number of samples mode
        if file_format == "text":
            with open(output_file, 'w') as f:
                for _ in range(num_samples):
                    ip = rand_ipv4()
                    f.write("{}\n".format(ip))
        else:
            with open(output_file, 'wb') as f:
                for _ in range(num_samples):
                    ip = rand_ipv4()
                    f.write(ipv4_to_packed(ip))

    print("Data generation complete.")
if __name__ == "__main__":
    main()
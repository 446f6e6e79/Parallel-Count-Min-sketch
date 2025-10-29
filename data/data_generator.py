#!/usr/bin/env python3
import sys
import random
import time

def rand_ipv4_string_batch(n):
    """
        Generate a batch of random IPv4 addresses as strings.
        INPUT:
            - n -> number of elements in the batch
        RETURNS:
            array of n random IPv4 addresses in string format
    """
    return [
        f"{random.randint(1, 254)}.{random.randint(0, 255)}.{random.randint(0, 255)}.{random.randint(0, 255)}"
        for _ in range(n)
    ]

def rand_ipv4_bin_batch(n):
    """
        Generate a batch of random IPv4 addresses in packed binary format.
        INPUT:
            - n -> number of elements in the batch
        RETURNS:
            bytearray of n random IPv4 addresses in packed binary format
    """
    # 4 bytes per IP, generated from os.urandom (faster than inet_aton per IP)
    data = bytearray()
    for _ in range(n):
        data += bytes([random.randint(1, 254), random.randint(0, 255),
                       random.randint(0, 255), random.randint(0, 255)])
    return data

def main():
    if len(sys.argv) < 3 or len(sys.argv) > 5:
        print("Usage: python data_generator.py <num_samples> <output_file> [text|bin] [duration_seconds]")
        sys.exit(1)
    
    # Parse arguments
    try:
        num_samples = int(sys.argv[1])
    except ValueError:
        print("First argument must be an integer (num_samples)")
        sys.exit(1)

    output_file = sys.argv[2]
    file_format = sys.argv[3] if len(sys.argv) >= 4 else "text"

    duration = None
    if len(sys.argv) == 5:
        try:
            duration = int(sys.argv[4])
            if duration <= 0:
                raise ValueError()
        except ValueError:
            print("duration_seconds must be a positive integer")
            sys.exit(1)

    print(f"Generating {'up to '+str(num_samples) if not duration else f'for {duration} seconds'} "
          f"IPv4 addresses in {file_format} format to {output_file}...")

    # Specify batch size for each generation
    batch_size = 10000 

    # Compute the time limit if duration is specified
    start = time.time()
    end = start + duration if duration else None
    
    # Counter for written samples
    written = 0

    mode = 'wb' if file_format == 'bin' else 'w'
    with open(output_file, mode) as f:
        
        while( (duration and time.time() < end) or         # Check time limit
              (not duration and written < num_samples)):   # Check sample limit
            # Generate a batch
            remaining = (num_samples - written) if not duration else batch_size
            count = min(batch_size, remaining)

            if file_format == "text":
                ips = rand_ipv4_string_batch(count)
                f.write("\n".join(ips) + "\n")
            else:
                f.write(rand_ipv4_bin_batch(count))
            written += count
    print(f"Data generation complete. {written:,} IPs written.")

if __name__ == "__main__":
    main()
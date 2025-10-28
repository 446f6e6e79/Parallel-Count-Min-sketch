# Parallel Count-Min Sketch
Implementation of a **parallel version of the Count-Min Sketch** data structure, using **MPI** for distributed processing.
## Repository Contents
---

## Program Execution

### 1. Compilation

To build the project, first load the **MPICH** module and compile:

```bash
module load mpich-3.2
make
```

The executable will be generated at `bin/CMsketch`

---

### 2. Generating the input data

The program expects input as a **binary file** containing IPv4 addresses, with each IP encoded as **4 bytes** in **network byte order**.

You can generate an artificial dataset with the provided script:

```bash
./scripts/data_generator_job.sh
```

#### Parameters (edit inside `data_generator_job.sh` or the python script it calls)

```bash
NUMBER_OF_IPS=<number_of_ips_to_generate>
OUTPUT_PATH=<path_to_output_file>     # e.g. ./data/data.bin
FILE_TYPE=<bin|txt>                   # bin for binary, txt for text (one IP per line)
RUN_FOR_SECONDS=<seconds>             # optional: run for exactly N seconds (overrides NUMBER_OF_IPS)
```

If `RUN_FOR_SECONDS` is set, the generator will run for that number of seconds and ignore `NUMBER_OF_IPS`.

---

### 3. Running the program

Once the dataset is ready, run the program with MPI.

**Single run :**

```bash

```

**Batch/multiple runs (for performance experiments):**

We provide a launcher that generates job scripts for different node/core combinations and submits them to the cluster:

```bash
./scripts/multiple_job_launcher.sh
```

This script:
- generates job scripts from `scripts/job_template.sh` by substituting node/core, placement and parameters;
- submits them via `qsub` to the job queue of the cluster.

---
## Outputs
---
## License & authors
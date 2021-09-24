# ELM Decision Branching
This repository contains the code for my master thesis:
'[ELM Decision Branching](http://resolver.tudelft.nl/uuid:69a32e76-2a55-466a-8d42-f867a02fe158)'


The research is based on the work on Evolving Local Means (ELM):
```
@inproceedings{baruah2012evolving,
  title={Evolving local means method for clustering of streaming data},
  author={Baruah, Rashmi Dutta and Angelov, Plamen},
  booktitle={2012 IEEE international conference on fuzzy systems},
  pages={1--8},
  year={2012},
  organization={IEEE}
}
```

This repository contains C implementations of both ELM (based on the algorithm described in the paper) and ELM-DB (extended with decision branching). 

## First use
The project must be compiled before ELM/ELM-DB can be used. Start by running CMake from this directory to generate the project:

```cmake .```

Next, use Make to compile the ELM and ELM-DB executables:

```make```

It is also possible to build only one of the executables by specifying either `elm` or `elm_db`:

```
make elm
make elm_db
```



## Usage
ELM and ELM-DB can be executed with the following command:

```./elm -i <input file> -o <output file> -r <radius>```

```./elm_db -i <input file> -o <output file> -r <radius> -s <number of states>```

- `-i <input file>`: Path to input file specified in csv format (comma separated).
- `-o <output file>`: Specifies output file for the algorithm to output labels corresponding to the samples in the input file.
- `-r <radius>`: Specifies the radius used in the algorithm
- `-s <number of states>`: specifies the maximum number of states used in the algorithm (only for ELM-DB).

If the input file contains a header line, the `-h` argument can be used to let the algorithm skip the first line of the input file. Note that this will not affect the contents of the output file.

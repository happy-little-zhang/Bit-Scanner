# Bit-Scanner
Dataset and source file for the paper "Bit Scanner: Anomaly Detection for in-vehicle CAN bus using Binary Sequence Whitelisting"


To promote transparency, reproducibility, and further research in our field, we have publicly released our dataset and source code on GitHub at [\href{BitScanner}{https://github.com/happy-little-zhang/Bit-Scanner}]. The dataset includes all the normal CAN traffic, and our source code includes detailed instructions on how to use it, as well as any necessary dependencies and parameters. By releasing our dataset and source code, we aim to facilitate research in this area and encourage collaboration and innovation in the scientific community. We hope that our contribution will help advance the state of the art and inspire others to build on our work.





A brief description about Folder organization:

1.CAN Dataset
The collected 35 CAN traffic traces.
All traces are stored in csv format (timestamp(us), ID, byte1 byte2 byte3 byte4 byte5 byte6 byte7 byte8).
All traces are collected under attack-free situation.

2.Bit-Scanner-source-code
Whole Source file are implemented in C/C++ language.

To run this code successfully, please ensure there are CAN dataset under the folder "Bit-Scanner-source-code/dataset/raw".


3.Bit-Scanner-experiments-results
Visualization and analysis of experiments results are implemented in python language.
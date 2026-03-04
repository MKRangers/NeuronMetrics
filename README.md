# NeuronMetrics

NeuronMetrics is a high-performance C++ framework for large-scale axon projection analysis using mouse brain atlas (CCF) mapping.

It is designed as a reusable neuroscience engine for:

- Axon length quantification by brain region

- Projection pattern analysis

- High-throughput morphology pipelines

- Integration into larger C++ systems

## Overview

Modern neuron morphology datasets often contain:

- Thousands of neurons

- Hundreds of thousands of nodes per neuron

- Region-level projection analysis requirements

NeuronMetrics provides:

- Efficient SWC parsing

- 3D atlas voxel lookup (TIFF-based)

- Region hierarchy traversal

- Subregion → parent region aggregation

- Multi-threaded batch processing

- Clean C++ API for reuse

## Architecture

The framework is modular and designed with separation of concerns:

- **Neuron** – SWC parsing and node representation

- **MouseCCF** – 3D atlas loading, voxel lookup, and brain region tree structure

- **AxonProjectionAnalyzer** – Region-level aggregation logic

- **MetricsEngine** – Multi-threaded processing engine

## Design Principles

- Modern C++ (C++17)

- Read-only shared atlas for safe concurrency

- Task-level parallelism (per-neuron processing)

- Minimal locking during result aggregation

- No global mutable state

## Concurrency Model

- NeuronMetrics uses thread-level parallelism:

- Each worker thread processes one neuron at a time

- Shared atlas data is immutable

- Results are merged with controlled synchronization

- Work distribution is handled using atomic indexing

This enables efficient scaling across CPU cores.

## Installation (Windows)

For researchers who want to use the software without building from source:

1. Download the latest installer from the Releases page.

2. Run NeuronMetrics_Installer.exe.

3. Follow the installation wizard.

4. Launch NeuronMetrics Console from the Start Menu.

The installer includes:

- Brain atlas (3D TIFF)

- Region hierarchy data

- Required runtime components

No manual dependency installation is required.

## Console Usage

Basic usage:

`nmConsole.exe "path\to\swc_folder" "path\to\region_list_file" "path\to\output_folder"`

Example:

`nmConsole.exe "D:\Neurons\SWCs" "C:\Neurons\regionList.txt" "D:\Neurons\"`

The program will:

1. Load each SWC file

2. Map axon nodes to atlas regions

3. Aggregate by targeted brain regions in regionList.txt

4. Compute total axon length per region

5. Export results

## Building From Source

### Requirements

- CMake ≥ 3.21

- Visual Studio 2022 (MSVC)

- Boost (header-only)

- libTIFF

### Build Steps

`git clone https://github.com/yourname/NeuronMetrics.git`

`cd NeuronMetrics`

`mkdir build`

`cd build`

`cmake ..`

`cmake --build . --config Release`

### Intended Use Cases

- Axon projection quantification

- Region-level statistical analysis

- Integration into C++ neuroscience pipelines

- High-throughput morphology screening

## Roadmap

Future development will focus on expanding analytical capabilities and improving usability:

### Feature Expansion

- Feature Expansion

    - Advanced neuron shape analysis

    - Branch order statistics

    - Sholl analysis

    - Curvature and tortuosity metrics

- Projection distribution pattern analysis

    - Regional density

    - Spatial gradients

    - Layer-specific projections

- Topological analysis

    - Bifurcation structure metrics

    - Path complexity

    - Subtree classification

- Graph-based morphology descriptors

- Population-level comparative analysis

- Statistical reporting and export modules

### Machine Learning / AI Module

- Feature extraction pipeline for morphology-based embeddings

- Classical ML classifiers (SVM, Random Forest, Logistic Regression)

- Deep learning models for neuron type classification

- Region-projection-based neuron subtype clustering

- Model evaluation tools (cross-validation, confusion matrices)

- Exportable feature vectors for downstream ML workflows

### Usability & Interface

- Graphical User Interface (GUI) for standalone usage

- Interactive region selection and visualization

- Configurable analysis pipelines

- Cross-platform support (Windows / Linux)

- Packaging as a complete desktop analysis tool

## Citation

If you use NeuronMetrics in academic research that results in a publication, please cite:

NeuronMetrics: A C++ framework for axon projection analysis.
GitHub: https://github.com/yourname/NeuronMetrics

Citation is not legally required but is greatly appreciated.

## Contributing

Contributions and collaborations are welcome. Please open an issue or submit a pull request.
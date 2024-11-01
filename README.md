# Accelerating Decision-Making in AI: Parallelizing Monte Carlo Tree Search for Connect 4 Using CPU and GPU
This repository contains the code and documentation for the research project, which explores the use of parallel computing to accelerate AI decision-making. We specifically focus on implementing and comparing the performance of Monte Carlo Tree Search (MCTS) for the Connect 4 game on both CPU and GPU platforms.

## Introduction
Artificial Intelligence (AI) is widely used for complex decision-making tasks across domains, including gaming, logistics, and autonomous systems. However, AI decision-making often requires significant computational resources to produce quick and accurate outcomes.

In this project, we investigate the Monte Carlo Tree Search (MCTS) algorithm—a popular AI method for decision-making in uncertain environments—using Connect 4 as a test case. We implement MCTS in both serial and parallel versions to compare their performance across CPU and GPU platforms.

## Methodology
To assess MCTS's performance on CPUs and GPUs, we implement MCTS in three versions:

**Serial MCTS**: A baseline, single-threaded MCTS for comparing parallelization gains.

**Parallel MCTS on CPU**: Utilizes OpenMP to parallelize MCTS on multiple threads.

**Parallel MCTS on GPU**: Leverages CUDA to parallelize MCTS across GPU cores for high computational efficiency.

## Steps in MCTS:
![Phases of MCTS](https://github.com/anirudhnayak27/Connect4_game_using_MCTS/blob/4d6dc55062bccbc717bcc26cbd9df047fb5a47ab/docs/Phases%20of%20MCTS%20.png)
- **Selection**: Starts at the root node, selecting child nodes based on an exploration-exploitation policy.
- **Expansion**: Adds new child nodes for previously unexplored game states.
- **Simulation**: Runs random simulations to estimate the outcome from the current node.
- **Backpropagation**: Updates statistics back up to the root, refining the understanding of each move's value.

## How to run
- clone the repo
  ```sh
  git clone https://github.com/anirudhnayak27/Connect4_game_using_MCTS.git
  ```
- Change current directory
  ```sh
  cd Connect 4_game_using_MCTS
  ```
- Make build directory
  ```sh
  mkdir build
  ```
- Build Makefile
  ```sh
  make
  ```
- Run the output file which is named `connect_four`
  ```sh
  ./connect_four
  ```
>**Note**
> - Make sure to install make before building Makefile.
> - If you are on vscode, you can install Makefile Tools extension <p align="here"><img src="https://ms-vscode.gallerycdn.vsassets.io/extensions/ms-vscode/makefile-tools/0.10.7/1714536879972/Microsoft.VisualStudio.Services.Icons.Default" height="50px">&nbsp;&nbsp;&nbsp;&nbsp;&nbsp; </p>
>   Makefile was made in Linux environment, do it is recommended to use vscode for windows to build Makefile.

**Remark** 
> There is a bug in game, the AI(Computer) which is player two that is using MCTS, always goes for a particular column as best move but after the column fills and game in not over, the AI tries to make move in the same column leading to a segmentation fault. Since I have less experience with the Makefilwe and Header files systems I just debugged it by creating a new files and writting the entire game in it. Looking for solutions in origin game(with bug) in Cpp folder.

**Bug Fixed** 
> Fixed the bug by making new header and source files , there was a problems of access the functions from class MCTS. 

- Build `Game_After_bug_fixed.cpp`
  ```
  g++ Game_After_bug_fixed.cpp -std=c++17 -o <output_filename>
  ```
  >Note: Can use `g++ Game_After_bug_fixed.cpp -fopenmp -std=c++17 -lstdc++ -o <output_filename>` if above command throws an error.
- Run `Game_After_bug_fixed.cpp`
  ```
  ./<output_filename>

**How to run on GPU with CUDA**
> Guide to run game optimized for GPU using CUDA
- Build the `Connectfour.cu` on GPU that supports CUDA (prefered Nvidia GPUs)
  ```
  nvcc Connectfour.cu -o <output_filename>
  ```
- Run `Connectfour.cu`
  ```
  ./<output_filename>
  ```

## Results
**CPU Performance**: Parallelizing with OpenMP showed modest gains, but the speedup was limited due to core constraints.

**GPU Performance**: CUDA-powered GPU parallelization achieved significant speedups, demonstrating the GPU’s advantage in handling MCTS's large-scale parallel tasks.

### Key takeaways:

- GPU-based parallelization greatly improves decision speed.
- Performance improvements are hardware-dependent, and memory bottlenecks on the GPU become apparent with high rollout counts.
  
## Contribution

Contributions are welcome! If you have any suggestions, improvements, or bug fixes, please submit a pull request or open an issue on the GitHub repository.To contribute, please:

- Fork the repository.
- Create a new branch for your feature or bug fix.
- Submit a pull request detailing your changes.

## License

This project is licensed under the [MIT License](LICENSE).
